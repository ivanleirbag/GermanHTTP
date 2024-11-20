#ifndef RACETRACK_H
#define RACETRACK_H

#include "QString"
#include <QImage>
#include <QColor>
#include "QFile"
#include <QSet>
#include <QPoint>
#include "car.h"
#include <QtMath>

class RaceTrack {
public:
    /**
     * @brief RaceTrack
     * Race track class constructor
     * @param pathMapDir
     * @param backgroundDIr
     */
    RaceTrack(QString& pathMapDir, QString& backgrounDir);

    /**
     * @brief isOnTrack
     * Checks if the coordinates given are inside of the track path
     * This function is overloaded
     * @param x
     * @param y
     * @return
     * True if it's on track, False if it's not
     */
    bool isOnTrack(int x, int y);

    void debugMask();

    bool isOnTrack(Car& car);

    /**
     * @brief getBackgroundImage
     * Returns a QByteArray cointaining the background image of the race track
     */
    QByteArray getBackgroundImage();

private:
    QString backgroundDir;
    QImage trackMask;
    int trackWidth;
    int trackHeight;
    QByteArray backgroundImage;

    void loadTrackFromImage(QString& imagePath);
    void setBackgroundImage(QString& backgroundDir);
};


#endif // RACETRACK_H
