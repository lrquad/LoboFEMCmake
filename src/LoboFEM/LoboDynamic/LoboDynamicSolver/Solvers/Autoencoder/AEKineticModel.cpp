#include "AEKineticModel.h"
#include "AEAutoDiffModel.h"
#include "LoboDynamic/LoboDynamicSolver/LoboDynamicModel/ElasticModel/HyperelasticModel.h"
#include "LoboDynamic/LoboDynamicSolver/LoboDynamicModel/ConstrainModel/ConstrainModel.h"
#include "LoboDynamic/LoboDynamicSolver/LoboDynamicModel/CollisionModel/CollisionModel.h"
Lobo::AEKineticModel::AEKineticModel(LoboDynamicScene *scene_, LoboTetMesh *tetmesh_, AEAutoDiffModel<double> *ae_ad_model_, HyperelasticModel *elastic_model_, ConstrainModel *constrain_model_, CollisionModel *collisionmodel_) : KineticModel(scene_, tetmesh_, elastic_model_, constrain_model_, collisionmodel_)
{
    this->ae_ad_model = ae_ad_model_;
    num_DOFs = ae_ad_model->getNum_latents();
    full_DoFs = tetmesh->getNumVertex() * 3;

    external_forces.resize(full_DoFs);
    external_forces.setZero();

    q.resize(full_DoFs);
    q.setZero();

    internalforce.resize(full_DoFs);
    internalforce.setZero();
}

Lobo::AEKineticModel::~AEKineticModel()
{
}

void Lobo::AEKineticModel::precompute()
{
    Lobo::KineticModel::precompute();
}

void Lobo::AEKineticModel::computeEnergyDense(Eigen::VectorXd *free_variables, double *energy, Eigen::VectorXd *jacobi, Eigen::MatrixXd *hessian, int computationflags)
{
    int n_output = ae_ad_model->getNumOutput();
    int n_latents = ae_ad_model->getNum_latents();

    //need merge together

    double elastic_energy = 0;
    hyperelasticmodel->computeEnergySparse(&q, &elastic_energy, &internalforce, &stiffness_matrix, computationflags);

    double kinetic_energy;

    Eigen::VectorXd sn = q - q_1 - timestep * q_vel;
    Eigen::VectorXd M_sn = (mass_matrix)*sn;
    kinetic_energy = sn.dot(M_sn);
    kinetic_energy = kinetic_energy / (2.0 * timestep * timestep);

    double externalforce_energy;
    externalforce_energy = -external_forces.dot(q);

    if (computationflags & Computeflags_energy)
    {
        *energy = elastic_energy + kinetic_energy + externalforce_energy;
    }

    if (computationflags & (Computeflags_fisrt | Computeflags_second))
    {
        //ae_ad_model->computeDerivatives(free_variables->data(), 1e-30);
        //ae_ad_model->decoder(free_variables->data(), q.data());
        ae_ad_model->computeDerivativesAndQ(free_variables->data(), 1e-30, q.data());
    }

    if (computationflags & (Computeflags_fisrt))
    {
        //comptue gradient
        for (int j = 0; j < n_latents; j++)
        {
            jacobi->data()[j] = M_sn.dot(ae_ad_model->du_dl_i[j]);
            jacobi->data()[j] /= timestep * timestep;
            jacobi->data()[j] += internalforce.dot(ae_ad_model->du_dl_i[j]);
            jacobi->data()[j] -= external_forces.dot(ae_ad_model->du_dl_i[j]);
        }
    }

    if (computationflags & (Computeflags_second))
    {
        //comptue hessian
        for (int j = 0; j < n_latents; j++)
        {
            Eigen::VectorXd M_ql = (mass_matrix)*ae_ad_model->du_dl_i[j];
            Eigen::VectorXd K_ql = (stiffness_matrix)*ae_ad_model->du_dl_i[j];

            for (int k = j; k < n_latents; k++)
            {
                double entry_ = ae_ad_model->du_dl_i[k].dot(M_ql);
                entry_ /= timestep * timestep;
                entry_ += M_sn.dot(ae_ad_model->du_dl_ii[k * n_latents + j]) / (timestep * timestep);

                //elastic
                entry_ += ae_ad_model->du_dl_i[k].dot(K_ql);
                entry_ += internalforce.dot(ae_ad_model->du_dl_ii[k * n_latents + j]);
                entry_ -= external_forces.dot(ae_ad_model->du_dl_ii[k * n_latents + j]);
                hessian->data()[k * n_latents + j] = entry_;
            }
        }

        //fill the matrix
        for (int u = 0; u < n_latents; u++)
        {
            for (int v = 0; v < u; v++)
            {
                hessian->data()[v * n_latents + u] = hessian->data()[u * n_latents + v];
            }
        }
    }
}