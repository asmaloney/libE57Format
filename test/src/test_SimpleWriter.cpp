// libE57Format testing Copyright © 2022 Andy Maloney <asmaloney@gmail.com>
// SPDX-License-Identifier: MIT

#include <array>
#include <fstream>

#include "gtest/gtest.h"

#include "E57SimpleWriter.h"

#include "Helpers.h"
#include "TestData.h"

namespace
{
   using Point = std::array<float, 3>;
   using Cube = std::array<Point, 8>;

   constexpr auto cCubeCorners =
      Cube{ Point{ -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f },
            Point{ -0.5f, 0.5f, 0.5f },   { 0.5f, 0.5f, 0.5f },   { 0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, 0.5f } };

   constexpr auto cIndexSequence = std::make_index_sequence<3>{};

   template <class T, size_t... Is, size_t N>
   constexpr std::array<T, N> multiply( std::array<T, N> const &src, std::index_sequence<Is...>, T const &mul )
   {
      return std::array<T, N>{ { ( src[Is] * mul )... } };
   }

   // Call a function for each of the corner points for a cube centred on the origin, sized using cubeSize.
   void generateCubePoints( float cubeSize, const std::function<void( const Point &point )> &lambda )
   {
      for ( const auto &point : cCubeCorners )
      {
         const auto cSized = multiply( point, cIndexSequence, cubeSize );
         lambda( cSized );
      }
   }
}

TEST( SimpleWriter, PathError )
{
   e57::WriterOptions options;
   options.guid = "File GUID";

   E57_ASSERT_THROW( e57::Writer( "./no-path/empty.e57", options ) );
}

TEST( SimpleWriter, WriteEmpty )
{
   e57::WriterOptions options;
   options.guid = "File GUID";

   E57_ASSERT_NO_THROW( e57::Writer writer( "./empty.e57", options ) );
}

TEST( SimpleWriter, WriteMultipleScans )
{
   e57::WriterOptions options;
   options.guid = "Multiple Scans File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./MultipleScans.e57", options ) );

   constexpr int cNumPoints = 8;

   e57::Data3D header;
   header.pointsSize = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;

   e57::Data3DPointsData pointsData( header );

   // scan 1
   header.guid = "Header Scan 1 GUID";

   const int64_t cScanIndex1 = writer->NewData3D( header );

   int64_t i = 0;
   auto writePointLambda = [&]( const Point &point ) {
      pointsData.cartesianX[i] = point[0];
      pointsData.cartesianY[i] = point[1];
      pointsData.cartesianZ[i] = point[2];
      ++i;
   };

   generateCubePoints( 1.0, writePointLambda );

   e57::CompressedVectorWriter dataWriter = writer->SetUpData3DPointsData( cScanIndex1, cNumPoints, pointsData );

   dataWriter.write( cNumPoints );
   dataWriter.close();

   // scan 2
   header.guid = "Header Scan 2 GUID";

   const int64_t cScanIndex2 = writer->NewData3D( header );

   i = 0;
   generateCubePoints( 0.5, writePointLambda );

   dataWriter = writer->SetUpData3DPointsData( cScanIndex2, cNumPoints, pointsData );

   dataWriter.write( cNumPoints );
   dataWriter.close();

   delete writer;
}

// https://github.com/asmaloney/libE57Format/issues/26
// File name UTF-8 encoded to avoid editor issues.
TEST( SimpleWriter, WriteChineseFileName )
{
   e57::WriterOptions options;
   options.guid = "File GUID";

   E57_ASSERT_NO_THROW( e57::Writer writer( "./\xe6\xb5\x8b\xe8\xaf\x95\xe7\x82\xb9\xe4\xba\x91.e57", options ) );
}

// https://github.com/asmaloney/libE57Format/issues/69
// File name UTF-8 encoded to avoid editor issues.
TEST( SimpleWriter, WriteUmlautFileName )
{
   e57::WriterOptions options;
   options.guid = "File GUID";

   E57_ASSERT_NO_THROW( e57::Writer writer( "./test filename \x61\xcc\x88\x6f\xcc\x88\x75\xcc\x88.e57", options ) );
}

TEST( SimpleWriter, WriteCartesianPoints )
{
   e57::WriterOptions options;
   options.guid = "File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./Cartesian-Points-1025.e57", options ) );

   constexpr int64_t cNumPoints = 1025;

   e57::Data3D header;
   header.guid = "Header GUID";
   header.pointsSize = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;

   const int64_t scanIndex = writer->NewData3D( header );

   e57::Data3DPointsData pointsData( header );

   for ( int64_t i = 0; i < cNumPoints; ++i )
   {
      auto floati = static_cast<float>( i );
      pointsData.cartesianX[i] = floati;
      pointsData.cartesianY[i] = floati;
      pointsData.cartesianZ[i] = floati;
   }

   e57::CompressedVectorWriter dataWriter = writer->SetUpData3DPointsData( scanIndex, cNumPoints, pointsData );

   dataWriter.write( cNumPoints );
   dataWriter.close();

   delete writer;
}

TEST( SimpleWriter, WriteColouredCartesianPoints )
{
   e57::WriterOptions options;
   options.guid = "File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./Coloured-Cartesian-Points-1025.e57", options ) );

   constexpr int64_t cNumPoints = 1025;

   e57::Data3D header;
   header.guid = "Header GUID";
   header.pointsSize = cNumPoints;
   header.pointFields.cartesianXField = true;
   header.pointFields.cartesianYField = true;
   header.pointFields.cartesianZField = true;
   header.pointFields.colorRedField = true;
   header.pointFields.colorGreenField = true;
   header.pointFields.colorBlueField = true;
   header.colorLimits.colorRedMaximum = 255;
   header.colorLimits.colorGreenMaximum = 255;
   header.colorLimits.colorBlueMaximum = 255;

   const int64_t scanIndex = writer->NewData3D( header );

   e57::Data3DPointsData pointsData( header );

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

   e57::CompressedVectorWriter dataWriter = writer->SetUpData3DPointsData( scanIndex, cNumPoints, pointsData );

   dataWriter.write( cNumPoints );
   dataWriter.close();

   delete writer;
}

TEST( SimpleWriterData, WriteVisualRefImage )
{
   e57::WriterOptions options;
   options.guid = "File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./VisualRefImage.e57", options ) );

   std::ifstream image( TestData::Path() + "/images/image.jpg", std::ifstream::ate | std::ifstream::binary );

   ASSERT_EQ( image.rdstate(), std::ios_base::goodbit );

   const int64_t cImageSize = image.tellg();

   image.clear();
   image.seekg( 0 );

   auto imageBuffer = new char[cImageSize];

   image.read( imageBuffer, cImageSize );

   ASSERT_EQ( image.rdstate(), std::ios_base::goodbit );

   e57::Image2D image2DHeader;
   image2DHeader.name = "JPEG Image Test";
   image2DHeader.guid = "JPEG Image GUID";
   image2DHeader.description = "JPEG image test";
   image2DHeader.visualReferenceRepresentation.imageWidth = 225;
   image2DHeader.visualReferenceRepresentation.imageHeight = 300;
   image2DHeader.visualReferenceRepresentation.jpegImageSize = cImageSize;

   int64_t imageIndex = writer->NewImage2D( image2DHeader );

   writer->WriteImage2DData( imageIndex, e57::E57_JPEG_IMAGE, e57::E57_VISUAL, imageBuffer, 0, cImageSize );

   delete[] imageBuffer;

   delete writer;
}
