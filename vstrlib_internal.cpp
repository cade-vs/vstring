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

#include "vstrlib_internal.h"

/****************************************************************************
**
** VString aditional functions
**
****************************************************************************/

  #ifdef _VSTRING_WIDE_
  VS_CHAR time2wstr_wchar_t_return[128];
  const VS_CHAR* time2wstr( const time_t tim )
  {
    time_t t = tim;
    mbstowcs( time2wstr_wchar_t_return, ctime( &t ), LENOF_VS_CHAR(time2wstr_wchar_t_return) );
    return time2wstr_wchar_t_return;
  }
  #else
  const VS_CHAR* time2str( const time_t tim )
  {
    time_t t = tim;
    return ctime( &t );
  }
  #endif

  time_t str2time( const VS_CHAR* timstr )
  {
    if ( str_len( timstr ) < 24) return 0;
    VS_CHAR ts[32];
    struct tm m; 
    memset( &m, 0, sizeof(m) );

    VS_FN_STRCPY( ts, timstr );
    str_up( ts );
    //  0    5   10    5   20   4
    // "Wed Jun 30 21:49:08 1993\n"
    ts[24] = 0; m.tm_year = VS_FN_STRTOL( ts + 20 ) - 1900;
    ts[19] = 0; m.tm_sec  = VS_FN_STRTOL( ts + 17 );
    ts[16] = 0; m.tm_min  = VS_FN_STRTOL( ts + 14 );
    ts[13] = 0; m.tm_hour = VS_FN_STRTOL( ts + 11 );
    ts[10] = 0; m.tm_mday = VS_FN_STRTOL( ts +  8 );
    ts[ 7] = 0; m.tm_mon  = str_find( VS_CHAR_L("JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC"), ts+4 ) / 3;
    m.tm_yday = 0;
    m.tm_wday = 0;
    m.tm_isdst = -1;
    time_t tim = mktime( &m );
    return tim;
  }

/*****************************************************************************
**
** sfn_match function provides simplified pattern matching for the common
** 
*****************************************************************************/

int __sfn_eq( const VS_CHAR c1, const VS_CHAR c2, int flags )
{
  int cc1 = flags & SFN_CASEFOLD ? VS_FN_TOUPPER( c1 ) : c1;
  int cc2 = flags & SFN_CASEFOLD ? VS_FN_TOUPPER( c2 ) : c2;
  return cc1 == cc2;
}

int __sfn_match_charset( const VS_CHAR* charset, const VS_CHAR c, int flags, int *advance = NULL )
{
  VS_STRING_CLASS charset_str;
  const VS_CHAR* cs = charset;
  int r = 0;
  if( *cs == VS_CHAR_L('!') || *cs == VS_CHAR_L('^') )
    {
    r = 1;
    cs++;
    }
  while( *cs && ( *cs != VS_CHAR_L(']') ) )
    {
    if( ! ( flags & SFN_NOESCAPE ) && *cs == VS_CHAR_L('\\') )
      {
      if( ! *++cs ) return 4;
      str_add_ch( charset_str, *cs );
      }
    if( cs[1] == VS_CHAR_L('-') )
      {
      if( ! cs[2] ) return 3;
      str_add_ch_range( charset_str, cs[0], cs[2] );
      cs += 2;
      }
    else
      {
      str_add_ch( charset_str, cs[0] );
      }  
    cs++;  
    }
  if( advance ) *advance = cs - charset;
  if( str_len( charset_str ) == 0 ) return 2;

  VS_CHAR cc = c;
  if( flags & SFN_CASEFOLD )
    {
    str_up( charset_str );
    cc = VS_FN_TOUPPER( cc );
    }
  if( str_find( charset_str, cc ) >= 0 ) return r ? 1 : 0;
  return 1;
}

