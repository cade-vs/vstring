/****************************************************************************
 #
 #  VSTRING Library
 #
 #  test suite for VString / WString and the str_*() function family
 #
 #  build:
 #    g++ -O0 -ggdb3 -I. -o test_vstring test_vstring.cpp libvstring.a \
 #        -lpcre2-8 -lpcre2-32
 #  build with memory checking:
 #    g++ -O0 -ggdb3 -fsanitize=address -I. -o test_vstring test_vstring.cpp \
 #        vstring.cpp wstring.cpp vstrlib.cpp wstrlib.cpp vstruti.cpp vref.cpp \
 #        -lpcre2-8 -lpcre2-32
 #
 #  part 1 -- named cases, each carrying its own __LINE__ so that a failure
 #            can be broken on directly:  break test_vstring.cpp:LINE
 #  part 2 -- exhaustive sweeps over bounded domains, checking invariants and
 #            cross-checking the three parallel APIs against each other:
 #              VS_CHAR* form  vs  VString form
 #              VString        vs  WString  (on ASCII, results must agree)
 #  part 3 -- unicode / multibyte behaviour, incl. the known problem areas
 #
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include "vstring.h"
#include "wstring.h"
#include "vstrlib.h"
#include "wstrlib.h"
#include "vstruti.h"

/****************************************************************************
** test bookkeeping
****************************************************************************/

int     tests_run    = 0;
int     tests_failed = 0;
int     quiet        = 0;
VString failed_lines;

static void pass_fail( int line, int ok, const char* what,
                       const char* got, const char* want )
{
  tests_run++;
  if( ! ok ) { tests_failed++; failed_lines = failed_lines + " " + VString( line ); }
  if( ok && quiet ) return;
  printf( "  %4d  %-4s  %-34s got %-22s want %s\n",
          line, ok ? "ok" : "FAIL", what, got, want );
}

/* narrow string result */
void eqs( int line, const char* what, const char* got, const char* want )
{
  VString g = VString( "[" ) + ( got  ? got  : "(null)" ) + "]";
  VString w = VString( "[" ) + ( want ? want : "(null)" ) + "]";
  pass_fail( line, got && want && strcmp( got, want ) == 0, what, g, w );
}

/* integer result */
void eqi( int line, const char* what, long got, long want )
{
  pass_fail( line, got == want, what, VString( (long)got ), VString( (long)want ) );
}

/* double result, compared with a tolerance */
void eqd( int line, const char* what, double got, double want )
{
  double d = got - want;
  if( d < 0 ) d = -d;
  pass_fail( line, d < 0.0000001, what, VString( got ), VString( want ) );
}

/* wide string result -- shown as its multibyte rendering */
void eqw( int line, const char* what, const wchar_t* got, const wchar_t* want )
{
  VString g = VString( "[" ) + VString( got  ? got  : L"(null)" ) + "]";
  VString w = VString( "[" ) + VString( want ? want : L"(null)" ) + "]";
  pass_fail( line, got && want && wcscmp( got, want ) == 0, what, g, w );
}

/* report only -- for behaviour that is known/accepted rather than required */
void nfo( int line, const char* what, const char* got, const char* note )
{
  printf( "  %4d  --    %-34s got %-22s %s\n",
          line, what, ( VString( "[" ) + ( got ? got : "(null)" ) + "]" ).data(),
          note ? note : "" );
}

void nfoi( int line, const char* what, long got, const char* note )
{
  printf( "  %4d  --    %-34s got %-22s %s\n",
          line, what, VString( (long)got ).data(), note ? note : "" );
}

void gr( const char* name )
{
  printf( "\n--- %s\n", name );
}

/****************************************************************************
** scratch buffers
**
** the VS_CHAR* half of the API writes into the buffer it is given and may
** grow it (str_ins, str_pad, str_mul ...), so those need room to work in;
** read-only functions are exercised on exact-size heap copies further down,
** where a build with -fsanitize=address can see past the end of them.
****************************************************************************/

#define BUFSZ 256

static char* buf( const char* s )
{
  static char ring[16][BUFSZ];
  static int  n = 0;
  n = ( n + 1 ) % 16;
  strncpyz( ring[n], s, BUFSZ );
  return ring[n];
}

/****************************************************************************
** part 1 -- named cases
****************************************************************************/

void t_construct()
{
  gr( "construction and assignment" );
  VString a;                    eqs( __LINE__, "VString()",              a, "" );
  VString b( "hello" );         eqs( __LINE__, "VString(\"hello\")",     b, "hello" );
  VString c( b );               eqs( __LINE__, "VString(copy)",          c, "hello" );
  VString d = b;                eqs( __LINE__, "VString d = b",          d, "hello" );
  VString e( (const char*)NULL );eqs( __LINE__, "VString((char*)NULL)",  e, "" );
  VString f( 42 );              eqs( __LINE__, "VString(42)",            f, "42" );
  VString g( -42 );             eqs( __LINE__, "VString(-42)",           g, "-42" );
  VString h( 0 );               eqs( __LINE__, "VString(0)",             h, "0" );
  VString i( (long)1234567 );   eqs( __LINE__, "VString(1234567L)",      i, "1234567" );
  VString j( (long long)-9876543210LL );
                                eqs( __LINE__, "VString(-9876543210LL)", j, "-9876543210" );
  a = "reassigned";             eqs( __LINE__, "a = \"reassigned\"",     a, "reassigned" );
  a = 7;                        eqs( __LINE__, "a = 7",                  a, "7" );
  a = b;                        eqs( __LINE__, "a = b",                  a, "hello" );
  a.undef();                    eqs( __LINE__, "a.undef()",              a, "" );
  eqi( __LINE__, "str_len(undef)", str_len( a ), 0 );

  gr( "empty string is not the null pointer" );
  VString z;
  eqi( __LINE__, "data() != NULL",   z.data() != NULL, 1 );
  eqi( __LINE__, "str_len(\"\")",    str_len( z ),     0 );
  eqi( __LINE__, "z == \"\"",        z == "",          1 );
}

void t_numbers()
{
  gr( "numeric conversion out" );
  VString a( "42" );            eqi( __LINE__, "VString(\"42\").i()",    a.i(),  42 );
  VString b( "-17" );           eqi( __LINE__, "VString(\"-17\").i()",   b.i(), -17 );
  VString c( "  99  " );        eqi( __LINE__, "leading space .i()",     c.i(),  99 );
  VString d( "abc" );           eqi( __LINE__, "VString(\"abc\").i()",   d.i(),   0 );
  VString e( "" );              eqi( __LINE__, "VString(\"\").i()",      e.i(),   0 );
  VString f( "12abc" );         eqi( __LINE__, "VString(\"12abc\").i()", f.i(),  12 );
  VString g( "3.5" );           eqd( __LINE__, "VString(\"3.5\").f()",   g.f(), 3.5 );
  VString h( "-0.25" );         eqd( __LINE__, "VString(\"-0.25\").f()", h.f(), -0.25 );
  VString i( "9999999999" );    eqi( __LINE__, "\"9999999999\".ll()",    i.ll(), 9999999999LL );

  gr( "numeric conversion in" );
  VString a2; a2.i( 42 );       eqs( __LINE__, "a.i(42)",                a2, "42" );
  VString b2; b2.l( -1 );       eqs( __LINE__, "a.l(-1)",                b2, "-1" );
  VString c2; c2.ll( 123456789012LL );
                                eqs( __LINE__, "a.ll(123456789012)",     c2, "123456789012" );
  VString d2; d2.fi( 3.99 );    eqs( __LINE__, "a.fi(3.99) rounds",      d2, "4" );
  VString e3; e3.fi( 3.01 );    eqs( __LINE__, "a.fi(3.01) rounds",      e3, "3" );
  VString f3; f3.fi( -3.99 );   eqs( __LINE__, "a.fi(-3.99) rounds",     f3, "-4" );

  gr( "str_is_int / str_is_double -- digits (and one dot) only, spaces trimmed" );
  eqi( __LINE__, "str_is_int(\"42\")",      str_is_int( "42" )      != 0, 1 );
  eqi( __LINE__, "str_is_int(\" 42 \")",    str_is_int( " 42 " )    != 0, 1 );
  eqi( __LINE__, "str_is_int(\"4.2\")",     str_is_int( "4.2" )     != 0, 0 );
  eqi( __LINE__, "str_is_int(\"abc\")",     str_is_int( "abc" )     != 0, 0 );
  eqi( __LINE__, "str_is_int(\"12a\")",     str_is_int( "12a" )     != 0, 0 );
  eqi( __LINE__, "str_is_double(\"4.2\")",  str_is_double( "4.2" )  != 0, 1 );
  eqi( __LINE__, "str_is_double(\"abc\")",  str_is_double( "abc" )  != 0, 0 );
  eqi( __LINE__, "str_is_double(\"4.2.3\")",str_is_double( "4.2.3" )!= 0, 0 );

  gr( "str_is_int / str_is_double -- open questions, reported not asserted" );
  nfoi( __LINE__, "str_is_int(\"-42\")",     str_is_int( "-42" ),
        "0 -- a leading sign is not accepted" );
  nfoi( __LINE__, "str_is_int(\"+42\")",     str_is_int( "+42" ),
        "0 -- a leading sign is not accepted" );
  nfoi( __LINE__, "str_is_int(\"\")",        str_is_int( "" ),
        "non-zero -- the empty string counts as an int (0 digits == 0 length)" );
  nfoi( __LINE__, "str_is_double(\"-4.2\")", str_is_double( "-4.2" ),
        "0 -- a leading sign is not accepted" );
  nfoi( __LINE__, "str_is_double(\"42\")",   str_is_double( "42" ),
        "0 -- exactly one '.' is required, so an integer is not a double" );
}

