// libE57Format testing Copyright © 2022 Andy Maloney <asmaloney@gmail.com>
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include "TestData.h"

int main( int argc, char **argv )
{
   ::testing::FLAGS_gtest_color = "yes";
   ::testing::InitGoogleTest( &argc, argv );

   // IF our data path doesn't exist, then exclude some tests.
   if ( !TestData::Exists() )
   {
      ::testing::GTEST_FLAG( filter ) = "-*Data.*";
   }

   return RUN_ALL_TESTS();
}
