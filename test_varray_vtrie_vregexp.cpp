/****************************************************************************
 #
 #  VSTRING Library
 #
 #  test suite for VArray / VTrie / VRegexp and their wide counterparts
 #  WArray / WTrie / WRegexp, plus the interoperability between them
 #
 #  build:
 #    g++ -O0 -ggdb3 -I. -o test_varray_vtrie_vregexp \
 #        test_varray_vtrie_vregexp.cpp libvstring.a -lpcre2-8 -lpcre2-32
 #  build with memory checking:
 #    g++ -O0 -ggdb3 -fsanitize=address -I. -o test_varray_vtrie_vregexp \
 #        test_varray_vtrie_vregexp.cpp vstring.cpp wstring.cpp vstrlib.cpp \
 #        wstrlib.cpp vstruti.cpp vref.cpp -lpcre2-8 -lpcre2-32
 #
 #  part 1 -- named cases, each carrying its own __LINE__ so a failure can be
 #            broken on directly:  break test_varray_vtrie_vregexp.cpp:LINE
 #  part 2 -- interoperability: array <-> trie, narrow <-> wide, regexp -> array
 #  part 3 -- exhaustive sweeps over bounded domains
 #
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <unistd.h>

#include "vstring.h"
#include "wstring.h"
#include "vstrlib.h"
#include "wstrlib.h"
#include "vstruti.h"

/****************************************************************************
** test bookkeeping
****************************************************************************/

/* nothing is gated any more -- kept as the switch for the next one */
#define TEST_KNOWN_CRASHERS 0

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
  printf( "  %4d  %-4s  %-36s got %-24s want %s\n",
          line, ok ? "ok" : "FAIL", what, got, want );
}

void eqs( int line, const char* what, const char* got, const char* want )
{
  VString g = VString( "[" ) + ( got  ? got  : "(null)" ) + "]";
  VString w = VString( "[" ) + ( want ? want : "(null)" ) + "]";
  pass_fail( line, got && want && strcmp( got, want ) == 0, what, g, w );
}

void eqi( int line, const char* what, long got, long want )
{
  pass_fail( line, got == want, what, VString( (long)got ), VString( (long)want ) );
}

void eqw( int line, const char* what, const wchar_t* got, const wchar_t* want )
{
  VString g = VString( "[" ) + VString( got  ? got  : L"(null)" ) + "]";
  VString w = VString( "[" ) + VString( want ? want : L"(null)" ) + "]";
  pass_fail( line, got && want && wcscmp( got, want ) == 0, what, g, w );
}

/* an array is compared as its elements joined with '|' */
void eqa( int line, const char* what, VArray& got, const char* want )
{
  VString g = str_join( got, "|" );
  eqs( line, what, g, want );
}

void eqwa( int line, const char* what, WArray& got, const char* want )
{
  VString g = VString( str_join( got, L"|" ).data() );
  eqs( line, what, g, want );
}

/* a trie is compared as its keys (sorted) joined with '|', or key=value */
void eqt( int line, const char* what, VTrie& got, const char* want )
{
  VArray k, v;
  got.keys_and_values( &k, &v );
  VArray kv;
  for( int z = 0; z < k.count(); z++ )
    kv.push( VString( k.get( z ) ) + "=" + v.get( z ) );
  kv.sort();
  VString g = str_join( kv, "|" );
  eqs( line, what, g, want );
}

void eqwt( int line, const char* what, WTrie& got, const char* want )
{
  WArray k, v;
  got.keys_and_values( &k, &v );
  VArray kv;
  for( int z = 0; z < k.count(); z++ )
    kv.push( VString( WString( k.get( z ) ).data() ) + "=" + VString( WString( v.get( z ) ).data() ) );
  kv.sort();
  VString g = str_join( kv, "|" );
  eqs( line, what, g, want );
}

void nfo( int line, const char* what, const char* got, const char* note )
{
  printf( "  %4d  --    %-36s got %-24s %s\n",
          line, what, ( VString( "[" ) + ( got ? got : "(null)" ) + "]" ).data(),
          note ? note : "" );
}

void gr( const char* name )
{
  printf( "\n--- %s\n", name );
}

static VArray mk( const char* csv )   /* build an array from "a,b,c" */
{
  return str_len( csv ) ? str_split_simple( ",", csv ) : VArray();
}

/****************************************************************************
** part 1a -- VArray
****************************************************************************/

