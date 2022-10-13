// libE57Format testing Copyright © 2022 Andy Maloney <asmaloney@gmail.com>
// SPDX-License-Identifier: MIT

#include <fstream>

#include "gtest/gtest.h"

#include "E57SimpleWriter.h"

#include "Helpers.h"
#include "TestData.h"

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

   e57::Data3DPointsData pointsData;
   pointsData.cartesianX = new float[cNumPoints];
   pointsData.cartesianY = new float[cNumPoints];
   pointsData.cartesianZ = new float[cNumPoints];

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

   delete[] pointsData.cartesianX;
   delete[] pointsData.cartesianY;
   delete[] pointsData.cartesianZ;

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

   e57::Data3DPointsData pointsData;
   pointsData.cartesianX = new float[cNumPoints];
   pointsData.cartesianY = new float[cNumPoints];
   pointsData.cartesianZ = new float[cNumPoints];
   pointsData.colorRed = new uint8_t[cNumPoints];
   pointsData.colorGreen = new uint8_t[cNumPoints];
   pointsData.colorBlue = new uint8_t[cNumPoints];

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

   delete[] pointsData.cartesianX;
   delete[] pointsData.cartesianY;
   delete[] pointsData.cartesianZ;
   delete[] pointsData.colorRed;
   delete[] pointsData.colorGreen;
   delete[] pointsData.colorBlue;

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
