#ifndef CAR_H
#define CAR_H

#include "QString"
#include <QImage>
#include <QColor>
#include "QFile"
#include <QPoint>
#include "hitbox.h"
#include <QJsonObject>
#include <QtMath>

class Car
{
public:
    //Public Vars
    int ID;             ///< Unique identifier for the car
    float posX;         ///< Current X position of the car
    float posY;         ///< Current Y position of the car
    int speed;          ///< Current speed of the car
    float direction;    ///< Current direction angle of the car in radians
    QString clientip;   ///< IP address of the client controlling the car
    qint16 clientport;   ///< Port of the client controlling the car
    QPointF position;   ///< Current position of the car as a QPointF for better handling with the hitbox and racetrack classes
    Hitbox hitbox;      ///< Hitbox for collision detection

    //Public Funcs
    /**
     * @brief Car
     * Car object constructor
     * Loads the the image for the car and asigns an ID to it
     * Sets initial X and Y positions, initializes the hitbox, and sets client information
     * @param carImgDir
     * Directory to the car image
     * @param sID
     * Unique identifier for the car
     * @param initX
     * Initial X position of the car
     * @param initY
     * Initial Y position of the car
     * @param radius
     * Radius of the car's hitbox
     * @param sDirection
     * Initial direction of the car in radians
     * @param sclientip
     * Client IP address as a QString
     * @param sclientport
     * Client port as a qint16
     */
    Car(QString& carImgDir, int sID, float initX, float initY, float radius, float sDirection, QString sclientip, qint16 sclientport);

    /**
     * @brief getImage
     * Car image getter
     * @return
     * Returns a QByteArray containing the car image
     */
    QByteArray getImage();

    /**
     * @brief savePreviousState
     * Saves the previous state of the car
     */
    void savePreviousState();

    /**
     * @brief restorePreviousState
     * Restores the previous state of the car
     */
    void restorePreviousState();

    /**
     * @brief updateCarState
     * Updates the car's state using data from a QJsonObject
     * Saves the car previous car's state first
     * @param json A QJsonObject containing the car's new state
     */
    void updateCarState(const QJsonObject &json);

    /**
     * @brief updatePosition
     * Updates the car's position based on its speed and direction
     * Ensures the new position is positive.
     */
    void updatePosition();


    /**
     * @brief collidesWith
     * Checks if the car collides with another car
     * @param otherCar
     * The other car to check collision with
     * @return True if there is a collision, false otherwise
     */
    bool collidesWith(Car &otherCar);

    /**
     * @brief carStateJson
     * Converts the car's current state to a JSON object
     * @return A QJsonObject containing the car's state (ID, position, speed, direction)
     */
    QJsonObject carStateJson();

private:
    //Private Variables
    QByteArray image;       ///< Byte array containing the car's image
    float prevPosX;         ///< Previous X position of the car
    float prevPosY;         ///< Previous Y position of the car
    int prevSpeed;          ///< Previous speed of the car
    float prevDirection;    ///< Previous direction of the car

    //Private Funcs
    /**
     * @brief setImage
     * Car image setter
     * @param carImgDir
     * Directory to the car image
     */
    void setImage(QString& carImgDir);


};

#endif // CAR_H
