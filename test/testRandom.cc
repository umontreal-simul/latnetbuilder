#include "latbuilder/LFSR258.h"
#include <iostream>

int main(){
    LatBuilder::LFSR258 gen = LatBuilder::LFSR258();
    for (int i=0; i<20; i++){
        std::cout << gen() << std::endl;
    }
}