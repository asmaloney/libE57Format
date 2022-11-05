/*
 * StructureNode.cpp - implementation of public functions of the StructureNode class.
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

//! @file StructureNode.cpp

#include "StringFunctions.h"
#include "StructureNodeImpl.h"

using namespace e57;

/*!
@class e57::StructureNode
@brief   An E57 element containing named child nodes.
@details
A StructureNode is a container of named child nodes, which may be any of the
eight node types. The children of a structure node must have unique
elementNames. Once a child node is set with a particular elementName, it may not
be modified.

See Node class discussion for discussion of the common functions that
StructureNode supports.
@section structurenode_invariant Class Invariant
A class invariant is a list of statements about an object that are always true
before and after any operation on the object. An invariant is useful for testing
correct operation of an implementation. Statements in an invariant can involve
only externally visible state, or can refer to internal implementation-specific
state that is not visible to the API user. The following C++ code checks
externally visible state for consistency and throws an exception if the
invariant is violated:
@dontinclude StructureNode.cpp
@skip beginExample StructureNode::checkInvariant
@until endExample StructureNode::checkInvariant

@see     Node
*/

/*!
@brief   Create an empty StructureNode.
@param   [in] destImageFile   The ImageFile where the new node will eventually
be stored.
@details
A StructureNode is a container for collections of named E57 nodes.
The @a destImageFile indicates which ImageFile the StructureNode will eventually
be attached to. A node is attached to an ImageFile by adding it underneath the
predefined root of the ImageFile (gotten from ImageFile::root). It is not an
error to fail to attach the StructureNode to the @a destImageFile. It is an
error to attempt to attach the StructureNode to a different ImageFile.
@pre     The @a destImageFile must be open (i.e. destImageFile.isOpen() must be
true).
@pre     The @a destImageFile must have been opened in write mode (i.e.
destImageFile.isWritable() must be true).
@throw   ::E57_ERROR_IMAGEFILE_NOT_OPEN
@throw   ::E57_ERROR_INTERNAL           All objects in undocumented state
@see     Node
*/
StructureNode::StructureNode( ImageFile destImageFile ) : impl_( new StructureNodeImpl( destImageFile.impl() ) )
{
}

//! @brief   Is this a root node.
//! @copydetails Node::isRoot()
bool StructureNode::isRoot() const
{
   return impl_->isRoot();
}

//! @brief   Return parent of node, or self if a root node.
//! @copydetails Node::parent()
Node StructureNode::parent() const
{
   return Node( impl_->parent() );
}

//! @brief   Get absolute pathname of node.
//! @copydetails Node::pathName()
ustring StructureNode::pathName() const
{
   return impl_->pathName();
}

//! @brief   Get elementName string, that identifies the node in its parent.
//! @copydetails Node::elementName()
ustring StructureNode::elementName() const
{
   return impl_->elementName();
}

//! @brief   Get the ImageFile that was declared as the destination for the node
//! when it was created.
//! @copydetails Node::destImageFile()
ImageFile StructureNode::destImageFile() const
{
   return ImageFile( impl_->destImageFile() );
}

//! @brief   Has node been attached into the tree of an ImageFile.
//! @copydetails Node::isAttached()
bool StructureNode::isAttached() const
{
   return impl_->isAttached();
}

/*!
@brief   Return number of child nodes contained by this StructureNode.
@pre     The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@post    No visible state is modified.
@return  Number of child nodes contained by this StructureNode.
@throw   ::E57_ERROR_IMAGEFILE_NOT_OPEN
@throw   ::E57_ERROR_INTERNAL           All objects in undocumented state
@see     StructureNode::get(int64_t) const,
StructureNode::set, VectorNode::childCount
*/
int64_t StructureNode::childCount() const
{
   return impl_->childCount();
}

