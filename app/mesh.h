#ifndef CORE_MESH_H
#define CORE_MESH_H

#include <QVector3D>
#include <QVector>
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


struct SourceArrays
{
    // primary source data
    QVector<QVector3D> points;
    QVector<Triangle> faces;
    // secondary source data
    PointGraph graph;
    QVector<QVector<FaceIndex>> pointFaces; // faces per point - point indexing follows the the numbering of `points` array
    QVector<QVector<FaceIndex>> faceFaces;  // adjacent faces for each face

    void clear()
    {
        points.clear();
        faces.clear();
        graph.clear();
        pointFaces.clear();
        faceFaces.clear();
    }
};


template <typename T>
class Indexer
{
protected:

    QVector<T> deltas;
    int deltas_i = 0;
    T index;

public:

    Indexer() : Indexer(0, {1}) {};
    Indexer(T index, QVector<T> deltas) : deltas(deltas), index(index) {};

    virtual bool available() = 0;
    virtual T& get() = 0;

    bool next()
    {
        deltas[0];
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

template <typename T>
class IndexerIndirect : public Indexer<T>
{
    using Indexer<T>::index;

private:
    QVector<T>& vector;

public:
    IndexerIndirect(QVector<T>& vec) : vector(vec) {};
    IndexerIndirect(QVector<T>& vec, QVector<T> deltas) : Indexer<T>(0, deltas), vector(vec) {};

    bool available() override
    {
        return (index < vector.size());
    }

    // returns current index
    T& get() override
    {
        return vector[index];
    }



};

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

    // returns _current_ index
    T& get() override
    {
        return index;
    }

};


class VertexIterator
{
public:
    enum Type
    {
        ITERATE_TRIANGLES,
        ITERATE_TRIANGLES_TO_LINES,
        ITERATE_POINTS
    };

    enum ActionType
    {
        ACTION_PUSH_POINT,
        ACTION_PUSH_FACEID
    };

    VertexIterator(const SourceArrays& sa, QVector<float>& target, Type type=ITERATE_TRIANGLES, ActionType actionType=ACTION_PUSH_POINT);
    VertexIterator(const SourceArrays& sa, QVector<FaceIndex>* faceIds, QVector<float>& target, Type type=ITERATE_TRIANGLES, ActionType actionType=ACTION_PUSH_POINT);
    void init();
    ~VertexIterator();

    typedef void (VertexIterator::*Action_cb)(); // action callback type
    void action_pushFacePoint();
    void action_pushFaceId();
    void action_pushPoint(PointIndex pointIndex);

    bool pumpByFace();
    bool pumpByPoint();


private:

    const SourceArrays& sourceArrays;
    const QVector<FaceIndex>* faceIds = nullptr;

    Indexer<FaceIndex>* faceIndexer = nullptr;
    Indexer<int>* pointIndexer = nullptr;
    // variables for iterating over faces and points
    FaceIndex faceIndex;
    int infaceIndex;

    Type type;
    ActionType actionType;
    Action_cb action;

    QVector<float>& targetArray;
    void setAction(ActionType t);

};



class Mesh : public SourceArrays
{
protected:
    // data ready to be put into a vertex buffer
    QVector<float> color;

public:

    typedef Triangle FaceType;


    enum ChewTypeFields
    {
        CHEW_NORMALS = 1,
        CHEW_FACEIDS = 2,
        CHEW_GRAPH = 4
    };

    union ChewType
    {
        struct Bits
        {
            bool normals: 1;
            bool faceIds: 1;
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

    void chew(ChewType chewType); // process 'high-level' vertex data to produce raw values for vertex buffers
    ChewType chewType(); // returns the chew type used for processing vertex info

    friend class Utils::Loader;

protected:

private:
    ChewType chewTypeUsed = 0;

};

QVector3D hideIntInVector3D(unsigned int i);
unsigned int unhideIntFromVector3D(QVector3D& v, bool normalized=false);


class VertexBufferDraft
{
    QVector<float> data;
};

} // namespace Core

#endif // CORE_MESH_H
