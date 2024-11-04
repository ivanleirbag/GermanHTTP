#include "car.h"


Car::Car(QString& carImgDir, int carID, int initX, int initY)
{
    setImage(carImgDir);
    ID = carID;
    position.setX(initX);
    position.setY(initY);
}

void Car::setImage(QString &carImgDir)
{
    QFile carImgFile;

    carImgFile.setFileName(carImgDir);
    carImgFile.open(QFile::ReadOnly);

    image.clear();
    image.append(carImgFile.readAll());

    carImgFile.close();
}

QByteArray Car::getImage()
{
    return image;
}

void Car::updateCorners()
{
    corners
}
