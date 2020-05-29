/*
 * Copyright (c) 2010 Stan Coleby (scoleby@intelisum.com)
 * Copyright (c) 2020 PTC Inc.
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

//! @file E57Simple.cpp

// for M_PI. This needs to be first, otherwise we might already include math header
// without M_PI and we would get nothing because of the header guards.
#define _USE_MATH_DEFINES
#include <cmath>

#include "E57Simple.h"
#include "E57SimpleImpl.h"

using namespace e57;
using namespace std;

// to avoid exposing M_PI constructor is here
SphericalBounds::SphericalBounds()
{
   rangeMinimum = 0.;
   rangeMaximum = E57_DOUBLE_MAX;
   azimuthStart = -M_PI;
   azimuthEnd = M_PI;
   elevationMinimum = -M_PI / 2.;
   elevationMaximum = M_PI / 2.;
}

////////////////////////////////////////////////////////////////////
//
//	e57::Reader
//
Reader::Reader( const ustring &filePath ) : impl_( new ReaderImpl( filePath ) )
{
}

bool Reader::IsOpen() const
{
   return impl_->IsOpen();
};

bool Reader::Close() const
{
   return impl_->Close();
};

bool Reader::GetE57Root( E57Root &fileHeader ) const
{
   return impl_->GetE57Root( fileHeader );
};

int64_t Reader::GetImage2DCount() const
{
   return impl_->GetImage2DCount();
};

bool Reader::ReadImage2D( int64_t imageIndex, Image2D &image2DHeader ) const
{
   return impl_->ReadImage2D( imageIndex, image2DHeader );
};

bool Reader::GetImage2DSizes( int64_t imageIndex, e57::Image2DProjection &imageProjection, e57::Image2DType &imageType,
                              int64_t &imageWidth, int64_t &imageHeight, int64_t &imageSize,
                              e57::Image2DType &imageMaskType, e57::Image2DType &imageVisualType ) const
{
   return impl_->GetImage2DSizes( imageIndex, imageProjection, imageType, imageWidth, imageHeight, imageSize,
                                  imageMaskType, imageVisualType );
};

int64_t Reader::ReadImage2DData( int64_t imageIndex, e57::Image2DProjection imageProjection, e57::Image2DType imageType,
                                 void *pBuffer, int64_t start, int64_t count ) const
{
   return impl_->ReadImage2DData( imageIndex, imageProjection, imageType, pBuffer, start, count );
};

int64_t Reader::GetData3DCount() const
{
   return impl_->GetData3DCount();
};

ImageFile Reader::GetRawIMF()
{
   return impl_->GetRawIMF();
}

StructureNode Reader::GetRawE57Root()
{
   return impl_->GetRawE57Root();
};

VectorNode Reader::GetRawData3D()
{
   return impl_->GetRawData3D();
};

VectorNode Reader::GetRawImages2D()
{
   return impl_->GetRawImages2D();
};

bool Reader::ReadData3D( int64_t dataIndex, Data3D &data3DHeader ) const
{
   return impl_->ReadData3D( dataIndex, data3DHeader );
}

bool Reader::GetData3DSizes( int64_t dataIndex, int64_t &rowMax, int64_t &columnMax, int64_t &pointsSize,
                             int64_t &groupsSize, int64_t &countSize, bool &bColumnIndex ) const
{
   return impl_->GetData3DSizes( dataIndex, rowMax, columnMax, pointsSize, groupsSize, countSize, bColumnIndex );
}

bool Reader::ReadData3DGroupsData( int64_t dataIndex, int64_t groupCount, int64_t *idElementValue,
                                   int64_t *startPointIndex, int64_t *pointCount ) const
{
   return impl_->ReadData3DGroupsData( dataIndex, groupCount, idElementValue, startPointIndex, pointCount );
}

CompressedVectorReader Reader::SetUpData3DPointsData( int64_t dataIndex, size_t pointCount,
                                                      const Data3DPointsData &buffers ) const
{
   return impl_->SetUpData3DPointsData( dataIndex, pointCount, buffers );
}

////////////////////////////////////////////////////////////////////
//
//	e57::Writer
//
Writer::Writer( const ustring &filePath, const ustring &coordinateMetaData ) :
   impl_( new WriterImpl( filePath, coordinateMetaData ) )
{
}

bool Writer::IsOpen() const
{
   return impl_->IsOpen();
};

bool Writer::Close() const
{
   return impl_->Close();
};

ImageFile Writer::GetRawIMF()
{
   return impl_->GetRawIMF();
}

StructureNode Writer::GetRawE57Root()
{
   return impl_->GetRawE57Root();
};

VectorNode Writer::GetRawData3D()
{
   return impl_->GetRawData3D();
};

VectorNode Writer::GetRawImages2D()
{
   return impl_->GetRawImages2D();
};

int64_t Writer::NewImage2D( Image2D &image2DHeader ) const
{
   return impl_->NewImage2D( image2DHeader );
};

int64_t Writer::WriteImage2DData( int64_t imageIndex, e57::Image2DType imageType,
                                  e57::Image2DProjection imageProjection, void *pBuffer, int64_t start,
                                  int64_t count ) const
{
   return impl_->WriteImage2DData( imageIndex, imageType, imageProjection, pBuffer, start, count );
};

int64_t Writer::NewData3D( Data3D &data3DHeader, bool ( *pointExtension )( ImageFile imf, StructureNode proto ) ) const
{
   return impl_->NewData3D( data3DHeader, pointExtension );
};

CompressedVectorWriter Writer::SetUpData3DPointsData( int64_t dataIndex, size_t pointCount,
                                                      const Data3DPointsData &buffers ) const
{
   return impl_->SetUpData3DPointsData( dataIndex, pointCount, buffers );
}

bool Writer::WriteData3DGroupsData( int64_t dataIndex, int64_t groupCount, int64_t *idElementValue,
                                    int64_t *startPointIndex, int64_t *pointCount ) const
{
   return impl_->WriteData3DGroupsData( dataIndex, groupCount, idElementValue, startPointIndex, pointCount );
}