void t_array_basics()
{
  gr( "VArray -- construction and count" );
  VArray a;
  eqi( __LINE__, "empty count()",            a.count(), 0 );
  eqa( __LINE__, "empty joins to \"\"",      a, "" );
  a.push( "one" );
  eqi( __LINE__, "count after 1 push",       a.count(), 1 );
  a.push( "two" ); a.push( "three" );
  eqa( __LINE__, "after 3 pushes",           a, "one|two|three" );
  VArray b( a );
  eqa( __LINE__, "copy constructor",         b, "one|two|three" );
  VArray c = a;
  eqa( __LINE__, "assignment",               c, "one|two|three" );
  a.undef();
  eqi( __LINE__, "count after undef()",      a.count(), 0 );
  eqa( __LINE__, "the copy is untouched",    c, "one|two|three" );

  gr( "VArray -- copy on write" );
  VArray d = mk( "x,y" );
  VArray e = d;
  e.push( "z" );
  eqa( __LINE__, "original after copy+push", d, "x|y" );
  eqa( __LINE__, "copy after copy+push",     e, "x|y|z" );
  e.set( 0, "CHANGED" );
  eqa( __LINE__, "original after copy+set",  d, "x|y" );
  e.del( 0 );
  eqa( __LINE__, "original after copy+del",  d, "x|y" );

  gr( "VArray -- get / set / ins / del" );
  VArray f = mk( "a,b,c" );
  eqs( __LINE__, "get(0)",                   f.get( 0 ), "a" );
  eqs( __LINE__, "get(2)",                   f.get( 2 ), "c" );
  eqs( __LINE__, "get(3) is NULL",           f.get( 3 ) ? "?" : "(null)", "(null)" );
  eqs( __LINE__, "get(-1) is NULL",          f.get( -1 ) ? "?" : "(null)", "(null)" );
  f.set( 1, "B" );
  eqa( __LINE__, "set(1,\"B\")",             f, "a|B|c" );
  f.ins( 0, "first" );
  eqa( __LINE__, "ins(0)",                   f, "first|a|B|c" );
  f.ins( 4, "last" );
  eqa( __LINE__, "ins at count()",           f, "first|a|B|c|last" );
  f.del( 0 );
  eqa( __LINE__, "del(0)",                   f, "a|B|c|last" );
  f.del( 99 );
  eqa( __LINE__, "del out of range is a no-op", f, "a|B|c|last" );
  f.del( -1 );
  eqa( __LINE__, "del(-1) is a no-op",       f, "a|B|c|last" );

  gr( "VArray -- push / pop / shift / unshift" );
  VArray g;
  eqs( __LINE__, "pop() on empty is NULL",   g.pop() ? "?" : "(null)", "(null)" );
  eqs( __LINE__, "shift() on empty is NULL", g.shift() ? "?" : "(null)", "(null)" );
  g.push( "1" ); g.push( "2" ); g.unshift( "0" );
  eqa( __LINE__, "unshift",                  g, "0|1|2" );
  eqs( __LINE__, "pop()",                    g.pop(), "2" );
  eqs( __LINE__, "shift()",                  g.shift(), "0" );
  eqa( __LINE__, "what is left",             g, "1" );

  gr( "VArray -- operator[] does not take negative indexes (unlike VString)" );
  VArray h = mk( "a,b,c" );
  eqs( __LINE__, "h[0]",                     h[0], "a" );
  eqs( __LINE__, "h[2]",                     h[2], "c" );
  eqs( __LINE__, "h[-1] is \"\", not last",  h[-1], "" );
  eqa( __LINE__, "and the array is unchanged", h, "a|b|c" );

  gr( "VArray -- operator[] past the end GROWS the array" );
  VArray i = mk( "a,b" );
  VString& ref = i[4];
  ref = "grown";
  eqi( __LINE__, "count() after i[4]",       i.count(), 5 );
  eqa( __LINE__, "the gap is filled with \"\"", i, "a|b|||grown" );

  gr( "VArray -- a negative index into ins() / set()" );
  VArray neg1 = mk( "a,b" );
  neg1.ins( -1, "x" );
  eqa( __LINE__, "ins(-1,char*) is a no-op",  neg1, "a|b" );
  VArray neg2 = mk( "a,b" );
  neg2.set( -1, "x" );
  eqa( __LINE__, "set(-1,char*) is a no-op",  neg2, "a|b" );
  VArray neg3 = mk( "a,b" );
  neg3.set( -99, "x" );
  eqa( __LINE__, "set(-99,char*) is a no-op", neg3, "a|b" );
  /* ins() and set() are each overloaded twice and the overload is chosen by
     the argument type at the call site, so both pairs need the guard */
  VArray neg4 = mk( "a,b" );
  neg4.ins( -1, VString( "x" ) );
  eqa( __LINE__, "ins(-1,VString) is a no-op", neg4, "a|b" );
  VArray neg5 = mk( "a,b" );
  neg5.set( -1, VString( "x" ) );
  eqa( __LINE__, "set(-1,VString) is a no-op", neg5, "a|b" );
  VArray neg6 = mk( "a,b" );
  neg6.ins( -99, VString( "x" ) );
  eqa( __LINE__, "ins(-99,VString) is a no-op", neg6, "a|b" );
  VArray neg7 = mk( "a,b" );
  neg7.set( -99, VString( "x" ) );
  eqa( __LINE__, "set(-99,VString) is a no-op", neg7, "a|b" );
  WArray neg8; neg8.push( L"a" ); neg8.push( L"b" );
  neg8.ins( -1, WString( L"x" ) );
  eqwa( __LINE__, "wide ins(-1,WString) too",  neg8, "a|b" );

  gr( "VArray -- max_len / min_len" );
  VArray j;
  eqi( __LINE__, "max_len() on empty",       j.max_len(), 0 );
  eqi( __LINE__, "min_len() on empty",       j.min_len(), 0 );
  VArray k = mk( "a,bbb,cc" );
  eqi( __LINE__, "max_len()",                k.max_len(), 3 );
  eqi( __LINE__, "min_len()",                k.min_len(), 1 );

  gr( "VArray -- sort / reverse" );
  VArray l = mk( "pear,apple,fig" );
  l.sort();
  eqa( __LINE__, "sort()",                   l, "apple|fig|pear" );
  l.sort( 1 );
  eqa( __LINE__, "sort(reverse)",            l, "pear|fig|apple" );
  VArray m = mk( "a,b,c" );
  m.reverse();
  eqa( __LINE__, "reverse()",                m, "c|b|a" );
  VArray n;
  n.sort();
  eqi( __LINE__, "sort() on empty",          n.count(), 0 );
  n.reverse();
  eqi( __LINE__, "reverse() on empty",       n.count(), 0 );
  VArray o = mk( "only" );
  o.sort(); o.reverse();
  eqa( __LINE__, "sort/reverse on one",      o, "only" );
  VArray p = mk( "b,a,b,a" );
  p.sort();
  eqa( __LINE__, "sort() keeps duplicates",  p, "a|a|b|b" );

  gr( "VArray -- shuffle keeps exactly the same elements" );
  VArray q = mk( "a,b,c,d,e,f" );
  q.shuffle();
  q.sort();
  eqa( __LINE__, "shuffle then sort",        q, "a|b|c|d|e|f" );

  gr( "VArray -- foreach interface" );
  VArray r = mk( "x,y,z" );
  r.reset();
  eqs( __LINE__, "first next()",             r.next(), "x" );
  eqi( __LINE__, "current_index()",          r.current_index(), 0 );
  eqs( __LINE__, "current()",                r.current(), "x" );
  eqs( __LINE__, "second next()",            r.next(), "y" );
  eqs( __LINE__, "third next()",             r.next(), "z" );
  eqs( __LINE__, "next() past the end",      r.next() ? "?" : "(null)", "(null)" );

  gr( "VArray -- append another array / a string" );
  VArray s = mk( "1,2" );
  VArray t = mk( "3,4" );
  s.push( &t );
  eqa( __LINE__, "push(&other)",             s, "1|2|3|4" );
  VArray u = mk( "9" );
  u.unshift( &t );
  eqa( __LINE__, "unshift(&other)",          u, "3|4|9" );
  VArray v = mk( "1,2" );
  v += t;
  eqa( __LINE__, "operator+=(array)",        v, "1|2|3|4" );
  VArray w = mk( "1,2" );
  w += VString( "x" );
  eqa( __LINE__, "operator+=(string)",       w, "1|2|x" );
  VArray x = mk( "1,2" );
  x = VString( "only" );
  eqa( __LINE__, "operator=(string) replaces", x, "only" );

  gr( "VArray -- fsave / fload round trip" );
  VArray y = mk( "alpha,beta,gamma" );
  VString fn = VString( "/tmp/vstring_test_array." ) + VString( (int)getpid() );
  eqi( __LINE__, "fsave() returns 0",        y.fsave( fn ), 0 );
  VArray z;
  eqi( __LINE__, "fload() returns 0",        z.fload( fn ), 0 );
  eqa( __LINE__, "round trip",               z, "alpha|beta|gamma" );
  VArray empty_arr;
  empty_arr.fsave( fn );
  VArray z2; z2.fload( fn );
  eqi( __LINE__, "empty array round trip",   z2.count(), 0 );
  VArray blanks; blanks.push( "a" ); blanks.push( "" ); blanks.push( "b" );
  blanks.fsave( fn );
  VArray z3; z3.fload( fn );
  eqa( __LINE__, "empty elements survive",   z3, "a||b" );
  unlink( fn );
  VArray z4;
  eqi( __LINE__, "fload of a missing file",  z4.fload( "/nonexistent/nope" ) != 0, 1 );
}

