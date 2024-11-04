#ifndef CAR_H
#define CAR_H

#include "QString"
#include <QImage>
#include <QColor>
#include "QFile"
#include <vector>
#include <QPoint>

class Car
{
public:
    //Public Vars
    QPoint position;
    std::vector<QPoint>corners;

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
    Car(QString& carImgDir, int carID, int initX, int initY);

    /**
     * @brief getImage
     * Car image getter
     * @return
     * Returns a QByteArray containing the car image
     */
    QByteArray getImage();

    void updateCorners();

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
