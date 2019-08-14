#include "LoboMesh.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <experimental/filesystem>
#include <iostream>
#include "MeshControl.h"
#include "OpenGLutils/glfunctions.h"
#include "Shaders/LoboShader.h"
#include "imgui.h"
#include "stb_image.h"

namespace fs = std::experimental::filesystem;

Lobo::LoboMesh::LoboMesh() { defaultValue(); }

Lobo::LoboMesh::LoboMesh(const char *filename) {
    loadObj(filename);
    defaultValue();
}

Lobo::LoboMesh::~LoboMesh() { this->deleteGL(); }

void Lobo::LoboMesh::defaultValue() {
    wireframe_mode = false;
    glinitialized = false;
    start_show_material = 0;
}

void Lobo::LoboMesh::drawImGui(bool *p_open) {
    static float f = 0.0f;
    static int counter = 0;
    ImGui::Begin(
        "Obj mesh",
        p_open);  // Create a window called "Hello, world!" and append into it.

    // ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
    // ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

    ImGui::Text("File name: %s ", obj_file_name.c_str());

    if (ImGui::CollapsingHeader("Info", ImGuiWindowFlags_NoCollapse)) {
        ImGui::Text("num vertices: %d ", attrib.vertices.size() / 3);
        ImGui::Text("num normals: %d ", attrib.normals.size() / 3);
        ImGui::Text("num texcoords: %d ", attrib.texcoords.size() / 2);
        ImGui::Text("num shapes: %d ", shapes.size());
        ImGui::Text("num Materials: %d ", materials.size());

        if (ImGui::CollapsingHeader("Materials", ImGuiWindowFlags_NoCollapse)) {
            ImGui::InputInt("Material start at", &start_show_material, 1, 5);
            if (start_show_material < 0) {
                start_show_material = 0;
            }
            start_show_material %= materials.size() - 3;

            for (size_t i = start_show_material; i < start_show_material + 3;
                 i++) {
                if (i > materials.size() - 1) {
                    break;
                }
                ImGui::PushID(i);
                ImGui::Text("material[%d].diffuse_texname: %s", int(i),
                            materials[i].diffuse_texname.c_str());
                ImGui::ColorEdit3("ambient color", &(materials[i].ambient)[0]);
                ImGui::ColorEdit3("diffuse color", &(materials[i].diffuse)[0]);
                ImGui::ColorEdit3("specular color",&(materials[i].specular)[0]);
                ImGui::DragFloat("shininess", &materials[i].shininess, 0.05f,0.0,100.0);

                ImGui::PopID();
            }
        }
    }

    if (ImGui::CollapsingHeader("Configuration", ImGuiWindowFlags_NoCollapse)) {
        ImGui::Checkbox("wireframe_mode", &wireframe_mode);
    }

    ImGui::End();
}

void Lobo::LoboMesh::loadObj(const char *filename, bool verbose) {
    std::string warn;
    std::string err;
    fs::path p = filename;
    std::string mtl_base_dir = p.parent_path().string();
    obj_file_name = p.filename().string();
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                filename, mtl_base_dir.c_str());

    // initial vertex color
    vertex_color.resize(attrib.vertices.size());
    std::fill(vertex_color.begin(), vertex_color.end(), 0.0);

    // Append `default` material
    materials.push_back(tinyobj::material_t());
    materials.back().diffuse[0] = 1.0;
    materials.back().diffuse[1] = 0.3;
    materials.back().diffuse[2] = 0.3;

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        std::cout << "failed to load obj file..." << ret << std::endl;
        std::cout << filename << std::endl;
    }
    uniformMesh();
}

void Lobo::LoboMesh::uniformMesh() {
    tinyobj::attrib_t output = attrib;
    Eigen::Vector3d origin_center;
    Lobo::centerTinyAttribute(attrib, output, origin_center);
    attrib = output;
    Lobo::uniformTinyAttribute(attrib, output);
    attrib = output;
    Lobo::updateSmoothNorm(attrib, shapes);
}

void Lobo::LoboMesh::initialGL() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    int num_shapes = shapes.size();
    shape_buffer.resize(num_shapes);

    for (int i = 0; i < num_shapes; i++) {
        shape_buffer[i].size_per_vertex = 11;
        int buffersize =
            shapes[i].mesh.indices.size() * (shape_buffer[i].size_per_vertex);
        shape_buffer[i].vb.resize(buffersize);  // pos(3 float) normal(3 float),
                                                // tex(2 float), color(3 float)
        // convert buffer
        updateShapeArrayBuffer(i);

        shape_buffer[i].material_id = (shapes)[i].mesh.material_ids[0];

        glGenBuffers(1, &shape_buffer[i].VBO);
        glBindBuffer(GL_ARRAY_BUFFER, shape_buffer[i].VBO);
        glBufferData(GL_ARRAY_BUFFER, shape_buffer[i].vb.size() * sizeof(float),
                     &(shape_buffer[i].vb)[0], GL_STATIC_DRAW);
        setPositionAttribute(0, 3, 11, 0);
        setPositionAttribute(1, 3, 11, 3);
        setPositionAttribute(2, 2, 11, 6);
        setPositionAttribute(3, 3, 11, 8);
        // load texture
    }

    material_buffer.resize(materials.size());
    for (size_t m = 0; m < materials.size(); m++) {
        tinyobj::material_t *mp = &materials[m];
        if (mp->diffuse_texname.length() > 0) {
            material_buffer[m].diffuse_texture = true;
            // diffuse texture
            glGenTextures(1, &material_buffer[m].diffuse_texid);
            bindTextureBuffer(mp->diffuse_texname.c_str(),
                              material_buffer[m].diffuse_texid);
            material_buffer[m].diffuse_texname = mp->diffuse_texname;
        }
    }

    glBindVertexArray(0);

    // Lobo::bindShapeBuffer(VBO,EBO,buffer,indices_b);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
    // GL_STATIC_DRAW);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
    // GL_STATIC_DRAW);

    // position attribute
    // setPositionAttribute(0,3,8,0);
    // setPositionAttribute(1,3,8,3);
    // setPositionAttribute(2,2,8,6);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void
    // *)0); glEnableVertexAttribArray(0);
    // // color attribute
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void
    // *)(3 * sizeof(float))); glEnableVertexAttribArray(1);

    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void
    // *)(6 * sizeof(float))); glEnableVertexAttribArray(2);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

    // texture
    // glGenTextures(1, &texture);
    // bindTextureBuffer("./models/earth/4096_night_lights.jpg",texture);
    glinitialized = true;
}

