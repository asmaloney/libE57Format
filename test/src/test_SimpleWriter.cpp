// libE57Format testing Copyright © 2022 Andy Maloney <asmaloney@gmail.com>
// SPDX-License-Identifier: BSL-1.0

#include <array>
#include <fstream>

#include "gtest/gtest.h"

#include "E57SimpleWriter.h"

#include "Helpers.h"
#include "RandomNum.h"
#include "TestData.h"

namespace
{
   constexpr uint8_t cNumAxes = 3;
   constexpr uint8_t cNumCubeFaces = 6;

   using Point = std::array<float, cNumAxes>;
   using Cube = std::array<Point, 8>;

   constexpr auto cCubeCorners =
      Cube{ Point{ -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f },
            Point{ -0.5f, 0.5f, -0.5f },  { -0.5f, 0.5f, 0.5f },  { 0.5f, 0.5f, 0.5f },
            Point{ 0.5f, -0.5f, 0.5f },   { -0.5f, -0.5f, 0.5f } };

   constexpr auto cIndexSequence = std::make_index_sequence<3>{};

   template <class T, size_t... Is, size_t N>
   constexpr std::array<T, N> multiply( std::array<T, N> const &src, std::index_sequence<Is...>,
                                        T const &mul )
   {
      return std::array<T, N>{ { ( src[Is] * mul )... } };
   }

   // Call a function for each of the corner points for a cube centred on the origin, sized using
   // cubeSize.
   void generateCubeCornerPoints( float inCubeSize,
                                  const std::function<void( const Point &inPoint )> &lambda )
   {
      for ( const auto &point : cCubeCorners )
      {
         const auto cSized = multiply( point, cIndexSequence, inCubeSize );
         lambda( cSized );
      }
   }

   // Create "inPointsPerFace" pseudo-random points per face for a cube centred on the origin,
   // sized using cubeSize, and call a function for each of them.
   // This gives us a cube with non-uniform points which can be useful for testing.
   // (Note that we set a seed in each test so the results will always be the same pseudo-random
   // points.)
   void generateCubePoints(
      float inCubeSize, uint16_t inPointsPerFace,
      const std::function<void( uint8_t inFace, const Point &inPoint )> &lambda )
   {
      Point point;

      const auto cHalfSize = inCubeSize / 2.0f;

      for ( uint8_t face = 0; face < cNumCubeFaces; ++face )
      {
         const uint8_t axis = face % cNumAxes;

         for ( uint16_t i = 0; i < inPointsPerFace; ++i )
         {
            point[axis] = ( face > 2 ) ? cHalfSize : -cHalfSize;
            point[( axis + 1 ) % cNumAxes] = Random::num() * inCubeSize - cHalfSize;
            point[( axis + 2 ) % cNumAxes] = Random::num() * inCubeSize - cHalfSize;

            lambda( face, point );
         }
      }
   }

   // Set the header to indicate we are using coloured cartesian points.
   // Just used to avoid a bunch of repetition.
   inline void setUsingColouredCartesianPoints( e57::Data3D &ioHeader )
   {
      ioHeader.pointFields.cartesianXField = true;
      ioHeader.pointFields.cartesianYField = true;
      ioHeader.pointFields.cartesianZField = true;

      ioHeader.pointFields.colorRedField = true;
      ioHeader.pointFields.colorGreenField = true;
      ioHeader.pointFields.colorBlueField = true;

      ioHeader.colorLimits.colorRedMaximum = 255;
      ioHeader.colorLimits.colorGreenMaximum = 255;
      ioHeader.colorLimits.colorBlueMaximum = 255;
   }

