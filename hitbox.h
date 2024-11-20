#ifndef HITBOX_H
#define HITBOX_H

#include <QPoint>
#include <QtMath>

class Hitbox {
public:
    //Public Vars
    QPointF center;
    float radius;

    Hitbox(float radius);

    void updatePosition(QPointF& newPos);

    bool collidesWith(Hitbox& other);
};

#endif // HITBOX_H
