#include "DenseLayer.h"
#include <cstring>
#include <stdlib.h>
#include <iostream>
LOBO_TEMPLATE_INSTANT_NN(DenseLayer);

template<class TYPE>
DenseLayer<TYPE>::DenseLayer(TYPE* weights_, TYPE* bias_, int n_, int m_)
{
	n = n_;
	m = m_;

	this->n_input = n_;
	this->n_output = m_;

	weights = (TYPE*)malloc(sizeof(TYPE)*n*m);
	memcpy(weights, weights_, sizeof(TYPE)*n*m);

	bias = (TYPE*)malloc(sizeof(TYPE)*m);
	memcpy(bias, bias_, sizeof(TYPE)*m);
}

template<class TYPE>
DenseLayer<TYPE>::~DenseLayer()
{
	free(weights);
	free(bias);
}

template<class TYPE>
void DenseLayer<TYPE>::layerRun(TYPE* input, TYPE* output)
{
	for (int i = 0;i < m;i++)
	{
		output[i] = 0.0;
		
	}
	for (int i = 0;i < m;i++)
	{
		for (int j = 0;j < n;j++)
		{
			output[i] += input[j] * weights[i*n + j];
		}
		output[i] += bias[i];
	}

}

