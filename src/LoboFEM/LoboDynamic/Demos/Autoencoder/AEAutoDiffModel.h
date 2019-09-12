#pragma once
#include <Eigen/Sparse>
#include "Functions/LoboMacros.h"
#include "AutoDiff/AutoDiffCore.h"

template<class TYPE> class LoboKerasModel;

template  <class TYPE>
class AEAutoDiffModel
{

public:

	typedef Eigen::Matrix<TYPE, Eigen::Dynamic, 1> VectorX;
	typedef Eigen::Matrix<TYPE, 3, 1> Vector3;
	typedef Eigen::Matrix<TYPE, 3, 3> Matrix3;
	typedef Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> MatrixX;
	typedef Eigen::Triplet<TYPE> EIGEN_TRI;

	LOBO_MAKE_TYPEDEFS(TYPE, t);

	AEAutoDiffModel(const char* decoderfilepath,TYPE data_min,TYPE data_scale);
	~AEAutoDiffModel();

	virtual void initModel();

	virtual void computeDerivatives(TYPE* L, TYPE h);

	virtual void decoder(double* latents, TYPE* output);
	virtual void decoder(float* latents, TYPE* output);

	int getNum_latents() const { return num_latents; }
	void setNum_latents(int val) { num_latents = val; }

	int getNumOutput()const;


	std::vector<VectorX> du_dl_i; // store the first order derivatives
	std::vector<VectorX> du_dl_ii; // store the second order diagnal entry

protected:

	int num_latents;

	LoboKerasModel<LoboComplexDualt>* keras_model_complex;

	

	LoboComplexDualt* input_complex;
	LoboComplexDualt* output_complex;

};

