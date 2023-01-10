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
 *  standard VS_CHAR* type, so there is no need to change function calls
 *  nor the implementation when you change from VS_CHAR* to VString (and
 *  vice versa). The main difference from other similar libs is that
 *  the dynamic VString class has no visible methods (except operators)
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
 *  VString -- dynamic char* string, which resizes automatically
 *  WString -- same but for wide char (wchar_t*)
 *
 *  VSTRLIB part (vstrlib.h and vstrlib.cpp) provides string data 
 *  structures which mimic Perl's. There are several classes:
 *
 *  VArray  -- array of VString elements
 *  WArray  -- array of WString elements
 *  VTrie   -- associative array (hash) of VString elements
 *  WTrie   -- associative array (hash) of WString elements
 *  VRegexp -- regular expression helper class
 *  WRegexp -- regular expression helper class (wide VS_CHAR)
 *
 *  All classes use shallow copy and copy-on-write functionality,
 *  so things like str1 = str2, varray1 = varray2 etc. are cheap and fast :)
 *
 *  usage:
 *
 *  include char* support:
 *            #include <vstring.h>
 *            #include <vstrlib.h>
 *
 *  include wchar_t* support:
 *            #include <wstring.h>
 *            #include <wstrlib.h>
 *
 *  include both char* and wchar_t* support:
 *            #include <wstring.h>
 *            #include <wstrlib.h>
 *            #include <wstring.h>
 *            #include <wstrlib.h>
 *  usage:
 *
 *  compile & link:
 *
 *            g++ -I/path/to/vstring ...
 *            ld  -L/path/to/vstring -lvstring -lwstring
 *
 ***************************************************************************/

#include "vstring_internal.h"

  size_t str_len( const VS_CHAR *s )
  {
    return VS_FN_STRLEN( s );
  }

/****************************************************************************
**
** VSTRING BOX
**
****************************************************************************/

  VS_STRING_BOX* VS_STRING_BOX::clone()
  {
    VS_STRING_BOX* box = new VS_STRING_BOX();
    box->resize_buf( size );
    box->sl = sl;
    box->compact = compact;
    vs_memcpy( box->s, s, size ); //TODO: FIXME: just sl?
    return box;
  }

  void VS_STRING_BOX::resize_buf( int new_size )
  {
    /* FIXME: this breaks a lot of things: ==, strcmp, const VS_CHAR*, ...
    if ( new_size == 0 )
      {
      sl = 0;
      if ( s ) free( s );
      s = NULL;
      size = 0;
      return;
      }
    */
    new_size++; /* for the trailing 0 */
    if ( !compact )
      {
      new_size = new_size / STR_BLOCK_SIZE  + ( new_size % STR_BLOCK_SIZE != 0 );
      new_size *= STR_BLOCK_SIZE;
      }
    if( s == NULL )
      { /* first time alloc */
      s = (VS_CHAR*)malloc( new_size * sizeof( VS_CHAR ) );
      ASSERT( s );
      s[ 0 ] = 0;
      size = new_size;
      sl = 0;
      } else
    if ( size != new_size )
      { /* expand/shrink */
      s = (VS_CHAR*)realloc( s, new_size * sizeof( VS_CHAR ) );
      size = new_size;
      s[ size - 1 ] = 0;
      if ( sl > size - 1 ) sl = size - 1;
      }
  }

/****************************************************************************
**
** VSTRING
**
****************************************************************************/

  VS_STRING_CLASS::VS_STRING_CLASS( VS_STRING_CLASS_R rs  )
  {
    box = new VS_STRING_BOX();
    set( rs.data() );
  }

  void VS_STRING_CLASS::detach()
  {
    if ( box->refs() == 1 ) return;
    VS_STRING_BOX *new_box = box->clone();
    box->unref();
    box = new_box;
  }

  void VS_STRING_CLASS::i( const int n )
  {
    VS_CHAR tmp[64];
    VS_FN_SPRINTF( tmp, LENOF_VS_CHAR(tmp), VS_CHAR_L("%d"), n );
    set( tmp );
  }

  void VS_STRING_CLASS::l( const long n )
  {
    VS_CHAR tmp[64];
    VS_FN_SPRINTF( tmp, LENOF_VS_CHAR(tmp), VS_CHAR_L("%ld"), n );
    set( tmp );
  }

  void VS_STRING_CLASS::f( const double d )
  {
    VS_CHAR tmp[64];
    VS_FN_SPRINTF( tmp, LENOF_VS_CHAR(tmp), VS_CHAR_L("%.10f"), d );
    int z = VS_FN_STRLEN( tmp );
    while( tmp[z-1] == VS_CHAR_L('0') ) z--;
    if ( tmp[z-1] == VS_CHAR_L('.') ) z--;
    tmp[z] = 0;
    set( tmp );
  }

  void VS_STRING_CLASS::fi( const double d ) // sets double as int (w/o frac)
  {
    VS_CHAR tmp[64];
    VS_FN_SPRINTF( tmp, LENOF_VS_CHAR(tmp), VS_CHAR_L("%.0f"), d );
    set( tmp );
  }

  void VS_STRING_CLASS::set( const VS_CHAR* ps )
  {
    if (ps == NULL || ps[0] == 0)
      {
      resize( 0 );
      ASSERT( box->s );
      box->sl = 0;
      box->s[ 0 ] = 0;
      }
    else
      {
      int sl = str_len( ps );
      resize( sl );
      vs_memcpy( box->s, ps, sl );
      box->sl = sl;
      box->s[ sl ] = 0;
      }
  }

  void VS_STRING_CLASS::cat( const VS_CHAR* ps )
  {
    if (ps == NULL) return;
    if (ps[0] == 0) return;
    int psl = str_len( ps );
    resize( box->sl + psl );
    vs_memcpy( box->s + box->sl, ps, psl );
    box->s[ box->sl + psl ] = 0;
    box->sl += psl;
  }

  void VS_STRING_CLASS::setn( const VS_CHAR* ps, int len )
  {
    if ( !ps || len < 1 )
      {
      resize( 0 );
      box->sl = 0;
      box->s[ 0 ] = 0;
      return;
      }
    int z = str_len( ps );
    if ( len < z ) z = len;
    resize( z );
    box->sl = z;
    vs_memcpy( box->s, ps, z );
    box->s[z] = 0;
  }

  void VS_STRING_CLASS::catn( const VS_CHAR* ps, int len )
  {
    if ( !ps || len < 1 ) return;
    int z = str_len( ps );
    if ( len < z ) z = len;
    resize( box->sl + z );
    vs_memcpy( box->s + box->sl, ps, z );
    box->sl += z;
    box->s[ box->sl ] = 0;
  }

  const VS_STRING_CLASS& VS_STRING_CLASS::operator  = ( const VS_STRING_CLASS_R& rs   ) 
  { 
    set( rs.data() ); 
    return *this; 
  }

  const VS_STRING_CLASS& VS_STRING_CLASS::operator  = ( const VS_CHAR_R* prs   )
  { 
    set( prs ); 
    return *this; 
  }
  
