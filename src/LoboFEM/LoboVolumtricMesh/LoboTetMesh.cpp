#include "LoboTetMesh.h"
#include <igl/copyleft/tetgen/tetrahedralize.h>
#include <igl/unproject_onto_mesh.h>
#include "Functions/EigenMatrixIO.h"
#include "LoboDynamic/LoboDynamicScene.h"
#include "LoboMesh/LoboMesh.h"
#include "imgui.h"
#include "LoboImGui/cpp/imgui_stdlib.h"
#include "OpenGLutils/glfunctions.h"
#include "OpenGLutils/LoboCamera.h"
#include "Utils/glmEigenConverter.h"

Lobo::LoboTetMesh::LoboTetMesh()
{
    initializedGL = false;
    tetgen_command = "pq1.414";
    status_flags = 0;
    default_material.diffuse[0] = 1.0;
    default_material.diffuse[1] = 0.3;
    default_material.diffuse[2] = 0.3;
    shader_config.wireframe_mode = true;
    lobomesh_binding = NULL;
}

Lobo::LoboTetMesh::~LoboTetMesh() {}

void Lobo::LoboTetMesh::drawImGui(bool *p_open)
{
    if (ImGui::CollapsingHeader(filebase.c_str(),
                                ImGuiWindowFlags_NoCollapse))
    {
        // static char str0[128] = tetgen_command;
        // ImGui::InputText("Tet file base", str0, IM_ARRAYSIZE(str0));
        // ImGui::InputText("Tet gen command", tetgen_command.data(),
        // IM_ARRAYSIZE(str0));
        ImGui::Text("InitialGl %s", status_flags & TetMeshStatusFlags_initialGL
                                        ? "true"
                                        : "false");
        ImGui::Text("datasize changed %s", status_flags & TetMeshStatusFlags_datasizeUpdated
                                       ? "true"
                                       : "false");
        ImGui::Text("Tetgened %s", status_flags & TetMeshStatusFlags_tetgened
                                       ? "true"
                                       : "false");
        ImGui::Text("Loadedgen %s", status_flags & TetMeshStatusFlags_loadtet
                                        ? "true"
                                        : "false");

        ImGui::InputText("Tet file base ", &filebase);
        ImGui::InputText("tetgen_command ", &tetgen_command);

        if (ImGui::Button("Generate Tet"))
        {
            this->generateTet(tetgen_command.c_str());
        };
        ImGui::SameLine();
        if (ImGui::Button("Save Tet"))
        {
            if (usebinary)
            {
                exportTetMeshBinary(Lobo::getPath(filebase.c_str()).c_str());
            }
            else
            {
                exportTetMeshAscii(Lobo::getPath(filebase.c_str()).c_str());
            }
        };
        ImGui::SameLine();
        if (ImGui::Button("load Tet"))
        {
            if (usebinary)
            {
                loadTetMeshBinary(Lobo::getPath(filebase.c_str()).c_str());
            }
            else
            {
                loadTetMeshAscii(Lobo::getPath(filebase.c_str()).c_str());
            }
        };

        if (status_flags &
            (TetMeshStatusFlags_tetgened | TetMeshStatusFlags_loadtet))
        {
            ImGui::Text("tet_vertice %d tet_num_tet %d", tet_vertice.rows() / 3,
                        tet_indices.rows() / 4);
        }

        if (ImGui::Button("hide trimesh"))
        {
            if(lobomesh_binding!=NULL)
            {
                lobomesh_binding->shader_config.visiable=!lobomesh_binding->shader_config.visiable;
            }
        }

        if (ImGui::TreeNodeEx("TetMeshConfig##1",
                              ImGuiWindowFlags_NoCollapse))
        {
            ImGui::Checkbox("usebinary", &usebinary);
            ImGui::TreePop();
            ImGui::Separator();
        }

        shader_config.drawImGui();
       
    }

    //mouse click
    if (ImGui::IsMouseReleased(1))
    {
        mouseSelection();
    }
}

void Lobo::LoboTetMesh::mouseSelection()
{
    if (status_flags &
        (TetMeshStatusFlags_tetgened | TetMeshStatusFlags_loadtet))
    {
        ImGuiIO &io = ImGui::GetIO();
        Lobo::Camera *current_camera = Lobo::getCurrentCamera();
        Eigen::Vector4f view_port = Lobo::GLM_2_E<float, 4>(current_camera->view_port);
        Eigen::Matrix4f view_m = Lobo::GLM_2_E<float, 4>(current_camera->view_matrix);
        Eigen::Matrix4f project_m = Lobo::GLM_2_E<float, 4>(current_camera->projection_matrix);

        Eigen::Vector3f bc;
        int fid;
        // Cast a ray in the view direction starting from the mouse position
        double x = io.MousePos.x;
        double y = view_port.data()[3] - io.MousePos.y;
        if (igl::unproject_onto_mesh(Eigen::Vector2f(x, y), view_m,
                                     project_m, view_port, tet_vertice_col, tet_faces_col, fid, bc))
        {
            for (int i = 0; i < 3; i++)
            {
                int vid = tet_faces.data()[fid * 3 + i];
                setTetVetAttriColor(vid, 1.0, 0.0, 0.0);
            }
        }
    }
}

