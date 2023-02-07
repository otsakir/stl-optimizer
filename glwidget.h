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
#include <QOpenGLBuffer>
#include <QMatrix4x4>
//#include "logo.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

#include <qopengl.h>

class Mesh
{
    QVector<QVector3D> points;

    void appendVector3D(QVector3D& vec)
    {
        data.append(vec.x());
        data.append(vec.y());
        data.append(vec.z());
        vertexCount ++;
    }

public:
    QVector<GLfloat> data;
    int vertexCount = 0;
    QColor meshColor = QColor(0,255,0); // will be set as a uniform value

    void addVertex(GLfloat x, GLfloat y, GLfloat z)
    {
        //data.append(x);
        //data.append(y);
        //data.append(z);

        //vertexCount ++;

        points.append(QVector3D(x,y,z));
    }

    // prepare the raw data array of vertices ready to be fed to the GPU
    void swallow()
    {
        data.clear();
        vertexCount = 0;

        // process points in groups of 3 assuming they're given as triangles
        int face_count = points.size() / 3;
        for (int i=0; i < face_count; i++)
        {
            QVector3D& p1 = points[i*3];
            QVector3D& p2 = points[i*3+1];
            QVector3D& p3 = points[i*3+2];

            // first calculate the normal for this triangle
            QVector3D n = QVector3D::normal( p1, p2, p3);
            appendVector3D(p1);
            appendVector3D(n);
            appendVector3D(p2);
            appendVector3D(n);
            appendVector3D(p3);
            appendVector3D(n);
        }

    }

    Mesh()
    {
        //data.resize(3 * 6); // 8 point, 3 floats each

        addVertex(0,0,0);
        addVertex(1,0,0);
        addVertex(1,1,0);
        addVertex(0,0,0);
        addVertex(1,1,0);
        addVertex(0,1,1);

        //addVertex(0,1,0);
        //addVertex(0,0,1);
        //addVertex(1,0,1);
        //addVertex(1,1,1);
        //addVertex(0,1,1);


    }
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    static bool isTransparent() { return m_transparent; }
    static void setTransparent(bool t) { m_transparent = t; }

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void setXTranslation(int length);
    void setYTranslation(int length);
    void setZTranslation(int length);

    void cleanup();

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void setupVertexAttribs();

    bool m_core;
    int m_xRot = 0;
    int m_yRot = 0;
    int m_zRot = 0;
    int xTrans = 0;
    int yTrans = 0;
    int zTrans = 0;
    QPoint m_lastPos;
    //Logo m_logo;
    Mesh mesh_cube;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_logoVbo;
    QOpenGLShaderProgram *m_program = nullptr;
    int m_projMatrixLoc = 0;
    int m_mvMatrixLoc = 0;
    int rgbColorLoc = 0;
    int m_normalMatrixLoc = 0;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;
    static bool m_transparent;
};

#endif
