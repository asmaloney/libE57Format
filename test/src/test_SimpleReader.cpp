// libE57Format testing Copyright © 2022 Andy Maloney <asmaloney@gmail.com>
// SPDX-License-Identifier: MIT

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

TEST( SimpleReaderData, ReadEmpty )
{
   e57::Reader *reader = nullptr;

   E57_ASSERT_NO_THROW( reader = new e57::Reader( TestData::Path() + "/self/empty.e57", {} ) );

   ASSERT_TRUE( reader->IsOpen() );
   EXPECT_EQ( reader->GetImage2DCount(), 0 );
   EXPECT_EQ( reader->GetData3DCount(), 0 );

   e57::E57Root fileHeader;
   ASSERT_TRUE( reader->GetE57Root( fileHeader ) );

   CheckFileHeader( fileHeader );
   EXPECT_EQ( fileHeader.guid, "File GUID" );

   delete reader;
}

TEST( SimpleReaderData, BadCRC )
{
   E57_ASSERT_THROW( e57::Reader( TestData::Path() + "/self/bad-crc.e57", {} ) );
}

TEST( SimpleReaderData, DoNotCheckCRC )
{
   E57_ASSERT_NO_THROW( e57::Reader( TestData::Path() + "/self/bad-crc.e57", { e57::CHECKSUM_POLICY_NONE } ) );
}

// https://github.com/asmaloney/libE57Format/issues/26
// File name UTF-8 encoded to avoid editor issues.
TEST( SimpleReaderData, ReadChineseFileName )
{
   E57_ASSERT_NO_THROW(
      e57::Reader( TestData::Path() + "/self/\xe6\xb5\x8b\xe8\xaf\x95\xe7\x82\xb9\xe4\xba\x91.e57", {} ) );
}

// https://github.com/asmaloney/libE57Format/issues/69
// File name UTF-8 encoded to avoid editor issues.
// No idea why this fails on Linux and Windows with "No such file or directory".
// TEST( SimpleReaderData, ReadUmlautFileName )
//{
//   E57_ASSERT_NO_THROW(
//      e57::Reader( TestData::Path() + "/self/test filename \x61\xcc\x88\x6f\xcc\x88\x75\xcc\x88.e57", {} ) );
//}

TEST( SimpleReaderData, ReadBunnyDouble )
{
   e57::Reader *reader = nullptr;

   E57_ASSERT_NO_THROW( reader = new e57::Reader( TestData::Path() + "/reference/bunnyDouble.e57", {} ) );

   ASSERT_TRUE( reader->IsOpen() );
   EXPECT_EQ( reader->GetImage2DCount(), 0 );
   ASSERT_EQ( reader->GetData3DCount(), 1 );

   e57::E57Root fileHeader;
   ASSERT_TRUE( reader->GetE57Root( fileHeader ) );

   CheckFileHeader( fileHeader );
   EXPECT_EQ( fileHeader.guid, "{19AA90ED-145E-4B3B-922C-80BC00648844}" );

   e57::Data3D data3DHeader;
   ASSERT_TRUE( reader->ReadData3D( 0, data3DHeader ) );

   ASSERT_EQ( data3DHeader.pointsSize, 30'571 );
   EXPECT_EQ( data3DHeader.guid, "{9CA24C38-C93E-40E8-A366-F49977C7E3EB}" );

   const uint64_t cNumPoints = data3DHeader.pointsSize;

   e57::Data3DPointsData pointsData( data3DHeader );

   auto vectorReader = reader->SetUpData3DPointsData( 0, cNumPoints, pointsData );

   const uint64_t cNumRead = vectorReader.read();

   vectorReader.close();

   EXPECT_EQ( cNumRead, cNumPoints );

   delete reader;
}

TEST( SimpleReaderData, ReadBunnyInt32 )
{
   e57::Reader *reader = nullptr;

   E57_ASSERT_NO_THROW( reader = new e57::Reader( TestData::Path() + "/reference/bunnyInt32.e57", {} ) );

   ASSERT_TRUE( reader->IsOpen() );
   EXPECT_EQ( reader->GetImage2DCount(), 0 );
   ASSERT_EQ( reader->GetData3DCount(), 1 );

   e57::E57Root fileHeader;
   ASSERT_TRUE( reader->GetE57Root( fileHeader ) );

   CheckFileHeader( fileHeader );
   EXPECT_EQ( fileHeader.guid, "{991574D2-854C-4CEF-8CB8-D0132E4BCD0A}" );

   e57::Data3D data3DHeader;
   ASSERT_TRUE( reader->ReadData3D( 0, data3DHeader ) );

   EXPECT_EQ( data3DHeader.pointsSize, 30'571 );
   EXPECT_EQ( data3DHeader.guid, "{9CA24C38-C93E-40E8-A366-F49977C7E3EB}" );

   const uint64_t cNumPoints = data3DHeader.pointsSize;

   e57::Data3DPointsData pointsData( data3DHeader );

   auto vectorReader = reader->SetUpData3DPointsData( 0, cNumPoints, pointsData );

   const uint64_t cNumRead = vectorReader.read();

   vectorReader.close();

   EXPECT_EQ( cNumRead, cNumPoints );

   delete reader;
}