   // Fill in a point and its colour given an index, which face it is on, and the data.
   template <typename T>
   inline void fillColouredCartesianPoint( e57::Data3DPointsData_t<T> &ioPointsData,
                                           int64_t inIndex, uint8_t inFace, const Point &inPoint )
   {
      static_assert( std::is_floating_point<T>::value, "Floating point type required." );

      ioPointsData.cartesianX[inIndex] = inPoint[0];
      ioPointsData.cartesianY[inIndex] = inPoint[1];
      ioPointsData.cartesianZ[inIndex] = inPoint[2];

      switch ( inFace )
      {
         case 0:
         case 3:
         {
            ioPointsData.colorRed[inIndex] = 0;
            ioPointsData.colorGreen[inIndex] = 0;
            ioPointsData.colorBlue[inIndex] = 255;
            break;
         }
         case 1:
         case 4:
         {
            ioPointsData.colorRed[inIndex] = 0;
            ioPointsData.colorGreen[inIndex] = 255;
            ioPointsData.colorBlue[inIndex] = 0;
            break;
         }
         case 2:
         case 5:
         {
            ioPointsData.colorRed[inIndex] = 255;
            ioPointsData.colorGreen[inIndex] = 0;
            ioPointsData.colorBlue[inIndex] = 0;
            break;
         }
      }
   }
}

TEST( SimpleWriter, PathError )
{
   e57::WriterOptions options;
   options.guid = "File GUID";

   E57_ASSERT_THROW( e57::Writer( "./no-path/empty.e57", options ) );
}

TEST( SimpleWriter, Empty )
{
   e57::WriterOptions options;
   options.guid = "Empty File GUID";

   E57_ASSERT_NO_THROW( e57::Writer writer( "./empty.e57", options ) );
}

TEST( SimpleWriter, ZeroPoints )
{
   e57::WriterOptions options;
   options.guid = "Zero Points GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./ZeroPoints.e57", options ) );

   constexpr uint16_t cNumPoints = 0;

   e57::Data3D header;
   header.guid = "Zero Points Header GUID";
   header.pointCount = cNumPoints;

   // For a valid file, we need to specify cartesian or spherical.
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;

   e57::Data3DPointsDouble pointsData( header );

   E57_ASSERT_NO_THROW( writer->WriteData3DData( header, pointsData ) );

   delete writer;
}

TEST( SimpleWriter, InvalidData3DValueCartesian )
{
   e57::WriterOptions options;
   options.guid = "Invalid Data3D Value Cartesian GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./InvalidData3DValueCartesian.e57", options ) );

   constexpr uint16_t cNumPoints = 1;

   e57::Data3D header;
   header.guid = "Invalid Data3D Value Cartesian Header GUID";
   header.pointCount = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;
   header.pointFields.pointRangeNodeType = e57::NumericalNodeType::ScaledInteger;

   // since we are requesting a scaled int, leaving pointRangeScale as 0.0 should throw an exception

   e57::Data3DPointsDouble pointsData( header );

   E57_ASSERT_THROW( writer->WriteData3DData( header, pointsData ) );

   delete writer;
}

TEST( SimpleWriter, InvalidData3DValueSpherical )
{
   e57::WriterOptions options;
   options.guid = "Invalid Data3D Value Spherical GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./InvalidData3DValueSpherical.e57", options ) );

   constexpr uint16_t cNumPoints = 1;

   e57::Data3D header;
   header.guid = "Invalid Data3D Value Spherical Header GUID";
   header.pointCount = cNumPoints;
   header.pointFields.sphericalRangeField = true;
   header.pointFields.sphericalAzimuthField = true;
   header.pointFields.sphericalElevationField = true;
   header.pointFields.angleNodeType = e57::NumericalNodeType::ScaledInteger;

   // since we are requesting a scaled int, leaving angleScale as 0.0 should throw an exception

   e57::Data3DPointsDouble pointsData( header );

   E57_ASSERT_THROW( writer->WriteData3DData( header, pointsData ) );

   delete writer;
}

