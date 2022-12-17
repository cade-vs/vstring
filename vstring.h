/****************************************************************************
 *
 *  VSTRING Library
 *
 *  Copyright (c) 1996-2022 Vladi Belperchinov-Shabanski "Cade" 
 *  http://cade.noxrun.com/  <cade@noxrun.com> <cade@bis.bg> <cade@cpan.org>
 *
 *  Distributed under the GPL license, you should receive copy of GPLv2!
 *
 *  SEE 'README','LICENSE' OR 'COPYING' FILE FOR LICENSE DETAILS!
 *
 *  VSTRING library provides wide set of string manipulation features
 *  including dynamic string object that can be freely exchanged with
 *  standard char* type, so there is no need to change function calls
 *  nor the implementation when you change from char* to VString (and
 *  vice versa). The main difference from other similar libs is that
 *  the dynamic VString class has no visible methods (except operators)
 *  so you will use it as a plain char* but it will expand/shrink as
 *  needed.
 *
 *  If you find bug or you have note about vstring lib, please feel
 *  free to contact me.
 *
 *  VSTRING part (vstring.h and vstring.cpp) implements plain string-only
 *  manipulations:
 *
 *  char* functions to manipulate in-memory string buffers
 *  VString -- dynamic string, which resizes automatically
 *
 *  VSTRLIB part (vstrlib.h and vstrlib.cpp) provides string data 
 *  structures which mimic Perl's. There are several classes:
 *
 *  VArray  -- array of VString elements
 *  VTrie   -- associative array (hash) of VString elements
 *  VRegexp -- regular expression helper class
 *
 *  VString, VArray, VTrie use shallow copy and copy-on-write functionality,
 *  so things like str1 = str2, varray1 = varray2 etc. are cheap and fast :)
 *
 ***************************************************************************/

#ifndef _VSTRING_H_
#define _VSTRING_H_

#undef _VSTRING_WIDE_
#include "vdef.h"
#include "vref.h"
#include "vstring_internal.h"

#endif /* TOP */

/***************************************************************************
**
** EOF
**
****************************************************************************/
