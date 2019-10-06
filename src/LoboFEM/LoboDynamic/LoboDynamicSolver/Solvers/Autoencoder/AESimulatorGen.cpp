#include "AESimulatorGen.h"
#include "LoboDynamic/WarpModel/ModalWarpingModel.h"
#include "LoboVolumtricMesh/Graph/LoboTetMeshGraph.h"

Lobo::AESimulatorGen::AESimulatorGen(Lobo::LoboDynamicScene *parent_scene) : ModalWarpingSimulator(parent_scene)
{
    save_record_result = true;
    start_frame = 0;
    end_frame = 100000;
    skip_frame = 5;
    ae_diff_model = NULL;
}

void Lobo::AESimulatorGen::runXMLscript(pugi::xml_node &solver_node)
{
    Lobo::ModalWarpingSimulator::runXMLscript(solver_node);

    if (solver_node.attribute("min"))
    {
        mesh_min = solver_node.attribute("min").as_double();
    }
    if (solver_node.attribute("scale"))
    {
        mesh_scale = solver_node.attribute("scale").as_double();
    }
}

void Lobo::AESimulatorGen::drawImGui()
{
    Lobo::ModalWarpingSimulator::drawImGui();

    ImGui::Separator();

    if (ImGui::Button("load decoder"))
    {
        delete ae_diff_model;
        std::string filepaht = Lobo::getPath("NN/decoder.txt");
        std::cout << filepaht << std::endl;
        ae_diff_model = new AEAutoDiffModel<double>(filepaht.c_str(), mesh_min, mesh_scale);
        ae_diff_model->initModel();
        latents.resize(ae_diff_model->getNum_latents());
        latents.setZero();
    }

    if (ImGui::Button("load Data"))
    {
        //load error to tetmesh
        int R = bind_tetMesh->getNumVertex() * 3;
        std::string filepath = Lobo::getPath("NN/error.npy");
        std::string filepath2 = Lobo::getPath("NN/x_test.npy");
        std::string filepath3 = Lobo::getPath("NN/x_test_decoded.npy");

        vertex_data_shape.clear();
        vertex_error_data.clear();

        vertex_deform_shape.clear();
        vertex_deform_data.clear();

        vertex_decoded_shape.clear();
        vertex_decoed_data.clear();


        std::cout << filepath << std::endl;
        npy::LoadArrayFromNumpy(filepath.c_str(), vertex_data_shape, vertex_error_data);

        npy::LoadArrayFromNumpy(filepath2.c_str(), vertex_deform_shape, vertex_deform_data);

        npy::LoadArrayFromNumpy(filepath3.c_str(), vertex_decoded_shape, vertex_decoed_data);

        //bind_tetMesh->updateTetAttri(vertex_data.data(),R,8,3,11);
        v_color.resize(R);
        v_position.resize(R);
        show_data_index = 0;
        show_decoded = false;
    }

    if (ImGui::Button("reset latenst"))
    {
        latents.setZero();
        updateByLatents();
    }

    if (vertex_data_shape.size() > 0)
    {
        if (ImGui::DragInt("ShowIndex", &show_data_index, 1.0, 0, vertex_data_shape[0])||ImGui::Checkbox("Decoded:",&show_decoded))
        {
            int R = bind_tetMesh->getNumVertex() * 3;
            v_color.setZero();
            for (int i = 0; i < R / 3; i++)
            {
                int nodeid = i;
                double mean = vertex_error_data.data()[R * show_data_index + nodeid * 3 + 0] + vertex_error_data.data()[R * show_data_index + nodeid * 3 + 1] + vertex_error_data.data()[R * show_data_index + nodeid * 3 + 2];
                mean /= 3.0;
                v_color.data()[nodeid * 3 + 0] = mean / 0.1;

                if (!show_decoded)
                {
                    v_position.data()[nodeid * 3 + 0] = vertex_deform_data.data()[R * show_data_index + nodeid * 3 + 0];
                    v_position.data()[nodeid * 3 + 1] = vertex_deform_data.data()[R * show_data_index + nodeid * 3 + 1];
                    v_position.data()[nodeid * 3 + 2] = vertex_deform_data.data()[R * show_data_index + nodeid * 3 + 2];
                }else
                {
                    v_position.data()[nodeid * 3 + 0] = vertex_decoed_data.data()[R * show_data_index + nodeid * 3 + 0];
                    v_position.data()[nodeid * 3 + 1] = vertex_decoed_data.data()[R * show_data_index + nodeid * 3 + 1];
                    v_position.data()[nodeid * 3 + 2] = vertex_decoed_data.data()[R * show_data_index + nodeid * 3 + 2];
                }
                
            }
            bind_tetMesh->updateTetAttri(v_color.data(), R, 8, 3, 11);
            bind_tetMesh->updateTetVertices(&v_position);
        }
    }

    ImGui::Separator();

    if (ae_diff_model != NULL)
    {
        int num_latents = ae_diff_model->getNum_latents();
        for (int i = 0; i < num_latents; i++)
        {
            ImGui::PushID(i);
            if (ImGui::DragFloat("latents", &(latents.data()[i]), 0.05f))
            {
                updateByLatents();
            }

            ImGui::PopID();
        }
    }
}

