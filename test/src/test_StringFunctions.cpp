// libE57Format testing Copyright © 2022 Andy Maloney <asmaloney@gmail.com>
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include "StringFunctions.h"

TEST( StringFunctions, FloatToStrConversion )
{
   const auto converted = e57::floatingPointToStr<float>( 3.14159265f, 7 );

   ASSERT_EQ( converted, "3.1415927e+00" );
}

TEST( StringFunctions, DoubleToStrConversion )
{
   const auto converted = e57::floatingPointToStr<double>( 3.141592653589793238, 17 );

   ASSERT_EQ( converted, "3.14159265358979312e+00" );
}

TEST( StringFunctions, FloatToStrConversion2 )
{
   const auto converted = e57::floatingPointToStr<float>( 123456.0f, 7 );

   ASSERT_EQ( converted, "1.2345600e+05" );
}