void Lobo::LoboTetMesh::paintGL(LoboShader *shader)
{
    if (!(status_flags & TetMeshStatusFlags_initialGL))
    {
        return;
    }
    if (status_flags & TetMeshStatusFlags_datasizeUpdated)
    {
        // need update
        updateGL();
    }

    shader_config.setShader(shader);
    if (shader_config.visiable == false)
    {
        return;
    }

    glm::vec3 diffuse_color =
        glm::vec3(default_material.diffuse[0], default_material.diffuse[1],
                  default_material.diffuse[2]);
    glm::vec3 ambient_color =
        glm::vec3(default_material.ambient[0], default_material.ambient[1],
                  default_material.ambient[2]);
    glm::vec3 specular_color =
        glm::vec3(default_material.specular[0], default_material.specular[1],
                  default_material.specular[2]);
    // shader->setVec3("diffuse_color", color);
    shader->setBool("useDiffuseTex", false);
    shader->setVec3("material.ambient", ambient_color);
    shader->setVec3("material.diffuse", diffuse_color);
    shader->setVec3("material.specular", specular_color);
    shader->setFloat("material.shininess", 32.0f);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    sizeof(float) * tet_vertice_attri.size(),
                    tet_vertice_attri.data());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    setPositionAttribute(0, 3, 11, 0);
    setPositionAttribute(1, 3, 11, 3);
    setPositionAttribute(2, 2, 11, 6);
    setPositionAttribute(3, 3, 11, 8);

    glDrawElements(GL_TRIANGLES, tet_faces_glint.size(), GL_UNSIGNED_INT, 0);
}

void Lobo::LoboTetMesh::initialGL()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // glBindVertexArray(VAO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    initializedGL = true;
    status_flags |= TetMeshStatusFlags_initialGL;
}

void Lobo::LoboTetMesh::updateGL()
{
    if (!(status_flags & TetMeshStatusFlags_datasizeUpdated))
    {
        // no need updateGL
        return;
    }

    tet_vertice_col = Lobo::eigen_vec_2_mat(tet_vertice, tet_vertice.size() / 3, 3);
    tet_faces_col = Lobo::eigen_vec_2_mat(tet_faces,tet_faces.size() / 3, 3);

    tet_vertice_attri.resize(tet_vertice.size() / 3 * 11);
    tet_vertice_attri.setZero();
    setTetAttriColor(0.8, 0.8, 0.8);
    tet_faces_glint.resize(tet_faces.size());
    for (int i = 0; i < tet_faces.size(); i++)
    {
        tet_faces_glint[i] = tet_faces[i];
    }
    updateTetAttri(tet_vertice, 0, 3, 11);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tet_vertice_attri.size(),
                 tet_vertice_attri.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * tet_faces_glint.size(),
                 tet_faces_glint.data(), GL_STATIC_DRAW);

    setPositionAttribute(0, 3, 11, 0);
    setPositionAttribute(1, 3, 11, 3);
    setPositionAttribute(2, 2, 11, 6);
    setPositionAttribute(3, 3, 11, 8);

    status_flags &= ~TetMeshStatusFlags_datasizeUpdated;
}

