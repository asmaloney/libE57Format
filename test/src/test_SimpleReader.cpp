// libE57Format testing Copyright © 2022 Andy Maloney <asmaloney@gmail.com>
// SPDX-License-Identifier: BSL-1.0

#include "gtest/gtest.h"

#include "E57SimpleReader.h"

#include "Helpers.h"
#include "TestData.h"

namespace
{
   void CheckFileHeader( const e57::E57Root &fileHeader )
   {
      // These are invariant.
      // See ASTM Standard Table 12.

      EXPECT_EQ( fileHeader.formatName, "ASTM E57 3D Imaging Data File" );
      EXPECT_EQ( fileHeader.versionMajor, 1 );
      EXPECT_EQ( fileHeader.versionMinor, 0 );
   }
}

TEST( SimpleReader, PathError )
{
   E57_ASSERT_THROW( e57::Reader( "./no-path/empty.e57", {} ) );
}

TEST( SimpleReaderData, Empty )
{
   e57::Reader *reader = nullptr;

   E57_ASSERT_NO_THROW( reader = new e57::Reader( TestData::Path() + "/self/empty.e57", {} ) );

   ASSERT_TRUE( reader->IsOpen() );
   EXPECT_EQ( reader->GetImage2DCount(), 0 );
   EXPECT_EQ( reader->GetData3DCount(), 0 );

   e57::E57Root fileHeader;
   ASSERT_TRUE( reader->GetE57Root( fileHeader ) );

   CheckFileHeader( fileHeader );
   EXPECT_EQ( fileHeader.guid, "Empty File GUID" );

   delete reader;
}

TEST( SimpleReaderData, ZeroPoints )
{
   e57::Reader *reader = nullptr;

   E57_ASSERT_NO_THROW( reader = new e57::Reader( TestData::Path() + "/self/ZeroPoints.e57", {} ) );

   ASSERT_TRUE( reader->IsOpen() );
   EXPECT_EQ( reader->GetImage2DCount(), 0 );
   EXPECT_EQ( reader->GetData3DCount(), 1 );

   e57::E57Root fileHeader;
   ASSERT_TRUE( reader->GetE57Root( fileHeader ) );

   CheckFileHeader( fileHeader );
   EXPECT_EQ( fileHeader.guid, "Zero Points GUID" );

   e57::Data3D data3DHeader;
   ASSERT_TRUE( reader->ReadData3D( 0, data3DHeader ) );

   ASSERT_EQ( data3DHeader.pointCount, 0 );

   const uint64_t cNumPoints = data3DHeader.pointCount;

   e57::Data3DPointsFloat pointsData( data3DHeader );

   auto vectorReader = reader->SetUpData3DPointsData( 0, cNumPoints, pointsData );

   const uint64_t cNumRead = vectorReader.read();

   vectorReader.close();

   EXPECT_EQ( cNumRead, cNumPoints );

   delete reader;
}

TEST( SimpleReaderData, ZeroPointsInvalid )
{
   e57::Reader *reader = nullptr;

   E57_ASSERT_NO_THROW(
      reader = new e57::Reader( TestData::Path() + "/self/ZeroPointsInvalid.e57", {} ) );

   ASSERT_TRUE( reader->IsOpen() );
   EXPECT_EQ( reader->GetImage2DCount(), 0 );
   EXPECT_EQ( reader->GetData3DCount(), 1 );

   e57::E57Root fileHeader;
   ASSERT_TRUE( reader->GetE57Root( fileHeader ) );

   CheckFileHeader( fileHeader );
   EXPECT_EQ( fileHeader.guid, "{EC1A0DE4-F76F-44CE-E527-789EEB818347}" );

   e57::Data3D data3DHeader;
   ASSERT_TRUE( reader->ReadData3D( 0, data3DHeader ) );

   ASSERT_EQ( data3DHeader.pointCount, 0 );

   const uint64_t cNumPoints = data3DHeader.pointCount;

   e57::Data3DPointsFloat pointsData( data3DHeader );

   E57_ASSERT_THROW( auto vectorReader =
                        reader->SetUpData3DPointsData( 0, cNumPoints, pointsData ); );

   delete reader;
}

