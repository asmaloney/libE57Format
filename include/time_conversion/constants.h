/**
\file    constants.h
\brief   GNSS core 'c' function library: Contains constants.
\author  Glenn D. MacGougan (GDM)
\date    2007-11-29
\since   2005-07-30

\b "LICENSE INFORMATION" \n
Copyright (c) 2007, refer to 'author' doxygen tags \n
All rights reserved. \n

Redistribution and use in source and binary forms, with or without
modification, are permitted provided the following conditions are met: \n

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer. \n
- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution. \n
- The name(s) of the contributor(s) may not be used to endorse or promote 
  products derived from this software without specific prior written 
  permission. \n

THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY EXPRESS 
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.
*/

#ifndef _C_CONSTANTS_H_
#define _C_CONSTANTS_H_

#ifdef __cplusplus
extern "C" {
#endif

//#define PI (3.1415926535898) // value from GPS-ICD pp. 101 used in orbit curve fit calculations by the control segment

#ifndef PI
#define PI        (3.1415926535897932384626433832795) //!< better value
#endif

#ifndef TWOPI
#define TWOPI     (6.283185307179586476925286766559)     //!< 2.0*PI
#endif

#ifndef HALFPI
#define HALFPI    (1.5707963267948966192313216916398)    //!< PI/2.0
#endif

#ifdef QUARTERPI
#define QUARTERPI (0.78539816339744830961566084581988)   //!< PI/4.0
#endif

#ifndef DEG2RAD
#define DEG2RAD   (0.017453292519943295769236907684886)  //!< PI/180.0
#endif

#ifndef RAD2DEG
#define RAD2DEG   (57.295779513082320876798154814105)    //!< 180.0/PI
#endif

#ifndef  SECONDS_IN_WEEK
#define  SECONDS_IN_WEEK (604800.0) //!< [s]
#endif

#ifndef  LIGHTSPEED
#define  LIGHTSPEED  (299792458.0) //!< light speed constant defined in ICD-GPS-200C p. 89  [m/s]
#endif

#ifndef  ONE_MS_IN_M
#define  ONE_MS_IN_M (299792.4580) //!< 1 millisecond * light speed constant defined in ICD-GPS-200C p. 89  [m/s]
#endif


#ifndef GPS_FREQUENCYL1
#define GPS_FREQUENCYL1  (1575.42e06) //!< [Hz]
#endif

#ifndef GPS_FREQUENCYL2
#define GPS_FREQUENCYL2  (1227.60e06) //!< [Hz]
#endif


#ifndef GPS_WAVELENGTHL1
#define GPS_WAVELENGTHL1 (0.19029367279836488047631742646405) //!< [m]
#endif


#ifndef GPS_WAVELENGTHL2
#define GPS_WAVELENGTHL2 (0.24421021342456825) //!< [m]
#endif


// not a constant but a very common type
#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (1)
#endif
  
#ifdef __cplusplus
}
#endif

#endif // _C_CONSTANTS_H_
