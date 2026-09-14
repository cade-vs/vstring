/****************************************************************************
 #
 #  VSTRING Library
 #
 #  test suite for sfn_match() -- shell-style filename pattern matching
 #
 #  build:
 #    g++ -O0 -g -I. -o test_sfn test_sfn.cpp libvstring.a -lpcre2-8 -lpcre2-32
 #  build with memory checking (recommended, patterns/strings live in
 #  exact-size heap buffers so any read outside them is reported):
 #    g++ -O0 -g -fsanitize=address -I. -o test_sfn test_sfn.cpp \
 #        vstring.cpp wstring.cpp vstrlib.cpp wstrlib.cpp vstruti.cpp vref.cpp \
 #        -lpcre2-8 -lpcre2-32
 #
 #  part 1 -- named cases with explicit expectations
 #  part 2 -- exhaustive sweep of every pattern/string over a small alphabet,
 #            checked against the system fnmatch(3)
 #
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <fnmatch.h>

#include "vstring.h"
#include "wstring.h"
#include "vstrlib.h"
#include "wstrlib.h"

/****************************************************************************
** part 1 -- named cases
****************************************************************************/

#define MATCH    1
#define NOMATCH  0

int    tests_run     = 0;
int    tests_failed  = 0;
int    quiet         = 0;

const char* flags_str( int flags )
{
  if( flags == ( SFN_NOESCAPE | SFN_CASEFOLD ) ) return "NOESC|FOLD";
  if( flags == SFN_NOESCAPE ) return "NOESCAPE";
  if( flags == SFN_CASEFOLD ) return "CASEFOLD";
  return "";
}

/* copies pattern and string into exact-size heap buffers, so that a build  */
/* with -fsanitize=address reports any access outside of them               */
void chk( const char* pattern, const char* str, int flags, int expect, const char* note = NULL )
{
  char* p = (char*)malloc( strlen( pattern ) + 1 );
  char* s = (char*)malloc( strlen( str     ) + 1 );
  strcpy( p, pattern );
  strcpy( s, str );

  int r   = sfn_match( p, s, flags );
  int got = ( r == 0 ) ? MATCH : NOMATCH;

  free( p );
  free( s );

  tests_run++;
  int ok = ( got == expect );
  if( ! ok ) tests_failed++;

  if( ok && quiet ) return;

  printf( "  %-4s  %-18s %-18s %-10s  got %-8s want %-8s %s\n",
          ok ? "ok" : "FAIL",
          pattern, str, flags_str( flags ),
          got == MATCH ? "MATCH" : "no",
          expect == MATCH ? "MATCH" : "no",
          note ? note : "" );
}

void wchk( const wchar_t* pattern, const wchar_t* str, int flags, int expect )
{
  int n1 = wcslen( pattern );
  int n2 = wcslen( str );
  wchar_t* p = (wchar_t*)malloc( ( n1 + 1 ) * sizeof( wchar_t ) );
  wchar_t* s = (wchar_t*)malloc( ( n2 + 1 ) * sizeof( wchar_t ) );
  wcscpy( p, pattern );
  wcscpy( s, str );

  int r   = sfn_match( p, s, flags );
  int got = ( r == 0 ) ? MATCH : NOMATCH;

  free( p );
  free( s );

  tests_run++;
  int ok = ( got == expect );
  if( ! ok ) tests_failed++;

  if( ok && quiet ) return;

  printf( "  %-4s  %-18ls %-18ls %-10s  got %-8s want %-8s\n",
          ok ? "ok" : "FAIL",
          pattern, str, flags_str( flags ),
          got == MATCH ? "MATCH" : "no",
          expect == MATCH ? "MATCH" : "no" );
}

