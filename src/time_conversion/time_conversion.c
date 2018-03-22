/**
\file    time_conversion.c
\brief   GNSS core 'c' function library: converting time information.
\author  Glenn D. MacGougan (GDM)
\date    2007-11-29
\since   2005-07-30

\b REFERENCES \n
- Hofmann-Wellenhof, B., H. Lichtenegger, and J. Collins (1994). GPS Theory and 
  Practice, Third, revised edition. Springer-Verlag, Wien New York. pp. 38-42 \n
- http://aa.usno.navy.mil/data/docs/JulianDate.html - Julian Date Converter \n
- http://aa.usno.navy.mil/faq/docs/UT.html \n
- http://wwwmacho.mcmaster.ca/JAVA/JD.html \n
- Raquet, J. F. (2002), GPS Receiver Design Lecture Notes. Geomatics Engineering, 
  University of Calgary Graduate Course. \n

\b "LICENSE INFORMATION" \n
Copyright (c) 2007, refer to 'author' doxygen tags \n
All rights reserved. \n

Redistribution and use in source and binary forms, with or without
modification, are permitted provided the following conditions are met: \n

- Redistributions of source code must retain te above copyright
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

#include <sys/timeb.h>
#include <time.h>
#include <math.h> /* for fmod() */
#include "gnss_error.h"
#include "time_conversion.h"
#include "constants.h"

#if 0
#ifndef WIN32 
  #define _CRT_SECURE_NO_DEPRECATE  
#endif
#endif

#ifdef WIN32
  #include <windows.h>
#endif


#define TIMECONV_JULIAN_DATE_START_OF_GPS_TIME (2444244.5)  // [days]
#define TIMECONV_JULIAN_DATE_START_OF_PC_TIME  (2440587.5)  // [days]
#define TIMECONV_DAYS_IN_JAN 31
#define TIMECONV_DAYS_IN_MAR 31
#define TIMECONV_DAYS_IN_APR 30
#define TIMECONV_DAYS_IN_MAY 31
#define TIMECONV_DAYS_IN_JUN 30
#define TIMECONV_DAYS_IN_JUL 31
#define TIMECONV_DAYS_IN_AUG 31
#define TIMECONV_DAYS_IN_SEP 30
#define TIMECONV_DAYS_IN_OCT 31
#define TIMECONV_DAYS_IN_NOV 30
#define TIMECONV_DAYS_IN_DEC 31


// A static function to check if the utc input values are valid.
// \return TRUE if valid, FALSE otherwise.
static BOOL TIMECONV_IsUTCTimeValid( 
 const unsigned short     utc_year,      //!< Universal Time Coordinated  [year]
 const unsigned char      utc_month,     //!< Universal Time Coordinated  [1-12 months] 
 const unsigned char      utc_day,       //!< Universal Time Coordinated  [1-31 days]
 const unsigned char      utc_hour,      //!< Universal Time Coordinated  [hours]
 const unsigned char      utc_minute,    //!< Universal Time Coordinated  [minutes]
 const float              utc_seconds    //!< Universal Time Coordinated  [s]
 );
 

BOOL TIMECONV_IsUTCTimeValid( 
 const unsigned short     utc_year,      //!< Universal Time Coordinated  [year]
 const unsigned char      utc_month,     //!< Universal Time Coordinated  [1-12 months] 
 const unsigned char      utc_day,       //!< Universal Time Coordinated  [1-31 days]
 const unsigned char      utc_hour,      //!< Universal Time Coordinated  [hours]
 const unsigned char      utc_minute,    //!< Universal Time Coordinated  [minutes]
 const float              utc_seconds    //!< Universal Time Coordinated  [s]
 )
{
  unsigned char daysInMonth;
  BOOL result;
  if( utc_month == 0 || utc_month > 12 )
  {
    GNSS_ERROR_MSG( "if( utc_month == 0 || utc_month > 12 )" );
    return FALSE;
  }
  result = TIMECONV_GetNumberOfDaysInMonth( utc_year, utc_month, &daysInMonth );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_GetNumberOfDaysInMonth returned FALSE." );       
    return FALSE;
  }
  if( utc_day == 0 || utc_day > daysInMonth )
  {
    GNSS_ERROR_MSG( "if( utc_day == 0 || utc_day > daysInMonth )" );
    return FALSE;
  }
  if( utc_hour > 23 )
  {
    GNSS_ERROR_MSG( "if( utc_hour > 23 )" );
    return FALSE;
  }
  if( utc_minute > 59 )
  {
    GNSS_ERROR_MSG( "if( utc_minute > 59 )" );
    return FALSE;
  }
  if( utc_seconds > 60 )
  {
    GNSS_ERROR_MSG( "if( utc_seconds > 60 )" );
    return FALSE;
  }

  return TRUE;
}