// Write a coloured cube of points using doubles.
TEST( SimpleWriter, ColouredCubeDouble )
{
   Random::seed( 42 );

   e57::WriterOptions options;
   options.guid = "Coloured Cube File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./ColouredCubeDouble.e57", options ) );

   constexpr uint16_t cNumPointsPerFace = 1280;
   constexpr uint16_t cNumPoints = cNumPointsPerFace * cNumCubeFaces;

   e57::Data3D header;
   header.guid = "Coloured Cube Double Scan Header GUID";
   header.description = "libE57Format test: cube of coloured points using doubles";
   header.pointCount = cNumPoints;

   setUsingColouredCartesianPoints( header );

   e57::Data3DPointsDouble pointsData( header );

   int64_t i = 0;
   auto writePointLambda = [&]( uint8_t inFace, const Point &inPoint ) {
      ASSERT_LE( i, cNumPoints );

      fillColouredCartesianPoint( pointsData, i, inFace, inPoint );

      ++i;
   };

   generateCubePoints( 1.0, cNumPointsPerFace, writePointLambda );

   writer->WriteData3DData( header, pointsData );

   delete writer;
}

// Write a coloured cube of points using floats.
TEST( SimpleWriter, ColouredCubeFloat )
{
   Random::seed( 42 );

   e57::WriterOptions options;
   options.guid = "Coloured Cube File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./ColouredCubeFloat.e57", options ) );

   constexpr uint16_t cNumPointsPerFace = 1280;
   constexpr uint16_t cNumPoints = cNumPointsPerFace * cNumCubeFaces;

   e57::Data3D header;
   header.guid = "Coloured Cube Float Scan Header GUID";
   header.description = "libE57Format test: cube of coloured points using floats";
   header.pointCount = cNumPoints;

   setUsingColouredCartesianPoints( header );

   e57::Data3DPointsFloat pointsData( header );

   int64_t i = 0;
   auto writePointLambda = [&]( uint8_t inFace, const Point &inPoint ) {
      ASSERT_LE( i, cNumPoints );

      fillColouredCartesianPoint( pointsData, i, inFace, inPoint );

      ++i;
   };

   generateCubePoints( 1.0, cNumPointsPerFace, writePointLambda );

   writer->WriteData3DData( header, pointsData );

   delete writer;
}

// Write a cube of points using scaled ints.
TEST( SimpleWriter, ColouredCubeScaledInt )
{
   Random::seed( 42 );

   e57::WriterOptions options;
   options.guid = "Coloured Cube Scaled Int File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./ColouredCubeScaledInt.e57", options ) );

   constexpr uint16_t cNumPointsPerFace = 1280;
   constexpr uint16_t cNumPoints = cNumPointsPerFace * cNumCubeFaces;

   e57::Data3D header;
   header.guid = "Cube Scaled Int Scan Header GUID";
   header.description = "libE57Format test: cube of coloured points using scaled integers";
   header.pointCount = cNumPoints;

   header.pointFields.pointRangeNodeType = e57::NumericalNodeType::ScaledInteger;
   header.pointFields.pointRangeScale = 0.001;

   setUsingColouredCartesianPoints( header );

   e57::Data3DPointsDouble pointsData( header );

   int64_t i = 0;
   auto writePointLambda = [&]( uint8_t inFace, const Point &inPoint ) {
      ASSERT_LE( i, cNumPoints );

      fillColouredCartesianPoint( pointsData, i, inFace, inPoint );

      ++i;
   };

   generateCubePoints( 1.0, cNumPointsPerFace, writePointLambda );

   writer->WriteData3DData( header, pointsData );

   delete writer;
}

TEST( SimpleWriter, MultipleScans )
{
   e57::WriterOptions options;
   options.guid = "Multiple Scans File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./MultipleScans.e57", options ) );

   constexpr int cNumPoints = 8;

   e57::Data3D header;
   header.pointCount = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;

   e57::Data3DPointsFloat pointsData( header );

   // scan 1
   header.guid = "Multiple Scans Scan 1 Header GUID";

   int64_t i = 0;
   auto writePointLambda = [&]( const Point &point ) {
      pointsData.cartesianX[i] = point[0];
      pointsData.cartesianY[i] = point[1];
      pointsData.cartesianZ[i] = point[2];
      ++i;
   };

   generateCubeCornerPoints( 1.0, writePointLambda );

   writer->WriteData3DData( header, pointsData );

   // scan 2
   header.guid = "Multiple Scans Scan 2 Header GUID";

   i = 0;
   generateCubeCornerPoints( 0.5, writePointLambda );

   writer->WriteData3DData( header, pointsData );

   delete writer;
}