/*!
@brief   Is the given pathName defined relative to this node.
@param   [in] pathName   The absolute pathname, or pathname relative to this
object, to check.
@details
The @a pathName may be relative to this node, or absolute (starting with a "/").
The origin of the absolute path name is the root of the tree that contains this
StructureNode. If this StructureNode is not attached to an ImageFile, the @a
pathName origin root will not the root node of an ImageFile.
@pre     The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@post    No visible state is modified.
@return  true if pathName is currently defined.
@throw   ::E57_ERROR_BAD_PATH_NAME
@throw   ::E57_ERROR_IMAGEFILE_NOT_OPEN
@throw   ::E57_ERROR_INTERNAL           All objects in undocumented state
@see     ImageFile::root, VectorNode::isDefined
*/
bool StructureNode::isDefined( const ustring &pathName ) const
{
   return impl_->isDefined( pathName );
}

/*!
@brief   Get a child element by positional index.
@param   [in] index   The index of child element to get, starting at 0.
@details
The order of children is not specified, and may be different than order the
children were added to the StructureNode. The order of children may change if
more children are added to the StructureNode.
@pre     0 <= @a index < childCount()
@pre     The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@post    No visible state is modified.
@return  A smart Node handle referencing the child node.
@throw   ::E57_ERROR_CHILD_INDEX_OUT_OF_BOUNDS
@throw   ::E57_ERROR_IMAGEFILE_NOT_OPEN
@throw   ::E57_ERROR_INTERNAL           All objects in undocumented state
@see     StructureNode::childCount,
StructureNode::get(const ustring&) const, VectorNode::get
*/
Node StructureNode::get( int64_t index ) const
{
   return Node( impl_->get( index ) );
}

/*!
@brief   Get a child by path name.
@param   [in] pathName   The absolute pathname, or pathname relative to this
object, of the object to get. The @a pathName may be relative to this node, or
absolute (starting with a "/"). The origin of the absolute path name is the root
of the tree that contains this StructureNode. If this StructureNode is not
attached to an ImageFile, the @a pathName origin root will not the root node of
an ImageFile.
@pre     The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@pre     The @a pathName must be defined (i.e. isDefined(pathName)).
@post    No visible state is modified.
@return  A smart Node handle referencing the child node.
@throw   ::E57_ERROR_BAD_PATH_NAME
@throw   ::E57_ERROR_PATH_UNDEFINED
@throw   ::E57_ERROR_IMAGEFILE_NOT_OPEN
@throw   ::E57_ERROR_INTERNAL           All objects in undocumented state
@see     StructureNode::get(int64_t) const
*/
Node StructureNode::get( const ustring &pathName ) const
{
   return Node( impl_->get( pathName ) );
}

/*!
@brief   Add a new child at a given path
    @param   [in] pathName  The absolute pathname, or pathname relative to this
object, that the child object @a n will be given.
@param   [in] n         The node to be added to the tree with given @a pathName.
@details
The @a pathName may be relative to this node, or absolute (starting with a "/").
The origin of the absolute path name is the root of the tree that contains this
StructureNode. If this StructureNode is not attached to an ImageFile, the @a
pathName origin root will not the root node of an ImageFile.

The path name formed from all element names in @a pathName except the last must
exist. If the @a pathName identifies the child of a VectorNode, then the last
element name in @a pathName must be numeric, and be equal to the childCount of
that VectorNode (the request is equivalent to VectorNode::append). The
StructureNode must not be a descendent of a homogeneous VectorNode with more
than one child.

The element naming grammar specified by the ASTM E57 format standard are not
enforced in this function. This would be very difficult to do dynamically, as
some of the naming rules involve combinations of names.
@pre     The new child node @a n must be a root node (i.e. n.isRoot()).
@pre     The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@pre     The associated destImageFile must have been opened in write mode (i.e.
destImageFile().isWritable()).
@pre     The @a pathName must not already be defined (i.e.
!isDefined(pathName)).
@pre     The associated destImageFile of this StructureNode and of @a n must be
same (i.e. destImageFile() == n.destImageFile()).
@post    The @a pathName will be defined (i.e. isDefined(pathName)).
@throw   ::E57_ERROR_IMAGEFILE_NOT_OPEN
@throw   ::E57_ERROR_BAD_PATH_NAME
@throw   ::E57_ERROR_PATH_UNDEFINED
@throw   ::E57_ERROR_SET_TWICE
@throw   ::E57_ERROR_ALREADY_HAS_PARENT
@throw   ::E57_ERROR_DIFFERENT_DEST_IMAGEFILE
@throw   ::E57_ERROR_HOMOGENEOUS_VIOLATION
@throw   ::E57_ERROR_FILE_IS_READ_ONLY
@throw   ::E57_ERROR_INTERNAL           All objects in undocumented state
@see     VectorNode::append
*/
void StructureNode::set( const ustring &pathName, const Node &n )
{
   impl_->set( pathName, n.impl(), false );
}