BOOL TIMECONV_GetSystemTime(
  unsigned short*     utc_year,     //!< Universal Time Coordinated    [year]
  unsigned char*      utc_month,    //!< Universal Time Coordinated    [1-12 months] 
  unsigned char*      utc_day,      //!< Universal Time Coordinated    [1-31 days]
  unsigned char*      utc_hour,     //!< Universal Time Coordinated    [hours]
  unsigned char*      utc_minute,   //!< Universal Time Coordinated    [minutes]
  float*              utc_seconds,  //!< Universal Time Coordinated    [s]
  unsigned char*      utc_offset,   //!< Integer seconds that GPS is ahead of UTC time, always positive             [s], obtained from a look up table
  double*             julian_date,  //!< Number of days since noon Universal Time Jan 1, 4713 BCE (Julian calendar) [days]
  unsigned short*     gps_week,     //!< GPS week (0-1024+)            [week]
  double*             gps_tow       //!< GPS time of week (0-604800.0) [s]
  )
{
  BOOL result;

#if defined(WIN32) && !defined(__GNUC__)
  struct _timeb timebuffer; // found in <sys/timeb.h>   
#else
  struct timeb timebuffer;
#endif
  double timebuffer_time_in_days;
  double timebuffer_time_in_seconds;
  //char *timeline; // for debugging

#ifdef _CRT_SECURE_NO_DEPRECATE
  if( _ftime_s( &timebuffer ) != 0 )
  {
    GNSS_ERROR_MSG( "if( _ftime_s( &timebuffer ) != 0 )" );
    return FALSE;
  }
#else

#if defined(WIN32) && !defined(__GNUC__)
  _ftime64_s( &timebuffer );
#else
  ftime( &timebuffer );
#endif 

#endif
  //timeline = ctime( & ( timebuffer.time ) ); // for debugging
  //printf( "%s\n", timeline ); // for debugging

  timebuffer_time_in_seconds = timebuffer.time + timebuffer.millitm / 1000.0; // [s] with ms resolution

  // timebuffer_time_in_seconds is the time in seconds since midnight (00:00:00), January 1, 1970, 
  // coordinated universal time (UTC). Julian date for (00:00:00), January 1, 1970 is: 2440587.5 [days]

  // convert timebuffer.time from seconds to days
  timebuffer_time_in_days = timebuffer_time_in_seconds/SECONDS_IN_DAY; // days since julian date 2440587.5000000 [days]

  // convert to julian date
  *julian_date = TIMECONV_JULIAN_DATE_START_OF_PC_TIME + timebuffer_time_in_days;

  result = TIMECONV_DetermineUTCOffset( *julian_date, utc_offset );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_DetermineUTCOffset returned FALSE." );
    return FALSE;
  }

  result = TIMECONV_GetGPSTimeFromJulianDate(
    *julian_date,
    *utc_offset,
    gps_week,
    gps_tow );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_GetGPSTimeFromJulianDate returned FALSE." );
    return FALSE;
  }

  result = TIMECONV_GetUTCTimeFromJulianDate(
    *julian_date,
    utc_year,
    utc_month,
    utc_day,
    utc_hour,
    utc_minute,
    utc_seconds );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_GetUTCTimeFromJulianDate" );
    return FALSE;
  }

  return TRUE;
}


