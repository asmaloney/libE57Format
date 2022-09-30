#pragma once
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

#include "Encoder.h"
#include "Packet.h"

namespace e57
{
   class CompressedVectorWriterImpl
   {
   public:
      CompressedVectorWriterImpl( std::shared_ptr<CompressedVectorNodeImpl> ni, std::vector<SourceDestBuffer> &sbufs );
      ~CompressedVectorWriterImpl();
      void write( size_t requestedRecordCount );
      void write( std::vector<SourceDestBuffer> &sbufs, const size_t requestedRecordCount );
      bool isOpen() const;
      std::shared_ptr<CompressedVectorNodeImpl> compressedVectorNode() const;
      void close();

#ifdef E57_DEBUG
      void dump( int indent = 0, std::ostream &os = std::cout );
#endif

   private:
      void checkImageFileOpen( const char *srcFileName, int srcLineNumber, const char *srcFunctionName ) const;
      void checkWriterOpen( const char *srcFileName, int srcLineNumber, const char *srcFunctionName ) const;
      void setBuffers( std::vector<SourceDestBuffer> &sbufs ); //???needed?
      size_t totalOutputAvailable() const;
      size_t currentPacketSize() const;
      uint64_t packetWrite();
      void flush();

      //??? no default ctor, copy, assignment?

      std::vector<SourceDestBuffer> sbufs_;
      std::shared_ptr<CompressedVectorNodeImpl> cVector_;
      NodeImplSharedPtr proto_;

      std::vector<std::shared_ptr<Encoder>> bytestreams_;
      DataPacket dataPacket_;

      bool isOpen_;
      uint64_t sectionHeaderLogicalStart_; /// start of CompressedVector binary section
      uint64_t sectionLogicalLength_;      /// total length of CompressedVector binary section
      uint64_t dataPhysicalOffset_;        /// start of first data packet
      uint64_t topIndexPhysicalOffset_;    /// top level index packet
      uint64_t recordCount_;               /// number of records written so far
      uint64_t dataPacketsCount_;          /// number of data packets written so far
      uint64_t indexPacketsCount_;         /// number of index packets written so far
   };
}
