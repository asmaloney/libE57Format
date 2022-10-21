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

#include <cstring>

#include "CheckedFile.h"
#include "Packet.h"
#include "StringFunctions.h"

using namespace e57;

struct IndexPacket
{
   static constexpr unsigned MAX_ENTRIES = 2048;

   const uint8_t packetType = INDEX_PACKET;

   uint8_t packetFlags = 0; // flag bitfields
   uint16_t packetLogicalLengthMinus1 = 0;
   uint16_t entryCount = 0;
   uint8_t indexLevel = 0;
   uint8_t reserved1[9] = {}; // must be zero

   struct IndexPacketEntry
   {
      uint64_t chunkRecordNumber = 0;
      uint64_t chunkPhysicalOffset = 0;
   } entries[MAX_ENTRIES];

   void verify( unsigned bufferLength = 0, uint64_t totalRecordCount = 0, uint64_t fileSize = 0 ) const;

#ifdef E57_DEBUG
   void dump( int indent = 0, std::ostream &os = std::cout ) const;
#endif
};

struct EmptyPacketHeader
{
   const uint8_t packetType = EMPTY_PACKET;

   uint8_t reserved1 = 0; // must be zero
   uint16_t packetLogicalLengthMinus1 = 0;

   void verify( unsigned bufferLength = 0 ) const; //???use

#ifdef E57_DEBUG
   void dump( int indent = 0, std::ostream &os = std::cout ) const;
#endif
};

//=============================================================================
// PacketReadCache

PacketReadCache::PacketReadCache( CheckedFile *cFile, unsigned packetCount ) : cFile_( cFile ), entries_( packetCount )
{
   if ( packetCount == 0 )
   {
      throw E57_EXCEPTION2( E57_ERROR_INTERNAL, "packetCount=" + toString( packetCount ) );
   }
}

std::unique_ptr<PacketLock> PacketReadCache::lock( uint64_t packetLogicalOffset, char *&pkt )
{
#ifdef E57_MAX_VERBOSE
   std::cout << "PacketReadCache::lock() called, packetLogicalOffset=" << packetLogicalOffset << std::endl;
#endif

   /// Only allow one locked packet at a time.
   if ( lockCount_ > 0 )
   {
      throw E57_EXCEPTION2( E57_ERROR_INTERNAL, "lockCount=" + toString( lockCount_ ) );
   }

   /// Offset can't be 0
   if ( packetLogicalOffset == 0 )
   {
      throw E57_EXCEPTION2( E57_ERROR_INTERNAL, "packetLogicalOffset=" + toString( packetLogicalOffset ) );
   }

   /// Linear scan for matching packet offset in cache
   for ( unsigned i = 0; i < entries_.size(); ++i )
   {
      auto &entry = entries_[i];

      if ( packetLogicalOffset == entry.logicalOffset_ )
      {
         /// Found a match, so don't have to read anything
#ifdef E57_MAX_VERBOSE
         std::cout << "  Found matching cache entry, index=" << i << std::endl;
#endif
         /// Mark entry with current useCount (keeps track of age of entry).
         entry.lastUsed_ = ++useCount_;

         /// Publish buffer address to caller
         pkt = entry.buffer_;

         /// Create lock so we are sure that we will be unlocked when use is
         /// finished.
         std::unique_ptr<PacketLock> plock( new PacketLock( this, i ) );

         /// Increment cache lock just before return
         ++lockCount_;

         return plock;
      }
   }
   /// Get here if didn't find a match already in cache.

   /// Find least recently used (LRU) packet buffer
   unsigned oldestEntry = 0;
   unsigned oldestUsed = entries_.at( 0 ).lastUsed_;

   for ( unsigned i = 0; i < entries_.size(); ++i )
   {
      const auto &entry = entries_[i];

      if ( entry.lastUsed_ < oldestUsed )
      {
         oldestEntry = i;
         oldestUsed = entry.lastUsed_;
      }
   }
#ifdef E57_MAX_VERBOSE
   std::cout << "  Oldest entry=" << oldestEntry << " lastUsed=" << oldestUsed << std::endl;
#endif

   readPacket( oldestEntry, packetLogicalOffset );

   /// Publish buffer address to caller
   pkt = entries_[oldestEntry].buffer_;

   /// Create lock so we are sure we will be unlocked when use is finished.
   std::unique_ptr<PacketLock> plock( new PacketLock( this, oldestEntry ) );

   /// Increment cache lock just before return
   ++lockCount_;

   return plock;
}

