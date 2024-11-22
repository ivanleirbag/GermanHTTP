#ifndef GLM_H
#define GLM_H

#include "car.h"
#include "racetrack.h"
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

//GAME LOGIC MODULE
class GLM {
public:
    /**
     * @brief GLM
     * Constructor for the GLM class
     * Initializes the racetrack with the specified track and background paths
     * @param trackPath
     * Path to the racetrack image file
     * @param backgroundPath
     * Path to the background image file
     */
    GLM(QString &trackPath, QString &backgroundPath);

    /**
     * @brief addCar
     * Adds a car to the game, ensuring no duplicates and a maximum of 4 cars
     * @param car
     * The car to add
     */
    void addCar(Car &car);

    /**
     * @brief removeCar
     * Removes a car from the game cheking its client IP and port
     * @param clientip
     * IP address of the client associated with the car
     * @param clientport
     * Port number of the client associated with the car
     */
    void removeCar(const QString clientip, const qint16 clientport);

    /**
     * @brief updateGameState
     * Updates the game state, resolving collisions with other cars and the track
     * Overloaded function
     */
    void updateGameState();

    /**
     * @brief updateGameState
     * Updates the game state based on input from a specific client,
     *  resolving collisions with other cars and the track
     * Overloaded function
     * @param clientInputs
     * JSON object containing client inputs
     */
    void updateGameState(const QJsonObject &clientInputs);

    /**
     * @brief getNextAvailableCar
     * Gets the ID for the next available car
     * @return An integer representing the next car ID (1-4)
     */
    int getNextAvailableCar();

    /**
     * @brief clearGLM
     * Clears the game state, removing all cars
     */
    void clearGLM();

    /**
     * @brief getGameState
     * Serializes the current state of all cars to JSON format
     * @return A QByteArray containing the serialized game state
     */
    QByteArray getGameState();

private:
    QVector<Car> cars;      ///< List of cars in game
    RaceTrack raceTrack;    ///< The racetrack object

    /**
     * @brief updateCarState
     * Updates the state of a specific car based on JSON input
     * @param car
     * The car to update
     * @param json
     * JSON object containing the car's new state
     */
    void updateCarState(Car &car, const QJsonObject &json);
};

#endif // GLM_H
