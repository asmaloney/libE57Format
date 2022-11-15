/*
 * CompressedVectorWriter.cpp - implementation of public functions of the
 *   CompressedVectorWriter class.
 *
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

/// @file CompressedVectorWriter.cpp

#include "CompressedVectorWriterImpl.h"

using namespace e57;

// Put this function first so we can reference the code in doxygen using @skip
/*!
@brief Check whether CompressedVectorWriter class invariant is true
@copydetails CompressedVectorReader::checkInvariant
*/
void CompressedVectorWriter::checkInvariant( bool /*doRecurse*/ )
{
   // If this CompressedVectorWriter is not open, can't test invariant (almost
   // every call would throw)
   if ( !isOpen() )
   {
      return;
   }

   CompressedVectorNode cv = compressedVectorNode();
   ImageFile imf = cv.destImageFile();

   // If destImageFile not open, can't test invariant (almost every call would
   // throw)
   if ( !imf.isOpen() )
   {
      return;
   }

   // Associated CompressedVectorNode must be attached to ImageFile
   if ( !cv.isAttached() )
   {
      throw E57_EXCEPTION1( ErrorInvarianceViolation );
   }

   // Dest ImageFile must be writable
   if ( !imf.isWritable() )
   {
      throw E57_EXCEPTION1( ErrorInvarianceViolation );
   }

   // Dest ImageFile must have exactly 1 writer (this one)
   if ( imf.writerCount() != 1 )
   {
      throw E57_EXCEPTION1( ErrorInvarianceViolation );
   }

   // Dest ImageFile can't have any readers
   if ( imf.readerCount() != 0 )
   {
      throw E57_EXCEPTION1( ErrorInvarianceViolation );
   }
}

/*!
@class e57::CompressedVectorWriter

@brief An iterator object keeping track of a write in progress to a CompressedVectorNode.

@details
A CompressedVectorWriter object is a block iterator that reads blocks of records from memory and
stores them in a CompressedVectorNode. Blocks of records are processed rather than a single
record-at-a-time for efficiency reasons. The CompressedVectorWriter class encapsulates all the state
that must be saved in between the processing of one record block and the next (e.g. partially
written disk pages, partially filled bytes in a bytestream, or data compression state). New memory
buffers can be used for each record block write, or the previous buffers can be reused.

CompressedVectorWriter objects have an open/closed state. Initially a newly created
CompressedVectorWriter is in the open state. After the API user calls CompressedVectorWriter::close,
the object will be in the closed state and no more data transfers will be possible.

There is no CompressedVectorWriter constructor in the API. The function CompressedVectorNode::writer
returns an already constructed CompressedVectorWriter object with given memory buffers
(SourceDestBuffers) already associated. CompressedVectorWriter::close must explicitly be called to
safely and gracefully end the transfer.

@warning If CompressedVectorWriter::close is not called before the CompressedVectorWriter destructor
is invoked, all writes to the CompressedVectorNode will be lost (it will have zero children).

@section CompressedVectorWriter_invariant Class Invariant A class invariant is a list of statements
about an object that are always true before and after any operation on the object. An invariant is
useful for testing correct operation of an implementation. Statements in an invariant can involve
only externally visible state, or can refer to internal implementation-specific state that is not
visible to the API user. The following C++ code checks externally visible state for consistency and
throws an exception if the invariant is violated:

@dontinclude CompressedVectorWriter.cpp
@skip void CompressedVectorWriter::checkInvariant
@until ^}

@see CompressedVectorNode, CompressedVectorReader
*/

/// @cond documentNonPublic The following isn't part of the API, and isn't documented.
CompressedVectorWriter::CompressedVectorWriter( std::shared_ptr<CompressedVectorWriterImpl> ni ) :
   impl_( ni )
{
}
/// @endcond

