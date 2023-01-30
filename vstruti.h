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
 *  standard VS_CHAR* type, so there is no need to change function calls
 *  nor the implementation when you change from VS_CHAR* to VS_STRING_CLASS (and
 *  vice versa). The main difference from other similar libs is that
 *  the dynamic VS_STRING_CLASS class has no visible methods (except operators)
 *  so you will use it as a plain VS_CHAR* but it will expand/shrink as
 *  needed.
 *
 *  If you find bug or you have note about vstring lib, please feel
 *  free to contact me.
 *
 *  VSTRING part (vstring.h and vstring.cpp) implements plain string-only
 *  manipulations:
 *
 *  VS_CHAR* functions to manipulate in-memory string buffers
 *  VS_STRING_CLASS -- dynamic string, which resizes automatically
 *
 *  VSTRLIB part (vstrlib.h and vstrlib.cpp) provides string data 
 *  structures which mimic Perl's. There are several classes:
 *
 *  VS_ARRAY_CLASS  -- array of VS_STRING_CLASS elements
 *  VS_TRIE_CLASS   -- associative array (hash) of VS_STRING_CLASS elements
 *  VS_REGEXP_CLASS -- regular expression helper class
 *
 *  VS_STRING_CLASS, VS_ARRAY_CLASS, VS_TRIE_CLASS use shallow copy and copy-on-write functionality,
 *  so things like str1 = str2, varray1 = varray2 etc. are cheap and fast :)
 *
 ***************************************************************************/

#ifndef _VSTRUTI_H_
#define _VSTRUTI_H_

#include "vstring.h"
#include "wstring.h"

VString& str_padw( VString& target, int len, char ch = ' ' );
char* str_padw( char* target, int len, char ch = ' ' );

#endif /* TOP */