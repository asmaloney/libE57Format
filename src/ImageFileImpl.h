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

#include <memory>

#include "Common.h"

namespace e57
{
   class CheckedFile;

   struct E57FileHeader;
   struct NameSpace;

   class ImageFileImpl : public std::enable_shared_from_this<ImageFileImpl>
   {
   public:
      explicit ImageFileImpl( ReadChecksumPolicy policy );

      void construct2( const ustring &fileName, const ustring &mode );
      void construct2( const char *input, uint64_t size );

      std::shared_ptr<StructureNodeImpl> root();

      void close();
      void cancel();
      bool isOpen() const;
      bool isWriter() const;
      int writerCount() const;
      int readerCount() const;
      ~ImageFileImpl();

      uint64_t allocateSpace( uint64_t byteCount, bool doExtendNow );
      CheckedFile *file() const;
      ustring fileName() const;

      /// Manipulate registered extensions in the file
      void extensionsAdd( const ustring &prefix, const ustring &uri );
      bool extensionsLookupPrefix( const ustring &prefix, ustring &uri ) const;
      bool extensionsLookupUri( const ustring &uri, ustring &prefix ) const;
      size_t extensionsCount() const;
      ustring extensionsPrefix( size_t index ) const;
      ustring extensionsUri( size_t index ) const;

      /// Utility functions:
      bool isElementNameExtended( const ustring &elementName );
      bool isElementNameLegal( const ustring &elementName, bool allowNumber = true );
      bool isPathNameLegal( const ustring &pathName );
      void checkElementNameLegal( const ustring &elementName, bool allowNumber = true );
      void elementNameParse( const ustring &elementName, ustring &prefix, ustring &localPart,
                             bool allowNumber = true );

      void pathNameCheckWellFormed( const ustring &pathName );
      void pathNameParse( const ustring &pathName, bool &isRelative, StringList &fields );
      ustring pathNameUnparse( bool isRelative, const StringList &fields );

      unsigned bitsNeeded( int64_t minimum, int64_t maximum );
      void incrWriterCount();
      void decrWriterCount();
      void incrReaderCount();
      void decrReaderCount();

#ifdef E57_DEBUG
      void dump( int indent = 0, std::ostream &os = std::cout ) const;
#endif

   private:
      friend class E57XmlParser;
      friend class BlobNodeImpl;
      friend class CompressedVectorWriterImpl;
      friend class CompressedVectorReaderImpl;

      static void readFileHeader( CheckedFile *file, E57FileHeader &header );

      void checkImageFileOpen( const char *srcFileName, int srcLineNumber,
                               const char *srcFunctionName ) const;

      ustring fileName_;
      bool isWriter_;
      int writerCount_;
      int readerCount_;

      ReadChecksumPolicy checksumPolicy;

      CheckedFile *file_;

      // Read file attributes
      uint64_t xmlLogicalOffset_;
      uint64_t xmlLogicalLength_;

      // Write file attributes
      uint64_t unusedLogicalStart_;

      /// Bidirectional map from namespace prefix to uri
      std::vector<NameSpace> nameSpaces_;

      /// Smart pointer to metadata tree
      std::shared_ptr<StructureNodeImpl> root_;
   };
}
