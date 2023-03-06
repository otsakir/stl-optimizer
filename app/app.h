#ifndef APP_H
#define APP_H


#include <QVector>
#include "mesh.h"


using Core::VertexIterator;

class ModelMesh : public Core::Mesh
{
public:
    //QVector<float> data; // point data for vertex buffers
    QVector<float> idprojectionData; // face ids to project
    QVector<Core::FaceIndex> uioverlayFaces;

    void swallow(Core::VertexBufferDraft& targetDraft)
    {
        QVector<float>* target = targetDraft.registerForFrame(this);
        if (target != nullptr)
        {
            assert(target != nullptr);
            VertexIterator vi(*this, *target, VertexIterator::ITERATE_TRIANGLES, VertexIterator::ACTION_PUSH_POINT);
            while (vi.pumpByFace()) {}; // process all
        }

        idprojectionData.clear();
        VertexIterator vi2(*this, idprojectionData, Core::VertexIterator::ITERATE_TRIANGLES, Core::VertexIterator::ACTION_PUSH_FACEID);
        while (vi2.pumpByFace()) {};
    }


    void swallowUioverlay(Core::VertexBufferDraft& targetDraft)
    {
        QVector<float>* target = targetDraft.registerForFrame(this);
        if (target != nullptr)
        {
            assert(target != nullptr);
            VertexIterator vi(*this, &uioverlayFaces, *target, Core::VertexIterator::ITERATE_TRIANGLES_TO_LINES, Core::VertexIterator::ACTION_PUSH_POINT);
            while (vi.pumpByFace()) {};
        }
    }
};

class BasegridMesh : public Core::Mesh
{
    float side;
    int squareCount; // how many squares in each dimmension
    QVector3D color;

public:

    BasegridMesh(int squareCount, float side): squareCount(squareCount), side(side)
    {
        float square_side = side/squareCount;

        for (int i = 0; i <= squareCount; i++)
        {
            // put X line
            QVector3D p1(0.0f, square_side*i, 0.0f);
            QVector3D p2(side, square_side*i, 0.0f);
            points.append(p1);
            points.append(p2);
            // put Y line
            points.append( QVector3D(square_side*i, 0.0f, 0.0f) );
            points.append( QVector3D(square_side*i, side, 0.0f) );
        }


    }

    void swallow(Core::VertexBufferDraft& targetDraft)
    {
        QVector<float>* target = targetDraft.registerForFrame(this);
        if (target != nullptr)
        {
            VertexIterator vi(*this, *target, Core::VertexIterator::ITERATE_POINTS, Core::VertexIterator::ACTION_PUSH_POINT);
            while (vi.pumpByPoint()) {};
        }

    }
};


#endif // APP_H
