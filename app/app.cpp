
#include "app.h"
#include "loader.h"


static MeshContext meshContext;

namespace App
{
    MeshContext& getMeshContext()
    {
        return meshContext;
    }
}

void ModelMesh::swallow()
{
    QVector<float>* target = meshContext.triangleBuffer.registerForFrame(this);
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


void ModelMesh::swallowUioverlay(Core::VertexBufferDraft& targetDraft)
{
    QVector<float>* target = targetDraft.registerForFrame(this);
    if (target != nullptr)
    {
        assert(target != nullptr);
        VertexIterator vi(*this, &uioverlayFaces, *target, Core::VertexIterator::ITERATE_TRIANGLES_TO_LINES, Core::VertexIterator::ACTION_PUSH_POINT);
        while (vi.pumpByFace()) {};
    }
}

ModelMesh::ModelMesh()
{
    // load primary source data
    Utils::Loader loader;
    loader.loadStl("box.stl", *this);

    // populate normals<float> array
    VertexIterator vi(*this, normalData, Core::VertexIterator::ITERATE_PER_TRIANGLE, Core::VertexIterator::ACTION_PUSH_NORMAL);
    while (vi.pumpByFaceOnly()) {};

}

BasegridMesh::BasegridMesh(int squareCount, float side): squareCount(squareCount), side(side)
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

void BasegridMesh::swallow(Core::VertexBufferDraft& targetDraft)
{
    QVector<float>* target = targetDraft.registerForFrame(this);
    if (target != nullptr)
    {
        VertexIterator vi(*this, *target, Core::VertexIterator::ITERATE_POINTS, Core::VertexIterator::ACTION_PUSH_POINT);
        while (vi.pumpByPoint()) {};
    }

}

