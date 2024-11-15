#include "racetrack.h"

RaceTrack::RaceTrack(QString &pathMapDir, QString &backgroundDir){
    loadTrackFromImage(pathMapDir);
    setBackgroundImage(backgroundDir);
}

bool RaceTrack::isOnTrack(int x, int y)
{
    return trackPixels.contains(QPoint(x, y));
}

bool RaceTrack::isOnTrack(Car &car)
{
    QPoint center = car.hitbox.center;
    int radius = car.hitbox.radius;

    const int numSegments = 16;
    const double angleIncrement = 2 * M_PI / numSegments;

    for (int i = 0; i < numSegments; ++i) {
        double angle = i * angleIncrement;
        int checkX = center.x() + static_cast<int>(radius * qCos(angle));
        int checkY = center.y() + static_cast<int>(radius * qSin(angle));

        if (!isOnTrack(checkX, checkY)) {
            return false;
        }
    }

    return true;
}



void RaceTrack::loadTrackFromImage(QString &pathMapDir) {
    QImage image(pathMapDir);
    QColor color;

    if (image.isNull()) {
        qWarning("No se pudo cargar la imagen de la pista.");
        return;
    }

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            color = image.pixelColor(x, y);
            if (color == Qt::black) {
                trackPixels.insert(QPoint(x, y));
            }
        }
    }
}

void RaceTrack::setBackgroundImage(QString &backgroundDir)
{
    QFile backgroundFile;

    backgroundFile.setFileName(backgroundDir);
    backgroundFile.open(QFile::ReadOnly);

    backgroundImage.clear();
    backgroundImage.append(backgroundFile.readAll());

    backgroundFile.close();
}

QByteArray RaceTrack::getBackgroundImage()
{
    return backgroundImage;
}
