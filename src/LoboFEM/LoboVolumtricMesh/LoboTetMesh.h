#pragma once
#include <Eigen/Dense>
#include <string>
#include "Shaders/LoboShader.h"

enum TetMeshStatusFlags_ {
    TetMeshStatusFlags_None = 0,
    TetMeshStatusFlags_tetgened = 1 << 0,  // generted mesh by tetgen
    TetMeshStatusFlags_loadtet = 1 <<1,
    TetMeshStatusFlags_initialGL = 1 << 2,
    TetMeshStatusFlags_updateGL = 1 <<3
};

namespace Lobo {
    class LoboMesh;
    class LoboTetMesh {
       public:
        LoboTetMesh();
        ~LoboTetMesh();

        virtual void drawImGui(bool* p_open = NULL);
        virtual void paintGL(LoboShader* shader);
        virtual void initialGL();
        virtual void updateGL();

        virtual void setInputPolygon(
            LoboMesh* lobomesh);  // assume the input mesh only has one shape
        virtual void setInputPolygon(Eigen::MatrixXd* vertices,
                                     Eigen::MatrixXi* Faces);

        virtual void generateTet(const char* tetgen_command = NULL);

        virtual void loadTetMeshBinary(const char* filename);
        virtual void loadTetMeshAscii(const char* filebase);

        virtual void exportTetMeshBinary(const char* filename);
        virtual void exportTetMeshAscii(const char* filebase);

        // generate tetgen
        std::string filebase;
        std::string tetgen_command;
        bool usebinary;
        bool initializedGL;
        int status_flags;

        // Input polygon
        Eigen::MatrixXd tri_vertices;
        Eigen::MatrixXi tri_faces;

        // Tetrahedralized interior
        Eigen::MatrixXd tet_vertice;  //#numVertices*3
        Eigen::MatrixXi tet_indices;  //#numTet*4
        Eigen::MatrixXi tet_faces;    //#numface*3 // for rendering

        Lobo::LoboShaderConfig shader_config;

        
    };
}  // namespace Lobo