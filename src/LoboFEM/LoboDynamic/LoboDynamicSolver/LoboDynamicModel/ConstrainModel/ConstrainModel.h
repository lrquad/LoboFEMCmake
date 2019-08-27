#pragma once
#include "LoboDynamic/LoboDynamicSolver/LoboDynamicModel/DynamicModel.h"
#include "Utils/pugixml/pugixml.hpp"

namespace Lobo
{
    class LoboTetMesh;

    class ConstrainModel:public DynamicModel
    {
        public:
        ConstrainModel(LoboTetMesh* tetmesh_);
        ~ConstrainModel();

        virtual void runXMLscript(pugi::xml_node &xml_node);

        virtual void computeEnergySparse(Eigen::VectorXd* free_variables,double * energy, Eigen::VectorXd* jacobi, Eigen::SparseMatrix<double>* hessian, int computationflags);
        virtual void computeEnergyDense(Eigen::VectorXd* free_variables,double * energy, Eigen::VectorXd* jacobi, Eigen::MatrixXd* hessian, int computationflags){};

        virtual void loadConstraints(const char* filename);

        double weight_stiffness;

        protected:

        LoboTetMesh* tetmesh;
        int num_constraints;
        int* constrained_DoFs;
    };
}
