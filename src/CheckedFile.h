/*
 * Original work Copyright 2009 - 2010 Kevin Ackley (kackley@gwi.net)
 * Modified work Copyright 2018 - 2020 Andy Maloney <asmaloney@gmail.com>
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

#pragma once

#include <algorithm>

#include "Common.h"

namespace e57
{
   /// Tool class to read buffer efficiently without
   /// multiplying copy operations.
   ///
   /// WARNING: pointer input is handled by user!
   class BufferView;

   class CheckedFile
   {
   public:
      static constexpr size_t physicalPageSizeLog2 = 10; // physical page size is 2 raised to this power
      static constexpr size_t physicalPageSize = 1 << physicalPageSizeLog2;
      static constexpr uint64_t physicalPageSizeMask = physicalPageSize - 1;
      static constexpr size_t logicalPageSize = physicalPageSize - 4;

   public:
      enum Mode
      {
         ReadOnly,
         WriteCreate,
         WriteExisting
      };

      enum OffsetMode
      {
         Logical,
         Physical
      };

      CheckedFile( const e57::ustring &fileName, Mode mode, ReadChecksumPolicy policy );
      CheckedFile( const char *input, uint64_t size, ReadChecksumPolicy policy );
      ~CheckedFile();

      void read( char *buf, size_t nRead, size_t bufSize = 0 );
      void write( const char *buf, size_t nWrite );
      CheckedFile &operator<<( const e57::ustring &s );
      CheckedFile &operator<<( int64_t i );
      CheckedFile &operator<<( uint64_t i );
      CheckedFile &operator<<( float f );
      CheckedFile &operator<<( double d );
      void seek( uint64_t offset, OffsetMode omode = Logical );
      uint64_t position( OffsetMode omode = Logical );
      uint64_t length( OffsetMode omode = Logical );
      void extend( uint64_t newLength, OffsetMode omode = Logical );

      e57::ustring fileName() const
      {
         return fileName_;
      }

      void close();
      void unlink();

      static inline uint64_t logicalToPhysical( uint64_t logicalOffset );
      static inline uint64_t physicalToLogical( uint64_t physicalOffset );

   private:
      uint32_t checksum( char *buf, size_t size ) const;
      void verifyChecksum( char *page_buffer, size_t page );

      template <class FTYPE> CheckedFile &writeFloatingPoint( FTYPE value, int precision );

      void getCurrentPageAndOffset( uint64_t &page, size_t &pageOffset, OffsetMode omode = Logical );
      void readPhysicalPage( char *page_buffer, uint64_t page );
      void writePhysicalPage( char *page_buffer, uint64_t page );
      int open64( const e57::ustring &fileName, int flags, int mode );
      uint64_t lseek64( int64_t offset, int whence );

      e57::ustring fileName_;
      uint64_t logicalLength_ = 0;
      uint64_t physicalLength_ = 0;

      ReadChecksumPolicy checkSumPolicy_ = ChecksumPolicy::ChecksumAll;

      int fd_ = -1;
      BufferView *bufView_ = nullptr;
      bool readOnly_ = false;
   };

   inline uint64_t CheckedFile::logicalToPhysical( uint64_t logicalOffset )
   {
      const uint64_t page = logicalOffset / logicalPageSize;
      const uint64_t remainder = logicalOffset - page * logicalPageSize;

      return page * physicalPageSize + remainder;
   }

   inline uint64_t CheckedFile::physicalToLogical( uint64_t physicalOffset )
   {
      const uint64_t page = physicalOffset >> physicalPageSizeLog2;
      const size_t remainder = static_cast<size_t>( physicalOffset & physicalPageSizeMask );

      return page * logicalPageSize + std::min( remainder, logicalPageSize );
   }

}
