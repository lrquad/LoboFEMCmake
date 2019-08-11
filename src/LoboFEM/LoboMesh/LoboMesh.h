#pragma once

#include "ObjLoader/tiny_obj_loader.h"

namespace Lobo
{

//test data info

static float vertices[] = {
    // positions          // colors           // texture coords
    1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
    1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
};

static unsigned int indices[] = {
    // note that we start from 0!
    0, 1, 3, // first Triangle
    1, 2, 3  // second Triangle
};

static float texCoords[] = {
    0.0f, 0.0f, // lower-left corner
    1.0f, 0.0f, // lower-right corner
    0.5f, 1.0f  // top-center corner
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

    unsigned int texture;

    bool wireframe_mode;

    bool glinitialized;

    virtual void defaultValue();
};

} // namespace Lobo