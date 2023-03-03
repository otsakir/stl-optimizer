#include "renderstate.h"



void RenderState::setVShader(const char* vshader)
{
    vshaderSourceCode = vshader;
}

void RenderState::setFShader(const char* fshader)
{
    fshaderSourceCode = fshader;
}

void RenderState::setupProgram()
{
    program = new QOpenGLShaderProgram;
    program->addShaderFromSourceCode(QOpenGLShader::Vertex, vshaderSourceCode);
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, fshaderSourceCode);
    for (int attr_i=0; attr_i < attributes.size(); attr_i++)
    {
        program->bindAttributeLocation(attributes[attr_i].name, attr_i);
    }

    program->link();
}

void RenderState::addAttribute(const char* name, QOpenGLBuffer& vbo)
{
    attributes.append({name, vbo});
}

void RenderState::setupVao()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

    vao.create();
    vao.bind();

    for (int attr_i=0; attr_i < attributes.size(); attr_i++)
    {
        Attribute& attr = attributes[attr_i];
        QOpenGLBuffer& vbo = attr.vbo;

        vbo.bind();
        f->glEnableVertexAttribArray(attr_i);
        f->glVertexAttribPointer(attr_i, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat),nullptr); // use three floats and no stride by convention
        vbo.release();
    }
    vao.release();
}

void RenderState::cleanup()
{
    vao.destroy();

    if (program)
    {
        delete program;
        program = nullptr;
    }
}