void Lobo::LoboMesh::updateShapeArrayBuffer(int shape_id) {
    int size_per_vertex = shape_buffer[shape_id].size_per_vertex;
    for (int j = 0; j < shapes[shape_id].mesh.indices.size(); j++) {
        int vid = shapes[shape_id].mesh.indices[j].vertex_index;
        shape_buffer[shape_id].vb[j * size_per_vertex + 0] =
            attrib.vertices[vid * 3 + 0];
        shape_buffer[shape_id].vb[j * size_per_vertex + 1] =
            attrib.vertices[vid * 3 + 1];
        shape_buffer[shape_id].vb[j * size_per_vertex + 2] =
            attrib.vertices[vid * 3 + 2];

        shape_buffer[shape_id].vb[j * size_per_vertex + 8] =
            vertex_color[vid * 3 + 0];
        shape_buffer[shape_id].vb[j * size_per_vertex + 9] =
            vertex_color[vid * 3 + 1];
        shape_buffer[shape_id].vb[j * size_per_vertex + 10] =
            vertex_color[vid * 3 + 2];

        int nid = shapes[shape_id].mesh.indices[j].normal_index;

        if (nid != -1) {
            shape_buffer[shape_id].vb[j * size_per_vertex + 3] =
                attrib.normals[nid * 3 + 0];
            shape_buffer[shape_id].vb[j * size_per_vertex + 4] =
                attrib.normals[nid * 3 + 1];
            shape_buffer[shape_id].vb[j * size_per_vertex + 5] =
                attrib.normals[nid * 3 + 2];
        }

        int tid = shapes[shape_id].mesh.indices[j].texcoord_index;

        if (tid == -1) {
            shape_buffer[shape_id].vb[j * size_per_vertex + 6] = 0;
            shape_buffer[shape_id].vb[j * size_per_vertex + 7] = 0;
            // continue;
        } else {
            shape_buffer[shape_id].vb[j * size_per_vertex + 6] =
                attrib.texcoords[tid * 2 + 0];
            shape_buffer[shape_id].vb[j * size_per_vertex + 7] =
                attrib.texcoords[tid * 2 + 1];
        }
    }
}

void Lobo::LoboMesh::paintGL(LoboShader *shader) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (wireframe_mode == true) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    for (int i = 0; i < shape_buffer.size(); i++) {
        int materia_id = shape_buffer[i].material_id;
        if (material_buffer[i].diffuse_texture == true) {
            unsigned int texture_id = material_buffer[materia_id].diffuse_texid;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_id);
        } else {
            glm::vec3 diffuse_color =
                glm::vec3(materials[materia_id].diffuse[0],
                          materials[materia_id].diffuse[1],
                          materials[materia_id].diffuse[2]);
            glm::vec3 ambient_color =
                glm::vec3(materials[materia_id].ambient[0],
                          materials[materia_id].ambient[1],
                          materials[materia_id].ambient[2]);
            glm::vec3 specular_color =
                glm::vec3(materials[materia_id].specular[0],
                          materials[materia_id].specular[1],
                          materials[materia_id].specular[2]);

            // shader->setVec3("diffuse_color", color);
            shader->setVec3("material.ambient", ambient_color);
            shader->setVec3("material.diffuse", diffuse_color);
            shader->setVec3("material.specular", specular_color);
            shader->setFloat("material.shininess", 32.0f);
        }
        glBindBuffer(GL_ARRAY_BUFFER, shape_buffer[i].VBO);
        setPositionAttribute(0, 3, 11, 0);
        setPositionAttribute(1, 3, 11, 3);
        setPositionAttribute(2, 2, 11, 6);
        setPositionAttribute(3, 3, 11, 8);
        glDrawArrays(
            GL_TRIANGLES, 0,
            shape_buffer[i].vb.size() / shape_buffer[i].size_per_vertex);
    }
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Lobo::LoboMesh::deleteGL() {
    if (glinitialized == true) {
        glDeleteVertexArrays(1, &VAO);
        for (int i = 0; i < shape_buffer.size(); i++) {
            glDeleteBuffers(1, &shape_buffer[i].VBO);
        }

        glinitialized = false;
    }
}