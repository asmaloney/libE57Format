/*
 * Original work Copyright 2009 - 2010 Kevin Ackley (kackley@gwi.net)
 * Modified work Copyright 2020 - 2020 Andy Maloney <asmaloney@gmail.com>
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

#include "VectorNodeImpl.h"
#include "CheckedFile.h"

namespace e57
{
   VectorNodeImpl::VectorNodeImpl( ImageFileImplWeakPtr destImageFile, bool allowHeteroChildren ) :
      StructureNodeImpl( destImageFile ), allowHeteroChildren_( allowHeteroChildren )
   {
      /// don't checkImageFileOpen, StructNodeImpl() will do it
   }

   bool VectorNodeImpl::isTypeEquivalent( NodeImplSharedPtr ni )
   {
      /// don't checkImageFileOpen

      /// Same node type?
      if ( ni->type() != E57_VECTOR )
      {
         return ( false );
      }

      std::shared_ptr<VectorNodeImpl> ai( std::static_pointer_cast<VectorNodeImpl>( ni ) );

      /// allowHeteroChildren must match
      if ( allowHeteroChildren_ != ai->allowHeteroChildren_ )
      {
         return ( false );
      }

      /// Same number of children?
      if ( childCount() != ai->childCount() )
      {
         return ( false );
      }

      /// Check each child, must be in same order
      for ( unsigned i = 0; i < childCount(); i++ )
      {
         if ( !children_.at( i )->isTypeEquivalent( ai->children_.at( i ) ) )
         {
            return ( false );
         }
      }

      /// Types match
      return ( true );
   }

   bool VectorNodeImpl::allowHeteroChildren() const
   {
      checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
      return allowHeteroChildren_;
   }

   void VectorNodeImpl::set( int64_t index64, NodeImplSharedPtr ni )
   {
      checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
      if ( !allowHeteroChildren_ )
      {
         /// New node type must match all existing children
         for ( auto &child : children_ )
         {
            if ( !child->isTypeEquivalent( ni ) )
            {
               throw E57_EXCEPTION2( E57_ERROR_HOMOGENEOUS_VIOLATION, "this->pathName=" + this->pathName() );
            }
         }
      }

      ///??? for now, use base implementation
      StructureNodeImpl::set( index64, ni );
   }

   void VectorNodeImpl::writeXml( ImageFileImplSharedPtr imf, CheckedFile &cf, int indent, const char *forcedFieldName )
   {
      /// don't checkImageFileOpen

      ustring fieldName;
      if ( forcedFieldName != nullptr )
      {
         fieldName = forcedFieldName;
      }
      else
      {
         fieldName = elementName_;
      }

      cf << space( indent ) << "<" << fieldName << " type=\"Vector\" allowHeterogeneousChildren=\""
         << static_cast<int64_t>( allowHeteroChildren_ ) << "\">\n";
      for ( auto &child : children_ )
      {
         child->writeXml( imf, cf, indent + 2, "vectorChild" );
      }
      cf << space( indent ) << "</" << fieldName << ">\n";
   }

#ifdef E57_DEBUG
   void VectorNodeImpl::dump( int indent, std::ostream &os ) const
   {
      /// don't checkImageFileOpen
      os << space( indent ) << "type:        Vector"
         << " (" << type() << ")" << std::endl;
      NodeImpl::dump( indent, os );
      os << space( indent ) << "allowHeteroChildren: " << allowHeteroChildren() << std::endl;
      for ( unsigned i = 0; i < children_.size(); i++ )
      {
         os << space( indent ) << "child[" << i << "]:" << std::endl;
         children_.at( i )->dump( indent + 2, os );
      }
   }
#endif
}