#ifdef WIN32
BOOL TIMECONV_SetSystemTime(
  const unsigned short  utc_year,     //!< Universal Time Coordinated    [year]
  const unsigned char   utc_month,    //!< Universal Time Coordinated    [1-12 months] 
  const unsigned char   utc_day,      //!< Universal Time Coordinated    [1-31 days]
  const unsigned char   utc_hour,     //!< Universal Time Coordinated    [hours]
  const unsigned char   utc_minute,   //!< Universal Time Coordinated    [minutes]
  const float           utc_seconds   //!< Universal Time Coordinated    [s]
  )
{
  BOOL result;
  SYSTEMTIME t;
  double julian_date = 0;
  unsigned char day_of_week = 0;

  result = TIMECONV_GetJulianDateFromUTCTime(
    utc_year,
    utc_month,
    utc_day,
    utc_hour,
    utc_minute,
    utc_seconds,
    &julian_date
    );
  if( !result )
  {
    GNSS_ERROR_MSG( "TIMECONV_GetJulianDateFromUTCTime returned FALSE.");
    return FALSE;
  }

  result = TIMECONV_GetDayOfWeekFromJulianDate( julian_date, &day_of_week );
  if( !result )
  {
    GNSS_ERROR_MSG( "TIMECONV_GetDayOfWeekFromJulianDate returned FALSE.");
    return FALSE;
  }
  
  t.wDayOfWeek = day_of_week;
  t.wYear = utc_year;
  t.wMonth = utc_month;
  t.wDay = utc_day;
  t.wHour = utc_hour;
  t.wMinute = utc_minute;
  t.wSecond = (WORD)(floor(utc_seconds));
  t.wMilliseconds = (WORD)((utc_seconds - t.wSecond)*1000);

  // Set the PC system time.
  result = SetSystemTime( &t );
  
  return result;
}
#endif


BOOL TIMECONV_GetDayOfWeekFromJulianDate(
  const double julian_date,   //!< Number of days since noon Universal Time Jan 1, 4713 BCE (Julian calendar) [days]
  unsigned char *day_of_week  //!< 0-Sunday, 1-Monday, 2-Tuesday, 3-Wednesday, 4-Thursday, 5-Friday, 6-Saturday [].
  )
{
  // "If the Julian date of noon is applied to the entire midnight-to-midnight civil 
  // day centered on that noon,[5] rounding Julian dates (fractional days) for the 
  // twelve hours before noon up while rounding those after noon down, then the remainder 
  // upon division by 7 represents the day of the week, with 0 representing Monday, 
  // 1 representing Tuesday, and so forth. Now at 17:48, Wednesday December 3 2008 (UTC) 
  // the nearest noon JDN is 2454804 yielding a remainder of 2." (http://en.wikipedia.org/wiki/Julian_day, 2008-12-03)
  int dow = 0;
  int jd = 0;

  if( julian_date - floor(julian_date) > 0.5 )
  {
    jd = (int)floor(julian_date+0.5);
  }
  else
  {
    jd = (int)floor(julian_date);
  }
  dow = jd%7; // 0 is monday, 1 is tuesday, etc

  switch( dow )
  {
    case 0: *day_of_week = 1; break;
    case 1: *day_of_week = 2; break;
    case 2: *day_of_week = 3; break;
    case 3: *day_of_week = 4; break;
    case 4: *day_of_week = 5; break;
    case 5: *day_of_week = 6; break;
    case 6: *day_of_week = 0; break;
    default: return FALSE; break;
  }
  
  return TRUE;
}


