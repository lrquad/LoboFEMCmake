#include "CUDAAEAutoDiffModel.h"
#include "LoboDynamic/LoboKeras/CUDA/CUDAKerasModel.h"
#include <time.h>
#include <iostream>
Lobo::CUDAAEAutoDiffModel::CUDAAEAutoDiffModel(const char *decoderfilepath, double data_min, double data_scale)
{
    keras_model_CUDA = new CUDAKerasModel();
    keras_model_CUDA->loadNN(decoderfilepath);

    keras_model_CUDA->setData_min(data_min);
    keras_model_CUDA->setData_scale(data_scale);

    num_latents = keras_model_CUDA->getInput();
}

Lobo::CUDAAEAutoDiffModel::~CUDAAEAutoDiffModel()
{
    delete keras_model_CUDA;
}

void Lobo::CUDAAEAutoDiffModel::initModel()
{
    int n_input = keras_model_CUDA->getInput();
    int n_output = keras_model_CUDA->getOutput();

    for (int i = 0; i < num_latents; i++)
    {
        for (int j = 0; j < num_latents; j++)
        {
            Eigen::VectorXd dudl_i(n_output);
            dudl_i.setZero();
            if (i == j)
            {
                du_dl_i.push_back(dudl_i);
            }
            du_dl_ii.push_back(dudl_i);
        }
    }

    input_r_r.resize(n_input);
    input_r_i.resize(n_input);
    input_i_r.resize(n_input);
    input_i_i.resize(n_input);
    output_r_r.resize(n_output);
    output_r_i.resize(n_output);
    output_i_r.resize(n_output);
    output_i_i.resize(n_output);
}

void Lobo::CUDAAEAutoDiffModel::computeDerivativesAndQ(double *L, double h, double *output)
{
    int n_input = keras_model_CUDA->getInput();
    int n_output = keras_model_CUDA->getOutput();

    for (int i = 0; i < num_latents; i++)
    {
        input_r_r.data()[i] = L[i];
    }
    clock_t t1 = clock();

    for (int i = 0; i < num_latents; i++)
    {
        for (int k = 0; k < num_latents; k++)
        {
            input_i_r.setZero();
            input_r_i.setZero();
            input_i_i.setZero();

            input_r_i.data()[k] = h;
            input_i_r.data()[i] = h;

            keras_model_CUDA->predict(input_r_r.data(), input_r_i.data(), input_i_r.data(), input_i_i.data(), output_r_r.data(), output_r_i.data(), output_i_r.data(), output_i_i.data());

            if (i == k)
                du_dl_i[i] = output_r_i / h;

            // if (i == 0 && k == 0)
            //     memcpy(output, output_r_r.data(), sizeof(double) * output_r_r.size());

            du_dl_ii[k * num_latents + i] = output_i_i / h / h;
            //std::cout<<du_dl_ii[k * num_latents + i].norm()<<std::endl;
        }
    }

    clock_t t2 = clock();
    double seconds = (double)(t2 - t1) / CLOCKS_PER_SEC;
    //std::cout << "compute phi " << seconds << "s" << std::endl;
}

void Lobo::CUDAAEAutoDiffModel::evalue(double *latents, double *output)
{
    keras_model_CUDA->predict(latents, output);
}

int Lobo::CUDAAEAutoDiffModel::getNumOutput() const
{
    return keras_model_CUDA->getOutput();
}
