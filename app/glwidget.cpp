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


using Core::Mesh;


GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;

    connect(this, &GLWidget::mouseClickedAt, this, &GLWidget::onMouseClicked);
    connect(this, &GLWidget::ctrlStateChanged, this, &GLWidget::onCtrlStateChanged);

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
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

void GLWidget::updateZoomLevel(int degreesDelta)
{
    zoomLevel += degreesDelta;
    qDebug() << "updateZoomLevel: " << degreesDelta << zoomLevel;
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

/// Rebuild face set that constitutes the overlay. Does not push to buffer draft.
bool GLWidget::updateUiOverlay()
{
    MeshContext& meshContext = App::getMeshContext();

    if (!selectedFaces.empty())
    {
        //qDebug() << "selected faces: " << selectedFaces.size();
        meshModel.uioverlayFaces.clear();
        meshModel.uioverlayFaces.append(selectedFaces);

        return true;
    } else
    {
        meshModel.uioverlayFaces.clear();
    }
    /*
    if (selectedFace != -1)
    {
        meshModel.uioverlayFaces.clear();
        meshModel.uioverlayFaces.append(selectedFace);

        return true;

        //QVector<Core::FaceIndex> out_faces;
        //adjacentFaces(meshModel, meshModel.uioverlayFaces, out_faces);
        //meshModel.uioverlayFaces.clear();
        //meshModel.uioverlayFaces.append(out_faces);

        //meshModel.swallowUioverlay(meshContext.wireframeBuffer); // populate meshModel.uioverlayData

//        makeCurrent();
//        uiOverlayVbo.bind();
//        uiOverlayVbo.allocate(data.constData(), data.size()* sizeof(GLfloat));
//        uiOverlayVbo.release();

    }
    */
    return false;

}

void GLWidget::initializeGL()
{
    MeshContext& meshContext = App::getMeshContext();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);
    meshContext.triangleBuffer.clear();
    meshContext.wireframeBuffer.clear();
    meshContext.normalBuffer.clear();

    // generate secondary source data
    meshModel.chew(Core::Mesh::CHEW_GRAPH );
    meshModel.swallow();

    initializeOpenGLFunctions();


    // buffer with model vertices
    vboPoints.create();
    vboPoints.bind();
    vboPoints.allocate(meshContext.triangleBuffer.getData().constData(), meshContext.triangleBuffer.getData().size() * sizeof(GLfloat));
    vboPoints.release();
    // and another one with normals
    vboNormals.create();
    vboNormals.bind();
    vboNormals.allocate(meshContext.normalBuffer.getData().constData(), meshContext.normalBuffer.getData().size() * sizeof(GLfloat));
    vboNormals.release();


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
        "attribute vec3 normal;\n"
        "varying vec3 vert;\n"
        "varying vec3 vertNormal;\n"
        "uniform mat4 mvpMatrix;\n"
        "uniform mat3 normalMatrix;\n"
        "void main() {\n"
        "   vert = vertex.xyz;\n"
        "   vertNormal = normalMatrix * normal;\n"
        "   gl_Position = mvpMatrix * vertex;\n"
        "}\n");
    renderState_model.setFShader(
        "varying highp vec3 vert;\n"
        "varying highp vec3 vertNormal;\n"
        "void main() {\n"
        "   highp vec3 lightDir = vec3(0.0, 0.0, -1.0);\n"
        "   highp float intensity =  dot(-lightDir, vertNormal);\n"
        "   gl_FragColor = vec4(0.5, 0.5, 0.5, 1)*intensity;\n"
        "}\n");
    renderState_model.addAttribute("vertex",vboPoints);
    renderState_model.addAttribute("normal",vboNormals);
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
        "uniform vec4 color;\n"
        "void main(){\n"
        "   gl_FragColor = color;\n"
        "}\n"
    );
    renderState_uiOverlay.addAttribute("vertex",uiOverlayVbo);
    renderState_uiOverlay.setupProgram();
    renderState_uiOverlay.setupVao();


}


