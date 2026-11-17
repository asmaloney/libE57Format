// libE57Format testing Copyright © 2022 Andy Maloney <asmaloney@gmail.com>
// SPDX-License-Identifier: BSL-1.0

#include <fstream>

#include "gtest/gtest.h"

#include "E57SimpleWriter.h"

#include "Helpers.h"
#include "TestData.h"

namespace
{
   class SimpleWriterImageOneJPEG : public testing::Test
   {
   public:
      char *imageBuffer()
      {
         return mImageBuffer.data();
      }

      int64_t imageSize() const
      {
         return mImageBuffer.size();
      }

      void SetUp() override
      {
         std::ifstream image( TestData::Path() + "/images/image.jpg",
                              std::ifstream::ate | std::ifstream::binary );

         ASSERT_EQ( image.rdstate(), std::ios_base::goodbit );

         const int64_t cImageSize = image.tellg();

         image.clear();
         image.seekg( 0 );

         mImageBuffer.resize( cImageSize );

         image.read( mImageBuffer.data(), cImageSize );

         ASSERT_EQ( image.rdstate(), std::ios_base::goodbit );
      }

      void TearDown() override
      {
      }

   private:
      std::vector<char> mImageBuffer;
   };
}

TEST_F( SimpleWriterImageOneJPEG, VisualRefImage )
{
   if ( !TestData::Exists() )
   {
      GTEST_SKIP_( "(test data not available)" );
   }

   e57::WriterOptions options;
   options.guid = "Visual Reference Image File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer = new e57::Writer( "./VisualRefImage.e57", options ) );

   e57::Image2D image2DHeader;
   image2DHeader.name = "JPEG Image Test";
   image2DHeader.guid = "Visual Reference Image - JPEG Image GUID";
   image2DHeader.description = "JPEG image test";
   image2DHeader.visualReferenceRepresentation.imageWidth = 225;
   image2DHeader.visualReferenceRepresentation.imageHeight = 300;
   image2DHeader.visualReferenceRepresentation.jpegImageSize = imageSize();

   E57_ASSERT_NO_THROW( writer->WriteImage2DData(
      image2DHeader, e57::ImageJPEG, e57::ProjectionVisual, 0, imageBuffer(), imageSize() ) );

   delete writer;
}

// Check that it throws when we don't provide an image representation
TEST( SimpleWriterImage, NoRepresentation )
{
   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW( writer =
                           new e57::Writer( "./NoRepresentation.e57", e57::WriterOptions{} ) );

   const int64_t cImageSize = 1;
   char imageBuffer[cImageSize];

   e57::Image2D image2DHeader;

   E57_ASSERT_THROW( writer->WriteImage2DData( image2DHeader, e57::ImageJPEG, e57::ProjectionVisual,
                                               0, imageBuffer, cImageSize ) );

   delete writer;
}

// Check various visual image fields
TEST( SimpleWriterImage, VisualRefImageInvalidFields )
{
   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW(
      writer = new e57::Writer( "./VisualRefImageInvalidFields.e57", e57::WriterOptions{} ) );

   const int64_t cImageSize = 1;
   char imageBuffer[cImageSize];

   e57::Image2D image2DHeader;
   image2DHeader.visualReferenceRepresentation.imageHeight = 300;
   image2DHeader.visualReferenceRepresentation.jpegImageSize = cImageSize;

   // Check that it complains about zero image width
   E57_ASSERT_THROW( writer->WriteImage2DData( image2DHeader, e57::ImageJPEG, e57::ProjectionVisual,
                                               0, imageBuffer, cImageSize ) );

   // Setting width should now result in success
   image2DHeader.visualReferenceRepresentation.imageWidth = 225;

   E57_ASSERT_NO_THROW( writer->WriteImage2DData(
      image2DHeader, e57::ImageJPEG, e57::ProjectionVisual, 0, imageBuffer, cImageSize ) );

   delete writer;
}

// Check various pinhole image fields
TEST( SimpleWriterImage, PinholeImageInvalidFields )
{
   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW(
      writer = new e57::Writer( "./PinholeImageInvalidFields.e57", e57::WriterOptions{} ) );

   const int64_t cImageSize = 1;
   char imageBuffer[cImageSize];

   // Check that it complains about zero image width
   e57::Image2D image2DHeaderZeroWidth;
   image2DHeaderZeroWidth.pinholeRepresentation.imageHeight = 300;
   image2DHeaderZeroWidth.pinholeRepresentation.jpegImageSize = cImageSize;
   image2DHeaderZeroWidth.pinholeRepresentation.focalLength = 0.1;
   image2DHeaderZeroWidth.pinholeRepresentation.pixelWidth = 0.1;
   image2DHeaderZeroWidth.pinholeRepresentation.pixelHeight = 0.1;

   E57_ASSERT_THROW( writer->WriteImage2DData(
      image2DHeaderZeroWidth, e57::ImageJPEG, e57::ProjectionVisual, 0, imageBuffer, cImageSize ) );

   // Check that it complains about negative focal
   e57::Image2D image2DHeaderNegativeFocal;
   image2DHeaderNegativeFocal.pinholeRepresentation.imageHeight = 300;
   image2DHeaderNegativeFocal.pinholeRepresentation.imageWidth = 225;
   image2DHeaderNegativeFocal.pinholeRepresentation.jpegImageSize = cImageSize;
   image2DHeaderNegativeFocal.pinholeRepresentation.focalLength = -0.1;
   image2DHeaderNegativeFocal.pinholeRepresentation.pixelWidth = 0.1;
   image2DHeaderNegativeFocal.pinholeRepresentation.pixelHeight = 0.1;

   E57_ASSERT_THROW( writer->WriteImage2DData( image2DHeaderNegativeFocal, e57::ImageJPEG,
                                               e57::ProjectionVisual, 0, imageBuffer,
                                               cImageSize ) );

   // Check that it complains about zero pixel width
   e57::Image2D image2DHeaderZeroPixelWidth;
   image2DHeaderZeroPixelWidth.pinholeRepresentation.imageHeight = 300;
   image2DHeaderZeroPixelWidth.pinholeRepresentation.imageWidth = 225;
   image2DHeaderZeroPixelWidth.pinholeRepresentation.jpegImageSize = cImageSize;
   image2DHeaderZeroPixelWidth.pinholeRepresentation.focalLength = 0.1;
   image2DHeaderZeroPixelWidth.pinholeRepresentation.pixelHeight = 0.1;

   E57_ASSERT_THROW( writer->WriteImage2DData( image2DHeaderZeroPixelWidth, e57::ImageJPEG,
                                               e57::ProjectionVisual, 0, imageBuffer,
                                               cImageSize ) );

   delete writer;
}