/****************************************************************************
** part 1b -- VTrie
****************************************************************************/

void t_trie_basics()
{
  gr( "VTrie -- set / get / exists / del" );
  VTrie a;
  eqi( __LINE__, "empty count()",            a.count(), 0 );
  eqi( __LINE__, "exists() on empty",        a.exists( "k" ) != 0, 0 );
  eqs( __LINE__, "get() on empty is NULL",   a.get( "k" ) ? "?" : "(null)", "(null)" );
  a.set( "key", "val" );
  eqi( __LINE__, "exists() after set",       a.exists( "key" ) != 0, 1 );
  eqs( __LINE__, "get()",                    a.get( "key" ), "val" );
  eqi( __LINE__, "count()",                  a.count(), 1 );
  a.set( "key", "val2" );
  eqs( __LINE__, "set() overwrites",         a.get( "key" ), "val2" );
  eqi( __LINE__, "count() unchanged",        a.count(), 1 );
  a.del( "key" );
  eqi( __LINE__, "exists() after del",       a.exists( "key" ) != 0, 0 );
  eqi( __LINE__, "count() after del",        a.count(), 0 );
  a.del( "nothing" );
  eqi( __LINE__, "del of a missing key",     a.count(), 0 );

  gr( "VTrie -- a bare prefix is not a key" );
  VTrie b;
  b.set( "abc", "1" );
  eqi( __LINE__, "exists(\"a\")",            b.exists( "a" ) != 0, 0 );
  eqi( __LINE__, "exists(\"ab\")",           b.exists( "ab" ) != 0, 0 );
  eqi( __LINE__, "exists(\"abc\")",          b.exists( "abc" ) != 0, 1 );
  eqi( __LINE__, "exists(\"abcd\")",         b.exists( "abcd" ) != 0, 0 );

  gr( "VTrie -- shared prefixes are independent entries" );
  VTrie c;
  c.set( "ab", "1" ); c.set( "abc", "2" ); c.set( "abd", "3" ); c.set( "x", "4" );
  eqi( __LINE__, "count()",                  c.count(), 4 );
  eqt( __LINE__, "contents",                 c, "ab=1|abc=2|abd=3|x=4" );
  eqs( __LINE__, "get(\"ab\")",              c.get( "ab" ),  "1" );
  eqs( __LINE__, "get(\"abc\")",             c.get( "abc" ), "2" );
  c.del( "ab" );
  eqi( __LINE__, "the longer keys survive",  c.count(), 3 );
  eqt( __LINE__, "contents after del",       c, "abc=2|abd=3|x=4" );

  gr( "VTrie -- count(key) counts a whole branch" );
  VTrie d;
  d.set( "ab", "1" ); d.set( "abc", "2" ); d.set( "abd", "3" ); d.set( "x", "4" );
  eqi( __LINE__, "count(NULL) is everything", d.count(), 4 );
  eqi( __LINE__, "count(\"ab\")",            d.count( "ab" ), 3 );
  eqi( __LINE__, "count(\"abc\")",           d.count( "abc" ), 1 );
  eqi( __LINE__, "count(\"z\")",             d.count( "z" ), 0 );

  gr( "VTrie -- del with branch removes everything below" );
  VTrie e;
  e.set( "ab", "1" ); e.set( "abc", "2" ); e.set( "abd", "3" ); e.set( "x", "4" );
  e.del( "ab", 1 );
  eqt( __LINE__, "del(\"ab\",branch)",       e, "x=4" );

  gr( "VTrie -- an empty value is still a key" );
  VTrie f;
  f.set( "k", "" );
  eqi( __LINE__, "exists()",                 f.exists( "k" ) != 0, 1 );
  eqs( __LINE__, "get() is \"\" not NULL",   f.get( "k" ), "" );
  eqi( __LINE__, "count()",                  f.count(), 1 );

  gr( "VTrie -- operator[] creates the key on access" );
  VTrie g;
  eqi( __LINE__, "count() before",           g.count(), 0 );
  g["made"] = "here";
  eqs( __LINE__, "get() after operator[]",   g.get( "made" ), "here" );
  eqi( __LINE__, "count() after",            g.count(), 1 );

  gr( "VTrie -- keys / values / keys_and_values" );
  VTrie h;
  h.set( "k1", "v1" ); h.set( "k2", "v2" );
  VArray hk = h.keys();
  VArray hv = h.values();
  hk.sort(); hv.sort();
  eqa( __LINE__, "keys()",                   hk, "k1|k2" );
  eqa( __LINE__, "values()",                 hv, "v1|v2" );
  VArray ka, va;
  h.keys_and_values( &ka, &va );
  eqi( __LINE__, "keys_and_values counts",   ka.count() == va.count() && ka.count() == 2, 1 );

  gr( "VTrie -- undef / copy on write" );
  VTrie i;
  i.set( "a", "1" );
  VTrie j = i;
  j.set( "b", "2" );
  eqi( __LINE__, "original after copy+set",  i.count(), 1 );
  eqi( __LINE__, "copy after copy+set",      j.count(), 2 );
  j.undef();
  eqi( __LINE__, "copy after undef",         j.count(), 0 );
  eqi( __LINE__, "original after copy undef", i.count(), 1 );

  gr( "VTrie -- merge" );
  VTrie k;
  k.set( "a", "1" ); k.set( "b", "2" );
  VTrie l;
  l.set( "b", "BB" ); l.set( "c", "3" );
  k.merge( &l );
  eqt( __LINE__, "merge(trie) overwrites",   k, "a=1|b=BB|c=3" );
  VTrie m;
  m.set( "a", "1" );
  VTrie n;
  n.set( "b", "2" );
  m += n;
  eqt( __LINE__, "operator+=(trie)",         m, "a=1|b=2" );

  gr( "VTrie -- reverse swaps keys and values" );
  VTrie o;
  o.set( "k1", "v1" ); o.set( "k2", "v2" );
  o.reverse();
  eqt( __LINE__, "reverse()",                o, "v1=k1|v2=k2" );

  gr( "VTrie -- long and awkward keys" );
  VTrie p;
  VString longkey = "k"; str_mul( longkey, 500 );
  p.set( longkey, "long" );
  eqs( __LINE__, "500 character key",        p.get( longkey ), "long" );
  eqi( __LINE__, "count()",                  p.count(), 1 );
  VTrie q;
  q.set( "a b\tc", "spaced" );
  eqs( __LINE__, "key with space and tab",   q.get( "a b\tc" ), "spaced" );
  VTrie r;
  r.set( "1", "a" ); r.set( "12", "b" ); r.set( "123", "c" );
  eqi( __LINE__, "nested keys count",        r.count(), 3 );
  eqs( __LINE__, "get(\"12\")",              r.get( "12" ), "b" );

  gr( "VTrie -- vacuum keeps the data" );
  VTrie s;
  s.set( "a", "1" ); s.set( "ab", "2" ); s.set( "abc", "3" );
  s.del( "ab" );
  s.vacuum();
  eqt( __LINE__, "after del + vacuum",       s, "a=1|abc=3" );

  gr( "VTrie -- fsave / fload round trip" );
  VTrie t;
  t.set( "k1", "v1" ); t.set( "k2", "v2" );
  VString fn = VString( "/tmp/vstring_test_trie." ) + VString( (int)getpid() );
  eqi( __LINE__, "fsave() returns 0",        t.fsave( fn ), 0 );
  VTrie u;
  eqi( __LINE__, "fload() returns 0",        u.fload( fn ), 0 );
  eqt( __LINE__, "round trip",               u, "k1=v1|k2=v2" );
  unlink( fn );
}