BOOL TIMECONV_GetJulianDateFromGPSTime(
  const unsigned short    gps_week,      //!< GPS week (0-1024+)             [week]
  const double            gps_tow,       //!< GPS time of week (0-604800.0)  [s]
  const unsigned char     utc_offset,    //!< Integer seconds that GPS is ahead of UTC time, always positive [s]
  double*                 julian_date    //!< Number of days since noon Universal Time Jan 1, 4713 BCE (Julian calendar) [days]
  )
{   
  if( gps_tow < 0.0  || gps_tow > 604800.0 )
  {
    GNSS_ERROR_MSG( "if( gps_tow < 0.0  || gps_tow > 604800.0 )" );
    return FALSE;  
  }

  // GPS time is ahead of UTC time and Julian time by the UTC offset
  *julian_date = ((double)gps_week + (gps_tow-(double)utc_offset)/604800.0)*7.0 + TIMECONV_JULIAN_DATE_START_OF_GPS_TIME;  
  return TRUE;
}


BOOL TIMECONV_GetJulianDateFromUTCTime(
 const unsigned short     utc_year,      //!< Universal Time Coordinated  [year]
 const unsigned char      utc_month,     //!< Universal Time Coordinated  [1-12 months] 
 const unsigned char      utc_day,       //!< Universal Time Coordinated  [1-31 days]
 const unsigned char      utc_hour,      //!< Universal Time Coordinated  [hours]
 const unsigned char      utc_minute,    //!< Universal Time Coordinated  [minutes]
 const float              utc_seconds,   //!< Universal Time Coordinated  [s]
 double*                  julian_date    //!< Number of days since noon Universal Time Jan 1, 4713 BCE (Julian calendar) [days]
 )
{   
  double y; // temp for year
  double m; // temp for month
  BOOL result;

  // Check the input.
  result = TIMECONV_IsUTCTimeValid( utc_year, utc_month, utc_day, utc_hour, utc_minute, utc_seconds );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_IsUTCTimeValid returned FALSE." );
    return FALSE;
  }

  if( utc_month <= 2 )
  {
    y = utc_year - 1;
    m = utc_month + 12;
  }
  else 
  {
    y = utc_year;
    m = utc_month;
  }

  *julian_date = (int)(365.25*y) + (int)(30.6001*(m+1.0)) + utc_day + utc_hour/24.0 + utc_minute/1440.0 + utc_seconds/86400.0 + 1720981.5;
  return TRUE;
}



BOOL TIMECONV_GetGPSTimeFromJulianDate(
  const double            julian_date, //!< Number of days since noon Universal Time Jan 1, 4713 BCE (Julian calendar) [days]
  const unsigned char     utc_offset,  //!< Integer seconds that GPS is ahead of UTC time, always positive [s]
  unsigned short*         gps_week,    //!< GPS week (0-1024+)            [week]
  double*                 gps_tow      //!< GPS time of week [s]
  )
{
  // Check the input.
  if( julian_date < 0.0 )
  {
    GNSS_ERROR_MSG( "if( julian_date < 0.0 )" );
    return FALSE;
  }

  *gps_week = (unsigned short)((julian_date - TIMECONV_JULIAN_DATE_START_OF_GPS_TIME)/7.0); //

  *gps_tow   = (julian_date - TIMECONV_JULIAN_DATE_START_OF_GPS_TIME)*SECONDS_IN_DAY; // seconds since start of gps time [s]
  *gps_tow  -= (*gps_week)*SECONDS_IN_WEEK;                                  // seconds into the current week [s] 

  // however, GPS time is ahead of utc time by the UTC offset (and thus the Julian date as well)
  *gps_tow += utc_offset;
  if( *gps_tow > SECONDS_IN_WEEK )
  {
    *gps_tow  -= SECONDS_IN_WEEK;
    *gps_week += 1;
  }
  return TRUE;
}


