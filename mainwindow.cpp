#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTcpMainServer = new QTcpServer(this);
    QTcpMainServer->setMaxPendingConnections(100);
    connect(QTcpMainServer, &QTcpServer::newConnection, this, &MainWindow::onNewConnection);


    QString trackPath = workingDir + "/htdocs/racedocs/assets/tracks/raceTrack.jpg";
    QString backgroundPath = workingDir + "/htdocs/racedocs/assets/tracks/background.jpg";

    glm = new GLM(trackPath, backgroundPath);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete glm;
}

void MainWindow::onNewConnection()
{
    QTcpSocket *client = QTcpMainServer->nextPendingConnection();
    if (client == nullptr) {
        return;
    }
    if (!clients.contains(client)) {
        clients.append(client);
    }

    //SIGNALS
    connect(client, &QTcpSocket::disconnected, this, &MainWindow::onClientDisconnect);
    connect(client, &QTcpSocket::readyRead, this, &MainWindow::onClientRequest);

    QString clientInfo = QString("CONNECTING: %1:%2")
                             .arg(client->peerAddress().toString())
                             .arg(client->peerPort());
    ui->plainTextEdit->appendPlainText(clientInfo);
    writeLogFile();
}

void MainWindow::onClientDisconnect()
{
    int clientIndex;

    QTcpSocket *client = static_cast<QTcpSocket *>(QObject::sender());
    if (!client) {
        return;
    }


    QString clientip = client->peerAddress().toString();
    qint16 clientport = client->peerPort();

    QString clientInfo = QString("DISCONNECTING: %1:%2")
                             .arg(client->peerAddress().toString())
                             .arg(client->peerPort());

    if(client->state() != QTcpSocket::ConnectedState){
        glm->removeCar(clientip, clientport);
        for(const auto raceClient : race4Clients){
            if (raceClient == client){
                race4Clients.removeAll(client);
            }
        }
    }

    clientIndex = clients.indexOf(client);
    if(clientIndex != -1){
        clients.takeAt(clientIndex);
    }


    clientIndex = QTcpMainServer->children().indexOf(client);
    if (clientIndex >= 0 && clientIndex < QTcpMainServer->children().count()) {
        QTcpMainServer->children().at(clientIndex)->deleteLater();
    }

    client->deleteLater();

    ui->plainTextEdit->appendPlainText(clientInfo);
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
        {
            QMutexLocker raceLocker(&raceMutex);

            QTextStream(&oppMsg)<<"Closing port "<<port<<"...";
            ui->plainTextEdit->appendPlainText(oppMsg);
            oppMsg.clear();

            //DISCONNECTS ALL CLIENTS
            while(clients.count()){
                QTcpSocket *client = clients.takeAt(0);
                client->disconnectFromHost();
                client->close();
                client->deleteLater();
            }
            race4Clients.clear();
            QTcpMainServer->close();

            QTextStream(&oppMsg)<<"Successfully closed port "<<port<<".";
            ui->plainTextEdit->appendPlainText(oppMsg);
            oppMsg.clear();

            glm->clearGLM();

            ui->OpenPortBttn->setText("OPEN PORT");
        }
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
    QByteArray dataSent;
    QString strDataSent;
    QString requestLines;
    QStringList reqstLinesTokens;

    QTcpSocket *client = static_cast<QTcpSocket *>(QObject::sender());

    if (!client || client->state() != QTcpSocket::ConnectedState) {
        qDebug() << "Client not connected...";
        return;
    }

    if (client->bytesAvailable() > 0) {
        dataSent = client->readAll();
        strDataSent = QString(dataSent);
    } else {
        return;
    }
    ui->plainTextEdit->appendPlainText(strDataSent);

    //REQUEST EXTRACTION
    int headerIndex = dataSent.indexOf("\r\n\r\n");
    requestLines = strDataSent.split("\r\n")[0];
    ui->plainTextEdit->appendPlainText(dataSent.left(headerIndex));

    {
        QMutexLocker raceLocker(&raceMutex);
        //METHOD PROCESSING
        reqstLinesTokens = requestLines.split(" ");
        qDebug() << "race4Clients size:" << race4Clients.size();
        for (auto c : race4Clients) {
            qDebug() << "Client:" << c->peerAddress().toString() << ":" << c->peerPort();
        }
        if(race4Clients.size() == 0){
            glm->clearGLM();
        }
    }

    if(reqstLinesTokens[0] == "GET"){
        onClientReqstGET(reqstLinesTokens[1], client);
    }else if(reqstLinesTokens[0] == "POST"){
        onClientReqstPOST(reqstLinesTokens[1], client, dataSent.mid(headerIndex+4));
    }
    else if(reqstLinesTokens[0] == "OPTIONS"){
        onClientReqstOPTIONS(client);
    }
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

    }else if(uri == "/race4/join"){

        QMutexLocker raceLocker(&raceMutex);

        if (client->state() != QTcpSocket::ConnectedState) {
            qDebug() << "Client is not in ConnectedState";
            return;
        }
        if (race4Clients.size() < 4 && !race4Clients.contains(client)) {
            race4Clients.append(client);
            qDebug() << "------------------";
            for (const auto client : race4Clients){
                qDebug() << "Client connected: " << client;
            }

            QString clientIP = client->peerAddress().toString();
            qint16 clientport = client->peerPort();

            {
            QMutexLocker glmLocker(&glmMutex);
            int carImgID = glm->getNextAvailableCar();
            QString carImage = resourcePath + "/racedocs/assets/cars/car" + QString::asprintf("%d", carImgID) + ".png";

            Car car(carImage, carImgID, 550, 495+(40*(carImgID-1)), 20, 0 , clientIP, clientport);
            glm->addCar(car);

            QJsonObject carJson = car.carStateJson();
            QJsonDocument jsonResponse(carJson);
            QByteArray jsonData = jsonResponse.toJson();

            QString response = QString("HTTP/1.1 200 OK\r\n"
                                       "Content-Type: application/json\r\n"
                                       "Content-Length: %1\r\n"
                                       "Connection: keep-alive\r\n\r\n")
                                   .arg(jsonData.size());
            client->write(response.toUtf8());
            client->write(jsonData);
            client->flush();
            return;
            }
        }else {
            client->write("HTTP/1.1 403 Forbidden\r\nContent-Type: text/plain\r\n\r\nMax player capacity reached.");
            client->flush();
            return;
        }
    }
    else if (uri == "/race4/getState") {
        QByteArray gamestate = glm->getGameState();
        if (client->state() == QTcpSocket::ConnectedState){
            client->write("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n");
            client->write("Content-Length: " + QByteArray::number(gamestate.size()) + "\r\n");
            client->write("Connection: keep-alive\r\n\r\n");
            ui->plainTextEdit->appendPlainText("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");
            ui->plainTextEdit->appendPlainText(QString(gamestate));
            client->write(gamestate);
        }else{
            ui->plainTextEdit->appendPlainText("THE CLIENT DISCONNECTED BEFORE THE DATA WAS SENT");
            return;
        }
        writeLogFile();
        client->flush();
        return;
    }
    else{
        resourcePath = resourcePath + uri;
    }

    fileRequested.setFileName(resourcePath);
    if (client->state() == QTcpSocket::ConnectedState){
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
}

