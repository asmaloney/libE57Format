// libE57Format testing Copyright © 2022 Andy Maloney <asmaloney@gmail.com>
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include "E57Version.h"

#include "RandomNum.h"
#include "TestData.h"

int main( int argc, char **argv )
{
   Random::seed( 42 );

   ::testing::FLAGS_gtest_color = "yes";
   ::testing::InitGoogleTest( &argc, argv );

   // IF our data path doesn't exist, then exclude some tests.
   if ( !TestData::Exists() )
   {
      ::testing::GTEST_FLAG( filter ) = "-*Data.*";
   }

   std::cout << "e57Format version: " << e57::Version::library() << std::endl;
   std::cout << "ASTM version: " << e57::Version::astm() << std::endl;

   int result = RUN_ALL_TESTS();

   return result;
}