BOOL TIMECONV_GetUTCTimeFromJulianDate(
  const double        julian_date,  //!< Number of days since noon Universal Time Jan 1, 4713 BCE (Julian calendar) [days]
  unsigned short*     utc_year,     //!< Universal Time Coordinated    [year]
  unsigned char*      utc_month,    //!< Universal Time Coordinated    [1-12 months] 
  unsigned char*      utc_day,      //!< Universal Time Coordinated    [1-31 days]
  unsigned char*      utc_hour,     //!< Universal Time Coordinated    [hours]
  unsigned char*      utc_minute,   //!< Universal Time Coordinated    [minutes]
  float*              utc_seconds   //!< Universal Time Coordinated    [s]
  )
{
  int a, b, c, d, e; // temporary values
  
  unsigned short year;  
  unsigned char month;
  unsigned char day;
  unsigned char hour;
  unsigned char minute;        
  unsigned char days_in_month = 0;  
  double td; // temporary double
  double seconds;
  BOOL result;

  // Check the input.
  if( julian_date < 0.0 )
  {
    GNSS_ERROR_MSG( "if( julian_date < 0.0 )" );
    return FALSE;
  }
  
  a = (int)(julian_date+0.5);
  b = a + 1537;
  c = (int)( ((double)b-122.1)/365.25 );
  d = (int)(365.25*c);
  e = (int)( ((double)(b-d))/30.6001 );
  
  td      = b - d - (int)(30.6001*e) + fmod( julian_date+0.5, 1.0 );   // [days]
  day     = (unsigned char)td;   
  td     -= day;
  td     *= 24.0;        // [hours]
  hour    = (unsigned char)td;
  td     -= hour;
  td     *= 60.0;        // [minutes]
  minute  = (unsigned char)td;
  td     -= minute;
  td     *= 60.0;        // [s]
  seconds = td;
  month   = (unsigned char)(e - 1 - 12*(int)(e/14));
  year    = (unsigned short)(c - 4715 - (int)( (7.0+(double)month) / 10.0 ));
  
  // check for rollover issues
  if( seconds >= 60.0 )
  {
    seconds -= 60.0;
    minute++;
    if( minute >= 60 )
    {
      minute -= 60;
      hour++;
      if( hour >= 24 )
      {
        hour -= 24;
        day++;
        
        result = TIMECONV_GetNumberOfDaysInMonth( year, month, &days_in_month );
        if( result == FALSE )
        {
          GNSS_ERROR_MSG( "TIMECONV_GetNumberOfDaysInMonth returned FALSE." );
          return FALSE;
        }
        
        if( day > days_in_month )
        {
          day = 1;
          month++;
          if( month > 12 )
          {
            month = 1;
            year++;
          }
        }
      }
    }
  }   
  
  *utc_year       = year;
  *utc_month      = month;
  *utc_day        = day;
  *utc_hour       = hour;
  *utc_minute     = minute;
  *utc_seconds    = (float)seconds;   

  return TRUE;
}

BOOL TIMECONV_GetGPSTimeFromUTCTime(
  unsigned short     utc_year,     //!< Universal Time Coordinated    [year]
  unsigned char      utc_month,    //!< Universal Time Coordinated    [1-12 months] 
  unsigned char      utc_day,      //!< Universal Time Coordinated    [1-31 days]
  unsigned char      utc_hour,     //!< Universal Time Coordinated    [hours]
  unsigned char      utc_minute,   //!< Universal Time Coordinated    [minutes]
  float              utc_seconds,  //!< Universal Time Coordinated    [s]
  unsigned short*    gps_week,     //!< GPS week (0-1024+)            [week]
  double*            gps_tow       //!< GPS time of week (0-604800.0) [s]
  )
{
  double julian_date=0.0;
  unsigned char utc_offset=0;
  BOOL result;

  // Check the input.
  result = TIMECONV_IsUTCTimeValid( utc_year, utc_month, utc_day, utc_hour, utc_minute, utc_seconds );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_IsUTCTimeValid returned FALSE." );
    return FALSE;
  }

  result = TIMECONV_GetJulianDateFromUTCTime(
    utc_year,
    utc_month,
    utc_day,
    utc_hour,
    utc_minute,
    utc_seconds,
    &julian_date );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_GetJulianDateFromUTCTime returned FALSE." );
    return FALSE;
  }

  result = TIMECONV_DetermineUTCOffset( julian_date, &utc_offset );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_DetermineUTCOffset returned FALSE." );
    return FALSE;
  }

  result = TIMECONV_GetGPSTimeFromJulianDate(
    julian_date,
    utc_offset,
    gps_week,
    gps_tow );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_GetGPSTimeFromJulianDate returned FALSE." );
    return FALSE;
  }

  return TRUE;
}



