#ifndef BLOB_H
#define BLOB_H

#include <QVector2D>
#include <QVector3D>
#include <QOpenGLFunctions>
class Blob : private QOpenGLFunctions
{
public:
    Blob(double x, double y);

    double getX() {return position.x();};
    double getY() {return position.y();};
    QVector2D getPosition() {return position;};
protected:
    QVector2D position;
    QVector3D color;
    QVector2D velocity;
};


#endif // BLOB_H
