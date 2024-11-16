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
    GLM(RaceTrack &track);

    void addCar(Car &car);

    void removeCar(const QString& carID);

    void updateGameState();

    void updateGameState(const QJsonArray &clientInputs);

    int getNextAvailableCar();

    QByteArray getGameState();

private:
    QVector<Car> cars;
    RaceTrack &raceTrack;

    void updateCarState(Car &car, const QJsonObject &json);
};

#endif // GLM_H
