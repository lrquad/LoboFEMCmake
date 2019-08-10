#pragma once

#include "ObjLoader/tiny_obj_loader.h"

namespace Lobo
{

static float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

static unsigned int indices[] = {
    // note that we start from 0!
    0, 1, 3, // first Triangle
    1, 2, 3  // second Triangle
};

class LoboMesh
{
public:
    LoboMesh();
    LoboMesh(const char *filename);
    ~LoboMesh();

    virtual void loadObj(const char *filename, bool verbose = false);
    virtual void drawImGui(bool *p_open = NULL);

    virtual void initialGL();
    virtual void paintGL();
    virtual void deleteGL();

protected:

    std::string obj_file_name;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    //GL
    unsigned int VBO; //vertex buffer
    unsigned int VAO; //vertex array buffer
    unsigned int EBO; //element buffer

    bool wireframe_mode;

    bool glinitialized;

    virtual void defaultValue();

};

} // namespace Lobo