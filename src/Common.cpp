// SPDX-License-Identifier: BSL-1.0
// Copyright (c) 2020 PTC Inc.

#include "Common.h"

#include <random>

namespace e57
{

   std::string generateRandomGUID()
   {
      static constexpr const char UUID_CHARS[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
      static std::random_device rd;
      static std::mt19937 gen( rd() );
      static std::uniform_int_distribution<> dis( 0, 61 /* number of chars in UUID_CHARS */ );

      std::string uuid( 36, ' ' );

      uuid[8] = '-';
      uuid[13] = '-';
      uuid[18] = '-';
      uuid[23] = '-';

      uuid[14] = '4';

      for ( int i = 0; i < 36; ++i )
      {
         if ( i != 8 && i != 13 && i != 18 && i != 14 && i != 23 )
         {
            uuid[i] = UUID_CHARS[dis( gen )];
         }
      }
      return uuid;
   }

} // end namespace e57
