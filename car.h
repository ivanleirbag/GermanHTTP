#ifndef CAR_H
#define CAR_H

#include "QString"
#include <QImage>
#include <QColor>
#include "QFile"
#include <QPoint>
#include "hitbox.h"

class Car
{
public:
    //Public Vars
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
    Car(QString& carImgDir, int carID, int initX, int initY, int width, int height);

    /**
     * @brief getImage
     * Car image getter
     * @return
     * Returns a QByteArray containing the car image
     */
    QByteArray getImage();

    void updateCarPosition(int newX, int newY);

    bool collidesWith(Car &otherCar);
private:
    //Private Variables
    QByteArray image;
    int ID;

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
