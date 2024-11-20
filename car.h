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
    int ID;
    float posX;
    float posY;
    int speed;
    float direction;
    QString clientip;
    qint16 clientport;
    QPointF position;
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

    void updateCarState(const QJsonObject &json);

    void updatePosition();

    bool collidesWith(Car &otherCar);

    QJsonObject carStateJson();
private:
    //Private Variables
    QByteArray image;
    float prevPosX;
    float prevPosY;
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
