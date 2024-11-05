#ifndef HITBOX_H
#define HITBOX_H

#include <QPoint>

class Hitbox {
public:
    //Public Vars
    QPoint topLeft, bottomRight;
    int width, height;

    Hitbox(int width, int height);

    void updatePosition(QPoint& newPos);

    bool collidesWith(Hitbox& other);
};

#endif // HITBOX_H