int sfn_match( const VS_CHAR* pattern, const VS_CHAR* string, int flags )
{
  const VS_CHAR* ps = pattern;
  const VS_CHAR* ss = string;

  if( ! *ps ) return 1; // empty pattern, will not match
  if( ! *ss ) return 1; // empty string,  will not match
  
  while(4)
    {
    if( ! ( flags & SFN_NOESCAPE ) && *ps == VS_CHAR_L('\\') )
      {
      if( ! *++ps ) return 6;
      if( *ps != *ss ) return 7;
      }
    else if( *ps == VS_CHAR_L('?') )
      {
      if( ! *ss ) return 2;
      }
    else if( *ps == VS_CHAR_L('*') )
      {
      while( *ps == VS_CHAR_L('*') ) ps++;
      if( ! *ps ) return 0; // pattern ends with *, will match anything

      while( *ss )
        {
        if( sfn_match( ps, ss++, flags ) ) continue;
        return 0;
        }
      return 11;
      }
    else if( *ps == VS_CHAR_L('[') )
      {
      ps++;
      int a = 0;
      int r = __sfn_match_charset( ps, *ss, flags, &a );
      ps += a; // advance data position
      if( r != 0 ) return 4;
      }
    else
      {
      if( ! __sfn_eq( *ps, *ss, flags ) ) return 5;
      }  
    
    if( ! *ps && ! *ss ) return 0; // end of pattern and string reached, matched so far, return ok
    
    ps++;
    ss++;
    }
  
  return 0;
}

/*****************************************************************************
**
** Knuth-Morris-Pratt search
**
*****************************************************************************/

void __kmp_preprocess( const VS_CHAR* p, int ps, int* next )
{
 int i = 0;
 int j = next[0] = -1;
 while (i < ps)
   {
   while ((j > -1) && (p[i] != p[j])) j=next[j];
   i++;
   j++;
   next[i] = p[i] == p[j] ? next[j] : j;
   }
}

#define MAX_KMP_PATTERN_SIZE 1024

int mem_kmp_search( const VS_CHAR *p, int ps, const VS_CHAR *d, int ds )
{
   int i;
   int j;
   int next[MAX_KMP_PATTERN_SIZE];
   if ( ps > MAX_KMP_PATTERN_SIZE ) return -1;

   __kmp_preprocess( p, ps, next );

   i = j = 0;
   while (j < ds)
     {
     while (i > -1 && p[i] != d[j]) i = next[i];
     i++;
     j++;
     if (i >= ps) return j - i;
     }
   return -1;
}

/*****************************************************************************
**
** Quick Search (simplified Boyer-Moore)
**
** Note: currently only 256-symbol alphabet supported (ASCII)
**
** The difference from Boyer-Moore is that good-suffix shift is not used.
** Actually this is quick search with Horspool hint which is that rightmost
** char is examined first.
**
*****************************************************************************/

#define QS_ASIZE 256

void __qs_preprocess( const VS_CHAR* p, int ps, int* badc )
{
   int i;
   for (i = 0; i < QS_ASIZE; i++) badc[i] = ps + 1;
   for (i = 0; i < ps; i++) badc[(unsigned)(VS_CHAR)p[i]] = ps - i;
}

int mem_quick_search( const VS_CHAR *p, int ps, const VS_CHAR *d, int ds )
{
   int  badc[QS_ASIZE];
   __qs_preprocess( p, ps, badc);

   int j = 0;
   while (j <= ds - ps)
   {
      int i;
      for ( i = ps - 1; i >= 0 && p[i] == d[i + j]; --i );
      if ( i < 0 ) return j;
      j += badc[(unsigned)(VS_CHAR)d[j + ps]];
   }
   return -1;
}

/* no-case version */

void __qs_preprocess_nc( const VS_CHAR* p, int ps, int* badc )
{
   int i;
   for (i = 0; i < QS_ASIZE; i++) badc[i] = ps + 1;
   for (i = 0; i < ps; i++) badc[toupper((VS_CHAR)p[i])] = ps - i;
}