/*!
@brief Request transfer of blocks of data to CompressedVectorNode from previously designated source
buffers.

@param [in] recordCount Number of records to write.

@details
The SourceDestBuffers used are previously designated either in CompressedVectorNode::writer where
this object was created, or in the last call to
CompressedVectorWriter::write(std::vector<SourceDestBuffer>&, unsigned) where new buffers were
designated.

If a conversion or bounds error occurs during the transfer, the CompressedVectorWriter is left in an
undocumented state (it can't be used any further), and all previously written records are deleted
from the associated CompressedVectorNode which will then have zero children. If a file I/O or
checksum error occurs during the transfer, both this CompressedVectorWriter and the associated
ImageFile are left in an undocumented state (they can't be used any further). If
CompressedVectorWriter::close is not called before the CompressedVectorWriter destructor is invoked,
all writes to the CompressedVectorNode will be lost (it will have zero children).

@pre The associated ImageFile must be open.
@pre This CompressedVectorWriter must be open (i.e isOpen())

@throw ::ErrorBadAPIArgument
@throw ::ErrorImageFileNotOpen
@throw ::ErrorWriterNotOpen
@throw ::ErrorPathUndefined
@throw ::ErrorNoBufferForElement
@throw ::ErrorBufferSizeMismatch
@throw ::ErrorBufferDuplicatePathName
@throw ::ErrorConversionRequired This CompressedVectorWriter in undocumented state, associated
CompressedVectorNode modified but consistent, associated ImageFile modified but consistent.
@throw ::ErrorValueOutOfBounds This CompressedVectorWriter in undocumented state, associated
CompressedVectorNode modified but consistent, associated ImageFile modified but consistent.
@throw ::ErrorValueNotRepresentable This CompressedVectorWriter in undocumented state, associated
CompressedVectorNode modified but consistent, associated ImageFile modified but consistent.
@throw ::ErrorScaledValueNotRepresentable This CompressedVectorWriter in undocumented state,
associated CompressedVectorNode modified but consistent, associated ImageFile modified but
consistent.
@throw ::ErrorReal64TooLarge This CompressedVectorWriter in undocumented state, associated
CompressedVectorNode modified but consistent, associated ImageFile modified but consistent.
@throw ::ErrorExpectingNumeric This CompressedVectorWriter in undocumented state, associated
CompressedVectorNode modified but consistent, associated ImageFile modified but consistent.
@throw ::ErrorExpectingUString This CompressedVectorWriter in undocumented state, associated
CompressedVectorNode modified but consistent, associated ImageFile modified but consistent.
@throw ::ErrorSeekFailed This CompressedVectorWriter, associated ImageFile in undocumented state
@throw ::ErrorReadFailed This CompressedVectorWriter, associated ImageFile in undocumented state
@throw ::ErrorWriteFailed This CompressedVectorWriter, associated ImageFile in undocumented state
@throw ::ErrorBadChecksum This CompressedVectorWriter, associated ImageFile in undocumented state
@throw ::ErrorInternal All objects in undocumented state

@see CompressedVectorWriter::write(std::vector<SourceDestBuffer>&,unsigned),
CompressedVectorNode::writer, CompressedVectorWriter::close, SourceDestBuffer, E57Exception
*/
void CompressedVectorWriter::write( const size_t recordCount )
{
   impl_->write( recordCount );
}

