#include "latbuilder/DigitalNet/Matrix.h"
#include <iostream>

using namespace LatBuilder::DigitalNet;

int main(int argc, const char *argv[])
{
    Matrix<2> test(5,5);

    std::cout << test << std::endl;

    test.flip(2,3);
    assert(test(2,3)==1);
    test.flip(2,3);
    assert(test(2,3)==0);

    test(2,3) = 4;
    std::cout << test << std::endl;

    test.swap_columns(0,3);
    std::cout << test << std::endl;

    
}