/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>

#include <QDebug>

Mesh::Mesh()
{
    //data.resize(3 * 6); // 8 point, 3 floats each

    addVertex(0,0,0);
    addVertex(0.3,0,0);
    addVertex(0.3,0.3,0);

    addVertex(0.3,0.3,0);
    addVertex(0,0.3,0);
    addVertex(0,0,0);

    appendVertex2(texData, 0,0);
    appendVertex2(texData, 1,0);
    appendVertex2(texData, 1, 1);

    //addVertex(0,0,0);
    //addVertex(1,1,0);
    //addVertex(0,1,1);

    //addVertex(0,1,0);
    //addVertex(0,0,1);
    //addVertex(1,0,1);
    //addVertex(1,1,1);
    //addVertex(0,1,1);

    // for three vertices, we put the same "color" i.e. faceid three times
    QVector3D v = hideIntInVector3D(0xff);
    faceidData.append(v.x());
    faceidData.append(v.y());
    faceidData.append(v.z());

    faceidData.append(v.x());
    faceidData.append(v.y());
    faceidData.append(v.z());

    faceidData.append(v.x());
    faceidData.append(v.y());
    faceidData.append(v.z());

    v = hideIntInVector3D(0xff00);
    faceidData.append(v.x());
    faceidData.append(v.y());
    faceidData.append(v.z());

    faceidData.append(v.x());
    faceidData.append(v.y());
    faceidData.append(v.z());

    faceidData.append(v.x());
    faceidData.append(v.y());
    faceidData.append(v.z());

}


QVector3D Mesh::hideIntInVector3D(unsigned int i)
{
    QVector3D v;
    unsigned int MAX_HIDDEN = 16777216; // i.e. 2^24 or 3*8bit precision offered by RGB colors up to 255 for each component

    if (i >= MAX_HIDDEN)
    {
        qDebug() << "Error. Cannot hide such a big int in a Vector3D";
        return v;
    }
    unsigned char i_part1 = i & 0xff;
    unsigned char i_part2 = (i >> 8) & 0xff;
    unsigned char i_part3 = (i >> 16) & 0xff;

    v.setX( ((float)i_part1)/255 );
    v.setY( ((float)i_part2)/255 );
    v.setZ( ((float)i_part3)/255 );

    return v;
}

unsigned int Mesh::unhideIntFromVector3D(QVector3D& v)
{
    unsigned char x = v.x() * 255.0;
    unsigned char y = v.y() * 255.0;
    unsigned char z = v.z() * 255.0;

    unsigned int i = (unsigned int)z << 16 | (unsigned int)y << 8 | (unsigned int)x;

    return i;
}



bool GLWidget::m_transparent = false;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.
    if (m_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);
        setFormat(fmt);
    }
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        emit xRotationChanged(angle);
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void GLWidget::setXTranslation(int length)
{
    xTrans = length;
    update();
}

void GLWidget::setYTranslation(int length)
{
    yTrans = length;
    update();
}

void GLWidget::setZTranslation(int length)
{
    zTrans = length;
    update();
}


void GLWidget::cleanup()
{
    if (m_program == nullptr)
        return;
    makeCurrent();
    m_logoVbo.destroy();
    delete m_program;
    m_program = nullptr;
    if (tex1)
        delete tex1;
    doneCurrent();
}


static const char *vertexShaderSource =
    "attribute vec4 vertex;\n"
    "attribute vec3 normal;\n"
    "attribute vec2 tex;\n"
    "varying vec3 vert;\n"
    "varying vec3 vertNormal;\n"
    "varying vec2 texCoord;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "uniform mat3 normalMatrix;\n"
    "void main() {\n"
    "   vert = vertex.xyz;\n"
    "   vertNormal = normalize(normalMatrix * normal);\n"
    "   texCoord = tex;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying highp vec3 vert;\n"
    "varying vec3 vertNormal;\n"
    "varying vec2 texCoord;\n"
    "uniform sampler2D texture;\n"
    "uniform highp vec4 rgbColor;\n"
    "void main() {\n"
    "highp vec3 lightDir = vec3(0.0, 0.0, -1.0);\n"
    "highp float intensity =  dot(-lightDir, vertNormal);\n"
    //"   gl_FragColor = rgbColor*intensity;\n"
    "   gl_FragColor = texture2D(texture, texCoord);\n"
    "}\n";

