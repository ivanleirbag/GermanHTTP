#ifndef GLM_H
#define GLM_H

#include "car.h"
#include "racetrack.h"
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class GLM {
public:
    GLM(QString &trackPath, QString &backgroundPath);

    void addCar(Car &car);

    void removeCar(const QString clientip, const qint16 clientport);

    void updateGameState();

    void updateGameState(const QJsonObject &clientInputs);

    int getNextAvailableCar();

    void clearGLM();

    QByteArray getGameState();

private:
    QVector<Car> cars;
    RaceTrack raceTrack;

    void updateCarState(Car &car, const QJsonObject &json);
};

#endif // GLM_H
