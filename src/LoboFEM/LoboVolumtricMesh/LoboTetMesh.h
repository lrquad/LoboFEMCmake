#pragma once
#include <Eigen/Dense>
#include <string>
#include "Shaders/LoboShader.h"
#include "ObjLoader/tiny_obj_loader.h"


enum TetMeshStatusFlags_ {
    TetMeshStatusFlags_None = 0,
    TetMeshStatusFlags_tetgened = 1 << 0,  // generted mesh by tetgen
    TetMeshStatusFlags_loadtet = 1 <<1,
    TetMeshStatusFlags_initialGL = 1 << 2,
    TetMeshStatusFlags_datasizeUpdated = 1 <<3
};

namespace Lobo {
    class LoboMesh;
    class LoboTetMesh {
       public:
        LoboTetMesh();
        ~LoboTetMesh();

        virtual void drawImGui(bool* p_open = NULL);
        virtual void mouseClicked();
        virtual void mouseRectSelect();

        virtual void paintGL(LoboShader* shader);
        virtual void initialGL();
        virtual void updateGL();
        virtual void deleteGL();

        virtual void setBindingTriMesh(LoboMesh* lobomesh);

        virtual void setInputPolygon(
            LoboMesh* lobomesh);  // assume the input mesh only has one shape
        
        virtual void setInputPolygon(Eigen::VectorXd* vertices,
                                     Eigen::VectorXi* Faces);

        virtual void generateTet(const char* tetgen_command = NULL);

        virtual void exportTetMesh();
        virtual void loadTetMesh();

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
        Eigen::VectorXd tet_vertice;  //#numVertices*3
        Eigen::VectorXf tet_vertice_attri;  //#numVertices*3
        Eigen::VectorXi tet_indices;  //#numTet*4
        Eigen::VectorXi tet_faces;    //#numface*3 // for rendering

        Eigen::MatrixXd tet_vertice_col;
        Eigen::MatrixXi tet_faces_col;
        //for test
        std::vector<unsigned int> tet_faces_glint;

        Lobo::LoboShaderConfig shader_config;

        unsigned int VAO;  // vertex array object
        unsigned int VBO;  // vertex buffer object
        unsigned int EBO;  // element buffer object
        
    protected:
        virtual void updateTetAttri(Eigen::VectorXd &inputattri,int offset,int attrisize,int totalsize);
        virtual void setTetAttriColor(double r,double g,double b,int offset = 8,int totalsize =11);
        virtual void setTetVetAttriColor(int vid,double r,double g,double b,int offset=8,int totalsize=11);


        tinyobj::material_t default_material;
        LoboMesh* lobomesh_binding;

    };
}  // namespace Lobo