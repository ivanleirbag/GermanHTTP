#include "racetrack.h"

RaceTrack::RaceTrack(QString &pathMapDir, QString &backgroundDir){
    loadTrackFromImage(pathMapDir);
    setBackgroundImage(backgroundDir);
}

bool RaceTrack::isOnTrack(int x, int y)
{
    return trackPixels.contains(QPoint(x, y));
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

    backgroundImage.append(backgroundFile.readAll());

    backgroundFile.close();
}

QByteArray RaceTrack::getBackgroundImage()
{
    return backgroundImage;
}
