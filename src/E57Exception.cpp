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

#include "E57Exception.h"

namespace e57
{
   /*!
   @class E57Exception

   @brief Object thrown by E57 API functions to communicate the conditions of an error.

   @details
   The E57Exception object communicates information about errors occurring in calls to the E57 API
   functions. The error information is communicated from the location in the E57 implementation
   where the error was detected to the @c catch statement in the code of the API user. The state of
   E57Exception object has one mandatory field, the errorCode, and several optional fields that can
   be set depending on the debug level of the E57 implementation. There are three optional fields
   that encode the location in the source code of the E57 implementation where the error was
   detected: @c sourceFileName, @c sourceFunctionName, and @c sourceLineNumber. Another optional
   field is the @c context string that is human (or at least programmer) readable, which can capture
   some variable values that might be useful in debugging. The E57Exception class is derived from
   std::exception. So applications that only catch std::exceptions will detect E57Exceptions (but
   with no information about the origin of the error).

   Many other APIs use error codes (defined integer constants) returned from the API functions to
   communicate success or failure of the requested command. In contrast, the E57 API uses the C++
   exception mechanism to communicate failure (success is communicated by the return of the function
   without exception). ::Success is never thrown. The API ErrorCode is packaged inside the
   E57Exception. The documentation for each function in the API declares which ErrorCode values
   (inside an E57Exception) can possibly be thrown by the function. Some API functions do not throw
   any E57Exceptions, and this is documented by the designation "No E57Exceptions." in the
   "Exceptions:" section of the function documentation page.

   If an API function does throw an E57Exception, the API user will rightfully be concerned about
   the state of all of the API objects. There are four categories of guarantee about the state of
   all objects that the API specifies.

   1) <b>All objects unchanged</b> - all API objects are left in their original state before the API
   function was called. This is the default guarantee, so if there is no notation next to the
   ErrorCode in the "Exceptions:" section of the function documentation page, the this category is
   implied.

   2) <b>XXX object modified, but consistent</b> - The given object (or objects) have been modified,
   but are left in a consistent state.

   3) <b>XXX object in undocumented state</b> - The given object (or objects) may have been left in
   an inconsistent state, and the only safe thing to do with them is to call their destructor.

   4) <b>All objects in undocumented state</b> - A very serious consistency error has been detected,
   and the state of all API objects is suspect.  The only safe thing to do is to call their
   destructors.

   Almost all of the API functions can throw the following two ErrorCodes: ::ErrorImageFileNotOpen
   and ::ErrorInternal. In some E57 implementations, the tree information may be stored on disk
   rather than in memory. If the disk file is closed, even the most basic information may not be
   available about nodes in the tree. So if the ImageFile is closed (by calling ImageFile::close),
   the API user must be ready for many of the API functions to throw ::ErrorImageFileNotOpen.
   Secondly, regarding the ErrorInternal error, there is a lot of consistency checking in the
   Reference Implementation, and there may be much more added. Even if some API routines do not now
   throw ::ErrorInternal, they could some time in the future, or in different implementations. So
   the right to throw ::ErrorInternal is reserved for every API function (except those that by
   design can't throw E57Exceptions).

   It is strongly recommended that catch statements in user code that call API functions catch
   E57Exception by reference (i.e. <tt>catch (E57Exception& ex)</tt> and, if necessary, rethrow
   using the syntax that throws the currently active exception (i.e. <tt>throw;</tt>).

   Exceptions other that E57Exception may be thrown by calls to API functions (e.g. std::bad_alloc).
   Production code will likely have catch handlers for these exceptions as well.
   */

   /*!
   @fn const char *E57Exception::what()
   @brief Get string description of exception category.

   @details
   Returns "E57 Exception" for all E57Exceptions, no matter what the errorCode.

   @post No visible state is modified.

   @return The string description of exception category.

   @throw No E57Exceptions.
   */

   /*!
   @fn    void E57Exception::report( const char *reportingFileName, int reportingLineNumber,
                              const char *reportingFunctionName, std::ostream &os )
   @brief Print error information on a given output stream.

   @param [in] reportingFileName Name of file where catch statement caught the exception. NULL if
   unknown.
   @param [in] reportingLineNumber Number of source code line where catch statement caught the
   exception. 0 if unknown.
   @param [in] reportingFunctionName String name of function containing catch statement that caught
   the exception. NULL if unknown.
   @param [in] os Output string to print a summary of exception information and location of catch
   statement.

   @details
   The amount of information printed to output stream may depend on whether the E57 implementation
   was built with debugging enabled.

   @post No visible state is modified.

   @throw No E57Exceptions.

   @see ErrorCode
   */

