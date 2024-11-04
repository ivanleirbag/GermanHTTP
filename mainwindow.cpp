#include "mainwindow.h"
#include "ui_mainwindow.h"

//PeerPort o PeerAddress

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qDebug() << "Directorio de trabajo: " << workingDir;

    ui->setupUi(this);

    QTcpMainServer = new QTcpServer(this);
    connect(QTcpMainServer, &QTcpServer::newConnection, this, &MainWindow::onNewConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewConnection()
{
    QString clientStr;

    QTcpSocket *client = QTcpMainServer->nextPendingConnection();
    clients.append(client);

    //SIGNALS
    connect(client, &QTcpSocket::disconnected, this, &MainWindow::onClientDisconnect);
    connect(client, &QTcpSocket::readyRead, this, &MainWindow::onClientRequest);

    clientStr = "CONNECTING: " + client->peerAddress().toString() + ": ";
    clientStr = clientStr + QString().number(client->peerPort());

    ui->plainTextEdit->appendPlainText(clientStr);
    writeLogFile();
}


void MainWindow::onClientDisconnect()
{
    int clientIndex;
    QString clientStr;

    QTcpSocket *client = static_cast<QTcpSocket *>(QObject::sender());

    clientStr = "DISCONNECTING: " + client->peerAddress().toString() + ": ";
    clientStr = clientStr + QString().number(client->peerPort());

    clientIndex = clients.indexOf(client);
    if(clientIndex != -1){
        clients.takeAt(clientIndex);
    }

    clientIndex = QTcpMainServer->children().indexOf(client);
    if(clientIndex >= 1){
        QTcpMainServer->children().at(clientIndex)->deleteLater();
    }

    ui->plainTextEdit->appendPlainText(clientStr);
    writeLogFile();
}

void MainWindow::writeLogFile()
{
    QFile logFile;
    QString logDir = workingDir + "/logfile.txt";
    logFile.setFileName(logDir);
    QString auxStr;

    if(logFile.open(QFile::WriteOnly | QFile::Text)){
        auxStr = ui->plainTextEdit->toPlainText() + "\r\n";
        logFile.write(auxStr.toUtf8());
        logFile.close();
    }
    else{
        ui->plainTextEdit->appendPlainText("ERROR: Could not open logfile");
    }
}


void MainWindow::on_OpenPortBttn_clicked()
{
    quint16 port;
    QString oppMsg;
    bool ok;

    port = ui->SVPortSetter->text().toUShort(&ok);

    if(QTcpMainServer->isListening()){

        QTextStream(&oppMsg)<<"Closing port "<<port<<"...";
        ui->plainTextEdit->appendPlainText(oppMsg);
        oppMsg.clear();

        //DISCONNECTS ALL CLIENTS
        while(clients.count()){
            clients.at(0)->close();
        }
        QTcpMainServer->close();

        QTextStream(&oppMsg)<<"Successfully closed port "<<port<<".";
        ui->plainTextEdit->appendPlainText(oppMsg);
        oppMsg.clear();

        ui->OpenPortBttn->setText("OPEN PORT");
    }
    else{
        if(ok){

            QTextStream(&oppMsg)<<"Oppenning port "<<port<<"...";
            ui->plainTextEdit->appendPlainText(oppMsg);
            oppMsg.clear();

            if(QTcpMainServer->listen(QHostAddress::Any, port)){
                QTextStream(&oppMsg)<<"Successfully oppened port "<<port<<".";
                ui->plainTextEdit->appendPlainText(oppMsg);
                oppMsg.clear();

                ui->OpenPortBttn->setText("CLOSE PORT");
            }
        }
    }
    writeLogFile();
}

void MainWindow::onClientRequest()
{
    int count;
    QByteArray dataSent;
    QString strDataSent;
    QString requestLines;
    QStringList reqstLinesTokens;

    QTcpSocket *client = static_cast<QTcpSocket *>(QObject::sender());

    count = client->bytesAvailable();
    if(count <= 0){
        return;
    }

    dataSent = client->readAll();
    strDataSent = QString(dataSent);

    ui->plainTextEdit->appendPlainText(strDataSent);

    //REQUEST EXTRACTION
    requestLines = strDataSent.split("\r\n")[0];

    //METHOD PROCESSING
    reqstLinesTokens = requestLines.split(" ");

    if(reqstLinesTokens[0] == "GET"){
        onClientReqstGET(reqstLinesTokens[1], client);
        client->waitForBytesWritten();
    }
    client->disconnectFromHost();
}

void MainWindow::onClientReqstGET(QString uri,  QTcpSocket* client)
{
    QFile fileRequested;
    QString fileDir = workingDir + "/htdocs";
    QString header;
    QByteArray response;

    if(uri == "/"){
        fileDir = fileDir + "/index.html";
    }
    //RACE GAME DEBUG
    else if(uri == "/race4"){
        QString raceDir = workingDir + "/racedocs";
        QString imageDir = raceDir + "background.png";
        QString pathDir = raceDir + "raceTrack.png";
        RaceTrack track(pathDir, imageDir);

        client->write("HTTP/1.1 200 OK\r\nContent-Type: image/png\r\n\r\n");
        client->write(track.getBackgroundImage());
        client->flush();
        return;
    }
    else{
        fileDir = fileDir + uri;
    }

    fileRequested.setFileName(fileDir);

    if (!fileRequested.open(QFile::ReadOnly)) {
        header = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        client->write(header.toUtf8());
        client->flush();
        ui->plainTextEdit->appendPlainText((header));
        writeLogFile();
        return;
    }

    if (fileDir.endsWith(".png", Qt::CaseInsensitive)) {
        header = "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\n";
    }
    else if (fileDir.endsWith(".html", Qt::CaseInsensitive)) {
        header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";
    }
    else if(fileDir.endsWith(".jpg", Qt::CaseInsensitive) || fileDir.endsWith(".jpeg", Qt::CaseInsensitive)){
        header = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n";
    }
    else if(fileDir.endsWith(".css", Qt::CaseInsensitive)){
        header = "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n";
    }else if(fileDir.endsWith(".js", Qt::CaseInsensitive)){
        header = "HTTP/1.1 200 OK\r\nContent-Type: text/javascript\r\n";
    }
    else{
        header = "HTTP/1.1 415 Unsupported Media Type\r\n\r\n";
        response.append(header.toUtf8());
        client->write(header.toUtf8());
        client->flush();
        ui->plainTextEdit->appendPlainText((header));
        writeLogFile();
        return;
    }


    //DATA IS ALWAYS SENT IN CHUNKS FOR INTEGRITY
    header += "Transfer-Encoding: chunked\r\n";
    header += "Connection: keep-alive\r\n\r\n";
    client->write(header.toUtf8());
    client->flush();

    const qint64 chunkSize = 4096;
    while (!fileRequested.atEnd()) {
        QByteArray chunk = fileRequested.read(chunkSize);
        QString chunkHeader = QString::number(chunk.size(), 16) + "\r\n";//CHUNK SIZE HAS TO BE IN HEX
        client->write(chunkHeader.toUtf8());
        client->write(chunk);
        client->write("\r\n");
        client->flush();
    }

    client->write("0\r\n\r\n");  //LAST CHUNCK WITH SIZE 0
    client->flush();

    fileRequested.close();
    ui->plainTextEdit->appendPlainText((header));
    writeLogFile();
}

void MainWindow::onClientReqstPOST()
{

}

void MainWindow::onClientReqstPUT()
{

}

void MainWindow::onClientReqstDELETE()
{

}

