#pragma once
#include "LoboDynamic/LoboDynamic.h"
#include "npy.h"
#include "imgui.h"
#include "AEAutoDiffModel.h"

namespace Lobo
{
class AESimulatorGen : public ModalWarpingSimulator
{
public:
    AEAutoDiffModel<double> *ae_diff_model;
    std::vector<Eigen::VectorXd> data;
    Eigen::VectorXf latents;
    bool save_record_result;
    int start_frame;
    int end_frame;
    int skip_frame;

    AESimulatorGen(Lobo::LoboDynamicScene *parent_scene);

    virtual void runXMLscript(pugi::xml_node &solver_node);

    void updateByLatents();
    virtual void drawImGui();
    virtual void stepForward();
    void exportData();

    double mesh_min;
    double mesh_scale;
};
} // namespace Lobo