// https://github.com/asmaloney/libE57Format/issues/26
TEST( SimpleWriter, ChineseFileName )
{
   e57::WriterOptions options;
   options.guid = "Chinese File Name File GUID";

   E57_ASSERT_NO_THROW( e57::Writer writer( "./测试点云.e57", options ) );
}

// https://github.com/asmaloney/libE57Format/issues/69
TEST( SimpleWriter, WriteUmlautFileName )
{
   e57::WriterOptions options;
   options.guid = "Umlaut File Name File GUID";

   E57_ASSERT_NO_THROW( e57::Writer writer( "./test filename äöü.e57", options ) );
}

TEST( SimpleWriter, CartesianPoints )
{
   e57::WriterOptions options;
   options.guid = "Cartesian Points File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./CartesianPoints-1025.e57", options ) );

   constexpr int64_t cNumPoints = 1025;

   e57::Data3D header;
   header.guid = "Cartesian Points Header GUID";
   header.pointCount = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;

   e57::Data3DPointsFloat pointsData( header );

   for ( int64_t i = 0; i < cNumPoints; ++i )
   {
      auto floati = static_cast<float>( i );
      pointsData.cartesianX[i] = floati;
      pointsData.cartesianY[i] = floati;
      pointsData.cartesianZ[i] = floati;
   }

   writer->WriteData3DData( header, pointsData );

   // Check that the cartesian bounds were filled in properly
   EXPECT_EQ( header.cartesianBounds.xMinimum, 0 );
   EXPECT_EQ( header.cartesianBounds.xMaximum, cNumPoints - 1 );
   EXPECT_EQ( header.cartesianBounds.yMinimum, 0 );
   EXPECT_EQ( header.cartesianBounds.yMaximum, cNumPoints - 1 );
   EXPECT_EQ( header.cartesianBounds.zMinimum, 0 );
   EXPECT_EQ( header.cartesianBounds.zMaximum, cNumPoints - 1 );

   delete writer;
}

TEST( SimpleWriter, ColouredCartesianPoints )
{
   e57::WriterOptions options;
   options.guid = "Coloured Cartesian Points File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./ColouredCartesianPoints-1025.e57", options ) );

   constexpr int64_t cNumPoints = 1025;

   e57::Data3D header;
   header.guid = "Coloured Cartesian Points Header GUID";
   header.pointCount = cNumPoints;

   setUsingColouredCartesianPoints( header );

   e57::Data3DPointsFloat pointsData( header );

   for ( int64_t i = 0; i < cNumPoints; ++i )
   {
      auto floati = static_cast<float>( i );
      pointsData.cartesianX[i] = floati;
      pointsData.cartesianY[i] = floati;
      pointsData.cartesianZ[i] = floati;

      pointsData.colorRed[i] = 0;
      pointsData.colorGreen[i] = 0;
      pointsData.colorBlue[i] = 255;
   }

   writer->WriteData3DData( header, pointsData );

   delete writer;
}

