#include "mesh.h"

namespace Core {


Mesh::Mesh()
{

}

void Mesh::chew(ChewType chewType)
{
    swallowedData.clear();

    if (chewType == POINTS_ONLY)
    {
        for (int face_i=0; face_i < faces.size(); face_i++ )
        {
            Triangle& triangle = faces[face_i];
            for (int corner_i=0; corner_i < Triangle::PointCount; corner_i++)
            {
                PointIndex vertex_i = triangle.points[corner_i];
                QVector3D& point = points[vertex_i];

                swallowedData.append(point.x());
                swallowedData.append(point.y());
                swallowedData.append(point.z());
            }
        }
        chewTypeUsed = POINTS_ONLY;
    }

}

Mesh::ChewType Mesh::chewType()
{
    return chewTypeUsed;
}

void Mesh::clear()
{
    points.clear();
    faces.clear();
}

const QVector<float> &Mesh::getSwallowedData()
{
    return swallowedData;
}

// number of vertices
int Mesh::chewedCount()
{
    return swallowedData.size() / Mesh::FaceType::PointCount;
}

} // namespace Core
