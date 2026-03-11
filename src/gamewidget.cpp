#include "include/gamewidget.h"

gameWidget::gameWidget(QWidget *parent) : QOpenGLWidget(parent){
    setFocusPolicy(Qt::StrongFocus);

    startTimer(0);
    connect(&m_updateTimer, &QTimer::timeout, this, &gameWidget::updateGame);
    m_updateTimer.start(16);
    setAttribute(Qt::WA_AcceptTouchEvents);
    int gridCols = static_cast<int>(width() / GRID_SIZE) + 1;
    int gridRows = static_cast<int>(height() / GRID_SIZE) + 1;
    spatialGrid.resize(gridCols);
    for (auto& col : spatialGrid) {
        col.resize(gridRows);
    }
}

gameWidget::~gameWidget(){
}




void gameWidget::initializeGL(){
    m_timer.start();
    m_startTime = m_timer.elapsed();


    ourShader.compile("../../../shaders/basicShader.vert",
                      "../../../shaders/basicShader.frag");
    pointShader.compile("../../../shaders/pointShader.vert",
                        "../../../shaders/pointShader.frag");
    arrowShader.compile("../../../shaders/arrowShader.vert",
                        "../../../shaders/arrowShader.frag");
    initializeOpenGLFunctions();
    glEnable(GL_PROGRAM_POINT_SIZE);


    std::vector<float> vertices;
    vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
    vertices.push_back(0.5f); vertices.push_back(0.5f);

    const int segments = 32;
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
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posXDist(-width()/2 + 100, width()/2 - 100);
    std::uniform_real_distribution<float> posYDist(-height()/2 + 100, height()/2 - 100);

    // Игрок
    Blob player(0.0, 0.0);
    blobs.push_back(player);

    // Много случайных шаров
    for (int i = 0; i < 1; ++i) {
        Blob bot(posXDist(gen), posYDist(gen));
        /*bot.setVelocity(QVector2D(
            (rand() % 600) - 300,
            (rand() % 600) - 300
            ));*/
        blobs.push_back(bot);
    }

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



    glBindVertexArray(VAO);

    if (isDragging) {
        drawArrow(startPoint, currentPoint, blobs[playerIndex]);
    }




    for(Blob& blob : blobs){
        //qDebug() << blob.getX() << blob.getY() << Qt::endl;
        ourShader.use();
        ourShader.setMatrix4("projection", projection);
        ourShader.setMatrix4("view", viewMatrix);
        qint64 seconds = m_timer.elapsed();
        float timeValue = seconds / 700.0f;
        ourShader.setVec4("ourColor", QVector4D((sin(timeValue)/2.0f)+0.5f,(sin(timeValue+2.0)/2.0)+0.5,(sin(timeValue+4.0)/2.0)+0.5, 1.0));
        std::vector<float> vertices = blob.getVertexData(); // только xy(й)
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        QMatrix4x4 modelMatrix;
        modelMatrix.translate(blob.getX(), blob.getY(), 0.0f);
        ourShader.setMatrix4("model", modelMatrix);

        glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size() / 2);
        //debugRenderVertices(vertices, viewMatrix, modelMatrix);

        glDeleteBuffers(1, &vbo);
    }
    glBindVertexArray(0);
}

void gameWidget::updateSpatialGrid() {
    // Очищаем сетку
    for (auto& col : spatialGrid) {
        for (auto& cell : col) {
            cell.clear();
        }
    }

    // Заполняем сетку шарами
    for (auto& blob : blobs) {
        QVector2D pos = blob.getPosition();

        // Преобразуем мировые координаты в индексы сетки
        int gridX = static_cast<int>((pos.x() + width()/2) / GRID_SIZE);
        int gridY = static_cast<int>((pos.y() + height()/2) / GRID_SIZE);

        // Проверяем, что индексы в пределах сетки
        if (gridX >= 0 && gridX < static_cast<int>(spatialGrid.size()) &&
            gridY >= 0 && gridY < static_cast<int>(spatialGrid[0].size())) {
            spatialGrid[gridX][gridY].push_back(&blob);
        }
    }
}