/****************************************************************************
** part 1c -- VRegexp
**
** note m() returns the pcre match count, that is 1 + the number of captured
** subpatterns, so it is 1 for a plain match and 0 for no match -- tests below
** compare it as a truth value unless the count itself is the point.
****************************************************************************/

void t_regexp_basics()
{
  gr( "VRegexp -- compile and match" );
  VRegexp a( "abc" );
  eqi( __LINE__, "ok() after construction",  a.ok(), 1 );
  eqi( __LINE__, "m(\"xabcx\")",             a.m( "xabcx" ) != 0, 1 );
  eqi( __LINE__, "m(\"xyz\")",               a.m( "xyz" ) != 0, 0 );
  eqi( __LINE__, "m(\"\")",                  a.m( "" ) != 0, 0 );
  VRegexp b;
  eqi( __LINE__, "ok() before comp()",       b.ok(), 0 );
  eqi( __LINE__, "m() before comp()",        b.m( "abc" ) != 0, 0 );
  eqi( __LINE__, "comp() returns > 0",       b.comp( "a+b" ) > 0, 1 );
  eqi( __LINE__, "ok() after comp()",        b.ok(), 1 );
  eqi( __LINE__, "m(\"aaab\")",              b.m( "aaab" ) != 0, 1 );
  eqi( __LINE__, "recompiled to something else", b.comp( "zzz" ) > 0, 1 );
  eqi( __LINE__, "old pattern no longer matches", b.m( "aaab" ) != 0, 0 );

  gr( "VRegexp -- a bad pattern fails to compile and says why" );
  VRegexp c( "(unclosed" );
  eqi( __LINE__, "ok() on a bad pattern",    c.ok(), 0 );
  eqi( __LINE__, "error_str() is not empty", str_len( VString( c.error_str() ) ) > 0, 1 );
  eqi( __LINE__, "m() on a bad pattern",     c.m( "unclosed" ) != 0, 0 );
  VRegexp d( "[z-a]" );
  eqi( __LINE__, "reversed range fails",     d.ok(), 0 );

  gr( "VRegexp -- captured subpatterns" );
  VRegexp e( "(\\d+)-(\\d+)" );
  eqi( __LINE__, "m() returns 1 + subs",     e.m( "ab 12-34" ), 3 );
  eqs( __LINE__, "sub(0) is the whole match", e.sub( 0 ), "12-34" );
  eqs( __LINE__, "sub(1)",                   e.sub( 1 ), "12" );
  eqs( __LINE__, "sub(2)",                   e.sub( 2 ), "34" );
  eqs( __LINE__, "sub(9) out of range",      e.sub( 9 ), "" );
  eqs( __LINE__, "sub(-1) out of range",     e.sub( -1 ), "" );
  eqs( __LINE__, "operator[] is sub()",      e[1], "12" );
  eqi( __LINE__, "sub_sp(0)",                e.sub_sp( 0 ), 3 );
  eqi( __LINE__, "sub_ep(0)",                e.sub_ep( 0 ), 8 );
  eqi( __LINE__, "sub_sp(1)",                e.sub_sp( 1 ), 3 );
  eqi( __LINE__, "sub_ep(1)",                e.sub_ep( 1 ), 5 );
  eqi( __LINE__, "sub_sp(2)",                e.sub_sp( 2 ), 6 );
  eqi( __LINE__, "sub_sp(9) out of range",   e.sub_sp( 9 ), -1 );

  gr( "VRegexp -- an unset optional group" );
  VRegexp f( "(a)?(b)" );
  eqi( __LINE__, "m(\"b\")",                 f.m( "b" ) != 0, 1 );
  eqs( __LINE__, "the unset group is \"\"",  f.sub( 1 ), "" );
  eqi( __LINE__, "and its start is -1",      f.sub_sp( 1 ), -1 );
  eqs( __LINE__, "the set group",            f.sub( 2 ), "b" );

  gr( "VRegexp -- after a failed match" );
  VRegexp g( "(\\d+)" );
  eqi( __LINE__, "first match works",        g.m( "12" ) != 0, 1 );
  eqi( __LINE__, "then a failing match",     g.m( "no digits" ) != 0, 0 );
  eqs( __LINE__, "sub(1) after the failure", g.sub( 1 ), "" );

  gr( "VRegexp -- m( line, pattern ) compiles and matches in one go" );
  VRegexp h;
  eqi( __LINE__, "m(line,pattern)",          h.m( "aXc", "a(.)c" ), 2 );
  eqs( __LINE__, "sub(1)",                   h.sub( 1 ), "X" );
  eqi( __LINE__, "ok() afterwards",          h.ok(), 1 );

  gr( "VRegexp -- option i, case insensitive" );
  VRegexp i;
  i.comp( "ABC", "i" );
  eqi( __LINE__, "m(\"xabcx\")",             i.m( "xabcx" ) != 0, 1 );
  VRegexp i2;
  i2.comp( "ABC" );
  eqi( __LINE__, "without i",                i2.m( "xabcx" ) != 0, 0 );
  VRegexp i3;
  i3.comp( "(b+)", "i" );
  i3.m( "aBBc" );
  eqs( __LINE__, "captures keep their case", i3.sub( 1 ), "BB" );

  gr( "VRegexp -- option m, multiline" );
  VRegexp j;
  j.comp( "^b", "m" );
  eqi( __LINE__, "m(\"a\\nb\") with m",      j.m( "a\nb" ) != 0, 1 );
  VRegexp j2;
  j2.comp( "^b" );
  eqi( __LINE__, "m(\"a\\nb\") without m",   j2.m( "a\nb" ) != 0, 0 );

  gr( "VRegexp -- option s, dot matches newline" );
  VRegexp k;
  k.comp( "a.b", "s" );
  eqi( __LINE__, "m(\"a\\nb\") with s",      k.m( "a\nb" ) != 0, 1 );
  VRegexp k2;
  k2.comp( "a.b" );
  eqi( __LINE__, "m(\"a\\nb\") without s",   k2.m( "a\nb" ) != 0, 0 );

  gr( "VRegexp -- option x, extended" );
  VRegexp l;
  l.comp( "a b # a comment", "x" );
  eqi( __LINE__, "whitespace is ignored",    l.m( "ab" ) != 0, 1 );

  gr( "VRegexp -- option f, plain substring search" );
  VRegexp m;
  m.comp( "a.c", "f" );
  eqi( __LINE__, "matches the literal",      m.m( "xxa.cyy" ) != 0, 1 );
  eqs( __LINE__, "sub(0) is what was found", m.sub( 0 ), "a.c" );
  eqi( __LINE__, "sub_sp(0)",                m.sub_sp( 0 ), 2 );
  eqi( __LINE__, "sub_ep(0)",                m.sub_ep( 0 ), 5 );
  eqs( __LINE__, "sub(1) is \"\"",           m.sub( 1 ), "" );
  eqi( __LINE__, "'.' is not a wildcard",    m.m( "xxabcyy" ) != 0, 0 );
  eqi( __LINE__, "sub_sp(0) after a miss",   m.sub_sp( 0 ), -1 );
  eqs( __LINE__, "sub(0) after a miss",      m.sub( 0 ), "" );

  gr( "VRegexp -- option h, hex pattern" );
  VRegexp n;
  n.comp( "41 42", "h" );
  eqi( __LINE__, "\"41 42\" finds \"AB\"",   n.m( "xABx" ) != 0, 1 );
  eqi( __LINE__, "and not \"AC\"",           n.m( "xACx" ) != 0, 0 );

  gr( "VRegexp -- anchors, classes, quantifiers" );
  VRegexp o( "^abc$" );
  eqi( __LINE__, "^abc$ vs \"abc\"",         o.m( "abc" ) != 0, 1 );
  eqi( __LINE__, "^abc$ vs \"xabc\"",        o.m( "xabc" ) != 0, 0 );
  VRegexp p( "[a-c]+" );
  eqi( __LINE__, "[a-c]+ vs \"xbbz\"",       p.m( "xbbz" ) != 0, 1 );
  eqi( __LINE__, "[a-c]+ vs \"xyz\"",        p.m( "xyz" ) != 0, 0 );
  VRegexp q( "a{2,3}" );
  eqi( __LINE__, "a{2,3} vs \"aa\"",         q.m( "aa" ) != 0, 1 );
  eqi( __LINE__, "a{2,3} vs \"a\"",          q.m( "a" ) != 0, 0 );
  VRegexp r( "\\bword\\b" );
  eqi( __LINE__, "word boundary hit",        r.m( "a word here" ) != 0, 1 );
  eqi( __LINE__, "word boundary miss",       r.m( "swordfish" ) != 0, 0 );
  VRegexp s( "(a|b)c" );
  s.m( "bc" );
  eqs( __LINE__, "alternation capture",      s.sub( 1 ), "b" );

  gr( "VRegexp -- str_split uses a regexp" );
  VArray t = str_split( "\\s+", "a  b   c" );
  eqa( __LINE__, "split on \\s+",            t, "a|b|c" );
  VArray u = str_split( ",", "a,b,c", 2 );
  eqi( __LINE__, "split with maxcount 2",    u.count(), 2 );
  VArray v = str_split( "\\d", "a1b2c" );
  eqa( __LINE__, "split on a digit",         v, "a|b|c" );
}

