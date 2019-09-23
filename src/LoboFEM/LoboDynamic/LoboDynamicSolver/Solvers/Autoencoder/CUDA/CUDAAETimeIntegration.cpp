#include "CUDAAETimeIntegration.h"
#include "CUDAAEKineticModel.h"
#include "CUDAAEAutoDiffModel.h"

Lobo::CUDAAETimeIntegration::CUDAAETimeIntegration(CUDAAEKineticModel *kineticmodel_, int num_DOfs_, double damping_ratio_, double timestep_, int skip_steps_, int flags_) : ImplicitDenseIntegration(kineticmodel_, num_DOfs_, damping_ratio_, timestep_, skip_steps_, flags_)
{
    ae_model = kineticmodel_;
    num_latents = ae_model->getNum_latents();

    q_latents.resize(num_latents);
    q_latents.setZero();
}

Lobo::CUDAAETimeIntegration::~CUDAAETimeIntegration()
{
}

void Lobo::CUDAAETimeIntegration::precompute()
{
    Lobo::ImplicitDenseIntegration::precompute();
}

void Lobo::CUDAAETimeIntegration::setInitLatentsq(Eigen::VectorXd &latents_q_)
{
    q_latents = latents_q_;
    ae_model->AE_value(q_latents.data(), q.data());
    q_1 = q;
}

void Lobo::CUDAAETimeIntegration::stepFoward()
{
    q_1 = q;
    ae_model->setKineticStatus(q_vel, q_1);
    opsovler->solve(&q_latents);
    opsovler->getResult(&q_latents);
    ae_model->AE_value(q_latents.data(), q.data());
    q_vel_1 = q_vel;
    q_vel = (q - q_1) / timestep;
    q_vel *= damping_ratio;
    step++;
}