#ifndef APP_H
#define APP_H


#include <QVector>
#include "mesh.h"


using Core::VertexIterator;


class ModelMesh : public Core::Mesh
{

public:
    //QVector<float> data; // point data for vertex buffers
    QVector<float> idprojectionData; // face ids to project
    QVector<Core::FaceIndex> uioverlayFaces;


    void swallow();
    void swallowUioverlay(Core::VertexBufferDraft& targetDraft);

    ModelMesh();
};

class BasegridMesh : public Core::Mesh
{
    float side;
    int squareCount; // how many squares in each dimmension
    QVector3D color;

public:

    BasegridMesh(int squareCount, float side);
    void swallow(Core::VertexBufferDraft& targetDraft);
};


class MeshContext
{
public:
    Core::VertexBufferDraft wireframeBuffer;
    Core::VertexBufferDraft triangleBuffer;
    Core::VertexBufferDraft normalBuffer;

};

namespace App
{
    MeshContext& getMeshContext();
}



#endif // APP_H
