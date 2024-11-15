#ifndef HITBOX_H
#define HITBOX_H

#include <QPoint>
#include <QtMath>

class Hitbox {
public:
    //Public Vars
    QPoint center;
    int radius;

    Hitbox(int radius);

    void updatePosition(QPoint& newPos);

    bool collidesWith(Hitbox& other);
};

#endif // HITBOX_H
