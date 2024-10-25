#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QMainWindow>
#include <QDateTime>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>


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
    void onNewConnection();
    void onClientRequest();
    void onClientDisconnect();

    void writeLogFile();

    void on_OpenPortBttn_clicked();

private:
    Ui::MainWindow *ui;

    //SERVER VARS
    QTcpServer *QTcpMainServer;
    QList<QTcpSocket *>clients;

    //WORKING DIRECTORY
    QString workingDir = QCoreApplication::applicationDirPath();
};
#endif // MAINWINDOW_H
