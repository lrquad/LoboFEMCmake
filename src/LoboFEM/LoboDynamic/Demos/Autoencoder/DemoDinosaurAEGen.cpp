#include "DemoDinosaurAEGen.h"

void Lobo::DemoDinosaurAEGen::drawImGui()
{
    Lobo::ModalWarpingSovler::drawImGui();

    ImGui::Separator();

    if (ImGui::Button("load decoder"))
    {
        delete ae_diff_model;
        std::string filepaht = Lobo::getPath("NN/decoder.txt");
        std::cout << filepaht << std::endl;
        ae_diff_model = new AEAutoDiffModel<double>(filepaht.c_str(), -0.794608789503259, 1.5678641116341878);
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

void Lobo::DemoDinosaurAEGen::stepForward()
{
    ModalWarpingSovler::stepForward();
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

void Lobo::DemoDinosaurAEGen::exportData()
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

void Lobo::DemoDinosaurAEGen::updateByLatents()
{
    if(ae_diff_model==NULL)
    {
        return;
    }
    int numDoFs = bind_tetMesh->getNumVertex()*3;
    Eigen::VectorXd q(numDoFs);
    ae_diff_model->decoder(latents.data(), q.data());
    bind_tetMesh->updateTetVertices(&q);

}