void PacketReadCache::unlock( unsigned cacheIndex )
{
   //??? why lockedEntry not used?
#ifdef E57_MAX_VERBOSE
   std::cout << "PacketReadCache::unlock() called, cacheIndex=" << cacheIndex << std::endl;
#endif

   if ( lockCount_ != 1 )
   {
      throw E57_EXCEPTION2( E57_ERROR_INTERNAL, "lockCount=" + toString( lockCount_ ) );
   }

   --lockCount_;
}

void PacketReadCache::readPacket( unsigned oldestEntry, uint64_t packetLogicalOffset )
{
#ifdef E57_MAX_VERBOSE
   std::cout << "PacketReadCache::readPacket() called, oldestEntry=" << oldestEntry
             << " packetLogicalOffset=" << packetLogicalOffset << std::endl;
#endif

   /// Read header of packet first to get length.  Use EmptyPacketHeader since
   /// it has the fields common to all packets.
   EmptyPacketHeader header;

   cFile_->seek( packetLogicalOffset, CheckedFile::Logical );
   cFile_->read( reinterpret_cast<char *>( &header ), sizeof( header ) );

   /// Can't verify packet header here, because it is not really an
   /// EmptyPacketHeader.
   unsigned packetLength = header.packetLogicalLengthMinus1 + 1;

   /// Be paranoid about packetLength before read
   if ( packetLength > DATA_PACKET_MAX )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "packetLength=" + toString( packetLength ) );
   }

   auto &entry = entries_.at( oldestEntry );

   /// Now read in whole packet into preallocated buffer_.  Note buffer is
   cFile_->seek( packetLogicalOffset, CheckedFile::Logical );
   cFile_->read( entry.buffer_, packetLength );

   /// Verify that packet is good.
   switch ( header.packetType )
   {
      case DATA_PACKET:
      {
         auto dpkt = reinterpret_cast<DataPacket *>( entry.buffer_ );

         dpkt->verify( packetLength );
#ifdef E57_MAX_VERBOSE
         std::cout << "  data packet:" << std::endl;
         dpkt->dump( 4 ); //???
#endif
      }
      break;
      case INDEX_PACKET:
      {
         auto ipkt = reinterpret_cast<IndexPacket *>( entry.buffer_ );

         ipkt->verify( packetLength );
#ifdef E57_MAX_VERBOSE
         std::cout << "  index packet:" << std::endl;
         ipkt->dump( 4 ); //???
#endif
      }
      break;
      case EMPTY_PACKET:
      {
         auto hp = reinterpret_cast<EmptyPacketHeader *>( entry.buffer_ );

         hp->verify( packetLength );
#ifdef E57_MAX_VERBOSE
         std::cout << "  empty packet:" << std::endl;
         hp->dump( 4 ); //???
#endif
      }
      break;
      default:
         throw E57_EXCEPTION2( E57_ERROR_INTERNAL, "packetType=" + toString( header.packetType ) );
   }

   entry.logicalOffset_ = packetLogicalOffset;

   /// Mark entry with current useCount (keeps track of age of entry).
   /// This is a cache, so a small hiccup when useCount_ overflows won't hurt.
   entry.lastUsed_ = ++useCount_;
}

