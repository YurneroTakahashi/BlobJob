// shaderprogram.h
#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QOpenGLFunctions>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QMatrix4x4>
class ShaderProgram : protected QOpenGLFunctions
{
public:
    ShaderProgram();
    ~ShaderProgram();

    bool compile(const char* vertexPath, const char* fragmentPath);

    void use();
    void setBool(const std::string& name, bool value);
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setMatrix4(const std::string& name, QMatrix4x4 value);
    void setVec4(const std::string& name, QVector4D value);

    unsigned int getID() const { return ID; }

private:
    unsigned int ID;
    bool m_initialized = false;
};

#endif // SHADERPROGRAM_H