/****************************************************************************
** part 1d -- the wide counterparts
****************************************************************************/

#define U_AE  "\xc3\xa4"                  /* U+00E4 */
#define U_BG  "\xd0\xb4\xd0\xb0"          /* U+0434 U+0430 */
#define U_EM  "\xf0\x9f\x98\x80"          /* U+1F600 */

void t_wide()
{
  gr( "WArray -- the same operations as VArray" );
  WArray a;
  eqi( __LINE__, "empty count()",            a.count(), 0 );
  a.push( L"one" ); a.push( L"two" ); a.push( L"three" );
  eqwa( __LINE__, "after 3 pushes",          a, "one|two|three" );
  eqw( __LINE__, "get(0)",                   a.get( 0 ), L"one" );
  eqs( __LINE__, "get(9) is NULL",           a.get( 9 ) ? "?" : "(null)", "(null)" );
  a.ins( 1, L"mid" );
  eqwa( __LINE__, "ins(1)",                  a, "one|mid|two|three" );
  a.del( 1 );
  eqwa( __LINE__, "del(1)",                  a, "one|two|three" );
  a.set( 0, L"ONE" );
  eqwa( __LINE__, "set(0)",                  a, "ONE|two|three" );
  eqw( __LINE__, "pop()",                    a.pop(), L"three" );
  eqw( __LINE__, "shift()",                  a.shift(), L"ONE" );
  a.unshift( L"first" );
  eqwa( __LINE__, "unshift()",               a, "first|two" );
  a.reverse();
  eqwa( __LINE__, "reverse()",               a, "two|first" );
  a.sort();
  eqwa( __LINE__, "sort()",                  a, "first|two" );
  eqi( __LINE__, "max_len()",                a.max_len(), 5 );
  eqi( __LINE__, "min_len()",                a.min_len(), 3 );
  WArray b = a;
  b.push( L"copy" );
  eqwa( __LINE__, "copy on write",           a, "first|two" );
  eqs( __LINE__, "operator[](-1) is \"\"",   VString( b[-1].data() ), "" );

  gr( "WArray -- unicode elements" );
  WArray c;
  c.push( WString( U_AE ) ); c.push( WString( U_BG ) ); c.push( WString( U_EM ) );
  eqi( __LINE__, "count()",                  c.count(), 3 );
  eqi( __LINE__, "element is 1 character",   str_len( WString( c.get( 0 ) ) ), 1 );
  eqi( __LINE__, "element is 2 characters",  str_len( WString( c.get( 1 ) ) ), 2 );
  eqi( __LINE__, "emoji is 1 character",     str_len( WString( c.get( 2 ) ) ), 1 );
  eqs( __LINE__, "back to utf8",             VString( WString( c.get( 1 ) ).data() ), U_BG );
  eqi( __LINE__, "max_len() counts chars",   c.max_len(), 2 );

  gr( "WTrie -- the same operations as VTrie" );
  WTrie d;
  eqi( __LINE__, "empty count()",            d.count(), 0 );
  d.set( L"key", L"val" );
  eqw( __LINE__, "get()",                    d.get( L"key" ), L"val" );
  eqi( __LINE__, "exists()",                 d.exists( L"key" ) != 0, 1 );
  eqi( __LINE__, "exists() on a prefix",     d.exists( L"ke" ) != 0, 0 );
  d.set( L"k2", L"v2" );
  eqwt( __LINE__, "contents",                d, "k2=v2|key=val" );
  eqi( __LINE__, "count()",                  d.count(), 2 );
  d.del( L"key" );
  eqwt( __LINE__, "after del",               d, "k2=v2" );
  WTrie e = d;
  e.set( L"k3", L"v3" );
  eqi( __LINE__, "copy on write",            d.count(), 1 );
  e.reverse();
  eqwt( __LINE__, "reverse()",               e, "v2=k2|v3=k3" );

  gr( "WTrie -- unicode keys and values" );
  WTrie f;
  f.set( WString( U_BG ), WString( U_AE ) );
  eqi( __LINE__, "count()",                  f.count(), 1 );
  eqi( __LINE__, "exists() with a utf8 key", f.exists( WString( U_BG ) ) != 0, 1 );
  eqs( __LINE__, "value back to utf8",       VString( WString( f.get( WString( U_BG ) ) ).data() ), U_AE );
  f.set( WString( U_EM ), L"emoji" );
  eqi( __LINE__, "emoji key",                f.exists( WString( U_EM ) ) != 0, 1 );
  eqi( __LINE__, "count() after emoji",      f.count(), 2 );

  gr( "WRegexp -- the same operations as VRegexp" );
  WRegexp g( L"(\\d+)-(\\d+)" );
  eqi( __LINE__, "ok()",                     g.ok(), 1 );
  eqi( __LINE__, "m() returns 1 + subs",     g.m( L"ab 12-34" ), 3 );
  eqw( __LINE__, "sub(0)",                   g.sub( 0 ), L"12-34" );
  eqw( __LINE__, "sub(1)",                   g.sub( 1 ), L"12" );
  eqw( __LINE__, "sub(2)",                   g.sub( 2 ), L"34" );
  eqi( __LINE__, "sub_sp(1)",                g.sub_sp( 1 ), 3 );
  WRegexp h( L"(unclosed" );
  eqi( __LINE__, "bad pattern ok()",         h.ok(), 0 );
  WRegexp i;
  i.comp( L"ABC", L"i" );
  eqi( __LINE__, "option i",                 i.m( L"xabcx" ) != 0, 1 );
  WRegexp j;
  j.comp( L"a.c", L"f" );
  eqi( __LINE__, "option f, literal",        j.m( L"xa.cy" ) != 0, 1 );
  eqi( __LINE__, "option f, no wildcard",    j.m( L"xabcy" ) != 0, 0 );

  gr( "WRegexp -- unicode subjects" );
  WRegexp k( L"." );
  eqi( __LINE__, "'.' matches one character", k.m( WString( U_EM ) ), 1 );
  eqi( __LINE__, "and it spans 1 char",      k.sub_ep( 0 ) - k.sub_sp( 0 ), 1 );
  WRegexp l( WString( U_BG ).data() );
  eqi( __LINE__, "a utf8 pattern matches",   l.m( WString( VString( "x" ) + U_BG + "y" ) ) != 0, 1 );
  WRegexp m( L"(\\w+)" );
  eqi( __LINE__, "\\w is ASCII only (no UCP)", m.m( WString( U_BG ) ) != 0, 0 );
  eqi( __LINE__, "\\w still matches ASCII",   m.m( L"abc" ) != 0, 1 );
  WRegexp m2( L"[\x430-\x44f]+" );
  eqi( __LINE__, "an explicit range matches",  m2.m( WString( U_BG ) ) != 0, 1 );
  WRegexp n( L"^.$" );
  eqi( __LINE__, "^.$ vs one emoji",         n.m( WString( U_EM ) ) != 0, 1 );
  eqi( __LINE__, "^.$ vs two characters",    n.m( WString( U_BG ) ) != 0, 0 );

  gr( "wide split and join" );
  WArray o = str_split_simple( L",", L"a,b,c" );
  eqwa( __LINE__, "str_split_simple",        o, "a|b|c" );
  eqw( __LINE__, "str_join",                 str_join( o, L"-" ), L"a-b-c" );
  WArray p = str_split( L"\\s+", L"a  b   c" );
  eqwa( __LINE__, "str_split on \\s+",       p, "a|b|c" );
}