void t_concat_compare()
{
  gr( "concatenation" );
  VString a = "foo";
  a += "bar";                   eqs( __LINE__, "\"foo\" += \"bar\"",     a, "foobar" );
  a += '!';                     eqs( __LINE__, "a += '!'",               a, "foobar!" );
  a += 12;                      eqs( __LINE__, "a += 12",                a, "foobar!12" );
  VString b = VString( "x" ) + "y";
                                eqs( __LINE__, "\"x\" + \"y\"",          b, "xy" );
  VString c = VString( "n=" ) + 5;
                                eqs( __LINE__, "\"n=\" + 5",             c, "n=5" );
  VString d = 5 + VString( "=n" );
                                eqs( __LINE__, "5 + \"=n\"",             d, "5=n" );
  VString e = "";
  e += "";                      eqs( __LINE__, "\"\" += \"\"",           e, "" );
  VString f = "a";
  f += VString( "b" ) ;         eqs( __LINE__, "VString += VString",     f, "ab" );

  gr( "cat / set / catn / setn" );
  VString g; g.set( "abc" );    eqs( __LINE__, "g.set(\"abc\")",         g, "abc" );
  g.cat( "def" );               eqs( __LINE__, "g.cat(\"def\")",         g, "abcdef" );
  VString h; h.setn( "abcdef", 3 );
                                eqs( __LINE__, "h.setn(\"abcdef\",3)",   h, "abc" );
  h.catn( "xyzw", 2 );          eqs( __LINE__, "h.catn(\"xyzw\",2)",     h, "abcxy" );
  VString i; i.setn( "ab", 10 );
                                eqs( __LINE__, "setn past the end",      i, "ab" );

  gr( "comparison operators" );
  VString p = "abc", q = "abc", r = "abd";
  eqi( __LINE__, "p == q",      p == q,    1 );
  eqi( __LINE__, "p != r",      p != r,    1 );
  eqi( __LINE__, "p <  r",      p <  r,    1 );
  eqi( __LINE__, "r >  p",      r >  p,    1 );
  eqi( __LINE__, "p <= q",      p <= q,    1 );
  eqi( __LINE__, "p >= q",      p >= q,    1 );
  eqi( __LINE__, "p == \"abc\"", p == "abc", 1 );
  eqi( __LINE__, "\"abc\" == p", "abc" == p, 1 );
  eqi( __LINE__, "\"abd\" >  p", "abd" >  p, 1 );
  VString s1 = "", s2 = "";
  eqi( __LINE__, "\"\" == \"\"", s1 == s2,  1 );
  VString s3 = "ab", s4 = "abc";
  eqi( __LINE__, "\"ab\" < \"abc\"", s3 < s4, 1 );
}

void t_index()
{
  gr( "indexing -- operator[], str_get_ch, str_set_ch (all range safe)" );
  VString a = "abcdef";
  eqi( __LINE__, "a[0]",                a[0],  'a' );
  eqi( __LINE__, "a[5]",                a[5],  'f' );
  eqi( __LINE__, "a[-1] is last",       a[-1], 'f' );
  eqi( __LINE__, "a[-6] is first",      a[-6], 'a' );
  eqi( __LINE__, "a[6] out of range",   a[6],   0  );
  eqi( __LINE__, "a[-7] out of range",  a[-7],  0  );
  eqi( __LINE__, "a[1000] out of range",a[1000],0  );
  eqi( __LINE__, "str_get_ch(a,2)",     str_get_ch( a,  2 ), 'c' );
  eqi( __LINE__, "str_get_ch(a,-2)",    str_get_ch( a, -2 ), 'e' );
  eqi( __LINE__, "str_get_ch(a,99)",    str_get_ch( a, 99 ),  0  );
  str_set_ch( a, 0, 'X' );      eqs( __LINE__, "str_set_ch(a,0,'X')",    a, "Xbcdef" );
  str_set_ch( a, -1, 'Z' );     eqs( __LINE__, "str_set_ch(a,-1,'Z')",   a, "XbcdeZ" );
  str_set_ch( a, 99, 'Q' );     eqs( __LINE__, "str_set_ch out of range",a, "XbcdeZ" );
  str_add_ch( a, '+' );         eqs( __LINE__, "str_add_ch(a,'+')",      a, "XbcdeZ+" );
  VString r; str_add_ch_range( r, 'a', 'e' );
                                eqs( __LINE__, "str_add_ch_range(a..e)", r, "abcde" );
  VString r2; str_add_ch_range( r2, 'e', 'a' );
                                eqs( __LINE__, "str_add_ch_range(e..a)", r2, "" );
  VString r3; str_add_ch_range( r3, 'q', 'q' );
                                eqs( __LINE__, "str_add_ch_range(q..q)", r3, "q" );
}

void t_slice()
{
  gr( "str_left / str_right / str_copy" );
  VString t;
  str_left( t, "abcdef", 3 );   eqs( __LINE__, "str_left(\"abcdef\",3)",  t, "abc" );
  str_left( t, "abcdef", 0 );   eqs( __LINE__, "str_left(...,0)",         t, "" );
  str_left( t, "abcdef", 99 );  eqs( __LINE__, "str_left(...,99)",        t, "abcdef" );
  str_left( t, "abcdef", -1 );  eqs( __LINE__, "str_left(...,-1) = to the end", t, "abcdef" );
  str_right( t, "abcdef", 3 );  eqs( __LINE__, "str_right(\"abcdef\",3)", t, "def" );
  str_right( t, "abcdef", 0 );  eqs( __LINE__, "str_right(...,0)",        t, "" );
  str_right( t, "abcdef", 99 ); eqs( __LINE__, "str_right(...,99)",       t, "abcdef" );
  str_copy( t, "abcdef", 2, 3 );eqs( __LINE__, "str_copy(...,2,3)",       t, "cde" );
  str_copy( t, "abcdef", 0, -1 );eqs(__LINE__, "str_copy(...,0,-1) = all",t, "abcdef" );
  str_copy( t, "abcdef", 4, 99 );eqs(__LINE__, "str_copy past the end",   t, "ef" );
  str_copy( t, "abcdef", 99, 2 );eqs(__LINE__, "str_copy pos past end",   t, "" );
  str_copy( t, "", 0, 3 );      eqs( __LINE__, "str_copy from \"\"",      t, "" );

  gr( "str_sleft / str_sright -- in place" );
  VString a = "abcdef"; str_sleft( a, 2 );
                                eqs( __LINE__, "str_sleft(a,2)",          a, "ab" );
  VString b = "abcdef"; str_sright( b, 2 );
                                eqs( __LINE__, "str_sright(b,2)",         b, "ef" );
  VString c = "abcdef"; str_sleft( c, 99 );
                                eqs( __LINE__, "str_sleft(c,99)",         c, "abcdef" );
  VString d = "abcdef"; str_sleft( d, 0 );
                                eqs( __LINE__, "str_sleft(d,0)",          d, "" );

  gr( "str_trim_left / str_trim_right / str_chop" );
  VString e = "abcdef"; str_trim_left( e, 2 );
                                eqs( __LINE__, "str_trim_left(e,2)",      e, "cdef" );
  VString f = "abcdef"; str_trim_right( f, 2 );
                                eqs( __LINE__, "str_trim_right(f,2)",     f, "abcd" );
  VString g = "abcdef"; str_trim_left( g, 99 );
                                eqs( __LINE__, "str_trim_left(g,99)",     g, "" );
  VString h = "abcdef"; str_trim_right( h, 0 );
                                eqs( __LINE__, "str_trim_right(h,0)",     h, "abcdef" );
  VString i = "abcdef"; str_chop( i );
                                eqs( __LINE__, "str_chop(i)",             i, "abcde" );
  VString j = ""; str_chop( j );
                                eqs( __LINE__, "str_chop(\"\")",          j, "" );
}

