/*
 * CompressedVectorNode.cpp - implementation of public functions of the
 *   CompressedVectorNode class.
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

/// @file CompressedVectorNode.cpp

#include "CompressedVectorNodeImpl.h"
#include "StringFunctions.h"

using namespace e57;

// Put this function first so we can reference the code in doxygen using @skip
/*!
@brief Check whether CompressedVectorNode class invariant is true
@copydetails IntegerNode::checkInvariant()
*/

void CompressedVectorNode::checkInvariant( bool doRecurse, bool doUpcast ) const
{
   // If destImageFile not open, can't test invariant (almost every call would throw)
   if ( !destImageFile().isOpen() )
   {
      return;
   }

   // If requested, call Node::checkInvariant
   if ( doUpcast )
   {
      static_cast<Node>( *this ).checkInvariant( false, false );
   }

   // Check prototype is good Node
   prototype().checkInvariant( doRecurse );

   // prototype attached state not same as this attached state
   if ( prototype().isAttached() != isAttached() )
   {
      throw E57_EXCEPTION1( ErrorInvarianceViolation );
   }

   // prototype not root
   if ( !prototype().isRoot() )
   {
      throw E57_EXCEPTION1( ErrorInvarianceViolation );
   }

   // prototype dest ImageFile not same as this dest ImageFile
   if ( prototype().destImageFile() != destImageFile() )
   {
      throw E57_EXCEPTION1( ErrorInvarianceViolation );
   }

   // Check codecs is good Node
   codecs().checkInvariant( doRecurse );

   // codecs attached state not same as this attached state
   if ( codecs().isAttached() != isAttached() )
   {
      throw E57_EXCEPTION1( ErrorInvarianceViolation );
   }

   // codecs not root
   if ( !codecs().isRoot() )
   {
      throw E57_EXCEPTION1( ErrorInvarianceViolation );
   }

   // codecs dest ImageFile not same as this dest ImageFile
   if ( codecs().destImageFile() != destImageFile() )
   {
      throw E57_EXCEPTION1( ErrorInvarianceViolation );
   }
}

/*!
@class e57::CompressedVectorNode

@brief An E57 element containing ordered vector of child nodes, stored in an efficient binary
format.

@details
The CompressedVectorNode encodes very long sequences of identically typed records. In an E57 file,
the per-point information (coordinates, intensity, color, time stamp etc.) are stored in a
CompressedVectorNode. For time and space efficiency, the CompressedVectorNode data is stored in a
binary section of the E57 file.

Conceptually, the CompressedVectorNode encodes a structure that looks very much like a homogeneous
VectorNode object. However because of the huge volume of data (E57 files can store more than 10
billion points) within a CompressedVectorNode, the functions for accessing the data are dramatically
different. CompressedVectorNode data is accessed in large blocks of records (100s to 1000s at a
time).

Two attributes are required to create a new CompressedVectorNode.

The first attribute describes the shape of the record that will be stored. This record type
description is called the @c prototype of the CompressedVectorNode. Often the @c prototype will be a
StructNode with a single level of named child elements. However, the prototype can be a tree of any
depth consisting of the following node types: IntegerNode, ScaledIntegerNode, FloatNode, StringNode,
StructureNode, or VectorNode (i.e. CompressedVectorNode and BlobNode are not allowed). Only the node
types and attributes are used in the prototype, the values stored are ignored. For example, if the
prototype contains an IntegerNode, with a value=0, minimum=0, maximum=1023, then this means that
each record will contain an integer that can take any value in the interval [0,1023]. As a second
example, if the prototype contains an ScaledIntegerNode, with a value=0, minimum=0, maximum=1023,
scale=.001, offset=0 then this means that each record will contain an integer that can take any
value in the interval [0,1023] and if a reader requests the scaledValue of the field, the rawValue
should be multiplied by 0.001.

The second attribute needed to describe a new CompressedVectorNode is the @c codecs description of
how the values of the records are to be represented on the disk. The codec object is a VectorNode of
a particular format that describes the encoding for each field in the record, which codec will be
used to transfer the values to and from the disk. Currently only one codec is defined for E57 files,
the bitPackCodec, which copies the numbers from memory, removes any unused bit positions, and stores
the without additional spaces on the disk. The bitPackCodec has no configuration options or
parameters to tune. In the ASTM standard, if no codec is specified, the bitPackCodec is assumed. So
specifying the @c codecs as an empty VectorNode is equivalent to requesting at all fields in the
record be encoded with the bitPackCodec.

Other than the @c prototype and @c codecs attributes, the only other state directly accessible is
the number of children (records) in the CompressedVectorNode. The read/write access to the contents
of the CompressedVectorNode is coordinated by two other Foundation API objects:
CompressedVectorReader and CompressedVectorWriter.

@section CompressedVectorNode_invariant Class Invariant

A class invariant is a list of statements about an object that are always true before and after any
operation on the object. An invariant is useful for testing correct operation of an implementation.
Statements in an invariant can involve only externally visible state, or can refer to internal
implementation-specific state that is not visible to the API user. The following C++ code checks
externally visible state for consistency and throws an exception if the invariant is violated:

@dontinclude CompressedVectorNode.cpp
@skip void CompressedVectorNode::checkInvariant
@until ^}

@see CompressedVectorReader, CompressedVectorWriter, Node
*/

