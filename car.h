#ifndef CAR_H
#define CAR_H

#include "QString"
#include <QImage>
#include <QColor>
#include "QFile"
#include <QPoint>
#include "hitbox.h"
#include <QJsonObject>

class Car
{
public:
    //Public Vars
    int ID;
    int posX;
    int posY;
    int speed;
    float direction;
    QPoint position;
    Hitbox hitbox;

    //Public Funcs
    /**
     * @brief Car
     * Car object constructor
     * Loads the the image for the car and asigns an ID to it
     * Asigns initial X and Y positions
     * @param carImgDir
     * Directory to the car image
     * @param carID
     * @param initX
     * @param initY
     */
    Car(QString& carImgDir, int carID, int initX, int initY, int width, int height, float sDirection);

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

    void updateCarState(const QJsonObject &json);

    bool collidesWith(Car &otherCar);

    QJsonObject carStateJson();
private:
    //Private Variables
    QByteArray image;
    int prevPosX;
    int prevPosY;
    int prevSpeed;
    float prevDirection;

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