void Lobo::LoboTetMesh::generateTet(const char *tetgen_command)
{
    std::string command_ = "pq1.414Y";
    if (tetgen_command != NULL)
    {
        command_ = tetgen_command;
    }
    Eigen::MatrixXd TV;
    Eigen::MatrixXi TT;

    int result = igl::copyleft::tetgen::tetrahedralize(
        tri_vertices, tri_faces, command_.c_str(), TV, TT, tet_faces_col);

    //test
    //Lobo::exportSimpleObj("test.obj",TV,TF);

    // copy data

    if (result == 0)
    {
        tet_vertice.resize(TV.rows() * TV.cols());
        for (int i = 0; i < TV.rows(); i++)
        {
            for (int j = 0; j < TV.cols(); j++)
            {
                tet_vertice.data()[i * TV.cols() + j] =
                    TV.data()[j * TV.rows() + i];
            }
        }
        tet_indices.resize(TT.rows() * TT.cols());
        for (int i = 0; i < TT.rows(); i++)
        {
            for (int j = 0; j < TT.cols(); j++)
            {
                // one tet has 4 vertices
                tet_indices.data()[i * TT.cols() + j] =
                    TT.data()[j * TT.rows() + i];
            }
        }

        tet_faces.resize(tet_faces_col.rows() * tet_faces_col.cols());
        for (int i = 0; i < tet_faces_col.rows(); i++)
        {
            for (int j = 0; j < tet_faces_col.cols(); j++)
            {
                tet_faces.data()[i * tet_faces_col.cols() + j] =
                    tet_faces_col.data()[j * tet_faces_col.rows() + i];
            }
        }

        status_flags |= TetMeshStatusFlags_datasizeUpdated;
        status_flags |= TetMeshStatusFlags_tetgened;
    }
    else
    {
        std::cout << "tetgen failed" << command_ << filebase << std::endl;
    }
}

 void Lobo::LoboTetMesh::setBindingTriMesh(LoboMesh* lobomesh)
 {
     this->lobomesh_binding = lobomesh;
 }

void Lobo::LoboTetMesh::setInputPolygon(LoboMesh *lobomesh)
{
    // set vertices
    int num_tri_vertices = lobomesh->attrib.vertices.size() / 3;
    tri_vertices.resize(num_tri_vertices, 3);

    for (int i = 0; i < num_tri_vertices; i++)
    {
        tri_vertices.data()[i] = lobomesh->attrib.vertices[i * 3 + 0];
        tri_vertices.data()[i + num_tri_vertices] =
            lobomesh->attrib.vertices[i * 3 + 1];
        tri_vertices.data()[i + 2 * num_tri_vertices] =
            lobomesh->attrib.vertices[i * 3 + 2];
    }
    int num_tri_faces = lobomesh->num_faces;
    tri_faces.resize(num_tri_faces, 3);
    int face_index_slid = 0;
    for (int i = 0; i < lobomesh->shapes.size(); i++)
    {
        for (int j = 0; j < lobomesh->shapes[i].mesh.indices.size() / 3; j++)
        {
            tri_faces.data()[face_index_slid] =
                lobomesh->shapes[i].mesh.indices[j * 3].vertex_index;
            tri_faces.data()[face_index_slid + num_tri_faces] =
                lobomesh->shapes[i].mesh.indices[j * 3 + 1].vertex_index;
            tri_faces.data()[face_index_slid + 2 * num_tri_faces] =
                lobomesh->shapes[i].mesh.indices[j * 3 + 2].vertex_index;
            face_index_slid += 1;
        }
    }

    //test obj
    //Lobo::exportSimpleObj("test.obj",tri_vertices,tri_faces);
}

void Lobo::LoboTetMesh::setInputPolygon(Eigen::VectorXd *vertices,
                                        Eigen::VectorXi *faces)
{
    tri_vertices.resize(vertices->rows(), vertices->cols());
    memcpy(tri_vertices.data(), vertices->data(),
           sizeof(double) * vertices->rows() * vertices->cols());
    tri_faces.resize(faces->rows(), faces->cols());
    memcpy(tri_faces.data(), faces->data(),
           sizeof(int) * faces->rows() * faces->cols());
}

void Lobo::LoboTetMesh::loadTetMeshBinary(const char *filename)
{
    std::cout << "loadTetMeshBinary " << filebase << std::endl;

    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in.good())
    {
        std::cout << filename << "file not open" << std::endl;
        return;
    }
    EigenMatrixIO::read_binary(in, tet_vertice);
    EigenMatrixIO::read_binary(in, tet_indices);
    EigenMatrixIO::read_binary(in, tet_faces);
    in.close();
    status_flags |= TetMeshStatusFlags_loadtet;
    status_flags |= TetMeshStatusFlags_datasizeUpdated;
}

void Lobo::LoboTetMesh::exportTetMeshBinary(const char *filename)
{
    if (!(status_flags &
          (TetMeshStatusFlags_tetgened | TetMeshStatusFlags_loadtet)))
    {
        return;
    }

    std::cout << "exportTetMeshBinary " << filename << std::endl;

    std::ofstream out(filename,
                      std::ios::out | std::ios::binary | std::ios::trunc);
    EigenMatrixIO::write_binary(out, tet_vertice);
    EigenMatrixIO::write_binary(out, tet_indices);
    EigenMatrixIO::write_binary(out, tet_faces);
    out.close();
}

