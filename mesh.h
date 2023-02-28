#ifndef CORE_MESH_H
#define CORE_MESH_H

#include <QVector3D>
#include <QVector>
#include "qvector3d.h"


namespace Utils
{
    class Loader; // forward declaration
}

namespace Core {

enum Status
{
    STATUS_OK,
    STATUS_ERROR,       // generic error
    STATUS_ERROR_IN,    // parameters passed to a function were invalid. Something should be wrong to the internal state of the application
};

typedef unsigned int PointIndex; // integer type that points to an array of vertices



struct PointGraph
{
    QVector<QVector<PointIndex>> connections; // TODO - set default size to 0
    int pointCount = 0;  // 0 is an invalid value if PointGraph has been initialized

    // non-qt implementation
    //PointIndex* connections = nullptr; // a 2X2 array owned by PointGraph
    //int maxRelatedCount = 0; // same for relatedCount

    /**
     * @brief Size 2X2 array containing the connections
     * @param pointCount number of points in the mesh
     * @param relatedCount number of joints eash point has
     */
    void resize(size_t pointCount/*, size_t relatedCount*/);
    void putPair(PointIndex m, PointIndex n);

    ~PointGraph();
};

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
    /// data ready to be put into a vertex buffer
    QVector<float> swallowedData;
    QVector<float> projectedFaceids; // face ids projected to window area
    QVector<float> color;

public:
    QVector<QVector3D> points;
    QVector<Triangle> faces;
    PointGraph graph;


    typedef Triangle FaceType;


    enum ChewTypeFields
    {
        CHEW_POINTS = 1,
        CHEW_NORMALS = 2,
        CHEW_FACEIDS = 4,
        CHEW_GRAPH = 8
    };

    union ChewType
    {
        struct Bits
        {
            bool points: 1;
            bool normals: 1;
            bool faceIds: 1;
            bool graph: 1;
        };

        unsigned int value;
        Bits bits;

        ChewType(unsigned int v) : value(v) {};
    };



    Mesh();
    QVector<QVector3D>& getPoints(); // use for pushing points onto 'points' vector

    void chew(ChewType chewType = CHEW_POINTS); // process 'high-level' vertex data to produce raw values for vertex buffers
    ChewType chewType(); // returns the chew type used for processing vertex info
    void clear(); // clear source arrays of vertices i.e. points, faces etc. TODO - shall we also clear swallowedData ?
    const QVector<float>& getSwallowedData();
    const QVector<float>& getProjectedFaceids();
    int chewedCount();

    friend class Utils::Loader;

private:
    ChewType chewTypeUsed = 0;

};

QVector3D hideIntInVector3D(unsigned int i);
unsigned int unhideIntFromVector3D(QVector3D& v, bool normalized=false);


} // namespace Core

#endif // CORE_MESH_H
