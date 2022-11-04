// libE57Format testing Copyright © 2022 Andy Maloney <asmaloney@gmail.com>
// SPDX-License-Identifier: MIT

#include "gtest/gtest.h"

#include "E57SimpleData.h"

#include "Helpers.h"

TEST( SimpleDataHeader, InvalidPointSize )
{
   e57::Data3D dataHeader;

   E57_ASSERT_THROW( e57::Data3DPointsData pointsData( dataHeader ) );
}

TEST( SimpleDataHeader, HeaderMinMaxFloat )
{
   e57::Data3D dataHeader;

   dataHeader.pointCount = 1;

   EXPECT_EQ( dataHeader.pointFields.pointRangeMinimum, e57::E57_DOUBLE_MIN );
   EXPECT_EQ( dataHeader.pointFields.pointRangeMaximum, e57::E57_DOUBLE_MAX );
   EXPECT_EQ( dataHeader.pointFields.angleMinimum, e57::E57_DOUBLE_MIN );
   EXPECT_EQ( dataHeader.pointFields.angleMaximum, e57::E57_DOUBLE_MAX );
   EXPECT_EQ( dataHeader.pointFields.timeMinimum, e57::E57_DOUBLE_MIN );
   EXPECT_EQ( dataHeader.pointFields.timeMaximum, e57::E57_DOUBLE_MAX );

   // This call should adjust our min/max for a variety of fields since we are using floats.
   e57::Data3DPointsData pointsData( dataHeader );

   EXPECT_EQ( dataHeader.pointFields.pointRangeMinimum, e57::E57_FLOAT_MIN );
   EXPECT_EQ( dataHeader.pointFields.pointRangeMaximum, e57::E57_FLOAT_MAX );
   EXPECT_EQ( dataHeader.pointFields.angleMinimum, e57::E57_FLOAT_MIN );
   EXPECT_EQ( dataHeader.pointFields.angleMaximum, e57::E57_FLOAT_MAX );
   EXPECT_EQ( dataHeader.pointFields.timeMinimum, e57::E57_FLOAT_MIN );
   EXPECT_EQ( dataHeader.pointFields.timeMaximum, e57::E57_FLOAT_MAX );
}

TEST( SimpleDataHeader, HeaderMinMaxDouble )
{
   e57::Data3D dataHeader;

   dataHeader.pointCount = 1;

   EXPECT_EQ( dataHeader.pointFields.pointRangeMinimum, e57::E57_DOUBLE_MIN );
   EXPECT_EQ( dataHeader.pointFields.pointRangeMaximum, e57::E57_DOUBLE_MAX );
   EXPECT_EQ( dataHeader.pointFields.angleMinimum, e57::E57_DOUBLE_MIN );
   EXPECT_EQ( dataHeader.pointFields.angleMaximum, e57::E57_DOUBLE_MAX );
   EXPECT_EQ( dataHeader.pointFields.timeMinimum, e57::E57_DOUBLE_MIN );
   EXPECT_EQ( dataHeader.pointFields.timeMaximum, e57::E57_DOUBLE_MAX );

   // This call should NOT adjust our min/max for a variety of fields since we are using doubles.
   e57::Data3DPointsData_d pointsData( dataHeader );

   EXPECT_EQ( dataHeader.pointFields.pointRangeMinimum, e57::E57_DOUBLE_MIN );
   EXPECT_EQ( dataHeader.pointFields.pointRangeMaximum, e57::E57_DOUBLE_MAX );
   EXPECT_EQ( dataHeader.pointFields.angleMinimum, e57::E57_DOUBLE_MIN );
   EXPECT_EQ( dataHeader.pointFields.angleMaximum, e57::E57_DOUBLE_MAX );
   EXPECT_EQ( dataHeader.pointFields.timeMinimum, e57::E57_DOUBLE_MIN );
   EXPECT_EQ( dataHeader.pointFields.timeMaximum, e57::E57_DOUBLE_MAX );
}
