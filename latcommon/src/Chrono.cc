// Copyright (c) 2012 Richard Simard, Pierre L'Ecuyer, Université de Montréal.
// 
// This file is part of Lattice Builder.
// 
// Lattice Builder is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Lattice Builder is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Lattice Builder.  If not, see <http://www.gnu.org/licenses/>.

#include "latcommon/Chrono.h"

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;


namespace {

#ifdef HAVE_WINDOWS_H
#include <windows.h>

HANDLE currentProcess = NULL;

/*
 * A helper function for converting FILETIME to a LONGLONG [safe from memory
 * alignment point of view].
 */
ULONGLONG fileTimeToInt64 (const FILETIME * time)
{
    ULARGE_INTEGER _time;
    _time.LowPart = time->dwLowDateTime;
    _time.HighPart = time->dwHighDateTime;
    return _time.QuadPart;
}
#endif

}  // namespace


//-------------------------------------------------------------------------
namespace LatCommon {

#ifdef HAVE_WINDOWS_H

void Chrono::heure () 
{
   if (currentProcess == NULL)
      currentProcess = GetCurrentProcess();
   FILETIME creationTime, exitTime, kernelTime, userTime;
   /* Strongly inspired from
    * http://www.javaworld.com/javaworld/javaqa/2002-11/01-qa-1108-cpu.html */
   GetProcessTimes (currentProcess, &creationTime, &exitTime,
		   &kernelTime, &userTime);
   ULONGLONG rawTime = fileTimeToInt64 (&kernelTime) +
     fileTimeToInt64 (&userTime);
   /* We have to divide by 10000 to get milliseconds out of
    * the computed time */
   second = static_cast<unsigned long>(rawTime / 10000000);
   microsec = static_cast<unsigned long>((rawTime % 10000000) / 10);
}


#elif defined(USE_ANSI_CLOCK)
// ANSI C timer

void Chrono::heure ()
/* 
 * Function returning the CPU time used by a program since it was
 * started. This function is ANSI C compliant.
 */
{
   clock_t t;
   double y;

   t = clock ();
   y = (static_cast<double> (t)) / CLOCKS_PER_SEC;
   second = static_cast<unsigned long>(y);
   microsec = static_cast<unsigned long>((y - second) * 1000000);
}


#else
// POSIX timer

#include <sys/times.h>
#include <unistd.h>

void Chrono::heure ()
/*
 * Function returning the CPU time used by a program since it was
 * started. This function is NOT ANSI C compliant.
 */
{
   struct tms us;
   long TICKS, z;

   TICKS = sysconf(_SC_CLK_TCK);
   if (TICKS == -1) {
      cout << "Chrono.cc:   'sysconf(_SC_CLK_TCK)' failed\n";
   }
   z = times (&us);
   if (z == -1) {
      cout << "Chrono.cc:   timer times failed\n";
   }

   /* CPU time = user time + system time */
   microsec = us.tms_utime + us.tms_stime;

   second = microsec / TICKS;
   microsec = (microsec % TICKS) * 1000000 / TICKS;
}

#endif


/*------------------------------------------------------------------------*/

void Chrono::init ()
{
   heure();
}


Chrono::Chrono()
{
   init();
}



double Chrono::val (TimeFormat Unit)
{
   Chrono now;
   now.heure();
   double temps;                     // Time elapsed, in seconds
   temps = (static_cast<double>(now.microsec) - 
            static_cast<double>(microsec)) / 1.E+6 +
            static_cast<double>(now.second) -
            static_cast<double>(second);

   switch (Unit) {
   case SEC:
      return temps;
   case MIN:
      return temps * 1.666666667E-2;
   case HOURS:
      return temps * 2.777777778E-4;
   case DAYS:
      return temps * 1.157407407E-5;
   case HMS:
      cerr << "Chrono.val: HMS is a wrong arg for Time Unit";
   }
   return 0.0;
}


void Chrono::write (TimeFormat Form)
{
   double temps;
   if (Form != HMS)
      temps = val (Form);
   else
      temps = 0.0;
   switch (Form) {
   case SEC:
      cout << setw(10) << setprecision (2) << temps;
      cout << " seconds";
      break;
   case MIN:
      cout << setw(10) << setprecision (2) << temps;
      cout << " minutes";
      break;
   case HOURS:
      cout << setw(10) << setprecision (2) << temps;
      cout << " hours";
      break;
   case DAYS:
      cout << setw(10) << setprecision (2) << temps;
      cout << " days";
      break;
   case HMS:
      temps = val (SEC);
      long heure = static_cast<long> (temps * 2.777777778E-4);
      if (heure > 0)
         temps -= static_cast<double> (heure) * 3600.0;
      long minute = static_cast<long> (temps * 1.666666667E-2);
      if (minute > 0)
         temps -= static_cast<double> (minute) * 60.0;
      long seconde = static_cast<long> (temps);
      long centieme = static_cast<long>
                   (100.0 * (temps - static_cast<double> (seconde)));
      cout << setw(2) << setfill('0') << right << heure << ":" ;
      cout << setw(2) << setfill('0') << right << minute << ":" ;
      cout << setw(2) << setfill('0') << right << seconde << "." ;
      cout << setw(2) << setprecision(2) << centieme;
      break;
   }
}

//===========================================================================

bool Chrono::timeOver (double limit)
{
   // Returne true si le temps limite de CPU est atteint
   double temps = val (SEC);
   if (temps >= limit)
      return true;
   else
      return false;
}


//===========================================================================

std::string Chrono::toString ()
{
   double temps = val (SEC);
   long heure = (long) (temps * 2.777777778E-4);
   if (heure > 0)
      temps -= heure * 3600.0;
   long minute = (long) (temps * 1.666666667E-2);
   if (minute > 0)
      temps -= minute * 60.0;
   long seconde = (long) temps;
   long frac = (long) (100.0 * (temps - seconde));

   std::ostringstream sortie;
   sortie << heure << ":" << minute << ":" << seconde << "." << frac;
   return sortie.str ();
}

}