/****************************************************************************
** part 2 -- interoperability
****************************************************************************/

void t_interop()
{
  gr( "array <-> trie" );
  VTrie a;
  a.set( "k1", "v1" ); a.set( "k2", "v2" );
  VArray b;
  b.push( &a );
  eqi( __LINE__, "push(&trie) flattens to pairs", b.count(), 4 );
  VTrie c;
  c.merge( &b );
  eqt( __LINE__, "merge(&array) rebuilds it",     c, "k1=v1|k2=v2" );
  VArray d;
  d = a;
  eqi( __LINE__, "array = trie",                  d.count(), 4 );
  VTrie e;
  e = d;
  eqt( __LINE__, "trie = array",                  e, "k1=v1|k2=v2" );
  VTrie f;
  f.set( "x", "1" );
  VArray g;
  g.push( &f );
  VTrie h;
  h = g;
  eqt( __LINE__, "round trip of one pair",        h, "x=1" );

  gr( "array <-> trie, wide" );
  WTrie i;
  i.set( L"k1", L"v1" ); i.set( L"k2", L"v2" );
  WArray j;
  j.push( &i );
  eqi( __LINE__, "push(&trie) flattens to pairs", j.count(), 4 );
  WTrie k;
  k.merge( &j );
  eqwt( __LINE__, "merge(&array) rebuilds it",    k, "k1=v1|k2=v2" );

  gr( "narrow <-> wide arrays, element by element" );
  VArray l = mk( "one,two,three" );
  WArray m;
  for( int z = 0; z < l.count(); z++ ) m.push( WString( l.get( z ) ) );
  eqwa( __LINE__, "VArray -> WArray",             m, "one|two|three" );
  VArray n;
  for( int z = 0; z < m.count(); z++ ) n.push( VString( m.get( z ) ) );
  eqa( __LINE__, "and back to VArray",            n, "one|two|three" );

  gr( "narrow <-> wide arrays carry utf8 through unchanged" );
  VArray o;
  o.push( U_AE ); o.push( U_BG ); o.push( U_EM );
  WArray p;
  for( int z = 0; z < o.count(); z++ ) p.push( WString( o.get( z ) ) );
  VArray q;
  for( int z = 0; z < p.count(); z++ ) q.push( VString( p.get( z ) ) );
  eqa( __LINE__, "utf8 round trip",               q, U_AE "|" U_BG "|" U_EM );
  eqi( __LINE__, "narrow max_len is in bytes",    o.max_len(), 4 );
  eqi( __LINE__, "wide max_len is in characters", p.max_len(), 2 );

  gr( "narrow <-> wide tries" );
  VTrie r;
  r.set( "k1", "v1" ); r.set( U_BG, U_AE );
  WTrie s;
  VArray rk, rv;
  r.keys_and_values( &rk, &rv );
  for( int z = 0; z < rk.count(); z++ ) s.set( WString( rk.get( z ) ), WString( rv.get( z ) ) );
  eqi( __LINE__, "VTrie -> WTrie count",          s.count(), 2 );
  eqi( __LINE__, "the utf8 key is found",         s.exists( WString( U_BG ) ) != 0, 1 );
  VTrie t;
  WArray sk, sv;
  s.keys_and_values( &sk, &sv );
  for( int z = 0; z < sk.count(); z++ ) t.set( VString( sk.get( z ) ), VString( sv.get( z ) ) );
  eqt( __LINE__, "and back again",                t, VString( "k1=v1|" ) + U_BG + "=" + U_AE );

  gr( "narrow and wide must agree -- split, join, sort" );
  static const char*    ns[] = { "a,b,c", "a", "", "a,,b", ",a", "x,y,z,w", NULL };
  static const wchar_t* ws[] = { L"a,b,c", L"a", L"", L"a,,b", L",a", L"x,y,z,w", NULL };
  for( int z = 0; ns[z]; z++ )
    {
    VArray na = str_split_simple( ",", ns[z] );
    WArray wa = str_split_simple( L",", ws[z] );
    eqi( __LINE__, "split counts agree",          na.count(), wa.count() );
    eqs( __LINE__, "join results agree",
         str_join( na, "-" ), VString( str_join( wa, L"-" ).data() ) );
    na.sort(); wa.sort();
    eqs( __LINE__, "sorted results agree",
         str_join( na, "-" ), VString( str_join( wa, L"-" ).data() ) );
    }

  gr( "narrow and wide regexps must agree" );
  static const char*    np[] = { "a.c", "(\\d+)", "^ab", "[a-c]+", "x|y", NULL };
  static const wchar_t* wp[] = { L"a.c", L"(\\d+)", L"^ab", L"[a-c]+", L"x|y", NULL };
  static const char*    nl[] = { "abc", "a12b", "abcd", "zzz", "", NULL };
  static const wchar_t* wl[] = { L"abc", L"a12b", L"abcd", L"zzz", L"", NULL };
  for( int p = 0; np[p]; p++ )
    for( int l = 0; nl[l]; l++ )
      {
      VRegexp nr( np[p] );
      WRegexp wr( wp[p] );
      eqi( __LINE__, "ok() agrees",               nr.ok(), wr.ok() );
      int a1 = nr.m( nl[l] );
      int a2 = wr.m( wl[l] );
      eqi( __LINE__, "m() agrees",                a1 != 0, a2 != 0 );
      if( a1 && a2 )
        eqs( __LINE__, "sub(0) agrees",           nr.sub( 0 ), VString( wr.sub( 0 ).data() ) );
      }
}

