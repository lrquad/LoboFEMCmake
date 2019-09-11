#pragma once
#include "LoboDynamic/LoboDynamic.h"

namespace Lobo
{
class DemoDinosaurAE : public ModalWarpingSovler
{
public:
    DemoDinosaurAE(Lobo::LoboDynamicScene *parent_scene) : ModalWarpingSovler(parent_scene)
    {
        save_record_result = true;
        start_frame = 0;
        end_frame = 200000;
        skip_frame = 10;
    }

    virtual void stepForward()
    {
        ModalWarpingSovler::stepForward();
        //store the result
        int time_step = his->time_integraion->step;
        if (time_step >= start_frame && time_step < end_frame)
        {

            if (time_step % skip_frame == 0)
            {
                Eigen::VectorXd q = bind_tetMesh->tet_vertice - bind_tetMesh->ori_tet_vertice;
                data.push_back(q);
            }
        }

        if (this->time_integraion->step == end_frame)
        {
            exportData();
        }
    }

    void exportData()
    {
        
    }

    std::vector<Eigen::VectorXd> data;
    bool save_record_result;
    int start_frame;
    int end_frame;
    int skip_frame;
};
} // namespace Lobo