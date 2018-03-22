/**
\file    gnss_error.h
\brief   GNSS core 'c' function library: error and warning macros.
\author  Glenn D. MacGougan (GDM)
\date    2008-03-19
\since   2008-03-19

\remarks

\b "LICENSE INFORMATION" \n
Copyright (c) 2008, refer to 'author' doxygen tags \n
All rights reserved. \n

Redistribution and use in source and binary forms, with or without
modification, are permitted provided the following conditions are met: \n

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer. \n
- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution. \n
- The name(s) of the contributor(s) may not be used to endorse or promote 
  products derived from this software without specific prior written 
  permission. \n

THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY EXPRESS 
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.
*/

#ifndef _C_ERROR_H_
#define _C_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/**
\brief    Output an error message with the following format:\n 
          file, function, line number, msg\n

\code
char msg[128];
char fname[24];
sprintf( fname, "test.txt" );
sprintf( msg, "Unable to open %s", fname );
ERROR_MSG( msg );
\endcode

\author   Glenn D. MacGougan (GDM)
\date     2008-03-19
\since    2008-03-19

*/
#define GNSS_ERROR_MSG( msg )  { const char *themsg = msg; if( themsg != NULL ){ printf( "\n%s,\n%s, %d,\n%s\n", __FILE__, __FUNCTION__, __LINE__, themsg ); }else{ printf( "\n%s,\n%s, %d,\nUnknown Error\n", __FILE__, __FUNCTION__, __LINE__ ); } }

#ifdef __cplusplus
}
#endif


#endif // _C_ERROR_H_
