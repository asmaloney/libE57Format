/*
 * Copyright 2009 - 2010 Kevin Ackley (kackley@gwi.net)
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

#include <climits>

#include "CheckedFile.h"
#include "ImageFileImpl.h"
#include "StructureNodeImpl.h"

using namespace e57;
using namespace std;

StructureNodeImpl::StructureNodeImpl( ImageFileImplWeakPtr destImageFile ) : NodeImpl( destImageFile )
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
}

NodeType StructureNodeImpl::type() const
{
   /// don't checkImageFileOpen
   return E57_STRUCTURE;
}

//??? use visitor?
bool StructureNodeImpl::isTypeEquivalent( NodeImplSharedPtr ni )
{
   /// don't checkImageFileOpen

   /// Same node type?
   if ( ni->type() != E57_STRUCTURE )
      return ( false );

   /// Downcast to shared_ptr<StructureNodeImpl>, should succeed
   shared_ptr<StructureNodeImpl> si( dynamic_pointer_cast<StructureNodeImpl>( ni ) );
   if ( !si ) // check if failed
      throw E57_EXCEPTION2( E57_ERROR_INTERNAL,
                            "this->pathName=" + this->pathName() + " elementName=" + ni->elementName() );

   /// Same number of children?
   if ( childCount() != si->childCount() )
      return ( false );

   /// Check each child is equivalent
   for ( unsigned i = 0; i < childCount(); i++ )
   { //??? vector iterator?
      ustring myChildsFieldName = children_.at( i )->elementName();
      /// Check if matching field name is in same position (to speed things up)
      if ( myChildsFieldName == si->children_.at( i )->elementName() )
      {
         if ( !children_.at( i )->isTypeEquivalent( si->children_.at( i ) ) )
            return ( false );
      }
      else
      {
         /// Children in different order, so lookup by name and check if equal
         /// to our child
         if ( !si->isDefined( myChildsFieldName ) )
            return ( false );
         if ( !children_.at( i )->isTypeEquivalent( si->lookup( myChildsFieldName ) ) )
            return ( false );
      }
   }

   /// Types match
   return ( true );
}

bool StructureNodeImpl::isDefined( const ustring &pathName )
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   NodeImplSharedPtr ni( lookup( pathName ) );
   return ( ni != nullptr );
}

void StructureNodeImpl::setAttachedRecursive()
{
   /// Mark this node as attached to an ImageFile
   isAttached_ = true;

   /// Not a leaf node, so mark all our children
   for ( auto &child : children_ )
   {
      child->setAttachedRecursive();
   }
}

int64_t StructureNodeImpl::childCount() const
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );

   return children_.size();
}
NodeImplSharedPtr StructureNodeImpl::get( int64_t index )
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   if ( index < 0 || index >= static_cast<int64_t>( children_.size() ) )
   { // %%% Possible truncation on platforms where size_t = uint64
      throw E57_EXCEPTION2( E57_ERROR_CHILD_INDEX_OUT_OF_BOUNDS, "this->pathName=" + this->pathName() +
                                                                    " index=" + toString( index ) +
                                                                    " size=" + toString( children_.size() ) );
   }
   return ( children_.at( static_cast<unsigned>( index ) ) );
}

NodeImplSharedPtr StructureNodeImpl::get( const ustring &pathName )
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   NodeImplSharedPtr ni( lookup( pathName ) );

   if ( !ni )
      throw E57_EXCEPTION2( E57_ERROR_PATH_UNDEFINED, "this->pathName=" + this->pathName() + " pathName=" + pathName );
   return ( ni );
}

NodeImplSharedPtr StructureNodeImpl::lookup( const ustring &pathName )
{
   /// don't checkImageFileOpen
   //??? use lookup(fields, level) instead, for speed.
   bool isRelative;
   vector<ustring> fields;
   ImageFileImplSharedPtr imf( destImageFile_ );
   imf->pathNameParse( pathName, isRelative, fields ); // throws if bad pathName

   if ( isRelative || isRoot() )
   {
      if ( fields.empty() )
         if ( isRelative )
         {
            return NodeImplSharedPtr(); /// empty pointer
         }
         else
         {
            NodeImplSharedPtr root( getRoot() );
            return ( root );
         }
      else
      {
         /// Find child with elementName that matches first field in path
         unsigned i;
         for ( i = 0; i < children_.size(); i++ )
         {
            if ( fields.at( 0 ) == children_.at( i )->elementName() )
               break;
         }
         if ( i == children_.size() )
         {
            return NodeImplSharedPtr(); /// empty pointer
         }

         if ( fields.size() == 1 )
         {
            return ( children_.at( i ) );
         }

         //??? use level here rather than unparse
         /// Remove first field in path
         fields.erase( fields.begin() );

         /// Call lookup on child object with remaining fields in path name
         return children_.at( i )->lookup( imf->pathNameUnparse( true, fields ) );
      }
   }
   else
   { /// Absolute pathname and we aren't at the root
      /// Find root of the tree
      NodeImplSharedPtr root( getRoot() );

      /// Call lookup on root
      return ( root->lookup( pathName ) );
   }
}

void StructureNodeImpl::set( int64_t index64, NodeImplSharedPtr ni )
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );

   auto index = static_cast<unsigned>( index64 );

   /// Allow index == current number of elements, interpret as append
   if ( index64 < 0 || index64 > UINT_MAX || index > children_.size() )
   {
      throw E57_EXCEPTION2( E57_ERROR_CHILD_INDEX_OUT_OF_BOUNDS, "this->pathName=" + this->pathName() +
                                                                    " index=" + toString( index64 ) +
                                                                    " size=" + toString( children_.size() ) );
   }

   /// Enforce "set once" policy, only allow append
   if ( index != children_.size() )
   {
      throw E57_EXCEPTION2( E57_ERROR_SET_TWICE,
                            "this->pathName=" + this->pathName() + " index=" + toString( index64 ) );
   }

   /// Verify that child is destined for same ImageFile as this is
   ImageFileImplSharedPtr thisDest( destImageFile() );
   ImageFileImplSharedPtr niDest( ni->destImageFile() );
   if ( thisDest != niDest )
   {
      throw E57_EXCEPTION2( E57_ERROR_DIFFERENT_DEST_IMAGEFILE,
                            "this->destImageFile" + thisDest->fileName() + " ni->destImageFile" + niDest->fileName() );
   }

   /// Field name is string version of index value, e.g. "14"
   stringstream elementName;
   elementName << index;

   /// If this struct is type constrained, can't add new child
   if ( isTypeConstrained() )
   {
      throw E57_EXCEPTION2( E57_ERROR_HOMOGENEOUS_VIOLATION, "this->pathName=" + this->pathName() );
   }

   ni->setParent( shared_from_this(), elementName.str() );
   children_.push_back( ni );
}

void StructureNodeImpl::set( const ustring &pathName, NodeImplSharedPtr ni, bool autoPathCreate )
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   //??? parse pathName! throw if impossible, absolute and multi-level paths...
   //??? enforce type constraints on path (non-zero index types match zero index
   // types for VECTOR,
   // COMPRESSED_VECTOR

#ifdef E57_MAX_VERBOSE
   cout << "StructureNodeImpl::set(pathName=" << pathName << ", ni, autoPathCreate=" << autoPathCreate << endl;
#endif

   bool isRelative;
   vector<ustring> fields;

   /// Path may be absolute or relative with several levels.  Break string into
   /// individual levels.
   ImageFileImplSharedPtr imf( destImageFile_ );
   imf->pathNameParse( pathName, isRelative, fields ); // throws if bad pathName
   if ( isRelative )
   {
      /// Relative path, starting from current object, e.g. "foo/17/bar"
      set( fields, 0, ni, autoPathCreate );
   }
   else
   {
      /// Absolute path (starting from root), e.g. "/foo/17/bar"
      getRoot()->set( fields, 0, ni, autoPathCreate );
   }
}

void StructureNodeImpl::set( const vector<ustring> &fields, unsigned level, NodeImplSharedPtr ni, bool autoPathCreate )
{
#ifdef E57_MAX_VERBOSE
   cout << "StructureNodeImpl::set: level=" << level << endl;
   for ( unsigned i = 0; i < fields.size(); i++ )
      cout << "  field[" << i << "]: " << fields.at( i ) << endl;
#endif

   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   //??? check if field is numeric string (e.g. "17"), verify number is same as
   // index, else throw
   // bad_path

   /// Check if trying to set the root node "/", which is illegal
   if ( level == 0 && fields.empty() )
      throw E57_EXCEPTION2( E57_ERROR_SET_TWICE, "this->pathName=" + this->pathName() + " element=/" );

   /// Serial search for matching field name, if find match, have error since
   /// can't set twice
   for ( auto &child : children_ )
   {
      if ( fields.at( level ) == child->elementName() )
      {
         if ( level == fields.size() - 1 )
         {
            /// Enforce "set once" policy, don't allow reset
            throw E57_EXCEPTION2( E57_ERROR_SET_TWICE,
                                  "this->pathName=" + this->pathName() + " element=" + fields[level] );
         }

         /// Recurse on child
         child->set( fields, level + 1, ni );

         return;
      }
   }
   /// Didn't find matching field name, so have a new child.

   /// If this struct is type constrained, can't add new child
   if ( isTypeConstrained() )
      throw E57_EXCEPTION2( E57_ERROR_HOMOGENEOUS_VIOLATION, "this->pathName=" + this->pathName() );

   /// Check if we are at bottom level
   if ( level == fields.size() - 1 )
   {
      /// At bottom, so append node at end of children
      ni->setParent( shared_from_this(), fields.at( level ) );
      children_.push_back( ni );
   }
   else
   {
      /// Not at bottom level, if not autoPathCreate have an error
      if ( !autoPathCreate )
      {
         throw E57_EXCEPTION2( E57_ERROR_PATH_UNDEFINED,
                               "this->pathName=" + this->pathName() + " field=" + fields.at( level ) );
      }
      //??? what if extra fields are numbers?

      /// Do autoPathCreate: Create nested Struct objects for extra field names
      /// in path
      NodeImplSharedPtr parent( shared_from_this() );
      for ( ; level != fields.size() - 1; level++ )
      {
         shared_ptr<StructureNodeImpl> child( new StructureNodeImpl( destImageFile_ ) );
         parent->set( fields.at( level ), child );
         parent = child;
      }
      parent->set( fields.at( level ), ni );
   }
}

void StructureNodeImpl::append( NodeImplSharedPtr ni )
{
   /// don't checkImageFileOpen, set() will do it

   /// Create new node at end of list with integer field name
   set( childCount(), ni );
}

//??? use visitor?
void StructureNodeImpl::checkLeavesInSet( const StringSet &pathNames, NodeImplSharedPtr origin )
{
   /// don't checkImageFileOpen

   /// Not a leaf node, so check all our children
   for ( auto &child : children_ )
   {
      child->checkLeavesInSet( pathNames, origin );
   }
}

//??? use visitor?
void StructureNodeImpl::writeXml( ImageFileImplSharedPtr imf, CheckedFile &cf, int indent, const char *forcedFieldName )
{
   /// don't checkImageFileOpen

   ustring fieldName;
   if ( forcedFieldName != nullptr )
      fieldName = forcedFieldName;
   else
      fieldName = elementName_;

   cf << space( indent ) << "<" << fieldName << " type=\"Structure\"";

   /// If this struct is the root for the E57 file, add name space declarations
   /// Note the prototype of a CompressedVector is a separate tree, so don't
   /// want to write out namespaces if not the ImageFile root
   if ( isRoot() && shared_from_this() == imf->root() )
   {
      bool gotDefaultNamespace = false;
      for ( size_t i = 0; i < imf->extensionsCount(); i++ )
      {
         const char *xmlnsExtension;
         if ( imf->extensionsPrefix( i ).empty() )
         {
            gotDefaultNamespace = true;
            xmlnsExtension = "xmlns";
         }
         else
            xmlnsExtension = "xmlns:";
         cf << "\n"
            << space( indent + fieldName.length() + 2 ) << xmlnsExtension << imf->extensionsPrefix( i ) << "=\""
            << imf->extensionsUri( i ) << "\"";
      }

      /// If user didn't explicitly declare a default namespace, use the current
      /// E57 standard one.
      if ( !gotDefaultNamespace )
         cf << "\n" << space( indent + fieldName.length() + 2 ) << "xmlns=\"" << E57_V1_0_URI << "\"";
   }
   if ( !children_.empty() )
   {
      cf << ">\n";

      /// Write all children nested inside Structure element
      for ( auto &child : children_ )
      {
         child->writeXml( imf, cf, indent + 2 );
      }

      /// Write closing tag
      cf << space( indent ) << "</" << fieldName << ">\n";
   }
   else
   {
      /// XML element has no child elements
      cf << "/>\n";
   }
}

//??? use visitor?
#ifdef E57_DEBUG
void StructureNodeImpl::dump( int indent, ostream &os ) const
{
   /// don't checkImageFileOpen
   os << space( indent ) << "type:        Structure"
      << " (" << type() << ")" << endl;
   NodeImpl::dump( indent, os );
   for ( unsigned i = 0; i < children_.size(); i++ )
   {
      os << space( indent ) << "child[" << i << "]:" << endl;
      children_.at( i )->dump( indent + 2, os );
   }
}
#endif
