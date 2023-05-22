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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFramebufferObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix4x4>

#include "rendering.h"
#include "app.h"


QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

#include <qopengl.h>




class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

    bool cleanedUp = false;
public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void setXTranslation(int length);
    void setYTranslation(int length);
    void setZTranslation(int length);
    void updateZoomLevel(int degreesDelta);
    void onMouseClicked(int x, int y);
    void onCtrlStateChanged(bool down);
    void rebaseOnFace();
    void onNewStlFilename(QString filename);
    void resetCamera();

    void cleanup();

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
    void zoomChangedBy(int degreesDelta);
    void mouseClickedAt(int x, int y);
    void ctrlStateChanged(bool down);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void processModel();
    bool updateUiOverlay();


private:

    bool m_core;
    int m_xRot = 0;
    int m_yRot = 0;
    int m_zRot = 0;
    int xTrans = 0;
    int yTrans = 0;
    int zTrans = 0;
    int zoomLevel = 0; // expressed as "mouse wheel rotation degrees"
    bool ctrlDown = false;

    Core::Camera* camera = 0;

    QPoint mouseLastPos;
    QPoint mousePressedPos;
    ModelMesh* modelMesh = 0;
    BasegridMesh* basegridMesh = 0;

    RenderState renderState_model;
    RenderState renderState_idProjection;
    RenderState renderState_uiOverlay;

    QOpenGLBuffer vboPoints;
    QOpenGLBuffer vboNormals;
    QOpenGLBuffer vboFaceid;

    QOpenGLFramebufferObject* fbo = 0;
    QImage snapshotImage;

    int selectedFace = -1; // id of the clicked face. -1 if none is selected
    QVector<Core::FaceIndex> selectedFaces;

    // ui overlay rendering
    QOpenGLShaderProgram* uiOverlayProgram = 0;
    QOpenGLVertexArrayObject uiOverlay_vao;
    QOpenGLBuffer uiOverlayVbo;

    // transformations
    QMatrix4x4 pTrans;

    float boundingRadius = 10;
};


#endif
