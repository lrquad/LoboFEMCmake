#pragma once
#include "LoboDynamic/LoboDynamic.h"
#include "npy.h"

namespace Lobo
{
class DemoDinosaurAE : public ModalWarpingSovler
{
public:
    DemoDinosaurAE(Lobo::LoboDynamicScene *parent_scene) : ModalWarpingSovler(parent_scene)
    {
        save_record_result = true;
        start_frame = 0;
        end_frame = 800;
        skip_frame = 10;
    }

    virtual void stepForward()
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

    void exportData()
    {
        int num_data = data.size();
        int num_dofs =  bind_tetMesh->tet_vertice.size();
        std::vector<double> export_data(num_data*num_dofs);
        for(int i=0;i<num_data;i++)
        {
            for(int j=0;j<num_dofs;j++)
            {
                export_data.data()[i*num_dofs+j] = data[i].data()[j];
            }
        }
        std::string filepaht = Lobo::getPath("data.npy");
        std::cout<<filepaht<<std::endl;
        const long unsigned leshape [] = {num_data,num_dofs};
        npy::SaveArrayAsNumpy(filepaht.c_str(), false, 2, leshape, export_data);

    }

    std::vector<Eigen::VectorXd> data;
    bool save_record_result;
    int start_frame;
    int end_frame;
    int skip_frame;
};
} // namespace Lobo