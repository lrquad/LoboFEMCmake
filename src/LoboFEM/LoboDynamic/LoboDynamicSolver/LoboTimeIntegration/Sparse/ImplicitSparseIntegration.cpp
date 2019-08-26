#include "ImplicitSparseIntegration.h"
#include "LoboDynamic/LoboDynamic.h"

Lobo::ImplicitSparseIntegration::ImplicitSparseIntegration(KineticModel *kineticmodel_, int num_DOfs_, double damping_ratio_, double timestep_,int skip_steps_, int flags_):LoboTimeIntegration(num_DOfs_,damping_ratio_,timestep_,skip_steps_,flags_) {
    
    kineticmodel = kineticmodel_;

}

Lobo::ImplicitSparseIntegration::~ImplicitSparseIntegration() {


}

void Lobo::ImplicitSparseIntegration::stepFoward()
{
    q_1 = q;
	//q_vel_1 = q_vel;

    //solve kineticmodel
    kineticmodel->setKineticStatus(q,q_vel,q_1);
    
    

    q_vel_1 = q_vel;
	q_vel = (q - q_1) / timestep;

	q_vel *= damping_ratio;
	step++;
}