/****************************************************************************
**
** VS_STRING_CLASS Functions (for class VS_STRING_CLASS)
**
****************************************************************************/

  VS_STRING_CLASS &str_mul( VS_STRING_CLASS &target, int n ) // multiplies the VS_STRING_CLASS n times, i.e. "1"*5 = "11111"
  {
    target.resize( target.box->sl * n );
    str_mul( target.box->s, n );
    target.fix();
    return target;
  }

  VS_STRING_CLASS &str_del( VS_STRING_CLASS &target, int pos, int len ) // deletes `len' VS_CHARs starting from `pos'
  {
    if ( pos > target.box->sl || pos < 0 ) return target;
    target.detach();
    str_del( target.box->s, pos, len );
    target.fix();
    return target;
  }

  VS_STRING_CLASS &str_ins( VS_STRING_CLASS &target, int pos, const VS_CHAR* s ) // inserts `s' in position `pos'
  {
    if ( pos > target.box->sl || pos < 0 ) return target;
    target.resize( target.box->sl + str_len(s) );
    str_ins( target.box->s, pos, s );
    target.fixlen();
    return target;
  }

  VS_STRING_CLASS &str_ins_ch( VS_STRING_CLASS &target, int pos, VS_CHAR ch ) // inserts `ch' in position `pos'
  {
    if ( pos > target.box->sl || pos < 0 ) return target;
    target.resize( target.box->sl + 1 );
    str_ins_ch( target.box->s, pos, ch );
    target.fixlen();
    return target;
  }

  VS_STRING_CLASS &str_replace( VS_STRING_CLASS &target, const VS_CHAR* out, const VS_CHAR* in ) // replace `out' w. `in'
  {
    int outl = str_len( out );
    int inl = str_len( in );
    int z = str_find( target, out );
    while(z != -1)
      {
      str_del( target, z, outl );
      str_ins( target, z, in );
      z = str_find( target, out, z + inl );
      }
    ASSERT(target.check());
    return target;
  }

  VS_STRING_CLASS &str_copy( VS_STRING_CLASS &target, const VS_CHAR* source, int pos, int len ) // returns `len' VS_CHARs from `pos'
  {
    //FIXME: too many str_len()'s...
    if ( pos < 0 )
      {
      pos = str_len( source ) + pos;
      if ( pos < 0 ) pos = 0;
      }
    if ( len == -1 ) len = str_len( source ) - pos;
    if ( len < 1 )
      {
      target = VS_CHAR_L("");
      return target;
      }
    target.resize( len );
    str_copy( target.box->s, source, pos, len );
    target.fix();
    ASSERT( target.check() );
    return target;
  }

  VS_STRING_CLASS &str_left( VS_STRING_CLASS &target, const VS_CHAR* source, int len ) // returns `len' VS_CHARs from the left
  {
    return str_copy( target, source, 0, len );
  }

  VS_STRING_CLASS &str_right( VS_STRING_CLASS &target, const VS_CHAR* source, int len ) // returns `len' VS_CHARs from the right
  {
    return str_copy( target, source, str_len( source ) - len, len );
  }

  VS_STRING_CLASS &str_sleft( VS_STRING_CLASS &target, int len ) // SelfLeft -- just as 'Left' but works on `this'
  {
    if ( len < target.box->sl )
      {
      target.detach();
      target.box->s[len] = 0;
      target.fix();
      }
    return target;
  }

  VS_STRING_CLASS &str_sright( VS_STRING_CLASS &target, int len ) // SelfRight -- just as 'Right' but works on `this'
  {
    target.detach();
    str_sright( target.box->s, len );
    target.fix();
    return target;
  }


  VS_STRING_CLASS &str_trim_left( VS_STRING_CLASS &target, int len ) // trims `len' VS_CHARs from the beginning (left)
  {
    target.detach();
    str_trim_left( target.box->s, len );
    target.fix();
    return target;
  }

  VS_STRING_CLASS &str_trim_right( VS_STRING_CLASS &target, int len ) // trim `len' VS_CHARs from the end (right)
  {
    target.detach();
    str_trim_right( target.box->s, len );
    target.fix();
    return target;
  }

  VS_STRING_CLASS &str_cut_left( VS_STRING_CLASS &target, const VS_CHAR* charlist ) // remove all VS_CHARs `charlist' from the beginning (i.e. from the left)
  {
    target.detach();
    str_cut_left( target.box->s, charlist );
    target.fix();
    return target;
  }

  VS_STRING_CLASS &str_cut_right( VS_STRING_CLASS &target, const VS_CHAR* charlist ) // remove all VS_CHARs `charlist' from the end (i.e. from the right)
  {
    target.detach();
    str_cut_right( target.box->s, charlist );
    target.fix();
    return target;
  }

  VS_STRING_CLASS &str_cut( VS_STRING_CLASS &target, const VS_CHAR* charlist ) // does `CutR(charlist);CutL(charlist);'
  {
    target.detach();
    str_cut_left( target.box->s, charlist );
    str_cut_right( target.box->s, charlist );
    target.fix();
    return target;
  }

  VS_STRING_CLASS &str_cut_spc( VS_STRING_CLASS &target ) // does `Cut(" ");'
  {
    return str_cut( target, VS_CHAR_L(" ") );
  }

  VS_STRING_CLASS &str_pad( VS_STRING_CLASS &target, int len, VS_CHAR ch )
  {
    target.resize( (len > 0) ? len : -len );
    str_pad( target.box->s, len, ch );
    target.fixlen();
    return target;
  }

  VS_STRING_CLASS &str_comma( VS_STRING_CLASS &target, VS_CHAR delim )
  {
    int new_size = str_len( target ) / 3 + str_len( target );
    target.resize( new_size );
    str_comma( target.box->s, delim );
    target.fix();
    return target;
  }

  void str_set_ch( VS_STRING_CLASS &target, int pos, const VS_CHAR ch ) // sets `ch' VS_CHAR at position `pos'
  {
    if ( pos < 0 ) pos = target.box->sl + pos;
    if ( pos < 0 || pos >= target.box->sl ) return;
    if (target.box->s[pos] != ch) target.detach();
    target.box->s[pos] = ch;
  }

  VS_CHAR str_get_ch( VS_STRING_CLASS &target, int pos ) // return VS_CHAR at position `pos'
  {
    if ( pos < 0 ) pos = target.box->sl + pos;
    if ( pos < 0 || pos >= target.box->sl ) return 0;
    return target.box->s[pos];
  }

  void str_add_ch( VS_STRING_CLASS &target, const VS_CHAR ch ) // adds `ch' at the end
  {
    int sl = target.box->sl;
    if( sl + 1 >= target.box->size ) target.resize( sl + 1 );
    target.box->s[sl] = ch;
    target.box->s[sl+1] = 0;
    target.box->sl++;
  }

  void str_add_ch_range( VS_STRING_CLASS &target, const VS_CHAR fr, const VS_CHAR to ) // adds all from `fr' to 'to' at the end
  {
    if( fr > to ) return;
    target.resize( target.box->sl + ( to - fr ) + 1 );
    for( int i = fr; i <= to; i++ )
      target.box->s[target.box->sl++] = i;
    target.box->s[target.box->sl] = 0;
  }

  VS_CHAR* str_word( VS_STRING_CLASS &target, const VS_CHAR* delimiters, VS_CHAR* result )
  {
    target.detach();
    str_word( target.box->s, delimiters, result );
    target.fix();
    return result[0] ? result : NULL;
  }

  VS_CHAR* str_rword( VS_STRING_CLASS &target, const VS_CHAR* delimiters, VS_CHAR* result )
  {
    target.detach();
    str_rword( target.box->s, delimiters, result );
    target.fix();
    return result;
  }

  int sprintf( int init_size, VS_STRING_CLASS &target, const VS_CHAR *format, ... )
  {
    VS_CHAR *tmp = new VS_CHAR[init_size];
    va_list vlist;
    va_start( vlist, format );
    int res = VS_FN_VSPRINTF( tmp, init_size, format, vlist );
    va_end( vlist );
    target = tmp;
    delete [] tmp;
    return res;
  }

  int sprintf( VS_STRING_CLASS &target, const VS_CHAR *format, ... )
  {
    #define VSPRINTF_BUF_SIZE 1024
    VS_CHAR tmp[VSPRINTF_BUF_SIZE];
    va_list vlist;
    va_start( vlist, format );
    int res = VS_FN_VSPRINTF( tmp, VSPRINTF_BUF_SIZE, format, vlist );
    va_end( vlist );
    target = tmp;
    return res;
  }

  VS_STRING_CLASS& str_tr ( VS_STRING_CLASS& target, const VS_CHAR *from, const VS_CHAR *to )
  {
    target.detach();
    str_tr( target.box->s, from, to );
    return target;
  }

  VS_STRING_CLASS& str_up ( VS_STRING_CLASS& target )
  {
    target.detach();
    str_up( target.box->s );
    return target;
  }

  VS_STRING_CLASS& str_low( VS_STRING_CLASS& target )
  {
    target.detach();
    str_low( target.box->s );
    return target;
  }

  VS_STRING_CLASS& str_flip_case( VS_STRING_CLASS& target )
  {
    target.detach();
    str_flip_case( target.box->s );
    return target;
  }

  VS_STRING_CLASS& str_reverse( VS_STRING_CLASS& target )
  {
    target.detach();
    str_reverse( target.box->s );
    return target;
  }

  VS_STRING_CLASS &str_squeeze( VS_STRING_CLASS &target, const VS_CHAR* sq_VS_CHARs ) // squeeze repeating VS_CHARs to one only
  {
    target.detach();
    str_squeeze( target.box->s, sq_VS_CHARs );
    target.fix();
    return target;
  }

  /* utilities */

  void VS_STRING_CLASS::print() // print string data to stdout (console)
  {
    #ifdef _VSTRING_WIDE_
    wprintf( L"%ls\n", box->s );
    #else
    printf( "%s\n", box->s );
    #endif
  }

  /* conversions/reversed char type functions */

  void VS_STRING_CLASS::set( const VS_CHAR_R* prs )
  {
    if (prs == NULL || prs[0] == 0)
      resize( 0 );
    else
      {
      int rz = VS_FN_CONVERT( NULL, prs, 0 ); // calc required "result size"
      if( rz == -1 )
        return resize( 0 );
      resize( rz );
      VS_FN_CONVERT( box->s, prs, rz );
      box->s[rz] = 0;
      box->sl = rz;
      }
  }

  #ifdef _VSTRING_WIDE_
  int   VS_STRING_CLASS::set_failsafe( const char* mbs )
  {
    int err = 0;
    undef();
    mbtowc( NULL, NULL, 0 ); /* reset mbtowc shift state */
    
    wchar_t wch;
    const char* ps = mbs;
    while( ps )
      {
      int r = mbtowc( &wch, ps, 4 );
      if( r == -1 )
        {
        err++;
        wch = 0xFFFD;
        ps++;
        }
      else if( r > 0 )  
        {
        ps += r;
        }
      else
        {
        return err;
        }  
      str_add_ch( *this, wch );
      }
    return err;
  }
  #endif


