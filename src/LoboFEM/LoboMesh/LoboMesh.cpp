#include "LoboMesh.h"
#include <iostream>
#include "imgui.h"
#include <experimental/filesystem>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace fs = std::experimental::filesystem;

Lobo::LoboMesh::~LoboMesh()
{
    this->deleteGL();
}

void Lobo::LoboMesh::drawImGui(bool *p_open)
{
    static float f = 0.0f;
    static int counter = 0;
    ImGui::Begin("Obj mesh", p_open, ImGuiWindowFlags_AlwaysAutoResize); // Create a window called "Hello, world!" and append into it.
    ImGui::Text("File name: %s ", obj_file_name.c_str());

    ImGui::Text("num vertices: %d ", attrib.vertices.size() / 3);
    ImGui::Text("num normals: %d ", attrib.normals.size() / 3);
    ImGui::Text("num texcoords: %d ", attrib.texcoords.size() / 2);

    ImGui::End();
}

void Lobo::LoboMesh::loadObj(const char *filename, bool verbose)
{
    std::string warn;
    std::string err;
    fs::path p = filename;
    std::string mtl_base_dir = p.parent_path().string();
    obj_file_name = p.filename().string();
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

void Lobo::LoboMesh::initialGL()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glinitialized = true;
}

void Lobo::LoboMesh::paintGL()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Lobo::LoboMesh::deleteGL()
{
    if (glinitialized == true)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glinitialized = false;
    }
}