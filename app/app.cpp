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
    VertexIterator vi(*this, meshContext.triangleBuffer, VertexIterator::ITERATE_TRIANGLES, VertexIterator::ACTION_PUSH_POINT);
    vi.pumpAll();

    VertexIterator vi2(*this, meshContext.normalBuffer, VertexIterator::ITERATE_PER_TRIANGLE, VertexIterator::ACTION_PUSH_NORMAL);
    vi2.pumpAll();

    idprojectionData.clear();
    VertexIterator vi3(*this, &idprojectionData, Core::VertexIterator::ITERATE_TRIANGLES, Core::VertexIterator::ACTION_PUSH_FACEID);
    vi3.pumpAll();
}


void ModelMesh::swallowUioverlay(Core::VertexBufferDraft& targetDraft)
{
    VertexIterator vi(*this, &uioverlayFaces, targetDraft, Core::VertexIterator::ITERATE_TRIANGLES_TO_LINES, Core::VertexIterator::ACTION_PUSH_POINT);
    vi.pumpAll();
}


ModelMesh::ModelMesh()
{
    // load primary source data
    Utils::Loader loader;
    loader.loadStl("dino.stl", *this);

    generateMetrics();

    qDebug() << "min point: " << minPoint;
    qDebug() << "max point: " << maxPoint;
    qDebug() << "mid point: " << centerPoint;
    qDebug() << "dimensions: " << width << " " << height << " " << depth;
    qDebug() << "bounding radius: " << boundingRadius;

    modelTrans.translate(-centerPoint.x(),-minPoint.y(), -centerPoint.z());
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
    modelTrans.translate(-side/2,0,side/2);
    modelTrans.rotate(-90, 1,0,0);

}

// appends processed vertices to the end of the draft
void BasegridMesh::swallow(Core::VertexBufferDraft& targetDraft)
{
        VertexIterator vi(*this, targetDraft, Core::VertexIterator::ITERATE_POINTS, Core::VertexIterator::ACTION_PUSH_POINT);
        vi.pumpAll();
}

void OverlayMesh::swallow(Core::VertexBufferDraft& targetDraft)
{
    VertexIterator vi(*this, targetDraft, Core::VertexIterator::ITERATE_POINTS, Core::VertexIterator::ACTION_PUSH_POINT);
    vi.pumpAll();
}