int mem_quick_search_nc( const VS_CHAR *p, int ps, const VS_CHAR *d, int ds )
{
   int  badc[QS_ASIZE];
   __qs_preprocess_nc( p, ps, badc);

   int j = 0;
   while (j <= ds - ps)
   {
      int i;
      for ( i = ps - 1; i >= 0 && toupper(p[i]) == toupper(d[i + j]); --i );
      if ( i < 0 ) return j;
      j += badc[toupper((VS_CHAR)d[j + ps])];
   }
   return -1;
}

/*****************************************************************************
**
** Sum search
**
** It is variation of Karp-Rabin idea of searching `similar' pattern and 
** if found check the actual one. The hash function here is simple sum of 
** bytes in the range of pattern size.
**
** Not much useful since Quick search performs better in almost all cases.
** Written for benchmarking purposes.
**
*****************************************************************************/

int mem_sum_search( const VS_CHAR *p, int ps, const VS_CHAR *d, int ds )
{
   int psum = 0;

   int i;
   for( i = 0; i < ps; i++ ) psum += p[i];

   int j = 0;
   int sum = 0;
   while( j <= ds - ps )
     {
     if ( sum == psum && memcmp( p, d + j, ps ) == 0 ) return j;
     sum -= d[j];
     j++;
     sum += d[j+ps];
     }
   return -1;
}

/*****************************************************************************
**
** mem_*_search benchmarks:
**
** (NOTE: These results are circa 2000
**
** For simple benchmark I used ~700MB file and tried to find 10- and 70-chars
** patterns. Results in seconds for both cases (similar to 1-2 seconds) were:
**
** Quick  26
** KMP    42
** Sum    39
**
** Even though KMP returns right result I prefer Quick search as default :))
**
** Case insensitive search is 2 times slower due to the simple implementation.
**
*****************************************************************************/

/*****************************************************************************
**
** file search frontend
**
*****************************************************************************/

long file_pattern_search( const VS_CHAR *p, int ps, FILE* f, const VS_CHAR* opt,
                          int (*mem_search)( const VS_CHAR *p, int ps,
                                             const VS_CHAR *d, int ds ) )
{
   #define BUFSIZE  (1024*1024)
   VS_CHAR* buff = new VS_CHAR[BUFSIZE];

   int nocase = str_find( opt, 'i' ) > -1;
   VS_CHAR* np = new VS_CHAR[ps+1];
   ASSERT(np);
   memcpy( np, p, ps );
   np[ps] = 0;

   if ( ! mem_search )
     mem_search = mem_quick_search;
   if ( nocase )
     mem_search = mem_quick_search_nc;

   off_t pos = -1;
   while(4)
     {
     int bs = fread( buff, 1, BUFSIZE, f );
     int cpos = mem_search( np, ps, buff, bs );
     if ( cpos > -1 )
       {
       pos = ftello(f) - bs + cpos;
       break;
       }
     else
       {
       fseeko( f, -ps, SEEK_CUR );
       }
     if ( bs < BUFSIZE ) break;
     }
   delete [] np;
   delete [] buff;
   return pos;
}

long file_pattern_search( const VS_CHAR *p, int ps, const char* fn, const VS_CHAR* opt,
                          int (*mem_search)( const VS_CHAR *p, int ps,
                                             const VS_CHAR *d, int ds ) )
{
  FILE *f = fopen( fn, "r" );
  if ( ! f ) return -1;
  int res = file_pattern_search( p, ps, f, opt, mem_search );
  fclose( f );
  return res;
}

/*****************************************************************************
**
** Grep -- regular expression search
**
*****************************************************************************/

/* FGrep -- regular expression search (I know `G' here stands for <nothing> :)) */

long file_grep( const VS_CHAR *re_string, const char* file_name, int nocase, off_t spos )
{
  FILE *f = fopen( file_name, "rb" );
  if (!f) return -1;
  long pos = file_grep( re_string, f, nocase, spos );
  fclose(f);
  return pos;
}

