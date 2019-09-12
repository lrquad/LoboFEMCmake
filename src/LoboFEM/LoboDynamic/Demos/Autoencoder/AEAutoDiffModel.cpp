#include "AEAutoDiffModel.h"
#include "LoboVolumtricMesh/LoboTetMesh.h"
#include "LoboDynamic/LoboKeras/LoboKerasModel.h"

LOBO_TEMPLATE_INSTANT(AEAutoDiffModel);

template <class TYPE>
AEAutoDiffModel<TYPE>::AEAutoDiffModel(const char *decoderfilepath, TYPE data_min, TYPE data_scale)
{
	keras_model_complex = new LoboKerasModel<LoboComplexDualt>();
	keras_model_complex->loadNN(decoderfilepath);

	LoboComplexDualt mesh_min_complex;
	mesh_min_complex = data_min;
	LoboComplexDualt mesh_scale_complex;
	mesh_scale_complex = data_scale;
	keras_model_complex->setData_min(mesh_min_complex);
	keras_model_complex->setData_scale(mesh_scale_complex);

	num_latents = keras_model_complex->getInput();
}

template <class TYPE>
AEAutoDiffModel<TYPE>::~AEAutoDiffModel()
{
	delete keras_model_complex;
	free(input_complex);
	free(output_complex);
}

template <class TYPE>
void AEAutoDiffModel<TYPE>::initModel()
{
	int n_input = keras_model_complex->getInput();
	int n_output = keras_model_complex->getOutput();

	input_complex = (LoboComplexDualt *)malloc(sizeof(LoboComplexDualt) * n_input);
	memset(input_complex, 0, sizeof(LoboComplexDualt) * n_input);
	output_complex = (LoboComplexDualt *)malloc(sizeof(LoboComplexDualt) * n_output);
	memset(output_complex, 0, sizeof(LoboComplexDualt) * n_output);

	for (int i = 0; i < num_latents; i++)
	{
		for (int j = 0; j < num_latents; j++)
		{
			VectorX dudl_i(n_output);
			dudl_i.setZero();
			if (i == j)
			{
				du_dl_i.push_back(dudl_i);
			}
			du_dl_ii.push_back(dudl_i);
		}
	}
}

template <class TYPE>
void AEAutoDiffModel<TYPE>::computeDerivatives(TYPE *L, TYPE h)
{
	int n_input = keras_model_complex->getInput();
	int n_output = keras_model_complex->getOutput();
	for (int i = 0; i < num_latents; i++)
	{
		for (int k = 0; k < num_latents; k++)
		{
			memset(input_complex, 0, sizeof(LoboComplexDualt) * n_input);
			for (int j = 0; j < num_latents; j++)
			{
				input_complex[j].real_.real_ = L[j];
			}

			input_complex[k].real_.image_ = h;
			input_complex[i].image_.real_ = h;

			keras_model_complex->predict(input_complex, output_complex);

			for (int j = 0; j < n_output; j++)
			{
				if (i == k)
				{
					du_dl_i[i].data()[j] = output_complex[j].real_.image_ / h;
				}

				du_dl_ii[k * num_latents + i].data()[j] = output_complex[j].image_.image_ / h / h;
			}
		}
	}
}

template <class TYPE>
void AEAutoDiffModel<TYPE>::decoder(double *latents, TYPE *output)
{
	int n_input = keras_model_complex->getInput();
	int n_output = keras_model_complex->getOutput();

	memset(input_complex, 0, sizeof(LoboComplexDualt) * n_input);
	memset(output_complex, 0, sizeof(LoboComplexDualt) * n_output);
	for (int i = 0; i < n_input; i++)
	{
		input_complex[i].real_.real_ = (double)(TYPE)latents[i];
	}
	keras_model_complex->predict(input_complex, output_complex);

	for (int i = 0; i < n_output; i++)
	{
		output[i] = output_complex[i].real_.real_;
	}
}

template <class TYPE>
void AEAutoDiffModel<TYPE>::decoder(float *latents, TYPE *output)
{
	int n_input = keras_model_complex->getInput();
	int n_output = keras_model_complex->getOutput();

	memset(input_complex, 0, sizeof(LoboComplexDualt) * n_input);
	memset(output_complex, 0, sizeof(LoboComplexDualt) * n_output);

	for (int i = 0; i < n_input; i++)
	{
		input_complex[i].real_.real_ = (double)(TYPE)latents[i];
	}

	keras_model_complex->predict(input_complex, output_complex);

	for (int i = 0; i < n_output; i++)
	{
		output[i] = output_complex[i].real_.real_;
	}
}

template <class TYPE>
int AEAutoDiffModel<TYPE>::getNumOutput() const
{
	return keras_model_complex->getOutput();
}
