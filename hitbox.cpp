#include "hitbox.h"

Hitbox::Hitbox(int hbRadius)
{
    radius = hbRadius;
}

void Hitbox::updatePosition(QPoint &newPos)
{
    center = newPos;
}

bool Hitbox::collidesWith(Hitbox &other)
{
    int dx = center.x() - other.center.x();
    int dy = center.y() - other.center.y();
    int dist = dx*dx + dy*dy;

    int radSum = radius + other.radius;

    //IF THE RADIUS'S SUM IS LESS THAN THE DISTANCE BETWEEN THEM THEN THERE'S A COLISIOn
    //EVERYTHING IS SQUARED SO SIGN DOESN'T MATTER
    return dist <= (radSum*radSum);
}