/* for malformed patterns: run it, print what happens, assert nothing */
void inf( const char* pattern, const char* str, int flags, const char* note = NULL )
{
  char* p = (char*)malloc( strlen( pattern ) + 1 );
  char* s = (char*)malloc( strlen( str     ) + 1 );
  strcpy( p, pattern );
  strcpy( s, str );

  int r = sfn_match( p, s, flags );

  free( p );
  free( s );

  printf( "  %-4s  %-18s %-18s %-10s  got %-8s %-9s %s\n",
          "--", pattern, str, flags_str( flags ),
          r == 0 ? "MATCH" : "no", "unspecified", note ? note : "" );
}

void gr( const char* name )
{
  printf( "\n--- %s\n", name );
}

void named_cases()
{
  gr( "literals" );
  chk( "abc",       "abc",        0, MATCH   );
  chk( "abc",       "abd",        0, NOMATCH );
  chk( "abc",       "ab",         0, NOMATCH );
  chk( "ab",        "abc",        0, NOMATCH );
  chk( "a",         "a",          0, MATCH   );

  gr( "? -- any single character" );
  chk( "?",         "a",          0, MATCH   );
  chk( "a?c",       "abc",        0, MATCH   );
  chk( "a?c",       "ac",         0, NOMATCH );
  chk( "???",       "abc",        0, MATCH   );
  chk( "???",       "ab",         0, NOMATCH );
  chk( "???",       "abcd",       0, NOMATCH );
  chk( "??*",       "vf",         0, MATCH   );
  chk( "??*",       "v",          0, NOMATCH );

  gr( "* -- any number of characters" );
  chk( "*",         "abc",        0, MATCH   );
  chk( "a*",        "abc",        0, MATCH   );
  chk( "a*",        "a",          0, MATCH   );
  chk( "*c",        "abc",        0, MATCH   );
  chk( "a*c",       "abc",        0, MATCH   );
  chk( "a*c",       "ac",         0, MATCH,  "* matches empty" );
  chk( "a*c",       "abbbc",      0, MATCH   );
  chk( "a*c",       "abcd",       0, NOMATCH );
  chk( "*a*",       "bab",        0, MATCH   );
  chk( "**",        "ab",         0, MATCH   );
  chk( "v*xt**",    "vfudirtest.txt", 0, MATCH   );
  chk( "v*xt**?",   "vfudirtest.txt", 0, NOMATCH );
  chk( "*vfu*?",    "vfuz",       0, MATCH   );
  chk( "vf*i*r",    "vfudir",     0, MATCH   );
  chk( "vf*x*r",    "vfudir",     0, NOMATCH );

  gr( "[...] -- character sets" );
  chk( "[abc]",     "b",          0, MATCH   );
  chk( "[abc]",     "d",          0, NOMATCH );
  chk( "[abc]",     "bb",         0, NOMATCH );
  chk( "x[abc]y",   "xby",        0, MATCH   );
  chk( "vf[you]*",  "vfudir",     0, MATCH   );
  chk( "[a]",       "a",          0, MATCH   );

  gr( "[...] -- ranges" );
  chk( "[a-f]",     "c",          0, MATCH   );
  chk( "[a-f]",     "a",          0, MATCH   );
  chk( "[a-f]",     "f",          0, MATCH   );
  chk( "[a-f]",     "g",          0, NOMATCH );
  chk( "[0-9]",     "5",          0, MATCH   );
  chk( "[a-cx-z]",  "y",          0, MATCH   );
  chk( "[a-cx-z]",  "m",          0, NOMATCH );
  chk( "v*[c-e]*",  "vfudirtest.txt", 0, MATCH );

  gr( "[!...] and [^...] -- negated sets" );
  chk( "[!abc]",    "d",          0, MATCH   );
  chk( "[!abc]",    "a",          0, NOMATCH );
  chk( "[^abc]",    "d",          0, MATCH   );
  chk( "[^abc]",    "a",          0, NOMATCH );
  chk( "vf[^you]*", "vfudir",     0, NOMATCH );
  chk( "[!a-f]",    "z",          0, MATCH   );
  chk( "[!a-f]",    "c",          0, NOMATCH );

  gr( "\\ -- escaping" );
  chk( "\\*",       "*",          0, MATCH   );
  chk( "\\*",       "a",          0, NOMATCH );
  chk( "\\?",       "?",          0, MATCH   );
  chk( "a\\?b",     "a?b",        0, MATCH   );
  chk( "a\\?b",     "axb",        0, NOMATCH );
  chk( "\\[",       "[",          0, MATCH   );
  chk( "\\\\",      "\\",         0, MATCH   );
  chk( "vf\\*d[g-k]?z", "vf*dirz", 0, MATCH  );
  chk( "vf*\\?*r",  "vfutest?xdir", 0, MATCH );

  gr( "SFN_NOESCAPE -- \\ is an ordinary character" );
  chk( "vf\\u*",    "vfudir",     SFN_NOESCAPE, NOMATCH );
  chk( "vf\\u*",    "vf\\udir",   SFN_NOESCAPE, MATCH   );
  chk( "a\\b",      "a\\b",       SFN_NOESCAPE, MATCH   );
  chk( "a\\b",      "ab",         SFN_NOESCAPE, NOMATCH );

  gr( "SFN_CASEFOLD -- case insensitive" );
  chk( "abc",       "ABC",        SFN_CASEFOLD, MATCH   );
  chk( "ABC",       "abc",        SFN_CASEFOLD, MATCH   );
  chk( "abc",       "ABC",        0,            NOMATCH );
  chk( "a?c",       "AXC",        SFN_CASEFOLD, MATCH   );
  chk( "a*c",       "ABC",        SFN_CASEFOLD, MATCH   );
  chk( "[a-f]",     "D",          SFN_CASEFOLD, MATCH   );
  chk( "[abc]",     "B",          SFN_CASEFOLD, MATCH   );
  chk( "[!abc]",    "B",          SFN_CASEFOLD, NOMATCH );
  chk( "vf\\U*",    "Vf\\udir",   SFN_NOESCAPE,                NOMATCH );
  chk( "vf\\U*",    "Vf\\udir",   SFN_NOESCAPE | SFN_CASEFOLD, MATCH   );
  chk( "vF\\*d[g-k]?z", "Vf*dIrz", SFN_CASEFOLD, MATCH );
  chk( "\\a",       "A",          SFN_CASEFOLD, MATCH,  "escaped char must fold too" );
  chk( "x\\ay",     "XAY",        SFN_CASEFOLD, MATCH,  "escaped char must fold too" );
  chk( "xay",       "XAY",        SFN_CASEFOLD, MATCH,  "same, unescaped" );

  gr( "empty pattern / empty string -- documented: neither matches" );
  chk( "",          "",           0, NOMATCH );
  chk( "",          "a",          0, NOMATCH );
  chk( "a",         "",           0, NOMATCH );
  chk( "*",         "",           0, NOMATCH, "differs from fnmatch(3) by design" );

  gr( "malformed charsets -- must not match" );
  chk( "[a-]",      "a",          0, NOMATCH, "range with no end, use [a\\-]" );
  chk( "[a-]",      "-",          0, NOMATCH, "range with no end, use [a\\-]" );
  chk( "[a-]",      "b",          0, NOMATCH, "range with no end, use [a\\-]" );
  chk( "[]",        "]",          0, NOMATCH, "empty set" );
  chk( "[]]",       "]",          0, NOMATCH, "bare ']' in a set, use [\\]]" );
  chk( "[]a]",      "a",          0, NOMATCH, "bare ']' in a set, use [\\]]" );

  gr( "escaped '-' inside a charset -- the proper way to mean a literal '-'" );
  chk( "[a\\-]",    "a",          0, MATCH   );
  chk( "[a\\-]",    "-",          0, MATCH   );
  chk( "[a\\-]",    "b",          0, NOMATCH );
  chk( "[\\-a]",    "-",          0, MATCH   );
  chk( "[\\-a]",    "a",          0, MATCH   );
  chk( "[a\\-c]",   "-",          0, MATCH   );
  chk( "[a\\-c]",   "a",          0, MATCH   );
  chk( "[a\\-c]",   "c",          0, MATCH   );
  chk( "[a\\-c]",   "b",          0, NOMATCH, "not a range" );
  chk( "[!a\\-]",   "-",          0, NOMATCH );
  chk( "[!a\\-]",   "z",          0, MATCH   );

  gr( "escaped ']' inside a charset -- the proper way to mean a literal ']'" );
  chk( "[\\]]",     "]",          0, MATCH   );
  chk( "[\\]]",     "a",          0, NOMATCH );
  chk( "[a\\]]",    "]",          0, MATCH   );
  chk( "[a\\]]",    "a",          0, MATCH   );
  chk( "[a\\]]",    "b",          0, NOMATCH );
  chk( "[!\\]]",    "]",          0, NOMATCH );
  chk( "[!\\]]",    "a",          0, MATCH   );

  gr( "']' without an opening '[' -- hard error" );
  chk( "]",         "]",          0, NOMATCH, "bare ']' is an error" );
  chk( "a]b",       "a]b",        0, NOMATCH, "bare ']' is an error" );
  chk( "]*",        "]ab",        0, NOMATCH, "bare ']' is an error" );
  chk( "a*]",       "abc]",       0, NOMATCH, "bare ']' is an error" );
  chk( "\\]",       "]",          0, MATCH,   "escape it to mean a literal ']'" );
  chk( "a\\]b",     "a]b",        0, MATCH,   "escape it to mean a literal ']'" );
  chk( "[a\\]]",    "]",          0, MATCH,   "inside a set it still needs escaping" );

  gr( "bracket edge cases" );
  chk( "[-a]",      "-",          0, MATCH   );
  chk( "[-a]",      "a",          0, MATCH   );
  chk( "[abc",      "[abc",       0, NOMATCH, "unterminated '[' is malformed" );
  chk( "a[b",       "a[b",        0, NOMATCH, "unterminated '[' is malformed" );
  chk( "a[b",       "ab",         0, NOMATCH );
  chk( "vf*[u*xz",  "vfu tar tar.xz", 0, NOMATCH, "unterminated '['" );
  chk( "vf*[u*xz",  "vfu[u tar.xz",   0, NOMATCH, "unterminated '['" );
  chk( "\\[abc",    "[abc",       0, MATCH,   "escape it to mean a literal '['" );
  chk( "a\\[b",     "a[b",        0, MATCH,   "escape it to mean a literal '['" );
  chk( "vf*[u]*xz", "vfu tar tar.xz", 0, MATCH   );
  chk( "vf*u*xz",   "vfu tar tar.xz", 0, MATCH   );
  chk( "a?[a]-",    "a[",         0, NOMATCH, "string ends before the set" );
  chk( "[a]",       "",           0, NOMATCH );

  gr( "typical file masks" );
  chk( "*.tar.gz",  "vfu-5.02.tar.gz",  0, MATCH   );
  chk( "*.tar*z",   "vfu-5.02.tar.xz",  0, MATCH   );
  chk( "*.tar*z",   "vfu-5.02_tar.xz",  0, NOMATCH );
  chk( "*.tar*z",   "vfu-5.tar.tar.xz", 0, MATCH   );
  chk( "*.tar*m*z", "vfu-5.tar.tar.xz", 0, NOMATCH );
  chk( "*tar*",     "vfu tar tar.xz",   0, MATCH   );
  chk( "*.deb",     "vfu.debug",        0, NOMATCH );
  chk( "*ing*",     "vstring.txt",      0, MATCH   );
  chk( "*.[ch]",    "vfu.c",            0, MATCH   );
  chk( "*.[ch]",    "vfu.o",            0, NOMATCH );
  chk( "*.cpp",     "vfu.cpp",          0, MATCH   );

  gr( "wide (wchar_t) build" );
  wchk( L"abc",           L"abc",           0, MATCH   );
  wchk( L"a?c",           L"abc",           0, MATCH   );
  wchk( L"a*c",           L"abbbc",         0, MATCH   );
  wchk( L"[a-f]",         L"c",             0, MATCH   );
  wchk( L"[!a-f]",        L"z",             0, MATCH   );
  wchk( L"vf\\*d[g-k]?z", L"vf*dirz",       0, MATCH   );
  wchk( L"vf*\\?*r",      L"vfutest?xdir",  0, MATCH   );
  wchk( L"vF\\*d[g-k]?z", L"Vf*dIrz",       SFN_CASEFOLD, MATCH );
  wchk( L"x\\ay",         L"XAY",           SFN_CASEFOLD, MATCH );
  wchk( L"[a\\-]",        L"-",             0, MATCH   );
  wchk( L"a[b",           L"a[b",           0, NOMATCH );
  wchk( L"a\\[b",         L"a[b",           0, MATCH   );
  wchk( L"\xe4\xf6\xfc",  L"\xe4\xf6\xfc",  0, MATCH   );
  wchk( L"\xe4?\xfc",     L"\xe4\xf6\xfc",  0, MATCH   );
  wchk( L"\xe4*",         L"\xe4\xf6\xfc",  0, MATCH   );
}

