#ifndef HITBOX_H
#define HITBOX_H

#include <QPoint>
#include <QtMath>

class Hitbox {
public:
    //Public Vars
    QPointF center;
    float radius;

    /**
     * @brief Hitbox
     * Constructor for the Hitbox class
     * Initializes the hitbox with a specific radius
     * @param radius Radius of the hitbox
     */
    Hitbox(float radius);

    /**
     * @brief updatePosition
     * Updates the position of the hitbox
     * @param newPos
     * New position for the hitbox center as a QPointF
     */
    void updatePosition(QPointF& newPos);

    /**
     * @brief collidesWith
     * Checks for a collision with another hitbox
     * Calculates the distance between the centers of the two hitboxes plus the radius
     * @param other
     * The other hitbox to check collision with
     * @return True if the hitboxes collide, false otherwise
     */
    bool collidesWith(Hitbox& other);
};

#endif // HITBOX_H
