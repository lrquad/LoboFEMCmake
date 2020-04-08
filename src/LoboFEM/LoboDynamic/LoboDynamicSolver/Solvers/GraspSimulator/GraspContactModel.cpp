#include "GraspContactModel.h"
#include <fstream>
#include "LoboDynamic/LoboDynamic.h"

Lobo::GraspContactModel::GraspContactModel(Lobo::LoboTetMesh* tetmesh_)
    : tetmesh(tetmesh_) {
    weight_stiffness = 1000.0;
}

Lobo::GraspContactModel::~GraspContactModel() {}

void Lobo::GraspContactModel::runXMLscript(pugi::xml_node& xml_node) {
    for (pugi::xml_node index_node : xml_node.children("Index")) {
        int contact_point_index = index_node.text().as_int();
        contact_points_list.push_back(contact_point_index);
        std::cout<<contact_point_index<<std::endl;
    }

    if(xml_node.attribute("weight"))
    {
        weight_stiffness = xml_node.attribute("weight").as_double();
    }
}

void Lobo::GraspContactModel::computeEnergySparse(Eigen::VectorXd *free_variables, double *energy, Eigen::VectorXd *jacobi, Eigen::SparseMatrix<double> *hessian, int computationflags)
{
    if (computationflags & Computeflags_reset)
    {
        if (computationflags & Computeflags_energy)
            *energy = 0;

        if (computationflags & Computeflags_fisrt)
            jacobi->setZero();

        if (computationflags & Computeflags_second)
        {
            for (int i = 0; i < hessian->outerSize(); ++i)
                for (Eigen::SparseMatrix<double>::InnerIterator it(*hessian, i);
                     it; ++it)
                {
                    it.valueRef() = 0;
                }
        }
    }

    


}

