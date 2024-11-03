#ifndef RACETRACK_H
#define RACETRACK_H

#include "QString"
#include <QImage>
#include <QColor>
#include <vector>
#include "QFile"

class RaceTrack {
public:
    /**
     * @brief RaceTrack
     * Race track class constructor
     * @param imagePath
     */
    RaceTrack(QString& pathMapDir, QString& backgrounDir);

    /**
     * @brief isOnTrack
     * Checks if the coordinates given are inside of the track path
     * @param x
     * @param y
     * @return
     * True if it's on track, False if it's not
     */
    bool isOnTrack(int x, int y);

private:
    QString backgroundDir;
    QSet<QPoint> pathPixels;

    void loadTrackFromImage(QString& imagePath);
    void setBackgroundImage(QString& backgroundDir);
};


#endif // RACETRACK_H
