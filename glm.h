#ifndef GLM_H
#define GLM_H

#include "car.h"
#include "racetrack.h"
#include "hitbox.h"

#include <QFile>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QJsonObject>
#include <QJsonDocument>

//Game Logic Module

class GLM
{
public:
    GLM();

    int initRaceTracks();

    int addNewPlayer(QTcpSocket *client);

    int disconnectPlayer(QHostAddress playerIP);

    validateMovement
};

#endif // GLM_H