/****************************************************************************
** part 2 -- exhaustive sweep against fnmatch(3)
**
** every pattern over PAT_ALPHA of length 1..PAT_MAX is tried against every
** string over STR_ALPHA of length 1..STR_MAX. '\' is left out of the
** alphabet, escaping is covered by the named cases above.
****************************************************************************/

static const char* PAT_ALPHA = "ab*?[]-!";
static const char* STR_ALPHA = "ab[]-!";
#define PAT_MAX 4
#define STR_MAX 3

/* builds the n-th string of length len over alpha, returns 0 when done */
static int nth( char* out, int len, const char* alpha, long n )
{
  int base = strlen( alpha );
  for( int i = len - 1; i >= 0; i-- )
    {
    out[i] = alpha[ n % base ];
    n /= base;
    }
  out[len] = 0;
  return n == 0;
}

/* fnmatch(3) falls back to a literal '[' when there is no closing ']',   */
/* sfn_match treats the charset as malformed and does not match, so these  */
/* disagree by design and are left out of the comparison                   */
static int unterminated_bracket( const char* p )
{
  const char* b = strrchr( p, '[' );
  return b && ! strchr( b + 1, ']' );
}

/* same story for ']': fnmatch(3) reads a ']' right after '[' (or '[!') as  */
/* a literal one, sfn_match wants [\]] -- another divergence by design      */
static int bare_close_in_set( const char* p )
{
  for( const char* b = strchr( p, '[' ); b; b = strchr( b + 1, '[' ) )
    {
    const char* q = b + 1;
    if( *q == '!' || *q == '^' ) q++;
    if( *q == ']' ) return 1;
    }
  return 0;
}