#ifdef E57_DEBUG
void PacketReadCache::dump( int indent, std::ostream &os )
{
   os << space( indent ) << "lockCount: " << lockCount_ << std::endl;
   os << space( indent ) << "useCount:  " << useCount_ << std::endl;
   os << space( indent ) << "entries:" << std::endl;
   for ( unsigned i = 0; i < entries_.size(); i++ )
   {
      os << space( indent ) << "entry[" << i << "]:" << std::endl;
      os << space( indent + 4 ) << "logicalOffset:  " << entries_[i].logicalOffset_ << std::endl;
      os << space( indent + 4 ) << "lastUsed:        " << entries_[i].lastUsed_ << std::endl;
      if ( entries_[i].logicalOffset_ != 0 )
      {
         os << space( indent + 4 ) << "packet:" << std::endl;
         switch ( reinterpret_cast<EmptyPacketHeader *>( entries_.at( i ).buffer_ )->packetType )
         {
            case DATA_PACKET:
            {
               auto dpkt = reinterpret_cast<DataPacket *>( entries_.at( i ).buffer_ );
               dpkt->dump( indent + 6, os );
            }
            break;
            case INDEX_PACKET:
            {
               auto ipkt = reinterpret_cast<IndexPacket *>( entries_.at( i ).buffer_ );
               ipkt->dump( indent + 6, os );
            }
            break;
            case EMPTY_PACKET:
            {
               auto hp = reinterpret_cast<EmptyPacketHeader *>( entries_.at( i ).buffer_ );
               hp->dump( indent + 6, os );
            }
            break;
            default:
               throw E57_EXCEPTION2(
                  E57_ERROR_INTERNAL,
                  "packetType=" +
                     toString( reinterpret_cast<EmptyPacketHeader *>( entries_.at( i ).buffer_ )->packetType ) );
         }
      }
   }
}
#endif

//=============================================================================
// PacketLock

PacketLock::PacketLock( PacketReadCache *cache, unsigned cacheIndex ) : cache_( cache ), cacheIndex_( cacheIndex )
{
#ifdef E57_MAX_VERBOSE
   std::cout << "PacketLock() called" << std::endl;
#endif
}

PacketLock::~PacketLock()
{
#ifdef E57_MAX_VERBOSE
   std::cout << "~PacketLock() called" << std::endl;
#endif
   try
   {
      /// Note cache must live longer than lock, this is reasonable assumption.
      cache_->unlock( cacheIndex_ );
   }
   catch ( ... )
   {
      //??? report?
   }
}

//=============================================================================
// DataPacketHeader

DataPacketHeader::DataPacketHeader()
{
   /// Double check that packet struct is correct length.  Watch out for RTTI
   /// increasing the size.
   static_assert( sizeof( DataPacketHeader ) == 6, "Unexpected size of DataPacketHeader" );
}

void DataPacketHeader::reset()
{
   packetFlags = 0;
   packetLogicalLengthMinus1 = 0;
   bytestreamCount = 0;
}

void DataPacketHeader::verify( unsigned bufferLength ) const
{
   /// Verify that packet is correct type
   if ( packetType != DATA_PACKET )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "packetType=" + toString( packetType ) );
   }

   /// ??? check reserved flags zero?

   /// Check packetLength is at least large enough to hold header
   unsigned packetLength = packetLogicalLengthMinus1 + 1;
   if ( packetLength < sizeof( *this ) )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "packetLength=" + toString( packetLength ) );
   }

   /// Check packet length is multiple of 4
   if ( packetLength % 4 )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "packetLength=" + toString( packetLength ) );
   }

   /// Check actual packet length is large enough.
   if ( bufferLength > 0 && packetLength > bufferLength )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET,
                            "packetLength=" + toString( packetLength ) + " bufferLength=" + toString( bufferLength ) );
   }

   /// Make sure there is at least one entry in packet  ??? 0 record cvect
   /// allowed?
   if ( bytestreamCount == 0 )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "bytestreamCount=" + toString( bytestreamCount ) );
   }

   /// Check packet is at least long enough to hold bytestreamBufferLength array
   if ( sizeof( DataPacketHeader ) + 2 * bytestreamCount > packetLength )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "packetLength=" + toString( packetLength ) +
                                                        " bytestreamCount=" + toString( bytestreamCount ) );
   }
}

#ifdef E57_DEBUG
void DataPacketHeader::dump( int indent, std::ostream &os ) const
{
   os << space( indent ) << "packetType:                " << static_cast<unsigned>( packetType ) << std::endl;
   os << space( indent ) << "packetFlags:               " << static_cast<unsigned>( packetFlags ) << std::endl;
   os << space( indent ) << "packetLogicalLengthMinus1: " << packetLogicalLengthMinus1 << std::endl;
   os << space( indent ) << "bytestreamCount:           " << bytestreamCount << std::endl;
}
#endif

