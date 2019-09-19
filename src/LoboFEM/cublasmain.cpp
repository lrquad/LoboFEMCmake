#include <iostream>
#include <Eigen/Dense>
#include <time.h>

int main()
{
    Eigen::MatrixXd test(10000,5);
    Eigen::MatrixXd test2(5,200);
    Eigen::MatrixXd result;
    test.setRandom();
    test2.setRandom();

    clock_t t1 = clock();
    result  = test*test2;
    clock_t t2 = clock();

    std::cout<<"hello world" << std::endl;
    double seconds = (double)(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "seconds " << seconds << "s" << std::endl;



    return 0;
}