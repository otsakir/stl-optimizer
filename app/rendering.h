#ifndef RENDERING_H
#define RENDERING_H

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
        const char* name; // attribute name as written in the vertex shader
        QOpenGLBuffer& vbo;
        const void* offset;
    };

    QVector<Attribute> attributes;

    void setVShader(const char* vshader);
    void setFShader(const char* fshader);
    void setupProgram();
    void addAttribute(const char* name, QOpenGLBuffer& vbo, const void* offset = 0);
    void setupVao();
    void cleanup();

};

#endif // RENDERING_H
