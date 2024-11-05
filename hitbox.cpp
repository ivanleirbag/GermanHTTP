#include "hitbox.h"

Hitbox::Hitbox(int hbWidth, int hbHeight)
{
    width = hbWidth;
    height = hbHeight;
}

void Hitbox::updatePosition(QPoint &newPos)
{
    topLeft = QPoint(newPos.x() - (width / 2), newPos.y() - (height / 2));
    bottomRight = QPoint(newPos.x() + (width / 2), newPos.y() + (height / 2));
}

bool Hitbox::collidesWith(Hitbox &other)
{
    if((bottomRight.x() >= other.topLeft.x() || topLeft.x() <= other.bottomRight.x()) &&
        (bottomRight.y() <= other.topLeft.y() || topLeft.y() >= other.bottomRight.y())){
        return true;
    }else{
        return false;
    }
}
