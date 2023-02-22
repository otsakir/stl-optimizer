#include "mesh.h"
#include <QDebug>

namespace Core {


QVector3D hideIntInVector3D(unsigned int i)
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

unsigned int unhideIntFromVector3D(QVector3D& v, bool normalized)
{
    unsigned char x = v.x() * (normalized ? 255.0 : 1.0);
    unsigned char y = v.y() * (normalized ? 255.0 : 1.0);
    unsigned char z = v.z() * (normalized ? 255.0 : 1.0);

    unsigned int i = (unsigned int)z << 16 | (unsigned int)y << 8 | (unsigned int)x;

    return i;
}


Mesh::Mesh()
{

}

QVector<QVector3D> &Mesh::getPoints()
{
    return points;
}

void Mesh::chew(ChewType chewType)
{
    swallowedData.clear();
    projectedFaceids.clear();
    color.clear();

    chewTypeUsed = chewType;

    if (chewType.points)
    {
        if (faces.empty())
        {
            for (int points_i=0; points_i<points.size(); points_i++)
            {
                QVector3D& point = points[points_i];
                swallowedData.append(point.x());
                swallowedData.append(point.y());
                swallowedData.append(point.z());
            }
        } else
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
        //chewTypeUsed = POINTS_ONLY;
    }

    if (chewType.faceIds)
    {
        for (int face_i=0; face_i < faces.size(); face_i++ )
        {
            QVector3D faceidAsVector = hideIntInVector3D(face_i);
            for (int corner_i=0; corner_i < Triangle::PointCount; corner_i++)
            {
                // same faceid for all three corners of the face
                projectedFaceids.append(faceidAsVector.x());
                projectedFaceids.append(faceidAsVector.y());
                projectedFaceids.append(faceidAsVector.z());
            }
        }
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

const QVector<float> &Mesh::getProjectedFaceids()
{
    return projectedFaceids;
}

// number of vertices
int Mesh::chewedCount()
{
    return swallowedData.size() / Mesh::FaceType::PointCount;
}

} // namespace Core
