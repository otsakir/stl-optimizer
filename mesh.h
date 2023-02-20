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

    /// data ready to be put into a vertex buffer
    QVector<float> swallowedData;
    QVector<float> faceidMap; // face ids projected to window area

public:
    typedef Triangle FaceType;

    /*
    enum ChewType
    {
        POINTS_ONLY,
        POINTS_AND_NORMALS,

        CHEW_TYPE_UNSET
    };
    */

    struct ChewType
    {
        bool points: 1;
        bool normals: 1;
        bool faceIds: 1;
    };

    Mesh();

    void chew(ChewType chewType={true, false, false}); // process 'high-level' vertex data to produce raw values for vertex buffers
    ChewType chewType(); // returns the chew type used for processing vertex info
    void clear(); // clear source arrays of vertices i.e. points, faces etc. TODO - shall we also clear swallowedData ?
    const QVector<float>& getSwallowedData();
    const QVector<float>& getFaceidMap();
    int chewedCount();

    friend class Utils::Loader;

private:
    ChewType chewTypeUsed = {false, false, false};

};

QVector3D hideIntInVector3D(unsigned int i);
unsigned int unhideIntFromVector3D(QVector3D& v);


} // namespace Core

#endif // CORE_MESH_H
