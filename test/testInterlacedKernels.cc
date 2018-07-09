#include <iostream>
#include <vector>

#include "latbuilder/Functor/BIDN.h"
#include "latbuilder/Functor/AIDNAlpha.h"

int main()
{
    unsigned int m = 5;
    unsigned int interlacing =  2;
    unsigned int alpha = 2;

    auto kernelA = LatBuilder::Functor::AIDNAlpha(alpha, interlacing);
    auto kernelB = LatBuilder::Functor::BIDN(interlacing);

    for(unsigned int i = 0; i < (1 << m); ++i)
    {
        std::cout << kernelA( (double) i / (1 << m)) << " ";
    }
    std::cout << std::endl;
    for(unsigned int i = 0; i < (1 << m); ++i)
    {
        std::cout << kernelB( (double) i / (1 << m)) << " ";
    }
    std::cout << std::endl;
}