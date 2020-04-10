#include "GraspContactModel.h"
#include <fstream>
#include "LoboDynamic/LoboDynamic.h"
#include "OpenGLutils/LoboScene.h"

Lobo::GraspContactModel::GraspContactModel(Lobo::LoboDynamicScene *scene_,
                                           Lobo::LoboTetMesh *tetmesh_)
    : tetmesh(tetmesh_) {
    weight_stiffness = 1000.0;
    friction_ratio = 1.0;
    radius = 0.1;
    this->scene = scene_;
}

Lobo::GraspContactModel::~GraspContactModel() {}

void Lobo::GraspContactModel::runXMLscript(pugi::xml_node &xml_node) {
    trimesh_list.clear();
    contact_points_list.clear();

    for (pugi::xml_node index_node : xml_node.children("Index")) {
        int contact_point_index = index_node.text().as_int();
        contact_points_list.push_back(contact_point_index);
        if (index_node.attribute("triid")) {
            int tri_id = index_node.attribute("triid").as_int();
            trimesh_list.push_back(scene->scene->getMesh(tri_id));
        }
    }

    if (xml_node.attribute("weight")) {
        weight_stiffness = xml_node.attribute("weight").as_double();
    }

    if (xml_node.attribute("friction")) {
        friction_ratio = xml_node.attribute("friction").as_double();
    }
    precompute();
}

void Lobo::GraspContactModel::precompute() {
    contact_normal.clear();
    contact_center.clear();
    for (int n = 0; n < contact_points_list.size(); n++) {
        Lobo::LoboMesh *trimesh = trimesh_list[n];
        int numtrinode = trimesh->attrib.vertices.size() / 3;
        Eigen::VectorXd buffer(numtrinode * 3);
        trimesh->getCurVertices(buffer.data());

        Eigen::Vector3d tri_mesh_center;
        tri_mesh_center.setZero();
        for (int i = 0; i < numtrinode; i++) {
            tri_mesh_center.data()[0] += buffer.data()[i * 3 + 0];
            tri_mesh_center.data()[1] += buffer.data()[i * 3 + 1];
            tri_mesh_center.data()[2] += buffer.data()[i * 3 + 2];
        }
        tri_mesh_center /= numtrinode;
        // move to index
        Eigen::Vector3d target_position =
            this->tetmesh->getNodeRestPosition(contact_points_list[n]);
        Eigen::Vector3d contact_normal_ =
            this->tetmesh->getNodeNormal(contact_points_list[n]);
        target_position += contact_normal_ * 0.0;

        contact_normal.push_back(contact_normal_);
        contact_center.push_back(target_position);

        Eigen::Vector3d translation_ = target_position - tri_mesh_center;
        for (int i = 0; i < numtrinode; i++) {
            buffer.data()[i * 3 + 0] += translation_.data()[0];
            buffer.data()[i * 3 + 1] += translation_.data()[1];
            buffer.data()[i * 3 + 2] += translation_.data()[2];
        }
        trimesh->updateVertices(buffer.data());
    }
}

void Lobo::GraspContactModel::computeEnergySparse(

    Eigen::VectorXd *free_variables, double *energy, Eigen::VectorXd *jacobi,
    Eigen::SparseMatrix<double> *hessian, int computationflags) {
    if (computationflags & Computeflags_reset) {
        if (computationflags & Computeflags_energy) *energy = 0;

        if (computationflags & Computeflags_fisrt) jacobi->setZero();

        if (computationflags & Computeflags_second) {
            for (int i = 0; i < hessian->outerSize(); ++i)
                for (Eigen::SparseMatrix<double>::InnerIterator it(*hessian, i);
                     it; ++it) {
                    it.valueRef() = 0;
                }
        }
    }

    // collision detect
    for(int i=0;i<contact_points_list.size();i++)
    {
        int nodeid = contact_points_list[i];
        Eigen::Vector3d node_position = tetmesh->getNodeRestPosition(nodeid);
        node_position.data()[0]+=free_variables->data()[nodeid*3+0];
        node_position.data()[1]+=free_variables->data()[nodeid*3+1];
        node_position.data()[2]+=free_variables->data()[nodeid*3+2];

        for(int j=0;j<3;j++)
        {
            int dof_id = nodeid*3+j;
            double loss = node_position.data()[j]-contact_center[i].data()[j];

            if (computationflags & Computeflags_energy) {
                *energy += weight_stiffness*loss*loss;
            }

            if (computationflags & Computeflags_fisrt)
            {
                jacobi->data()[dof_id]+=2.0*weight_stiffness*loss;
            }

            if (computationflags & Computeflags_second)
            {
                 hessian->valuePtr()[diagonal_[dof_id]]+=2.0*weight_stiffness;
            }
        }
    }
        
}

void Lobo::GraspContactModel::setpForward(int step) {
    for (int i = 0; i < contact_points_list.size(); i++) {
        Lobo::LoboMesh *trimesh = trimesh_list[i];
        int numtrinode = trimesh->attrib.vertices.size() / 3;
        Eigen::VectorXd buffer(numtrinode * 3);
        buffer.setZero();
        trimesh->getCurVertices(buffer.data());
        double speed = 0.001;
        if (step < 50) {
            for (int j = 0; j < numtrinode; j++) {
                buffer.data()[j * 3 + 0] +=
                    -contact_normal[i].data()[0] * speed;
                buffer.data()[j * 3 + 1] +=
                    -contact_normal[i].data()[1] * speed;
                buffer.data()[j * 3 + 2] +=
                    -contact_normal[i].data()[2] * speed;
            }
            contact_center[i].data()[0] += -contact_normal[i].data()[0] * speed;
            contact_center[i].data()[1] += -contact_normal[i].data()[1] * speed;
            contact_center[i].data()[2] += -contact_normal[i].data()[2] * speed;
        } else {
            for (int j = 0; j < numtrinode; j++) {
                buffer.data()[j * 3 + 1] += speed;
            }
            contact_center[i].data()[1] += speed;
        }
        trimesh->updateVertices(buffer.data());
    }
}
