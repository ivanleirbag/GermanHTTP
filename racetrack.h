#ifndef RACETRACK_H
#define RACETRACK_H

#include "QString"
#include <QImage>
#include <QColor>
#include "QFile"
#include <QSet>
#include <QPoint>

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

    /**
     * @brief getBackgroundImage
     * Returns a QByteArray cointaining the background image of the race track
     */
    QByteArray getBackgroundImage();

private:
    QString backgroundDir;
    QSet<QPoint> trackPixels;
    QByteArray backgroundImage;

    void loadTrackFromImage(QString& imagePath);
    void setBackgroundImage(QString& backgroundDir);
};


#endif // RACETRACK_H
