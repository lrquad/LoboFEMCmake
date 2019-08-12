#pragma once

#include "ObjLoader/tiny_obj_loader.h"
#include <map>
#include <glm/glm.hpp>

namespace Lobo
{

struct ShapeBuffer
{
    unsigned int VBO; //vertex array buffer

	std::vector<float> vb;
	int size_per_vertex;
	int material_id;
};

struct MaterialBuffer
{
    unsigned int diffuse_texid; //vertex array buffer
    std::string diffuse_texname;
    //std::map<std::string,unsigned int> disffues_tex;
    bool diffuse_texture;
};

class LoboShader;

class LoboMesh
{
public:
    LoboMesh();
    LoboMesh(const char *filename);
    ~LoboMesh();

    virtual void loadObj(const char *filename, bool verbose = false);
    virtual void uniformMesh();
    virtual void drawImGui(bool *p_open = NULL);

    virtual void initialGL();
    virtual void paintGL(LoboShader* shader);
    virtual void deleteGL();

protected:
    std::string obj_file_name;

    tinyobj::attrib_t attrib;
    std::vector<float> vertex_color;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    //GL
    std::vector<ShapeBuffer> shape_buffer;
    std::vector<MaterialBuffer> material_buffer;
    unsigned int VAO; //vertex array buffer

    bool wireframe_mode;

    bool glinitialized;

    virtual void updateShapeArrayBuffer(int shape_id);
    virtual void defaultValue();
};

} // namespace Lobo