/*!
@brief Create an empty CompressedVectorNode, for writing, that will store records specified by the
prototype.

@param [in] destImageFile The ImageFile where the new node will eventually be stored.
@param [in] prototype A tree that describes the fields in each record of the CompressedVectorNode.
@param [in] codecs A VectorNode describing which codecs should be used for each field described in
the prototype.

@details
The @a destImageFile indicates which ImageFile the CompressedVectorNode will eventually be attached
to. A node is attached to an ImageFile by adding it underneath the predefined root of the ImageFile
(gotten from ImageFile::root). It is not an error to fail to attach the CompressedVectorNode to the
@a destImageFile. It is an error to attempt to attach the CompressedVectorNode to a different
ImageFile. The CompressedVectorNode may not be written to until it is attached to the destImageFile
tree.

The @a prototype may be any tree consisting of only the following node types: IntegerNode,
ScaledIntegerNode, FloatNode, StringNode, StructureNode, or VectorNode (i.e. CompressedVectorNode
and BlobNode are not allowed). See CompressedVectorNode for discussion about the @a prototype
argument.

The @a codecs must be a heterogeneous VectorNode with children as specified in the ASTM E57 data
format standard. Since currently only one codec is supported (bitPackCodec), and it is the default,
passing an empty VectorNode will specify that all record fields will be encoded with bitPackCodec.

@pre The @a destImageFile must be open (i.e. destImageFile.isOpen() must be true).
@pre The @a destImageFile must have been opened in write mode (i.e. destImageFile.isWritable() must
be true).
@pre @a prototype must be an unattached root node (i.e. !prototype.isAttached() &&
prototype.isRoot())
@pre @a prototype cannot contain BlobNodes or CompressedVectorNodes.
@pre @a codecs must be an unattached root node (i.e. !codecs.isAttached() && codecs.isRoot())
@post prototype.isAttached()
@post codecs.isAttached()

@throw ::ErrorImageFileNotOpen
@throw ::ErrorFileReadOnly
@throw ::ErrorBadPrototype
@throw ::ErrorBadCodecs
@throw ::ErrorAlreadyHasParent
@throw ::ErrorDifferentDestImageFile
@throw ::ErrorInternal All objects in undocumented state

@see SourceDestBuffer, Node, CompressedVectorNode::reader, CompressedVectorNode::writer
*/
CompressedVectorNode::CompressedVectorNode( const ImageFile &destImageFile, const Node &prototype,
                                            const VectorNode &codecs ) :
   impl_( new CompressedVectorNodeImpl( destImageFile.impl() ) )
{
   // Because of shared_ptr quirks, can't set prototype,codecs in CompressedVectorNodeImpl(), so set
   // it afterwards
   impl_->setPrototype( prototype.impl() );
   impl_->setCodecs( codecs.impl() );
}

/*!
@brief Is this a root node.
@copydetails Node::isRoot()
*/
bool CompressedVectorNode::isRoot() const
{
   return impl_->isRoot();
}

/*!
@brief Return parent of node, or self if a root node.
@copydetails Node::parent()
*/
Node CompressedVectorNode::parent() const
{
   return Node( impl_->parent() );
}