/****************************************************************************
**
** VS_STRING_CLASS Functions (for VS_CHAR*)
**
****************************************************************************/

  VS_CHAR* str_set( VS_CHAR* target, const VS_CHAR* ps ) 
  { 
    target[0] = 0; 
    if (ps) VS_FN_STRCPY( target, ps ); 
    VS_FN_STRCPY( target, ps ); 
    return target; 
  }

  VS_CHAR* str_mul( VS_CHAR* target, int n ) // multiplies the string n times, i.e. "1"*5 = "11111"
  {
    if ( n < 0 ) return target;
    if ( n == 0 )
      {
      target[0] = 0;
      return target;
      }
    int sl = str_len( target );
    int z = (n - 1) * sl;
    while( z > 0 )
      {
      vs_memcpy( target + z, target, sl );
      z -= sl;
      }
    target[sl*n] = 0;
    return target;
  }

  int str_find( const VS_CHAR* target, const VS_CHAR c, int startpos ) // returns first zero-based position of VS_CHAR, or -1 if not found
  {
    if (startpos < 0) return -1;
    int sl = str_len( target );
    if (startpos >= sl) return -1;
    const VS_CHAR* pc = VS_FN_STRCHR( target + startpos, c );
    if( ! pc )
      return -1;
    return  pc - target;
  }

  int str_rfind( const VS_CHAR* target, const VS_CHAR c ) // returns last zero-based position of VS_CHAR, or -1 if not found
  {
    const VS_CHAR* pc = VS_FN_STRRCHR( target, c );
    if( ! pc )
      return -1;
    return  pc - target;
  }

  int str_find( const VS_CHAR* target, const VS_CHAR* s, int startpos ) // returns first zero-based position of VS_STRING_CLASS, or -1 if not found
  {
    if (startpos < 0) return -1;
    int sl = str_len( target );
    if (startpos >= sl) return -1;
    const VS_CHAR* pc = VS_FN_STRSTR( target + startpos, s );
    if( ! pc )
      return -1;
    return  pc - target;
  }

  int str_rfind( const VS_CHAR* target, const VS_CHAR* s ) // returns last zero-based position of VS_STRING_CLASS, or -1 if not found
  {
    int sl = str_len( target );
    int sls = str_len( s );
    int z = sl - sls;
    while ( z > 0 )
      {
      if ( VS_FN_STRNCMP( target + z, s, sls ) == 0 ) return z;
      z--;
      }
    return -1;
  }

  VS_CHAR* str_del( VS_CHAR* target, int pos, int len ) // deletes `len' VS_CHARs starting from `pos'
  {
    int sl = str_len( target );
    if ( pos > sl || pos < 0 ) return target;
    int z = sl - pos - len;
    if ( pos + len < sl )
      vs_memmove( target + pos, target + pos + len, z + 1 );
    else
      target[pos] = 0;
    return target;
  }

  VS_CHAR* str_ins( VS_CHAR* target, int pos, const VS_CHAR* s ) // inserts `s' in position `pos'
  {
    int sl = str_len( target );
    if ( pos > sl || pos < 0 ) return target;
    int sls = str_len( s );
    if ( sls < 1 ) return target;

    vs_memmove( target + pos + sls, target + pos, sl - pos + 1 );
    vs_memmove( target + pos, s, sls );

    return target;
  }

  VS_CHAR* str_ins_ch( VS_CHAR* target, int pos, VS_CHAR ch ) // inserts `ch' in position `pos'
  {
    VS_CHAR tmp[2];
    tmp[0] = ch;
    tmp[1] = 0;
    str_ins( target, pos, tmp );
    return target;
  }

  VS_CHAR* str_replace( VS_CHAR* target, const VS_CHAR* out, const VS_CHAR* in ) // replace `out' w. `in'
  {
    int outl = str_len( out );
    int inl = str_len( in );
    int z = str_find( target, out );
    while(z != -1)
      {
      str_del( target, z, outl );
      str_ins( target, z, in );
      z = str_find( target, out, z + inl );
      }
    return target;
  }

  VS_CHAR* str_copy( VS_CHAR* target, const VS_CHAR* source, int pos, int len ) // returns `len' VS_CHARs from `pos'
  {
    ASSERT( len >= -1 );
    int sl = str_len( source );
    if ( pos < 0 )
      {
      pos = sl + pos;
      if ( pos < 0 ) pos = 0;
      }
    if ( pos < 0 || pos >= sl ) return target;
    if ( len == -1 ) len = sl - pos; // untill the end of the string (default arg)
    if ( len < 1 ) return target;
    if ( pos + len >= sl ) len = sl - pos;

    vs_memmove( target, source + pos, len );
    target[ len ] = 0;
    return target;
  }

  VS_CHAR* str_left( VS_CHAR* target, const VS_CHAR* source, int len ) // returns `len' VS_CHARs from the left
  {
    return str_copy( target, source, 0, len );
  }

  VS_CHAR* str_right( VS_CHAR* target, const VS_CHAR* source, int len ) // returns `len' VS_CHARs from the right
  {
    return str_copy( target, source, str_len( source ) - len, len );
  }

  VS_CHAR* str_sleft( VS_CHAR* target, int len ) // SelfLeft -- just as 'Left' but works on `this'
  {
    if ( (size_t)len < str_len(target) && len >= 0 ) target[len] = 0;
    return target;
  }

  VS_CHAR* str_sright( VS_CHAR* target, int len ) // SelfRight -- just as 'Right' but works on `this'
  {
    int sl = str_len( target );
    if( len < sl )
      {
      vs_memmove( target, target + ( sl - len ), len + 1 );
      target[len] = 0;
      }
    return target;
  }

  VS_CHAR* str_trim_left( VS_CHAR* target, int len ) // trims `len' VS_CHARs from the beginning (left)
  {
    int sl = str_len( target );
    int sr = sl - len; // result string length (without trailing 0)
    if( sr >= sl )
      return target;
    if( sr > 0 )
      {
      vs_memmove( target, target + len, sr + 1 );
      target[sr] = 0;
      }
    else
      target[0] = 0;
    return target;
  }

  VS_CHAR* str_trim_right( VS_CHAR* target, int len ) // trim `len' VS_CHARs from the end (right)
  {
    int sl = str_len( target );
    int sr = sl - len; // result string length (without trailing 0)
    if( sr >= sl )
      return target;
    if( sr > 0 )
      target[sr] = 0;
    else
      target[0] = 0;
    return target;
  }

  void str_set_ch( VS_CHAR* target, int pos, const VS_CHAR ch ) // sets `ch' VS_CHAR at position `pos'
  {
    int sl = str_len( target );
    if ( pos < 0 )
      pos = sl + pos;
    if ( pos < 0 || pos >= sl ) return;
    target[pos] = ch;
  }

  VS_CHAR str_get_ch( VS_CHAR* target, int pos ) // return VS_CHAR at position `pos'
  {
    int sl = str_len( target );
    if ( pos < 0 )
      pos = sl + pos;
    if ( pos < 0 || pos >= sl ) return 0;
    return target[pos];
  }

  void str_add_ch( VS_CHAR* target, const VS_CHAR ch ) // adds `ch' at the end
  {
    int sl = str_len( target );
    target[sl] = ch;
    target[sl+1] = 0;
  }

  void str_add_ch_range( VS_CHAR* target, const VS_CHAR fr, const VS_CHAR to ) // adds all from `fr' to 'to' at the end
  {
    if( fr > to ) return;
    int sl = str_len( target );
    for( int i = fr; i < to; i++ )
      target[sl++] = i;
    target[sl] = 0;
  }

  // return first `word', i.e. from pos 0 to first found delimiter VS_CHAR
  // after that deletes this `word' from the target
  VS_CHAR* str_word( VS_CHAR* target, const VS_CHAR* delimiters, VS_CHAR* result )
  {
    int z = 0;
    int sl = str_len( target );
    while ((VS_FN_STRCHR(delimiters, target[z]) == NULL) && (target[z] != 0)) z++;
    vs_memmove(result, target, z);
    result[z] = 0;
    if ( z > 0 )
      {
      if( z < sl )
        vs_memmove( target, target + z + 1, sl - z ); // including trailing zero
      else
        target[0] = 0;
      }
    return result[0] ? result : NULL;
  }

  // ...same but `last' word
  VS_CHAR* str_rword( VS_CHAR* target, const VS_CHAR* delimiters, VS_CHAR* result )
  {
    result[0] = 0;
    int sl = str_len( target );
    int z = sl - 1;
    while ( VS_FN_STRCHR( delimiters, target[z] ) == NULL && z > 0 ) z--;
    if (z < 0) return NULL;
    vs_memmove( result, target + z + 1, sl - z );
    target[z] = 0;
    return result;
  }


  VS_CHAR* str_cut_left( VS_CHAR* target, const VS_CHAR* charlist ) // remove all VS_CHARs `charlist' from the beginning (i.e. from the left)
  {
    int sl = str_len(target);
    if (sl == 0) return target;
    int z = 0;
    while ((VS_FN_STRCHR(charlist, target[z]) != NULL) && (target[z] != 0)) z++;
    if (z == 0) return target;
    vs_memmove( target, target + z, sl - z + 1 );
    return target;
  }

  VS_CHAR* str_cut_right( VS_CHAR* target, const VS_CHAR* charlist ) // remove all VS_CHARs `charlist' from the end (i.e. from the right)
  {
    if (str_len(target) == 0) return target;
    int z = str_len(target) - 1;
    while ((VS_FN_STRCHR(charlist, target[z]) != NULL) && (z > 0)) z--;
    target[z+1] = 0;
    return target;
  }

  VS_CHAR* str_cut( VS_CHAR* target, const VS_CHAR* charlist ) // does `CutR(charlist);CutL(charlist);'
  {
    str_cut_left( target, charlist );
    str_cut_right( target, charlist );
    return target;
  }

  VS_CHAR* str_cut_spc( VS_CHAR* target ) // does `Cut(" ");'
  {
    str_cut_left( target, VS_CHAR_L(" ") );
    str_cut_right( target, VS_CHAR_L(" ") );
    return target;
  }

  // expand string to width 'len' filling with VS_CHAR 'ch'
  // if len > 0 target will be padded right, else left
  VS_CHAR* str_pad( VS_CHAR* target, int len, VS_CHAR ch )
  {
    int sl = str_len( target );
    int _len;
    _len = (len >= 0) ? len : - len;
    if ( _len <= sl ) return target;

    VS_CHAR *tmp = new VS_CHAR[_len + 1]; // result buffer -- need len + 1
    tmp[0] = ch;
    tmp[1] = 0;
    str_mul( tmp, _len - sl );

    if ( len < 0 )
      {
      VS_FN_STRCAT( target, tmp );
      }
    else
      {
      VS_FN_STRCAT( tmp, target );
      VS_FN_STRCPY( target, tmp );
      }
    delete [] tmp;
    return target;
  }


  // insert `commas' for 1000's delimiter or use another delimiter
  // VS_STRING_CLASS supposed to be a integer or real w/o `e' format
  VS_CHAR* str_comma( VS_CHAR* target, VS_CHAR delim )
  {
    int dot = str_rfind( target, VS_CHAR_L('.') );
    if (dot == -1) dot = str_len( target );
    dot -= 3;
    while( dot > 0 )
      {
      str_ins_ch( target, dot , delim );
      dot -= 3;
      }
    return target;
  }


  // translate VS_CHARs from `from' to `to'
  // length of `from' MUST be equal to length of `to'
  VS_CHAR* str_tr( VS_CHAR* target, const VS_CHAR *from, const VS_CHAR *to )
  {
    ASSERT(str_len( from ) == str_len( to ));
    if (str_len( from ) != str_len( to )) return target;
    int z = 0;
    int sl = str_len( target );
    for( z = 0; z < sl; z++ )
      {
      const VS_CHAR *pc = VS_FN_STRCHR( from, target[z] );
      if (pc) target[z] = to[ pc - from ];
      }
    return target;
  }


  VS_CHAR* str_up( VS_CHAR* target )
  {
    int sl = str_len( target );
    for( int z = 0; z < sl; z++ ) target[z] = VS_FN_TOUPPER( target[z] );
    return target;
  }

  VS_CHAR* str_low( VS_CHAR* target )
  {
    int sl = str_len( target );
    for( int z = 0; z < sl; z++ ) target[z] = VS_FN_TOLOWER( target[z] );
    return target;
  }

  VS_CHAR* str_flip_case( VS_CHAR* target ) // CUTE nali? :) // vladi
  {
    int sl = str_len( target );
    for( int z = 0; z < sl; z++ )
      {
      if ( target[z] >= 'a' && target[z] <= 'z' ) target[z] -= 32; else
      if ( target[z] >= 'A' && target[z] <= 'Z' ) target[z] += 32;
      }
    return target;
  }

  VS_CHAR* str_reverse( VS_CHAR* target ) // reverse the VS_STRING_CLASS: `abcde' becomes `edcba' :)
  {
    int z = 0;
    int x = str_len(target)-1;
    while( z < x )
      {
      VS_CHAR ch = target[ z ];
      target[ z++ ] = target[ x ];
      target[ x-- ] = ch;
      }
    return target;
  }

  VS_CHAR* str_squeeze( VS_CHAR* target, const VS_CHAR* sq_VS_CHARs ) // squeeze repeating VS_CHARs to one only
  {
  if ( ! target   ) return NULL;
  if ( ! sq_VS_CHARs ) return NULL;
  int rc = -1;
  int pos = 0;
  while( target[pos] )
    {
    if ( rc == -1 && VS_FN_STRCHR( sq_VS_CHARs, target[pos] ) )
      {
      rc = target[pos];
      pos++;
      }
    else if ( rc != -1 && target[pos] == rc )
      {
      str_del( target, pos, 1 );
      }
    else if ( rc != -1 && target[pos] != rc )
      {
      rc = -1;
      }
    else
      pos++;
    }
  return target;
  }

