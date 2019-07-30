#include <iostream>
#include "MyClass.h"
MyClass::MyClass(/* args */)
{
    std::cout<<"My Class" <<std::endl;
}

MyClass::~MyClass()
{
}

int MyClass::print()
{
    std::cout<<"what!!!!!!!!!!!!!"<<std::endl;
    return 10;
}