// https://github.com/asmaloney/libE57Format/issues/160
TEST( SimpleWriter, MinMaxIssuesCartesianFloat )
{
   e57::WriterOptions options;
   options.guid = "Cartesian Points Min/Max Float File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./CartesianPointsMinMaxFloat.e57", options ) );

   constexpr int64_t cNumPoints = 1025;

   e57::Data3D header;
   header.guid = "Cartesian Points Min/Max Float Header GUID";
   header.pointCount = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;

   // Using any of these (as ScaledInteger) without setting their min/max explicitly should not fail
   header.pointFields.pointRangeNodeType = e57::NumericalNodeType::ScaledInteger;
   header.pointFields.pointRangeScale = 0.1;
   header.pointFields.timeStampField = true;
   header.pointFields.timeNodeType = e57::NumericalNodeType::ScaledInteger;
   header.pointFields.timeScale = 0.1;

   e57::Data3DPointsFloat pointsData( header );

   for ( int64_t i = 0; i < cNumPoints; ++i )
   {
      auto floati = static_cast<float>( i );
      pointsData.cartesianX[i] = floati;
      pointsData.cartesianY[i] = floati;
      pointsData.cartesianZ[i] = floati;
      pointsData.timeStamp[i] = floati;
   }

   try
   {
      writer->WriteData3DData( header, pointsData );
   }
   catch ( e57::E57Exception &err )
   {
      FAIL() << err.errorStr() << ": " << err.context();
   }

   delete writer;

   EXPECT_NE( header.pointFields.pointRangeMinimum, e57::FLOAT_MIN );
   EXPECT_NE( header.pointFields.pointRangeMaximum, e57::FLOAT_MAX );

   EXPECT_EQ( header.pointFields.timeMinimum, 0.0 );
   EXPECT_EQ( header.pointFields.timeMaximum, 1024.0 );
}

// https://github.com/asmaloney/libE57Format/issues/160
TEST( SimpleWriter, MinMaxIssuesSphericalDouble )
{
   e57::WriterOptions options;
   options.guid = "Spherical Points Min/Max Double File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./SphericalPointsMinMaxDouble.e57", options ) );

   constexpr int64_t cNumPoints = 1025;

   e57::Data3D header;
   header.guid = "Spherical Points Min/Max Double Header GUID";
   header.pointCount = cNumPoints;
   header.pointFields.sphericalRangeField = true;
   header.pointFields.sphericalAzimuthField = true;
   header.pointFields.sphericalElevationField = true;

   // Using any of these (as ScaledInteger) without setting their min/max explicitly should not fail
   header.pointFields.pointRangeNodeType = e57::NumericalNodeType::ScaledInteger;
   header.pointFields.pointRangeScale = 0.1;
   header.pointFields.timeStampField = true;
   header.pointFields.timeNodeType = e57::NumericalNodeType::ScaledInteger;
   header.pointFields.timeScale = 0.1;

   e57::Data3DPointsDouble pointsData( header );

   for ( int64_t i = 0; i < cNumPoints; ++i )
   {
      auto floati = static_cast<float>( i );
      pointsData.sphericalRange[i] = floati;
      pointsData.sphericalAzimuth[i] = floati;
      pointsData.sphericalElevation[i] = floati;
      pointsData.timeStamp[i] = floati;
   }

   try
   {
      writer->WriteData3DData( header, pointsData );
   }
   catch ( e57::E57Exception &err )
   {
      FAIL() << err.errorStr() << ": " << err.context();
   }

   delete writer;

   EXPECT_NE( header.pointFields.pointRangeMinimum, e57::DOUBLE_MIN );
   EXPECT_NE( header.pointFields.pointRangeMaximum, e57::DOUBLE_MAX );

   EXPECT_EQ( header.pointFields.timeMinimum, 0.0 );
   EXPECT_EQ( header.pointFields.timeMaximum, 1024.0 );
}