/****************************************************************************
** part 3 -- exhaustive sweeps
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

/* a plain model of the array, to check the real one against */
#define MAXMODEL 16
struct Model
{
  VString e[MAXMODEL];
  int n;
  Model() { n = 0; }
  void push( const char* s ) { e[n++] = s; }
  void ins( int p, const char* s )
    { if( p < 0 || p > n ) return; for( int z = n; z > p; z-- ) e[z] = e[z-1]; e[p] = s; n++; }
  void del( int p )
    { if( p < 0 || p >= n ) return; for( int z = p; z < n - 1; z++ ) e[z] = e[z+1]; n--; }
  VString join()
    { VString r; for( int z = 0; z < n; z++ ) { if( z ) r += "|"; r += e[z]; } return r; }
};

void sweep_array()
{
  gr( "sweep: VArray ins/del at every position, for every size" );

  for( int size = 0; size <= 8; size++ )
    {
    for( int pos = -2; pos <= size + 2; pos++ )
      {
      VArray a;  Model m;
      char b[8];
      for( int z = 0; z < size; z++ ) { sprintf( b, "e%d", z ); a.push( b ); m.push( b ); }

      /* ins() past the end grows the array rather than being a no-op, so only
         the in-range positions are compared against the plain model */
      VArray ai = a;  Model mi = m;
      ai.ins( pos, "NEW" );  mi.ins( pos, "NEW" );
      if( pos >= 0 && pos <= size )
        sw( __LINE__, str_join( ai, "|" ) == mi.join(), "ins", str_join( ai, "|" ) );

      VArray ad = a;  Model md = m;
      ad.del( pos );  md.del( pos );
      sw( __LINE__, str_join( ad, "|" ) == md.join(), "del", str_join( ad, "|" ) );

      /* a deletion of every element in turn empties it */
      VArray ae = a;
      while( ae.count() ) ae.del( 0 );
      sw( __LINE__, ae.count() == 0, "del until empty", str_join( ae, "|" ) );
      }

    /* push/pop and unshift/shift are inverses */
    VArray c;
    char b[8];
    for( int z = 0; z < size; z++ ) { sprintf( b, "e%d", z ); c.push( b ); }
    VString before = str_join( c, "|" );
    c.push( "tmp" );
    c.pop();
    sw( __LINE__, str_join( c, "|" ) == before, "push then pop", str_join( c, "|" ) );
    c.unshift( "tmp" );
    c.shift();
    sw( __LINE__, str_join( c, "|" ) == before, "unshift then shift", str_join( c, "|" ) );
    c.reverse();
    c.reverse();
    sw( __LINE__, str_join( c, "|" ) == before, "reverse twice", str_join( c, "|" ) );

    /* and the wide array must behave identically */
    WArray w;
    wchar_t wb[8];
    for( int z = 0; z < size; z++ ) { swprintf( wb, 8, L"e%d", z ); w.push( wb ); }
    for( int pos = -2; pos <= size + 2; pos++ )
      {
      VArray an = c;  WArray wn = w;
      an = str_split_simple( ",", "" );
      an.undef();
      for( int z = 0; z < size; z++ ) { sprintf( b, "e%d", z ); an.push( b ); }
      an.del( pos ); wn.del( pos );
      sw( __LINE__, str_join( an, "|" ) == VString( str_join( wn, L"|" ).data() ),
          "narrow/wide del", str_join( an, "|" ) );
      }
    }
}

