#ifndef CORE_MESH_H
#define CORE_MESH_H

#include <QVector3D>
#include <QVector>
#include "qvector3d.h"

typedef unsigned int PointIndex; // integer type that points to an array of vertices


namespace Utils
{
    class Loader; // forward declaration
}

namespace Core {

template <int pc> // pc -> PointCount
class Face
{

public:
    const static int PointCount = pc;
    PointIndex points[PointCount]; // indices to point 1, 2, 3
};

typedef Face<3> Triangle;

class Mesh
{
    QVector<QVector3D> points;
    QVector<Triangle> faces;

public:
    Mesh();
    void clear();

    friend class Utils::Loader;
};

} // namespace Core

#endif // CORE_MESH_H
