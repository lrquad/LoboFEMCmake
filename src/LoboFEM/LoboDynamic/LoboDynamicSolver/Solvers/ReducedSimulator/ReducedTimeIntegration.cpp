#include "ReducedTimeIntegration.h"
#include "ReducedKineticModel.h"

Lobo::RedcuedTimeIntegration::RedcuedTimeIntegration(ReducedKineticModel* kineticmodel_,int full_DoFs_, int reduced_DoFs_, double damping_ratio_, double timestep_, int skip_steps_, int flags_):ImplicitDenseIntegration(kineticmodel_, full_DoFs_, damping_ratio_, timestep_, skip_steps_, flags_)
{
    reduced_DOFs = reduced_DoFs_;
    reduced_model = kineticmodel_;

    q_reduced.resize(reduced_DOFs);
    q_reduced.setZero();
}

Lobo::RedcuedTimeIntegration::~RedcuedTimeIntegration()
{

}

void Lobo::RedcuedTimeIntegration::precompute()
{
    Lobo::ImplicitDenseIntegration::precompute();
}

void Lobo::RedcuedTimeIntegration::setInitLatentsq(Eigen::VectorXd &q_reduced_)
{
    q_reduced = q_reduced_;
    q = reduced_model->phi*q_reduced;
    q_1 = q;
}

void Lobo::RedcuedTimeIntegration::stepFoward()
{
    q_1 = q;
    reduced_model->setKineticStatus(q_vel, q_1);
    opsovler->solve(&q_reduced);
    opsovler->getResult(&q_reduced);
    q = reduced_model->phi*q_reduced;
    q_vel_1 = q_vel;
    q_vel = (q - q_1) / timestep;
    q_vel *= damping_ratio;
    step++;
}