void sweep_trie()
{
  gr( "sweep: VTrie is order independent and survives every deletion order" );

  static const char* keys[] = { "a", "ab", "abc", "b", "bc", "x" };
  const int NK = 6;

  /* inserting the same keys in any rotation gives the same contents */
  VString reference;
  for( int rot = 0; rot < NK; rot++ )
    {
    VTrie t;
    for( int z = 0; z < NK; z++ )
      {
      const char* k = keys[ ( z + rot ) % NK ];
      t.set( k, VString( "v_" ) + k );
      }
    VArray ka = t.keys();
    ka.sort();
    VString got = str_join( ka, "|" );
    if( rot == 0 ) reference = got;
    sw( __LINE__, got == reference, "insertion order independent", got );
    sw( __LINE__, t.count() == NK, "count after inserts", VString( t.count() ) );
    for( int z = 0; z < NK; z++ )
      sw( __LINE__, VString( t.get( keys[z] ) ) == VString( "v_" ) + keys[z],
          "value kept", keys[z] );
    }

  /* deleting the keys in any rotation empties it, and never takes a
     neighbour with it */
  for( int rot = 0; rot < NK; rot++ )
    {
    VTrie t;
    for( int z = 0; z < NK; z++ ) t.set( keys[z], "v" );
    for( int z = 0; z < NK; z++ )
      {
      const char* k = keys[ ( z + rot ) % NK ];
      t.del( k );
      sw( __LINE__, t.exists( k ) == 0, "deleted key is gone", k );
      sw( __LINE__, t.count() == NK - z - 1, "count after del", VString( t.count() ) );
      }
    sw( __LINE__, t.count() == 0, "empty at the end", VString( t.count() ) );
    }

  /* the trie and the wide trie must agree */
  for( int rot = 0; rot < NK; rot++ )
    {
    VTrie t;  WTrie w;
    for( int z = 0; z < NK; z++ )
      {
      const char* k = keys[ ( z + rot ) % NK ];
      t.set( k, "v" );
      w.set( WString( k ), L"v" );
      }
    VArray ka = t.keys();  ka.sort();
    WArray wa = w.keys();  wa.sort();
    sw( __LINE__, str_join( ka, "|" ) == VString( str_join( wa, L"|" ).data() ),
        "narrow/wide trie keys", str_join( ka, "|" ) );
    }

  /* set/get over every prefix of a long key */
  VTrie p;
  VString key;
  for( int z = 0; z < 40; z++ )
    {
    key += "k";
    p.set( key, VString( z ) );
    }
  sw( __LINE__, p.count() == 40, "40 nested keys", VString( p.count() ) );
  VString key2;
  for( int z = 0; z < 40; z++ )
    {
    key2 += "k";
    sw( __LINE__, VString( p.get( key2 ) ) == VString( z ), "nested value", key2 );
    }
}

void sweep_regexp()
{
  gr( "sweep: every pattern against every subject, narrow vs wide" );

  static const char*    np[] = { "a", "a.c", "^a", "c$", "[abc]", "[^abc]", "a*",
                                 "a+", "a?b", "(a)(b)", "a|b", "\\d", "\\w+",
                                 "\\s", ".", "^$", "(a+)(b+)", NULL };
  static const wchar_t* wp[] = { L"a", L"a.c", L"^a", L"c$", L"[abc]", L"[^abc]", L"a*",
                                 L"a+", L"a?b", L"(a)(b)", L"a|b", L"\\d", L"\\w+",
                                 L"\\s", L".", L"^$", L"(a+)(b+)", NULL };
  static const char*    ns[] = { "", "a", "b", "ab", "abc", "aabb", "a c", "1", "  ",
                                 "xyz", "cba", NULL };
  static const wchar_t* ws[] = { L"", L"a", L"b", L"ab", L"abc", L"aabb", L"a c", L"1", L"  ",
                                 L"xyz", L"cba", NULL };

  for( int p = 0; np[p]; p++ )
    {
    VRegexp nr( np[p] );
    WRegexp wr( wp[p] );
    sw( __LINE__, nr.ok() == wr.ok(), "ok() agrees", np[p] );
    for( int s = 0; ns[s]; s++ )
      {
      int a = nr.m( ns[s] );
      int b = wr.m( ws[s] );
      sw( __LINE__, ( a != 0 ) == ( b != 0 ), "m() agrees",
          ( VString( np[p] ) + " vs [" + ns[s] + "]" ) );
      sw( __LINE__, a == b, "m() count agrees",
          ( VString( np[p] ) + " vs [" + ns[s] + "] " + a + "/" + b ) );
      if( a == 0 || b == 0 ) continue;
      for( int g = 0; g < a; g++ )
        {
        sw( __LINE__, nr.sub( g ) == VString( wr.sub( g ).data() ), "sub() agrees",
            ( VString( np[p] ) + " vs [" + ns[s] + "] #" + g ) );
        sw( __LINE__, nr.sub_sp( g ) == wr.sub_sp( g ), "sub_sp() agrees",
            ( VString( np[p] ) + " vs [" + ns[s] + "] #" + g ) );
        sw( __LINE__, nr.sub_ep( g ) == wr.sub_ep( g ), "sub_ep() agrees",
            ( VString( np[p] ) + " vs [" + ns[s] + "] #" + g ) );
        }
      /* a capture must be the slice its own positions describe */
      int sp = nr.sub_sp( 0 ), ep = nr.sub_ep( 0 );
      if( sp >= 0 && ep >= sp )
        {
        VString slice;
        str_copy( slice, ns[s], sp, ep - sp );
        sw( __LINE__, nr.sub( 0 ) == slice, "sub(0) == its own slice",
            ( VString( np[p] ) + " vs [" + ns[s] + "]" ) );
        }
      }
    }
}

/****************************************************************************/

int main( int argc, char** argv )
{
  setlocale( LC_ALL, "" );
  for( int z = 1; z < argc; z++ )
    if( strcmp( argv[z], "-q" ) == 0 ) quiet = 1;

  printf( "=========================================================================\n" );
  printf( " VArray / VTrie / VRegexp -- part 1: named cases\n" );
  printf( "=========================================================================\n" );
  t_array_basics();
  t_trie_basics();
  t_regexp_basics();
  t_wide();

  printf( "\n=========================================================================\n" );
  printf( " part 2: interoperability\n" );
  printf( "=========================================================================\n" );
  t_interop();

  int named_run    = tests_run;
  int named_failed = tests_failed;

  printf( "\n=========================================================================\n" );
  printf( " part 3: exhaustive sweeps\n" );
  printf( "=========================================================================\n" );
  sweep_array();
  sweep_trie();
  sweep_regexp();
  if( sweep_fail > 25 )
    printf( "  ... and %ld more\n", sweep_fail - 25 );

  printf( "\n=========================================================================\n" );
  printf( " named cases : %d run, %d failed\n", named_run, named_failed );
  if( named_failed )
    {
    printf( " failed lines:%s\n", failed_lines.data() );
    printf( " debug with  : gdb --args ./test_varray_vtrie_vregexp\n" );
    printf( "               then  break test_varray_vtrie_vregexp.cpp:LINE\n" );
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
