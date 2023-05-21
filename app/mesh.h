#ifndef CORE_MESH_H
#define CORE_MESH_H

#include <QVector3D>
#include <QVector>
#include "qmatrix4x4.h"
#include "qvector3d.h"
#include <functional>


namespace Utils
{
    class Loader; // forward declaration
}

namespace Core {

QVector3D hideIntInVector3D(unsigned int i);

enum Status
{
    STATUS_OK,
    STATUS_ERROR,       // generic error
    STATUS_ERROR_IN,    // parameters passed to a function were invalid. Something should be wrong to the internal state of the application
};

typedef unsigned int PointIndex; // integer type that points to an array of vertices
typedef unsigned int FaceIndex;


/*!
    \brief Keeps connections between points
*/
struct PointGraph
{
    QVector<QVector<PointIndex>> connections; // for each point holds an array of point this is connected to - TODO: set default size to 0
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

    void clear();
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


/*!
    \brief Data model for a mesh

    Keeps primary point and face data as well as processed data on the mesh.
*/
struct SourceArrays
{
    // primary source data
    QVector<QVector3D> points;
    QVector<Triangle> faces;
    // secondary source data
    PointGraph graph;
    QVector<QVector<FaceIndex>> pointFaces; // for each point there is an array of faces. Point and face indices point to 'points' and 'faces' arrays respectively.
    QVector<QVector<FaceIndex>> faceFaces;  // tells which faces are adjacent to a face. Faces indices point to 'faces' array.

    void clear()
    {
        points.clear();
        faces.clear();
        graph.clear();
        pointFaces.clear();
        faceFaces.clear();
    }

    QVector3D faceNormal(FaceIndex faceIndex) const
    {
        const QVector3D& p1 = points[ faces[faceIndex].points[0] ];
        const QVector3D& p2 = points[ faces[faceIndex].points[1] ];
        const QVector3D& p3 = points[ faces[faceIndex].points[2] ];

        const QVector3D v1 = p2-p1;
        const QVector3D v2 = p3-p2;

        QVector3D n = QVector3D::normal(v1,v2);
        return n;
    }
};

/*!
 * \brief Ιndex generator base class
 *
 * Use with get(), next() and available().
 */
template <typename T>
class Indexer
{
protected:

    QVector<T> deltas; // array with delta values to add to current 'Index'
    int deltas_i = 0; // Index within deltas array. It will wrap-around at the end of it.
    T index; // Current index value.

public:

    Indexer() : Indexer(0, {1}) {};
    Indexer(T index, QVector<T> deltas) : deltas(deltas), index(index) {};

    virtual bool available() = 0;
    virtual T& get() = 0;

    bool next()
    {
        //deltas[0];
        index += deltas[deltas_i];
        deltas_i ++;
        if (deltas_i >= deltas.size())
            deltas_i = 0;

        return available();
    }

    bool next(int step)
    {
        index += step;

        return available();
    }

    virtual ~Indexer() {};

};

/*!
 * \brief Index generator moving in steps
 *
 * Produces a range of values advancing in given increment steps. If the steps
 * add up to 0 it will keep circling around.
 */
template <typename T>
class IndexerRanged : public Indexer<T>
{
    using Indexer<T>::index;

private:
    T begin;
    T pastend;

public:
    IndexerRanged(T begin, T pastend) : IndexerRanged(begin, pastend, {1}) {}
    IndexerRanged(T begin, T pastend, QVector<T> deltas) : Indexer<T>(begin, deltas), begin(begin), pastend(pastend)
    {
        assert(deltas.size() > 0);
    };

    bool available() override
    {
        return (index < pastend);
    }

    /// returns current index
    T& get() override
    {
        return index;
    }

};

/*!
 * \brief Index generator using lookup array
 *
 * Instead of directly returning indices, it will use them to lookup values in an
 * arbitrary intermediate array.
 */
template <typename T>
class IndexerIndirect : public Indexer<T>
{
    using Indexer<T>::index;

private:
    QVector<T>& vector; /// lookup array

public:
    IndexerIndirect(QVector<T>& vec) : vector(vec) {};
    IndexerIndirect(QVector<T>& vec, QVector<T> deltas) : Indexer<T>(0, deltas), vector(vec) {};

