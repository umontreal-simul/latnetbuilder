// This file is part of Lattice Builder.
//
// Copyright (C) 2012-2016  Pierre L'Ecuyer and Universite de Montreal
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DIGITAL_LINE_H
#define DIGITAL_LINE_H
#include <boost/dynamic_bitset.hpp> 
#include <iostream>

// For now, only works in base 2, needs to be templated by the base.

namespace LatBuilder { namespace DigitalNet {

class Row : public boost::dynamic_bitset<> {

    public:

    Row(int number_of_columns=0, int repr_as_int=0)
    : boost::dynamic_bitset<>(number_of_columns, repr_as_int){}

    Row(std::string s)
    : boost::dynamic_bitset<>(s){}

    Row(boost::dynamic_bitset<> b)
    : boost::dynamic_bitset<>(b){}

    Row(const Row& r)
    : boost::dynamic_bitset<>(r){}

    // Row operator^(Row r1, Row r2){
    //     return (boost::dynamic_bitset<> r1)^(boost::dynamic_bitset<> r2);
    // }    

    Row& operator=(const Row& d)
    {
        boost::dynamic_bitset<>::operator=(d);
        return *this;
    }

    Row& operator=(const dynamic_bitset<>& d)
    {
        boost::dynamic_bitset<>::operator=(d);
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Row& row)
    {
        for (Row::size_type i = 0; i < row.size(); ++i){
            std::cout << row[i];
        }
        std::cout << std::endl; 
        return os; 
    }

    void swap_columns(int j1, int j2){
        auto temp = (*this)[j2];
        (*this)[j2] = (*this)[j1];
        (*this)[j1] = temp;
        // line = line ^ Row(m, ((temp << j_1) | (temp << j_2)));
    }

    static Row permutation(Row& row, std::vector<int>& C){
        Row new_row(row.size());
        for (int i=0; i < row.size(); i++){
            new_row[C[i]] = row[C[i]];
        }
        return new_row;
    }
};
}
}

#endif