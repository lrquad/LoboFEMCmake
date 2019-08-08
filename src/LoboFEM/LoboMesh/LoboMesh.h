#pragma once
#include "imgui.h"

#include "ObjLoader/tiny_obj_loader.h"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace Lobo {

class LoboMesh {
   public:
    LoboMesh() {}
    LoboMesh(const char* filename) {
        loadObj(filename);
    }
    ~LoboMesh() {}

    virtual void loadObj(const char* filename) {
        std::string warn;
        std::string err;
        fs::path p = filename;
        std::string mtl_base_dir = p.parent_path().string();
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename,mtl_base_dir.c_str());

        if (!warn.empty()) {
            std::cout << warn << std::endl;
        }

        if (!err.empty()) {
            std::cerr << err << std::endl;
        }

        if (!ret) {
            std::cout<<"failed to load obj file..."<<ret<<std::endl;
            std::cout<<filename<<std::endl;
        }
    }

   protected:
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
};

}  // namespace Lobo