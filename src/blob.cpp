#include "../include/blob.h"
Blob::Blob(double x, double y) {
    initializeOpenGLFunctions();
    position.setX(x);
    position.setY(y);
}
