#include "hitbox.h"

Hitbox::Hitbox(float hbRadius)
{
    radius = hbRadius;
}

void Hitbox::updatePosition(QPointF &newPos)
{
    center = newPos;
}

bool Hitbox::collidesWith(Hitbox &other)
{
    float dx = center.x() - other.center.x();
    float dy = center.y() - other.center.y();
    float dist = std::sqrt(dx*dx + dy*dy);

    float radSum = radius*2;

    if(dist <= radSum){
        return true;
    }else{
        return false;
    }
}