/*
int file_grep_max_line   = MAX_GREP_LINE;
int file_grep_lines_read = 0;
*/
long file_grep( const VS_CHAR *re_string __attribute__((unused)), FILE* f __attribute__((unused)), int nocase __attribute__((unused)), off_t spos  __attribute__((unused)))
{
  /*
  if ( str_len(re_string) >= (size_t)file_grep_max_line ) return -2; // just in case, and for now...

  VS_CHAR newpat[MAX_PATTERN+1];
  VS_FN_STRCPY( newpat, re_string );
  if ( nocase ) str_up( newpat );

  VS_REGEXP_CLASS re;
  if ( ! re.comp( newpat ) ) return -2;
  VS_CHAR *line = (VS_CHAR*)malloc( ( file_grep_max_line+1 ) * sizeof( VS_CHAR ) ); //TODO: FIXME: malloc func

  off_t opos = ftello( f );
  ASSERT( spos >= -1 );
  if (spos != -1) fseeko( f, spos, SEEK_SET );
  off_t cpos = ftello( f );

  file_grep_lines_read = 0;
  int found = 0;
  while( fgets( line, file_grep_max_line, f ) )
    {
    if ( nocase ) str_up( line );
    if ( re.m( line ) )
      {
      found = 1;
      break;
      }
    cpos = ftello( f );
    file_grep_lines_read++;
    if (feof(f)) break;
    }

  fseeko( f, opos, SEEK_SET );
  if (found)
    cpos += ( re.sub_sp( 0 ) );

  free(line);
  file_grep_max_line = MAX_GREP_LINE;
  return found ? cpos : -1;
  */
  return -111;
}

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

long file_string_search( const VS_CHAR *p, const char* fn, const VS_CHAR* opt )
{
  FILE *f = fopen( fn, "rb" );
  if (!f) return -1;
  long pos = file_string_search( p, f, opt );
  fclose(f);
  return pos;
}

long file_string_search( const VS_CHAR *p, FILE *f, const VS_CHAR* opt )
{
  int ps = str_len(p);
  ASSERT( ps < MAX_PATTERN );

  int nocase = str_find( opt, VS_CHAR_L('i') ) > -1;

  long pos = -1;

  if( str_find( opt, VS_CHAR_L('r') ) > -1 )
    {
    pos = file_grep( p, f, 0, -1 );
    } else
  if( str_find( opt, VS_CHAR_L('h') ) > -1 )
    {
    VS_CHAR new_p[MAX_PATTERN+1];
    int pl = hex_string_to_pattern( p, new_p );
    if (pl > 0)
      pos = file_pattern_search( new_p, pl, f, nocase ? VS_CHAR_L("i") : VS_CHAR_L("") );
    }
  else
    {
    pos = file_pattern_search( p, str_len(p), f, nocase ? VS_CHAR_L("i") : VS_CHAR_L("") );
    }

  return pos;
}

int mem_string_search( const VS_CHAR *p, const VS_CHAR* d, const VS_CHAR* opt )
{
  int ps = str_len(p);
  ASSERT( ps < MAX_PATTERN );

  int nocase = str_find( opt, VS_CHAR_L('i') ) > -1;

  long pos = -1;

  if( str_find( opt, VS_CHAR_L('r') ) > -1 )
    {
    VS_REGEXP_CLASS re;
    if ( ! re.comp( p ) ) return -1;
    if ( ! re.m( d ) ) return -1;
    pos = re.sub_sp( 0 );
    } else
  if( str_find( opt, VS_CHAR_L('h') ) > -1 )
    {
    VS_CHAR new_p[MAX_PATTERN+1];
    int pl = hex_string_to_pattern( p, new_p );
    if (pl > 0)
      {
      if ( nocase )
        pos = mem_quick_search_nc( new_p, pl, d, str_len(d) );
      else
        pos = mem_quick_search( new_p, pl, d, str_len(d) );
      }
    }
  else
    {
    if ( nocase )
      pos = mem_quick_search_nc( p, ps, d, str_len(d) );
    else
      pos = mem_quick_search( p, ps, d, str_len(d) );
    }

  return pos;
}

