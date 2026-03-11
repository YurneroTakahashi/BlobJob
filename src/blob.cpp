#include "include/blob.h"

Blob::Blob(double x1, double y1) {
    initializeOpenGLFunctions();
    position.setX(x1);
    position.setY(y1);

    Point curPoint;
    curPoint.pos = QVector2D(x1, y1);
    points.push_back(curPoint);

    const int segments = 32;
    mass = radius * radius * M_PI / 2500;
    for(int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = cos(angle) * radius + x1;
        float y = sin(angle) * radius + y1;

        curPoint.pos = QVector2D(x, y);
        points.push_back(curPoint);

    }
}




std::vector<float> Blob::getVertexData(){
    std::vector<float> data;
    for (auto& p : points){
        data.push_back(p.pos.x());
        data.push_back(p.pos.y());
    }
    return data;
}

void Blob::applyHit(QPointF direction, float power){
    QVector2D hitForce(direction);
    hitForce.normalize();
    hitForce*=power*200;
    velocity += hitForce/mass;
    qDebug() << "HIT APPLIED: " << power << Qt::endl;
}

void Blob::update(float dt) {
    position += velocity * dt;

    velocity *= 0.995f;

    updatePointsPosition();
}

void Blob::updatePointsPosition() {
    points.clear();

    Point curPoint;
    curPoint.pos = position;
    points.push_back(curPoint);

    const int segments = 32;
    for(int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = cos(angle) * radius + position.x();
        float y = sin(angle) * radius + position.y();

        curPoint.pos = QVector2D(x, y);
        points.push_back(curPoint);
    }
}


bool Blob::checkCollision(Blob& other) {
    QVector2D delta = other.position - position;
    float distance = delta.length();
    float minDistance = radius;

    return distance < minDistance;
}

void Blob::resolveCollision(Blob& other) {
    QVector2D delta = other.position - position;
    float distance = delta.length();
    float minDistance = radius;

    // Добавим небольшой буфер, чтобы избежать множественных столкновений
    float epsilon = 0.1f;

    if (distance < minDistance - epsilon && distance > 0) {
        QVector2D normal = delta / distance;

        // Разделяем шары
        float overlap = minDistance - distance;
        QVector2D correction = normal * (overlap * 0.5f);

        position -= correction;
        other.position += correction;

        // Относительная скорость
        QVector2D relVelocity = other.velocity - velocity;
        float velAlongNormal = QVector2D::dotProduct(relVelocity, normal);

        // Если шары уже удаляются, не обрабатываем
        if (velAlongNormal > 0) return;

        // Коэффициент восстановления (меньше = менее упругий удар)
        float restitution = 0.5f; // Уменьшил с 0.8 до 0.5

        // Импульс
        float impulseScalar = -(1 + restitution) * velAlongNormal;
        impulseScalar /= (1.0f / mass + 1.0f / other.mass);

        QVector2D impulse = normal * impulseScalar;

        // Применяем импульс
        velocity -= impulse / mass;
        other.velocity += impulse / other.mass;

        // Небольшое трение при столкновении
        velocity *= 0.95f;
        other.velocity *= 0.95f;

        updatePointsPosition();
        other.updatePointsPosition();

        qDebug() << "COLLISION: distance=" << distance << "minDistance=" << minDistance;
    }
}

void Blob::checkBoundaryCollision(float worldWidth, float worldHeight) {
    float halfWidth = worldWidth / 2;
    float halfHeight = worldHeight / 2;
    bool collided = false;

    if (position.x() - radius < -halfWidth) {
        position.setX(-halfWidth + radius);
        velocity.setX(-velocity.x() * 0.8);
        collided = true;
    }
    else if (position.x() + radius > halfWidth) {
        position.setX(halfWidth - radius);
        velocity.setX(-velocity.x() * 0.8);
        collided = true;
    }

    if (position.y() - radius < -halfHeight) {
        position.setY(-halfHeight + radius);
        velocity.setY(-velocity.y() * 0.8);
        collided = true;
    }
    else if (position.y() + radius > halfHeight) {
        position.setY(halfHeight - radius);
        velocity.setY(-velocity.y() * 0.8);
        collided = true;
    }

    if (collided) {
        updatePointsPosition();
    }
}
