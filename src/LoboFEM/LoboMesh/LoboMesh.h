#pragma once
#include "imgui.h"

#include <experimental/filesystem>
#include "ObjLoader/tiny_obj_loader.h"

namespace fs = std::experimental::filesystem;

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

    virtual void loadObj(const char *filename, bool verbose = true)
    {
        std::string warn;
        std::string err;
        fs::path p = filename;
        std::string mtl_base_dir = p.parent_path().string();
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, mtl_base_dir.c_str());

        if (!warn.empty())
        {
            std::cout << warn << std::endl;
        }

        if (!err.empty())
        {
            std::cerr << err << std::endl;
        }

        if (!ret)
        {
            std::cout << "failed to load obj file..." << ret << std::endl;
            std::cout << filename << std::endl;
        }

        if (verbose)
        {
            std::cout << "num vertices: " << attrib.vertices.size() / 3 << std::endl;
            std::cout << "num normals: " << attrib.normals.size() / 3 << std::endl;
            std::cout << "num texcoords: " << attrib.texcoords.size() / 2 << std::endl;
            int num_faces = 0;
            for (size_t s = 0; s < shapes.size(); s++)
            {
                num_faces += shapes[s].mesh.num_face_vertices.size();
            }
            std::cout << "num faces: " << num_faces << std::endl;

            std::cout << "num materials: " << materials.size() << std::endl;
        }
    }

protected:
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
};

} // namespace Lobo