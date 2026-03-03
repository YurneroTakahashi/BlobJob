#include "include/shaderprogram.h"

ShaderProgram::ShaderProgram() : ID(0)
{

}

ShaderProgram::~ShaderProgram()
{
    if (m_initialized && ID) {
        glDeleteProgram(ID);
    }
}

bool ShaderProgram::compile(const char* vertexPath, const char* fragmentPath)
{
    initializeOpenGLFunctions();

    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        return false;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // Вершинный шейдер
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }

    // Фрагментный шейдер
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return false;
    }

    // Шейдерная программа
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    m_initialized = true;
    return true;
}

void ShaderProgram::use()
{
    if (m_initialized) {
        glUseProgram(ID);
    }
}

void ShaderProgram::setBool(const std::string& name, bool value)
{
    if (m_initialized) {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
}

void ShaderProgram::setInt(const std::string& name, int value)
{
    if (m_initialized) {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
}

void ShaderProgram::setFloat(const std::string& name, float value)
{
    if (m_initialized) {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
}

void ShaderProgram::setMatrix4(const std::string& name, QMatrix4x4 value){
    if(m_initialized) {
        const GLfloat* matData = value.constData();
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, matData);
    }
}

void ShaderProgram::setVec4(const std::string& name, QVector4D value){
    if(m_initialized){
        glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x(),value.y(),value.z(),value.w());
    }
}
