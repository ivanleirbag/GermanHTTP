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

    QString trackPath = workingDir + "/racedocs/assets/tracks/raceTrack.jpg";
    QString backgroundPath = workingDir + "/racedocs/assets/tracks/background.jpg";
    RaceTrack track(trackPath, backgroundPath);
    glm = new GLM(track);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateRace4State);
    gameTimer->start(50);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete glm;
    delete gameTimer;
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
    QString clientIP;

    QTcpSocket *client = static_cast<QTcpSocket *>(QObject::sender());
    clientIP = client->peerAddress().toString();

    glm->removeCar(clientIP);

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

void MainWindow::updateRace4State()
{
    glm->updateGameState();

    QByteArray gameStateJson = glm->getGameState();

    for(QTcpSocket *client : clients){
        if(client->isOpen()){
            client->write("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
            ui->plainTextEdit->appendPlainText("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
            writeLogFile();
            client->write(gameStateJson);
            client->flush();
        }
    }
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
    int headerIndex = dataSent.indexOf("\r\n\r\n");
    requestLines = strDataSent.split("\r\n")[0];
    ui->plainTextEdit->appendPlainText(dataSent.left(headerIndex));

    //METHOD PROCESSING
    reqstLinesTokens = requestLines.split(" ");

    if(reqstLinesTokens[0] == "GET"){
        onClientReqstGET(reqstLinesTokens[1], client);
        client->waitForBytesWritten();
    }else if(reqstLinesTokens[0] == "POST"){
        onClientReqstPOST(reqstLinesTokens[1], client, dataSent.mid(headerIndex+4));
        client->waitForBytesWritten();
    }
    else if(reqstLinesTokens[0] == "OPTIONS"){
        onClientReqstOPTIONS(client);
        client->waitForBytesWritten();
    }
    client->disconnectFromHost();
}

void MainWindow::onClientReqstGET(QString uri,  QTcpSocket* client)
{
    QFile fileRequested;
    QString resourcePath = workingDir + "/htdocs";
    QString header;
    QByteArray response;

    if(uri == "/"){
        resourcePath = resourcePath + "/html/index.html";
    }
    //RACE GAME DEBUG
    else if(uri == "/race4"){
        resourcePath = resourcePath + "/racedocs/race4.html";
    }
    else if (uri == "/race4/getState") {
        QString carID = client->peerAddress().toString() + ":" + QString::number(client->peerPort());
        int carImgID = glm->getNextAvailableCar();
        if (carImgID > 0){
            QString carImage = workingDir + "/racedocs/assets/cars/car" + QString::asprintf("%d", carImgID) + ".png";
            Car car(carImage, carID, 0, 0, 20, 0);
            glm->addCar(car);
            QJsonObject response;
            response["carID"] = carImgID;
            client->write("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
            client->write(QJsonDocument(response).toJson());
        }else{
            client->write("HTTP/1.1 403 Forbidden\r\nContent-Type: text/plain\r\n\r\nMax player capacity reached.");
        }
        client->flush();
    }
    else{
        resourcePath = resourcePath + uri;
    }

    fileRequested.setFileName(resourcePath);

    if (!fileRequested.open(QFile::ReadOnly)) {
        header = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        client->write(header.toUtf8());
        client->flush();
        ui->plainTextEdit->appendPlainText((header));
        writeLogFile();
        return;
    }

    if (resourcePath.endsWith(".png", Qt::CaseInsensitive)) {
        header = "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\n";
    }
    else if (resourcePath.endsWith(".html", Qt::CaseInsensitive)) {
        header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";
    }
    else if(resourcePath.endsWith(".jpg", Qt::CaseInsensitive) || resourcePath.endsWith(".jpeg", Qt::CaseInsensitive)){
        header = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n";
    }
    else if(resourcePath.endsWith(".css", Qt::CaseInsensitive)){
        header = "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n";
    }else if(resourcePath.endsWith(".js", Qt::CaseInsensitive)){
        header = "HTTP/1.1 200 OK\r\nContent-Type: text/javascript\r\n";
    }else if(resourcePath.endsWith(".json", Qt::CaseInsensitive)){
        header = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n";
    }else if(resourcePath.endsWith(".mp3", Qt::CaseInsensitive)){
        header = "HTTP/1.1 200 OK\r\nContent-Type: audio/mpeg3\r\n";
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

void MainWindow::onClientReqstPOST(QString uri,  QTcpSocket* client, const QByteArray &dataSent)
{
    QFile fileRequested;
    QString resourcePath = workingDir + "/htdocs" + uri;
    QString header;


    if (uri == "/race4/update") {
        QJsonDocument inputDoc = QJsonDocument::fromJson(dataSent);
        QJsonArray clientInputs = inputDoc.array();

        // Actualizar estado del juego con inputs de los clientes
        glm->updateGameState(clientInputs);

        // Obtener el estado actualizado del juego
        QByteArray gameState = glm->getGameState();

        // Enviar respuesta al cliente
        client->write("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
        client->write(gameState);
        client->flush();
        client->disconnectFromHost();

        return;
    }

    //CREATES THE RESOURCE IF IT DOES NOT EXISTS
    fileRequested.setFileName(resourcePath);
    if (!fileRequested.open(QFile::WriteOnly | QFile::Append)) {
        header = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nCould not create the resource";
        client->write(header.toUtf8());
        client->flush();
        ui->plainTextEdit->appendPlainText(header);
        writeLogFile();
        return;
    }

    //WRITES THE RESOURCE
    fileRequested.write(dataSent);
    fileRequested.close();

    if (resourcePath.endsWith(".json", Qt::CaseInsensitive)) {
        header += "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n";
    }
    else{
        header = "HTTP/1.1 415 Unsupported Media Type\r\n\r\n";
        client->write(header.toUtf8());
        client->flush();
        ui->plainTextEdit->appendPlainText((header));
        writeLogFile();
        return;
    }
    header += "Access-Control-Allow-Origin: *\r\n";
    header += "Connection: keep-alive\r\n\r\n";

    client->write(header.toUtf8());
    client->flush();

    ui->plainTextEdit->appendPlainText(header);
    writeLogFile();
}

void MainWindow::onClientReqstPUT(QString uri,  QTcpSocket* client, QByteArray &dataSent)
{
    return;
}

void MainWindow::onClientReqstOPTIONS(QTcpSocket *client)
{
    QString header = "HTTP/1.1 204 No Content\r\n";
    header += "Access-Control-Allow-Origin: *\r\n";
    header += "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n";
    header += "Access-Control-Allow-Headers: Content-Type\r\n";
    header += "Connection: keep-alive\r\n\r\n";
    client->write(header.toUtf8());
    client->flush();
    client->disconnectFromHost();
}

