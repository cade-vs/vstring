/****************************************************************************
 *
 *  VSTRING Library
 *
 *  Copyright (c) 1996-2022 Vladi Belperchinov-Shabanski "Cade" 
 *  http://cade.noxrun.com/  <cade@noxrun.com> <cade@bis.bg> <cade@cpan.org>
 *
 *  Distributed under the GPL license, you should receive copy of GPLv2!
 *
 *  SEE `README',`LICENSE' OR `COPYING' FILE FOR LICENSE AND OTHER DETAILS!
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

#include "vdef.h"
#include <pcre2.h>

/***************************************************************************
**
** GLOBALS
**
****************************************************************************/

#define VCHARSET_BLOCK_SIZE 32

/* max pattern length for file_find_*() and ... */
#define MAX_PATTERN   2048

/* max file_grep() text line input length... :| */
#define MAX_GREP_LINE   4096


/****************************************************************************
**
** VString aditional functions
**
****************************************************************************/

#ifdef _VSTRING_WIDE_
const VS_CHAR*  time2wstr( const time_t tim );
#else
const VS_CHAR*  time2str( const time_t tim );
#endif

time_t str2time( const VS_CHAR* timstr );

/*****************************************************************************
**
** Next mem* search functions are used to find pattern into memory block
** p is pattern, ps is pattern size, d is data searched and ds is its size
** return found pttern position or -1 for not found
**
*****************************************************************************/

int mem_kmp_search  ( const VS_CHAR *p, int ps, const VS_CHAR *d, int ds );
int mem_quick_search( const VS_CHAR *p, int ps, const VS_CHAR *d, int ds );
int mem_sum_search  ( const VS_CHAR *p, int ps, const VS_CHAR *d, int ds );

/* no-case versions */

int mem_quick_search_nc( const VS_CHAR *p, int ps, const VS_CHAR *d, int ds );

/*****************************************************************************
**
** Function which return position of pattern into a file
** this uses mem* functions above or defaults to mem_quick_search
**
*****************************************************************************/

long file_pattern_search( const VS_CHAR *p, int ps, FILE* f, const VS_CHAR* opt = VS_CHAR_L(""),
                          int (*mem_search)( const VS_CHAR *p, int ps,
                                             const VS_CHAR *d, int ds ) = NULL );

long file_pattern_search( const VS_CHAR *p, int ps, const char* fn, const VS_CHAR* opt = VS_CHAR_L(""),
                          int (*mem_search)( const VS_CHAR *p, int ps,
                                             const VS_CHAR *d, int ds ) = NULL );

/*****************************************************************************
**
** This function reads lines from a text file and runs regexp on it.
** file_grep_max_line defines the max line length read (1024)
** file_grep_lines_read reports how many lines are read in during the
**                      last file_grep() call
** re_string is regexp string, not arbitrary (binary) pattern
** spos defines what file start offset should be accepted
**
*****************************************************************************/

extern int file_grep_max_line;
extern int file_grep_lines_read;
long file_grep( const VS_CHAR *re_string, const char* file_name, int nocase, off_t spos = -1 );
long file_grep( const VS_CHAR *re_string, FILE* f,               int nocase, off_t spos = -1 );

/*****************************************************************************
**
** Search interface functions
**
** options are:
**
** i    -- ignore case
** r    -- regular expression (grep)
** h    -- hex pattern search
**
*****************************************************************************/

int  mem_string_search ( const VS_CHAR *p, const VS_CHAR* d, const VS_CHAR* opt );
long file_string_search( const VS_CHAR *p, const char* fn,   const VS_CHAR* opt );
long file_string_search( const VS_CHAR *p, FILE *f,          const VS_CHAR* opt );

/***************************************************************************
**
** VCHARSET
**
****************************************************************************/

class VS_CHARSET_CLASS
  {
    VS_CHAR* _data;
    int      _size;  // size (in bytes)

    void resize( int new_size );

    public:

    VS_CHARSET_CLASS();
    ~VS_CHARSET_CLASS();

    void push( VS_CHAR n, int val = 1 );
    void undef( VS_CHAR n );
    void undef();

    int  in( VS_CHAR n );
/*

    push

    int  get ( int pn );

    void set_range1( int start, int end );
    void set_range0( int start, int end );

    void set_str1( const char* str );
    void set_str0( const char* str );

    int in( const char *str ); // return 1 if all str's chars are in the set
    int in( int pn )
        { if ( pn < 0 || pn >= size ) return 0; else return get( pn ); };

    void reverse() { for(int z = 0; z < datasize; z++) data[z] = ~data[z]; };
    void set( int pn, int val ) { if ( val ) set1( pn ); else set0( pn ); };
    void set_all1() { if ( data ) memset( data, 0xff, datasize ); };
    void set_all0() { if ( data ) memset( data, 0x00, datasize ); };

    const int operator [] ( int pn )
       { ASSERT( pn >= 0 && pn < size  ); return get( pn ); };

    int resize( int p_size );

    VCharSet& operator  = ( const VCharSet &b1 );
    VCharSet& operator &= ( const VCharSet &b1 );
    VCharSet& operator |= ( const VCharSet &b1 );
    VCharSet  operator  ~ ();

    friend VCharSet operator & ( const VCharSet &b1, const VCharSet &b2 );
    friend VCharSet operator | ( const VCharSet &b1, const VCharSet &b2 );
*/
  };