/// @brief Get absolute pathname of node.
/// @copydetails Node::pathName()
ustring CompressedVectorNode::pathName() const
{
   return impl_->pathName();
}

/// @brief Get elementName string, that identifies the node in its parent..
/// @copydetails Node::elementName()
ustring CompressedVectorNode::elementName() const
{
   return impl_->elementName();
}

/// @brief Get the ImageFile that was declared as the destination for the node  when it was created.
/// @copydetails Node::destImageFile()
ImageFile CompressedVectorNode::destImageFile() const
{
   return ImageFile( impl_->destImageFile() );
}

/// @brief Has node been attached into the tree of an ImageFile.
/// @copydetails Node::isAttached()
bool CompressedVectorNode::isAttached() const
{
   return impl_->isAttached();
}

/*!
@brief Get current number of records in a CompressedVectorNode.

@details
For a CompressedVectorNode with an active CompressedVectorWriter, the returned number will reflect
any writes completed.

@pre The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@post No visible state is modified.

@return Current number of records in CompressedVectorNode.

@throw ::ErrorImageFileNotOpen
@throw ::ErrorInternal All objects in undocumented state

@see CompressedVectorNode::reader, CompressedVectorNode::writer
*/
int64_t CompressedVectorNode::childCount() const
{
   return impl_->childCount();
}

/*!
@brief Get the prototype tree that describes the types in the record.

@pre The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@post No visible state is modified.

@return A smart Node handle referencing the root of the prototype tree.

@throw ::ErrorImageFileNotOpen
@throw ::ErrorInternal All objects in undocumented state

@see CompressedVectorNode::CompressedVectorNode, SourceDestBuffer, CompressedVectorNode::reader,
CompressedVectorNode::writer
*/
Node CompressedVectorNode::prototype() const
{
   return Node( impl_->getPrototype() );
}

/*!
@brief Get the codecs tree that describes the encoder/decoder configuration of the
CompressedVectorNode.

@pre The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@post No visible state is modified.

@return A smart VectorNode handle referencing the root of the codecs tree.

@throw ::ErrorImageFileNotOpen
@throw ::ErrorInternal All objects in undocumented state

@see CompressedVectorNode::CompressedVectorNode, SourceDestBuffer, CompressedVectorNode::reader,
CompressedVectorNode::writer
*/
VectorNode CompressedVectorNode::codecs() const
{
   return VectorNode( impl_->getCodecs() );
}

/*!
@brief Diagnostic function to print internal state of object to output stream in an indented format.
@copydetails Node::dump()
*/
#ifdef E57_DEBUG
void CompressedVectorNode::dump( int indent, std::ostream &os ) const
{
   impl_->dump( indent, os );
}
#else
void CompressedVectorNode::dump( int indent, std::ostream &os ) const
{
}
#endif

/*!
@brief Upcast a CompressedVectorNode handle to a generic Node handle.

@details
An upcast is always safe, and the compiler can automatically insert it for initializations of Node
variables and Node function arguments.

@return A smart Node handle referencing the underlying object.

@throw No E57Exceptions.

@see explanation in Node, Node::type(), CompressedVectorNode(const Node&)
*/
CompressedVectorNode::operator Node() const
{
   // Implicitly upcast from shared_ptr<CompressedVectorNodeImpl> to SharedNodeImplPtr and
   // construct a Node object
   return Node( impl_ );
}

/*!
@brief Downcast a generic Node handle to a CompressedVectorNode handle.

@param [in] n The generic handle to downcast.

@details
The handle @a n must be for an underlying CompressedVectorNode, otherwise an exception is thrown. In
designs that need to avoid the exception, use Node::type() to determine the actual type of the @a n
before downcasting. This function must be explicitly called (c++ compiler cannot insert it
automatically).

@throw ::ErrorBadNodeDowncast

@see Node::type(), CompressedVectorNode::operator, Node()
*/
CompressedVectorNode::CompressedVectorNode( const Node &n )
{
   if ( n.type() != TypeCompressedVector )
   {
      throw E57_EXCEPTION2( ErrorBadNodeDowncast, "nodeType=" + toString( n.type() ) );
   }

   // Set our shared_ptr to the downcast shared_ptr
   impl_ = std::static_pointer_cast<CompressedVectorNodeImpl>( n.impl() );
}