/* a ']' with no opening '[' is a hard error for sfn_match, fnmatch(3)     */
/* takes it as a literal -- divergence by design, leave it out             */
static int bare_close_bracket( const char* p )
{
  int in_set = 0;
  for( const char* q = p; *q; q++ )
    {
    if( *q == '\\' && q[1] ) { q++; continue; }
    if( in_set )
      { if( *q == ']' ) in_set = 0; }
    else if( *q == '[' )
      { in_set = 1; if( q[1] == '!' || q[1] == '^' ) q++; }
    else if( *q == ']' )
      return 1;
    }
  return 0;
}

static long pow_l( long b, int e )
{
  long r = 1;
  while( e-- > 0 ) r *= b;
  return r;
}

void exhaustive()
{
  VArray found;   /* one "shape<TAB>example" line per disagreement */

  long compared = 0;
  long differ   = 0;
  long skipped  = 0;

  char pat[ PAT_MAX + 1 ];
  char str[ STR_MAX + 1 ];

  for( int pl = 1; pl <= PAT_MAX; pl++ )
    {
    long pn = pow_l( strlen( PAT_ALPHA ), pl );
    for( long pi = 0; pi < pn; pi++ )
      {
      nth( pat, pl, PAT_ALPHA, pi );
      for( int sl = 1; sl <= STR_MAX; sl++ )
        {
        long sn = pow_l( strlen( STR_ALPHA ), sl );
        for( long si = 0; si < sn; si++ )
          {
          nth( str, sl, STR_ALPHA, si );

          /* fnmatch(3) reads "a-]" as a literal '-', sfn_match wants [a\-],  */
          /* so these disagree by design -- leave them out of the comparison  */
          if( strstr( pat, "-]" ) || unterminated_bracket( pat ) ||
              bare_close_in_set( pat ) || bare_close_bracket( pat ) )
            { skipped++; continue; }

          char* hp = (char*)malloc( pl + 1 );
          char* hs = (char*)malloc( sl + 1 );
          strcpy( hp, pat );
          strcpy( hs, str );
          int mine = sfn_match( hp, hs, 0 ) == 0;
          free( hp );
          free( hs );

          int theirs = fnmatch( pat, str, 0 ) == 0;

          compared++;
          if( mine == theirs ) continue;
          differ++;

          VString shape = pat;
          str_tr( shape, "ab", "XX" );
          VString line = shape;
          line = line + "\t" + pat + " vs " + str + "  sfn=";
          line = line + ( mine ? "MATCH" : "no" ) + " fnmatch=" + ( theirs ? "MATCH" : "no" );
          found.push( line );
          }
        }
      }
    }

  printf( "\n  skipped       : %ld malformed ('-]', '[]', bare ']', no closing ']')\n", skipped );
  printf( "  compared      : %ld pattern/string pairs\n", compared );
  printf( "  agree         : %ld\n", compared - differ );
  printf( "  disagree      : %ld\n", differ );

  tests_failed += differ;
  if( differ == 0 ) return;

  /* group the "shape<TAB>example" lines into runs of equal shape */
  found.sort();
  VArray shape_list;
  VArray count_list;
  VArray sample_list;
  for( int z = 0; z < found.count(); z++ )
    {
    VArray parts = str_split( "\t", found.get( z ) );
    VString sh = parts[0];
    int last = shape_list.count() - 1;
    if( last >= 0 && sh == shape_list[ last ] )
      count_list.set( last, VString( atoi( count_list.get( last ) ) + 1 ) );
    else
      {
      shape_list.push( sh );
      count_list.push( "1" );
      sample_list.push( parts[1] );
      }
    }

  /* show the heaviest shapes first */
  printf( "\n  %d distinct pattern shapes disagree (a,b shown as X), worst first:\n\n",
          shape_list.count() );
  int shown = 0;
  while( shown < 20 )
    {
    int best = -1;
    for( int z = 0; z < shape_list.count(); z++ )
      if( atoi( count_list.get( z ) ) > 0 &&
          ( best == -1 || atoi( count_list.get( z ) ) > atoi( count_list.get( best ) ) ) )
        best = z;
    if( best == -1 ) break;
    printf( "    %-8s %5s cases   e.g.  %s\n",
            shape_list.get( best ), count_list.get( best ), sample_list.get( best ) );
    count_list.set( best, "0" );
    shown++;
    }
}

/****************************************************************************/

int main( int argc, char** argv )
{
  setlocale( LC_ALL, "" );
  for( int z = 1; z < argc; z++ )
    if( strcmp( argv[z], "-q" ) == 0 ) quiet = 1;

  printf( "=========================================================================\n" );
  printf( " sfn_match() -- part 1: named cases\n" );
  printf( "=========================================================================\n" );
  named_cases();

  int named_run    = tests_run;
  int named_failed = tests_failed;

  printf( "\n=========================================================================\n" );
  printf( " sfn_match() -- part 2: exhaustive sweep against fnmatch(3)\n" );
  printf( "=========================================================================\n" );
  exhaustive();

  printf( "\n=========================================================================\n" );
  printf( " named cases : %d run, %d failed\n", named_run, named_failed );
  printf( " sweep       : %d disagreements\n", tests_failed - named_failed );
  printf( "=========================================================================\n" );

  return tests_failed != 0;
}
