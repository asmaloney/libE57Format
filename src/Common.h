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

#include <set>
#include <string>
#include <vector>

// Define the following symbol adds some functions to the API for implementation
// purposes. These functions are not available to a normal API user.
#define E57_INTERNAL_IMPLEMENTATION_ENABLE 1
#include "E57Format.h"

#ifdef _MSC_VER
// Disable MSVC warning: warning C4224: nonstandard extension used : formal
// parameter 'locale' was previously defined as a type
#pragma warning( disable : 4224 )
#endif

namespace e57
{
#define E57_EXCEPTION1( ecode )                                                                    \
   ( E57Exception( ( ecode ), ustring(), __FILE__, __LINE__,                                       \
                   static_cast<const char *>( __FUNCTION__ ) ) )
#define E57_EXCEPTION2( ecode, context )                                                           \
   ( E57Exception( ( ecode ), ( context ), __FILE__, __LINE__,                                     \
                   static_cast<const char *>( __FUNCTION__ ) ) )

   using ImageFileImplSharedPtr = std::shared_ptr<class ImageFileImpl>;
   using ImageFileImplWeakPtr = std::weak_ptr<class ImageFileImpl>;
   using NodeImplSharedPtr = std::shared_ptr<class NodeImpl>;
   using NodeImplWeakPtr = std::weak_ptr<class NodeImpl>;

   using StringList = std::vector<std::string>;
   using StringSet = std::set<std::string>;

   /// generates a new random GUID
   std::string generateRandomGUID();
}
