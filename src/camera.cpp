#include "../include/camera.h"

Camera::Camera() {
    m_zoom = 1.0;
    m_x = 0.0;
    m_y = 0.0;
}

Camera::~Camera(){

}

QMatrix4x4 Camera::getViewMatrix() const {
    QMatrix4x4 view;
    view.translate(-m_x, -m_y);
    view.scale(m_zoom);
    return view;
}


void Camera::move(float dx, float dy){
    m_x+=dx;
    m_y+=dy;
}

void Camera::zoomIn(float factor){
    m_zoom+=factor/50.0f;
    m_zoom=std::min(m_zoom, 1.5f);
}
void Camera::zoomOut(float factor){
    m_zoom-=factor/50.0f;
    m_zoom=std::max(m_zoom,0.2f);
}
