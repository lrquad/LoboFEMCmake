#pragma once
#include "ObjLoader/tiny_obj_loader.h"

namespace Lobo
{

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



};

} // namespace Lobo