    bool available() override
    {
        return (index < vector.size());
    }

    /// returns current index
    T& get() override
    {
        return vector[index];
    }



};


class VertexBufferDraft;

class VertexIterator
{
public:
    enum Type
    {
        ITERATE_TRIANGLES, // for each face produce 3 points A, B, C.
        ITERATE_TRIANGLES_TO_LINES, // for each face produce points A-B, B-C, C-A
        ITERATE_POINTS, // go over all source mesh 'points' one by one
        ITERATE_PER_TRIANGLE // both faces and point source buffers are needed. One iteration step per face.
    };

    enum ActionType
    {
        ACTION_PUSH_POINT, // pushed the point determined by face/
        ACTION_PUSH_FACEID,
        ACTION_PUSH_NORMAL,
        ACTION_CALLBACK_POINT, // just invoke the callback passing each point
    };

    typedef bool (VertexIterator::*Pump_cb)(); // typedef a member function pointer for pumpBy*() members
    //typedef void (*PointCallback)(const QVector3D& point);
    typedef std::function<void(QVector3D& point)> PointCallback; // a callback type to be used when iterating over points

    VertexIterator(SourceArrays& sourceArrays, Type type, ActionType actionType, PointCallback pointCallback);
    VertexIterator(SourceArrays& sa, QVector<float>* target, Type type=ITERATE_TRIANGLES, ActionType actionType=ACTION_PUSH_POINT);
    VertexIterator(SourceArrays& sa, VertexBufferDraft& bufferDraft, Type type=ITERATE_TRIANGLES, ActionType actionType=ACTION_PUSH_POINT);
    // iterator within face id lookup table that pushes directly to a QVector target
    VertexIterator(SourceArrays& sa, QVector<FaceIndex>* faceIds, QVector<float>& target, Type type=ITERATE_TRIANGLES, ActionType actionType=ACTION_PUSH_POINT);
    // iterator within face id lookup table that appends to a VertexBufferDraft
    VertexIterator(SourceArrays& sa, QVector<FaceIndex>* faceIds, VertexBufferDraft& bufferDraft, Type type=ITERATE_TRIANGLES, ActionType actionType=ACTION_PUSH_POINT);

    void init();
    ~VertexIterator();

    bool pumpByFace();
    bool pumpByPoint();
    bool pumpByFaceOnly();
    bool pump();
    void pumpAll();
    Pump_cb pumpFunction;

private:

    SourceArrays& sourceArrays;
    const QVector<FaceIndex>* faceIds = nullptr;

    Indexer<FaceIndex>* faceIndexer = nullptr;
    Indexer<int>* pointIndexer = nullptr;
    // variables for iterating over faces and points
    FaceIndex faceIndex;
    int infaceIndex;
    int pointIndex;     // index to .points array

    Type type;
    ActionType actionType;

    typedef void (VertexIterator::*Action_cb)(); // action callback type
    void action_pushFacePoint(); /// Push a QVector3D point determined from faceIndec/infaceIndex to target after converting it to 3 floats.
    void action_pushFaceId();
    void action_pushPoint(); /// Push a QVector3D point from mesh.points into target after converting it to 3 floats.
    void action_pushNormal();
    void action_callbackFacePoint();
    void action_callbackPoint();
    Action_cb actionFunction;
    PointCallback pointCallback;

    QVector<float>* targetArray;
    VertexBufferDraft* bufferDraft;
    void setAction(ActionType t);

};


class Mesh : public SourceArrays
{
protected:
    // data ready to be put into a vertex buffer
    QVector<float> color;

public:

    QMatrix4x4 modelTrans; // place item in the world
    typedef Triangle FaceType;
    // metrics
    QVector3D minPoint;
    QVector3D maxPoint;
    QVector3D centerPoint;
    float width;    // size in x
    float height;   // y
    float depth;    // z
    float boundingRadius; // radius of a bounding sphere



