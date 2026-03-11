#ifndef BLOB_H
#define BLOB_H

#include <QVector2D>
#include <QVector3D>
#include <QOpenGLFunctions>
class Blob : private QOpenGLFunctions
{
public:
    Blob(double x, double y);

    QVector2D getCenter() const { return position; }

    double getX() {return position.x();};
    double getY() {return position.y();};
    QVector2D getPosition() {return position;};
    QVector2D getVelocity() {return velocity;}
    std::vector<float> getVertexData();

    void update(float dt);
    void applyHit(QPointF direction, float power);
    void updatePointsPosition();
    bool checkCollision(Blob& other);
    void resolveCollision(Blob& other);
    void checkBoundaryCollision(float worldWidth, float worldHeight);
protected:
    struct Point {
        QVector2D pos;
    };
    std::vector<Point> points;
    QVector2D position;
    QVector3D color;
    QVector2D velocity;
    float mass;
    float radius = 50.0f;
};


#endif