// Check various spherical image fields
TEST( SimpleWriterImage, SphericalImageInvalidFields )
{
   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW(
      writer = new e57::Writer( "./SphericalImageInvalidFields.e57", e57::WriterOptions{} ) );

   const int64_t cImageSize = 1;
   char imageBuffer[cImageSize];

   // Check that it complains about zero image width
   e57::Image2D image2DHeaderZeroWidth;
   image2DHeaderZeroWidth.sphericalRepresentation.imageHeight = 300;
   image2DHeaderZeroWidth.sphericalRepresentation.jpegImageSize = cImageSize;
   image2DHeaderZeroWidth.sphericalRepresentation.pixelWidth = 0.1;
   image2DHeaderZeroWidth.sphericalRepresentation.pixelHeight = 0.1;

   E57_ASSERT_THROW( writer->WriteImage2DData(
      image2DHeaderZeroWidth, e57::ImageJPEG, e57::ProjectionVisual, 0, imageBuffer, cImageSize ) );

   // Check that it complains about zero pixel width
   e57::Image2D image2DHeaderZeroPixelWidth;
   image2DHeaderZeroPixelWidth.sphericalRepresentation.imageHeight = 300;
   image2DHeaderZeroPixelWidth.sphericalRepresentation.imageWidth = 225;
   image2DHeaderZeroPixelWidth.sphericalRepresentation.jpegImageSize = cImageSize;
   image2DHeaderZeroPixelWidth.sphericalRepresentation.pixelHeight = 0.1;

   E57_ASSERT_THROW( writer->WriteImage2DData( image2DHeaderZeroPixelWidth, e57::ImageJPEG,
                                               e57::ProjectionVisual, 0, imageBuffer,
                                               cImageSize ) );

   delete writer;
}

// Check various cylindrical image fields
TEST( SimpleWriterImage, CylindricalImageInvalidFields )
{
   e57::WriterOptions options;
   options.guid = "Cylindrical Invalid Fields Image File GUID";

   e57::Writer *writer = nullptr;

   E57_ASSERT_NO_THROW(
      writer = new e57::Writer( "./CylindricalImageInvalidFields.e57", e57::WriterOptions{} ) );

   const int64_t cImageSize = 1;
   char imageBuffer[cImageSize];

   // Check that it complains about zero image width
   e57::Image2D image2DHeaderZeroWidth;
   image2DHeaderZeroWidth.cylindricalRepresentation.imageHeight = 300;
   image2DHeaderZeroWidth.cylindricalRepresentation.jpegImageSize = cImageSize;
   image2DHeaderZeroWidth.cylindricalRepresentation.pixelWidth = 0.1;
   image2DHeaderZeroWidth.cylindricalRepresentation.pixelHeight = 0.1;

   E57_ASSERT_THROW( writer->WriteImage2DData(
      image2DHeaderZeroWidth, e57::ImageJPEG, e57::ProjectionVisual, 0, imageBuffer, cImageSize ) );

   // Check that it complains about radius
   e57::Image2D image2DHeaderRadius;
   image2DHeaderRadius.cylindricalRepresentation.imageHeight = 300;
   image2DHeaderRadius.cylindricalRepresentation.imageWidth = 225;
   image2DHeaderRadius.cylindricalRepresentation.jpegImageSize = cImageSize;
   image2DHeaderRadius.cylindricalRepresentation.radius = 0.0;
   image2DHeaderRadius.cylindricalRepresentation.pixelWidth = 0.1;
   image2DHeaderRadius.cylindricalRepresentation.pixelHeight = 0.1;

   E57_ASSERT_THROW( writer->WriteImage2DData(
      image2DHeaderRadius, e57::ImageJPEG, e57::ProjectionVisual, 0, imageBuffer, cImageSize ) );

   // Check that it complains about zero pixel width
   e57::Image2D image2DHeaderZeroPixelWidth;
   image2DHeaderZeroPixelWidth.cylindricalRepresentation.imageHeight = 300;
   image2DHeaderZeroPixelWidth.cylindricalRepresentation.imageWidth = 225;
   image2DHeaderZeroPixelWidth.cylindricalRepresentation.jpegImageSize = cImageSize;
   image2DHeaderZeroPixelWidth.cylindricalRepresentation.pixelHeight = 0.1;

   E57_ASSERT_THROW( writer->WriteImage2DData( image2DHeaderZeroPixelWidth, e57::ImageJPEG,
                                               e57::ProjectionVisual, 0, imageBuffer,
                                               cImageSize ) );

   delete writer;
}