    enum ChewTypeFields
    {
        //CHEW_NORMALS = 1,
        //CHEW_FACEIDS = 2,
        CHEW_GRAPH = 1
    };

    union ChewType
    {
        struct Bits
        {
            //bool normals: 1;
            //bool faceIds: 1;
            bool graph: 1;
        };

        unsigned int value;
        Bits bits;

        ChewType(unsigned int v) : value(v) {};
    };

    enum PrimitiveType
    {
        PRIMITIVE_GL_TRIANGLES,
        PRIMITIVE_GL_LINES
    };



    Mesh();
    QVector<QVector3D>& getPoints(); // use for pushing points onto 'points' vector
    void chew(ChewType chewType); // processes primary point and face data to product higher level secondary mesh data like a graph of points, faces adjacent to points etc.
    //ChewType chewType(); // returns the chew type used for processing vertex info
    void swallow(Core::VertexBufferDraft& targetDraft);
    void generateMetrics();

    friend class Utils::Loader;

protected:

private:
    ChewType chewTypeUsed = 0;

};

QVector3D hideIntInVector3D(unsigned int i);
unsigned int unhideIntFromVector3D(QVector3D& v, bool normalized=false);


/*!
 * \brief The VertexBufferDraft class
 *
 * A QVector with blocks of vertex data and supplementary information for these blocks.
 */
class VertexBufferDraft
{
public:
    struct RegisteredInfo
    {
        int offset;
        int size;

        RegisteredInfo(int offset = 0)
        {
            this->offset = offset;
            size = 0;
        }

    };

private:
    QVector<const SourceArrays*> registeredMeshes;
    QVector<RegisteredInfo> registeredInfo;
    QVector<float> data;

    void startCountingPumped()
    {
        registeredInfo.append(RegisteredInfo(data.size()));
    }

    void stopCountingPumped()
    {
        RegisteredInfo& info = registeredInfo.last();
        info.size = data.size() - info.offset;
    }

public:

    void clear()
    {
        registeredMeshes.clear();
        registeredInfo.clear();
        data.clear();
    }

    QVector<float>* registerForFrame(const SourceArrays* mesh)
    {
        if (registeredMeshes.contains(mesh))
            return nullptr;

        registeredMeshes.append(mesh);


        return &data;
    }

    const QVector<float>& getData()
    {
        return data;
    }

    // same as getData() but will also return an offset inside the 'data' QVector where the data for the specific mesh reside or -1 if the mesh* is not found
    RegisteredInfo* getMeshInfo(const SourceArrays* mesh)
    {
        int infoIndex = registeredMeshes.indexOf(mesh);
        if (infoIndex == -1)
        {
            return 0;
        }

        return &registeredInfo[infoIndex];
    }

    friend void VertexIterator::pumpAll();
};

class Camera
{
private:
    float zoom;
    float zPos;
    float xRot, yRot, zRot;
public:
    QMatrix4x4 trans; // holds initial positioning

    Camera()
        : xRot(0), yRot(0), zRot(0), zPos(0), zoom(0)
    {}

    Camera(float xRot, float yRot, float zRot, float zPos)
        : xRot(xRot), yRot(yRot), zRot(zRot), zPos(zPos), zoom(0)
    {}

    // view transformation (inverted camera position) ready to use
    QMatrix4x4 getTrans()
    {
        QMatrix4x4 viewTrans = trans;
        viewTrans.rotate(xRot, 1, 0, 0);
        viewTrans.rotate(yRot, 0, 1, 0);
        viewTrans.rotate(zRot, 0, 0, 1);
        viewTrans.translate(0,0,zPos + zoom);
        return viewTrans.inverted();
    }

    void setTrans(QMatrix4x4& trans)
    {
        this->trans = trans;
    }

    void setZoom(float zoom)
    {
        this->zoom = zoom;
    }

    void setRot(float x, float y, float z)
    {
        xRot = x;
        yRot = y;
        zRot = z;
    }
};

} // namespace Core

#endif // CORE_MESH_H