//=============================================================================
// DataPacket

DataPacket::DataPacket()
{
   /// Double check that packet struct is correct length.  Watch out for RTTI
   /// increasing the size.
   static_assert( sizeof( DataPacket ) == 64 * 1024, "Unexpected size of DataPacket" );
}

void DataPacket::verify( unsigned bufferLength ) const
{
   //??? do all packets need versions?  how extend without breaking older
   // checking?  need to check
   // file version#?

   /// Verify header is good
   auto hp = reinterpret_cast<const DataPacketHeader *>( this );

   hp->verify( bufferLength );

   /// Calc sum of lengths of each bytestream buffer in this packet
   auto bsbLength = reinterpret_cast<const uint16_t *>( &payload[0] );
   unsigned totalStreamByteCount = 0;

   for ( unsigned i = 0; i < header.bytestreamCount; i++ )
   {
      totalStreamByteCount += bsbLength[i];
   }

   /// Calc size of packet needed
   const unsigned packetLength = header.packetLogicalLengthMinus1 + 1;
   const unsigned needed = sizeof( DataPacketHeader ) + 2 * header.bytestreamCount + totalStreamByteCount;
#ifdef E57_MAX_VERBOSE
   std::cout << "needed=" << needed << " actual=" << packetLength << std::endl; //???
#endif

   /// If needed is not with 3 bytes of actual packet size, have an error
   if ( needed > packetLength || needed + 3 < packetLength )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET,
                            "needed=" + toString( needed ) + "packetLength=" + toString( packetLength ) );
   }

   /// Verify that padding at end of packet is zero
   for ( unsigned i = needed; i < packetLength; i++ )
   {
      if ( reinterpret_cast<const char *>( this )[i] != 0 )
      {
         throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "i=" + toString( i ) );
      }
   }
}

char *DataPacket::getBytestream( unsigned bytestreamNumber, unsigned &byteCount )
{
#ifdef E57_MAX_VERBOSE
   std::cout << "getBytestream called, bytestreamNumber=" << bytestreamNumber << std::endl;
#endif

   /// Verify that packet is correct type
   if ( header.packetType != DATA_PACKET )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "packetType=" + toString( header.packetType ) );
   }

   /// Check bytestreamNumber in bounds
   if ( bytestreamNumber >= header.bytestreamCount )
   {
      throw E57_EXCEPTION2( E57_ERROR_INTERNAL, "bytestreamNumber=" + toString( bytestreamNumber ) +
                                                   "bytestreamCount=" + toString( header.bytestreamCount ) );
   }

   /// Calc positions in packet
   auto bsbLength = reinterpret_cast<uint16_t *>( &payload[0] );
   auto streamBase = reinterpret_cast<char *>( &bsbLength[header.bytestreamCount] );

   /// Sum size of preceding stream buffers to get position
   unsigned totalPreceeding = 0;
   for ( unsigned i = 0; i < bytestreamNumber; i++ )
   {
      totalPreceeding += bsbLength[i];
   }

   byteCount = bsbLength[bytestreamNumber];

   /// Double check buffer is completely within packet
   if ( sizeof( DataPacketHeader ) + 2 * header.bytestreamCount + totalPreceeding + byteCount >
        header.packetLogicalLengthMinus1 + 1U )
   {
      throw E57_EXCEPTION2( E57_ERROR_INTERNAL,
                            "bytestreamCount=" + toString( header.bytestreamCount ) + " totalPreceeding=" +
                               toString( totalPreceeding ) + " byteCount=" + toString( byteCount ) +
                               " packetLogicalLengthMinus1=" + toString( header.packetLogicalLengthMinus1 ) );
   }

   /// Return start of buffer
   return ( &streamBase[totalPreceeding] );
}

unsigned DataPacket::getBytestreamBufferLength( unsigned bytestreamNumber )
{
   //??? for now:
   unsigned byteCount;
   (void)getBytestream( bytestreamNumber, byteCount );
   return ( byteCount );
}