/****************************************************************************
**
** VS_STRING_CLASS Functions (for const VS_CHAR*)
**
****************************************************************************/

  VS_STRING_CLASS str_up ( const VS_CHAR* src )
  {
    VS_STRING_CLASS ret = src;
    return str_up( ret );
  }

  VS_STRING_CLASS str_low( const VS_CHAR* src )
  {
    VS_STRING_CLASS ret = src;
    return str_low( ret );
  }

  VS_STRING_CLASS str_flip_case( const VS_CHAR* src )
  {
    VS_STRING_CLASS ret = src;
    return str_flip_case( ret );
  }

/****************************************************************************
**
** VS_STRING_CLASS Functions -- common (VS_STRING_CLASS class will pass transparently)
**
****************************************************************************/

  int str_count( const VS_CHAR* target, const VS_CHAR* charlist, int startpos ) // returns match count of all VS_CHARs from `charlist'
  {
    if (!target) return 0;
    int sl = str_len( target );
    if ( startpos >= sl || startpos < 0 ) return 0;
    int z;
    int cnt = 0;
    for ( z = startpos; z < sl; z++ )
      cnt += ( VS_FN_STRCHR( charlist, target[z]) != NULL );
    return cnt;
  }

  int str_str_count( const VS_CHAR* target, const VS_CHAR* s, int startpos ) // returns match count of `s' VS_STRING_CLASS into target
  {
    if (!target) return 0;
    int cnt = 0;
    int sl = str_len( s );
    if ( startpos >= sl || startpos < 0 ) return 0;
    const VS_CHAR* pc = target + startpos;
    while( (pc = VS_FN_STRSTR( pc, s )) )
      {
      pc += sl;
      cnt++;
      }
    return cnt;
  }

  int str_is_int( const VS_CHAR* target ) // check if VS_STRING_CLASS is correct int value
  {
    if (!target) return 0;
    VS_CHAR *tmp = VS_FN_STRDUP( target );
    str_cut_spc( tmp );
    int dc = str_count( tmp, VS_CHAR_L("0123456789") );
    int sl = str_len( tmp );
    free( tmp );
    return ( dc == sl );
  }

  int str_is_double( const VS_CHAR* target ) // check if VS_STRING_CLASS is correct double (w/o `e' format :( )
  {
    if (!target) return 0;
    VS_CHAR *tmp = VS_FN_STRDUP( target );
    str_cut_spc( tmp );
    int dc = str_count( tmp, VS_CHAR_L("0123456789") );
    int cc = str_count( tmp, VS_CHAR_L(".") );
    int sl = str_len( tmp );
    free( tmp );
    return ( (dc + cc == sl) && ( cc == 1 ) );
  }

