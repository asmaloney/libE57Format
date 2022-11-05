/*
 * FloatNode.cpp - implementation of public functions of the FloatNode class.
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

//! @file FloatNode.cpp

#include "FloatNodeImpl.h"
#include "StringFunctions.h"

using namespace e57;

/*!
@class e57::FloatNode
@brief   An E57 element encoding a single or double precision IEEE floating
point number.
@details
An FloatNode is a terminal node (i.e. having no children) that holds an IEEE
floating point value, and minimum/maximum bounds. The precision of the floating
point value and attributes may be either single or double precision. Once the
FloatNode value and attributes are set at creation, they may not be modified.

If the precision option of the FloatNode is E57_SINGLE:
The minimum attribute may be a number in the interval
[-3.402823466e+38, 3.402823466e+38]. The maximum attribute may be a number in
the interval [maximum, 3.402823466e+38]. The value may be a number in the
interval [minimum, maximum].

If the precision option of the FloatNode is E57_DOUBLE:
The minimum attribute may be a number in the interval
[-1.7976931348623158e+308, 1.7976931348623158e+308]. The maximum attribute may
be a number in the interval [maximum, 1.7976931348623158e+308]. The value may be
a number in the interval [minimum, maximum].

See Node class discussion for discussion of the common functions that
StructureNode supports.

@section FloatNode_invariant Class Invariant
A class invariant is a list of statements about an object that are always true
before and after any operation on the object. An invariant is useful for testing
correct operation of an implementation. Statements in an invariant can involve
only externally visible state, or can refer to internal implementation-specific
state that is not visible to the API user. The following C++ code checks
externally visible state for consistency and throws an exception if the
invariant is violated:
@dontinclude FloatNode.cpp
@skip beginExample FloatNode::checkInvariant
@until endExample FloatNode::checkInvariant

@see     Node
*/

/*!
@brief   Create an E57 element for storing an double precision IEEE floating
point number.
@param   [in] destImageFile   The ImageFile where the new node will eventually
be stored.
@param   [in] value     The double precision IEEE floating point value of the
element.
@param   [in] precision The precision of IEEE floating point to use. May be
E57_SINGLE or E57_DOUBLE.
@param   [in] minimum   The smallest value that the value may take.
@param   [in] maximum   The largest value that the value may take.
@details
An FloatNode stores an IEEE floating point number and a lower and upper bound.
The FloatNode class corresponds to the ASTM E57 standard Float element.
See the class discussion at bottom of FloatNode page for more details.

The @a destImageFile indicates which ImageFile the FloatNode will eventually be
attached to. A node is attached to an ImageFile by adding it underneath the
predefined root of the ImageFile (gotten from ImageFile::root). It is not an
error to fail to attach the FloatNode to the @a destImageFile. It is an error to
attempt to attach the FloatNode to a different ImageFile.

There is only one FloatNode constructor that handles both E57_SINGLE and
E57_DOUBLE precision cases. If @a precision = E57_SINGLE, then the object will
silently round the double precision @a value to the nearest representable single
precision value. In this case, the lower bits will be lost, and if the value is
outside the representable range of a single precision number, the exponent may
be changed. The same is true for the @a minimum and @a maximum arguments.

@b Warning: it is an error to give an @a value outside the @a minimum / @a
maximum bounds, even if the FloatNode is destined to be used in a
CompressedVectorNode prototype (where the @a value will be ignored). If the
FloatNode is to be used in a prototype, it is recommended to specify a @a value
= 0 if 0 is within bounds, or a @a value = @a minimum if 0 is not within bounds.
@pre     The @a destImageFile must be open (i.e. destImageFile.isOpen() must be
true).
@pre     The @a destImageFile must have been opened in write mode (i.e.
destImageFile.isWritable() must be true).
@pre     minimum <= value <= maximum
@throw   ::E57_ERROR_BAD_API_ARGUMENT
@throw   ::E57_ERROR_IMAGEFILE_NOT_OPEN
@throw   ::E57_ERROR_FILE_IS_READ_ONLY
@throw   ::E57_ERROR_VALUE_OUT_OF_BOUNDS
@throw   ::E57_ERROR_INTERNAL           All objects in undocumented state
@see     FloatPrecision, FloatNode::value, Node, CompressedVectorNode, CompressedVectorNode::prototype
*/
FloatNode::FloatNode( ImageFile destImageFile, double value, FloatPrecision precision, double minimum,
                      double maximum ) :
   impl_( new FloatNodeImpl( destImageFile.impl(), value, precision, minimum, maximum ) )
{
}

//! @brief   Is this a root node.
//! @copydetails Node::isRoot()
bool FloatNode::isRoot() const
{
   return impl_->isRoot();
}

//! @brief   Return parent of node, or self if a root node.
//! @copydetails Node::parent()
Node FloatNode::parent() const
{
   return Node( impl_->parent() );
}

//! @brief   Get absolute pathname of node.
//! @copydetails Node::pathName()
ustring FloatNode::pathName() const
{
   return impl_->pathName();
}

//! @brief   Get elementName string, that identifies the node in its parent..
//! @copydetails Node::elementName()
ustring FloatNode::elementName() const
{
   return impl_->elementName();
}

//! @brief   Get the ImageFile that was declared as the destination for the node
//! when it was created.
//! @copydetails Node::destImageFile()
ImageFile FloatNode::destImageFile() const
{
   return ImageFile( impl_->destImageFile() );
}