static const char *vshaderIdProjection =
    "attribute vec4 vertex;\n"
    "attribute vec3 faceid;\n"
    "varying vec3 vfaceid;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "void main() {\n"
    "   vfaceid = faceid;\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fshaderIdProjection =
    "varying vec3 vfaceid;\n"
    "void main() {\n"
    "gl_FragColor = vec4(vfaceid, 1.0);\n"
    //"gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

void GLWidget::setupIdProjectionProgram()
{
    idProjection_vao.create();
    idProjection_vao.bind();

    idProjectionProgram = new QOpenGLShaderProgram;
    idProjectionProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vshaderIdProjection);
    idProjectionProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fshaderIdProjection);
    idProjectionProgram->bindAttributeLocation("vertex", 0);
    idProjectionProgram->bindAttributeLocation("faceid", 1);
    idProjectionProgram->link();

    idProjectionProgram->bind();
    idProjection_projMatrixLoc = idProjectionProgram->uniformLocation("projMatrix");
    idProjection_mvMatrixLoc = idProjectionProgram->uniformLocation("mvMatrix");

    QOpenGLVertexArrayObject::Binder vao_binder(&idProjection_vao);
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);

    // Setup our vertex buffer object.
    m_logoVbo.bind();
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    m_logoVbo.release();


    //texVbo.create();
    //texVbo.bind();
    //texVbo.allocate(mesh_cube.texData.constData(), mesh_cube.data.size()* sizeof(GLfloat));

    faceidVbo.create();
    faceidVbo.bind();
    faceidVbo.allocate(mesh_cube.faceidData.constData(), mesh_cube.faceidData.size() * sizeof(GLfloat));
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), nullptr);
    faceidVbo.release();


    idProjectionProgram->release();
    idProjection_vao.release();
}

void GLWidget::initializeGL()
{
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);
    m_program->bindAttributeLocation("tex", 2);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    rgbColorLoc = m_program->uniformLocation("rgbColor");

    // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
    // implementations this is optional and support may not be present
    // at all. Nonetheless the below code works in all cases and makes
    // sure there is a VAO when one is needed.
    m_vao.create();
    m_vao.bind();
    //QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    mesh_cube.swallow();

    // Setup our vertex buffer object.
    m_logoVbo.create();
    m_logoVbo.bind();
    m_logoVbo.allocate(mesh_cube.data.constData(), mesh_cube.data.size() * sizeof(GLfloat));

    texVbo.create();
    texVbo.bind();
    texVbo.allocate(mesh_cube.texData.constData(), mesh_cube.data.size()* sizeof(GLfloat));

    // Store the vertex attribute bindings for the program.
    m_logoVbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                             nullptr);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    m_logoVbo.release();

    texVbo.bind();
    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 2 * sizeof(GLfloat), nullptr);
    texVbo.release();

    //QOpenGLTexture

    // Prepare texture
    tex1 = new QOpenGLTexture(QImage("/home/nando/Projects/gui-opengl/build/tex1.png").mirrored());
    tex1->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    tex1->setMagnificationFilter(QOpenGLTexture::Linear);

    // Our camera never changes in this example.
    m_camera.setToIdentity();
    //m_camera.translate(0, 0, -1);
    //m_camera.rotate(180, QVector3D(0,1,0));
    //m_camera.translate(0, 0, 10);

    m_program->release();
    m_vao.release();

    setupIdProjectionProgram();
}


void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_vao.bind();
    m_program->bind();

    m_camera.setToIdentity();
    m_world.setToIdentity();

    m_world.translate(0,0,-5.0);

    //m_world.translate(xTrans*0.001, yTrans*0.001, zTrans*0.001);

    QMatrix4x4 worldMatrix;

    QMatrix4x4 projMatrix;
    projMatrix.frustum(-10, 10, -10, 10, 1, 100);

    QVector4D p(10.0, 0, -2, 1);

    QVector4D p2 = projMatrix * worldMatrix * p;

    m_world.rotate(m_xRot / 16.0f, 1, 0, 0);
    m_world.rotate(m_yRot / 16.0f, 0, 1, 0);
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);

    QMatrix4x4 mview = m_camera * m_world;
    QRect viewport(0,0,400,400);

    QVector3D near(m_lastPos.x(), 400-m_lastPos.y(), 0.);
    QVector3D far(m_lastPos.x(), 400-m_lastPos.y(), 1.);
    QVector3D origin = near.unproject(mview, m_proj, viewport);
    QVector3D dir = far.unproject(mview, m_proj, viewport) - origin;

    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_mvMatrixLoc, mview);
    //m_program->setUniformValue(rgbColorLoc, QVector3D(0.0,0.0,1.0));
    m_program->setUniformValue(rgbColorLoc, mesh_cube.meshColor);

    //QMatrix3x3 normalMatrix = m_world.normalMatrix();
    QMatrix3x3 normalMatrix =  m_world.toGenericMatrix<3,3>();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

    // Render with texture
    tex1->bind();

    glDrawArrays(GL_TRIANGLES, 0, mesh_cube.vertexCount);

    m_program->release();
    m_vao.release();

    // render triangle ids to image
    idProjection_vao.bind();
    idProjectionProgram->bind();
    idProjectionProgram->setUniformValue(idProjection_mvMatrixLoc, mview);
    idProjectionProgram->setUniformValue(idProjection_projMatrixLoc, m_proj);
    fbo->bind();
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, mesh_cube.vertexCount);
    snapshotImage = fbo->toImage();
    fbo->release();
    idProjection_vao.release();

}

void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f); // near/far only care about clipping

    if (fbo)
    {
        fbo->release();
        delete fbo;
    }
    fbo = new QOpenGLFramebufferObject(w,h);

}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
    qInfo() << "mouse pressed at: " << m_lastPos;

    snapshotImage.save("snapshot.png");
    //makeCurrent();
    //fbo->bind();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();


    if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }
    m_lastPos = event->pos();
    qInfo() << "mouse moved";
}