BOOL TIMECONV_GetGPSTimeFromRinexTime(
  unsigned short     utc_year,     //!< Universal Time Coordinated    [year]
  unsigned char      utc_month,    //!< Universal Time Coordinated    [1-12 months] 
  unsigned char      utc_day,      //!< Universal Time Coordinated    [1-31 days]
  unsigned char      utc_hour,     //!< Universal Time Coordinated    [hours]
  unsigned char      utc_minute,   //!< Universal Time Coordinated    [minutes]
  float              utc_seconds,  //!< Universal Time Coordinated    [s]
  unsigned short*    gps_week,     //!< GPS week (0-1024+)            [week]
  double*            gps_tow       //!< GPS time of week (0-604800.0) [s]
  )
{
  double julian_date=0.0;
  unsigned char utc_offset=0;
  BOOL result;

  // Check the input.
  result = TIMECONV_IsUTCTimeValid( utc_year, utc_month, utc_day, utc_hour, utc_minute, utc_seconds );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_IsUTCTimeValid returned FALSE." );
    return FALSE;
  }

  result = TIMECONV_GetJulianDateFromUTCTime(
    utc_year,
    utc_month,
    utc_day,
    utc_hour,
    utc_minute,
    utc_seconds,
    &julian_date );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_GetJulianDateFromUTCTime returned FALSE." );
    return FALSE;
  }

  result = TIMECONV_GetGPSTimeFromJulianDate(
    julian_date,
    utc_offset,
    gps_week,
    gps_tow );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_GetGPSTimeFromJulianDate returned FALSE." );
    return FALSE;
  }

  return TRUE;
}



BOOL TIMECONV_GetUTCTimeFromGPSTime(
  unsigned short     gps_week,     //!< GPS week (0-1024+)            [week]
  double             gps_tow,      //!< GPS time of week (0-604800.0) [s]
  unsigned short*    utc_year,     //!< Universal Time Coordinated    [year]
  unsigned char*     utc_month,    //!< Universal Time Coordinated    [1-12 months] 
  unsigned char*     utc_day,      //!< Universal Time Coordinated    [1-31 days]
  unsigned char*     utc_hour,     //!< Universal Time Coordinated    [hours]
  unsigned char*     utc_minute,   //!< Universal Time Coordinated    [minutes]
  float*             utc_seconds   //!< Universal Time Coordinated    [s]
  )
{
  double julian_date = 0.0; 
  unsigned char utc_offset = 0;
  int i;
  BOOL result;

  if( gps_tow < 0.0 || gps_tow > 604800.0 )
  {
    GNSS_ERROR_MSG( "if( gps_tow < 0.0 || gps_tow > 604800.0 )" );
    return FALSE;  
  }

  // iterate to get the right utc offset
  for( i = 0; i < 4; i++ )
  {
    result = TIMECONV_GetJulianDateFromGPSTime(
      gps_week,
      gps_tow,
      utc_offset,
      &julian_date );
    if( result == FALSE )
    {
      GNSS_ERROR_MSG( "TIMECONV_GetJulianDateFromGPSTime returned FALSE." );
      return FALSE;
    }

    result = TIMECONV_DetermineUTCOffset( julian_date, &utc_offset );
    if( result == FALSE )
    {
      GNSS_ERROR_MSG( "TIMECONV_DetermineUTCOffset returned FALSE." );
      return FALSE;
    }
  }

  result = TIMECONV_GetUTCTimeFromJulianDate(
    julian_date,
    utc_year,
    utc_month,
    utc_day,
    utc_hour,
    utc_minute,
    utc_seconds );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_GetUTCTimeFromJulianDate returned FALSE." );
    return FALSE;
  }

  return TRUE;
}



