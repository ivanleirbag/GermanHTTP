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

void MainWindow::onClientRequest()
{
    int count;
    QByteArray dataSent;

    QTcpSocket *client = static_cast<QTcpSocket *>(QObject::sender());

    count = client->bytesAvailable();
    if(count <= 0){
        return;
    }

    dataSent = client->readAll();

    ui->plainTextEdit->appendPlainText(QString(dataSent));
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

