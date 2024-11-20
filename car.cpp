#include "car.h"


Car::Car(QString& carImgDir, int sID, float initX, float initY, float radius, float sDirection, QString sclientip, qint16 sclientport)
    : hitbox(radius)
{
    setImage(carImgDir);
    ID = sID;
    posX = initX;
    posY = initY;
    position.setX(initX);
    position.setY(initY);
    hitbox.updatePosition(position);
    speed = 0;
    direction = sDirection;

    clientip = sclientip;
    clientport = sclientport;

    prevPosX = initX;
    prevPosY = initY;
    prevSpeed = speed;
    prevDirection = sDirection;

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
    prevPosX = position.x();
    prevPosY = position.y();
    prevSpeed = speed;
    prevDirection = direction;
}

void Car::restorePreviousState()
{
  //  qDebug() << "RESTORING STATE";
  //  qDebug() << " prevx " << prevPosX << " prevy " << prevPosY<< " prevspeed " << speed<< " prevdir " << prevDirection;
    speed = prevSpeed;
    direction = prevDirection;
    position.setX(prevPosX);
    position.setY(prevPosY);
    hitbox.updatePosition(position);
}

void Car::updateCarState(const QJsonObject &json)
{
    savePreviousState();
    position.setX(json["posX"].toDouble());
    position.setY(json["posY"].toDouble());
    hitbox.updatePosition(position);
    speed = json["speed"].toInt();
    direction = json["direction"].toDouble();
}

void Car::updatePosition()
{
    savePreviousState();

    float xIncrement = speed * qCos(direction);
    float yIncrement = speed * qSin(direction);
    float newX = (position.x()+xIncrement);
    float newY = (position.y()+yIncrement);

    if (!(newX > 0)){
        newX = position.x();
    }
    if (!(newY > 0)){
        newY = position.y();
    }
    position.setX(newX);
    position.setY(newY);
    hitbox.updatePosition(position);
}

bool Car::collidesWith(Car &otherCar)
{
    return hitbox.collidesWith(otherCar.hitbox);
}

QJsonObject Car::carStateJson()
{
    QJsonObject json;
    json["ID"] = ID;
    json["posX"] = position.x();
    json["posY"] = position.y();
    json["speed"] = speed;
    json["direction"] = direction;
    return json;
}


