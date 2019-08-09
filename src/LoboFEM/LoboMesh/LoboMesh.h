#pragma once

#include "ObjLoader/tiny_obj_loader.h"

namespace Lobo
{

static float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

class LoboMesh
{
public:
    LoboMesh() {}
    LoboMesh(const char *filename)
    {
        loadObj(filename);
    }
    ~LoboMesh() {}

    virtual void loadObj(const char *filename, bool verbose = false);
    virtual void drawImGui(bool *p_open = NULL);

    virtual void initialGL();
    virtual void paintGL();

protected:


    std::string obj_file_name;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    //GL
    unsigned int VBO;

};

} // namespace Lobo