#include "loader.h"
#include "qvector3d.h"
#include "stl_reader.h"

using namespace stl_reader; // only for loader.cpp local use

namespace Utils {

typedef StlMesh<float, unsigned int> ReaderMesh;

Loader::Loader()
{

}

void Loader::loadStl(const char* filename, Core::Mesh& new_mesh)
{
    ReaderMesh reader_mesh (filename);
    new_mesh.clear();

    // first, copy vertices
    for (ReaderMesh::IndexType vertex_i = 0; vertex_i < reader_mesh.num_vrts(); vertex_i++)
    {
        const ReaderMesh::ValueType* v = reader_mesh.vrt_coords(vertex_i);
        QVector3D qvertex(v[0], v[1], v[2]);

        new_mesh.points.append(qvertex);
    }
    // next, copy indices to vertices
    for (ReaderMesh::IndexType triangle_i = 0; triangle_i < reader_mesh.num_tris(); triangle_i++)
    {
        Core::Triangle triangle;
        // iterate over face/triangle corners
        for (unsigned int corner_i = 0; corner_i < triangle.PointCount; corner_i++)
        {
            triangle.points[corner_i] = reader_mesh.tri_corner_ind(triangle_i, 0);
        }
        new_mesh.faces.append(triangle);
    }

}


} // namespace Utils
