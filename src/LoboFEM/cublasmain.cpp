#include <iostream>
#include <Eigen/Dense>
#include <time.h>
#include "CUDA/libcumatrix/device_matrix.h"
#include <cuda_runtime.h>
#include <cublas_v2.h>
#include <helper_cuda.h>

template <typename T>
void randomInit(device_matrix<T> &m)
{
    T *h_data = new T[m.size()];
    for (int i = 0; i < m.size(); ++i)
        h_data[i] = rand() / (T)RAND_MAX;
    cudaMemcpy(m.getData(), h_data, m.size() * sizeof(T), cudaMemcpyHostToDevice);
    delete[] h_data;
}

int main()
{
    int size = 10;
    Eigen::MatrixXf test(size, size);
    Eigen::MatrixXf test2(size, size);
    Eigen::MatrixXf result;
    test.setRandom();
    test2.setRandom();

    clock_t t1 = clock();
    result = test * test2;
    clock_t t2 = clock();

    std::cout << "hello world" << std::endl;
    double seconds = (double)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "seconds " << seconds << "s" << std::endl;
    std::cout << result.data()[0] << std::endl;

    device_matrix<float> A(size, size);
    device_matrix<float> B(size, size);
    device_matrix<float> C;

    cudaMemcpy(A.getData(), test.data(), A.size() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(B.getData(), test2.data(), B.size() * sizeof(float), cudaMemcpyHostToDevice);

    size_t m = A.getRows();
    size_t n = A.getCols();

    size_t k = B.getRows();
    size_t l = B.getCols();

    assert(n == k);

    C.resize(m, l);

    int lda = A.getRows();
    int ldb = B.getRows();
    int ldc = C.getRows();

    cublasOperation_t opA = CUBLAS_OP_N;
    cublasOperation_t opB = CUBLAS_OP_N;

    // Fill A with 0
    t1 = clock();

    device_matrix<float>::cublas_gemm(opA, opB, m, l, k, 1.0, A.getData(), lda, B.getData(), ldb, 0.0, C.getData(), ldc);
    
    //gemm(A, B, C);
    t2 = clock();
    seconds = (double)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "seconds " << seconds << "s" << std::endl;
    float *data = new float[C.size()];
    CCE(cudaMemcpy(data, C.getData(), sizeof(float) * C.size(), cudaMemcpyDeviceToHost));
    std::cout << data[0] << std::endl;

    return 0;
}