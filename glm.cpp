#include "glm.h"


GLM::GLM(QString &trackPath, QString &backgroundPath)
    : raceTrack(trackPath, backgroundPath) {

}

void GLM::addCar(Car &car) {
    //CHECKS FOR CAR DUPLICATES
    for ( Car &existingCar : cars) {
        if (existingCar.ID == car.ID) {
            return;
        }
    }

    if (cars.size() < 4){
        cars.append(car);
    }else{
        return;
    }
}

void GLM::removeCar(const QString clientip, const qint16 clientport)
{
    for (int i = 0; i < cars.size(); ++i) {
        if (cars[i].clientip == clientip && cars[i].clientport == clientport) {
            cars.removeAt(i);
            return;
        }
    }
}

void GLM::updateGameState() {
    for (Car &car : cars) {

        if (!raceTrack.isOnTrack(car)) {
            car.restorePreviousState();
        }

        for (Car &otherCar : cars) {
            if (car.ID != otherCar.ID && car.collidesWith(otherCar)) {
                car.restorePreviousState();
            }
        }
    }
}

void GLM::updateGameState(const QJsonObject &clientInputs) {
    int carID = clientInputs["ID"].toInt();

    for (Car &car : cars) {
        if (carID == car.ID) {
            updateCarState(car, clientInputs);
            car.updatePosition();
            if (raceTrack.isOnTrack(car) == false) {
                car.restorePreviousState();
                return;
            }

            for (Car &otherCar : cars) {
                if (car.ID != otherCar.ID && car.collidesWith(otherCar)) {
                    car.restorePreviousState();
                    return;
                }
            }
            break;
        }
    }
}

int GLM::getNextAvailableCar()
{
    if (cars.size() < 4){
        return (cars.size()+1);
    }else{
        return 4;
    }
}

void GLM::clearGLM()
{
    cars.clear();
}


void GLM::updateCarState(Car &car, const QJsonObject &json)
{
    car.savePreviousState();
    car.updateCarState(json);

    if (raceTrack.isOnTrack(car) == false) {
        car.restorePreviousState();
        return;
    }

    for (Car &otherCar : cars) {
        if (car.ID != otherCar.ID && car.collidesWith(otherCar)) {
            car.restorePreviousState();
            return;
        }
    }
}



QByteArray GLM::getGameState() {
    QJsonObject gameState;
    QJsonArray carsArray;

    for (Car &car : cars) {
        carsArray.append(car.carStateJson());
    }

    gameState["cars"] = carsArray;
    return QJsonDocument(gameState).toJson();
}

