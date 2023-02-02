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

  #undef PCRE2_CODE_UNIT_WIDTH

  #undef VS_CHAR          
  #undef VS_CHAR_R
  #undef VS_CHAR_L
  #undef VS_SFMT

  #undef VS_STRING_CLASS  
  #undef VS_STRING_CLASS_R
  #undef VS_ARRAY_CLASS   
  #undef VS_TRIE_CLASS    
  #undef VS_REGEXP_CLASS
  #undef VS_CHARSET_CLASS

  #undef VS_STRING_BOX    
  #undef VS_ARRAY_BOX     
  #undef VS_TRIE_BOX      
  #undef VS_TRIE_NODE     

  #undef VS_FN_PRINTF     
  #undef VS_FN_SPRINTF    
  #undef VS_FN_VSPRINTF   
  #undef VS_FN_STRLEN     
  #undef VS_FN_STRCPY     
  #undef VS_FN_STRCAT     
  #undef VS_FN_STRCMP     
  #undef VS_FN_STRNCMP    
  #undef VS_FN_STRCHR     
  #undef VS_FN_STRRCHR
  #undef VS_FN_STRSTR     
  #undef VS_FN_STRDUP     
  #undef VS_FN_STRTOL
  #undef VS_FN_STRTOD
  #undef VS_FN_TOUPPER    
  #undef VS_FN_TOLOWER    
  
  #undef VS_FN_CONVERT

  #undef LENOF_VS_CHAR

#ifdef _VSTRING_WIDE_

  #include <wctype.h>
  #include <wchar.h>

  #define PCRE2_CODE_UNIT_WIDTH 32
  
  #define VS_CHAR           wchar_t
  #define VS_CHAR_R         char
  #define VS_CHAR_L(s)      L##s
  #define VS_SFMT           "%ls"

  #define VS_STRING_CLASS   WString
  #define VS_STRING_CLASS_R VString
  #define VS_ARRAY_CLASS    WArray
  #define VS_TRIE_CLASS     WTrie
  #define VS_REGEXP_CLASS   WRegexp
  #define VS_CHARSET_CLASS  WCharSet

  #define VS_STRING_BOX     WStringBox
  #define VS_ARRAY_BOX      WArrayBox
  #define VS_TRIE_BOX       WTrieBox
  #define VS_TRIE_NODE      WTrieNode

  #define VS_FN_PRINTF      wprintf
  #define VS_FN_SPRINTF     swprintf
  #define VS_FN_VSPRINTF    vswprintf
  #define VS_FN_STRLEN      wcslen
  #define VS_FN_STRCPY      wcscpy
  #define VS_FN_STRCAT      wcscat
  #define VS_FN_STRCMP      wcscmp
  #define VS_FN_STRNCMP     wcsncmp
  #define VS_FN_STRCHR      wcschr
  #define VS_FN_STRRCHR     wcsrchr
  #define VS_FN_STRSTR      wcsstr
  #define VS_FN_STRDUP      wcsdup
  #define VS_FN_STRTOL(s)   wcstol(s,NULL,10)
  #define VS_FN_STRTOD(s)   wcstod(s,NULL)
  #define VS_FN_TOUPPER     towupper
  #define VS_FN_TOLOWER     towlower

  #define VS_FN_CONVERT     mbstowcs

#else

  #define PCRE2_CODE_UNIT_WIDTH  8

  #define VS_CHAR           char
  #define VS_CHAR_R         wchar_t
  #define VS_CHAR_L 
  #define VS_SFMT           "%s"

  #define VS_STRING_CLASS   VString
  #define VS_STRING_CLASS_R WString
  #define VS_ARRAY_CLASS    VArray
  #define VS_TRIE_CLASS     VTrie
  #define VS_REGEXP_CLASS   VRegexp
  #define VS_CHARSET_CLASS  VCharSet

  #define VS_STRING_BOX     VStringBox
  #define VS_ARRAY_BOX      VArrayBox
  #define VS_TRIE_BOX       VTrieBox
  #define VS_TRIE_NODE      VTrieNode

  #define VS_FN_PRINTF      printf
  #define VS_FN_SPRINTF     snprintf
  #define VS_FN_VSPRINTF    vsnprintf
  #define VS_FN_STRLEN      strlen
  #define VS_FN_STRCPY      strcpy
  #define VS_FN_STRCAT      strcat
  #define VS_FN_STRCMP      strcmp
  #define VS_FN_STRNCMP     strncmp
  #define VS_FN_STRCHR      strchr
  #define VS_FN_STRRCHR     strrchr
  #define VS_FN_STRSTR      strstr
  #define VS_FN_STRDUP      strdup
  #define VS_FN_STRTOL(s)   strtol(s,NULL,10)
  #define VS_FN_STRTOD(s)   strtod(s,NULL)
  #define VS_FN_TOUPPER     toupper
  #define VS_FN_TOLOWER     tolower

  #define VS_FN_CONVERT     wcstombs

#endif

  #define LENOF_VS_CHAR(n) ((sizeof(n)<1)?(0):(sizeof(n)/sizeof(VS_CHAR)))

/***************************************************************************
**
** EOF
**
****************************************************************************/


