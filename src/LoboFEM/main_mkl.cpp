#define EIGEN_USE_MKL_ALL

#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <ctime>
#include <iostream>

int main()
{
    clock_t t1 = clock();

    Eigen::MatrixXd A(10000,200);
    Eigen::MatrixXd B(200,300);
    A.setRandom();
    B.setRandom();

    Eigen::MatrixXd C = A*B;
    
    clock_t t2 = clock()-t1;
    std::cout<<C.data()[0]<<std::endl;
    std::cout<<((double) (t2)) / CLOCKS_PER_SEC <<" s"<<std::endl;

    return 0;
}