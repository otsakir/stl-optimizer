
#include "app.h"
#include "loader.h"
#include "limits.h"
#include "cmath"


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
    : minPoint(QVector3D(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max())),
      maxPoint(QVector3D(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
{
    // load primary source data
    Utils::Loader loader;
    loader.loadStl("/home/nando/tmp/cone.stl", *this);

    QVector3D& minPoint = this->minPoint;
    QVector3D& maxPoint = this->maxPoint;

    VertexIterator vi(*this, Core::VertexIterator::ITERATE_TRIANGLES, Core::VertexIterator::ACTION_CALLBACK_POINT, [&minPoint, &maxPoint](const QVector3D& point){
        // find min
        if (point.x() < minPoint.x())
            minPoint.setX(point.x());
        if (point.y() < minPoint.y())
            minPoint.setY(point.y());
        if (point.z() < minPoint.z())
            minPoint.setZ(point.z());
        // find max
        if (point.x() > maxPoint.x())
            maxPoint.setX(point.x());
        if (point.y() > maxPoint.y())
            maxPoint.setY(point.y());
        if (point.z() > maxPoint.z())
            maxPoint.setZ(point.z());

    });
    vi.pumpAll();
    centerPoint = (minPoint + maxPoint)/2;
    width = maxPoint.x() - minPoint.x();
    height = maxPoint.y() - minPoint.y();
    depth = maxPoint.z() - minPoint.z();

    boundingRadius = sqrt(width*width + height*height + depth*depth);

    qDebug() << "min point: " << minPoint;
    qDebug() << "max point: " << maxPoint;
    qDebug() << "mid point: " << centerPoint;
    qDebug() << "dimensions: " << width << " " << height << " " << depth;
    qDebug() << "bounding radius: " << boundingRadius;

    modelTrans.rotate(-90, 1, 0, 0);
    modelTrans.translate(-centerPoint.x(),-centerPoint.y(), -minPoint.z());
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

