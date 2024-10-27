#include "mainwindow.h"
#include "ui_mainwindow.h"

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
        QByteArray response = onClientReqstGET(reqstLinesTokens[1]);

        qint64 bytesSent = 0;
        qint64 totalBytes = response.size();
        while (bytesSent < totalBytes) {
            //DATA IS SENT IN CHUNKS
            qint64 chunkSize = client->write(response.mid(bytesSent, 4096));
            if (chunkSize == -1) {
                ui->plainTextEdit->appendPlainText("ERROR: An error occured during data transmition.");
                return;
            }
            bytesSent += chunkSize;
        }
        client->write(response, response.size());
        client->flush();
    }
}

QByteArray MainWindow::onClientReqstGET(QString route)
{
    QFile fileRequested;
    QByteArray fileContent;
    QString fileDir = workingDir + "/htdocs";
    QString header;
    QByteArray response;
    quint16 contentSize;

    if(route == "/"){
        fileDir = fileDir + "/index.html";
    }
    else{
        fileDir = fileDir + route;
    }

    fileRequested.setFileName(fileDir);

    if(fileRequested.open(QFile::ReadOnly)){
        fileContent = fileRequested.readAll();
        contentSize = fileContent.size();

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
        }
        else{
            header = "HTTP/1.1 415 Unsupported Media Type\r\n\r\n";
            response.append(header.toUtf8());
            return response;
        }

        QTextStream(&header)<<"Content-Length: "<<contentSize<<"\r\n";
        QTextStream(&header)<<"Connection: keep-alive\r\n\r\n";

        response.append(header.toUtf8());
        response.append(fileContent);

        fileRequested.close();
    }
    else{
        header = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        QString body = "<html><body><h1>404 Not Found</h1></body></html>";
        response.append(header.toUtf8());
        response.append(body.toUtf8());
    }

    ui->plainTextEdit->appendPlainText((header));
    writeLogFile();
    return response;
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

