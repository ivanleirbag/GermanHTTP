#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "racetrack.h"
#include "glm.h"


#include <QFile>
#include <QMainWindow>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>
#include <QJsonObject>
#include <QJsonDocument>


#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    //SERVER FUNCS
    /**
     * @brief onNewConnection
     */
    void onNewConnection();
    void onClientRequest();
    void onClientDisconnect();

    void writeLogFile();

    void on_OpenPortBttn_clicked();

    void updateRace4State();

    void onClientReqstGET(QString route, QTcpSocket *client);
    void onClientReqstPOST(QString uri,  QTcpSocket* client, const QByteArray &dataSent);
    void onClientReqstPUT(QString uri,  QTcpSocket* client, QByteArray &dataSent);
    void onClientReqstOPTIONS(QTcpSocket* client);

private:
    Ui::MainWindow *ui;

    //SERVER VARS
    QTcpServer *QTcpMainServer;
    QList<QTcpSocket *>clients;
    QList<QTcpSocket *> race4Clients;
    GLM *glm;

    //WORKING DIRECTORY
    QString workingDir = QCoreApplication::applicationDirPath();
};
#endif // MAINWINDOW_H
