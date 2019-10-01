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

    std::vector<double> vertex_error_data;
    std::vector<double> vertex_deform_data;
    std::vector<double> vertex_decoed_data;

    std::vector<unsigned long> vertex_data_shape;
    std::vector<unsigned long> vertex_deform_shape;
    std::vector<unsigned long> vertex_decoded_shape;



    int show_data_index;
    bool show_decoded;
    //buffer
    Eigen::VectorXd v_color;
    Eigen::VectorXd v_position;

};
} // namespace Lobo