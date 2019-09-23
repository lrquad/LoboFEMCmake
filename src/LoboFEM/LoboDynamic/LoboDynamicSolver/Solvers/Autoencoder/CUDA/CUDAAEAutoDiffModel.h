#pragma once
#include <Eigen/Dense>
#include <vector>

namespace Lobo
{
class CUDAKerasModel;

class CUDAAEAutoDiffModel
{
private:
    /* data */
public:
    CUDAAEAutoDiffModel(const char *decoderfilepath, double data_min, double data_scale);
    ~CUDAAEAutoDiffModel();

    virtual void initModel();
    virtual void computeDerivativesAndQ(double* latents,double h, double* output);

    virtual void evalue(double* latents,double* output);

    int getNum_latents() const { return num_latents; }
	void setNum_latents(int val) { num_latents = val; }

	int getNumOutput()const;

    std::vector<Eigen::VectorXd> du_dl_i; // store the first order derivatives
	std::vector<Eigen::VectorXd> du_dl_ii; // store the second order diagnal entry

    CUDAKerasModel* keras_model_CUDA;

protected:

    int num_latents;

    Eigen::VectorXd input_r_r;
    Eigen::VectorXd input_r_i;
    Eigen::VectorXd input_i_r;
    Eigen::VectorXd input_i_i;
    
    Eigen::VectorXd output_r_r;
    Eigen::VectorXd output_r_i;
    Eigen::VectorXd output_i_r;
    Eigen::VectorXd output_i_i;

};

} // namespace Lobo
