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

    QString addCar(Car &car);

    void updateGameState(const QJsonArray &clientInputs);

    void handleCollisions();

    QJsonObject getGameStateJson();

private:
    QVector<Car> cars;
    RaceTrack &raceTrack;

    void updateCarState(Car &car, const QJsonObject &json, RaceTrack &track);
};

#endif // GLM_H
