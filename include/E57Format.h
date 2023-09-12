/*
 * E57Format.h - public header of E57 API for reading/writing .e57 files.
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

#pragma once

/// @file  E57Format.h Header file for the E57 API.

#include <cfloat>
#include <cstdint>
#include <memory>
#include <vector>

#include "E57Exception.h"

namespace e57
{
   using std::int16_t;
   using std::int32_t;
   using std::int64_t;
   using std::int8_t;
   using std::uint16_t;
   using std::uint32_t;
   using std::uint64_t;
   using std::uint8_t;

   // Shorthand for unicode string
   /// @brief UTF-8 encoded Unicode string
   using ustring = std::string;

   /// @brief Identifiers for types of E57 elements
   enum NodeType
   {
      TypeStructure = 1,        ///< StructureNode class
      TypeVector = 2,           ///< VectorNode class
      TypeCompressedVector = 3, ///< CompressedVectorNode class
      TypeInteger = 4,          ///< IntegerNode class
      TypeScaledInteger = 5,    ///< ScaledIntegerNode class
      TypeFloat = 6,            ///< FloatNode class
      TypeString = 7,           ///< StringNode class
      TypeBlob = 8,             ///< BlobNode class

      /// @deprecated Will be removed in 4.0. Use e57::TypeStructure.
      E57_STRUCTURE DEPRECATED_ENUM( "Will be removed in 4.0. Use TypeStructure." ) = TypeStructure,
      /// @deprecated Will be removed in 4.0. Use e57::TypeVector.
      E57_VECTOR DEPRECATED_ENUM( "Will be removed in 4.0. Use TypeVector." ) = TypeVector,
      /// @deprecated Will be removed in 4.0. Use e57::TypeCompressedVector.
      E57_COMPRESSED_VECTOR DEPRECATED_ENUM( "Will be removed in 4.0. Use TypeCompressedVector." ) =
         TypeCompressedVector,
      /// @deprecated Will be removed in 4.0. Use e57::TypeInteger.
      E57_INTEGER DEPRECATED_ENUM( "Will be removed in 4.0. Use TypeInteger." ) = TypeInteger,
      /// @deprecated Will be removed in 4.0. Use e57::TypeScaledInteger.
      E57_SCALED_INTEGER DEPRECATED_ENUM( "Will be removed in 4.0. Use TypeScaledInteger." ) =
         TypeScaledInteger,
      /// @deprecated Will be removed in 4.0. Use e57::TypeFloat.
      E57_FLOAT DEPRECATED_ENUM( "Will be removed in 4.0. Use TypeFloat." ) = TypeFloat,
      /// @deprecated Will be removed in 4.0. Use e57::TypeString.
      E57_STRING DEPRECATED_ENUM( "Will be removed in 4.0. Use TypeString." ) = TypeString,
      /// @deprecated Will be removed in 4.0. Use e57::TypeBlob.
      E57_BLOB DEPRECATED_ENUM( "Will be removed in 4.0. Use TypeBlob." ) = TypeBlob
   };

   /// @brief The IEEE floating point number precisions supported
   enum FloatPrecision
   {
      PrecisionSingle = 1, ///< 32 bit IEEE floating point number format
      PrecisionDouble = 2, ///< 64 bit IEEE floating point number format

      /// @deprecated Will be removed in 4.0. Use e57::PrecisionSingle.
      E57_SINGLE DEPRECATED_ENUM( "Will be removed in 4.0. Use PrecisionSingle." ) =
         PrecisionSingle,
      /// @deprecated Will be removed in 4.0. Use e57::PrecisionDouble.
      E57_DOUBLE DEPRECATED_ENUM( "Will be removed in 4.0. Use PrecisionDouble." ) = PrecisionDouble
   };

   /// @brief Identifies the representations of memory elements API can transfer data to/from
   enum MemoryRepresentation
   {
      Int8 = 1,     ///< 8 bit signed integer
      UInt8 = 2,    ///< 8 bit unsigned integer
      Int16 = 3,    ///< 16 bit signed integer
      UInt16 = 4,   ///< 16 bit unsigned integer
      Int32 = 5,    ///< 32 bit signed integer
      UInt32 = 6,   ///< 32 bit unsigned integer
      Int64 = 7,    ///< 64 bit signed integer
      Bool = 8,     ///< C++ boolean type
      Real32 = 9,   ///< C++ float type
      Real64 = 10,  ///< C++ double type
      UString = 11, ///< Unicode UTF-8 std::string

      /// @deprecated Will be removed in 4.0. Use e57::Int8.
      E57_INT8 DEPRECATED_ENUM( "Will be removed in 4.0. Use Int8." ) = Int8,
      /// @deprecated Will be removed in 4.0. Use e57::UInt8.
      E57_UINT8 DEPRECATED_ENUM( "Will be removed in 4.0. Use UInt8." ) = UInt8,
      /// @deprecated Will be removed in 4.0. Use e57::Int16.
      E57_INT16 DEPRECATED_ENUM( "Will be removed in 4.0. Use Int16." ) = Int16,
      /// @deprecated Will be removed in 4.0. Use e57::UInt16.
      E57_UINT16 DEPRECATED_ENUM( "Will be removed in 4.0. Use UInt16." ) = UInt16,
      /// @deprecated Will be removed in 4.0. Use e57::Int32.
      E57_INT32 DEPRECATED_ENUM( "Will be removed in 4.0. Use Int32." ) = Int32,
      /// @deprecated Will be removed in 4.0. Use e57::UInt32.
      E57_UINT32 DEPRECATED_ENUM( "Will be removed in 4.0. Use UInt32." ) = UInt32,
      /// @deprecated Will be removed in 4.0. Use e57::Int64.
      E57_INT64 DEPRECATED_ENUM( "Will be removed in 4.0. Use Int64." ) = Int64,
      /// @deprecated Will be removed in 4.0. Use e57::Bool.
      E57_BOOL DEPRECATED_ENUM( "Will be removed in 4.0. Use Bool." ) = Bool,
      /// @deprecated Will be removed in 4.0. Use e57::Real32.
      E57_REAL32 DEPRECATED_ENUM( "Will be removed in 4.0. Use Real32." ) = Real32,
      /// @deprecated Will be removed in 4.0. Use e57::Real64.
      E57_REAL64 DEPRECATED_ENUM( "Will be removed in 4.0. Use Real64." ) = Real64,
      /// @deprecated Will be removed in 4.0. Use e57::UString.
      E57_USTRING DEPRECATED_ENUM( "Will be removed in 4.0. Use UString." ) = UString
   };

   /// @brief Default checksum policies for e57::ReadChecksumPolicy
   /// @details These are some convenient default checksum policies, though you can use any value
   /// you want (0-100).
   enum ChecksumPolicy
   {
      ChecksumNone = 0,    ///< Do not verify the checksums. (fast)
      ChecksumSparse = 25, ///< Only verify 25% of the checksums. The last block is always verified.
      ChecksumHalf = 50,   ///< Only verify 50% of the checksums. The last block is always verified.
      ChecksumAll = 100    ///< Verify all checksums. This is the default. (slow)
   };

   /// @brief Specifies the percentage of checksums which are verified when reading an ImageFile
   /// (0-100%).
   /// @see e57::ChecksumPolicy
   using ReadChecksumPolicy = int;

   /// @name Deprecated Checksum Policies
   /// These have been replaced by the enum e57::ChecksumPolicy.
   ///@{

   /// @deprecated Will be removed in 4.0. Use ChecksumPolicy::ChecksumNone.
   [[deprecated(
      "Will be removed in 4.0. Use ChecksumPolicy::ChecksumNone." )]] // TODO Remove in 4.0
   constexpr ReadChecksumPolicy CHECKSUM_POLICY_NONE = ChecksumNone;

   /// @deprecated Will be removed in 4.0. Use ChecksumPolicy::ChecksumSparse.
   [[deprecated(
      "Will be removed in 4.0. Use ChecksumPolicy::ChecksumSparse." )]] // TODO Remove in 4.0
   constexpr ReadChecksumPolicy CHECKSUM_POLICY_SPARSE = ChecksumSparse;

   /// @deprecated Will be removed in 4.0. Use ChecksumPolicy::ChecksumHalf.
   [[deprecated(
      "Will be removed in 4.0. Use ChecksumPolicy::ChecksumHalf." )]] // TODO Remove in 4.0
   constexpr ReadChecksumPolicy CHECKSUM_POLICY_HALF = ChecksumHalf;

   /// @deprecated Will be removed in 4.0. Use ChecksumPolicy::ChecksumAll.
   [[deprecated(
      "Will be removed in 4.0. Use ChecksumPolicy::ChecksumAll." )]] // TODO Remove in 4.0
   constexpr ReadChecksumPolicy CHECKSUM_POLICY_ALL = ChecksumAll;

   ///@}

   /// @brief The URI of ASTM E57 v1.0 standard XML namespace
   /// @note Even though this URI does not point to a valid document, the standard (section 8.4.2.3)
   /// says that this is the required namespace.
   constexpr char VERSION_1_0_URI[] = "http://www.astm.org/COMMIT/E57/2010-e57-v1.0";

   /// @deprecated Will be removed in 4.0. Use e57::VERSION_1_0_URI.
   [[deprecated( "Will be removed in 4.0. Use e57::VERSION_1_0_URI." )]] // TODO Remove in 4.0
   constexpr auto E57_V1_0_URI = VERSION_1_0_URI;

   /// @cond documentNonPublic   The following aren't documented
   // Minimum and maximum values for integers
   constexpr uint8_t UINT8_MIN = 0U;
   constexpr uint16_t UINT16_MIN = 0U;
   constexpr uint32_t UINT32_MIN = 0U;
   constexpr uint64_t UINT64_MIN = 0ULL;

   constexpr float FLOAT_MIN = -FLT_MAX;
   constexpr float FLOAT_MAX = FLT_MAX;
   constexpr double DOUBLE_MIN = -DBL_MAX;
   constexpr double DOUBLE_MAX = DBL_MAX;
   /// @endcond

   /// @cond documentNonPublic   The following isn't part of the API, and isn't documented.
   // Internal implementation files should include Common.h first which defines symbol
   // E57_INTERNAL_IMPLEMENTATION_ENABLE. Normal API users should not define this symbol.
   // Basically the internal version allows access to the pointer to the implementation (impl_)
#ifdef E57_INTERNAL_IMPLEMENTATION_ENABLE
#define E57_INTERNAL_ACCESS( T )                                                                   \
public:                                                                                            \
   std::shared_ptr<T##Impl> impl() const                                                           \
   {                                                                                               \
      return ( impl_ );                                                                            \
   }
#else
#define E57_INTERNAL_ACCESS( T )
#endif
   /// @endcond

   class BlobNode;
   class BlobNodeImpl;
   class CompressedVectorNode;
   class CompressedVectorNodeImpl;
   class CompressedVectorReader;
   class CompressedVectorReaderImpl;
   class CompressedVectorWriter;
   class CompressedVectorWriterImpl;
   class FloatNode;
   class FloatNodeImpl;
   class ImageFile;
   class ImageFileImpl;
   class IntegerNode;
   class IntegerNodeImpl;
   class Node;
   class NodeImpl;
   class ScaledIntegerNode;
   class ScaledIntegerNodeImpl;
   class SourceDestBuffer;
   class SourceDestBufferImpl;
   class StringNode;
   class StringNodeImpl;
   class StructureNode;
   class StructureNodeImpl;
   class VectorNode;
   class VectorNodeImpl;

   class E57_DLL Node
   {
   public:
      Node() = delete;

      NodeType type() const;
      bool isRoot() const;
      Node parent() const;
      ustring pathName() const;
      ustring elementName() const;
      ImageFile destImageFile() const;
      bool isAttached() const;
      void dump( int indent = 0, std::ostream &os = std::cout ) const;
      void checkInvariant( bool doRecurse = true, bool doDowncast = true );
      bool operator==( const Node &n2 ) const;
      bool operator!=( const Node &n2 ) const;

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
#ifdef E57_INTERNAL_IMPLEMENTATION_ENABLE
      explicit Node( std::shared_ptr<NodeImpl> ); // internal use only
#endif

   private:
      friend class NodeImpl;

      E57_INTERNAL_ACCESS( Node )

   protected:
      std::shared_ptr<NodeImpl> impl_;
      /// @endcond
   };

   class E57_DLL StructureNode
   {
   public:
      StructureNode() = delete;
      explicit StructureNode( const ImageFile &destImageFile );

      int64_t childCount() const;
      bool isDefined( const ustring &pathName ) const;
      Node get( int64_t index ) const;
      Node get( const ustring &pathName ) const;
      void set( const ustring &pathName, const Node &n );

      // Up/Down cast conversion
      operator Node() const;
      explicit StructureNode( const Node &n );

      // Common generic Node functions
      bool isRoot() const;
      Node parent() const;
      ustring pathName() const;
      ustring elementName() const;
      ImageFile destImageFile() const;
      bool isAttached() const;

      // Diagnostic functions:
      void dump( int indent = 0, std::ostream &os = std::cout ) const;
      void checkInvariant( bool doRecurse = true, bool doUpcast = true ) const;

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
   private:
      friend class ImageFile;

      explicit StructureNode( std::shared_ptr<StructureNodeImpl> ni );
      explicit StructureNode( std::weak_ptr<ImageFileImpl> fileParent );

      E57_INTERNAL_ACCESS( StructureNode )

   protected:
      std::shared_ptr<StructureNodeImpl> impl_;
      /// @endcond
   };

   class E57_DLL VectorNode
   {
   public:
      VectorNode() = delete;
      explicit VectorNode( const ImageFile &destImageFile, bool allowHeteroChildren = false );

      bool allowHeteroChildren() const;

      int64_t childCount() const;
      bool isDefined( const ustring &pathName ) const;
      Node get( int64_t index ) const;
      Node get( const ustring &pathName ) const;
      void append( const Node &n );

      // Up/Down cast conversion
      operator Node() const;
      explicit VectorNode( const Node &n );

      // Common generic Node functions
      bool isRoot() const;
      Node parent() const;
      ustring pathName() const;
      ustring elementName() const;
      ImageFile destImageFile() const;
      bool isAttached() const;

      // Diagnostic functions:
      void dump( int indent = 0, std::ostream &os = std::cout ) const;
      void checkInvariant( bool doRecurse = true, bool doUpcast = true ) const;

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
   private:
      friend class CompressedVectorNode;

      explicit VectorNode( std::shared_ptr<VectorNodeImpl> ni ); // internal use only

      E57_INTERNAL_ACCESS( VectorNode )

   protected:
      std::shared_ptr<VectorNodeImpl> impl_;
      /// @endcond
   };

   class E57_DLL SourceDestBuffer
   {
   public:
      SourceDestBuffer() = delete;
      SourceDestBuffer( const ImageFile &destImageFile, const ustring &pathName, int8_t *b,
                        size_t capacity, bool doConversion = false, bool doScaling = false,
                        size_t stride = sizeof( int8_t ) );
      SourceDestBuffer( const ImageFile &destImageFile, const ustring &pathName, uint8_t *b,
                        size_t capacity, bool doConversion = false, bool doScaling = false,
                        size_t stride = sizeof( uint8_t ) );
      SourceDestBuffer( const ImageFile &destImageFile, const ustring &pathName, int16_t *b,
                        size_t capacity, bool doConversion = false, bool doScaling = false,
                        size_t stride = sizeof( int16_t ) );
      SourceDestBuffer( const ImageFile &destImageFile, const ustring &pathName, uint16_t *b,
                        size_t capacity, bool doConversion = false, bool doScaling = false,
                        size_t stride = sizeof( uint16_t ) );
      SourceDestBuffer( const ImageFile &destImageFile, const ustring &pathName, int32_t *b,
                        size_t capacity, bool doConversion = false, bool doScaling = false,
                        size_t stride = sizeof( int32_t ) );
      SourceDestBuffer( const ImageFile &destImageFile, const ustring &pathName, uint32_t *b,
                        size_t capacity, bool doConversion = false, bool doScaling = false,
                        size_t stride = sizeof( uint32_t ) );
      SourceDestBuffer( const ImageFile &destImageFile, const ustring &pathName, int64_t *b,
                        size_t capacity, bool doConversion = false, bool doScaling = false,
                        size_t stride = sizeof( int64_t ) );
      SourceDestBuffer( const ImageFile &destImageFile, const ustring &pathName, bool *b,
                        size_t capacity, bool doConversion = false, bool doScaling = false,
                        size_t stride = sizeof( bool ) );
      SourceDestBuffer( const ImageFile &destImageFile, const ustring &pathName, float *b,
                        size_t capacity, bool doConversion = false, bool doScaling = false,
                        size_t stride = sizeof( float ) );
      SourceDestBuffer( const ImageFile &destImageFile, const ustring &pathName, double *b,
                        size_t capacity, bool doConversion = false, bool doScaling = false,
                        size_t stride = sizeof( double ) );
      SourceDestBuffer( const ImageFile &destImageFile, const ustring &pathName,
                        std::vector<ustring> *b );

      ustring pathName() const;
      enum MemoryRepresentation memoryRepresentation() const;
      size_t capacity() const;
      bool doConversion() const;
      bool doScaling() const;
      size_t stride() const;

      // Diagnostic functions:
      void dump( int indent = 0, std::ostream &os = std::cout ) const;
      void checkInvariant( bool doRecurse = true ) const;

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
      E57_INTERNAL_ACCESS( SourceDestBuffer )

   protected:
      std::shared_ptr<SourceDestBufferImpl> impl_;
      /// @endcond
   };

   class E57_DLL CompressedVectorReader
   {
   public:
      CompressedVectorReader() = delete;

      unsigned read();
      unsigned read( std::vector<SourceDestBuffer> &dbufs );
      void seek( int64_t recordNumber ); // !!! not implemented yet
      void close();
      bool isOpen();
      CompressedVectorNode compressedVectorNode() const;

      void dump( int indent = 0, std::ostream &os = std::cout ) const;
      void checkInvariant( bool doRecurse = true );

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
   private:
      friend class CompressedVectorNode;

      explicit CompressedVectorReader( std::shared_ptr<CompressedVectorReaderImpl> ni );

      E57_INTERNAL_ACCESS( CompressedVectorReader )

   protected:
      std::shared_ptr<CompressedVectorReaderImpl> impl_;
      /// @endcond
   };

   class E57_DLL CompressedVectorWriter
   {
   public:
      CompressedVectorWriter() = delete;

      void write( size_t recordCount );
      void write( std::vector<SourceDestBuffer> &sbufs, size_t recordCount );
      void close();
      bool isOpen();
      CompressedVectorNode compressedVectorNode() const;

      void dump( int indent = 0, std::ostream &os = std::cout ) const;
      void checkInvariant( bool doRecurse = true );

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
   private:
      friend class CompressedVectorNode;

      explicit CompressedVectorWriter( std::shared_ptr<CompressedVectorWriterImpl> ni );

      E57_INTERNAL_ACCESS( CompressedVectorWriter )

   protected:
      std::shared_ptr<CompressedVectorWriterImpl> impl_;
      /// @endcond
   };

   class E57_DLL CompressedVectorNode
   {
   public:
      CompressedVectorNode() = delete;
      explicit CompressedVectorNode( const ImageFile &destImageFile, const Node &prototype,
                                     const VectorNode &codecs );

      int64_t childCount() const;
      Node prototype() const;
      VectorNode codecs() const;

      // Iterators
      CompressedVectorWriter writer( std::vector<SourceDestBuffer> &sbufs );
      CompressedVectorReader reader( const std::vector<SourceDestBuffer> &dbufs, bool allowParallel = false );

      // Up/Down cast conversion
      operator Node() const;
      explicit CompressedVectorNode( const Node &n );

      // Common generic Node functions
      bool isRoot() const;
      Node parent() const;
      ustring pathName() const;
      ustring elementName() const;
      ImageFile destImageFile() const;
      bool isAttached() const;

      // Diagnostic functions:
      void dump( int indent = 0, std::ostream &os = std::cout ) const;
      void checkInvariant( bool doRecurse = true, bool doUpcast = true ) const;

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
   private:
      friend class CompressedVectorReader;
      friend class CompressedVectorWriter;
      friend class E57XmlParser;

      CompressedVectorNode( std::shared_ptr<CompressedVectorNodeImpl> ni );

      E57_INTERNAL_ACCESS( CompressedVectorNode )

   protected:
      std::shared_ptr<CompressedVectorNodeImpl> impl_;
      /// @endcond
   };

   class E57_DLL IntegerNode
   {
   public:
      IntegerNode() = delete;
      explicit IntegerNode( const ImageFile &destImageFile, int64_t value = 0,
                            int64_t minimum = INT64_MIN, int64_t maximum = INT64_MAX );

      int64_t value() const;
      int64_t minimum() const;
      int64_t maximum() const;

      // Up/Down cast conversion
      operator Node() const;
      explicit IntegerNode( const Node &n );

      // Common generic Node functions
      bool isRoot() const;
      Node parent() const;
      ustring pathName() const;
      ustring elementName() const;
      ImageFile destImageFile() const;
      bool isAttached() const;

      // Diagnostic functions:
      void dump( int indent = 0, std::ostream &os = std::cout ) const;
      void checkInvariant( bool doRecurse = true, bool doUpcast = true ) const;

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
   private:
      explicit IntegerNode( std::shared_ptr<IntegerNodeImpl> ni );

      E57_INTERNAL_ACCESS( IntegerNode )

   protected:
      std::shared_ptr<IntegerNodeImpl> impl_;
      /// @endcond
   };

   class E57_DLL ScaledIntegerNode
   {
   public:
      ScaledIntegerNode() = delete;
      explicit ScaledIntegerNode( const ImageFile &destImageFile, int64_t rawValue, int64_t minimum,
                                  int64_t maximum, double scale = 1.0, double offset = 0.0 );
      explicit ScaledIntegerNode( const ImageFile &destImageFile, int rawValue, int64_t minimum,
                                  int64_t maximum, double scale = 1.0, double offset = 0.0 );
      explicit ScaledIntegerNode( const ImageFile &destImageFile, int rawValue, int minimum,
                                  int maximum, double scale = 1.0, double offset = 0.0 );
      explicit ScaledIntegerNode( const ImageFile &destImageFile, double scaledValue,
                                  double scaledMinimum, double scaledMaximum, double scale = 1.0,
                                  double offset = 0.0 );

      int64_t rawValue() const;
      double scaledValue() const;
      int64_t minimum() const;
      double scaledMinimum() const;
      int64_t maximum() const;
      double scaledMaximum() const;
      double scale() const;
      double offset() const;

      // Up/Down cast conversion
      operator Node() const;
      explicit ScaledIntegerNode( const Node &n );

      // Common generic Node functions
      bool isRoot() const;
      Node parent() const;
      ustring pathName() const;
      ustring elementName() const;
      ImageFile destImageFile() const;
      bool isAttached() const;

      // Diagnostic functions:
      void dump( int indent = 0, std::ostream &os = std::cout ) const;
      void checkInvariant( bool doRecurse = true, bool doUpcast = true ) const;

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
   private:
      explicit ScaledIntegerNode( std::shared_ptr<ScaledIntegerNodeImpl> ni );

      E57_INTERNAL_ACCESS( ScaledIntegerNode )

   protected:
      std::shared_ptr<ScaledIntegerNodeImpl> impl_;
      /// @endcond
   };

   class E57_DLL FloatNode
   {
   public:
      FloatNode() = delete;
      explicit FloatNode( const ImageFile &destImageFile, double value = 0.0,
                          FloatPrecision precision = PrecisionDouble, double minimum = DOUBLE_MIN,
                          double maximum = DOUBLE_MAX );

      double value() const;
      FloatPrecision precision() const;
      double minimum() const;
      double maximum() const;

      // Up/Down cast conversion
      operator Node() const;
      explicit FloatNode( const Node &n );

      // Common generic Node functions
      bool isRoot() const;
      Node parent() const;
      ustring pathName() const;
      ustring elementName() const;
      ImageFile destImageFile() const;
      bool isAttached() const;

      // Diagnostic functions:
      void dump( int indent = 0, std::ostream &os = std::cout ) const;
      void checkInvariant( bool doRecurse = true, bool doUpcast = true ) const;

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
   private:
      explicit FloatNode( std::shared_ptr<FloatNodeImpl> ni );

      E57_INTERNAL_ACCESS( FloatNode )

   protected:
      std::shared_ptr<FloatNodeImpl> impl_;
      /// @endcond
   };

   class E57_DLL StringNode
   {
   public:
      StringNode() = delete;
      explicit StringNode( const ImageFile &destImageFile, const ustring &value = "" );

      ustring value() const;

      // Up/Down cast conversion
      operator Node() const;
      explicit StringNode( const Node &n );

      // Common generic Node functions
      bool isRoot() const;
      Node parent() const;
      ustring pathName() const;
      ustring elementName() const;
      ImageFile destImageFile() const;
      bool isAttached() const;

      // Diagnostic functions:
      void dump( int indent = 0, std::ostream &os = std::cout ) const;
      void checkInvariant( bool doRecurse = true, bool doUpcast = true ) const;

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
   private:
      friend class StringNodeImpl;

      explicit StringNode( std::shared_ptr<StringNodeImpl> ni );

      E57_INTERNAL_ACCESS( StringNode )

   protected:
      std::shared_ptr<StringNodeImpl> impl_;
      /// @endcond
   };

   class E57_DLL BlobNode
   {
   public:
      BlobNode() = delete;
      explicit BlobNode( const ImageFile &destImageFile, int64_t byteCount );

      int64_t byteCount() const;
      void read( uint8_t *buf, int64_t start, size_t count );
      void write( uint8_t *buf, int64_t start, size_t count );

      // Up/Down cast conversion
      operator Node() const;
      explicit BlobNode( const Node &n );

      // Common generic Node functions
      bool isRoot() const;
      Node parent() const;
      ustring pathName() const;
      ustring elementName() const;
      ImageFile destImageFile() const;
      bool isAttached() const;

      // Diagnostic functions:
      void dump( int indent = 0, std::ostream &os = std::cout ) const;
      void checkInvariant( bool doRecurse = true, bool doUpcast = true ) const;

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
   private:
      friend class E57XmlParser;

      explicit BlobNode( std::shared_ptr<BlobNodeImpl> ni );

      // create blob already in a file
      BlobNode( const ImageFile &destImageFile, int64_t fileOffset, int64_t length );

      E57_INTERNAL_ACCESS( BlobNode )

   protected:
      std::shared_ptr<BlobNodeImpl> impl_;
      /// @endcond
   };

   class E57_DLL ImageFile
   {
   public:
      ImageFile() = delete;
      ImageFile( const ustring &fname, const ustring &mode,
                 ReadChecksumPolicy checksumPolicy = ChecksumAll );
      ImageFile( const char *input, uint64_t size,
                 ReadChecksumPolicy checksumPolicy = ChecksumAll );

      StructureNode root() const;
      void close();
      void cancel();
      bool isOpen() const;
      bool isWritable() const;
      ustring fileName() const;
      int writerCount() const;
      int readerCount() const;

      // Manipulate registered extensions in the file
      void extensionsAdd( const ustring &prefix, const ustring &uri );
      bool extensionsLookupPrefix( const ustring &prefix ) const;
      bool extensionsLookupPrefix( const ustring &prefix, ustring &uri ) const;
      bool extensionsLookupUri( const ustring &uri, ustring &prefix ) const;
      size_t extensionsCount() const;
      ustring extensionsPrefix( size_t index ) const;
      ustring extensionsUri( size_t index ) const;

      // Field name functions:
      bool isElementNameExtended( const ustring &elementName ) const;
      void elementNameParse( const ustring &elementName, ustring &prefix,
                             ustring &localPart ) const;

      // Diagnostic functions:
      void dump( int indent = 0, std::ostream &os = std::cout ) const;
      void checkInvariant( bool doRecurse = true ) const;
      bool operator==( const ImageFile &imf2 ) const;
      bool operator!=( const ImageFile &imf2 ) const;

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
   private:
      explicit ImageFile( double ); // Dummy constructor for better error msg for: ImageFile(0)

      friend class Node;
      friend class StructureNode;
      friend class VectorNode;
      friend class CompressedVectorNode;
      friend class IntegerNode;
      friend class ScaledIntegerNode;
      friend class FloatNode;
      friend class StringNode;
      friend class BlobNode;

      explicit ImageFile( std::shared_ptr<ImageFileImpl> imfi );

      E57_INTERNAL_ACCESS( ImageFile )

   protected:
      std::shared_ptr<ImageFileImpl> impl_;
      /// @endcond
   };
}