/***************************************************************************
**
** VARRAYBOX
**
****************************************************************************/

  VS_ARRAY_BOX* VS_ARRAY_BOX::clone()
  {
    VS_ARRAY_BOX *new_box = new VS_ARRAY_BOX();
    new_box->resize( _size );
    new_box->_count = _count;
    int i;
    for( i = 0; i < _count; i++ )
      {
      new_box->_data[i] = new VS_STRING_CLASS;
      *new_box->_data[i] = *_data[i];
      }
    return new_box;
  }

  void VS_ARRAY_BOX::resize( int new_size )
  {
    if ( new_size < 0 ) new_size = 0;
    while ( new_size < _count )
      {
      ASSERT( _data[ _count - 1 ] );
      delete _data[ _count - 1 ];
      _data[ _count - 1 ] = NULL;
      _count--;
      }
    if ( new_size == 0 )
      {
      if ( _data ) delete [] _data;
      _data = NULL;
      _size = 0;
      _count = 0;
      return;
      }
    new_size  = new_size / VARRAY_BLOCK_SIZE + (new_size % VARRAY_BLOCK_SIZE != 0);
    new_size *= VARRAY_BLOCK_SIZE;
    if ( new_size == _size ) return;
    VS_STRING_CLASS** new_data = new VS_STRING_CLASS*[ new_size ];
    ASSERT( new_data );
    memset( new_data, 0, new_size * sizeof(VS_STRING_CLASS*) );
    if ( _data )
      {
      memcpy( new_data, _data,
              (_size < new_size ? _size : new_size) * sizeof(VS_STRING_CLASS*) );
      delete [] _data;
      }
    _size = new_size;
    _data = new_data;
  }

