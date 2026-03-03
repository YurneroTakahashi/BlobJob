#ifndef BLOB_H
#define BLOB_H

#include <QVector2D>
#include <QVector3D>
#include <QOpenGLFunctions>
class Blob : private QOpenGLFunctions
{
public:
    Blob(double x, double y);
protected:
    QVector2D position;
    QVector3D color;
    QVector2D velocity;
};


#endif // BLOB_H
