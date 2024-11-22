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
#include <QMutex>

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
     * Handles new client connections to the server.
     * Configures signals and adds the client to clients list.
     */
    void onNewConnection();

    /**
     * @brief onClientRequest
     * Processes incoming requests from clients.
     * Parses HTTP headers and uri's to the appropriate handler.
     */
    void onClientRequest();

    /**
     * @brief onClientDisconnect
     * Handles client disconnections.
     * Cleans up resources, such as clients and race4clients list, and updates internal connection lists.
     */
    void onClientDisconnect();

    /**
     * @brief writeLogFile
     * Writes log messages to a file for server operations such as client connections, requests, responses and disconnections.
     */
    void writeLogFile();

    /**
     * @brief on_OpenPortBttn_clicked
     * Opens or closes the server specified port when the button is clicked.
     * Calls for client disconnections when closing the port.
     */
    void on_OpenPortBttn_clicked();

    /**
     * @brief onClientReqstGET
     * Handles HTTP GET requests from clients.
     * Retrieves requested resources or provides dynamic data for specific endpoints.
     * @param route
     * The URI of the requested resource.
     * @param client
     * The client socket sending the request.
     */
    void onClientReqstGET(QString route, QTcpSocket *client);

    /**
     * @brief onClientReqstPOST
     * Handles HTTP POST requests from clients.
     * Processes and stores data sent by the client, or updates game state if applicable.
     * @param uri
     * The URI where the data is being posted.
     * @param client
     * The client socket sending the request.
     * @param dataSent
     * The data sent by the client.
     */
    void onClientReqstPOST(QString uri,  QTcpSocket* client, const QByteArray &dataSent);

    /**
     * @brief onClientReqstPUT
     * Currently not developed
     */
    void onClientReqstPUT(QString uri,  QTcpSocket* client, QByteArray &dataSent);

    /**
     * @brief onClientReqstOPTIONS
     * Handles HTTP OPTIONS requests from clients.
     * Sends supported HTTP methods to the client.
     * @param client
     * The client socket sending the request.
     */
    void onClientReqstOPTIONS(QTcpSocket* client);

private:
    Ui::MainWindow *ui;

    //SERVER VARS
    QTcpServer *QTcpMainServer;         ///< The main TCP server
    QList<QTcpSocket *>clients;         ///< List of connected clients.
    QList<QTcpSocket *>race4Clients;    ///< List of clients playing race4
    GLM *glm;                           ///< The main Game Logic Module. Handles Race4 requests
    QMutex raceMutex;                   ///< Mutex for synchronizing race4 operations. Prevents simultaneous access to sensitive lists
    QMutex glmMutex;                    ///< Mutex for synchronizing GLM operations. Prevents simultaneous access to sensitive lists

    //WORKING DIRECTORY
    QString workingDir = QCoreApplication::applicationDirPath();
};
#endif // MAINWINDOW_H