// Check that we don't overwrite the user's min/max data when we use the float version
TEST( SimpleWriter, MinMaxOverwriteUserDataFloat )
{
   e57::WriterOptions options;
   options.guid = "Min/Max Overwrite User Data Float File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./MinMaxOverwriteUserDataFloat.e57", options ) );

   constexpr int64_t cNumPoints = 10;

   e57::Data3D header;
   header.guid = "Min/Max Overwrite User Data Float Header GUID";
   header.pointCount = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;

   // Set a min/max on the time field
   header.pointFields.timeStampField = true;
   header.pointFields.timeNodeType = e57::NumericalNodeType::ScaledInteger;
   header.pointFields.timeScale = 0.1;
   header.pointFields.timeMinimum = -1.0;
   header.pointFields.timeMaximum = 2048.0;

   e57::Data3DPointsFloat pointsData( header );

   for ( int64_t i = 0; i < cNumPoints; ++i )
   {
      auto floati = static_cast<float>( i );
      pointsData.cartesianX[i] = floati;
      pointsData.cartesianY[i] = floati;
      pointsData.cartesianZ[i] = floati;
      pointsData.timeStamp[i] = floati + 0.1f;
   }

   try
   {
      writer->WriteData3DData( header, pointsData );
   }
   catch ( e57::E57Exception &err )
   {
      FAIL() << err.errorStr() << ": " << err.context();
   }

   delete writer;

   EXPECT_EQ( header.pointFields.timeMinimum, -1.0 );
   EXPECT_EQ( header.pointFields.timeMaximum, 2048.0 );
}

// Checks that an integer timestamp with a minimum > 0 does not cause an exception
TEST( SimpleWriter, MinMaxIssuesIntegerNode )
{
   e57::WriterOptions options;
   options.guid = "MinMax Issues Integer File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./MinMaxIssuesIntegerNode.e57", options ) );

   constexpr int64_t cNumPoints = 8;

   e57::Data3D header;
   header.guid = "MinMax Issues Integer Header GUID";
   header.pointCount = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;

   header.pointFields.intensityField = true;
   header.pointFields.intensityNodeType = e57::NumericalNodeType::Integer;

   header.pointFields.timeStampField = true;
   header.pointFields.timeNodeType = e57::NumericalNodeType::Integer;

   e57::Data3DPointsFloat pointsData( header );

   for ( int64_t i = 0; i < cNumPoints; ++i )
   {
      auto floati = static_cast<float>( i );
      pointsData.cartesianX[i] = floati;
      pointsData.cartesianY[i] = floati;
      pointsData.cartesianZ[i] = floati;
      pointsData.intensity[i] = ( i % 2 ) ? 1.0 : 0.0;
      pointsData.timeStamp[i] = floati + 1;
   }

   try
   {
      writer->WriteData3DData( header, pointsData );
   }
   catch ( e57::E57Exception &err )
   {
      FAIL() << err.errorStr() << ": " << err.context();
   }

   delete writer;

   EXPECT_EQ( header.pointFields.timeMinimum, 1.0 );
   EXPECT_EQ( header.pointFields.timeMaximum, 8.0 );
}

// Checks that a scaled integer timestamp with a minimum > 0 does not cause an exception
TEST( SimpleWriter, MinMaxIssuesScaledIntegerNode )
{
   e57::WriterOptions options;
   options.guid = "MinMax Issues Scaled Integer File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer =
                           new e57::Writer( "./MinMaxIssuesScaledIntegerNode.e57", options ) );

   constexpr int64_t cNumPoints = 8;

   e57::Data3D header;
   header.guid = "MinMax Issues Scaled Integer Header GUID";
   header.pointCount = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;

   header.pointFields.timeStampField = true;
   header.pointFields.timeNodeType = e57::NumericalNodeType::ScaledInteger;
   header.pointFields.timeScale = 0.1;

   e57::Data3DPointsFloat pointsData( header );

   for ( int64_t i = 0; i < cNumPoints; ++i )
   {
      auto floati = static_cast<float>( i );
      pointsData.cartesianX[i] = floati + 0.22;
      pointsData.cartesianY[i] = floati;
      pointsData.cartesianZ[i] = floati;
      pointsData.timeStamp[i] = floati + 1;
   }

   try
   {
      writer->WriteData3DData( header, pointsData );
   }
   catch ( e57::E57Exception &err )
   {
      FAIL() << err.errorStr() << ": " << err.context();
   }

   delete writer;

   EXPECT_EQ( header.pointFields.timeMinimum, 1.0 );
   EXPECT_EQ( header.pointFields.timeMaximum, 8.0 );
}

