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




void VertexIterator::init()
{
    switch (type)
    {
        case ITERATE_TRIANGLES:
            pointIndexer = new IndexerRanged<int>(0, 3, {1,1,-2});
        break;
        case ITERATE_TRIANGLES_TO_LINES:
            pointIndexer = new IndexerRanged<int>(0, 3, {1,0,1,0,-2,0});
        break;
        case ITERATE_POINTS:
            pointIndexer = new IndexerRanged<int>(0, sourceArrays.points.size());
        break;
    }
}

VertexIterator::VertexIterator(const SourceArrays& sa, QVector<float>& target, Type type, ActionType actionType)
    : sourceArrays(sa),
      targetArray(target),
      faceIndex(0),
      infaceIndex(0),
      type(type)
{
    setAction(actionType);
    switch (type)
    {
        case ITERATE_TRIANGLES:
            faceIndexer = new IndexerRanged<FaceIndex>(0, sa.faces.size(),{0,0,1});
        break;
        case ITERATE_TRIANGLES_TO_LINES:
            faceIndexer = new IndexerRanged<FaceIndex>(0, sa.faces.size(),{0,0,0,0,0,1});
        break;
    }

    init();

}

VertexIterator::VertexIterator(const SourceArrays& sa, QVector<FaceIndex>* faceIds, QVector<float>& target, Type type, ActionType actionType)
    : sourceArrays(sa),
      targetArray(target),
      faceIndex(0),
      infaceIndex(0),
      faceIds(faceIds),
      type(type)
{
    setAction(actionType);
    faceIndexer = new IndexerIndirect<FaceIndex>(*faceIds);
    switch (type)
    {
        case ITERATE_TRIANGLES:
            faceIndexer = new IndexerIndirect<FaceIndex>(*faceIds,{0,0,1});
        break;
        case ITERATE_TRIANGLES_TO_LINES:
            faceIndexer = new IndexerIndirect<FaceIndex>(*faceIds,{0,0,0,0,0,1});
        break;
    }

    init();
}

VertexIterator::~VertexIterator()
{
    delete faceIndexer;
    faceIndexer = nullptr;
}


void VertexIterator::action_pushFacePoint()
{
    const QVector3D& v = sourceArrays.points[ sourceArrays.faces[faceIndex].points[infaceIndex] ];
    targetArray.append(v.x());
    targetArray.append(v.y());
    targetArray.append(v.z());
}

void VertexIterator::action_pushFaceId()
{
    QVector3D faceidAsVector = hideIntInVector3D(faceIndex);
    targetArray.append(faceidAsVector.x());
    targetArray.append(faceidAsVector.y());
    targetArray.append(faceidAsVector.z());
}

void VertexIterator::action_pushPoint(PointIndex pointIndex)
{
    const QVector3D& v = sourceArrays.points[ pointIndex ];
    targetArray.append(v.x());
    targetArray.append(v.y());
    targetArray.append(v.z());
}


bool VertexIterator::pumpByFace()
{
    if (!faceIndexer->available())
        return false;

    faceIndex = faceIndexer->get();
    infaceIndex = pointIndexer->get();

    ((*this).*(action))(); // invoke object method as a callback

    faceIndexer->next();
    pointIndexer->next();

    return true;
}

bool VertexIterator::pumpByPoint()
{
    if (!pointIndexer->available())
        return false;

    action_pushPoint( pointIndexer->get());

    pointIndexer->next();
}


void VertexIterator::setAction(ActionType t)
{
    switch (t)
    {
        case ACTION_PUSH_POINT:
            action = &VertexIterator::action_pushFacePoint;
        break;
        case ACTION_PUSH_FACEID:
            action = &VertexIterator::action_pushFaceId;
        break;
    }
    actionType = t;

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