   /*!
   @fn ErrorCode E57Exception::errorCode()
   @brief Get numeric ::ErrorCode associated with the exception.

   @post No visible state is modified.

   @return The numeric ::ErrorCode associated with the exception.

   @throw No E57Exceptions.

   @see E57Exception::errorStr, Utilities::errorCodeToString, ErrorCode
   */

   /*!
   @fn std::string E57Exception::errorStr()
   @brief Get error string associated with the exception.

   @post No visible state is modified.

   @return The error string associated with the exception.

   @throw No E57Exceptions.
   */

   /*!
   @fn std::string E57Exception::context()
   @brief Get human-readable string that describes the context of the error.

   @details
   The context string may include values in object state, or function arguments. The format of the
   context string is not standardized. However, in the Reference Implementation, many strings
   contain a sequence of " VARNAME=VARVALUE" fields.

   @post No visible state is modified.

   @return The human-readable string that describes the context of the error.

   @throw No E57Exceptions.
   */

   /*!
   @fn const char *E57Exception::sourceFileName()
   @brief Get name of source file where exception occurred, for debugging.

   @details
   May return the value of the macro variable __FILE__ at the location where the E57Exception was
   constructed. May return the empty string ("") in some E57 implementations.

   @post No visible state is modified.

   @return The name of source file where exception occurred, for debugging.

   @throw No E57Exceptions.
   */

   /*!
   @fn const char *E57Exception::sourceFunctionName()
   @brief Get name of function in source code where the error occurred , for
   debugging.

   @details
   May return the value of the macro variable __FUNCTION__ at the location where the E57Exception
   was constructed. May return the empty string ("") in some E57 implementations.

   @post No visible state is modified.

   @return The name of source code function where the error occurred , for debugging.

   @throw No E57Exceptions.
   */

   /*!
   @fn int E57Exception::sourceLineNumber()
   @brief Get line number in source code file where exception occurred, for debugging.

   @details
   May return the value of the macro variable __LINE__ at the location where the E57Exception was
   constructed. May return the empty string ("") in some E57 implementations.

   @post No visible state is modified.

   @return The line number in source code file where exception occurred, for debugging.

   @throw No E57Exceptions.
   */

   //=====================================================================================

