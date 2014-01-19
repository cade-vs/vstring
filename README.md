
# NAME

VSTRING is C++ string manipulation and handling library.

# SYNOPSIS

    #include <stdio.h>
    #include "vstrlib.h"

    VString str = "Hello";
    str += " World";    // str is `Hello World' now
    str_reverse( str ); // str is `dlroW olleH' now
    str_low( str );     // lower case

    VArray va = str_split( " +", str ); // array has `dlroW', `olleH'
                                        // " +" is Regexp
    str_reverse( va[0] );
    str_reverse( va[1] );

    str = str_join( va, " " );  // str is back to "Hello World"
    
    VTrie tr = va;
    
    // tr[ "Hello" ] contains "World"

# DESCRIPTION

VSTRING provides dynamic strings and char* compatibility and also 
Perl-like arrays, hashes and regexp objects.

The dynamic string object can be freely exchanged with
standard char* type, so there is no need to change function calls
nor the implementation when you change from char* to String (and
vice versa). The main difference from other similar libs is that
the dynamic string class has no visible methods (except operators)
so you will use it as a plain char* but it will expand/shrink as
needed. 

If you find bug or you have note about vstring lib, please feel
free to contact me at: 

# BASE char* FUNCTIONS REFERENCE

todo...

# VString CLASS REFERENCE

todo...

# VArray CLASS REFERENCE

todo...

# VTrie CLASS REFERENCE

todo...

# VRegexp CLASS REFERENCE

todo...

# FEEDBACK

If you find bug or have comment on library API, code or documentation text,
please, contact me.

# AUTHOR

VSTRING Library

Vladi Belperchinov-Shabanski "Cade" 
<cade@bis.bg> <cade@biscom.net> <cade@datamax.bg> <cade@cpan.org>
http://cade.datamax.bg/away/vstring/

Distributed under the GPL license, see end of this file for full text!

NOTE: vstring is distributed standalone as well as a part from vslib/vfu:
http://cade.datamax.bg/vfu/



