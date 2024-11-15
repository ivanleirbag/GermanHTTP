#include "glm.h"

#include "glm.h"

GLM::GLM(RaceTrack &track)
    : raceTrack(track) {}

QString GLM::addCar(Car &car) {
    QString success;
    if (cars.size() < 4){
        cars.append(car);
        success = "New car added successfully";
        return success;
    }else{
        success = "Could not add new car. Max player capacity reached.";
        return success;
    }
}


void GLM::updateGameState(const QJsonArray &clientInputs) {
    for (const auto &input : clientInputs) {
        QJsonObject carInput = input.toObject();
        int carID = carInput["id"].toInt();

        for (auto &car : cars) {
            if (carID == car.ID) {
                updateCarState(car, carInput, raceTrack);
                break;
            }
        }
    }

    handleCollisions();
}

void GLM::updateCarState(Car &car, const QJsonObject &json,  RaceTrack &track)
{
    car.updateCarState(json);

    if (!track.isOnTrack(car)) {
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


void GLM::handleCollisions() {
    for (int i = 0; i < cars.size(); ++i) {
        for (int j = i + 1; j < cars.size(); ++j) {
            if (cars[i].collidesWith(cars[j])) {
                cars[i].speed = 0;
                cars[j].speed = 0;
            }
        }
    }
}

QJsonObject GLM::getGameStateJson() {
    QJsonObject gameState;
    QJsonArray carsArray;

    for (auto &car : cars) {
        carsArray.append(car.carStateJson());
    }

    gameState["cars"] = carsArray;
    return gameState;
}