void t_edit()
{
  gr( "str_del" );
  VString a = "abcdef"; str_del( a, 1, 2 );
                                eqs( __LINE__, "str_del(a,1,2)",          a, "adef" );
  VString b = "abcdef"; str_del( b, 0, 99 );
                                eqs( __LINE__, "str_del(b,0,99)",         b, "" );
  VString c = "abcdef"; str_del( c, 99, 2 );
                                eqs( __LINE__, "str_del past the end",    c, "abcdef" );
  VString d = "abcdef"; str_del( d, 2, 0 );
                                eqs( __LINE__, "str_del(d,2,0)",          d, "abcdef" );

  gr( "str_ins / str_ins_ch" );
  VString e = "abcdef"; str_ins( e, 0, "XY" );
                                eqs( __LINE__, "str_ins(e,0,\"XY\")",     e, "XYabcdef" );
  VString f = "abcdef"; str_ins( f, 3, "XY" );
                                eqs( __LINE__, "str_ins(f,3,\"XY\")",     f, "abcXYdef" );
  VString g = "abcdef"; str_ins( g, 6, "XY" );
                                eqs( __LINE__, "str_ins at the end",      g, "abcdefXY" );
  VString h = ""; str_ins( h, 0, "XY" );
                                eqs( __LINE__, "str_ins into \"\"",       h, "XY" );
  VString i = "abcdef"; str_ins_ch( i, 2, '-' );
                                eqs( __LINE__, "str_ins_ch(i,2,'-')",     i, "ab-cdef" );

  gr( "str_replace" );
  VString j = "a.b.c"; str_replace( j, ".", "-" );
                                eqs( __LINE__, "str_replace(.,-)",        j, "a-b-c" );
  VString k = "aaa"; str_replace( k, "a", "bb" );
                                eqs( __LINE__, "replace grows",           k, "bbbbbb" );
  VString l = "hello"; str_replace( l, "zz", "y" );
                                eqs( __LINE__, "replace not found",       l, "hello" );
  VString m = "hello"; str_replace( m, "l", "" );
                                eqs( __LINE__, "replace with \"\"",       m, "heo" );

  gr( "str_mul" );
  VString n = "ab"; str_mul( n, 3 );
                                eqs( __LINE__, "str_mul(\"ab\",3)",       n, "ababab" );
  VString o = "ab"; str_mul( o, 1 );
                                eqs( __LINE__, "str_mul(\"ab\",1)",       o, "ab" );
  VString p = "ab"; str_mul( p, 0 );
                                eqs( __LINE__, "str_mul(\"ab\",0)",       p, "" );
  VString q = "ab"; q *= 2;     eqs( __LINE__, "q *= 2",                  q, "abab" );

  gr( "str_pad -- positive pads left, negative pads right, over-long truncates" );
  VString r = "ab"; str_pad( r, 5 );
                                eqs( __LINE__, "str_pad(\"ab\",5)",       r, "   ab" );
  VString s = "ab"; str_pad( s, -5 );
                                eqs( __LINE__, "str_pad(\"ab\",-5)",      s, "ab   " );
  VString t = "ab"; str_pad( t, 5, '.' );
                                eqs( __LINE__, "str_pad(\"ab\",5,'.')",   t, "...ab" );
  VString u = "abcdef"; str_pad( u, 3 );
                                eqs( __LINE__, "str_pad shorter = cut",   u, "abc" );
  VString v = "ab"; str_pad( v, 2 );
                                eqs( __LINE__, "str_pad to same len",     v, "ab" );
  VString w = ""; str_pad( w, 3, 'x' );
                                eqs( __LINE__, "str_pad(\"\",3,'x')",     w, "xxx" );
}

void t_cut_case()
{
  gr( "str_cut_left / str_cut_right / str_cut / str_cut_spc" );
  VString a = "xxabcxx"; str_cut_left( a, "x" );
                                eqs( __LINE__, "str_cut_left(a,\"x\")",   a, "abcxx" );
  VString b = "xxabcxx"; str_cut_right( b, "x" );
                                eqs( __LINE__, "str_cut_right(b,\"x\")",  b, "xxabc" );
  VString c = "xxabcxx"; str_cut( c, "x" );
                                eqs( __LINE__, "str_cut(c,\"x\")",        c, "abc" );
  VString d = "  abc  "; str_cut_spc( d );
                                eqs( __LINE__, "str_cut_spc(d)",          d, "abc" );
  VString e = "xyxabcxyx"; str_cut( e, "xy" );
                                eqs( __LINE__, "str_cut multi charlist",  e, "abc" );
  VString f = "abc"; str_cut( f, "z" );
                                eqs( __LINE__, "str_cut nothing to cut",  f, "abc" );
  VString g = "xxx"; str_cut( g, "x" );
                                eqs( __LINE__, "str_cut everything",      g, "" );
  VString h = ""; str_cut_spc( h );
                                eqs( __LINE__, "str_cut_spc(\"\")",       h, "" );

  gr( "str_up / str_low / str_flip_case" );
  VString a2 = "aBc1"; str_up( a2 );
                                eqs( __LINE__, "str_up(\"aBc1\")",        a2, "ABC1" );
  VString b2 = "aBc1"; str_low( b2 );
                                eqs( __LINE__, "str_low(\"aBc1\")",       b2, "abc1" );
  VString c2 = "aBc1"; str_flip_case( c2 );
                                eqs( __LINE__, "str_flip_case(\"aBc1\")", c2, "AbC1" );
  eqs( __LINE__, "str_up(const) returns copy",   str_up( "aBc" ),  "ABC" );
  eqs( __LINE__, "str_low(const) returns copy",  str_low( "aBc" ), "abc" );
  VString src = "aBc";
  eqs( __LINE__, "str_up(const) leaves source",  src,              "aBc" );

  gr( "str_tr" );
  VString d2 = "hello"; str_tr( d2, "el", "ip" );
                                eqs( __LINE__, "str_tr(\"el\"->\"ip\")",  d2, "hippo" );
  /* note: str_tr() asserts str_len(from) == str_len(to), so a shorter */
  /* 'to' is API misuse and is not exercised here                       */
  VString f2 = "abc"; str_tr( f2, "z", "y" );
                                eqs( __LINE__, "str_tr no match",         f2, "abc" );

  gr( "str_reverse / str_squeeze / str_comma" );
  VString g2 = "abcde"; str_reverse( g2 );
                                eqs( __LINE__, "str_reverse(\"abcde\")",  g2, "edcba" );
  VString h2 = "ab"; str_reverse( h2 );
                                eqs( __LINE__, "str_reverse(\"ab\")",     h2, "ba" );
  VString i2 = ""; str_reverse( i2 );
                                eqs( __LINE__, "str_reverse(\"\")",       i2, "" );
  VString j2 = "aabbbcc"; str_squeeze( j2, "b" );
                                eqs( __LINE__, "str_squeeze(j,\"b\")",    j2, "aabcc" );
  VString k2 = "aabbbcc"; str_squeeze( k2, "abc" );
                                eqs( __LINE__, "str_squeeze(k,\"abc\")",  k2, "abc" );
  VString l2 = "1234567"; str_comma( l2 );
                                eqs( __LINE__, "str_comma(\"1234567\")",  l2, "1'234'567" );
  VString m2 = "1234567"; str_comma( m2, ',' );
                                eqs( __LINE__, "str_comma(...,',')",      m2, "1,234,567" );
  VString n2 = "123"; str_comma( n2 );
                                eqs( __LINE__, "str_comma(\"123\")",      n2, "123" );
  /* the sign is not a digit: nothing may be inserted in front of it, which
     only shows up when the digit count is a multiple of three */
  VString o2 = "-999999"; str_comma( o2 );
                                eqs( __LINE__, "negative, 6 digits",      o2, "-999'999" );
  VString p2 = "-123"; str_comma( p2 );
                                eqs( __LINE__, "negative, 3 digits",      p2, "-123" );
  VString q2 = "-1234567"; str_comma( q2 );
                                eqs( __LINE__, "negative, 7 digits",      q2, "-1'234'567" );
  VString r2 = "+999999"; str_comma( r2 );
                                eqs( __LINE__, "leading plus",            r2, "+999'999" );
  VString s2 = "-1234.56"; str_comma( s2 );
                                eqs( __LINE__, "negative with a decimal", s2, "-1'234.56" );
  VString t2 = "-1"; str_comma( t2 );
                                eqs( __LINE__, "negative, 1 digit",       t2, "-1" );
}

