#include "mesh.h"
#include <limits.h>
#include <algorithm>  // std::fill()
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



// generate data for vertex buffers
/**
 * @brief Mesh::swallow
 *
 * Assumes `points` is full and faces may have triangle information or not. If 'faces' is full
 * each entry will have indices of three points, the corners of a triangle.
 *
 *
 * @param primitiveType    how should the target vertex data be organized. GL_TRIANGLES will produce
 * three vertices for each face. GL_LINES will produce six vertices for each face.
 * @param target    place to store the produces vertices
 */
//void Mesh::swallow(PrimitiveType primitiveType, QVector<float>& target)
//{
//    swallowedData.clear();

//    swallowedData.clear();
//    VertexIterator vi(*this, swallowedData, VertexIterator::ITERATE_TRIANGLES, VertexIterator::ACTION_PUSH_POINT);

//    while (vi.pump()) {}; // process all

//    /*if (chewType.bits.points)
//    {
//        if (faces.empty())
//        {
//            for (int points_i=0; points_i<points.size(); points_i++)
//            {
//                QVector3D& point = points[points_i];
//                swallowedData.append(point.x());
//                swallowedData.append(point.y());
//                swallowedData.append(point.z());
//            }
//        } else
//        for (int face_i=0; face_i < faces.size(); face_i++ )
//        {
//            Triangle& triangle = faces[face_i];
//            for (int corner_i=0; corner_i < Triangle::PointCount; corner_i++)
//            {
//                PointIndex vertex_i = triangle.points[corner_i];
//                QVector3D& point = points[vertex_i];

//                swallowedData.append(point.x());
//                swallowedData.append(point.y());
//                swallowedData.append(point.z());
//            }
//        }
//    }*/

//    projectedFaceids.clear();
//    VertexIterator vi2(*this, projectedFaceids, VertexIterator::ITERATE_TRIANGLES, VertexIterator::ACTION_PUSH_FACEID );
//    while (vi2.pump()) {};

///*
//    if (chewType.bits.faceIds)
//    {
//        for (int face_i=0; face_i < faces.size(); face_i++ )
//        {
//            QVector3D faceidAsVector = hideIntInVector3D(face_i);
//            for (int corner_i=0; corner_i < Triangle::PointCount; corner_i++)
//            {
//                // same faceid for all three corners of the face
//                projectedFaceids.append(faceidAsVector.x());
//                projectedFaceids.append(faceidAsVector.y());
//                projectedFaceids.append(faceidAsVector.z());
//            }
//        }
//    }
//    */
//}




void Mesh::chew(ChewType chewType)
{
    color.clear();

    chewTypeUsed = chewType;

    if (chewType.bits.graph)
    {
        graph.resize(points.size());
        if (!faces.empty())
        {
            for (int face_i=0; face_i < faces.size(); face_i++ )
            {
                Triangle& triangle = faces[face_i];
                graph.putPair(triangle.points[0], triangle.points[1]);
                graph.putPair(triangle.points[1], triangle.points[2]);
                graph.putPair(triangle.points[2], triangle.points[0]);
            }
        }
    }

    // populate pointFaces array. Be able to tell which faces are adjacent to any point.
    if (!faces.empty())
    {
        // clear pointFaces. Initially, each point should have 0 adjacent faces
        pointFaces.resize(points.size());
        for (int point_i=0; point_i<pointFaces.size(); point_i++)
        {
            pointFaces[point_i].clear();
        }

        for (int face_i=0; face_i < faces.size(); face_i++ )
        {
            Triangle& triangle = faces[face_i];
            for (PointIndex point_i=0; point_i < Triangle::PointCount; point_i++)
            {
                QVector<FaceIndex>& faces_for_point = pointFaces[triangle.points[point_i]];
                if (! faces_for_point.contains(face_i))
                {
                    faces_for_point.append(face_i);
                }
            }
        }

        faceFaces.resize(faces.size());
        for (int face_i=0; face_i < faces.size(); face_i++ )
        {
            faceFaces[face_i].clear();

            Triangle& triangle = faces[face_i];
            for (PointIndex point_i=0; point_i < Triangle::PointCount; point_i++)
            {
                // get all adjacent faces for this point of the triangle
                QVector<FaceIndex>& faces_for_point = pointFaces[triangle.points[point_i]];
                for (int pointfaces_i=0; pointfaces_i < faces_for_point.size(); pointfaces_i++)
                {
                    // push adjacent faces of the triangle's point to the faceFaces array, if not already there
                    if ( ! faceFaces[face_i].contains(faces_for_point[pointfaces_i]) )
                    {
                        faceFaces[face_i].append(faces_for_point[pointfaces_i]);
                    }
                }
            }
        }
    }


}

Mesh::ChewType Mesh::chewType()
{
    return chewTypeUsed;
}


const QVector<float> &Mesh::getProjectedFaceids()
{
    return projectedFaceids;
}

// number of vertices
/*
int Mesh::chewedCount()
{
    return swallowedData.size() / Mesh::FaceType::PointCount;
}
*/

//#define EMPTY_VALUE UINT_MAX

// resize and clear
void PointGraph::resize(size_t pointCount/*, size_t relatedCount*/)
{
/*
    size_t area = pointCount * relatedCount;

    if (pointCount == 0 || relatedCount == 0)
    {
        qDebug() << "invalid dimmensions for PointGraph";
        return STATUS_ERROR;
    }

    // if "area" of new connections array is different we need to re-allocate
    if (area != this->pointCount * this->maxRelatedCount)
    {
        delete [] connections;
        pointCount = this->pointCount;
        relatedCount = this->maxRelatedCount;
        connections = new PointIndex[area];
    }

    // reset content
    std::fill(connections, connections+area, EMPTY_VALUE);
    */

    connections.resize(pointCount);
    //connections.squeeze(); // TODO - release memory for point entries that are now not used
    for (int point_i = 0; point_i < connections.size(); point_i++)
    {
        connections[point_i].clear();

    }
}

void PointGraph::putPair(PointIndex m, PointIndex n)
{
    //assert((int)m < pointCount);
    //assert((int)n < pointCount);

    if (connections[m].indexOf(n) == -1)
    {
        connections[m].append(n);
    }

    if (connections[n].indexOf(m) == -1)
    {
        connections[n].append(m);
    }

}

void PointGraph::clear()
{
    connections.clear();
    pointCount = 0;
}

/*
void PointGraph::putPoint(PointIndex basePoint, PointIndex relative)
{
    PointIndex* baseColumn = connections + basePoint*this->maxRelatedCount;
    PointIndex relative_i = 0;
    while (relative_i < this->maxRelatedCount)
    {
        if (baseColumn[relative_i] == EMPTY_VALUE)
        {
            baseColumn[relative_i] = relative;
            break;
        } else
        {

        }
    }
}
*/

PointGraph::~PointGraph()
{
    /*if ( ! connections )
        return;

    delete [] connections;
    connections =  nullptr;*/
}

} // namespace Core
