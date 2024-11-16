#include "car.h"


Car::Car(QString& carImgDir, QString& clientIP, int initX, int initY, int radius, float sDirection)
    : hitbox(radius)
{
    setImage(carImgDir);
    ID = clientIP;
    position.setX(initX);
    position.setY(initY);
    hitbox.updatePosition(position);
    speed = 0;
    direction = sDirection;

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

void Car::savePreviousState()
{
    prevPosX = posX;
    prevPosY = posY;
    prevSpeed = speed;
    prevDirection = direction;
}

void Car::restorePreviousState()
{
    posX = prevPosX;
    posY = prevPosY;
    speed = prevSpeed;
    direction = prevDirection;
    position.setX(posX);
    position.setY(posY);
    hitbox.updatePosition(position);
}

void Car::updateCarState(const QJsonObject &json)
{
    savePreviousState();

    position.setX(json["posX"].toInt());
    position.setY(json["posY"].toInt());
    hitbox.updatePosition(position);

    speed = json["speed"].toInt();
    direction = json["direction"].toDouble();
}

void Car::updatePosition()
{
    savePreviousState();

    int xIncrement = speed * qCos(direction);
    int yIncrement = speed * qSin(direction);

    position.setX((position.x()+xIncrement));
    position.setY((position.y()+yIncrement));
    hitbox.updatePosition(position);
}

bool Car::collidesWith(Car &otherCar)
{
    return hitbox.collidesWith(otherCar.hitbox);
}

QJsonObject Car::carStateJson()
{
    QJsonObject json;
    json["id"] = ID;
    json["posX"] = position.x();
    json["posY"] = position.y();
    json["speed"] = speed;
    json["direction"] = direction;
    return json;
}


