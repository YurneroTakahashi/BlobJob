#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>

// camera.h
class Camera {
public:
    Camera();
    ~Camera();
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    void setPosition(float x, float y);
    void move(float dx, float dy);

    float getZoom() const { return m_zoom; }
    void setZoom(float zoom);
    void zoomIn(float factor);
    void zoomOut(float factor);

    void follow(float targetX, float targetY, float followSpeed = 0.1f);

    void setBounds(float minX, float maxX, float minY, float maxY);
    void applyBounds();

    QMatrix4x4 getViewMatrix() const;

private:
    float m_x, m_y;
    float m_zoom;
    float m_minX, m_maxX, m_minY, m_maxY;
    bool m_boundsEnabled;
};
#endif // CAMERA_H