//! @brief   Has node been attached into the tree of an ImageFile.
//! @copydetails Node::isAttached()
bool FloatNode::isAttached() const
{
   return impl_->isAttached();
}

/*!
@brief   Get IEEE floating point value stored.
@details
If precision is E57_SINGLE, the single precision value is returned as a double.
If precision is E57_DOUBLE, the double precision value is returned as a double.
@pre     The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@post    No visible state is modified.
@return  The IEEE floating point value stored, represented as a double.
@throw   ::E57_ERROR_IMAGEFILE_NOT_OPEN
@throw   ::E57_ERROR_INTERNAL           All objects in undocumented state
@see     FloatNode::minimum, FloatNode::maximum
*/
double FloatNode::value() const
{
   return impl_->value();
}

/*!
@brief   Get declared precision of the floating point number.
@pre     The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@post    No visible state is modified.
@return  The declared precision of the floating point number, either
::E57_SINGLE or ::E57_DOUBLE.
@throw   ::E57_ERROR_IMAGEFILE_NOT_OPEN
@throw   ::E57_ERROR_INTERNAL           All objects in undocumented state
@see     FloatPrecision
*/
FloatPrecision FloatNode::precision() const
{
   return impl_->precision();
}

/*!
@brief   Get the declared minimum that the value may take.
@details
If precision is E57_SINGLE, the single precision minimum is returned as a
double. If precision is E57_DOUBLE, the double precision minimum is returned as
a double.
@pre     The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@post    No visible state is modified.
@return  The declared minimum that the value may take.
@throw   ::E57_ERROR_IMAGEFILE_NOT_OPEN
@throw   ::E57_ERROR_INTERNAL           All objects in undocumented state
@see     FloatNode::maximum, FloatNode::value
*/
double FloatNode::minimum() const
{
   return impl_->minimum();
}

/*!
@brief   Get the declared maximum that the value may take.
@details
If precision is E57_SINGLE, the single precision maximum is returned as a
double. If precision is E57_DOUBLE, the double precision maximum is returned as
a double.
@pre     The destination ImageFile must be open (i.e. destImageFile().isOpen()).
@post    No visible state is modified.
@return  The declared maximum that the value may take.
@throw   ::E57_ERROR_IMAGEFILE_NOT_OPEN
@throw   ::E57_ERROR_INTERNAL           All objects in undocumented state
@see     FloatNode::minimum, FloatNode::value
*/
double FloatNode::maximum() const
{
   return impl_->maximum();
}

//! @brief   Diagnostic function to print internal state of object to output
//! stream in an indented format.
//! @copydetails Node::dump()
#ifdef E57_DEBUG
void FloatNode::dump( int indent, std::ostream &os ) const
{
   impl_->dump( indent, os );
}
#else
void FloatNode::dump( int indent, std::ostream &os ) const
{
}
#endif

//! @brief Check whether FloatNode class invariant is true
//! @copydetails IntegerNode::checkInvariant()
// beginExample FloatNode::checkInvariant
void FloatNode::checkInvariant( bool /*doRecurse*/, bool doUpcast )
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

   if ( precision() == E57_SINGLE )
   {
      if ( static_cast<float>( minimum() ) < E57_FLOAT_MIN || static_cast<float>( maximum() ) > E57_FLOAT_MAX )
      {
         throw E57_EXCEPTION1( E57_ERROR_INVARIANCE_VIOLATION );
      }
   }

   // If value is out of bounds
   if ( value() < minimum() || value() > maximum() )
   {
      throw E57_EXCEPTION1( E57_ERROR_INVARIANCE_VIOLATION );
   }
}
// endExample FloatNode::checkInvariant

/*!
@brief   Upcast a FloatNode handle to a generic Node handle.
@details An upcast is always safe, and the compiler can automatically insert it
for initializations of Node variables and Node function arguments.
@return  A smart Node handle referencing the underlying object.
@throw   No E57Exceptions.
@see     Explanation in Node, Node::type()
*/
FloatNode::operator Node() const
{
   /// Upcast from shared_ptr<FloatNodeImpl> to SharedNodeImplPtr and construct
   /// a Node object
   return Node( impl_ );
}

/*!
@brief   Downcast a generic Node handle to a FloatNode handle.
@param   [in] n The generic handle to downcast.
@details The handle @a n must be for an underlying FloatNode, otherwise an
exception is thrown. In designs that need to avoid the exception, use
Node::type() to determine the actual type of the @a n before downcasting. This
function must be explicitly called (c++ compiler cannot insert it
automatically).
@throw   ::E57_ERROR_BAD_NODE_DOWNCAST
@see     Node::type(), FloatNode::operator Node()
*/
FloatNode::FloatNode( const Node &n )
{
   if ( n.type() != E57_FLOAT )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_NODE_DOWNCAST, "nodeType=" + toString( n.type() ) );
   }

   /// Set our shared_ptr to the downcast shared_ptr
   impl_ = std::static_pointer_cast<FloatNodeImpl>( n.impl() );
}

//! @cond documentNonPublic   The following isn't part of the API, and isn't
//! documented.
FloatNode::FloatNode( std::shared_ptr<FloatNodeImpl> ni ) : impl_( ni )
{
}
//! @endcond