//! @brief   Diagnostic function to print internal state of object to output
//! stream in an indented format.
//! @copydetails Node::dump()
#ifdef E57_DEBUG
void StructureNode::dump( int indent, std::ostream &os ) const
{
   impl_->dump( indent, os );
}
#else
void StructureNode::dump( int indent, std::ostream &os ) const
{
}
#endif

//! @brief Check whether StructureNode class invariant is true
//! @copydetails IntegerNode::checkInvariant()
// beginExample StructureNode::checkInvariant
void StructureNode::checkInvariant( bool doRecurse, bool doUpcast )
{
   // If destImageFile not open, can't test invariant (almost every call would
   // throw)
   if ( !destImageFile().isOpen() )
   {
      return;
   }

   // If requested, call Node::checkInvariant
   if ( doUpcast )
   {
      static_cast<Node>( *this ).checkInvariant( false, false );
   }

   // Check each child
   for ( int64_t i = 0; i < childCount(); i++ )
   {
      Node child = get( i );

      // If requested, check children recursively
      if ( doRecurse )
      {
         child.checkInvariant( doRecurse, true );
      }

      // Child's parent must be this
      if ( static_cast<Node>( *this ) != child.parent() )
      {
         throw E57_EXCEPTION1( E57_ERROR_INVARIANCE_VIOLATION );
      }

      // Child's elementName must be defined
      if ( !isDefined( child.elementName() ) )
      {
         throw E57_EXCEPTION1( E57_ERROR_INVARIANCE_VIOLATION );
      }

      // Getting child by element name must yield same child
      Node n = get( child.elementName() );
      if ( n != child )
      {
         throw E57_EXCEPTION1( E57_ERROR_INVARIANCE_VIOLATION );
      }
   }
}
// endExample StructureNode::checkInvariant

/*!
@brief   Upcast a StructureNode handle to a generic Node handle.
@details An upcast is always safe, and the compiler can automatically insert it
for initializations of Node variables and Node function arguments.
@return  A smart Node handle referencing the underlying object.
@throw   No E57Exceptions.
@see     explanation in Node, Node::type(), StructureNode(const Node&)
*/
StructureNode::operator Node() const
{
   /// Implicitly upcast from shared_ptr<StructureNodeImpl> to SharedNodeImplPtr
   /// and construct a Node object
   return Node( impl_ );
}

/*!
@brief   Downcast a generic Node handle to a StructureNode handle.
@param   [in] n The generic handle to downcast.
@details The handle @a n must be for an underlying StructureNode, otherwise an
exception is thrown. In designs that need to avoid the exception, use
Node::type() to determine the actual type of the @a n before downcasting. This
function must be explicitly called (c++ compiler cannot insert it
automatically).
@throw   ::E57_ERROR_BAD_NODE_DOWNCAST
@see     Node::type(), StructureNode::operator Node()
*/
StructureNode::StructureNode( const Node &n )
{
   if ( n.type() != E57_STRUCTURE )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_NODE_DOWNCAST, "nodeType=" + toString( n.type() ) );
   }

   /// Set our shared_ptr to the downcast shared_ptr
   impl_ = std::static_pointer_cast<StructureNodeImpl>( n.impl() );
}

//! @cond documentNonPublic   The following isn't part of the API, and isn't
//! documented.
StructureNode::StructureNode( std::weak_ptr<ImageFileImpl> fileParent ) : impl_( new StructureNodeImpl( fileParent ) )
{
}

StructureNode::StructureNode( std::shared_ptr<StructureNodeImpl> ni ) : impl_( ni )
{
}
//! @endcond