TEST( SimpleReaderData, InvalidCVHeader )
{
   e57::Reader *reader = nullptr;

   E57_ASSERT_NO_THROW( reader =
                           new e57::Reader( TestData::Path() + "/self/InvalidCVHeader.e57", {} ) );

   ASSERT_TRUE( reader->IsOpen() );
   EXPECT_EQ( reader->GetImage2DCount(), 0 );
   EXPECT_EQ( reader->GetData3DCount(), 1 );

   e57::E57Root fileHeader;
   ASSERT_TRUE( reader->GetE57Root( fileHeader ) );

   CheckFileHeader( fileHeader );
   EXPECT_EQ( fileHeader.guid, "InvalidCVHeader GUID" );

   e57::Data3D data3DHeader;
   ASSERT_TRUE( reader->ReadData3D( 0, data3DHeader ) );

   const uint64_t cNumPoints = data3DHeader.pointCount;

   e57::Data3DPointsFloat pointsData( data3DHeader );

// This test should fail if validation is ON, but pass if it is OFF
#if VALIDATE_BASIC
   E57_ASSERT_THROW( {
      auto vectorReader = reader->SetUpData3DPointsData( 0, cNumPoints, pointsData );

      vectorReader.close();
   } );
#else
   E57_ASSERT_NO_THROW( {
      auto vectorReader = reader->SetUpData3DPointsData( 0, cNumPoints, pointsData );

      vectorReader.close();
   } );
#endif

   delete reader;
}

TEST( SimpleReaderData, BadCRC )
{
   E57_ASSERT_THROW( e57::Reader( TestData::Path() + "/self/bad-crc.e57", {} ) );
}

TEST( SimpleReaderData, DoNotCheckCRC )
{
   E57_ASSERT_NO_THROW(
      e57::Reader( TestData::Path() + "/self/bad-crc.e57", { e57::ChecksumNone } ) );
}

// https://github.com/asmaloney/libE57Format/issues/26
TEST( SimpleReaderData, ChineseFileName )
{
   E57_ASSERT_NO_THROW( e57::Reader( TestData::Path() + "/self/测试点云.e57", {} ) );
}

// https://github.com/asmaloney/libE57Format/issues/69
TEST( SimpleReaderData, UmlautFileName )
{
   E57_ASSERT_NO_THROW( e57::Reader( TestData::Path() + "/self/test filename äöü.e57", {} ) );
}