/***************************************************************************
**
** VREGEXP
**
****************************************************************************/
/*
** options are:
**               i -- case insensitive
**               m -- multiline matches
**               s -- single line (`.' matches and NEWLINE's)
**               x -- extended (ifnores whitespace and comments)
**
**               f -- plain find (substring) using quick search
**               h -- hex search, input pattern is converted from hex string
**
** for more docs see perlre(1) and pcre library docs
**
**
** WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING!
** extracting of the captured substring is only possible while subject input
** line which is used as matching target is intact! which means that if you
** change this line between match and substring extraction this will lead to
** segmentation fault!
**
*/

/* number of subpatterns which can be catched by VRegexp::m() */
#ifndef VREGEXP_MAX_SUBS
#define VREGEXP_MAX_SUBS    32
#endif

class VS_REGEXP_CLASS
{
  /* search modes */
  enum SearchMode { MODE_REGEXP = 0, MODE_FIND, MODE_HEX };

  /* common data */
  SearchMode opt_mode;
  int opt_nocase; // 1 if caseless search needed

  /* regexp data */
  pcre2_code       *re; // regexp object, allocated here, for MODE_REGEXP
  pcre2_match_data *md; // match data
  int               rc; // result after successful pcre_exec()
  const VS_CHAR    *lp; // last subject data to search in, external, just keep ptr

  /* no-regexp/hex search pattern */
  VS_CHAR*    pt; // pattern for MODE_FIND and MODE_HEX
  int         pl; // pattern length
  int         pos; // last match found pos

  /* common data */
  //VString substr;
  VS_STRING_CLASS errstr;

  int get_options( const VS_CHAR* opt );

  public:

  VS_REGEXP_CLASS();
  VS_REGEXP_CLASS( const VS_CHAR* pattern, const VS_CHAR *opt = NULL ); // compiles new regexp
  ~VS_REGEXP_CLASS();

  int comp( const VS_CHAR* pattern, const VS_CHAR *opt = NULL ); // compile re, return > 0 for success
    // options are:
    //   i   -- ignore case
    //   m   -- multiline match
    //   s   -- match dot against all chars (\n)
    //   x   -- extended, ignore whitespace
    //   f   -- plain string search (no regexp used)
    //   h   -- hex search, converts string ( `56 6C 61 64 69' ) to search pattern
    //   r   -- regexp match (default, no need to specify)
    // last options found are mandatory: "fhr" options sets regexp match
  int study(); // optimizing regexp for (big-size) multiple matches
  int ok(); // return 1 if regexp is compiled ok, 0 if not

  int m( const VS_CHAR* line ); // execute re against line, return 1 for match
  int m( const VS_CHAR* line, const VS_CHAR* pattern, const VS_CHAR *opt = NULL ); // same as exec, but compiles first

  VS_STRING_CLASS sub( int n ); // return n-th substring match
  int sub_sp( int n ); // return n-th substring start position
  int sub_ep( int n ); // return n-th substring end position

  VS_STRING_CLASS operator []( int n ) // same as sub()
    { return sub( n ); }

  const VS_CHAR* error_str() { return errstr.data(); };
};

/***************************************************************************
**
** UTILITIES
**
****************************************************************************/

// split `source' with `regexp_str' regexp
VS_ARRAY_CLASS str_split( const VS_CHAR* regexp_str, const VS_CHAR* source, int maxcount = -1 );

// split `source' with exact string `delimiter_str'
VS_ARRAY_CLASS str_split_simple( const VS_CHAR* delimiter_str, const VS_CHAR* source, int maxcount = -1 );

// join array data to single string with `glue' string
// returns the result string or store to optional `dest'
VS_STRING_CLASS str_join( VS_ARRAY_CLASS array, const VS_CHAR* glue = VS_CHAR_L("") );

/*****************************************************************************
**
** find/rfind versions for regexp separators
**
*****************************************************************************/

int str_find_regexp( const VS_CHAR* target, const VS_CHAR* pattern, int startpos = 0 );
// warning: str_rfind_regexp() is slow! it can execute pattern matching to `n'
// times where n is the target string length...
int str_rfind_regexp( const VS_CHAR* target, const VS_CHAR* pattern );

/*****************************************************************************
**
** Hex string to pattern conversion
**
** Converts hex-string to binary pattern (data)
** example: `56 6C 61 64 69' -> ...
** returns pattern length
**
*****************************************************************************/

int hex_string_to_pattern( const VS_CHAR *str, VS_CHAR* pattern );

/***************************************************************************
**
** EOF
**
****************************************************************************/
