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

#include "vstruti.h"

VString& str_padw( VString& target, int len, char ch )
{
  WString str;
  str = target;
  str_pad( str, len, wchar_t( ch ) );
  target = str;
  return target;
}

char* str_padw( char* target, int len, char ch )
{
  WString str;
  VString vvv;

  str = target;
  str_pad( str, len, wchar_t( ch ) );
  vvv = str;
  strcpy( target, vvv.data() );
  return target;
}