#ifdef E57_DEBUG
void DataPacket::dump( int indent, std::ostream &os ) const
{
   if ( header.packetType != DATA_PACKET )
   {
      throw E57_EXCEPTION2( E57_ERROR_INTERNAL, "packetType=" + toString( header.packetType ) );
   }

   reinterpret_cast<const DataPacketHeader *>( this )->dump( indent, os );

   auto bsbLength = reinterpret_cast<const uint16_t *>( &payload[0] );
   auto p = reinterpret_cast<const uint8_t *>( &bsbLength[header.bytestreamCount] );

   for ( unsigned i = 0; i < header.bytestreamCount; i++ )
   {
      os << space( indent ) << "bytestream[" << i << "]:" << std::endl;
      os << space( indent + 4 ) << "length: " << bsbLength[i] << std::endl;
      /*====
           unsigned j;
           for (j=0; j < bsbLength[i] && j < 10; j++)
               os << space(indent+4) << "byte[" << j << "]=" << (unsigned)p[j]
   << std::endl; if (j < bsbLength[i]) os << space(indent+4) << bsbLength[i]-j << "
   more unprinted..." << std::endl;
   ====*/
      p += bsbLength[i];
      if ( p - reinterpret_cast<const uint8_t *>( this ) > DATA_PACKET_MAX )
      {
         throw E57_EXCEPTION2( E57_ERROR_INTERNAL,
                               "size=" + toString( p - reinterpret_cast<const uint8_t *>( this ) ) );
      }
   }
}
#endif

//=============================================================================
// IndexPacket

void IndexPacket::verify( unsigned bufferLength, uint64_t totalRecordCount, uint64_t fileSize ) const
{
   //??? do all packets need versions?  how extend without breaking older
   // checking?  need to check
   // file version#?

   /// Verify that packet is correct type
   if ( packetType != INDEX_PACKET )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "packetType=" + toString( packetType ) );
   }

   /// Check packetLength is at least large enough to hold header
   unsigned packetLength = packetLogicalLengthMinus1 + 1;
   if ( packetLength < sizeof( *this ) )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "packetLength=" + toString( packetLength ) );
   }

   /// Check packet length is multiple of 4
   if ( packetLength % 4 )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "packetLength=" + toString( packetLength ) );
   }

   /// Make sure there is at least one entry in packet  ??? 0 record cvect
   /// allowed?
   if ( entryCount == 0 )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "entryCount=" + toString( entryCount ) );
   }

   /// Have to have <= 2048 entries
   if ( entryCount > MAX_ENTRIES )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "entryCount=" + toString( entryCount ) );
   }

   /// Index level should be <= 5.  Because (5+1)* 11 bits = 66 bits, which will
   /// cover largest number of chunks possible.
   if ( indexLevel > 5 )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "indexLevel=" + toString( indexLevel ) );
   }

   /// Index packets above level 0 must have at least two entries (otherwise no
   /// point to existing).
   ///??? check that this is in spec
   if ( indexLevel > 0 && entryCount < 2 )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET,
                            "indexLevel=" + toString( indexLevel ) + " entryCount=" + toString( entryCount ) );
   }

   /// If not later version, verify reserved fields are zero. ??? test file
   /// version if (version <= E57_FORMAT_MAJOR) { //???
   for ( unsigned i = 0; i < sizeof( reserved1 ); i++ )
   {
      if ( reserved1[i] != 0 )
      {
         throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "i=" + toString( i ) );
      }
   }

   /// Check actual packet length is large enough.
   if ( bufferLength > 0 && packetLength > bufferLength )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET,
                            "packetLength=" + toString( packetLength ) + " bufferLength=" + toString( bufferLength ) );
   }

   /// Check if entries will fit in space provided
   unsigned neededLength = 16 + 8 * entryCount;
   if ( packetLength < neededLength )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET,
                            "packetLength=" + toString( packetLength ) + " neededLength=" + toString( neededLength ) );
   }