/***************************************************************************
**
** VARRAY
**
****************************************************************************/

  VS_ARRAY_CLASS::VS_ARRAY_CLASS()
  {
    box = new VS_ARRAY_BOX();
    compact = 1;
  }

  VS_ARRAY_CLASS::VS_ARRAY_CLASS( const VS_ARRAY_CLASS& arr )
  {
    box = arr.box;
    box->ref();
    compact = 1;
  }

  VS_ARRAY_CLASS::VS_ARRAY_CLASS( const VS_TRIE_CLASS& tr )
  {
    box = new VS_ARRAY_BOX();
    compact = 1;
    *this = tr;
  }

  VS_ARRAY_CLASS::~VS_ARRAY_CLASS()
  {
    box->unref();
  }

  void VS_ARRAY_CLASS::detach()
  {
    if ( box->refs() == 1 ) return;
    VS_ARRAY_BOX *new_box = box->clone();
    box->unref();
    box = new_box;
  }

  void VS_ARRAY_CLASS::ins( int n, const VS_CHAR* s )
  {
    detach();
    ASSERT( n >= 0 && n <= box->_count );
    if ( box->_count == box->_size ) box->resize( box->_size + 1 );
    memmove( &box->_data[0] + n + 1,
             &box->_data[0] + n,
             ( box->_count - n ) * sizeof(VS_STRING_CLASS*) );
    box->_count++;

    VS_STRING_CLASS* ne = new VS_STRING_CLASS;
    ne->compact( compact );
    ne->set( s );
    box->_data[n] = ne;
  }

  void VS_ARRAY_CLASS::del( int n )
  {
    detach();
    if ( n < 0 || n >= box->_count ) return;
    delete box->_data[n];
    memmove( &box->_data[0] + n,
             &box->_data[0] + n + 1,
             ( box->_count - n ) * sizeof(VS_STRING_CLASS*) );
    box->_count--;
    if ( box->_size - box->_count > VARRAY_BLOCK_SIZE ) box->resize( box->_count );
  }

  void VS_ARRAY_CLASS::set( int n, const VS_CHAR* s )
  {
    detach();
    ASSERT( n >= 0 );
    if ( n >= box->_count )
      {
      int i = n - box->_count + 1;
      while ( i-- ) push( VS_CHAR_L("") );
      }
    ASSERT( n < box->_count );
    box->_data[n]->set( s );
  }

  const VS_CHAR* VS_ARRAY_CLASS::get( int n )
  {
    if ( n < 0 || n >= box->_count )
      return NULL;
    else
      return box->_data[n]->data();
  }

  int VS_ARRAY_CLASS::push( const VS_CHAR* s )
  {
    ins( box->_count, s );
    return box->_count;
  }

  int VS_ARRAY_CLASS::push( VS_TRIE_CLASS *tr )
  {
    tr->keys_and_values( this, this );
    return box->_count;
  }

  int VS_ARRAY_CLASS::push( VS_ARRAY_CLASS *arr )
  {
    ASSERT( arr != this );
    int cnt = arr->count();
    for( int z = 0; z < cnt; z++ )
      push( arr->get( z ) );
    return box->_count;
  }

  const VS_CHAR* VS_ARRAY_CLASS::pop()
  {
    if ( box->_count == 0 ) return NULL;
    _ret_str = get( box->_count - 1 );
    del( box->_count - 1 );
    return _ret_str.data();
  }

  int VS_ARRAY_CLASS::unshift( const VS_CHAR* s )
  {
    ins( 0, s );
    return box->_count;
  }

  int VS_ARRAY_CLASS::unshift( VS_TRIE_CLASS *tr )
  {
    VS_ARRAY_CLASS tmp_arr;
    tr->keys_and_values( &tmp_arr, &tmp_arr );
    unshift( &tmp_arr );
    return box->_count;
  }

  int VS_ARRAY_CLASS::unshift( VS_ARRAY_CLASS *arr )
  {
    ASSERT( arr != this );
    // TODO: this is not efficient, data storage must be moved by input count and filled in place!
    int cnt = arr->count();
    for( int z = cnt - 1; z >= 0; z-- )
      unshift( arr->get( z ) );
    return box->_count;
  }

  const VS_CHAR* VS_ARRAY_CLASS::shift()
  {
    if ( box->_count == 0 ) return NULL;
    _ret_str = get( 0 );
    del( 0 );
    return _ret_str.data();
  }

  int VS_ARRAY_CLASS::fload( const char* fname )
  {
    undef();
    FILE* f = fopen( fname, "rt" );
    if (!f) return 1;
    int r = fload( f );
    fclose(f);
    return r;
  }

  int VS_ARRAY_CLASS::fsave( const char* fname )
  {
    FILE* f = fopen( fname, "wt" );
    if (!f) return 1;
    int r = fsave( f );
    fclose(f);
    return r;
  }

  int VS_ARRAY_CLASS::fload( FILE* f )
  {
    undef();
    char buf[1024*1024];
    VString vstr;
    while( fgets( buf, sizeof(buf)-1, f ) )
      {
      vstr += buf;
      if ( str_get_ch( vstr, -1 ) != '\n' && !feof(f) ) continue;
      while ( str_get_ch( vstr, -1 ) == '\n' ) str_trim_right( vstr, 1 );
      #ifdef _VSTRING_WIDE_
      push( WString( vstr ) );
      #else
      push( vstr );
      #endif
      vstr.undef();
      }
    return 0;
  }

  int VS_ARRAY_CLASS::fsave( FILE* f )
  {
    for( int z = 0; z < box->_count; z++ )
      {
      size_t len;
      const char* ps;
      #ifdef _VSTRING_WIDE_
      VString vstr = get(z);
      ps  = vstr.data();
      len = str_len( vstr );
      #else
      ps  = get(z);
      len = str_len( ps );
      #endif
      if ( fwrite( ps, 1, len, f ) != len ) return 2;
      if ( fwrite( "\n", 1, 1, f ) != 1 ) return 2;
      }
    return 0;
  }

  void VS_ARRAY_CLASS::sort( int rev, int (*q_strcmp)(const VS_CHAR *, const VS_CHAR *) )
  {
    if ( count() > 1 )
      q_sort( 0, count() - 1, q_strcmp ? q_strcmp : VS_FN_STRCMP );
    if ( rev ) // FIXME: not optimal...
      reverse();
  }

  void VS_ARRAY_CLASS::q_sort( int lo, int hi, int (*q_strcmp)(const VS_CHAR *, const VS_CHAR *) )
  {
    int m, l, r;
    const VS_CHAR* v;

    m = ( hi + lo ) / 2;
    v = box->_data[m]->data();
    l = lo;
    r = hi;

    do
      {
      while( (l <= hi) && (q_strcmp(box->_data[l]->data(),v) < 0) ) l++;
      while( (r >= lo) && (q_strcmp(v,box->_data[r]->data()) < 0) ) r--;
      if ( l <= r )
        {
        VS_STRING_CLASS *t;
        t = box->_data[l];
        box->_data[l] = box->_data[r];
        box->_data[r] = t;
        l++;
        r--;
        }
      }
    while( l <= r );

    if ( lo < r ) q_sort( lo, r, q_strcmp );
    if ( l < hi ) q_sort( l, hi, q_strcmp );
  }

  void VS_ARRAY_CLASS::reverse()
  {
    int m = box->_count / 2;
    for( int z = 0; z < m; z++ )
      {
      VS_STRING_CLASS *t;
      t = box->_data[z];
      box->_data[z] = box->_data[box->_count-1-z];
      box->_data[box->_count-1-z] = t;
      }
  }

  void VS_ARRAY_CLASS::shuffle() /* Fisher-Yates shuffle */
  {
    int i = box->_count - 1;
    while( i >= 0 )
      {
      int j = rand() % ( i + 1 );
      VS_STRING_CLASS *t;
      t = box->_data[i];
      box->_data[i] = box->_data[j];
      box->_data[j] = t;
      i--;
      }
  }

  void VS_ARRAY_CLASS::print()
  {
    int z;
    for( z = 0; z < count(); z++ )
      VS_FN_PRINTF( VS_CHAR_L( "%d=" VS_SFMT "\n" ), z, get(z) );
  }

  int VS_ARRAY_CLASS::max_len()
  {
    if ( count() == 0 ) return 0;
    int l = 0;
    int z;
    for( z = 0; z < count(); z++ )
      {
      int sl = str_len(get(z));
      if ( sl > l )
        l = sl;
      }
    return l;
  }

  int VS_ARRAY_CLASS::min_len()
  {
    if ( count() == 0 ) return 0;
    int l = str_len(get(0));
    int z;
    for( z = 0; z < count(); z++ )
      {
      int sl = str_len(get(z));
      if ( sl < l )
        l = sl;
      }
    return l;
  }