/// @cond documentNonPublic The following isn't part of the API, and isn't documented.
CompressedVectorNode::CompressedVectorNode( std::shared_ptr<CompressedVectorNodeImpl> ni ) :
   impl_( ni )
{
}
/// @endcond

/*!
@brief Create an iterator object for writing a series of blocks of data to a CompressedVectorNode.

@param [in] sbufs Vector of memory buffers that will hold data to be written to a
CompressedVectorNode.

@details
See CompressedVectorWriter::write(std::vector<SourceDestBuffer>&, unsigned) for discussion about
restrictions on @a sbufs.

The pathNames in the @a sbufs must match one-to-one with the terminal nodes (i.e. nodes that can
have no children: IntegerNode, ScaledIntegerNode, FloatNode, StringNode) in this
CompressedVectorNode's prototype. It is an error for two SourceDestBuffers in @a dbufs to identify
the same terminal node in the prototype.

It is an error to call this function if the CompressedVectorNode already has any records (i.e. a
CompressedVectorNode cannot be set twice).

@pre @a sbufs can't be empty (i.e. sbufs.length() > 0).
@pre The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@pre The @a destImageFile must have been opened in write mode (i.e. destImageFile.isWritable()).
@pre The destination ImageFile can't have any readers or writers open
(destImageFile().readerCount()==0 && destImageFile().writerCount()==0)
@pre This CompressedVectorNode must be attached (i.e. isAttached()).
@pre This CompressedVectorNode must have no records (i.e. childCount() == 0).

@return A smart CompressedVectorWriter handle referencing the underlying iterator object.

@throw ::ErrorBadAPIArgument
@throw ::ErrorImageFileNotOpen
@throw ::ErrorFileReadOnly
@throw ::ErrorSetTwice
@throw ::ErrorTooManyWriters
@throw ::ErrorTooManyReaders
@throw ::ErrorNodeUnattached
@throw ::ErrorPathUndefined
@throw ::ErrorBufferSizeMismatch
@throw ::ErrorBufferDuplicatePathName
@throw ::ErrorNoBufferForElement
@throw ::ErrorInternal All objects in undocumented state

@see CompressedVectorWriter, SourceDestBuffer, CompressedVectorNode::CompressedVectorNode,
CompressedVectorNode::prototype
*/
CompressedVectorWriter CompressedVectorNode::writer( std::vector<SourceDestBuffer> &sbufs )
{
   return CompressedVectorWriter( impl_->writer( sbufs ) );
}

/*!
@brief Create an iterator object for reading a series of blocks of data from a CompressedVectorNode.

@param [in] dbufs Vector of memory buffers that will receive data read from a CompressedVectorNode.

@details
The pathNames in the @a dbufs must identify terminal nodes (i.e. node that can have no children:
IntegerNode, ScaledIntegerNode, FloatNode, StringNode) in this CompressedVectorNode's prototype. It
is an error for two SourceDestBuffers in @a dbufs to identify the same terminal node in the
prototype. It is not an error to create a CompressedVectorReader for an empty CompressedVectorNode.

@pre @a dbufs can't be empty
@pre The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@pre The destination ImageFile can't have any writers open (destImageFile().writerCount()==0)
@pre This CompressedVectorNode must be attached (i.e. isAttached()).

@return A smart CompressedVectorReader handle referencing the underlying iterator object.

@throw ::ErrorBadAPIArgument
@throw ::ErrorImageFileNotOpen
@throw ::ErrorTooManyWriters
@throw ::ErrorNodeUnattached
@throw ::ErrorPathUndefined
@throw ::ErrorBufferSizeMismatch
@throw ::ErrorBufferDuplicatePathName
@throw ::ErrorBadCVHeader
@throw ::ErrorInternal All objects in undocumented state

@see CompressedVectorReader, SourceDestBuffer, CompressedVectorNode::CompressedVectorNode,
CompressedVectorNode::prototype
*/
CompressedVectorReader CompressedVectorNode::reader( const std::vector<SourceDestBuffer> &dbufs )
{
   return CompressedVectorReader( impl_->reader( dbufs ) );
}