/*!
@brief Request transfer of block of data to CompressedVectorNode from given source buffers.

@param [in] sbufs Vector of memory buffers that hold data to be written to a CompressedVectorNode.
@param [in] recordCount Number of records to write.

@details
The @a sbufs must all have the same capacity.

The @a sbufs capacity must be >= @a recordCount.

The specified @a sbufs must have same number of elements as previously designated SourceDestBuffer
vector. The each SourceDestBuffer within @a sbufs must be identical to the previously designated
SourceDestBuffer except for capacity and buffer address.

The @a sbufs locations are saved so that a later call to CompressedVectorWriter::write(unsigned) can
be used without having to re-specify the SourceDestBuffers.

If a conversion or bounds error occurs during the transfer, the CompressedVectorWriter is left in an
undocumented state (it can't be used any further), and all previously written records are deleted
from the the associated CompressedVectorNode which will then have zero children. If a file I/O or
checksum error occurs during the transfer, both this CompressedVectorWriter and the associated
ImageFile are left in an undocumented state (they can't be used any further).

@warning If CompressedVectorWriter::close is not called before the CompressedVectorWriter destructor
is invoked, all writes to the CompressedVectorNode will be lost (it will have zero children).

@pre The associated ImageFile must be open.
@pre This CompressedVectorWriter must be open (i.e isOpen())

@throw ::ErrorBadAPIArgument
@throw ::ErrorImageFileNotOpen
@throw ::ErrorWriterNotOpen
@throw ::ErrorPathUndefined
@throw ::ErrorNoBufferForElement
@throw ::ErrorBufferSizeMismatch
@throw ::ErrorBufferDuplicatePathName
@throw ::ErrorConversionRequired This CompressedVectorWriter in undocumented state, associated
ImageFile modified but consistent.
@throw ::ErrorValueOutOfBounds This CompressedVectorWriter in undocumented state, associated
ImageFile modified but consistent.
@throw ::ErrorValueNotRepresentable This CompressedVectorWriter in undocumented state, associated
ImageFile modified but consistent.
@throw ::ErrorScaledValueNotRepresentable This CompressedVectorWriter in undocumented state,
associated ImageFile modified but consistent.
@throw ::ErrorReal64TooLarge This CompressedVectorWriter in undocumented state, associated ImageFile
modified but consistent.
@throw ::ErrorExpectingNumeric This CompressedVectorWriter in undocumented state, associated
ImageFile modified but consistent.
@throw ::ErrorExpectingUString This CompressedVectorWriter in undocumented state, associated
ImageFile modified but consistent.
@throw ::ErrorSeekFailed This CompressedVectorWriter, associated ImageFile in undocumented state
@throw ::ErrorReadFailed This CompressedVectorWriter, associated ImageFile in undocumented state
@throw ::ErrorWriteFailed This CompressedVectorWriter, associated ImageFile in undocumented state
@throw ::ErrorBadChecksum This CompressedVectorWriter, associated ImageFile in undocumented state
@throw ::ErrorInternal All objects in undocumented state

@see CompressedVectorWriter::write(unsigned), CompressedVectorNode::writer,
CompressedVectorWriter::close, SourceDestBuffer, E57Exception
*/
void CompressedVectorWriter::write( std::vector<SourceDestBuffer> &sbufs, const size_t recordCount )
{
   impl_->write( sbufs, recordCount );
}

/*!
@brief End the write operation.

@details
This function must be called to safely and gracefully end a transfer to a CompressedVectorNode. This
is required because errors cannot be communicated from the CompressedVectorNode destructor (in C++
destructors can't throw exceptions). It is not an error to call this function if the
CompressedVectorWriter is already closed. This function will cause the CompressedVectorWriter to
enter the closed state, and any further transfers requests will fail.

@warning If this function is not called before the CompressedVectorWriter destructor is invoked, all
writes to the CompressedVectorNode will be lost (it will have zero children).

@pre The associated ImageFile must be open.
@post This CompressedVectorWriter is closed (i.e !isOpen())

@throw ::ErrorImageFileNotOpen
@throw ::ErrorSeekFailed This CompressedVectorWriter, associated ImageFile in undocumented state
@throw ::ErrorReadFailed  This CompressedVectorWriter, associated ImageFile in undocumented state
@throw ::ErrorWriteFailed This CompressedVectorWriter, associated ImageFile in undocumented state
@throw ::ErrorBadChecksum This CompressedVectorWriter, associated ImageFile in undocumented state
@throw ::ErrorInternal All objects in undocumented state

@see CompressedVectorWriter::isOpen
*/
void CompressedVectorWriter::close()
{
   impl_->close();
}

/*!
@brief Test whether CompressedVectorWriter is still open for writing.

@pre The associated ImageFile must be open.

@throw ::ErrorImageFileNotOpen
@throw ::ErrorInternal All objects in undocumented state

@see CompressedVectorWriter::close, CompressedVectorNode::writer
*/
bool CompressedVectorWriter::isOpen()
{
   return impl_->isOpen();
}

/*!
@brief Return the CompressedVectorNode being written to.

@pre The associated ImageFile must be open.

@return A smart CompressedVectorNode handle referencing the underlying object being written to.

@throw ::ErrorImageFileNotOpen
@throw ::ErrorInternal All objects in undocumented state

@see CompressedVectorNode::writer
*/
CompressedVectorNode CompressedVectorWriter::compressedVectorNode() const
{
   return impl_->compressedVectorNode();
}

/*!
@brief Diagnostic function to print internal state of object to output stream in an indented format.
@copydetails Node::dump()
*/
#ifdef E57_DEBUG
void CompressedVectorWriter::dump( int indent, std::ostream &os ) const
{
   impl_->dump( indent, os );
}
#else
void CompressedVectorWriter::dump( int indent, std::ostream &os ) const
{
}
#endif