/***************************************************************************
**
** VTRIENODE
**
****************************************************************************/

  VS_TRIE_NODE::VS_TRIE_NODE()
  {
    next = NULL;
    down = NULL;
    c = 0;
    data = NULL;
  }

  VS_TRIE_NODE::~VS_TRIE_NODE()
  {
    if ( next ) delete next;
    if ( down ) delete down;
    if ( data ) delete data;
  }

  void VS_TRIE_NODE::del_node( const VS_CHAR *key, int branch )
  {
    if ( !key ) return;
    if ( !key[0] ) return;

    if ( key[1] == 0 )
      { // last VS_CHAR reached
      if ( key[0] != c ) return;  // not found
      // key found
      if ( data ) delete data; // release key's value
      data = NULL;
      if ( down )
        { // there are more keys below
        if ( branch )
          {
          delete down; // delete all keys below
          c = 0; // mark current as `not used'
          }
        }
      else
        { // there is no more keys below
        c = 0; // mark current as `not used'
        }
      }
    else
      { // last VS_CHAR is not reached
      if ( key[0] == c )
        { // current VS_CHAR is in the key
        if ( down )
          { // try key down
          down->del_node( key + 1, branch );
          if ( down->c == 0 )
            { // down node is not used--remove it
            ASSERT( down->down == NULL );
            VS_TRIE_NODE *tmp = down;
            down = down->next;
            delete tmp;
            }
          }
        else
          return; // not found
        }
      else
        {
        if ( next ) // VS_CHAR is not in the key, try next
          {
          next->del_node( key, branch );
          if ( next->c == 0 )
            { // down node is not used--remove it
            ASSERT( next->down == NULL );
            VS_TRIE_NODE *tmp = next;
            next = next->next;
            delete tmp;
            }
          }
        else
          return; // not found
        }
      }
  }

  VS_TRIE_NODE* VS_TRIE_NODE::find_node( const VS_CHAR* key, int create )
  {
    if ( !key || !key[0] ) return NULL;
    if ( key[0] == c )
      { // VS_CHAR in the current key
      if ( key[1] == 0 )
        { // last VS_CHAR and is in the key--found!
        return this;
        }
      else
        { // not last VS_CHAR...
        if ( ! down && create )
          { // nothing below but should create
          down = new VS_TRIE_NODE();
          down->c = key[1];
          }
        if ( down )
          return down->find_node( key + 1, create ); // search below
        else
          return NULL; // not found
        }
      }
    else
      { // VS_CHAR not in the current key--try next
      if ( ! next && create )
        { // no next but should create
        next = new VS_TRIE_NODE();
        next->c = key[0];
        }
      if ( next )
        return next->find_node( key, create ); // search next
      else
        return NULL; // not found
      }
  }

  VS_TRIE_NODE *VS_TRIE_NODE::clone()
  {
    VS_TRIE_NODE *tmp = new VS_TRIE_NODE();
    tmp->c = c;
    if ( next ) tmp->next = next->clone();
    if ( down ) tmp->down = down->clone();
    if ( data ) tmp->data = new VS_STRING_CLASS( *data );
    return tmp;
  }

  void VS_TRIE_NODE::print()
  {
    VS_FN_PRINTF( VS_CHAR_L("---------------------------------\n") );
    VS_FN_PRINTF( VS_CHAR_L("this = %p\n"), (void*)this );
    VS_FN_PRINTF( VS_CHAR_L("key  = %c\n"), c );
    VS_FN_PRINTF( VS_CHAR_L("next = %p\n"), (void*)next );
    VS_FN_PRINTF( VS_CHAR_L("down = %p\n"), (void*)down );
    VS_FN_PRINTF( VS_CHAR_L("data = " VS_SFMT "\n"), data ? data->data() : VS_CHAR_L("(null)") );
    if (next) next->print();
    if (down) down->print();
  }

/***************************************************************************
**
** VTRIEBOX
**
****************************************************************************/

  VS_TRIE_BOX* VS_TRIE_BOX::clone()
  {
    VS_TRIE_BOX *new_box = new VS_TRIE_BOX();
    delete new_box->root;
    new_box->root = root->clone();
    return new_box;
  }

