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



    std::vector<float> vertices;
    vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
    vertices.push_back(0.5f); vertices.push_back(0.5f);

    const int segments = 100;
    for(int i = 0;i <= segments;i++){
        float angle = 2.0f * M_PI * i / segments;
        float x = cos(angle) * 0.5;
        float y = sin(angle) * 0.5;

        vertices.push_back(x); vertices.push_back(y); vertices.push_back(0.0f);
        vertices.push_back(x+0.5); vertices.push_back(y+0.5);
    }


    m_vertexCount = vertices.size() / 5;


    //buffer initialization
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);


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
    projection.ortho(-width()/2, width()/2, -height()/2, height()/2, -1, 1);

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


        glDrawArrays(GL_TRIANGLE_FAN, 0, m_vertexCount);
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
        camera.move(0, moveSpeed);
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        camera.move(0, -moveSpeed);
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