TEST( SimpleReaderData, ColouredCubeFloat )
{
   e57::Reader *reader = nullptr;

   E57_ASSERT_NO_THROW(
      reader = new e57::Reader( TestData::Path() + "/self/ColouredCubeFloat.e57", {} ) );

   ASSERT_TRUE( reader->IsOpen() );
   EXPECT_EQ( reader->GetImage2DCount(), 0 );
   ASSERT_EQ( reader->GetData3DCount(), 1 );

   e57::E57Root fileHeader;
   ASSERT_TRUE( reader->GetE57Root( fileHeader ) );

   CheckFileHeader( fileHeader );
   EXPECT_EQ( fileHeader.guid, "Coloured Cube File GUID" );

   e57::Data3D data3DHeader;
   ASSERT_TRUE( reader->ReadData3D( 0, data3DHeader ) );

   ASSERT_EQ( data3DHeader.pointCount, 7'680 );
   EXPECT_EQ( data3DHeader.guid, "Coloured Cube Float Scan Header GUID" );

   const uint64_t cNumPoints = data3DHeader.pointCount;

   e57::Data3DPointsFloat pointsData( data3DHeader );

   auto vectorReader = reader->SetUpData3DPointsData( 0, cNumPoints, pointsData );

   const uint64_t cNumRead = vectorReader.read();

   vectorReader.close();

   EXPECT_EQ( cNumRead, cNumPoints );

   delete reader;
}

TEST( SimpleReaderData, ColouredCubeFloatToDouble )
{
   e57::Reader *reader = nullptr;

   E57_ASSERT_NO_THROW(
      reader = new e57::Reader( TestData::Path() + "/self/ColouredCubeFloat.e57", {} ) );

   ASSERT_TRUE( reader->IsOpen() );
   EXPECT_EQ( reader->GetImage2DCount(), 0 );
   ASSERT_EQ( reader->GetData3DCount(), 1 );

   e57::E57Root fileHeader;
   ASSERT_TRUE( reader->GetE57Root( fileHeader ) );

   CheckFileHeader( fileHeader );
   EXPECT_EQ( fileHeader.guid, "Coloured Cube File GUID" );

   e57::Data3D data3DHeader;
   ASSERT_TRUE( reader->ReadData3D( 0, data3DHeader ) );

   ASSERT_EQ( data3DHeader.pointCount, 7'680 );
   EXPECT_EQ( data3DHeader.guid, "Coloured Cube Float Scan Header GUID" );

   const uint64_t cNumPoints = data3DHeader.pointCount;

   e57::Data3DPointsDouble pointsData( data3DHeader );

   auto vectorReader = reader->SetUpData3DPointsData( 0, cNumPoints, pointsData );

   const uint64_t cNumRead = vectorReader.read();

   vectorReader.close();

   EXPECT_EQ( cNumRead, cNumPoints );

   delete reader;
}

TEST( SimpleReaderData, BunnyDouble )
{
   e57::Reader *reader = nullptr;

   E57_ASSERT_NO_THROW( reader =
                           new e57::Reader( TestData::Path() + "/reference/bunnyDouble.e57", {} ) );

   ASSERT_TRUE( reader->IsOpen() );
   EXPECT_EQ( reader->GetImage2DCount(), 0 );
   ASSERT_EQ( reader->GetData3DCount(), 1 );

   e57::E57Root fileHeader;
   ASSERT_TRUE( reader->GetE57Root( fileHeader ) );

   CheckFileHeader( fileHeader );
   EXPECT_EQ( fileHeader.guid, "{19AA90ED-145E-4B3B-922C-80BC00648844}" );

   e57::Data3D data3DHeader;
   ASSERT_TRUE( reader->ReadData3D( 0, data3DHeader ) );

   ASSERT_EQ( data3DHeader.pointCount, 30'571 );
   EXPECT_EQ( data3DHeader.guid, "{9CA24C38-C93E-40E8-A366-F49977C7E3EB}" );

   const uint64_t cNumPoints = data3DHeader.pointCount;

   e57::Data3DPointsFloat pointsData( data3DHeader );

   auto vectorReader = reader->SetUpData3DPointsData( 0, cNumPoints, pointsData );

   const uint64_t cNumRead = vectorReader.read();

   vectorReader.close();

   EXPECT_EQ( cNumRead, cNumPoints );

   delete reader;
}

TEST( SimpleReaderData, BunnyInt32 )
{
   e57::Reader *reader = nullptr;

   E57_ASSERT_NO_THROW( reader =
                           new e57::Reader( TestData::Path() + "/reference/bunnyInt32.e57", {} ) );

   ASSERT_TRUE( reader->IsOpen() );
   EXPECT_EQ( reader->GetImage2DCount(), 0 );
   ASSERT_EQ( reader->GetData3DCount(), 1 );

   e57::E57Root fileHeader;
   ASSERT_TRUE( reader->GetE57Root( fileHeader ) );

   CheckFileHeader( fileHeader );
   EXPECT_EQ( fileHeader.guid, "{991574D2-854C-4CEF-8CB8-D0132E4BCD0A}" );

   e57::Data3D data3DHeader;
   ASSERT_TRUE( reader->ReadData3D( 0, data3DHeader ) );

   EXPECT_EQ( data3DHeader.pointCount, 30'571 );
   EXPECT_EQ( data3DHeader.guid, "{9CA24C38-C93E-40E8-A366-F49977C7E3EB}" );

   const uint64_t cNumPoints = data3DHeader.pointCount;

   e57::Data3DPointsFloat pointsData( data3DHeader );

   auto vectorReader = reader->SetUpData3DPointsData( 0, cNumPoints, pointsData );

   const uint64_t cNumRead = vectorReader.read();

   vectorReader.close();

   EXPECT_EQ( cNumRead, cNumPoints );

   delete reader;
}

TEST( SimpleReaderData, ColourRepresentation )
{
   e57::Reader *reader = nullptr;

   E57_ASSERT_NO_THROW( reader = new e57::Reader(
                           TestData::Path() + "/3rdParty/las2e57/ColourRepresentation.e57", {} ) );

   ASSERT_TRUE( reader->IsOpen() );
   EXPECT_EQ( reader->GetImage2DCount(), 0 );
   ASSERT_EQ( reader->GetData3DCount(), 1 );

   e57::E57Root fileHeader;
   ASSERT_TRUE( reader->GetE57Root( fileHeader ) );

   CheckFileHeader( fileHeader );
   EXPECT_EQ( fileHeader.guid, "6107aa44-6289-4e9c-80bd-f36cc3fbd44b" );

   e57::Data3D data3DHeader;
   ASSERT_TRUE( reader->ReadData3D( 0, data3DHeader ) );

   ASSERT_EQ( data3DHeader.pointCount, 153 );
   EXPECT_EQ( data3DHeader.guid, "98d85152-82b3-4120-b06e-0c1bb10b6dec" );

   const uint64_t cNumPoints = data3DHeader.pointCount;

   e57::Data3DPointsFloat pointsData( data3DHeader );

   auto vectorReader = reader->SetUpData3DPointsData( 0, cNumPoints, pointsData );

   uint64_t cNumRead = 0;
   E57_ASSERT_NO_THROW( cNumRead = vectorReader.read() );

   vectorReader.close();

   EXPECT_EQ( cNumRead, cNumPoints );

   delete reader;
}


TEST( SimpleReaderData, PinholeImageWithDistortionParameters )
{
   e57::Reader *reader = nullptr;

   E57_ASSERT_NO_THROW( reader = new e57::Reader(
                           TestData::Path() + "/self/PinholeImageWithDistortionParameters.e57", {} ) );

   ASSERT_TRUE( reader->IsOpen() );
   EXPECT_EQ( reader->GetImage2DCount(), 3 );
   EXPECT_EQ( reader->GetData3DCount(), 0 );

   e57::E57Root fileHeader;
   ASSERT_TRUE( reader->GetE57Root( fileHeader ) );

   CheckFileHeader( fileHeader );
   EXPECT_EQ( fileHeader.guid, "Pinhole image with distortion parameters GUID" );

   {
      // test reading image with distortion parameters set
      e57::Image2D image2Dheader;
      ASSERT_TRUE( reader->ReadImage2D( 0, image2Dheader ) );

      EXPECT_EQ( image2Dheader.pinholeRepresentation.imageWidth, 225 );
      EXPECT_EQ( image2Dheader.pinholeRepresentation.imageHeight, 300 );

      const auto& pcd = image2Dheader.pinholeCameraDistortion;
      EXPECT_TRUE( pcd );
      EXPECT_EQ( pcd->cameraNumber, 1 );
      EXPECT_EQ( pcd->type, "Testing type" );
      EXPECT_FLOAT_EQ( pcd->CV_K1, 1.01 );
      EXPECT_FLOAT_EQ( pcd->CV_K2, 2.02 );
      EXPECT_FLOAT_EQ( pcd->CV_K3, 3.03 );
      EXPECT_FLOAT_EQ( pcd->CV_K4, 4.04 );
      EXPECT_FLOAT_EQ( pcd->CV_K5, 5.05 );
      EXPECT_FLOAT_EQ( pcd->CV_K6, 6.06 );
      EXPECT_FLOAT_EQ( pcd->CV_P1, 11.11 );
      EXPECT_FLOAT_EQ( pcd->CV_P2, 12.12 );
      EXPECT_FLOAT_EQ( pcd->CV_CX, 21.21 );
      EXPECT_FLOAT_EQ( pcd->CV_CY, 22.22 );
      EXPECT_FLOAT_EQ( pcd->CV_FX, 31.31 );
      EXPECT_FLOAT_EQ( pcd->CV_FY, 32.32 );
      EXPECT_EQ( pcd->CV_HEIGHT, 225 );
      EXPECT_EQ( pcd->CV_WIDTH, 300 );
   }

   {
      // test reading image with some distortion parameters omitted
      e57::Image2D image2Dheader;
      EXPECT_TRUE( reader->ReadImage2D( 1, image2Dheader ) );

      EXPECT_EQ( image2Dheader.pinholeRepresentation.imageWidth, 225 );
      EXPECT_EQ( image2Dheader.pinholeRepresentation.imageHeight, 300 );

      const auto& pcd = image2Dheader.pinholeCameraDistortion;
      EXPECT_TRUE( pcd );
      EXPECT_EQ( pcd->cameraNumber, 2 );
      EXPECT_EQ( pcd->type, "" );
      EXPECT_FLOAT_EQ( pcd->CV_K1, 1.01 );
      EXPECT_FLOAT_EQ( pcd->CV_K2, 2.02 );
      EXPECT_FLOAT_EQ( pcd->CV_K3, 0 );
      EXPECT_FLOAT_EQ( pcd->CV_K4, 0 );
      EXPECT_FLOAT_EQ( pcd->CV_K5, 0 );
      EXPECT_FLOAT_EQ( pcd->CV_K6, 0 );
      EXPECT_FLOAT_EQ( pcd->CV_P1, 0 );
      EXPECT_FLOAT_EQ( pcd->CV_P2, 0 );
      EXPECT_FLOAT_EQ( pcd->CV_CX, 0 );
      EXPECT_FLOAT_EQ( pcd->CV_CY, 0 );
      EXPECT_FLOAT_EQ( pcd->CV_FX, 0 );
      EXPECT_FLOAT_EQ( pcd->CV_FY, 0 );
      EXPECT_EQ( pcd->CV_HEIGHT, 225 );
      EXPECT_EQ( pcd->CV_WIDTH, 300 );
   }

   {
      // test reading image without distortion header
      e57::Image2D image2Dheader;
      EXPECT_TRUE( reader->ReadImage2D( 2, image2Dheader ) );

      EXPECT_EQ( image2Dheader.pinholeRepresentation.imageWidth, 225 );
      EXPECT_EQ( image2Dheader.pinholeRepresentation.imageHeight, 300 );

      EXPECT_FALSE( image2Dheader.pinholeCameraDistortion );
   }

   delete reader;
}