BOOL TIMECONV_DetermineUTCOffset(
  double julian_date,       //!< Number of days since noon Universal Time Jan 1, 4713 BCE (Julian calendar) [days]
  unsigned char* utc_offset //!< Integer seconds that GPS is ahead of UTC time, always positive             [s], obtained from a look up table
  )
{
  if( julian_date < 0.0 )
  {
    GNSS_ERROR_MSG( "if( julian_date < 0.0 )" );
    return FALSE;
  }

  if(      julian_date < 2444786.5000 ) *utc_offset = 0;
  else if( julian_date < 2445151.5000 ) *utc_offset = 1;
  else if( julian_date < 2445516.5000 ) *utc_offset = 2;
  else if( julian_date < 2446247.5000 ) *utc_offset = 3;
  else if( julian_date < 2447161.5000 ) *utc_offset = 4;
  else if( julian_date < 2447892.5000 ) *utc_offset = 5;
  else if( julian_date < 2448257.5000 ) *utc_offset = 6;
  else if( julian_date < 2448804.5000 ) *utc_offset = 7;
  else if( julian_date < 2449169.5000 ) *utc_offset = 8;
  else if( julian_date < 2449534.5000 ) *utc_offset = 9;
  else if( julian_date < 2450083.5000 ) *utc_offset = 10;
  else if( julian_date < 2450630.5000 ) *utc_offset = 11;
  else if( julian_date < 2451179.5000 ) *utc_offset = 12;  
  else if( julian_date < 2453736.5000 ) *utc_offset = 13;  
  else if( julian_date < 2454832.5000 ) *utc_offset = 14;  // 12/Sep/2009 (KA): updated for leap second at 2008-12-31 23:59:60
  else                                  *utc_offset = 15;
/*
 * 12/Sep/2009 (KA): The following program will print out the required julian date for the next leap second (after editting year/month).
 * #include <iostream>
 * #include <iomanip>
 * #include "time_conversion.h"
 * void main() {
 *     double julianDate;
 *     if (TIMECONV_GetJulianDateFromUTCTime(2008, 12, 31, 23, 59, 60.0, &julianDate))
 *         std::cout << "utc:2008-12-31 23:59:60 --> julian:" << std::setprecision(12) << julianDate << std::endl;
 * }
 */

  return TRUE;
}  


  

BOOL TIMECONV_GetNumberOfDaysInMonth(
  const unsigned short year,        //!< Universal Time Coordinated    [year]
  const unsigned char month,        //!< Universal Time Coordinated    [1-12 months] 
  unsigned char* days_in_month      //!< Days in the specified month   [1-28|29|30|31 days]
  )
{
  BOOL is_a_leapyear;
  unsigned char utmp = 0;
  
  is_a_leapyear = TIMECONV_IsALeapYear( year );
  
  switch(month)
  {
  case  1: utmp = TIMECONV_DAYS_IN_JAN; break;
  case  2: if( is_a_leapyear ){ utmp = 29; }else{ utmp = 28; }break;    
  case  3: utmp = TIMECONV_DAYS_IN_MAR; break;
  case  4: utmp = TIMECONV_DAYS_IN_APR; break;
  case  5: utmp = TIMECONV_DAYS_IN_MAY; break;
  case  6: utmp = TIMECONV_DAYS_IN_JUN; break;
  case  7: utmp = TIMECONV_DAYS_IN_JUL; break;
  case  8: utmp = TIMECONV_DAYS_IN_AUG; break;
  case  9: utmp = TIMECONV_DAYS_IN_SEP; break;
  case 10: utmp = TIMECONV_DAYS_IN_OCT; break;
  case 11: utmp = TIMECONV_DAYS_IN_NOV; break;
  case 12: utmp = TIMECONV_DAYS_IN_DEC; break;
  default: 
    { 
      GNSS_ERROR_MSG( "unexpected default case." ); 
      return FALSE; 
      break;    
    }
  }
  
  *days_in_month = utmp;

  return TRUE;
}

  