void Lobo::AESimulatorGen::precompute()
{
    Lobo::ModalWarpingSimulator::precompute();
    
    //compute weight
    LoboTetMeshGraph* graph = new LoboTetMeshGraph(bind_tetMesh);
    graph->init();

    this->constrainmodel;
    
    
    
    delete graph;


}

void Lobo::AESimulatorGen::stepForward()
{
    int step = time_integraion->step;
    double scale = std::sin(step * 0.1) * 0.2;
    if (step % 600 > 200)
    {
        scale = 0.0;
    }

    if (step % 600 == 599)
    {
        Eigen::Vector3d newdireciont;
        newdireciont.setRandom();
        newdireciont.data()[1] -= 1.0;
        newdireciont.normalize();
        newdireciont *= 11.0;
        kinetic_model->computeFiledForce(newdireciont);
    }

    kinetic_model->external_forces = kinetic_model->gravity_force * scale;

    kinetic_model->external_forces += bind_tetMesh->tet_vertice_force * 0.01;

    modal_warping_model->warpForce(kinetic_model->external_forces, time_integraion->q, true);
    time_integraion->stepFoward();

    Eigen::VectorXd warped_q = time_integraion->q;
    modal_warping_model->warp(warped_q);
    bind_tetMesh->updateTetVertices(&(warped_q));
    //store the result
    int time_step = this->time_integraion->step;

    if (time_step >= start_frame && time_step < end_frame)
    {

        if (time_step % skip_frame == 0)
        {
            Eigen::VectorXd q = bind_tetMesh->tet_vertice - bind_tetMesh->ori_tet_vertice;
            data.push_back(q);
        }
    }

    if (time_step == end_frame)
    {
        exportData();
    }
}

void Lobo::AESimulatorGen::exportData()
{
    int num_data = data.size();
    int num_dofs = bind_tetMesh->tet_vertice.size();
    std::vector<double> export_data(num_data * num_dofs);
    for (int i = 0; i < num_data; i++)
    {
        for (int j = 0; j < num_dofs; j++)
        {
            export_data.data()[i * num_dofs + j] = data[i].data()[j];
        }
    }
    std::string filepaht = Lobo::getPath("data.npy");
    std::cout << filepaht << std::endl;
    const long unsigned leshape[] = {num_data, num_dofs};
    npy::SaveArrayAsNumpy(filepaht.c_str(), false, 2, leshape, export_data);
}

void Lobo::AESimulatorGen::updateByLatents()
{
    if (ae_diff_model == NULL)
    {
        return;
    }
    int numDoFs = bind_tetMesh->getNumVertex() * 3;
    Eigen::VectorXd q(numDoFs);
    ae_diff_model->decoder(latents.data(), q.data());
    bind_tetMesh->updateTetVertices(&q);
}