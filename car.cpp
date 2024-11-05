#include "car.h"


Car::Car(QString& carImgDir, int carID, int initX, int initY, int width, int height)
    : hitbox(width, height)
{
    setImage(carImgDir);
    ID = carID;
    position.setX(initX);
    position.setY(initY);
    hitbox.updatePosition(position);
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

void Car::updateCarPosition(int newX, int newY)
{
    position.setX(newX);
    position.setY(newY);
    hitbox.updatePosition(position);
}

bool Car::collidesWith(Car &otherCar)
{
    return hitbox.collidesWith(otherCar.hitbox);
}


