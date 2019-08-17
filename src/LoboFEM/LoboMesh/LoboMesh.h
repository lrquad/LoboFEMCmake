#pragma once

#include <glm/glm.hpp>
#include <map>
#include "ObjLoader/tiny_obj_loader.h"

namespace Lobo {

struct ShapeBuffer {
    unsigned int VBO;  // vertex array buffer

    std::vector<float> vb;
    int size_per_vertex;
    int material_id;
};

struct MaterialBuffer {
    unsigned int diffuse_texid;  // vertex array buffer
    unsigned int emissive_texid;  // vertex array buffer

    std::string diffuse_texname;
    std::string emissive_texname;
    // std::map<std::string,unsigned int> disffues_tex;
    bool has_diffuse_tex;
    bool has_emissive_tex;
    bool use_diffuse_tex;
};

class LoboShader;

class LoboMesh {
   public:
    LoboMesh();
    LoboMesh(const char *filename);
    ~LoboMesh();

    virtual void loadObj(const char *filename, bool verbose = false);
    virtual void uniformMesh();
    virtual void drawImGui(bool *p_open = NULL);

    virtual void initialGL();
    virtual void updateGLbuffer();
    virtual void paintGL(LoboShader *shader);
    virtual void deleteGL();

   protected:
    std::string obj_file_name;

    tinyobj::attrib_t attrib;
    std::vector<float> vertex_color;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    // GL
    std::vector<ShapeBuffer> shape_buffer;
    std::vector<MaterialBuffer> material_buffer;
    unsigned int VAO;  // vertex array buffer

    //imgui
    bool wireframe_mode;
    int start_show_material;


    bool glinitialized;
    bool bufferNeedUpdate;
    
    virtual void updateShapeArrayBuffer(int shape_id);
    virtual void updateShapeArrayBufferVertices(int shape_id);

    
    //mesh rigid configure;
    glm::vec3 position;
    glm::vec3 eular_angle; //"xyz"

    //dynamic interface
    std::vector<float> ori_vertices; //3n

    virtual void updateRigidTransformation(glm::vec3 position,glm::vec3 eular_angle);

    virtual void updateVertices(float* newPosition);
    virtual void updateVertices(double* newPosition);

    virtual void getCurVertices(float* outPosition);
    virtual void getCurVertices(double* outPosition);

    virtual void resetVertice();
    virtual void defaultValue();


};

}  // namespace Lobo