// Checks that a float timestamp with a minimum > 0.0 does not cause an exception
TEST( SimpleWriter, MinMaxIssuesFloatNode )
{
   e57::WriterOptions options;
   options.guid = "MinMax Issues Float File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./MinMaxIssuesFloatNode.e57", options ) );

   constexpr int64_t cNumPoints = 8;

   e57::Data3D header;
   header.guid = "MinMax Issues Float Header GUID";
   header.pointCount = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;

   header.pointFields.timeStampField = true;

   e57::Data3DPointsFloat pointsData( header );

   for ( int64_t i = 0; i < cNumPoints; ++i )
   {
      auto floati = static_cast<float>( i );
      pointsData.cartesianX[i] = floati;
      pointsData.cartesianY[i] = floati;
      pointsData.cartesianZ[i] = floati;
      pointsData.timeStamp[i] = floati + 1;
   }

   try
   {
      writer->WriteData3DData( header, pointsData );
   }
   catch ( e57::E57Exception &err )
   {
      FAIL() << err.errorStr() << ": " << err.context();
   }

   delete writer;

   EXPECT_EQ( header.pointFields.timeMinimum, 1.0 );
   EXPECT_EQ( header.pointFields.timeMaximum, 8.0 );
}

// Checks that a constant timestamp when using ScaledInteger with a scale != 1.0 does not cause an
// exception
TEST( SimpleWriter, ConstantIntegerNodeException )
{
   e57::WriterOptions options;
   options.guid = "Constant Integer Node Exception File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./ConstantIntegerNodeException.e57", options ) );

   constexpr int64_t cNumPoints = 8;

   e57::Data3D header;
   header.guid = "Constant Integer Node Exception Header GUID";
   header.pointCount = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;

   header.pointFields.timeStampField = true;
   header.pointFields.timeNodeType = e57::NumericalNodeType::ScaledInteger;
   header.pointFields.timeScale = 0.1;

   e57::Data3DPointsFloat pointsData( header );

   for ( int64_t i = 0; i < cNumPoints; ++i )
   {
      auto floati = static_cast<float>( i );
      pointsData.cartesianX[i] = floati;
      pointsData.cartesianY[i] = floati;
      pointsData.cartesianZ[i] = floati;
      pointsData.timeStamp[i] = 42.0;
   }

   try
   {
      writer->WriteData3DData( header, pointsData );
   }
   catch ( e57::E57Exception &err )
   {
      FAIL() << err.errorStr() << ": " << err.context();
   }

   delete writer;

   EXPECT_EQ( header.pointFields.timeMinimum, 42 );
   EXPECT_EQ( header.pointFields.timeMaximum, 42 );
}

TEST( SimpleWriterData, VisualRefImage )
{
   if ( !TestData::Exists() )
   {
      GTEST_SKIP_( "(test data not available)" );
   }

   e57::WriterOptions options;
   options.guid = "Visual Reference Image File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./VisualRefImage.e57", options ) );

   std::ifstream image( TestData::Path() + "/images/image.jpg",
                        std::ifstream::ate | std::ifstream::binary );

   ASSERT_EQ( image.rdstate(), std::ios_base::goodbit );

   const int64_t cImageSize = image.tellg();

   image.clear();
   image.seekg( 0 );

   auto imageBuffer = new char[cImageSize];

   image.read( imageBuffer, cImageSize );

   ASSERT_EQ( image.rdstate(), std::ios_base::goodbit );

   e57::Image2D image2DHeader;
   image2DHeader.name = "JPEG Image Test";
   image2DHeader.guid = "Visual Reference Image - JPEG Image GUID";
   image2DHeader.description = "JPEG image test";
   image2DHeader.visualReferenceRepresentation.imageWidth = 225;
   image2DHeader.visualReferenceRepresentation.imageHeight = 300;
   image2DHeader.visualReferenceRepresentation.jpegImageSize = cImageSize;

   writer->WriteImage2DData( image2DHeader, e57::ImageJPEG, e57::ProjectionVisual, 0, imageBuffer,
                             cImageSize );

   delete[] imageBuffer;

   delete writer;
}