void gameWidget::handleCollisions() {
    // Обновляем пространственную сетку
    updateSpatialGrid();

    // Проверяем столкновения только между шарами в соседних ячейках
    for (int gx = 0; gx < static_cast<int>(spatialGrid.size()); ++gx) {
        for (int gy = 0; gy < static_cast<int>(spatialGrid[0].size()); ++gy) {
            auto& currentCell = spatialGrid[gx][gy];

            // Проверка столкновений внутри текущей ячейки
            for (size_t i = 0; i < currentCell.size(); ++i) {
                for (size_t j = i + 1; j < currentCell.size(); ++j) {
                    if (currentCell[i]->checkCollision(*currentCell[j])) {
                        currentCell[i]->resolveCollision(*currentCell[j]);
                    }
                }
            }

            // Проверка с соседними ячейками
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx == 0 && dy == 0) continue;

                    int ngx = gx + dx;
                    int ngy = gy + dy;

                    if (ngx >= 0 && ngx < static_cast<int>(spatialGrid.size()) &&
                        ngy >= 0 && ngy < static_cast<int>(spatialGrid[0].size())) {

                        auto& neighborCell = spatialGrid[ngx][ngy];

                        // Проверяем столкновения между шарами из текущей и соседней ячейки
                        for (auto* ball1 : currentCell) {
                            for (auto* ball2 : neighborCell) {
                                if (ball1 != ball2 && ball1->checkCollision(*ball2)) {
                                    ball1->resolveCollision(*ball2);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


void gameWidget::drawArrow(QPointF start, QPointF end, Blob& player) {
    // Получаем позицию игрока в мировых координатах
    float playerX = player.getX();
    float playerY = player.getY();

    // Конвертируем начальную и конечную точки в мировые координаты
    QPointF worldStart = screenToWorld(start);
    QPointF worldEnd = screenToWorld(end);

    // Вычисляем вектор направления от начальной точки к текущей
    float dirX = worldEnd.x() - worldStart.x();
    float dirY = worldEnd.y() - worldStart.y();

    // Вычисляем длину (силу удара)
    float length = sqrt(dirX * dirX + dirY * dirY);

    // Ограничиваем максимальную длину
    float maxLength = 300.0f;
    float limitedLength = qMin(length, maxLength);

    // Направление удара (нормализованное)
    float normDirX = 0;
    float normDirY = 0;
    if (length > 0) {
        normDirX = dirX / length;
        normDirY = dirY / length;
    }

    // Стрелка начинается от позиции игрока
    float arrowStartX = playerX;
    float arrowStartY = playerY;

    // Конец стрелки - позиция игрока + нормализованное направление * ограниченную длину
    float arrowEndX = playerX + normDirX * limitedLength;
    float arrowEndY = playerY + normDirY * limitedLength;

    // Вершины для линии
    std::vector<float> lineVertices = {
        arrowStartX, arrowStartY,
        arrowEndX, arrowEndY
    };

    arrowShader.use();

    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();

    arrowShader.setMatrix4("projection", projection);
    arrowShader.setMatrix4("view", camera.getViewMatrix());
    arrowShader.setMatrix4("model", modelMatrix);

    GLuint lineVBO;
    glGenBuffers(1, &lineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(float),
                 lineVertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glLineWidth(15.0f);

    // Цвет от зеленого (слабо) до красного (сильно)
    float power = limitedLength / maxLength;
    arrowShader.setVec4("ourColor", QVector4D(power, 1.0f - power, 0.0f, 1.0f));

    glDrawArrays(GL_LINES, 0, 2);

    // Рисуем наконечник стрелки
    if (limitedLength > 20.0f) {
        drawArrowhead(arrowStartX, arrowStartY, arrowEndX, arrowEndY);
    }

    glDisableVertexAttribArray(0);
    glDeleteBuffers(1, &lineVBO);

    ourShader.use();
}
void gameWidget::drawArrowhead(float startX, float startY, float endX, float endY) {
    // Вычисляем угол направления стрелки
    float angle = atan2(endY - startY, endX - startX);

    // Размер наконечника (зависит от длины стрелки)
    float arrowSize = 25.0f;
    float arrowAngle = M_PI / 6.0f; // 30 градусов

    // Вершины наконечника (треугольник)
    float x1 = endX; // острие наконечника
    float y1 = endY;

    // Левое "крыло"
    float x2 = endX - arrowSize * cos(angle - arrowAngle);
    float y2 = endY - arrowSize * sin(angle - arrowAngle);

    // Правое "крыло"
    float x3 = endX - arrowSize * cos(angle + arrowAngle);
    float y3 = endY - arrowSize * sin(angle + arrowAngle);

    std::vector<float> arrowVertices = {
        x1, y1,
        x2, y2,
        x3, y3
    };

    arrowShader.use();

    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();

    arrowShader.setMatrix4("projection", projection);
    arrowShader.setMatrix4("view", camera.getViewMatrix());
    arrowShader.setMatrix4("model", modelMatrix);

    // Белый цвет для наконечника
    arrowShader.setVec4("ourColor", QVector4D(0.0f, 0.0f, 0.0f, 1.0f));

    GLuint arrowVBO;
    glGenBuffers(1, &arrowVBO);
    glBindBuffer(GL_ARRAY_BUFFER, arrowVBO);
    glBufferData(GL_ARRAY_BUFFER, arrowVertices.size() * sizeof(float),
                 arrowVertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
    glDeleteBuffers(1, &arrowVBO);

    ourShader.use();
}



void gameWidget::debugRenderVertices(std::vector<float>& vertices, QMatrix4x4& viewMatrix, QMatrix4x4& modelMatrix){
    pointShader.use();
    pointShader.setMatrix4("projection", projection);
    pointShader.setMatrix4("view", viewMatrix);
    pointShader.setMatrix4("model", modelMatrix);
    glDisableVertexAttribArray(1);
    GLuint pointVBO;
    glGenBuffers(1, &pointVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                vertices.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_POINTS, 0, vertices.size() / 2);
    glDeleteBuffers(1, &pointVBO);
    glEnableVertexAttribArray(1);
    ourShader.use();
}


void gameWidget::updateGame() {
    static QElapsedTimer timer;
    if (!timer.isValid()) timer.start();

    deltaTime = timer.elapsed() / 1000.0f;
    deltaTime = qMin(deltaTime, 0.016f);
    timer.restart();

    float cameraSpeed = 200.0f;  // pixels per second
    float zoomSpeed = 100.0f;
    float moveDelta = cameraSpeed * deltaTime;
    float zoomDelta = zoomSpeed * deltaTime;
    if (m_keys.w || m_keys.up) camera.move(0, moveDelta);
    if (m_keys.s || m_keys.down) camera.move(0, -moveDelta);
    if (m_keys.a || m_keys.left) camera.move(-moveDelta, 0);
    if (m_keys.d || m_keys.right) camera.move(moveDelta, 0);
    if (m_keys.zoomingIn) camera.zoomIn(zoomDelta);
    if (m_keys.zoomingOut) camera.zoomOut(zoomDelta);

    for (Blob& blob : blobs) {
        blob.update(deltaTime);
        blob.checkBoundaryCollision(width(), height());
    }

    // ПОТОМ обрабатываем столкновения
    handleCollisions();

    update(); // Запрашиваем перерисовку
}




QPointF gameWidget::screenToWorld(const QPointF& screenPos) {
    int w = width();
    int h = height();

    float ndcX = (2.0f * screenPos.x() / w) - 1.0f;
    float ndcY = 1.0f - (2.0f * screenPos.y() / h);

    QVector4D ndcPos(ndcX, ndcY, 0.0f, 1.0f);
    QMatrix4x4 viewMatrix = camera.getViewMatrix();
    QMatrix4x4 viewProjection = projection * viewMatrix;
    bool invertible;
    QMatrix4x4 inverseViewProjection = viewProjection.inverted(&invertible);

    if (!invertible) {
        return QPointF(0, 0);
    }

    QVector4D worldPos = inverseViewProjection * ndcPos;

    if (worldPos.w() != 0.0f) {
        worldPos /= worldPos.w();
    }

    return QPointF(worldPos.x(), worldPos.y());
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





void gameWidget::wheelEvent(QWheelEvent* event) {
    int delta = event->angleDelta().y();

    if (delta > 0) {
        camera.zoomIn(1.7f);
    } else {
        camera.zoomOut(1.7f);
    }

    update();
}




void gameWidget::keyPressEvent(QKeyEvent* event) {
    switch(event->key()) {
    case Qt::Key_W: m_keys.w = true; break;
    case Qt::Key_A: m_keys.a = true; break;
    case Qt::Key_S: m_keys.s = true; break;
    case Qt::Key_D: m_keys.d = true; break;
    case Qt::Key_Up: m_keys.up = true; break;
    case Qt::Key_Down: m_keys.down = true; break;
    case Qt::Key_Left: m_keys.left = true; break;
    case Qt::Key_Right: m_keys.right = true; break;
    case Qt::Key_Equal: m_keys.zoomingIn = true; break;
    case Qt::Key_Plus: m_keys.zoomingIn = true; break;
    case Qt::Key_Minus: m_keys.zoomingOut = true; break;
    }

    update();
}

void gameWidget::keyReleaseEvent(QKeyEvent* event) {
    switch(event->key()) {
    case Qt::Key_W: m_keys.w = false; break;
    case Qt::Key_A: m_keys.a = false; break;
    case Qt::Key_S: m_keys.s = false; break;
    case Qt::Key_D: m_keys.d = false; break;
    case Qt::Key_Up: m_keys.up = false; break;
    case Qt::Key_Down: m_keys.down = false; break;
    case Qt::Key_Left: m_keys.left = false; break;
    case Qt::Key_Right: m_keys.right = false; break;
    case Qt::Key_Equal: m_keys.zoomingIn = false; break;
    case Qt::Key_Plus: m_keys.zoomingIn = false; break;
    case Qt::Key_Minus: m_keys.zoomingOut = false; break;
    }
}


void gameWidget::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton){
        startPoint = event->position();
        currentPoint = startPoint;
        isDragging = true;

        update();
    }
}

void gameWidget::mouseMoveEvent(QMouseEvent *event){
    if(isDragging){
        currentPoint = event->position();
        update();
    }
}

void gameWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && isDragging) {
        // Получаем позицию игрока в мировых координатах
        float playerX = blobs[playerIndex].getX();
        float playerY = blobs[playerIndex].getY();

        // Конвертируем начальную и конечную точки в мировые координаты
        QPointF worldStart = screenToWorld(startPoint);
        QPointF worldEnd = screenToWorld(event->position());

        // Вычисляем вектор от начальной точки к конечной (направление удара)
        QPointF direction = worldEnd - worldStart;

        // Вычисляем длину вектора
        float distance = sqrt(direction.x() * direction.x() +
                              direction.y() * direction.y());

        // Нормализуем силу (0.0 - 1.0)
        float maxLength = 300.0f;
        float power = qMin(distance / maxLength, 1.0f);

        qDebug() << "Mouse release:"
                 << "start world =" << worldStart
                 << "end world =" << worldEnd
                 << "direction =" << direction
                 << "distance =" << distance
                 << "power =" << power;

        // Применяем удар если есть значимое направление
        if (distance > 10.0f) {
            blobs[playerIndex].applyHit(direction, power);
        }

        isDragging = false;
        update();
    }
}