void t_search()
{
  gr( "str_find / str_rfind -- single character" );
  eqi( __LINE__, "str_find(\"abcabc\",'b')",     str_find(  "abcabc", 'b' ),     1 );
  eqi( __LINE__, "str_find(...,'z')",            str_find(  "abcabc", 'z' ),    -1 );
  eqi( __LINE__, "str_find(...,'b',2)",          str_find(  "abcabc", 'b', 2 ),  4 );
  eqi( __LINE__, "str_rfind(\"abcabc\",'b')",    str_rfind( "abcabc", 'b' ),     4 );
  eqi( __LINE__, "str_rfind(...,'z')",           str_rfind( "abcabc", 'z' ),    -1 );
  eqi( __LINE__, "str_find(\"\",'a')",           str_find(  "", 'a' ),          -1 );

  gr( "str_find / str_rfind -- substring" );
  eqi( __LINE__, "str_find(\"abcabc\",\"bc\")",  str_find(  "abcabc", "bc" ),    1 );
  eqi( __LINE__, "str_find(...,\"zz\")",         str_find(  "abcabc", "zz" ),   -1 );
  eqi( __LINE__, "str_find(...,\"bc\",2)",       str_find(  "abcabc", "bc", 2 ), 4 );
  eqi( __LINE__, "str_rfind(\"abcabc\",\"bc\")", str_rfind( "abcabc", "bc" ),    4 );
  eqi( __LINE__, "str_find(\"abc\",\"\")",       str_find(  "abc", "" ),         0 );
  eqi( __LINE__, "str_find(\"abc\",\"abc\")",    str_find(  "abc", "abc" ),      0 );
  eqi( __LINE__, "str_find(\"abc\",\"abcd\")",   str_find(  "abc", "abcd" ),    -1 );

  gr( "str_count / str_str_count" );
  eqi( __LINE__, "str_count(\"abcabc\",\"a\")",  str_count( "abcabc", "a" ),     2 );
  eqi( __LINE__, "str_count(\"abcabc\",\"ab\")", str_count( "abcabc", "ab" ),    4 );
  eqi( __LINE__, "str_count(\"abc\",\"z\")",     str_count( "abc", "z" ),        0 );
  eqi( __LINE__, "str_str_count(\"aaaa\",\"aa\")", str_str_count( "aaaa", "aa" ), 2 );
  eqi( __LINE__, "str_str_count(\"abcabc\",\"bc\")", str_str_count( "abcabc", "bc" ), 2 );
  eqi( __LINE__, "str_str_count(\"abc\",\"z\")", str_str_count( "abc", "z" ),    0 );

  gr( "str_word / str_rword" );
  char w[64];
  VString a = "one two three";
  str_word( a, " ", w, sizeof(w) );   eqs( __LINE__, "str_word 1st",  w, "one" );
  str_word( a, " ", w, sizeof(w) );   eqs( __LINE__, "str_word 2nd",  w, "two" );
  eqs( __LINE__, "str_word consumes", a, "three" );
  VString b = "one two three";
  str_rword( b, " ", w, sizeof(w) );  eqs( __LINE__, "str_rword 1st", w, "three" );
  eqs( __LINE__, "str_rword consumes", b, "one two" );
}

void t_paths()
{
  gr( "path and filename helpers" );
  eqs( __LINE__, "str_file_ext(\"/a/b.txt\")",      str_file_ext( "/a/b.txt" ),      "txt" );
  eqs( __LINE__, "str_file_ext(\"/a/b\")",          str_file_ext( "/a/b" ),          "" );
  eqs( __LINE__, "str_file_ext(\"a.b.c\")",         str_file_ext( "a.b.c" ),         "c" );
  eqs( __LINE__, "str_file_name(\"/a/b.txt\")",     str_file_name( "/a/b.txt" ),     "b" );
  eqs( __LINE__, "str_file_name_ext(\"/a/b.txt\")", str_file_name_ext( "/a/b.txt" ), "b.txt" );
  eqs( __LINE__, "str_file_path(\"/a/b.txt\")",     str_file_path( "/a/b.txt" ),     "/a/" );
  eqs( __LINE__, "str_file_path(\"b.txt\")",        str_file_path( "b.txt" ),        "" );
  eqs( __LINE__, "str_file_name_ext(\"b.txt\")",    str_file_name_ext( "b.txt" ),    "b.txt" );

  gr( "str_fix_path" );
  VString a = "/a/b";  str_fix_path( a );
                                eqs( __LINE__, "str_fix_path(\"/a/b\")",   a, "/a/b/" );
  VString b = "/a/b/"; str_fix_path( b );
                                eqs( __LINE__, "already has a slash",      b, "/a/b/" );

  gr( "str_dot_reduce" );
  eqs( __LINE__, "fits, unchanged",  str_dot_reduce( "abc", 10 ),          "abc" );
  eqi( __LINE__, "long one is cut",  str_len( str_dot_reduce( "abcdefghijklmn", 8 ) ) <= 8, 1 );

  gr( "hex2long" );
  eqi( __LINE__, "hex2long(\"ff\")",   hex2long( "ff" ),   255 );
  eqi( __LINE__, "hex2long(\"FF\")",   hex2long( "FF" ),   255 );
  eqi( __LINE__, "hex2long(\"0\")",    hex2long( "0" ),      0 );
  eqi( __LINE__, "hex2long(\"10\")",   hex2long( "10" ),    16 );
}