/***************************************************************************
**
** VCHARSET
**
****************************************************************************/

  VS_CHARSET_CLASS::VS_CHARSET_CLASS()
    {
    _data = NULL;
    _size = 0;
    }

  VS_CHARSET_CLASS::~VS_CHARSET_CLASS()
    {
    _data = NULL;
    _size = 0;
    }

  void VS_CHARSET_CLASS::resize( int new_size )
  {
    if ( new_size < 1 )
      {
      if ( _data ) delete [] _data;
      _data = NULL;
      _size = 0;
      return;
      }
    // calc required mem size in chars (bytes?)
    new_size = new_size / sizeof(VS_CHAR) + (new_size % sizeof(VS_CHAR) != 0);
    // pad mem size in blocks of VCHARSET_BLOCK_SIZE
    new_size = new_size / VCHARSET_BLOCK_SIZE + (new_size % VCHARSET_BLOCK_SIZE != 0);
    // calc size back to chars (bytes?)
    new_size *= VCHARSET_BLOCK_SIZE;
    VS_CHAR *new_data = new VS_CHAR[ new_size ];
    memset( new_data, 0, new_size );
    if ( _data )
      {
      memcpy( new_data, _data, _size < new_size ? _size : new_size );
      delete [] _data;
      }
    _data = new_data;
    _size = new_size;
  }


  void VS_CHARSET_CLASS::push( VS_CHAR n, int val )
  {
    if ( n < 0 ) return;
    if ( n >= _size * (int)sizeof(VS_CHAR) ) resize( n + 1 );
    if ( val )
      _data[ n / sizeof(VS_CHAR) ] |= 1 << (n % sizeof(VS_CHAR));
    else
      _data[ n / sizeof(VS_CHAR) ] &= ~(1 << (n % sizeof(VS_CHAR)));
  }

  void VS_CHARSET_CLASS::undef( VS_CHAR n )
  {
    push( n, 0 );
  }

  void VS_CHARSET_CLASS::undef()
  {
    resize( 0 );
  }

  int VS_CHARSET_CLASS::in( VS_CHAR n )
  {
    if ( n < 0 || n >= _size * (int)sizeof(VS_CHAR) ) return 0;
    return ( _data[ n / sizeof(VS_CHAR) ] & ( 1 << ( n % sizeof(VS_CHAR) ) ) ) != 0;
  }


/*
  int VCharSet::get( int pn )
    {
      if ( pn < 0 || pn >= size ) return 0;
      return (data[pn / 8] & (1 << (pn % 8))) != 0;
    }

  void VCharSet::set_range1( int start, int end ) // set range
  {
    char s = ( start < end ) ? start : end;
    char e = ( start > end ) ? start : end;
    for( int z = s; z <= e; z++) set1( z );
  }

  void VCharSet::set_range0( int start, int end ) // set range
  {
    char s = ( start < end ) ? start : end;
    char e = ( start > end ) ? start : end;
    for( int z = s; z <= e; z++) set0( z );
  }

  void VCharSet::set_str1( const char* str )
  {
    int sl = str_len( str );
    for( int z = 0; z < sl; z++ )
      set1( str[z] );
  }

  void VCharSet::set_str0( const char* str )
  {
    int sl = str_len( str );
    for( int z = 0; z < sl; z++ )
      set0( str[z] );
  }

  int VCharSet::in( const char *str )
  {
    int sl = str_len( str );
    for( int z = 0; z < sl; z++ )
      if ( !in( str[z] ) ) return 0;
    return 1;
  }

  int VCharSet::resize( int p_size )
    {
      ASSERT( p_size > 0 );
      int new_size = p_size;
      int new_datasize = p_size / 8 + (p_size % 8 != 0);
      char *new_data = (char*)malloc( new_datasize );
      if (new_data == NULL) return CE_MEM;
      memset( new_data, 0, new_datasize );
      if (data)
        {
        memcpy( new_data, data, datasize < new_datasize ? datasize : new_datasize );
        free( data );
        data = NULL;
        }
      data = new_data;
      size = new_size;
      datasize = new_datasize;
      return CE_OK;
    }

  VCharSet& VCharSet::operator  = ( const VCharSet &b1 )
  {
    resize( b1.size );
    memcpy( data, b1.data, datasize );
    return *this;
  }

  VCharSet& VCharSet::operator &= ( const VCharSet &b1 )
  {
    int z;
    for(z = 0; z < (datasize < b1.datasize ? datasize : b1.datasize ); z++)
      data[z] &= b1.data[z];
    return *this;
  }

  VCharSet& VCharSet::operator |= ( const VCharSet &b1 )
  {
    int z;
    for(z = 0; z < (datasize < b1.datasize ? datasize : b1.datasize ); z++)
      data[z] |= b1.data[z];
    return *this;
  }

  VCharSet VCharSet::operator ~ ()
  {
    VCharSet b;
    b = *this;
    int z;
    for(z = 0; z < b.datasize; z++)
      b.data[z] = ~b.data[z];
    return b;
  }

  VCharSet operator & ( const VCharSet &b1, const VCharSet &b2 )
  {
    VCharSet b;
    b = b1;
    b &= b2;
    return b;
  }

  VCharSet operator | ( const VCharSet &b1, const VCharSet &b2 )
  {
    VCharSet b;
    b = b1;
    b |= b2;
    return b;
  }
*/

