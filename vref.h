/****************************************************************************
 #
 #  VSTRING Library
 #
 #  Copyright (c) 1996-2023 Vladi Belperchinov-Shabanski "Cade" 
 #  http://cade.noxrun.com/  <cade@noxrun.com> <cade@bis.bg> <cade@cpan.org>
 #
 #  Distributed under the GPL license, you should receive copy of GPLv2!
 #
 #  SEE 'README', 'LICENSE' OR 'COPYING' FILE FOR LICENSE AND OTHER DETAILS!
 #
 #  VSTRING library provides wide set of string manipulation features
 #  including dynamic string object that can be freely exchanged with
 #  standard char* (or wchar_t*) type, so there is no need to change 
 #  function calls nor the implementation when you change from 
 #  char* to VString (and from wchar_t* to WString). 
 # 
 ***************************************************************************/

#ifndef _VREF_H_
#define _VREF_H_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include <assert.h>
#ifndef ASSERT
#define ASSERT assert
#endif

/***************************************************************************
**
** VREF
**
****************************************************************************/

class VRef
{
  int _ref;

public:

  VRef() { _ref = 1; }  // creator get first reference
  virtual ~VRef() { ASSERT( _ref == 0 ); }

  void ref() { _ref++; }
  void unref() { ASSERT( _ref > 0 ); _ref--; if ( _ref < 1 ) delete this; }

  int refs() { return _ref; }
};

/****************************************************************************
**
** aux functions
**
****************************************************************************/

  void *vs_memcpy(  char    *dest, const char    *src, size_t n );
  void *vs_memmove( char    *dest, const char    *src, size_t n );
  void *vs_memcpy(  wchar_t *dest, const wchar_t *src, size_t n );
  void *vs_memmove( wchar_t *dest, const wchar_t *src, size_t n );

#endif /* TOP */

/***************************************************************************
**
** EOF
**
****************************************************************************/