void t_array_trie()
{
  gr( "VArray basics" );
  VArray a;
  eqi( __LINE__, "empty count",        a.count(), 0 );
  a.push( "one" ); a.push( "two" ); a.push( "three" );
  eqi( __LINE__, "count after 3 push", a.count(), 3 );
  eqs( __LINE__, "a.get(0)",           a.get( 0 ), "one" );
  eqs( __LINE__, "a[2]",               a[2],       "three" );
  eqs( __LINE__, "a.get(-1) is NULL",  a.get( -1 ) ? "?" : "(null)", "(null)" );
  eqs( __LINE__, "a.get(99) is NULL",  a.get( 99 ) ? "?" : "(null)", "(null)" );
  a.ins( 1, "mid" );
  eqs( __LINE__, "after ins(1)",       a.get( 1 ), "mid" );
  eqi( __LINE__, "count after ins",    a.count(),  4 );
  a.del( 1 );
  eqs( __LINE__, "after del(1)",       a.get( 1 ), "two" );
  eqi( __LINE__, "count after del",    a.count(),  3 );
  a.set( 0, "ONE" );
  eqs( __LINE__, "after set(0)",       a.get( 0 ), "ONE" );
  eqs( __LINE__, "pop()",              a.pop(),    "three" );
  eqs( __LINE__, "shift()",            a.shift(),  "ONE" );
  a.unshift( "first" );
  eqs( __LINE__, "after unshift",      a.get( 0 ), "first" );
  eqi( __LINE__, "max_len",            a.max_len(), 5 );
  eqi( __LINE__, "min_len",            a.min_len(), 3 );
  a.reverse();
  eqs( __LINE__, "after reverse",      a.get( 0 ), "two" );
  a.undef();
  eqi( __LINE__, "after undef",        a.count(),  0 );

  gr( "VArray sort" );
  VArray b;
  b.push( "pear" ); b.push( "apple" ); b.push( "fig" );
  b.sort();
  eqs( __LINE__, "sort()[0]",          b.get( 0 ), "apple" );
  eqs( __LINE__, "sort()[2]",          b.get( 2 ), "pear" );
  b.sort( 1 );
  eqs( __LINE__, "sort(rev)[0]",       b.get( 0 ), "pear" );

  gr( "str_split / str_split_simple / str_join" );
  VArray c = str_split_simple( ",", "a,b,c" );
  eqi( __LINE__, "split_simple count", c.count(), 3 );
  eqs( __LINE__, "split_simple[1]",    c.get( 1 ), "b" );
  eqs( __LINE__, "str_join(c,\"-\")",  str_join( c, "-" ), "a-b-c" );
  eqs( __LINE__, "str_join(c)",        str_join( c ),      "abc" );
  VArray d = str_split_simple( ",", "" );
  eqi( __LINE__, "split of \"\"",      d.count() <= 1, 1 );
  VArray e = str_split_simple( ",", "abc" );
  eqi( __LINE__, "split no delimiter", e.count(), 1 );
  eqs( __LINE__, "split no delim [0]", e.get( 0 ), "abc" );
  gr( "str_split_simple -- trailing empty fields are dropped, like perl's split" );
  eqi( __LINE__, "split(\"a,\") count",     str_split_simple( ",", "a," ).count(),    1 );
  eqi( __LINE__, "split(\"a,b,\") count",   str_split_simple( ",", "a,b," ).count(),  2 );
  eqi( __LINE__, "split(\",a\") keeps head", str_split_simple( ",", ",a" ).count(),   2 );
  eqi( __LINE__, "split(\"a,,b\") keeps mid",str_split_simple( ",", "a,,b" ).count(), 3 );
  eqi( __LINE__, "split(\",,\") count",     str_split_simple( ",", ",," ).count(),    2 );

  VArray f = str_split( "\\s+", "a  b   c" );
  eqi( __LINE__, "regexp split count", f.count(), 3 );
  eqs( __LINE__, "regexp split [2]",   f.get( 2 ), "c" );

  gr( "VTrie basics" );
  VTrie t;
  eqi( __LINE__, "empty exists()",     t.exists( "k" ) != 0, 0 );
  t.set( "key", "val" );
  eqi( __LINE__, "exists after set",   t.exists( "key" ) != 0, 1 );
  eqs( __LINE__, "get(\"key\")",       t.get( "key" ),  "val" );
  t.set( "key", "val2" );
  eqs( __LINE__, "overwrite",          t.get( "key" ),  "val2" );
  t.set( "k2", "v2" );
  eqi( __LINE__, "count()",            t.count(), 2 );
  VArray k = t.keys();
  eqi( __LINE__, "keys() count",       k.count(), 2 );
  t.del( "key" );
  eqi( __LINE__, "exists after del",   t.exists( "key" ) != 0, 0 );
  t.undef();
  eqi( __LINE__, "count after undef",  t.count(), 0 );
}

void t_interop()
{
  gr( "VString <-> WString -- construction" );
  WString w1( "ascii text" );
  eqw( __LINE__, "WString(char*)",          w1, L"ascii text" );
  VString v1( L"ascii text" );
  eqs( __LINE__, "VString(wchar_t*)",       v1, "ascii text" );

  gr( "VString <-> WString -- assignment" );
  WString w2; w2 = "from narrow";
  eqw( __LINE__, "WString = char*",         w2, L"from narrow" );
  VString v2; v2 = L"from wide";
  eqs( __LINE__, "VString = wchar_t*",      v2, "from wide" );
  WString w3; w3.set( "via set()" );
  eqw( __LINE__, "WString.set(char*)",      w3, L"via set()" );
  VString v3; v3.set( L"via set()" );
  eqs( __LINE__, "VString.set(wchar_t*)",   v3, "via set()" );

  gr( "VString <-> WString -- round trip" );
  VString a = "round trip 123 !@#";
  WString b( a.data() );
  VString c( b.data() );
  eqs( __LINE__, "narrow->wide->narrow",    c, a );
  WString d = L"wide round trip";
  VString e( d.data() );
  WString f( e.data() );
  eqw( __LINE__, "wide->narrow->wide",      f, d );

  gr( "VString <-> WString -- empty and length" );
  VString g = "";
  WString h( g.data() );
  eqi( __LINE__, "empty narrow->wide len",  str_len( h ), 0 );
  WString i = L"";
  VString j( i.data() );
  eqi( __LINE__, "empty wide->narrow len",  str_len( j ), 0 );

  gr( "the same str_*() call on both types must agree (ASCII)" );
  VString n = "Hello World";
  WString m = L"Hello World";
  str_up( n ); str_up( m );
  eqs( __LINE__, "str_up narrow",           n, "HELLO WORLD" );
  eqw( __LINE__, "str_up wide",             m, L"HELLO WORLD" );
  VString n2 = "abcdef";  WString m2 = L"abcdef";
  str_reverse( n2 ); str_reverse( m2 );
  eqs( __LINE__, "str_reverse narrow",      n2, "fedcba" );
  eqw( __LINE__, "str_reverse wide",        m2, L"fedcba" );
  eqi( __LINE__, "str_find narrow",         str_find( "abcabc", "bc" ),   1 );
  eqi( __LINE__, "str_find wide",           str_find( L"abcabc", L"bc" ), 1 );
  eqi( __LINE__, "str_len narrow",          str_len( n2 ), 6 );
  eqi( __LINE__, "str_len wide",            str_len( m2 ), 6 );
}

/****************************************************************************
** part 3 -- unicode / multibyte
**
** the narrow VString is a string of bytes and every str_*() on it counts and
** cuts bytes; character semantics are what WString is for. the groups below
** pin down both halves, and report the places where the byte oriented half
** produces output that is no longer valid UTF-8.
****************************************************************************/

/* UTF-8 sources used throughout: */
#define U_AE  "\xc3\xa4"                  /* U+00E4                  2 bytes */
#define U_BG  "\xd0\xb4\xd0\xb0"          /* U+0434 U+0430           4 bytes */
#define U_EM  "\xf0\x9f\x98\x80"          /* U+1F600 emoji           4 bytes */
#define U_MIX "a" U_AE "b" U_BG "c"

static int valid_utf8( const char* s )
{
  const unsigned char* p = (const unsigned char*)s;
  while( *p )
    {
    int n;
    if     ( ( *p & 0x80 ) == 0x00 ) n = 0;
    else if( ( *p & 0xE0 ) == 0xC0 ) n = 1;
    else if( ( *p & 0xF0 ) == 0xE0 ) n = 2;
    else if( ( *p & 0xF8 ) == 0xF0 ) n = 3;
    else return 0;
    p++;
    while( n-- > 0 )
      if( ( *p++ & 0xC0 ) != 0x80 ) return 0;
    }
  return 1;
}

