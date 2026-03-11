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
#include <QTimer>
#include <QKeyEvent>
#include <cstdlib>
#include <random>
class gameWidget : public QOpenGLWidget, private QOpenGLExtraFunctions
{
public:
    explicit gameWidget(QWidget *parent = nullptr);
    ~gameWidget();
protected:

    GLuint VAO,VBO,EBO;
    GLuint m_vertexCount;
    std::vector<Blob> blobs;
    size_t playerIndex = 0;

    ShaderProgram ourShader;
    ShaderProgram pointShader;
    ShaderProgram arrowShader;
    float deltaTime;
    float maxDistance = 500.0f;
    QMatrix4x4 projection;

    QElapsedTimer m_timer;
    qint64 m_startTime;

    QPointF startPoint;
    QPointF currentPoint;
    bool isDragging;

    struct KeyState {
        bool w = false, a = false, s = false, d = false;
        bool up = false, down = false, left = false, right = false;
        bool zoomingIn = false, zoomingOut = false;
    };




    KeyState m_keys;
    QTimer m_updateTimer;

    Camera camera;
    void timerEvent(QTimerEvent* event) override;
    void initializeGL() override;
    void resizeGL(int w,int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent*) override;
    void updateGame();
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void playerJump(int playerIndex, QVector2D direction, float force);
    void debugRenderVertices(std::vector<float>& vertices, QMatrix4x4& viewMatrix, QMatrix4x4& modelMatrix);
    void drawArrow(QPointF start, QPointF end, Blob& player);
    QPointF screenToWorld(const QPointF& screenPos);
    void drawArrowhead(float startX, float startY, float endX, float endY);
    void handleCollisions();
    void updateSpatialGrid();

    static constexpr int GRID_SIZE = 100;
    std::vector<std::vector<std::vector<Blob*>>> spatialGrid;
};

#endif // GAMEWIDGET_H
