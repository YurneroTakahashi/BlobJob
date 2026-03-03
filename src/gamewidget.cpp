#include "include/gamewidget.h"

gameWidget::gameWidget(QWidget *parent) : QOpenGLWidget(parent){
    setFocusPolicy(Qt::StrongFocus);

    startTimer(0);

    setAttribute(Qt::WA_AcceptTouchEvents);
}

gameWidget::~gameWidget(){
}




void gameWidget::initializeGL(){
    m_timer.start();
    m_startTime = m_timer.elapsed();


    ourShader.compile("C:/Users/Yurnero/QT/BlobJob/shaders/basicShader.vert",
                      "C:/Users/Yurnero/QT/BlobJob/shaders/basicShader.frag");
    initializeOpenGLFunctions();

    float vertices[] = {
        // positions         // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f    // top left
    };


    GLuint indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };


    //buffer initialization
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    Blob player(0.0,0.0);
    blobs.push_back(player);


}

void gameWidget::resizeGL(int w,int h){
    glViewport(0, 0, w, h);
    projection.setToIdentity();
    projection.ortho(0, w, h, 0, -1, 1);

}
void gameWidget::paintGL(){
    glClearColor(1.0f, 1.0f, 1.20f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    QMatrix4x4 viewMatrix = camera.getViewMatrix();

    ourShader.use();
    ourShader.setMatrix4("projection", projection);
    ourShader.setMatrix4("view", viewMatrix);

    qint64 seconds = m_timer.elapsed();
    float timeValue = seconds / 700.0f;
    ourShader.setVec4("ourColor", QVector4D((sin(timeValue)/2.0f)+0.5f,(sin(timeValue+2.0)/2.0)+0.5,(sin(timeValue+4.0)/2.0)+0.5, 1.0));

    glBindVertexArray(VAO);

    for(Blob& blob : blobs){
        QMatrix4x4 modelMatrix;
        modelMatrix.translate(blob.getX(),blob.getY(),0.0f);
        modelMatrix.scale(100.0f);

        ourShader.setMatrix4("model", modelMatrix);


        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);

}






void gameWidget::timerEvent(QTimerEvent* event){
    static int frameCount = 0;
    static qint64 lastTime = 0;

    frameCount++;
    qint64 currentTime = m_timer.elapsed();

    if (currentTime - lastTime >= 1000) {
        qDebug() << "FPS:" << frameCount;
        frameCount = 0;
        lastTime = currentTime;
    }

    update();
}


void gameWidget::keyPressEvent(QKeyEvent* event) {
    float moveSpeed = 100.0f;

    switch(event->key()) {
    case Qt::Key_W:
    case Qt::Key_Up:
        camera.move(0, -moveSpeed);
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        camera.move(0, moveSpeed);
        break;
    case Qt::Key_A:
    case Qt::Key_Left:
        camera.move(-moveSpeed, 0);
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        camera.move(moveSpeed, 0);
        break;
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        camera.zoomIn(1.2f);
        break;
    case Qt::Key_Minus:
        camera.zoomOut(1.2f);
        break;
    }
    qDebug() << camera.getZoom() << Qt::endl;
    update();
}



void gameWidget::wheelEvent(QWheelEvent* event) {
    int delta = event->angleDelta().y();

    if (delta > 0) {
        camera.zoomIn(1.7f);
    } else {
        camera.zoomOut(1.7f);
    }

    update();
}

