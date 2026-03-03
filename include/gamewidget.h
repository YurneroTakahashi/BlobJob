#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "include/blob.h"
#include "include/shaderprogram.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
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

    void initializeGL() override;
    void resizeGL(int w,int h) override;
    void paintGL() override;

};

#endif // GAMEWIDGET_H