BOOL TIMECONV_IsALeapYear( const unsigned short year )
{
  BOOL is_a_leap_year = FALSE;

  if( (year%4) == 0 )
  {
    is_a_leap_year = TRUE;
    if( (year%100) == 0 )
    {
      if( (year%400) == 0 )
      {
        is_a_leap_year = TRUE;
      }
      else
      {
        is_a_leap_year = FALSE;
      }
    }
  }
  if( is_a_leap_year )
  {
    return TRUE;
  }
  else
  {    
    return FALSE;
  }
}





BOOL TIMECONV_GetDayOfYear(
  const unsigned short utc_year,    // Universal Time Coordinated           [year]
  const unsigned char  utc_month,   // Universal Time Coordinated           [1-12 months] 
  const unsigned char  utc_day,     // Universal Time Coordinated           [1-31 days]
  unsigned short*      dayofyear    // number of days into the year (1-366) [days]
  )
{
  unsigned char days_in_feb = 0;
  BOOL result;
  result = TIMECONV_GetNumberOfDaysInMonth( utc_year, 2, &days_in_feb );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_GetNumberOfDaysInMonth returned FALSE." );
    return FALSE;
  }

  switch( utc_month )
  {
  case  1: *dayofyear = utc_day; break;
  case  2: *dayofyear = (unsigned short)(TIMECONV_DAYS_IN_JAN               + utc_day); break;
  case  3: *dayofyear = (unsigned short)(TIMECONV_DAYS_IN_JAN + days_in_feb + utc_day); break;
  case  4: *dayofyear = (unsigned short)(62          + days_in_feb + utc_day); break;
  case  5: *dayofyear = (unsigned short)(92          + days_in_feb + utc_day); break;
  case  6: *dayofyear = (unsigned short)(123         + days_in_feb + utc_day); break;
  case  7: *dayofyear = (unsigned short)(153         + days_in_feb + utc_day); break;
  case  8: *dayofyear = (unsigned short)(184         + days_in_feb + utc_day); break;
  case  9: *dayofyear = (unsigned short)(215         + days_in_feb + utc_day); break;
  case 10: *dayofyear = (unsigned short)(245         + days_in_feb + utc_day); break;
  case 11: *dayofyear = (unsigned short)(276         + days_in_feb + utc_day); break;
  case 12: *dayofyear = (unsigned short)(306         + days_in_feb + utc_day); break;
  default: 
    {
      GNSS_ERROR_MSG( "unexpected default case." );
      return FALSE; 
      break;
    }
  }

  return TRUE;
}


BOOL TIMECONV_GetGPSTimeFromYearAndDayOfYear(
 const unsigned short year,      // The year [year]
 const unsigned short dayofyear, // The number of days into the year (1-366) [days]
 unsigned short*      gps_week,  //!< GPS week (0-1024+)            [week]
 double*              gps_tow    //!< GPS time of week (0-604800.0) [s]
 )
{
  BOOL result;
  double julian_date = 0;

  if( gps_week == NULL )
  {
    GNSS_ERROR_MSG( "if( gps_week == NULL )" );
    return FALSE;
  }
  if( gps_tow == NULL )
  {
    GNSS_ERROR_MSG( "if( gps_tow == NULL )" );
    return FALSE;
  }
  if( dayofyear > 366 )
  {
    GNSS_ERROR_MSG( "if( dayofyear > 366 )" );
    return FALSE;
  }

  result = TIMECONV_GetJulianDateFromUTCTime(
    year,
    1,
    1,
    0,
    0,
    0,
    &julian_date 
    );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_GetJulianDateFromUTCTime returned FALSE." );
    return FALSE;
  }

  julian_date += dayofyear - 1; // at the start of the day so -1.

  result = TIMECONV_GetGPSTimeFromJulianDate(
    julian_date,
    0,
    gps_week,
    gps_tow );
  if( result == FALSE )
  {
    GNSS_ERROR_MSG( "TIMECONV_GetGPSTimeFromJulianDate returned FALSE." );
    return FALSE;
  }

  return TRUE;
}