   /*!
   @brief Get short string description of an E57 ErrorCode.

   @param [in] ecode The numeric errorCode from an E57Exception.

   @details
   The errorCode is translated into a one-line English string.

   @return English std::string describing error.

   @throw No E57Exceptions.

   @see E57Exception::errorCode
   */
   std::string Utilities::errorCodeToString( ErrorCode ecode ) noexcept
   {
      switch ( ecode )
      {
         case Success:
            return "operation was successful (Success)";
         case ErrorBadCVHeader:
            return "a CompressedVector binary header was bad (ErrorBadCVHeader)";
         case ErrorBadCVPacket:
            return "a CompressedVector binary packet was bad (ErrorBadCVPacket)";
         case ErrorChildIndexOutOfBounds:
            return "a numerical index identifying a child was out of bounds "
                   "(ErrorChildIndexOutOfBounds)";
         case ErrorSetTwice:
            return "attempted to set an existing child element to a new value (ErrorSetTwice)";
         case ErrorHomogeneousViolation:
            return "attempted to add an E57 Element that would have made the children of a "
                   "homogeneous Vector have "
                   "different types (E57_ERROR_HOMOGENEOUS_VIOLATION)";
         case ErrorValueNotRepresentable:
            return "a value could not be represented in the requested type "
                   "(ErrorValueNotRepresentable)";
         case ErrorScaledValueNotRepresentable:
            return "after scaling the result could not be represented in the requested type "
                   "(ErrorScaledValueNotRepresentable)";
         case ErrorReal64TooLarge:
            return "a 64 bit IEEE float was too large to store in a 32 bit IEEE float "
                   "(ErrorReal64TooLarge)";
         case ErrorExpectingNumeric:
            return "Expecting numeric representation in user's buffer, found ustring "
                   "(ErrorExpectingNumeric)";
         case ErrorExpectingUString:
            return "Expecting string representation in user's buffer, found numeric "
                   "(ErrorExpectingUString)";
         case ErrorInternal:
            return "An unrecoverable inconsistent internal state was detected (ErrorInternal)";
         case ErrorBadXMLFormat:
            return "E57 primitive not encoded in XML correctly (ErrorBadXMLFormat)";
         case ErrorXMLParser:
            return "XML not well formed (ErrorXMLParser)";
         case ErrorBadAPIArgument:
            return "bad API function argument provided by user (ErrorBadAPIArgument)";
         case ErrorFileReadOnly:
            return "can't modify read only file (ErrorFileReadOnly)";
         case ErrorBadChecksum:
            return "checksum mismatch, file is corrupted (ErrorBadChecksum)";
         case ErrorOpenFailed:
            return "open() failed (ErrorOpenFailed)";
         case ErrorCloseFailed:
            return "close() failed (ErrorCloseFailed)";
         case ErrorReadFailed:
            return "read() failed (ErrorReadFailed)";
         case ErrorWriteFailed:
            return "write() failed (ErrorWriteFailed)";
         case ErrorSeekFailed:
            return "lseek() failed (ErrorSeekFailed)";
         case ErrorPathUndefined:
            return "E57 element path well formed but not defined (ErrorPathUndefined)";
         case ErrorBadBuffer:
            return "bad SourceDestBuffer (ErrorBadBuffer)";
         case ErrorNoBufferForElement:
            return "no buffer specified for an element in CompressedVectorNode during write "
                   "(ErrorNoBufferForElement)";
         case ErrorBufferSizeMismatch:
            return "SourceDestBuffers not all same size (ErrorBufferSizeMismatch)";
         case ErrorBufferDuplicatePathName:
            return "duplicate pathname in CompressedVectorNode read/write "
                   "(ErrorBufferDuplicatePathName)";
         case ErrorBadFileSignature:
            return "file signature not ASTM-E57 (ErrorBadFileSignature)";
         case ErrorUnknownFileVersion:
            return "incompatible file version (ErrorUnknownFileVersion)";
         case ErrorBadFileLength:
            return "size in file header not same as actual (ErrorBadFileLength)";
         case ErrorXMLParserInit:
            return "XML parser failed to initialize (ErrorXMLParserInit)";
         case ErrorDuplicateNamespacePrefix:
            return "namespace prefix already defined (ErrorDuplicateNamespacePrefix)";
         case ErrorDuplicateNamespaceURI:
            return "namespace URI already defined (ErrorDuplicateNamespaceURI)";
         case ErrorBadPrototype:
            return "bad prototype in CompressedVectorNode (ErrorBadPrototype)";
         case ErrorBadCodecs:
            return "bad codecs in CompressedVectorNode (ErrorBadCodecs)";
         case ErrorValueOutOfBounds:
            return "element value out of min/max bounds (ErrorValueOutOfBounds)";
         case ErrorConversionRequired:
            return "conversion required to assign element value, but not requested "
                   "(ErrorConversionRequired)";
         case ErrorBadPathName:
            return "E57 path name is not well formed (ErrorBadPathName)";
         case ErrorNotImplemented:
            return "functionality not implemented (ErrorNotImplemented)";
         case ErrorBadNodeDowncast:
            return "bad downcast from Node to specific node type (ErrorBadNodeDowncast)";
         case ErrorWriterNotOpen:
            return "CompressedVectorWriter is no longer open (ErrorWriterNotOpen)";
         case ErrorReaderNotOpen:
            return "CompressedVectorReader is no longer open (ErrorReaderNotOpen)";
         case ErrorNodeUnattached:
            return "node is not yet attached to tree of ImageFile (ErrorNodeUnattached)";
         case ErrorAlreadyHasParent:
            return "node already has a parent (ErrorAlreadyHasParent)";
         case ErrorDifferentDestImageFile:
            return "nodes were constructed with different destImageFiles "
                   "(ErrorDifferentDestImageFile)";
         case ErrorImageFileNotOpen:
            return "destImageFile is no longer open (ErrorImageFileNotOpen)";
         case ErrorBuffersNotCompatible:
            return "SourceDestBuffers not compatible with previously given ones "
                   "(ErrorBuffersNotCompatible)";
         case ErrorTooManyWriters:
            return "too many open CompressedVectorWriters of an ImageFile (ErrorTooManyWriters)";
         case ErrorTooManyReaders:
            return "too many open CompressedVectorReaders of an ImageFile (ErrorTooManyReaders)";
         case ErrorBadConfiguration:
            return "bad configuration string (ErrorBadConfiguration)";
         case ErrorInvarianceViolation:
            return "class invariance constraint violation in debug mode (ErrorInvarianceViolation)";
         case ErrorInvalidNodeType:
            return "an invalid node type was passed in Data3D pointFields";

         default:
            return "unknown error (" + std::to_string( ecode ) + ")";
      }
   }
}