#ifdef E57_MAX_DEBUG
   /// Verify padding at end is zero.
   const char *p = reinterpret_cast<const char *>( this );
   for ( unsigned i = neededLength; i < packetLength; i++ )
   {
      if ( p[i] != 0 )
         throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "i=" + toString( i ) );
   }

   /// Verify records and offsets are in sorted order
   for ( unsigned i = 0; i < entryCount; i++ )
   {
      /// Check chunkRecordNumber is in bounds
      if ( totalRecordCount > 0 && entries[i].chunkRecordNumber >= totalRecordCount )
      {
         throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET,
                               "i=" + toString( i ) + " chunkRecordNumber=" + toString( entries[i].chunkRecordNumber ) +
                                  " totalRecordCount=" + toString( totalRecordCount ) );
      }

      /// Check record numbers are strictly increasing
      if ( i > 0 && entries[i - 1].chunkRecordNumber >= entries[i].chunkRecordNumber )
      {
         throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET,
                               "i=" + toString( i ) +
                                  " prevChunkRecordNumber=" + toString( entries[i - 1].chunkRecordNumber ) +
                                  " currentChunkRecordNumber=" + toString( entries[i].chunkRecordNumber ) );
      }

      /// Check chunkPhysicalOffset is in bounds
      if ( fileSize > 0 && entries[i].chunkPhysicalOffset >= fileSize )
      {
         throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "i=" + toString( i ) + " chunkPhysicalOffset=" +
                                                           toString( entries[i].chunkPhysicalOffset ) +
                                                           " fileSize=" + toString( fileSize ) );
      }

      /// Check chunk offsets are strictly increasing
      if ( i > 0 && entries[i - 1].chunkPhysicalOffset >= entries[i].chunkPhysicalOffset )
      {
         throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET,
                               "i=" + toString( i ) +
                                  " prevChunkPhysicalOffset=" + toString( entries[i - 1].chunkPhysicalOffset ) +
                                  " currentChunkPhysicalOffset=" + toString( entries[i].chunkPhysicalOffset ) );
      }
   }
#endif
}

#ifdef E57_DEBUG
void IndexPacket::dump( int indent, std::ostream &os ) const
{
   os << space( indent ) << "packetType:                " << static_cast<unsigned>( packetType ) << std::endl;
   os << space( indent ) << "packetFlags:               " << static_cast<unsigned>( packetFlags ) << std::endl;
   os << space( indent ) << "packetLogicalLengthMinus1: " << packetLogicalLengthMinus1 << std::endl;
   os << space( indent ) << "entryCount:                " << entryCount << std::endl;
   os << space( indent ) << "indexLevel:                " << indexLevel << std::endl;
   unsigned i;
   for ( i = 0; i < entryCount && i < 10; i++ )
   {
      os << space( indent ) << "entry[" << i << "]:" << std::endl;
      os << space( indent + 4 ) << "chunkRecordNumber:    " << entries[i].chunkRecordNumber << std::endl;
      os << space( indent + 4 ) << "chunkPhysicalOffset:  " << entries[i].chunkPhysicalOffset << std::endl;
   }
   if ( i < entryCount )
   {
      os << space( indent ) << entryCount - i << "more entries unprinted..." << std::endl;
   }
}
#endif

//=============================================================================
// EmptyPacketHeader

void EmptyPacketHeader::verify( unsigned bufferLength ) const
{
   /// Verify that packet is correct type
   if ( packetType != EMPTY_PACKET )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "packetType=" + toString( packetType ) );
   }

   /// Check packetLength is at least large enough to hold header
   unsigned packetLength = packetLogicalLengthMinus1 + 1;
   if ( packetLength < sizeof( *this ) )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "packetLength=" + toString( packetLength ) );
   }

   /// Check packet length is multiple of 4
   if ( packetLength % 4 )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET, "packetLength=" + toString( packetLength ) );
   }

   /// Check actual packet length is large enough.
   if ( bufferLength > 0 && packetLength > bufferLength )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_CV_PACKET,
                            "packetLength=" + toString( packetLength ) + " bufferLength=" + toString( bufferLength ) );
   }
}

#ifdef E57_DEBUG
void EmptyPacketHeader::dump( int indent, std::ostream &os ) const
{
   os << space( indent ) << "packetType:                " << static_cast<unsigned>( packetType ) << std::endl;
   os << space( indent ) << "packetLogicalLengthMinus1: " << packetLogicalLengthMinus1 << std::endl;
}
#endif
