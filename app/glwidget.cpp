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

#include "loader.h"

using Core::Mesh;


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
    if (! cleanedUp)
    {
        cleanedUp = true;
        makeCurrent();

        vboPoints.destroy();
        vboFaceid.destroy();
        renderState_idProjection.cleanup();
        renderState_model.cleanup();

        doneCurrent();
    }
}

void adjacentFaces(Core::Mesh& mesh, QVector<Core::FaceIndex>& inner_faces, QVector<Core::FaceIndex>& out_faces )
{
    for (int inner_i = 0; inner_i < inner_faces.size(); inner_i++)
    {
        QVector<Core::FaceIndex>& adjacent_faces = mesh.faceFaces[inner_faces[inner_i]];
        for (int adj_i=0; adj_i<adjacent_faces.size(); adj_i++)
        {
            if (!out_faces.contains(adjacent_faces[adj_i]))
            {
                out_faces.append(adjacent_faces[adj_i]);
            }
        }
    }
}

void GLWidget::updateUiOverlayMesh()
{
    if (selectedFace != -1)
    {
        meshModel.uioverlayFaces.clear();
        meshModel.uioverlayFaces.append(selectedFace);

        //QVector<Core::FaceIndex> out_faces;
        //adjacentFaces(meshModel, meshModel.uioverlayFaces, out_faces);
        //meshModel.uioverlayFaces.clear();
        //meshModel.uioverlayFaces.append(out_faces);


        meshModel.swallowUioverlay(); // populate meshModel.uioverlayData

        makeCurrent();
        uiOverlayVbo.bind();
        uiOverlayVbo.allocate(meshModel.uioverlayData.constData(), meshModel.uioverlayData.size()* sizeof(GLfloat));
        uiOverlayVbo.release();

    }

}

void GLWidget::initializeGL()
{
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    // load primary source data
    Utils::Loader loader;
    loader.loadStl("box.stl", meshModel);
    // generate secondary source data
    meshModel.chew(Core::Mesh::CHEW_GRAPH | Core::Mesh::CHEW_FACEIDS);
    meshModel.swallow();

    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, m_transparent ? 0 : 1);

    // buffer with model vertices
    vboPoints.create();
    vboPoints.bind();
    vboPoints.allocate(meshModel.data.constData(), meshModel.data.size() * sizeof(GLfloat));
    vboPoints.release();


    // buffer with face ids
    vboFaceid.create();
    vboFaceid.bind();
    vboFaceid.allocate(meshModel.idprojectionData.constData(), meshModel.idprojectionData.size() * sizeof(GLfloat));
    vboFaceid.release();
    //

    uiOverlayVbo.create();
    uiOverlayVbo.bind();
    uiOverlayVbo.release();


    // main scene model
    renderState_model.setVShader(
        "attribute vec4 vertex;\n"
        "varying vec3 vert;\n"
        "uniform mat4 mvpMatrix;\n"
        "void main() {\n"
        "   vert = vertex.xyz;\n"
        "   gl_Position = mvpMatrix * vertex;\n"
        "}\n");
    renderState_model.setFShader(
        "varying highp vec3 vert;\n"
        "void main() {\n"
        "   gl_FragColor = vec4(1.0, 0, 0, 1);\n"
        "}\n");
    renderState_model.addAttribute("vertex",vboPoints);
    renderState_model.setupProgram();
    renderState_model.setupVao();


    // id projection
    renderState_idProjection.setVShader(
        "attribute vec4 vertex;\n"
        "attribute vec3 faceid;\n"
        "varying vec3 vfaceid;\n"
        "uniform mat4 mvpMatrix;\n"
        "void main() {\n"
        "   vfaceid = faceid;\n"
        "   gl_Position = mvpMatrix * vertex;\n"
        "}\n"
    );
    renderState_idProjection.setFShader(
        "varying vec3 vfaceid;\n"
        "void main() {\n"
        "gl_FragColor = vec4(vfaceid, 1.0);\n"
        //"gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n"
    );
    renderState_idProjection.addAttribute("vertex", vboPoints);
    renderState_idProjection.addAttribute("faceid", vboFaceid);
    renderState_idProjection.setupProgram();
    renderState_idProjection.setupVao();

    // ui overlay
    renderState_uiOverlay.setVShader(
        "attribute vec4 vertex;\n"
        "uniform mat4 mvpMatrix;\n"
        "void main() {\n"
        "  gl_Position = mvpMatrix * vertex;\n"
        "}\n"
    );
    renderState_uiOverlay.setFShader(
        "void main(){\n"
        "   gl_FragColor = vec4(0, 1.0, 0, 1.0);\n"
        "}\n"
    );
    renderState_uiOverlay.addAttribute("vertex",uiOverlayVbo);
    renderState_uiOverlay.setupProgram();
    renderState_uiOverlay.setupVao();


}


void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // camera
    matCamera.setToIdentity();
    // world
    matWorld.setToIdentity();
    matWorld.translate(0,0,-5.0);
    matWorld.rotate(m_xRot / 16.0f, 1, 0, 0);
    matWorld.rotate(m_yRot / 16.0f, 0, 1, 0);
    matWorld.rotate(m_zRot / 16.0f, 0, 0, 1);
    // camera & world
    QMatrix4x4 mview = matCamera * matWorld;

    matMvpTransformation = matProj * mview; // used all over the place


    // render triangle ids to image
    renderState_idProjection.vao.bind();
    renderState_idProjection.program->bind();
    renderState_idProjection.program->setUniformValue(0, matMvpTransformation);
    fbo->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, meshModel.idprojectionData.size()/3);
    snapshotImage = fbo->toImage();
    fbo->release();
    renderState_idProjection.vao.release();


    // Render model
    renderState_model.vao.bind();
    renderState_model.program->bind();
    renderState_model.program->setUniformValue(0, matMvpTransformation);
    glDrawArrays(GL_TRIANGLES, 0, meshModel.data.size()/3); // 3 floats per point
    renderState_model.program->release();
    renderState_model.vao.release();

    // render ui overlay
    updateUiOverlayMesh();


    glClear(GL_DEPTH_BUFFER_BIT);
    renderState_uiOverlay.vao.bind();
    renderState_uiOverlay.program->bind();
    int loc = renderState_uiOverlay.program->uniformLocation("mvpMatrix");
    renderState_uiOverlay.program->setUniformValue(loc, matMvpTransformation);
    glDrawArrays(GL_LINES, 0, meshModel.uioverlayData.size()/3);
    renderState_uiOverlay.program->release();
    renderState_uiOverlay.vao.release();

}

void GLWidget::resizeGL(int w, int h)
{
    matProj.setToIdentity();
    matProj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f); // near/far only care about clipping

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

    QRgb rgb = snapshotImage.pixel(m_lastPos);
    int red = qRed(rgb);
    int green = qGreen(rgb);
    int blue = qBlue(rgb);


    QVector3D faceidVector(red,green,blue);
    unsigned int faceid = Core::unhideIntFromVector3D(faceidVector);

    if (faceid == 0)
    {
        this->selectedFace = -1; // nothing selected
    } else
    {
        this->selectedFace = faceid;
        updateUiOverlayMesh();
        update();
    }

    qDebug() << "face at clicked position: " << red << green << blue << ". Faceid: " << faceid;

    //snapshotImage.save("snapshot.png");
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