void MainWindow::onClientReqstPOST(QString uri,  QTcpSocket* client, const QByteArray &dataSent)
{
    QFile fileRequested;
    QString resourcePath = workingDir + "/htdocs" + uri;
    QString header;

    if (uri == "/race4/updateState") {
        QMutexLocker glmLocker(&glmMutex);
        QJsonDocument inputDoc = QJsonDocument::fromJson(dataSent);

        glm->updateGameState(inputDoc.object());
        QByteArray gameState = glm->getGameState();

        if (client->state() == QTcpSocket::ConnectedState) {
            client->write("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n");
            client->write("Content-Length: " + QByteArray::number(gameState.size()) + "\r\n");
            client->write("Connection: keep-alive\r\n\r\n");
            client->write(gameState);
            client->flush();
        }else{
            qDebug() << "Client is not connected";
        }

        return;
    }

    //CREATES THE RESOURCE IF IT DOES NOT EXISTS
    fileRequested.setFileName(resourcePath);
    if (!fileRequested.open(QFile::WriteOnly | QFile::Append)) {
        header = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nCould not create the resource";
        if (client->state() == QTcpSocket::ConnectedState){
            client->write(header.toUtf8());
            client->flush();
            ui->plainTextEdit->appendPlainText(header);
            writeLogFile();
        }
        return;
    }

    //WRITES THE RESOURCE
    fileRequested.write(dataSent);
    fileRequested.close();

    if (resourcePath.endsWith(".json", Qt::CaseInsensitive)) {
        header += "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n";
    }
    else{
        if (client->state() == QTcpSocket::ConnectedState){
            header = "HTTP/1.1 415 Unsupported Media Type\r\n\r\n";
            client->write(header.toUtf8());
            client->flush();
            ui->plainTextEdit->appendPlainText((header));
            writeLogFile();
        }

        return;
    }
    header += "Access-Control-Allow-Origin: *\r\n";
    header += "Connection: keep-alive\r\n\r\n";
    if (client->state() == QTcpSocket::ConnectedState){
        client->write(header.toUtf8());
        client->flush();
        ui->plainTextEdit->appendPlainText(header);
    }



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