void t_unicode_narrow()
{
  gr( "unicode -- narrow VString counts bytes, not characters (by design)" );
  VString a = U_AE;   eqi( __LINE__, "str_len(\"ae\" utf8) = bytes",  str_len( a ), 2 );
  VString b = U_BG;   eqi( __LINE__, "str_len(\"da\" utf8) = bytes",  str_len( b ), 4 );
  VString c = U_EM;   eqi( __LINE__, "str_len(emoji) = bytes",        str_len( c ), 4 );
  VString d = U_MIX;  eqi( __LINE__, "str_len(mixed) = bytes",        str_len( d ), 9 );

  gr( "unicode -- narrow storage is byte transparent (nothing is mangled)" );
  VString e = U_MIX;
  eqs( __LINE__, "utf8 survives assignment",      e, U_MIX );
  eqi( __LINE__, "and is still valid utf8",       valid_utf8( e ), 1 );
  VString f = VString( U_AE ) + U_BG;
  eqs( __LINE__, "utf8 survives concatenation",   f, U_AE U_BG );
  eqi( __LINE__, "and is still valid utf8",       valid_utf8( f ), 1 );
  VString g = U_MIX;
  eqi( __LINE__, "str_find finds a utf8 needle",  str_find( g, U_BG ), 4 );
  eqi( __LINE__, "str_count over utf8",           str_count( g, "a" ), 1 );
  VString h = U_MIX; str_replace( h, U_BG, "X" );
  eqs( __LINE__, "str_replace of a utf8 needle",  h, "a" U_AE "bXc" );
  eqi( __LINE__, "and is still valid utf8",       valid_utf8( h ), 1 );

  gr( "unicode -- KNOWN PROBLEM: byte operations split characters" );
  VString i = U_AE; str_reverse( i );
  nfoi( __LINE__, "valid_utf8(str_reverse(utf8))", valid_utf8( i ),
        "0 = broken; str_reverse walks bytes, use WString" );
  VString j = U_BG; str_sleft( j, 1 );
  nfoi( __LINE__, "valid_utf8(str_sleft(utf8,1))", valid_utf8( j ),
        "0 = broken; cuts inside a character, use WString" );
  VString k; str_left( k, U_BG, 3 );
  nfoi( __LINE__, "valid_utf8(str_left(utf8,3))",  valid_utf8( k ),
        "0 = broken; cuts inside a character, use WString" );
  VString l = U_MIX;
  nfoi( __LINE__, "str_len(mixed) vs 6 characters", str_len( l ),
        "bytes, not characters -- 6 characters here" );
  VString m = U_AE; str_up( m );
  nfo(  __LINE__, "str_up(utf8) narrow",           m,
        "non-ASCII left alone; WString uppercases it" );
  VString n = U_AE; str_pad( n, 4, '.' );
  nfoi( __LINE__, "str_len(str_pad(utf8,4))",      str_len( n ),
        "pads to a byte width, not a display width" );
}

void t_unicode_wide()
{
  gr( "unicode -- wide WString counts characters" );
  WString a = U_AE;   eqi( __LINE__, "str_len(\"ae\") = 1 char",   str_len( a ), 1 );
  WString b = U_BG;   eqi( __LINE__, "str_len(\"da\") = 2 chars",  str_len( b ), 2 );
  WString c = U_EM;   eqi( __LINE__, "str_len(emoji) = 1 char",    str_len( c ), 1 );
  WString d = U_MIX;  eqi( __LINE__, "str_len(mixed) = 6 chars",   str_len( d ), 6 );
  eqi( __LINE__, "emoji code point kept whole",    (long)c[0], 0x1F600 );

  gr( "unicode -- wide operations respect character boundaries" );
  WString e = U_BG; str_reverse( e );
  eqw( __LINE__, "str_reverse wide",               e, L"\x430\x434" );
  eqi( __LINE__, "reverse -> valid utf8",          valid_utf8( VString( e.data() ) ), 1 );
  WString f = U_BG; str_sleft( f, 1 );
  eqw( __LINE__, "str_sleft(wide,1)",              f, L"\x434" );
  eqi( __LINE__, "sleft -> valid utf8",            valid_utf8( VString( f.data() ) ), 1 );
  WString g = U_AE; str_up( g );
  eqw( __LINE__, "str_up(\"ae\") wide",            g, L"\xc4" );
  WString h = U_BG; str_up( h );
  eqw( __LINE__, "str_up(\"da\") wide",            h, L"\x414\x410" );
  WString i = L"\x414\x410"; str_low( i );
  eqw( __LINE__, "str_low wide",                   i, L"\x434\x430" );
  WString j = U_MIX; str_sleft( j, 2 );
  eqi( __LINE__, "sleft(mixed,2) -> valid utf8",   valid_utf8( VString( j.data() ) ), 1 );
  eqi( __LINE__, "sleft(mixed,2) length",          str_len( j ), 2 );

  gr( "unicode -- narrow/wide round trip keeps the bytes identical" );
  const char* src[] = { U_AE, U_BG, U_EM, U_MIX, "plain ascii", "", NULL };
  for( int z = 0; src[z]; z++ )
    {
    WString w( src[z] );
    VString v( w.data() );
    eqs( __LINE__, "utf8 -> WString -> VString",    v, src[z] );
    }

  gr( "unicode -- invalid UTF-8 input is counted, not fatal" );
  WString k;
  eqi( __LINE__, "set_failsafe(\"\\xc3\\x28\") errors", k.set_failsafe( "\xc3\x28" ), 1 );
  eqi( __LINE__, "and still produced output",      str_len( k ) > 0, 1 );
  WString l;
  eqi( __LINE__, "set_failsafe(\"\\x80\\x80\") errors", l.set_failsafe( "\x80\x80" ), 2 );
  WString m;
  eqi( __LINE__, "set_failsafe(valid) = 0 errors", m.set_failsafe( U_MIX ), 0 );
  eqi( __LINE__, "and decoded all 6 characters",   str_len( m ), 6 );
}

/****************************************************************************
** part 2 -- exhaustive sweeps
**
** three kinds of check, each over every combination in a bounded domain:
**   a) the VS_CHAR* form and the VString form of a function must agree
**   b) VString and WString must agree on ASCII input
**   c) invariants that hold whatever the implementation does
****************************************************************************/

long sweep_run  = 0;
long sweep_fail = 0;

static void sw( int line, int ok, const char* what, const char* detail )
{
  sweep_run++;
  if( ok ) return;
  sweep_fail++;
  if( sweep_fail <= 25 )
    printf( "  %4d  FAIL  %-30s %s\n", line, what, detail );
}

static VString detail2( const char* a, const char* b )
{
  return VString( "[" ) + a + "] vs [" + b + "]";
}

#define SWEEP_SRC "abcdef"
#define SWEEP_LEN 6
#define LO (-(SWEEP_LEN)-2)
#define HI ( (SWEEP_LEN)+2)

/* argument contracts, so the sweeps stay inside them:                      */
/*   str_copy/str_left/str_right  ASSERT( len >= -1 ), -1 meaning "all"     */
/*   str_del                      negative len reads before the buffer      */
/*   str_tr                       ASSERT( str_len(from) == str_len(to) )    */
/* everything else below guards its own arguments and is swept in full.     */
#define LO_CPY (-1)
#define LO_DEL ( 0)
#define LO_SLF LO

/* str_sleft() and str_sright() used to corrupt memory for a negative len:

     str_sleft( VString&, negative )
       tested only  if( len < target.box->sl )  and then did
       target.box->s[len] = 0, writing before the start of the buffer

     str_sright( VS_CHAR*, <= -2 )
       vs_memmove( target, target + (sl - len), len + 1 ) -- source past the
       end of the string and a count that wraps when taken as a size;
       len == -1 wrote target[-1]

   both now carry the  && len >= 0  guard the VS_CHAR* str_sleft() always
   had, so a negative len leaves the target alone, as str_mul(), str_ins()
   and str_trim_*() already did.  the sweeps below cover the whole range. */