void GLWidget::paintGL()
{
    MeshContext& meshContext = App::getMeshContext();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1);


    // view transformation (camera)
    QMatrix4x4 vTrans;
    // vTrans.setToIdentity(); // implied
    vTrans.translate(0,0,10.0f + (float)zoomLevel * Config::wheelDegreesToZUnits);
    vTrans = vTrans.inverted();

    // world transformation (transform the world as whole)
    QMatrix4x4 wTrans;
    // wTrans.setToIndentity();
    wTrans.translate(0,0,-5.0);
    wTrans.rotate(m_xRot / 16.0f, 1, 0, 0);
    wTrans.rotate(m_yRot / 16.0f, 0, 1, 0);
    wTrans.rotate(m_zRot / 16.0f, 0, 0, 1);

    // camera & world
    QMatrix4x4 vwTrans = vTrans * wTrans;
    QMatrix4x4 pvwTrans = pTrans * vwTrans; // used all over the place


    //matMvpTransformation = pTrans * vwTrans;
    QMatrix3x3 normalTrans3 = vwTrans.toGenericMatrix<3,3>();

    // render triangle ids to image
    renderState_idProjection.vao.bind();
    renderState_idProjection.program->bind();
    renderState_idProjection.program->setUniformValue(0, pvwTrans);
    fbo->bind();
    glDisable(GL_BLEND);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, meshModel.idprojectionData.size()/3);
    snapshotImage = fbo->toImage();
    fbo->release();
    renderState_idProjection.vao.release();

    // Render model
    glEnable(GL_BLEND);
    glClearColor(0.2, 0.2, 0.2, 1.0);
    renderState_model.vao.bind();
    renderState_model.program->bind();
    int loc = renderState_model.program->uniformLocation("mvpMatrix");
    renderState_model.program->setUniformValue(loc, pvwTrans);
    loc = renderState_model.program->uniformLocation("normalMatrix");
    renderState_model.program->setUniformValue(loc, normalTrans3);
    glDrawArrays(GL_TRIANGLES, 0, meshContext.triangleBuffer.getData().size()/3); // 3 floats per point
    renderState_model.program->release();
    renderState_model.vao.release();

    // process wireframe data
    if (updateUiOverlay()) // update set of faces according to UI state
    {
        meshModel.swallowUioverlay(meshContext.wireframeBuffer); // populate meshModel.uioverlayData
    }
    basegridMesh.swallow(meshContext.wireframeBuffer);

    // push wireframe data to the GPU
    const QVector<float>* wireframeData = &meshContext.wireframeBuffer.getData();
    uiOverlayVbo.bind();
    uiOverlayVbo.allocate(wireframeData->constData(), wireframeData->size()* sizeof(GLfloat));
    uiOverlayVbo.release();

    // render basegrid overlay
    renderState_uiOverlay.vao.bind();
    renderState_uiOverlay.program->bind();
    loc = renderState_uiOverlay.program->uniformLocation("mvpMatrix");
    renderState_uiOverlay.program->setUniformValue(loc, pvwTrans * basegridMesh.modelTrans);
    QColor gridColor(Qt::white); gridColor.setAlpha(40);
    renderState_uiOverlay.program->setUniformValue(renderState_uiOverlay.program->uniformLocation("color"), gridColor);

    Core::VertexBufferDraft::RegisteredInfo* meshinfo = meshContext.wireframeBuffer.getMeshInfo(&basegridMesh);
    if (meshinfo)
    {
        glDrawArrays(GL_LINES, meshinfo->offset/3, meshinfo->size/3);
    }

    glLineWidth(3);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderState_uiOverlay.program->setUniformValue(renderState_uiOverlay.program->uniformLocation("mvpMatrix"), pvwTrans);
    QColor selectionColor(Qt::green); selectionColor.setAlpha(200);
    renderState_uiOverlay.program->setUniformValue(renderState_uiOverlay.program->uniformLocation("color"), selectionColor);
    meshinfo = meshContext.wireframeBuffer.getMeshInfo(&meshModel);
    if (meshinfo)
    {
        glDrawArrays(GL_LINES, meshinfo->offset/3, meshinfo->size/3);
    }

    renderState_uiOverlay.program->release();
    renderState_uiOverlay.vao.release();

    meshContext.wireframeBuffer.clear();
}

void GLWidget::resizeGL(int w, int h)
{
    pTrans.setToIdentity();
    pTrans.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f); // near/far only care about clipping

    if (fbo)
    {
        fbo->release();
        delete fbo;
    }
    fbo = new QOpenGLFramebufferObject(w,h);

}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    mousePressedPos = event->pos();
    mouseLastPos = event->pos();
    qInfo() << "mouse pressed at: " << mouseLastPos;
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->pos() == mousePressedPos)
    {
        qDebug() << "mouse clicked at " << mousePressedPos;
        emit mouseClickedAt(mousePressedPos.x(), mousePressedPos.y());
    }
}

void GLWidget::onMouseClicked(int x, int y)
{
    // check for face picking
    QRgb rgb = snapshotImage.pixel(QPoint(x,y));
    int red = qRed(rgb);
    int green = qGreen(rgb);
    int blue = qBlue(rgb);

    QVector3D faceidVector(red,green,blue);
    unsigned int faceid = Core::unhideIntFromVector3D(faceidVector);

    if (faceid == 0)
    {
        this->selectedFace = -1; // nothing selected
        this->selectedFaces.clear();
        updateUiOverlay();
    } else
    {
        this->selectedFace = faceid;
        if (ctrlDown)
        {
            if (!selectedFaces.contains(faceid))
                selectedFaces.append(faceid);
            else
                selectedFaces.removeOne(faceid);
        } else
        {
            selectedFaces.clear();
            selectedFaces.append(selectedFace);
        }
        updateUiOverlay();
        update();
    }

    qDebug() << "face at clicked position: " << red << green << blue << ". Faceid: " << faceid;
}

void GLWidget::onCtrlStateChanged(bool down)
{
    if (down)
        setCursor(Qt::IBeamCursor);
    else
        unsetCursor();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - mouseLastPos.x();
    int dy = event->y() - mouseLastPos.y();

    Qt::KeyboardModifiers m = event->modifiers();
    if (m && m.testFlag(Qt::KeyboardModifier::ControlModifier))
    {
        if (!ctrlDown)
        {
            ctrlDown = true;
            emit ctrlStateChanged(true);
        }
    } else
    {
        if (ctrlDown)
        {
            ctrlDown = false;
            emit ctrlStateChanged(false);
        }
    }

    if (m && m.testFlag(Qt::ShiftModifier))
    {
        qDebug() << "x-moved while pressing shift: " << dx;
    }

    //if (!m)
    //{
        if (event->buttons() & Qt::LeftButton) {
            setXRotation(m_xRot + 8 * dy);

            setYRotation(m_yRot + 8 * dx);
        } else if (event->buttons() & Qt::RightButton) {
            setXRotation(m_xRot + 8 * dy);
            setZRotation(m_zRot + 8 * dx);
        }
    //}
    mouseLastPos = event->pos();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    emit zoomChangedBy(numDegrees.y());
    event->ignore();
}

void GLWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control)
    {
        if (!ctrlDown)
        {
            ctrlDown = true;
            emit ctrlStateChanged(true);
        }
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control)
    {
        if (ctrlDown)
        {
            ctrlDown = false;
            emit ctrlStateChanged(false);
        }
    }
}
