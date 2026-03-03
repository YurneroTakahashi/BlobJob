#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "include/blob.h"
#include "include/shaderprogram.h"
#include "include/camera.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QMatrix4x4>
#include <QElapsedTimer>
#include <QKeyEvent>
class gameWidget : public QOpenGLWidget, private QOpenGLExtraFunctions
{
public:
    explicit gameWidget(QWidget *parent = nullptr);
    ~gameWidget();
protected:

    GLuint VAO,VBO,EBO;
    std::vector<Blob> blobs;
    size_t playerIndex = 0;
    ShaderProgram ourShader;
    QMatrix4x4 projection;

    QElapsedTimer m_timer;
    qint64 m_startTime;


    Camera camera;
    void timerEvent(QTimerEvent* event) override;
    void initializeGL() override;
    void resizeGL(int w,int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent*) override;
};

#endif // GAMEWIDGET_H