/***************************************************************************
**
** VREGEXP
**
****************************************************************************/

  VS_REGEXP_CLASS::VS_REGEXP_CLASS()
  {
    re = NULL;
    md = NULL;
    rc = 0;
    lp = NULL;

    pt = NULL;
    pl = 0;
  }

  VS_REGEXP_CLASS::VS_REGEXP_CLASS( const VS_CHAR* rs, const VS_CHAR* opt )
  {
    re = NULL;  
    md = NULL;
    rc = 0;     
    lp = NULL;  

    pt = NULL;  
    pl = 0;     

    opt_mode = MODE_REGEXP;
    comp( rs, opt );
  }

  VS_REGEXP_CLASS::~VS_REGEXP_CLASS()
  {
    if ( re ) pcre2_code_free( re );
    if ( md ) pcre2_match_data_free( md );
    if ( pt ) delete pt;
  }

  int VS_REGEXP_CLASS::get_options( const VS_CHAR* opt )
  {
    opt_mode = MODE_REGEXP;
    opt_nocase = 0;
    if ( ! opt    ) return 0;
    if ( ! opt[0] ) return 0;
    int options = 0;
    int sl = str_len( opt );
    int z;
    for( z = 0; z < sl; z++ )
      {
      switch( opt[z] )
        {
        case VS_CHAR_L('i'): options |= PCRE2_CASELESS; opt_nocase = 1; break;
        case VS_CHAR_L('m'): options |= PCRE2_MULTILINE; break;
        case VS_CHAR_L('s'): options |= PCRE2_DOTALL; break;
        case VS_CHAR_L('x'): options |= PCRE2_EXTENDED; break;
        case VS_CHAR_L('f'): opt_mode = MODE_FIND; break;
        case VS_CHAR_L('h'): opt_mode = MODE_HEX; break;
        case VS_CHAR_L('r'): opt_mode = MODE_REGEXP; break;
        default: errstr = VS_CHAR_L("invalid option, expected are: imsxfhr"); return -1;
        }
      }
    return options;
  }

  int VS_REGEXP_CLASS::comp( const VS_CHAR* pattern, const VS_CHAR *opt )
  {
    if ( re ) pcre2_code_free( re );
    if ( pt ) delete [] pt;
    re = NULL;
    pt = NULL;
    pl = 0;

    int options = get_options( opt );
    if( options == -1 ) return 0;

    if ( opt_mode == MODE_REGEXP )
      {
      int errorcode;
      PCRE2_SIZE erroffset;
      re = pcre2_compile( (PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, options, &errorcode, &erroffset, NULL );

      if ( re )
        {
        errstr = VS_CHAR_L("OK");
        return 1;
        }
      else
        {
        VS_CHAR errbuf[256];
        pcre2_get_error_message( errorcode, (PCRE2_UCHAR*)errbuf, sizeof(errbuf)/2 );
        errstr  = errbuf;
        errstr += VS_CHAR_L(", at pos ");
        errstr += (int)erroffset;
        return 0;
        }
      }
    else
      {
      pl = str_len( pattern );
      pt = new VS_CHAR[pl+1];
      if ( opt_mode == MODE_HEX )
        pl = hex_string_to_pattern( pattern, pt );
      else
        VS_FN_STRCPY( pt, pattern );
      pt[pl] = 0;
      return pl;
      }
  }

  int VS_REGEXP_CLASS::study()
  {
    return 1; // not implemented
  }

  int VS_REGEXP_CLASS::ok()
  {
    if ( opt_mode == MODE_REGEXP )
      return re != NULL;
    else
      return pt != NULL && pl > 0;
  }

  int VS_REGEXP_CLASS::m( const VS_CHAR* line )
  {
    if ( ! ok() )
      {
      errstr = VS_CHAR_L("no pattern compiled");
      return 0;
      }
    if ( ! line )
      {
      errstr = VS_CHAR_L("no data to search into");
      return 0;
      }
    errstr = VS_CHAR_L("");
    lp = line;
    if ( opt_mode == MODE_REGEXP )
      {
      if( md ) pcre2_match_data_free( md );
      md = pcre2_match_data_create_from_pattern( re, NULL );

      unsigned int options = 0;
      rc = pcre2_match( re, (PCRE2_SPTR)lp, str_len(lp), 0, options, md, NULL);
      if ( rc < 1 ) rc = 0;
      return rc;
      }
    else
      {
      if ( opt_nocase )
        pos = mem_quick_search_nc( pt, pl, line, str_len(lp) );
      else
        pos = mem_quick_search( pt, pl, line, str_len(lp) );
      return pos >= 0;
      }
  }

  int VS_REGEXP_CLASS::m(  const VS_CHAR* line, const VS_CHAR* pattern, const VS_CHAR *opt )
  {
    comp( pattern, opt );
    return m( line );
  }

  VS_STRING_CLASS VS_REGEXP_CLASS::sub( int n )
  {
    VS_STRING_CLASS substr;
    if ( ! ok() ) return substr;
    if ( ! lp   ) return substr;
    if ( opt_mode == MODE_REGEXP )
      {
      if ( n < 0 || n >= rc ) return substr;
      PCRE2_SIZE *ovector = pcre2_get_ovector_pointer( md );

      size_t s = ovector[n*2];
      size_t e = ovector[n*2+1];
      
      if ( s == PCRE2_UNSET || e == PCRE2_UNSET ) return substr;
      
      size_t l = e - s;
      substr.setn( lp + s, l );
      }
    else
      {
      if ( n != 0 ) return substr;
      substr.setn( lp + pos, pl );
      }
    return substr;
  }

  int VS_REGEXP_CLASS::sub_sp( int n )
  {
    if ( opt_mode == MODE_REGEXP )
      {
      if ( n < 0 || n >= rc ) return -1;
      PCRE2_SIZE *ovector = pcre2_get_ovector_pointer( md );
      return ovector[n*2] == PCRE2_UNSET ? -1 : ovector[n*2];
      }
    else
      {
      if ( n != 0 ) return -1;
      return pos;
      }
  }

  int VS_REGEXP_CLASS::sub_ep( int n )
  {
    if ( opt_mode == MODE_REGEXP )
      {
      if ( n < 0 || n >= rc ) return -1;
      PCRE2_SIZE *ovector = pcre2_get_ovector_pointer( md );
      return ovector[n*2+1] == PCRE2_UNSET ? -1 : ovector[n*2+1];
      }
    else
      {
      if ( n != 0 ) return -1;
      return pos+pl;
      }
  }

/***************************************************************************
**
** UTILITIES
**
****************************************************************************/

  // split `source' with `regexp_str' regexp
  VS_ARRAY_CLASS str_split( const VS_CHAR* regexp_str, const VS_CHAR* source, int maxcount )
  {
    VS_ARRAY_CLASS arr;
    VS_REGEXP_CLASS re;
    int z = re.comp( regexp_str );
    ASSERT( z );
    if ( ! z ) return arr;

    const VS_CHAR* ps = source;

    while( ps && ps[0] && re.m( ps ) )
      {
      if ( maxcount != -1 )
        {
        maxcount--;
        if ( maxcount == 0 ) break;
        }
      VS_STRING_CLASS s;
      s.setn( ps, re.sub_sp( 0 ) );
      arr.push( s );
      ps += re.sub_ep( 0 );
      }
    if ( ps && ps[0] )
      arr.push( ps );
    return arr;
  }

  // split `source' with exact string `delimiter_str'
  VS_ARRAY_CLASS str_split_simple( const VS_CHAR* delimiter_str, const VS_CHAR* source, int maxcount )
  {
    VS_ARRAY_CLASS arr;
    const VS_CHAR* ps = source;
    const VS_CHAR* fs;

    int rl = str_len( delimiter_str );

    VS_STRING_CLASS s;
    while( (fs = VS_FN_STRSTR( ps, delimiter_str )) )
      {
      if ( maxcount != -1 )
        {
        maxcount--;
        if ( maxcount == 0 ) break;
        }
      int l = fs - ps;
      s.setn( ps, l );
      arr.push( s );
      ps = (const VS_CHAR *)(ps + l + rl);
      }
    if ( ps && ps[0] )
      arr.push( ps );
    return arr;
  }

  // join array data to single string with `glue' string
  // returns the result string or store to optional `dest'
  VS_STRING_CLASS str_join( VS_ARRAY_CLASS array, const VS_CHAR* glue )
  {
    VS_STRING_CLASS str;
    for( int z = 0; z < array.count()-1; z++ )
      {
      str += array.get( z );
      str += glue;
      }
    str += array.get( array.count()-1 );
    return str;
  }

/*****************************************************************************
**
** find/rfind versions for regexp separators
**
*****************************************************************************/

  int str_find_regexp( const VS_CHAR* target, const VS_CHAR* pattern, int startpos )
  {
    VS_REGEXP_CLASS re;
    if ( ! re.comp( pattern ) ) return -1;
    if ( startpos < 0 ) return -1;
    int z = 0;
    while( startpos-- )
      {
      if ( target[z] == 0 ) return -1;
      z++;
      }
    if ( re.m( target + z ) )
      return z + re.sub_sp( 0 );
    else
      return -1;
  }

  int str_rfind_regexp( const VS_CHAR* target, const VS_CHAR* pattern )
  {
    VS_REGEXP_CLASS re;
    if ( ! re.comp( pattern ) ) return -1;
    int z = str_len( target );
    while(4)
      {
      z--;
      if ( re.m( target + z ) )
        return z + re.sub_sp( 0 );
      if ( z == 0 ) break;
      }
    return -1;
  }

/*****************************************************************************
**
** Hex string to pattern conversion
**
** Converts hex-string to binary pattern (data)
** example: `56 6C 61 64 69' -> ...
** returns pattern length
**
*****************************************************************************/

  int __hex_code( VS_CHAR ch )
  {
    ch = VS_FN_TOUPPER( ch );
    if( ch >= '0' && ch <= '9' ) return ch - '0';
    if( ch >= 'A' && ch <= 'F' ) return ch - 'A' + 10;
    return -1;
  }

  int hex_string_to_pattern( const VS_CHAR *str, VS_CHAR* pattern )
  {
     const VS_CHAR *pc = pattern;
     while( *str )
     {
       while( *str == ' ' || *str == '\t' ) str++;
       int hex  = __hex_code( *str++ );
       if( hex == -1 )  return 0;
       int hex2 = __hex_code( *str++ );
       if( hex2 == -1 )  return 0;
       hex <<= 4;
       hex += hex2;
       *pattern = hex;
       pattern ++;
     }
     return pattern - pc;
  }


/***************************************************************************
**
** EOF
**
****************************************************************************/
