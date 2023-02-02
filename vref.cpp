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

#include "vdef.h"
#include "vref.h"

/****************************************************************************
**
** aux functions
**
****************************************************************************/

  void *vs_memcpy( char *dest, const char *src, size_t n )
  {
    return memcpy( dest, src, n * sizeof( char ) );
  }

  void *vs_memmove( char *dest, const char *src, size_t n )
  {
    return memmove( dest, src, n * sizeof( char ) );
  }

  void *vs_memcpy( wchar_t *dest, const wchar_t *src, size_t n )
  {
    return memcpy( dest, src, n * sizeof( wchar_t ) );
  }

  void *vs_memmove( wchar_t *dest, const wchar_t *src, size_t n )
  {
    return memmove( dest, src, n * sizeof( wchar_t ) );
  }