/***************************************************************************
**
** VTRIE
**
****************************************************************************/

  VS_TRIE_CLASS::VS_TRIE_CLASS()
  {
    box = new VS_TRIE_BOX();
  }

  VS_TRIE_CLASS::VS_TRIE_CLASS( const VS_ARRAY_CLASS& arr )
  {
    box = new VS_TRIE_BOX();
    merge( (VS_ARRAY_CLASS*)&arr );
  }

  VS_TRIE_CLASS::VS_TRIE_CLASS( const VS_TRIE_CLASS& tr )
  {
    box = tr.box;
    box->ref();
  }


  VS_TRIE_CLASS::~VS_TRIE_CLASS()
  {
    box->unref();
  }

  void VS_TRIE_CLASS::detach()
  {
    if ( box->refs() == 1 ) return;
    VS_TRIE_BOX *new_box = box->clone();
    box->unref();
    box = new_box;
  }

  void VS_TRIE_CLASS::trace_node( VS_TRIE_NODE *node, VS_ARRAY_CLASS* keys, VS_ARRAY_CLASS *vals )
  {
    int kl = str_len( temp_key );
    if ( node->c ) str_add_ch( temp_key, node->c );
    if ( node->data )
      {
      if ( keys ) keys->push( temp_key );
      if ( vals ) vals->push( node->data->data() );
      }
    if ( node->down ) trace_node( node->down, keys, vals );
    str_sleft( temp_key, kl );
    if ( node->next ) trace_node( node->next, keys, vals );
  }

  void VS_TRIE_CLASS::set( const VS_CHAR* key, const VS_CHAR* value )
  {
    if ( !value || !key || !key[0] ) return;
    detach();
    VS_TRIE_NODE *node = box->root->find_node( key, 1 );
    ASSERT( node );
    if ( ! node->data )
      node->data = new VS_STRING_CLASS();
    node->data->set( value );
  }

  const VS_CHAR* VS_TRIE_CLASS::get( const VS_CHAR* key )
  {
    if ( !key || !key[0] ) return NULL;
    VS_TRIE_NODE *node = box->root->find_node( key );
    if ( node && node->data )
      return node->data->data();
    else
      return NULL;
  }

  void VS_TRIE_CLASS::del( const VS_CHAR* key )
  {
    if ( !key || !key[0] ) return;
    detach();
    box->root->del_node( key );
  }

  int VS_TRIE_CLASS::exists( const VS_CHAR* key ) // return != 0 if key exist (with data)
  {
    VS_TRIE_NODE *node = box->root->find_node( key );
    if ( node && node->data )
      return 1;
    else
      return 0;
  }

  void VS_TRIE_CLASS::keys_and_values( VS_ARRAY_CLASS *keys, VS_ARRAY_CLASS *values )
  {
    trace_node( box->root, keys, values );
  }

  VS_ARRAY_CLASS VS_TRIE_CLASS::keys()
  {
    VS_ARRAY_CLASS arr;
    keys_and_values( &arr, NULL );
    return arr;
  }

  VS_ARRAY_CLASS VS_TRIE_CLASS::values()
  {
    VS_ARRAY_CLASS arr;
    keys_and_values( NULL, &arr );
    return arr;
  }

  void VS_TRIE_CLASS::reverse()
  {
    VS_ARRAY_CLASS ka = keys();
    VS_ARRAY_CLASS va = values();
    ASSERT( ka.count() == va.count() );
    undef();
    int z = ka.count();
    while( z-- )
      {
      set( va.get( z ), ka.get( z ) );
      }
  }

  void VS_TRIE_CLASS::merge( VS_TRIE_CLASS *tr )
  {
    ASSERT( tr != this );
    VS_ARRAY_CLASS ka = tr->keys();
    VS_ARRAY_CLASS va = tr->values();
    ASSERT( ka.count() == va.count() );
    int z = ka.count();
    while( z-- )
      {
      set( ka.get( z ), va.get( z ) );
      }
  }

  void VS_TRIE_CLASS::merge( VS_ARRAY_CLASS *arr )
  {
    int z = 0;
    while( z < arr->count() )
      {
      set( arr->get( z ), arr->get( z + 1 ) );
      z += 2;
      }
  }

  int VS_TRIE_CLASS::fload( const char* fname )
  {
    FILE* f = fopen( fname, "rt" );
    if (!f) return 1;
    int r = fload( f );
    fclose(f);
    return r;
  }

  int VS_TRIE_CLASS::fsave( const char* fname )
  {
    FILE* f = fopen( fname, "wt" );
    if (!f) return 1;
    int r = fsave( f );
    fclose(f);
    return r;
  }

  int VS_TRIE_CLASS::fload( FILE* f )
  {
    VS_ARRAY_CLASS arr;
    int r = arr.fload( f );
    if ( r == 0 )
      merge( &arr );
    return r;
  }

  int VS_TRIE_CLASS::fsave( FILE* f )
  {
    VS_ARRAY_CLASS arr;
    arr.push( this );
    return arr.fsave( f );
  }

  void VS_TRIE_CLASS::print()
  {
    VS_ARRAY_CLASS ka = keys();
    VS_ARRAY_CLASS va = values();
    ASSERT( ka.count() == va.count() );
    while( ka.count() && va.count() )
      {
      VS_FN_PRINTF( VS_CHAR_L( "" VS_SFMT "=" VS_SFMT "\n"), ka.pop(), va.pop() );
      }
  }

/****************************************************************************
**
** VS_STRING_CLASS Utilities -- functions and classes
**
****************************************************************************/

  VS_STRING_CLASS str_dot_reduce( const VS_CHAR* s, int width )
  {
    VS_STRING_CLASS dest;
    dest = s;
    int sl = str_len( dest );
    if ( sl <= width ) return dest;
    int pos = (width-3) / 2;
    str_del( dest, pos, sl - width + 3 );
    str_ins( dest, pos, VS_CHAR_L("...") );
    return dest;
  }

/****************************************************************************
**
** VS_STRING_CLASS file names utilities -- functions and classes
** NOTE: does not use any external function calls!
**
****************************************************************************/

VS_CHAR* str_fix_path( VS_CHAR* s, int slashtype )
{
  size_t sl = str_len( s );
  if ( s[sl-1] != slashtype )
    {
    s[sl] = slashtype;
    s[sl+1] = 0;
    }
  return s;
}

const VS_STRING_CLASS& str_fix_path( VS_STRING_CLASS &s, int slashtype )
{
  size_t sl = str_len( s );
  if ( s[sl-1] != slashtype )
    str_add_ch( s, slashtype );
  return s;
}

VS_STRING_CLASS str_file_ext( const VS_CHAR *ps )
{
  VS_STRING_CLASS ext;
  int len = str_len(ps);
  int z = len - 1;
  while ( ps[z] != VS_CHAR_L('.') && ps[z] != VS_CHAR_L('/') && z > 0 ) z--;
  if ( ps[z] == VS_CHAR_L('.') )
    if ( !(z == 0 || (z > 0 && ps[z-1] == VS_CHAR_L('/'))) ) // `.name' has no extension!
      ext = ps + z + 1;
  return ext;
}

VS_STRING_CLASS str_file_name( const VS_CHAR *ps )
{
  VS_STRING_CLASS name;

  int len = str_len( ps );
  int z = len - 1;

  while ( z >= 0 && ps[z] != VS_CHAR_L('/') ) z--;
  name = ps + z + 1;

  z = str_len( name ) - 1;
  while ( z > 0 && name[z] != VS_CHAR_L('.') && name[z] != VS_CHAR_L('/') ) z--;
  if ( z > 0 && name[z] == VS_CHAR_L('.')) // `.name' has no extension!
    str_sleft( name, z );
  return name;
}

VS_STRING_CLASS str_file_name_ext( const VS_CHAR *ps )
{
  VS_STRING_CLASS name;

  int len = str_len( ps );
  int z = len - 1;

  while ( z >= 0 && ps[z] != VS_CHAR_L('/') ) z--;
  name = ps + z + 1;
  return name;
}

VS_STRING_CLASS str_file_path( const VS_CHAR *ps )
{
  VS_STRING_CLASS name;

  int len = str_len( ps );
  int z = len;

  while ( z >= 0 && ps[z] != VS_CHAR_L('/') ) z--;
  name = ps;
  str_sleft( name, z+1 );
  return name;
}


VS_STRING_CLASS str_reduce_path( const VS_CHAR* path ) // removes ".."s
{
  VS_STRING_CLASS dest;
  dest = path;
  str_replace( dest, VS_CHAR_L("/./"), VS_CHAR_L("/") );
  int i = -1;
  while( (i = str_find( dest, VS_CHAR_L("/../") ) ) != -1 )
    {
    int j = i - 1;
    while( j >= 0 && dest[j] != VS_CHAR_L('/') ) j--;
    ASSERT( j >= -1 );
  if ( j < 0 )
    {
    if ( dest[0] == VS_CHAR_L('/') )
      str_del( dest, 0, 3 );
    }
  else
    str_del( dest, j+1, i+3-j );
    }
  return dest;
}

/****************************************************************************
**
** VS_STRING_CLASS Conversions
**
****************************************************************************/

  long hex2long( const VS_CHAR* s ) // hex to long
  {
    long P = 1;
    long C = 0;

    VS_CHAR tmp[255];
    VS_FN_STRCPY( tmp, s );
    str_up( tmp );
    str_cut_spc( tmp );

    int sl = str_len(tmp);
    for( int z=sl-1; z >= 0; z-- )
      {
      int i = -1;
      if( (i = str_find( VS_CHAR_L("0123456789ABCDEF"), tmp[z] )) != -1)
        C = C + P*i;
      else
        C = 0;
      P = P*16;
      }
    return C;
  }

/***************************************************************************
**
** EOF
**
****************************************************************************/

