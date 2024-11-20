#include "racetrack.h"

RaceTrack::RaceTrack(QString &pathMapDir, QString &backgroundDir){
    loadTrackFromImage(pathMapDir);
    setBackgroundImage(backgroundDir);

}

bool RaceTrack::isOnTrack(int x, int y)
{
    if (x < 0 || x >= trackMask.width() || y < 0 || y >= trackMask.height()) {
        return false;
    }
    return trackMask.pixelIndex(x, y) == 1;
}


bool RaceTrack::isOnTrack(Car &car)
{
    QPointF center = car.hitbox.center;
    float radius = car.hitbox.radius;

    int numSegments = 6;
    double angleIncrement = 2 * M_PI / numSegments;

    for (int i = 0; i < numSegments; ++i) {
        double angle = i * angleIncrement;
        int checkX =  static_cast<int>(center.x() + (radius * qCos(angle)));
        int checkY =  static_cast<int>(center.y() + (radius * qSin(angle)));
        if (isOnTrack(checkX, checkY) == false) {
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

    trackMask = QImage(image.size(), QImage::Format_Mono);
    trackMask.fill(0);

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            color = image.pixelColor(x, y);
            bool isBlack = (color.redF() < 0.2 && color.greenF() < 0.2 && color.blueF() < 0.2);
            if (isBlack) {
                trackMask.setPixel(x, y, 1);
            }
        }
    }

    qDebug() << "Máscara de pista cargada. Tamaño:" << trackMask.size();
    qDebug() << trackMask.save("trackMask_debug.png");

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
