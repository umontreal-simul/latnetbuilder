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

#ifndef DIGITAL_NET_DIGITAL_NET_BASE2_H
#define DIGITAL_NET_DIGITAL_NET_BASE2_H
#include "latbuilder/Types.h"
#include "latbuilder/Util.h"


namespace LatBuilder { namespace DigitalNet {

//available output formats for digital nets
enum class OutputFormat {SSJ, web_interface};
/**
 * Definition of a Digital net in base 2
 * Defining parameters:
 * - m_rows  number of rows;
 * - m_columns  number of columns;
 * - m_numPoints  number of points;
 * - m_genratorMatrices  generator matrices;
 * - m_outputFormat  output format.
 * 
 * internally, each culumn of a generator matrice is stored in m_genratorMatrices as an integer,
 * with the corresponding binary represention, as follows : 
 * a columns \f$ c = (c_0,\dots,c_{r-1} )\f$ is represented by the integer  \f$ x = \sum_{l=0}^{r-1} c_l2^l \f$. 
 * if the generating matrices of the digital net are \f$ C^{(0)}, \dots, C^{(s-1)}\f$, m_genratorMatrices[i][j]
 * contains the \f$j^{\text{th}}\f$ column of the matrix \f$ C^{(i)}\f$ (as an integer. where columns are indexed starting from 0.
 * (\f$i = 0,\dots,s-1\f$ and  \f$j = 0,\dots,k-1 \f$)
 *
 * 
 */
class DigitalNetBase2 {
public:
   // standard container type definitions
   typedef size_t size_type;
   // standard container type definitions
   typedef std::vector<std::vector<uInteger>> GeneratorMatrices;

   /**
    * Constructor.
    * \param rows  number of rows.
    * \param columns  number of columns.
    * \param genratorMatrices  generator matrices.
    */

   DigitalNetBase2(size_type rows, size_type columns, GeneratorMatrices genratorMatrices):
      m_rows(rows),
      m_columns(columns),
      m_numPoints(intPow(2, m_columns)),
      m_genratorMatrices(std::move(genratorMatrices))
   { init(); }

   // returns the modulus of the digital net
   size_type numColumns() const { return m_columns; }

   // returns the modulus of the digital net
   size_type numRows() const { return m_rows; }

   // returns the number of points of the digital net
   size_type numPoints() const { return m_numPoints; }

   // returns the number of points of the digital net
   size_type size() const { return m_numPoints; }

   // returns the dimension of the digital net
   size_type dimension() const { return m_genratorMatrices.size(); }

   // set output format
   void setOutputFormat(OutputFormat Out) { m_outputFormat = Out; }

   // get output format
   OutputFormat getOutputFormat() { return m_outputFormat; }

   // outputs the matrices in the format required by Out
   template<OutputFormat Out> 
   std::ostream& format(std::ostream& os) const ;

   

private:
   
   size_type m_rows;
   size_type m_columns;
   size_type m_numPoints;
   GeneratorMatrices m_genratorMatrices;
   OutputFormat m_outputFormat;

   void init()
   {m_outputFormat = OutputFormat::SSJ; }

};

//================================================================================
template <>
std::ostream& DigitalNetBase2::format<OutputFormat::SSJ>(std::ostream& os) const
{
   std::ostream& out = os;
   out << "2  //Base" << std::endl;
  out << numColumns() << "  //Number of columns" << std::endl;
   out << numRows() << "  //Number of rows" << std::endl;
   out << numPoints() << "  //Number of points" << std::endl;
   out << dimension() << "  //dimension of points" << std::endl;
   out << std::endl;
   for(uInteger j=0; j<dimension(); j++){
      out << "//dim = " << j+1 << std::endl;
      for(uInteger c=0; c<numColumns(); c++){
          uInteger x = m_genratorMatrices[j][c];
          uInteger mult = intPow(2,30);
          uInteger res = 0;
          while(x > 0){
              res += (x%2) * mult ;
              x /= 2;
              mult /= 2;
          }
          out << res << std::endl;
      }
      out << std::endl;
   }
   return out;


}


template <>
std::ostream& DigitalNetBase2::format<OutputFormat::web_interface>(std::ostream& os) const
{
   std::ostream& out = os;
   for(uInteger j=0; j<dimension(); j++){
      out << "//dim = " << j+1 << std::endl;
      for(uInteger c=0; c<numColumns(); c++){
          uInteger x = m_genratorMatrices[j][c];
          out << x << std::endl;
      }
      out << std::endl;
   }
   return out;


}

// outputs the matrices in the format required by m_outputFormat
   std::ostream& operator<<(std::ostream& os, DigitalNetBase2 net) 
   {
    OutputFormat out = net.getOutputFormat();
    if(out == OutputFormat::SSJ)
      return net.format<OutputFormat::SSJ>(os);
    if(out == OutputFormat::web_interface)
      return net.format<OutputFormat::web_interface>(os);
    return os;
   }



}}

#endif