void Lobo::LoboTetMesh::loadTetMeshAscii(const char *filebase)
{
    std::cout << "loadTetMeshAscii " << filebase << std::endl;

    std::ostringstream stringStream;
    stringStream << filebase << ".ele";
    std::string elementfile = stringStream.str();
    stringStream.str("");
    stringStream.clear();
    stringStream << filebase << ".node";
    std::string nodefile = stringStream.str();
    stringStream.str("");
    stringStream.clear();
    stringStream << filebase << ".face";
    std::string facefile = stringStream.str();

    int tmp;
    int numele, numvet, numface;
    std::ifstream inputstream(elementfile);
    inputstream >> numele >> tmp >> tmp;
    tet_indices.resize(numele * 4);
    for (int i = 0; i < numele; i++)
    {
        inputstream >> tmp;
        for (int j = 0; j < 4; j++)
        {
            inputstream >> tet_indices.data()[i * 4 + j];
        }
    }
    inputstream.close();

    inputstream.open(nodefile);
    inputstream >> numvet >> tmp >> tmp >> tmp;
    tet_vertice.resize(numvet * 3);
    for (int i = 0; i < numvet; i++)
    {
        inputstream >> tmp;
        for (int j = 0; j < 3; j++)
        {
            inputstream >> tet_vertice.data()[i * 3 + j];
        }
    }
    inputstream.close();

    inputstream.open(facefile);
    inputstream >> numface;
    tet_faces.resize(numface * 3);
    for (int i = 0; i < numface; i++)
    {
        inputstream >> tmp;
        for (int j = 0; j < 3; j++)
        {
            inputstream >> tet_faces.data()[i * 3 + j];
        }
    }
    inputstream.close();
    status_flags |= TetMeshStatusFlags_datasizeUpdated;
    status_flags |= TetMeshStatusFlags_loadtet;
}
void Lobo::LoboTetMesh::exportTetMeshAscii(const char *filebase)
{
    if (!(status_flags &
          (TetMeshStatusFlags_tetgened | TetMeshStatusFlags_loadtet)))
    {
        return;
    }
    std::cout << "exportTetMeshAscii " << filebase << std::endl;
    std::ostringstream stringStream;
    stringStream << filebase << ".ele";
    std::string elementfile = stringStream.str();
    stringStream.str("");
    stringStream.clear();
    stringStream << filebase << ".node";
    std::string nodefile = stringStream.str();
    stringStream.str("");
    stringStream.clear();
    stringStream << filebase << ".face";
    std::string facefile = stringStream.str();

    std::ofstream outstream(elementfile);
    outstream << tet_indices.size() / 4 << " 4 0 " << std::endl;
    for (int i = 0; i < tet_indices.rows() / 4; i++)
    {
        outstream << i << " ";
        for (int j = 0; j < 4; j++)
        {
            outstream << tet_indices.data()[i * 4 + j] << " ";
        }
        outstream << std::endl;
    }
    outstream.close();

    outstream.open(nodefile);
    outstream << tet_vertice.size() / 3 << " 3 0 0 " << std::endl;
    for (int i = 0; i < tet_vertice.rows() / 3; i++)
    {
        outstream << i << " ";
        for (int j = 0; j < 3; j++)
        {
            outstream << tet_vertice.data()[i * 3 + j] << " ";
        }
        outstream << std::endl;
    }
    outstream.close();
    outstream.open(facefile);
    outstream << tet_faces.size() / 3 << std::endl;
    for (int i = 0; i < tet_faces.rows() / 3; i++)
    {
        outstream << i << " ";
        for (int j = 0; j < 3; j++)
        {
            outstream << tet_faces.data()[i * 3 + j] << " ";
        }
        outstream << std::endl;
    }
    outstream.close();
}

void Lobo::LoboTetMesh::updateTetAttri(Eigen::VectorXd &inputattri, int offset,
                                       int attrisize, int totalsize)
{
    for (int i = 0; i < inputattri.size() / attrisize; i++)
    {
        for (int j = 0; j < attrisize; j++)
            tet_vertice_attri.data()[i * totalsize + offset + j] =
                inputattri.data()[i * attrisize + j];
    }
}

void Lobo::LoboTetMesh::setTetAttriColor(double r, double g, double b, int offset, int totalsize)
{
    for (int i = 0; i < tet_vertice_attri.size() / totalsize; i++)
    {
        tet_vertice_attri.data()[i * totalsize + offset + 0] = r;
        tet_vertice_attri.data()[i * totalsize + offset + 1] = g;
        tet_vertice_attri.data()[i * totalsize + offset + 2] = b;
    }
}

void Lobo::LoboTetMesh::setTetVetAttriColor(int vid, double r, double g, double b, int offset, int totalsize)
{
    tet_vertice_attri.data()[vid * totalsize + offset + 0] = r;
    tet_vertice_attri.data()[vid * totalsize + offset + 1] = g;
    tet_vertice_attri.data()[vid * totalsize + offset + 2] = b;
}