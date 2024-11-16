#include "glm.h"

#include "glm.h"

GLM::GLM(RaceTrack &track)
    : raceTrack(track) {}

void GLM::addCar(Car &car) {
    //CHECKS FOR CAR DUPLICATES
    for (const auto &existingCar : cars) {
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

void GLM::removeCar(const QString &carID)
{
    for (int i = 0; i < cars.size(); ++i) {
        if (cars[i].ID == carID) {
            cars.removeAt(i);
            return;
        }
    }
}

void GLM::updateGameState() {
    for (Car &car : cars) {
        car.updatePosition();

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

void GLM::updateGameState(const QJsonArray &clientInputs) {
    QJsonObject carInput;
    QString carID;
    for (const auto &input : clientInputs) {
        carInput = input.toObject();
        carID = carInput["id"].toString();

        for (auto &car : cars) {
            if (carID == car.ID) {
                updateCarState(car, carInput);
                break;
            }
        }
    }
}

int GLM::getNextAvailableCar()
{
    if (cars.size() < 4){
        return cars.max_size()+1;
    }else{
        return -1;
    }
}


void GLM::updateCarState(Car &car, const QJsonObject &json)
{
    car.updateCarState(json);

    if (!raceTrack.isOnTrack(car)) {
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

    for (auto &car : cars) {
        carsArray.append(car.carStateJson());
    }

    gameState["cars"] = carsArray;
    return QJsonDocument(gameState).toJson();
}

