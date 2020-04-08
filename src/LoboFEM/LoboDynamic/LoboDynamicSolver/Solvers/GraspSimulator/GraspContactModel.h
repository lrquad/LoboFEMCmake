#pragma once
#include "LoboDynamic/LoboDynamicSolver/LoboDynamicModel/DynamicModel.h"
#include "Utils/pugixml/pugiconfig.hpp"

namespace Lobo
{
    class LoboTetMesh;

    class GraspContactModel: public DynamicModel
    {
        public:
        GraspContactModel(LoboTetMesh* tetmesh_);
        ~GraspContactModel();

        virtual void runXMLscript(pugi::xml_node &xml_node);

        virtual void computeEnergySparse(Eigen::VectorXd* free_variables,double * energy, Eigen::VectorXd* jacobi, Eigen::SparseMatrix<double>* hessian, int computationflags);
        virtual void computeEnergyDense(Eigen::VectorXd* free_variables,double * energy, Eigen::VectorXd* jacobi, Eigen::MatrixXd* hessian, int computationflags){};

        virtual void setContactPointsIndex(std::vector<int> index_){};

        double weight_stiffness;

        protected:
        std::vector<int> contact_points_list; //the grasping force will apply to
        LoboTetMesh* tetmesh;
    };
}