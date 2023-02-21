#ifndef RENDERSTATE_H
#define RENDERSTATE_H

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>




class RenderState
{
    const char* vshaderSourceCode;
    const char* fshaderSourceCode;


public:
    QOpenGLShaderProgram* program = nullptr; // public for easier access
    QOpenGLVertexArrayObject vao; // same here

    struct Attribute
    {
        const char* name;
        QOpenGLBuffer& vbo;
    };

    QVector<Attribute> attributes;

    void setVShader(const char* vshader);
    void setFShader(const char* fshader);
    void setupProgram();
    void addAttribute(const char* name, QOpenGLBuffer& vbo);
    void setupVao();
    void cleanup();

};

#endif // RENDERSTATE_H
