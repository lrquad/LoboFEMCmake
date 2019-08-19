#include "LoboTetMesh.h"
#include <igl/copyleft/tetgen/tetrahedralize.h>
#include "Functions/EigenMatrixIO.h"
#include "LoboMesh/LoboMesh.h"

Lobo::LoboTetMesh::LoboTetMesh() {}

Lobo::LoboTetMesh::~LoboTetMesh() {}

void Lobo::LoboTetMesh::generateTet(const char* tetgen_command) {
    std::string command_ = "pq1.414Y";
    if (tetgen_command != NULL) {
        command_ = tetgen_command;
    }

    igl::copyleft::tetgen::tetrahedralize(tri_vertices, tri_faces,
                                          command_.c_str(), tet_vertice,
                                          tet_indices, tet_faces);
}

void Lobo::LoboTetMesh::setInputPolygon(LoboMesh* lobomesh) {
    // set vertices
    int num_tri_vertices = lobomesh->attrib.vertices.size() / 3;
    tri_vertices.resize(num_tri_vertices, 3);

    for (int i = 0; i < num_tri_vertices; i++) {
        tri_vertices.data()[i] = lobomesh->attrib.vertices[i * 3 + 0];
        tri_vertices.data()[i + num_tri_vertices] =
            lobomesh->attrib.vertices[i * 3 + 1];
        tri_vertices.data()[i + 2 * num_tri_vertices] =
            lobomesh->attrib.vertices[i * 3 + 2];
    }
    int num_tri_faces = lobomesh->num_faces;
    tri_faces.resize(num_tri_faces, 3);
    int face_index_slid = 0;
    for (int i = 0; i < lobomesh->shapes.size(); i++) {
        for (int j = 0; j < lobomesh->shapes[i].mesh.indices.size() / 3; j++) {
            tri_faces.data()[face_index_slid] =
                lobomesh->shapes[i].mesh.indices[j * 3].vertex_index;
            tri_faces.data()[face_index_slid + num_tri_faces] =
                lobomesh->shapes[i].mesh.indices[j * 3 + 1].vertex_index;
            tri_faces.data()[face_index_slid + 2 * num_tri_faces] =
                lobomesh->shapes[i].mesh.indices[j * 3 + 2].vertex_index;
            face_index_slid += 1;
        }
    }
}

void Lobo::LoboTetMesh::setInputPolygon(Eigen::MatrixXd* vertices,
                                        Eigen::MatrixXi* faces) {
    tri_vertices.resize(vertices->rows(), vertices->cols());
    memcpy(tri_vertices.data(), vertices->data(),
           sizeof(double) * vertices->rows() * vertices->cols());
    tri_faces.resize(faces->rows(), faces->cols());
    memcpy(tri_faces.data(), faces->data(),
           sizeof(int) * faces->rows() * faces->cols());
}

void Lobo::LoboTetMesh::loadTetMeshBinary(const char* filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in.good()) {
        std::cout << filename << "file not open" << std::endl;
        return;
    }
    EigenMatrixIO::read_binary(in, tet_vertice);
    EigenMatrixIO::read_binary(in, tet_indices);
    EigenMatrixIO::read_binary(in, tet_faces);
    in.close();
}

void Lobo::LoboTetMesh::exportTetMeshBinary(const char* filename) {
    std::ofstream out(filename,
                      std::ios::out | std::ios::binary | std::ios::trunc);
    EigenMatrixIO::write_binary(out, tet_vertice);
    EigenMatrixIO::write_binary(out, tet_indices);
    EigenMatrixIO::write_binary(out, tet_faces);
    out.close();
}

void Lobo::LoboTetMesh::loadTetMeshAscii(const char* filebase) {
    std::ostringstream stringStream;
    stringStream << filebase << ".ele";
    std::string elementfile = stringStream.str();
    stringStream.str("");
    stringStream.clear();
    stringStream << filebase << ".node";
    std::string nodefile = stringStream.str();
    stringStream.str("");
    stringStream.clear();
    stringStream << filebase << ".face";
    std::string facefile = stringStream.str();

    int tmp;
    int numele, numvet, numface;
    std::ifstream inputstream(elementfile);
    inputstream >> numele >> tmp >> tmp;
    tet_indices.resize(numele, 3);
    for (int i = 0; i < numele; i++) {
        inputstream >> tmp;
        for (int j = 0; j < 4; j++) {
            inputstream >> tet_indices.data()[j * numele + i];
        }
    }
    inputstream.close();

    inputstream.open(nodefile);
    inputstream >> numvet >> tmp >> tmp >> tmp;
    tet_vertice.resize(numvet, 3);
    for (int i = 0; i < numvet; i++) {
        inputstream >> tmp;
        for (int j = 0; j < 3; j++) {
            inputstream >> tet_vertice.data()[j * numvet + i];
        }
    }
    inputstream.close();

    inputstream.open(facefile);
    inputstream >> numface;
    tet_faces.resize(numface, 3);
    for (int i = 0; i < numface; i++) {
        inputstream >> tmp;
        for (int j = 0; j < 3; j++) {
            inputstream >> tet_faces.data()[j * numface + i];
        }
    }
    inputstream.close();
}
void Lobo::LoboTetMesh::exportTetMeshAscii(const char* filebase) {
    std::ostringstream stringStream;
    stringStream << filebase << ".ele";
    std::string elementfile = stringStream.str();
    stringStream.str("");
    stringStream.clear();
    stringStream << filebase << ".node";
    std::string nodefile = stringStream.str();
    stringStream.str("");
    stringStream.clear();
    stringStream << filebase << ".face";
    std::string facefile = stringStream.str();

    std::ofstream outstream(elementfile);
    outstream << tet_indices.rows() << " 4 0 " << std::endl;
    for (int i = 0; i < tet_indices.rows(); i++) {
        outstream << i << " ";
        for (int j = 0; j < 4; j++) {
            outstream << tet_indices.data()[j * tet_indices.rows() + i] << " ";
        }
        outstream << std::endl;
    }
    outstream.close();

    outstream.open(nodefile);
    outstream << tet_vertice.rows() << " 3 0 0 " << std::endl;
    for (int i = 0; i < tet_vertice.rows(); i++) {
        outstream << i << " ";
        for (int j = 0; j < 3; j++) {
            outstream << tet_vertice.data()[j * tet_vertice.rows() + i] << " ";
        }
        outstream << std::endl;
    }
    outstream.close();
    outstream.open(facefile);
    outstream << tet_faces.rows() << std::endl;
    for (int i = 0; i < tet_faces.rows(); i++) {
        outstream << i << " ";
        for (int j = 0; j < 3; j++) {
            outstream << tet_faces.data()[j * tet_faces.rows() + i] << " ";
        }
        outstream << std::endl;
    }
    outstream.close();
}