#include <iostream>
#include <Eigen/Dense>
#include "MyClass.h"

int main()
{
    std::cout<<"hello world" << std::endl;
    Eigen::VectorXd x_p(1);
    std::cout<<x_p<<std::endl;
    MyClass m;
    std::cout<<m.print()<<std::endl;
    return 0;
}