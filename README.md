
# NAME

VSTRING is C++ string manipulation and handling library with Unicode support.

# SYNOPSIS

    #include "vstring.h"
    #include "vstrlib.h"

    VString str = "Hello";
    str += " World";    // str is 'Hello World' now
    str_reverse( str ); // str is 'dlroW olleH' now
    str_low( str );     // lower case

    VArray va = str_split( " +", str ); // array has 'dlroW', 'olleH'
                                        // " +" is Regexp
    str_reverse( va[0] );
    str_reverse( va[1] );

    str = str_join( va, " " );  // str is back to "Hello World"
    
    VTrie tr = va;
    
    // tr[ "Hello" ] contains "World"

# DESCRIPTION

VSTRING provides dynamic strings and char* compatibility and also 
Perl-like arrays, hashes and regexp objects. It also offers wide
char (wchar_t) support for ISO10646/Unicode strings.


The dynamic string object can be freely exchanged with
standard char* (or wchar_t*) type, so there is no need to change 
function calls nor the implementation when you change from char* 
to String (and vice versa). The main difference from other similar 
libs is that the dynamic string class has no visible methods (except 
operators) so you will use it as a plain char* but it will expand/shrink 
as needed. 

All classes (VString, VArray, Vtrie and their counterparts WString, WArray, 
WTrie) implementation provide shallow copy and copy-on-write functionality 
so assignment operators like:

    str1  = str2
    arr1  = arr2
    trie1 = tri2
  
are cheap and fast!

# REFERENCE

vstring.h and vstrlib.h files can be used as reference. This file contains
brief introduction and some notes but for further API documentation check
the .h files.

# BASE char*/wchar_t* AND VString/WString FUNCTIONS NOTES

All functions for char*/wchar_t* handling may overflow! If you need safe 
strings, use the same functions but with VString instead of char* 
(and WString instead of wchar_t*).

Functions common for char* and VString:

    str_set( str, "hello" );
    str_mul( str, 4 );
    str_replace( str, "o", " " );
    str_left( dest_str, str, 4 );
    str_up( dest_str );

In the examples above, str, dest_str and source_str may be either char* 
or VString.

# VString CLASS NOTES

    VString str = "hello";
    str += " world";
    if( str == "hello world") { ... }
    int len = str_len( str );
    str[3] = 'z'; // safe! even outside string boundaries

# VArray CLASS NOTES

    VArray va;
    
    // append array elements
    va.push( "element 1" );
    va.push( str ); // i.e. VString
    va.push( other_varray ); 
    va.push( trie ); // see VTrie below

    // take out the last element
    VString str = va.pop()
    
    // push elements at the beginning 
    va.unshift( "element 1" );
    va.unshift( str ); // i.e. VString
    va.unshift( other_varray ); 
    va.unshift( trie ); // see VTrie below
    
    // take out the first element
    VString str = va.shift();

    va.reverse(); // reverse elements order
    va.undef(); // remove all elements

# VTrie CLASS NOTES

    VTrie tr;
    
    tr[ "hello"  ] = "world";
    tr[ "number" ] = "12345";
    
    VArray va = tr; // array is: hello world number 12345
                    // however only key+value order is preserved!

    tr.reverse(); // reverse keys <-> values
                    
    tr.undef(); // remove all keys

# VRegexp CLASS NOTES

    VRegexp re( "a([0-9]+)" ); // compiling new regexp

    if( re.m( "tralala85." ) ) // match against compiled regexp
      res1 = re[1]; // re[1] returns '85'

    if( re.m( "tralala85.", "(la)+" ) ) // match against new regexp pattern
      {
      str_all_matched   = re[0]; // 'lala'
      str_first_capture = re[1]; // 'la'
      }
      
    re.comp( "^[a-z]+[0-9]*" ); // reuse/recompile new regexp in the same obj
    re.study(); // takes extra time to speed multiple matchings with m()

# W-CLASSES AND wchar_t* FUNCTIONS

All WString, WArray, WTrie classes and functions behave the same way as 
V-ones. The only difference is that they hold wider-data (wchar_t) and keys.

Conversions between the two are implicit:

    VString str;
    WString wide;
    
    str  = "проста проба едно";
    wide = str;
    wide = L"две прости проби";
    str  = wide;
    
    int f1 = str_find( str,  "проб" ); // returns  7
    int f2 = str_find( wide, "проб" ); // returns 11

VString always holds byte string, it has no knowledge if the string is UTF8 or
not. It does not have functions to manage characers in UTF8-encoded string.
To do so it is needed that VString be converted to WString, which always works
on Unicode Level 1 characters. 

Conversion from VString to WString is safe, i.e. if VString holds incorrectly
encoded UTF8, all incorrect chars will be replaced by 0xFFFD (unknown char)
symbol and the rest will be converted properly.

Conversion from WString to VString is always correct.

If it is needed to check the result of the conversion from VString to WString,
there is explicit function "set_failsafe()" which returns the count of the
incorrect chars in the string:

    VString str;
    WString wide;
    
    str  = "ще се видим на славейков";
    int err = wide.set_failsafe( str );
    if( err )
      {
      // there were errors, reset string or whatever...
      // wide.undef();
      }

# FEEDBACK

If you find bug or have comment on library API, code or documentation text,
please, contact me.

# AUTHOR

    Vladi Belperchinov-Shabanski "Cade" 

    <cade@noxrun.com> <cade@bis.bg> <cade@cpan.org>

    http://cade.noxrun.bg/projects/vstring/

    https://github.com/cade-vs/vstring

# LICENSE

Distributed under the GPL license, see COPYING file for the full text.