void sweep_char_vs_vstring()
{
  gr( "sweep: VS_CHAR* form vs VString form must agree" );

  for( int n = LO_CPY; n <= HI; n++ )
    {
    VString v;
    str_left( v, SWEEP_SRC, n );
    sw( __LINE__, v == str_left( buf( "" ), SWEEP_SRC, n ),
        "str_left", detail2( v, buf( SWEEP_SRC ) ) );

    str_right( v, SWEEP_SRC, n );
    sw( __LINE__, v == str_right( buf( "" ), SWEEP_SRC, n ),
        "str_right", detail2( v, SWEEP_SRC ) );
    }

  for( int n = LO_SLF; n <= HI; n++ )
    {
    VString a = SWEEP_SRC; str_sleft( a, n );
    char* b = str_sleft( buf( SWEEP_SRC ), n );
    sw( __LINE__, a == b, "str_sleft", detail2( a, b ) );

    VString c = SWEEP_SRC; str_sright( c, n );
    char* d = str_sright( buf( SWEEP_SRC ), n );
    sw( __LINE__, c == d, "str_sright", detail2( c, d ) );
    }

  for( int n = LO; n <= HI; n++ )
    {
    VString e = SWEEP_SRC; str_trim_left( e, n );
    char* f = str_trim_left( buf( SWEEP_SRC ), n );
    sw( __LINE__, e == f, "str_trim_left", detail2( e, f ) );

    VString g = SWEEP_SRC; str_trim_right( g, n );
    char* h = str_trim_right( buf( SWEEP_SRC ), n );
    sw( __LINE__, g == h, "str_trim_right", detail2( g, h ) );

    VString i = SWEEP_SRC; str_pad( i, n, '.' );
    char* j = str_pad( buf( SWEEP_SRC ), n, '.' );
    sw( __LINE__, i == j, "str_pad", detail2( i, j ) );
    }

  for( int pos = LO; pos <= HI; pos++ )
    {
    for( int len = LO_CPY; len <= HI; len++ )
      {
      VString v;
      str_copy( v, SWEEP_SRC, pos, len );
      char* c = str_copy( buf( "" ), SWEEP_SRC, pos, len );
      sw( __LINE__, v == c, "str_copy", detail2( v, c ) );
      }
    for( int len = LO_DEL; len <= HI; len++ )
      {
      VString a = SWEEP_SRC; str_del( a, pos, len );
      char* b = str_del( buf( SWEEP_SRC ), pos, len );
      sw( __LINE__, a == b, "str_del", detail2( a, b ) );
      }
    }

  for( int pos = LO; pos <= HI; pos++ )
    {
    VString a = SWEEP_SRC; str_ins( a, pos, "XY" );
    char* b = str_ins( buf( SWEEP_SRC ), pos, "XY" );
    sw( __LINE__, a == b, "str_ins", detail2( a, b ) );

    VString c = SWEEP_SRC; str_ins_ch( c, pos, 'Z' );
    char* d = str_ins_ch( buf( SWEEP_SRC ), pos, 'Z' );
    sw( __LINE__, c == d, "str_ins_ch", detail2( c, d ) );
    }
}

void sweep_narrow_vs_wide()
{
  gr( "sweep: VString vs WString must agree on ASCII" );

  for( int n = LO; n <= HI; n++ )
    {
    VString a = SWEEP_SRC;  WString A = L"" SWEEP_SRC;
    str_sleft( a, n ); str_sleft( A, n );
    sw( __LINE__, a == VString( A.data() ), "str_sleft", detail2( a, VString( A.data() ) ) );

    VString b = SWEEP_SRC;  WString B = L"" SWEEP_SRC;
    str_sright( b, n ); str_sright( B, n );
    sw( __LINE__, b == VString( B.data() ), "str_sright", detail2( b, VString( B.data() ) ) );

    VString c = SWEEP_SRC;  WString C = L"" SWEEP_SRC;
    str_trim_left( c, n ); str_trim_left( C, n );
    sw( __LINE__, c == VString( C.data() ), "str_trim_left", detail2( c, VString( C.data() ) ) );

    VString d = SWEEP_SRC;  WString D = L"" SWEEP_SRC;
    str_trim_right( d, n ); str_trim_right( D, n );
    sw( __LINE__, d == VString( D.data() ), "str_trim_right", detail2( d, VString( D.data() ) ) );

    VString e = SWEEP_SRC;  WString E = L"" SWEEP_SRC;
    str_pad( e, n, '.' ); str_pad( E, n, L'.' );
    sw( __LINE__, e == VString( E.data() ), "str_pad", detail2( e, VString( E.data() ) ) );

    VString f = SWEEP_SRC;  WString F = L"" SWEEP_SRC;
    str_mul( f, n ); str_mul( F, n );
    sw( __LINE__, f == VString( F.data() ), "str_mul", detail2( f, VString( F.data() ) ) );
    }

  for( int pos = LO; pos <= HI; pos++ )
    {
    for( int len = LO_CPY; len <= HI; len++ )
      {
      VString a; WString A;
      str_copy( a, SWEEP_SRC, pos, len );
      str_copy( A, L"" SWEEP_SRC, pos, len );
      sw( __LINE__, a == VString( A.data() ), "str_copy", detail2( a, VString( A.data() ) ) );
      }
    for( int len = LO_DEL; len <= HI; len++ )
      {
      VString b = SWEEP_SRC; WString B = L"" SWEEP_SRC;
      str_del( b, pos, len ); str_del( B, pos, len );
      sw( __LINE__, b == VString( B.data() ), "str_del", detail2( b, VString( B.data() ) ) );
      }
    }

  for( int pos = LO; pos <= HI; pos++ )
    {
    VString a = SWEEP_SRC; WString A = L"" SWEEP_SRC;
    str_ins( a, pos, "XY" ); str_ins( A, pos, L"XY" );
    sw( __LINE__, a == VString( A.data() ), "str_ins", detail2( a, VString( A.data() ) ) );
    }

  /* every str_find/str_count answer must be the same on both types */
  static const char*    ns[] = { "", "a", "abcabc", "aaaa", "abc", NULL };
  static const wchar_t* ws[] = { L"", L"a", L"abcabc", L"aaaa", L"abc", NULL };
  static const char*    nn[] = { "", "a", "bc", "abc", "z", NULL };
  static const wchar_t* wn[] = { L"", L"a", L"bc", L"abc", L"z", NULL };
  for( int h = 0; ns[h]; h++ )
    for( int e = 0; nn[e]; e++ )
      {
      sw( __LINE__, str_find( ns[h], nn[e] ) == str_find( ws[h], wn[e] ),
          "str_find", detail2( ns[h], nn[e] ) );
      sw( __LINE__, str_rfind( ns[h], nn[e] ) == str_rfind( ws[h], wn[e] ),
          "str_rfind", detail2( ns[h], nn[e] ) );
      sw( __LINE__, str_count( ns[h], nn[e] ) == str_count( ws[h], wn[e] ),
          "str_count", detail2( ns[h], nn[e] ) );
      sw( __LINE__, str_str_count( ns[h], nn[e] ) == str_str_count( ws[h], wn[e] ),
          "str_str_count", detail2( ns[h], nn[e] ) );
      }
}

void t_negative_len()
{
  gr( "negative len leaves the target alone (was a buffer overrun)" );
  VString a = "abcdef"; str_sleft( a, -1 );
  eqs( __LINE__, "str_sleft(VString,-1)",   a, "abcdef" );
  VString b = "abcdef"; str_sleft( b, -99 );
  eqs( __LINE__, "str_sleft(VString,-99)",  b, "abcdef" );
  char c[64]; strncpyz_buf( c, "abcdef" ); str_sleft( c, -1 );
  eqs( __LINE__, "str_sleft(char*,-1)",     c, "abcdef" );
  VString d = "abcdef"; str_sright( d, -1 );
  eqs( __LINE__, "str_sright(VString,-1)",  d, "abcdef" );
  VString e = "abcdef"; str_sright( e, -2 );
  eqs( __LINE__, "str_sright(VString,-2)",  e, "abcdef" );
  char f[64]; strncpyz_buf( f, "abcdef" ); str_sright( f, -2 );
  eqs( __LINE__, "str_sright(char*,-2)",    f, "abcdef" );
  char g[64]; strncpyz_buf( g, "abcdef" ); str_sright( g, -99 );
  eqs( __LINE__, "str_sright(char*,-99)",   g, "abcdef" );
  /* len == 0 still empties, that is unchanged */
  VString h = "abcdef"; str_sleft( h, 0 );
  eqs( __LINE__, "str_sleft(VString,0)",    h, "" );
  VString i = "abcdef"; str_sright( i, 0 );
  eqs( __LINE__, "str_sright(VString,0)",   i, "" );
}

/* every mutating str_*() must detach first: changing a copy must never be
   visible through the string it was copied from */
