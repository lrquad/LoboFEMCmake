#pragma once
#include <Eigen/Dense>

namespace Lobo {
class LoboMesh;
class LoboTetMesh {
   public:
    LoboTetMesh();
    ~LoboTetMesh();

    virtual void setInputPolygon(LoboMesh* lobomesh); //assume the input mesh only has one shape
    virtual void setInputPolygon(Eigen::MatrixXd *vertices,Eigen::MatrixXi* Faces);

    virtual void generateTet(const char* tetgen_command = NULL);

    virtual void loadTetMeshBinary(const char* filename);
    virtual void loadTetMeshAscii(const char* filebase);

    virtual void exportTetMeshBinary(const char* filename);
    virtual void exportTetMeshAscii(const char* filebase);

protected:

    // Input polygon
    Eigen::MatrixXd tri_vertices;
    Eigen::MatrixXi tri_faces;

    // Tetrahedralized interior
    Eigen::MatrixXd tet_vertice; //#numVertices*3
    Eigen::MatrixXi tet_indices; //#numTet*4
    Eigen::MatrixXi tet_faces; //#numface*3 // for rendering
};
}  // namespace Lobo