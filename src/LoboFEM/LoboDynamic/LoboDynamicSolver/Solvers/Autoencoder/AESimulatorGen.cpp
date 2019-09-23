#include "AESimulatorGen.h"
#include "LoboDynamic/WarpModel/ModalWarpingModel.h"

Lobo::AESimulatorGen::AESimulatorGen(Lobo::LoboDynamicScene *parent_scene) : ModalWarpingSimulator(parent_scene)
{
    save_record_result = true;
    start_frame = 0;
    end_frame = 1000000;
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

    if (ImGui::Button("reset latenst"))
    {
        latents.setZero();
        updateByLatents();
    }

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

void Lobo::AESimulatorGen::stepForward()
{
    int step = time_integraion->step;
    double scale = std::sin(step * 0.1) * 0.2;
    if (step % 400 > 200)
    {
        scale = 0.0;
    }
    
    if(step%400==399)
    {
        Eigen::Vector3d newdireciont;
        newdireciont.setRandom();
        newdireciont.normalize();
        newdireciont*=5.0;
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