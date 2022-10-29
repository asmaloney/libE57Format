// SPDX-License-Identifier: BSL-1.0
// Copyright (c) 2020 PTC Inc.
// Copyright (c) 2022 Andy Maloney <asmaloney@gmail.com>

// For M_PI. This needs to be first, otherwise we might already include math header
// without M_PI and we would get nothing because of the header guards.
#define _USE_MATH_DEFINES
#include <cmath>

#include "E57SimpleData.h"

#include "Common.h"
#include "StringFunctions.h"

namespace e57
{
   // To avoid exposing M_PI, we define the constructor here.
   SphericalBounds::SphericalBounds()
   {
      rangeMinimum = 0.;
      rangeMaximum = E57_DOUBLE_MAX;
      azimuthStart = -M_PI;
      azimuthEnd = M_PI;

      constexpr auto HALF_PI = M_PI / 2.0;

      elevationMinimum = -HALF_PI;
      elevationMaximum = HALF_PI;
   }

   template <typename COORDTYPE>
   Data3DPointsData_t<COORDTYPE>::Data3DPointsData_t( const Data3D &data3D ) : _selfAllocated( true )
   {
      const auto cSize = data3D.pointsSize;

      if ( cSize < 1 )
      {
         throw E57_EXCEPTION2( E57_ERROR_VALUE_OUT_OF_BOUNDS, "pointsSize=" + toString( cSize ) + " minimum=1" );
      }

      if ( data3D.pointFields.cartesianXField )
      {
         cartesianX = new COORDTYPE[cSize];
      }

      if ( data3D.pointFields.cartesianYField )
      {
         cartesianY = new COORDTYPE[cSize];
      }

      if ( data3D.pointFields.cartesianZField )
      {
         cartesianZ = new COORDTYPE[cSize];
      }

      if ( data3D.pointFields.cartesianInvalidStateField )
      {
         cartesianInvalidState = new int8_t[cSize];
      }

      if ( data3D.pointFields.intensityField )
      {
         intensity = new float[cSize];
      }

      if ( data3D.pointFields.isIntensityInvalidField )
      {
         isIntensityInvalid = new int8_t[cSize];
      }

      if ( data3D.pointFields.colorRedField )
      {
         colorRed = new uint8_t[cSize];
      }

      if ( data3D.pointFields.colorGreenField )
      {
         colorGreen = new uint8_t[cSize];
      }

      if ( data3D.pointFields.colorBlueField )
      {
         colorBlue = new uint8_t[cSize];
      }

      if ( data3D.pointFields.isColorInvalidField )
      {
         isColorInvalid = new int8_t[cSize];
      }

      if ( data3D.pointFields.sphericalRangeField )
      {
         sphericalRange = new COORDTYPE[cSize];
      }

      if ( data3D.pointFields.sphericalAzimuthField )
      {
         sphericalAzimuth = new COORDTYPE[cSize];
      }

      if ( data3D.pointFields.sphericalElevationField )
      {
         sphericalElevation = new COORDTYPE[cSize];
      }

      if ( data3D.pointFields.sphericalInvalidStateField )
      {
         sphericalInvalidState = new int8_t[cSize];
      }

      if ( data3D.pointFields.rowIndexField )
      {
         rowIndex = new int32_t[cSize];
      }

      if ( data3D.pointFields.columnIndexField )
      {
         columnIndex = new int32_t[cSize];
      }

      if ( data3D.pointFields.returnIndexField )
      {
         returnIndex = new int8_t[cSize];
      }

      if ( data3D.pointFields.returnCountField )
      {
         returnCount = new int8_t[cSize];
      }

      if ( data3D.pointFields.timeStampField )
      {
         timeStamp = new double[cSize];
      }

      if ( data3D.pointFields.isTimeStampInvalidField )
      {
         isTimeStampInvalid = new int8_t[cSize];
      }

      if ( data3D.pointFields.normalXField )
      {
         normalX = new float[cSize];
      }

      if ( data3D.pointFields.normalYField )
      {
         normalY = new float[cSize];
      }

      if ( data3D.pointFields.normalZField )
      {
         normalZ = new float[cSize];
      }
   }

   template <typename COORDTYPE> Data3DPointsData_t<COORDTYPE>::~Data3DPointsData_t()
   {
      if ( !_selfAllocated )
      {
         return;
      }

      delete[] cartesianX;
      delete[] cartesianY;
      delete[] cartesianZ;
      delete[] cartesianInvalidState;

      delete[] intensity;
      delete[] isIntensityInvalid;

      delete[] colorRed;
      delete[] colorGreen;
      delete[] colorBlue;
      delete[] isColorInvalid;

      delete[] sphericalRange;
      delete[] sphericalAzimuth;
      delete[] sphericalElevation;
      delete[] sphericalInvalidState;

      delete[] rowIndex;
      delete[] columnIndex;

      delete[] returnIndex;
      delete[] returnCount;

      delete[] timeStamp;
      delete[] isTimeStampInvalid;

      delete[] normalX;
      delete[] normalY;
      delete[] normalZ;

      // Set them all to nullptr.
      *this = Data3DPointsData_t<COORDTYPE>();
   }

   template Data3DPointsData_t<float>::Data3DPointsData_t( const Data3D &data3D );
   template Data3DPointsData_t<double>::Data3DPointsData_t( const Data3D &data3D );

   template Data3DPointsData_t<float>::~Data3DPointsData_t();
   template Data3DPointsData_t<double>::~Data3DPointsData_t();
} // end namespace e57