void sweep_cow()
{
  gr( "sweep: mutating a copy must not touch the original" );
  const char* SRC = "abcdef";

  #define COW( name, code )                                     \
    {                                                           \
    VString a = SRC;                                            \
    VString b = a;                                              \
    { VString& t = b; code; }                                   \
    sw( __LINE__, a == SRC, name, detail2( a, SRC ) );          \
    }

  COW( "str_add_ch",     str_add_ch( t, 'X' ) )
  COW( "str_add_ch_range", str_add_ch_range( t, 'x', 'z' ) )
  COW( "str_set_ch",     str_set_ch( t, 0, 'X' ) )
  COW( "str_ins",        str_ins( t, 0, "X" ) )
  COW( "str_ins_ch",     str_ins_ch( t, 0, 'X' ) )
  COW( "str_del",        str_del( t, 0, 2 ) )
  COW( "str_replace",    str_replace( t, "a", "X" ) )
  COW( "str_sleft",      str_sleft( t, 2 ) )
  COW( "str_sright",     str_sright( t, 2 ) )
  COW( "str_trim_left",  str_trim_left( t, 2 ) )
  COW( "str_trim_right", str_trim_right( t, 2 ) )
  COW( "str_cut_left",   str_cut_left( t, "a" ) )
  COW( "str_cut_right",  str_cut_right( t, "f" ) )
  COW( "str_cut",        str_cut( t, "af" ) )
  COW( "str_cut_spc",    str_cut_spc( t ) )
  COW( "str_pad",        str_pad( t, 10, '.' ) )
  COW( "str_comma",      str_comma( t ) )
  COW( "str_mul",        str_mul( t, 2 ) )
  COW( "str_tr",         str_tr( t, "a", "X" ) )
  COW( "str_up",         str_up( t ) )
  COW( "str_low",        str_low( t ) )
  COW( "str_flip_case",  str_flip_case( t ) )
  COW( "str_reverse",    str_reverse( t ) )
  COW( "str_squeeze",    str_squeeze( t, "a" ) )
  COW( "str_chop",       str_chop( t ) )
  COW( "operator +=",    t += "X" )
  COW( "operator []",    t[0] = 'X' )
  COW( "cat()",          t.cat( "X" ) )
  COW( "set()",          t.set( "X" ) )
  COW( "setn()",         t.setn( "XY", 1 ) )
  COW( "catn()",         t.catn( "XY", 1 ) )
  #undef COW

  /* and the same through a VArray element, which shares the box too */
  VString c = "abcdef";
  VArray  d;
  d.push( c );
  str_add_ch( c, 'X' );
  sw( __LINE__, VString( d.get( 0 ) ) == "abcdef", "array element after push",
      d.get( 0 ) );
}

void sweep_invariants()
{
  gr( "sweep: invariants" );

  /* left(n) + right(len-n) == the whole string, for every split point */
  for( int n = 0; n <= SWEEP_LEN; n++ )
    {
    VString l, r;
    str_left( l, SWEEP_SRC, n );
    str_right( r, SWEEP_SRC, SWEEP_LEN - n );
    sw( __LINE__, ( l + r ) == SWEEP_SRC, "left(n)+right(len-n)", l + r );
    }

  /* copy(0,n) == left(n) ; copy(len-n,n) == right(n) */
  for( int n = 0; n <= SWEEP_LEN; n++ )
    {
    VString a, b;
    str_copy( a, SWEEP_SRC, 0, n );
    str_left( b, SWEEP_SRC, n );
    sw( __LINE__, a == b, "copy(0,n) == left(n)", detail2( a, b ) );
    str_copy( a, SWEEP_SRC, SWEEP_LEN - n, n );
    str_right( b, SWEEP_SRC, n );
    sw( __LINE__, a == b, "copy(len-n,n) == right(n)", detail2( a, b ) );
    }

  /* insert then delete the same span gets the original back */
  for( int pos = 0; pos <= SWEEP_LEN; pos++ )
    {
    VString a = SWEEP_SRC;
    str_ins( a, pos, "XY" );
    str_del( a, pos, 2 );
    sw( __LINE__, a == SWEEP_SRC, "ins then del is identity", a );
    }

  /* trim_left(n) == sright(len-n) and trim_right(n) == sleft(len-n) */
  for( int n = 0; n <= SWEEP_LEN; n++ )
    {
    VString a = SWEEP_SRC; str_trim_left( a, n );
    VString b = SWEEP_SRC; str_sright( b, SWEEP_LEN - n );
    sw( __LINE__, a == b, "trim_left == sright", detail2( a, b ) );
    VString c = SWEEP_SRC; str_trim_right( c, n );
    VString d = SWEEP_SRC; str_sleft( d, SWEEP_LEN - n );
    sw( __LINE__, c == d, "trim_right == sleft", detail2( c, d ) );
    }

  /* reverse twice is identity, for every prefix of the sample */
  for( int n = 0; n <= SWEEP_LEN; n++ )
    {
    VString a; str_left( a, SWEEP_SRC, n );
    VString b = a;
    str_reverse( b ); str_reverse( b );
    sw( __LINE__, a == b, "reverse(reverse(s)) == s", detail2( a, b ) );
    }

  /* case folding is stable */
  static const char* cs[] = { "", "a", "A", "aBc", "ABC", "abc123", "!@#", NULL };
  for( int z = 0; cs[z]; z++ )
    {
    VString a = cs[z]; str_low( a ); str_up( a );
    VString b = cs[z]; str_up( b );
    sw( __LINE__, a == b, "up(low(s)) == up(s)", detail2( a, b ) );
    VString c = cs[z]; str_flip_case( c ); str_flip_case( c );
    sw( __LINE__, c == cs[z], "flip(flip(s)) == s", c );
    }

  /* split then join with the same glue gets the original back */
  /* trailing empty fields are dropped (as perl's split does), so the round
     trip only holds for sources that do not end in the delimiter */
  static const char* js[] = { "a,b,c", "a", "", "a,,b", ",a", NULL };
  for( int z = 0; js[z]; z++ )
    {
    VArray v = str_split_simple( ",", js[z] );
    sw( __LINE__, str_join( v, "," ) == js[z], "join(split(s)) == s",
        detail2( str_join( v, "," ), js[z] ) );
    }

  /* narrow -> wide -> narrow is the identity, utf8 included */
  static const char* rs[] = { "", "a", "abc", U_AE, U_BG, U_EM, U_MIX,
                              "line\twith\ttabs", "0123456789", NULL };
  for( int z = 0; rs[z]; z++ )
    {
    WString w( rs[z] );
    VString v( w.data() );
    sw( __LINE__, v == rs[z], "narrow->wide->narrow", detail2( v, rs[z] ) );
    }
}

/****************************************************************************/

int main( int argc, char** argv )
{
  setlocale( LC_ALL, "" );
  for( int z = 1; z < argc; z++ )
    if( strcmp( argv[z], "-q" ) == 0 ) quiet = 1;

  printf( "=========================================================================\n" );
  printf( " VString / WString -- part 1: named cases\n" );
  printf( "=========================================================================\n" );
  t_construct();
  t_numbers();
  t_concat_compare();
  t_index();
  t_slice();
  t_edit();
  t_cut_case();
  t_search();
  t_paths();
  t_array_trie();
  t_interop();

  printf( "\n=========================================================================\n" );
  printf( " VString / WString -- part 3: unicode\n" );
  printf( "=========================================================================\n" );
  t_unicode_narrow();
  t_unicode_wide();
  t_negative_len();

  int named_run    = tests_run;
  int named_failed = tests_failed;

  printf( "\n=========================================================================\n" );
  printf( " VString / WString -- part 2: exhaustive sweeps\n" );
  printf( "=========================================================================\n" );
  sweep_char_vs_vstring();
  sweep_narrow_vs_wide();
  sweep_cow();
  sweep_invariants();
  if( sweep_fail > 25 )
    printf( "  ... and %ld more\n", sweep_fail - 25 );

  printf( "\n=========================================================================\n" );
  printf( " named cases : %d run, %d failed\n", named_run, named_failed );
  if( named_failed )
    {
    printf( " failed lines:%s\n", failed_lines.data() );
    printf( " debug with  : gdb --args ./test_vstring   then  break test_vstring.cpp:LINE\n" );
    }
  printf( " sweeps      : %ld run, %ld failed\n", sweep_run, sweep_fail );
  printf( "=========================================================================\n" );

  return ( named_failed || sweep_fail ) ? 1 : 0;
}

/****************************************************************************
**
** EOF
**
****************************************************************************/
