#pragma once
#include "LoboDynamic/LoboDynamic.h"
#include "npy.h"
#include "imgui.h"
#include "AEAutoDiffModel.h"

namespace Lobo
{
class DemoDinosaurAEGen : public ModalWarpingSovler
{
public:
    AEAutoDiffModel<double> *ae_diff_model;
    std::vector<Eigen::VectorXd> data;
    Eigen::VectorXf latents;
    bool save_record_result;
    int start_frame;
    int end_frame;
    int skip_frame;

    DemoDinosaurAEGen(Lobo::LoboDynamicScene *parent_scene) : ModalWarpingSovler(parent_scene)
    {
        save_record_result = true;
        start_frame = 0;
        end_frame = 10000;
        skip_frame = 2;
        ae_diff_model = NULL;
    }

    void updateByLatents();
    virtual void drawImGui();
    virtual void stepForward();
    void exportData();

    
};
} // namespace Lobo