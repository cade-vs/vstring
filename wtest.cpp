/****************************************************************************
 *
 *  VSTRING Library
 *
 *  Copyright (c) 1996-2022 Vladi Belperchinov-Shabanski "Cade" 
 *  http://cade.noxrun.com/  <cade@noxrun.com> <cade@bis.bg> <cade@cpan.org>
 *
 *  Distributed under the GPL license, you should receive copy of GPL!
 *
 *  SEE `README',`LICENSE' OR `COPYING' FILE FOR LICENSE AND OTHER DETAILS!
 *
 *  VSTRING library provides wide set of string manipulation features
 *  including dynamic string object that can be freely exchanged with
 *  standard char* type, so there is no need to change function calls
 *  nor the implementation when you change from char* to WString (and
 *  vice versa). The main difference from other similar libs is that
 *  the dynamic WString class has no visible methods (except operators)
 *  so you will use it as a plain char* but it will expand/shrink as
 *  needed.
 *
 *  If you find bug or you have note about vstring lib, please feel
 *  free to contact me.
 *
 ***************************************************************************/

#include <stdio.h>
#include <locale.h>
#include "vstring.h"
#include "wstring.h"
#include "vstrlib.h"
#include "wstrlib.h"

typedef VString VPath;

void test1()
{
  WString str = L"Hello";
  str += L" World"; // str is `Hello World' now
  str_reverse( str ); // str is `dlroW olleH' now
  ASSERT( str == L"dlroW olleH" );
  str_low( str ); // lower case
  ASSERT( str == L"dlrow olleh" );

  wprintf( L"************************ test 1 mid: %ls\n", str.data() ); // this should print `hello world'

  WArray va = str_split( L" +", str ); // array contains `dlroW' at pos 0 and `olleH' at 1
  va.print();

  va.reverse(); // array reversed: `dlroW' at pos 1 and `olleH' at 0

  int z;
  for( z = 0; z < va.count(); z++ )
    {
    str_reverse( va[z] ); // reverses each string element
    }

  str = str_join( va, L" " ); // joins into temporary string

  wprintf( L"************************ test 1 result is: %ls\n", str.data() ); // this should print `hello world'

  ASSERT( str == L"hello world" );
  
  str.compact( 1 );
  str.fix();
  
  str_add_ch( str, '!' );
  str_add_ch( str, '?' );
  str_add_ch( str, '*' );
}

void test2()
{
  WArray va;
  va.push( L"hello" ); // pos 0
  va.push( L"world" ); // pos 1

  va.ins( 1, L"your" ); // pos 1 shifted

  va[1] = L"my"; // replaces `your'
  va[3] = L"!";  // set outside the size, array is extended

  WString str = va.pop(); // pops last element, str is now `!'

  str = str_join( va, L"-" ); // joins to given string

  str_tr( str, L"-", L" " ); // replaces dashes with spaces

  str_replace( str, L" my ", L" " ); // removes ` my '

  wprintf( L"************************ test 2 result is: %ls\n", str.data() ); // this should print `hello world'
  ASSERT( str == L"hello world" );
  
  WString res1;
  str_copy( res1, str, 3, 5 );
  ASSERT( res1 == L"lo wo" );

  WString res2;
  str_copy( res2, str, 8, 9 );
  ASSERT( res2 == L"rld" );
}

void test3()
{
  WTrie  tr; // hash-like
  WArray va;

  // inserting keys and values
  tr[ L"tralala" ] = L"data1";
  tr[ L"opala"   ] = L"data2";
  tr[ L"keynext" ] = L"data3";

  // inserting elements into the array
  va.push( L"this" );
  va.push( L"just" );
  va.push( L"test" );
  va.push( L"simple" );

  // adding string to the first element of the array
  va[1] += L" x2";

  // the array is converted to trie (hash) and merged into `tr'
  tr += va; // same as: tr.merge( &va );

  ASSERT( tr[ L"this" ] == L"just x2" );

  // clear the array--remove all elements
  va.undef();

  // take keys from `tr' as array and store them into va, returns count
  // i.e. i = tr.count();
  int i;
  va = tr.keys();

  wprintf( L"keys count = %d\n", va.count() );
  ASSERT( va.count() == 5 );

  // printing the array and trie data
  for( i = 0; i < va.count(); i++ )
    {
    wprintf( L"%d -> %ls (%ls)\n", i, va[i].data(), tr[ va[i] ].data() );
    }

  WArray v1;

  wprintf( L"--------------------\n" );
  v1 = tr;    // same as: v1.undef; v1.push( &tr );
  v1.print(); // print array data

  WRegexp re( L"a([0-9]+)" ); // compiling new regexp
  wprintf( L"regexp error: %ls\n", re.error_str() );

  if( re.m( L"tralala85.zz" ) ) // match against regexp
    {
    wprintf( L"sub 0 = %ls\n", re[0].data() ); // re[0] returns `a85'
    wprintf( L"sub 1 = %ls\n", re[1].data() ); // re[1] returns `85'
    ASSERT( re[0] == L"a85" );
    ASSERT( re[1] == L"85" );
    }

  WString vs;
  if( re.m( L"tralala85.", L"a(la)+" ) ) // match against regexp
    {
    wprintf( L"sub 0 = %ls\n", re[0].data() ); // `alala'
    wprintf( L"sub 1 = %ls\n", re[1].data() ); // `la'
    ASSERT( re[0] == L"alala" );
    ASSERT( re[1] == L"la"    );
    }

  wprintf( L"--------------------\n" );
  v1 = str_split( L",", L"*.tralala,opala and another   one" ); // splits on spaces
  v1.print();

  WString js1 = str_join( v1, L"---" );
  wprintf( L"joined: %ls\n", (const wchar_t*)js1 ); // join the same data back
  ASSERT( js1 == L"*.tralala---opala and another   one" );
  
  WString m1 = v1[0];
  WString m2 = v1[1];
  wprintf( L"1[%ls] 2[%ls]\n", m1.data(), m2.data() );

  wprintf( L"--------------------\n" );
  v1 = str_split( L" +", L"tralala  opala and another   one", 3 ); // splits data on spaces up to 3 elements
  v1.print();
  ASSERT( v1[2] == L"and another   one" );

//exit(1);

  wprintf( L"--------------------\n" );
  v1[1] = L"hack this one here"; // set (overwrite) element 1
  str_sleft( v1[2], 11 ); // reset element 2 to the left 11 chars only
  v1[0] = 12345; // convert integer into string
  v1.print();

  wprintf( L"--------------------\n" );
  WArray aa[3]; // array of arrays

  aa[0] = str_split( L" ", L"this is just a simple test" );
  aa[1] = str_split( L" ", L"never ending story" );
  aa[2] = str_split( L" ", L"star-wars rulez" );

  aa[0][1] = L"was"; // first array, second element, replaces `is' with `was'
  aa[2][0] = L"slackware"; // third array, first element, `star-wars' is now `slackware'

  // expands the array from 3 to 11 elements
  aa[1][10] = L"king of the hill";

  for( i = 0; i < 3; i++ )
    {
    wprintf( L"---\n");
    aa[i].print();
    }

  wprintf( L"---box test-----------------------------\n" );
  i = 20;
  while( i-- )
  {
  v1.push( L"this" );
  v1.push( L"just" );
  v1.push( L"test" );
  v1.push( L"simple" );
  }

  v1.print();
  WArray vv = v1; // this makes vv data aliased to the data of v1
  vv.print(); // actually print the v1's data which is shared right now
  vv.set( 0, L"---" ); // vv makes own copy of the array data
  vv.print(); // vv's data is no more aliased to v1's



  WRegexp re_see( L"^\\s*see\\s*=\\s*([^, \011]*)\\s*,(.*)$", L"i" );
  if( re_see.m( L"see=*.tgz,tralala" ) )
    {
    WString str;
    str = str + re_see[1] + L":" + re_see[2];
    wprintf( L"WRegexp[1+2]=[%ls]\n", str.data() );
    ASSERT( str == L"*.tgz:tralala" );
    }

  wprintf( L"************************ test 3 ends here\n" );
}

void test4()
{
  // this is regression test, please ignore it...

  int i;
  int ii;

  WArray va;
  ii = 20;
  i = ii;
  while( i-- )
    {
    va = str_split( L",", L"this is, just a simple. but fixed, nonsense test, voila :)" );
    ASSERT( va.count() == 4 );
    printf( "%d%% va count = %d\n", (100*i)/ii, va.count() );
    }

  WString set;
  WString cat;
  WString setn;
  WString catn;
  WString sete;
  WString setp;

  i = 2000;

  while( i-- )
    {
    set.set( L"this is, just a simple. but fixed, nonsense test, voila :)" );
    cat.cat( L"this is, just a simple. but fixed, nonsense test, voila :)" );
    setn.setn( L"this is, just a simple. but fixed, nonsense test, voila :)", 20 );
    catn.catn( L"this is, just a simple. but fixed, nonsense test, voila :)", 20 );

    sete = L"this is, just a simple. but fixed, nonsense test, voila :)";
    setp += L"this is, just a simple. but fixed, nonsense test, voila :)";
    }

  printf( "set  = %ld\n", str_len( set  ) );
  printf( "cat  = %ld\n", str_len( cat  ) );
  printf( "setn = %ld\n", str_len( setn ) );
  printf( "catn = %ld\n", str_len( catn ) );
  printf( "sete = %ld\n", str_len( sete ) );
  printf( "setp = %ld\n", str_len( setp ) );

  printf( "--------------------\n" );

  i = 2000;
  while( i-- )
    {
    set = L"this is, just a simple. but fixed, nonsense test, voila :)";
    setn = set;
    str_del( set, 20, 10 );
    str_ins( set, 30, L"***opa***" );
    str_del( set, 40, 100 );
    str_replace( setn, L"i", L"[I]" );
    ASSERT( set == L"this is, just a simpxed, nonse***opa***n" );
    ASSERT( str_len(set) == str_len(L"this is, just a simpxed, nonse***opa***n") );
    ASSERT( setn == L"th[I]s [I]s, just a s[I]mple. but f[I]xed, nonsense test, vo[I]la :)" );
    ASSERT( str_len(setn) == str_len(L"th[I]s [I]s, just a s[I]mple. but f[I]xed, nonsense test, vo[I]la :)") );
    }
  printf( "set  = %ls\n", set.data() );
  printf( "setn = %ls\n", setn.data() );

  printf( "---array sort-------\n" );
  va.undef();
  va = str_split( L"[, \t]+", L"this is, just a simple. but fixed, nonsense test, voila :)" );
  va.sort();
  va.print();
  printf( "--------------------\n" );
  va.sort( 1 );
  va.print();
  printf( "--------------------\n" );

}

/*
void test5()
{
  WTrie tr; // hash-like
  WArray va;

  // inserting keys and values
  tr[ "key1" ] = "data1";
  tr[ "key2" ] = "data2";
  tr[ "key3" ] = "data3";
  tr[ "key4" ] = "data4";
  tr[ "key5" ] = "data5";

  tr.print();
  
exit(1);  
  
  tr.reverse();
  tr.print();
  tr.reverse();
  tr.print();

  printf( "************************ test 5 ends here\n" );
}

void test6()
{
  WRegexp re;
  WArray va;

  re.comp( "^([^!]+)!(.+)=apquxz(.+)$" );
  int i = re.m( "abc!pqr=apquxz.ixr.zzz.ac.uk" );
  i--;
  while( i >= 0 )
    {
    va.push( re[i] );
    i--;
    }
  va.print();

  va.undef();
  va += "/this/is/samle/file.tail";
  va += "/file.tail";
  va += "/this/is/./samle/file.tail/";
  va += "/this/..../is/../samle/.file.tail";
  va += "/.file.tail";
  va += "/";

  const char* ps;

  va.reset();
  while( ( ps = va.next() ) )
    {
    printf( "------------------------------------\n" );
    printf( "file is: %ls\n", ps );
    printf( "path is: %ls\n", (const char*)str_file_path( ps ) );
    printf( "name is: %ls\n", (const char*)str_file_name( ps ) );
    printf( "ext  is: %ls\n", (const char*)str_file_ext( ps ) );
    printf( "n+ex is: %ls\n", (const char*)str_file_name_ext( ps ) );
    printf( "reduced path is: %ls\n", (const char*)str_reduce_path( ps ) );
    printf( "dot reduce sample is: %ls\n", (const char*)str_dot_reduce( ps, 10 ) );
    }

  va.fsave( "/tmp/a.aaa" );
  va.fload( "/tmp/a.aaa" );
  va.print();
}

void test7()
{
  WTrie tr; // hash-like
  WTrie tr2; // hash-like
  WArray va;

  // inserting keys and values
  tr[ "key1" ] = "data1";
  tr[ "key2" ] = "data2";
  tr[ "key3" ] = "data3";

  tr.print();
  printf( "---------------------------------1---\n" );
  tr.reverse();
  tr.print();
  printf( "---------------------------------2---\n" );
  tr.reverse();
  tr.print();
  printf( "---------------------------------3---\n" );

  tr2 = str_split( " ", "this is simple one way test" );
  tr2.print();
  printf( "---------------------------------4---\n" );

  tr2 += tr;
  tr2.print();
  printf( "---------------------------------5---\n" );

  va = tr2;
  va.print();
  printf( "---------------------------------6---\n" );
}

void test8()
{
  WString v1;
  WString v2;

  v1 = "this is simple test ";
  v1 *= 1024;

  printf( "v1 len: %d\n", str_len( v1 ) );

  v2.compact( 1 ); // makes v2 compact, i.e. it will get as much memory as it
                   // needs. otherwise it will get fixed amount of blocks

  v2 = v1; // data is shared between v1 and v2. any change to v1 or v2 will
           // detach this data and both will get own copy

  v2[0] = ' '; // this will create own data for v2

  str_tr( v2, "ti", "TI" ); // capitalize T and I

  v2 = ""; // this will free all data allocated by v2

  printf( "copy 7,6: [%ls]", (const char*)str_copy( v2, v1, 8, 6 ) );
  printf( "copy 10: [%ls]", (const char*)str_copy( v2, v1, -10 ) );

  printf( "************************ test 5 ends here\n" );
}

void test9()
{
  WArray va;
  WTrie  tr;
  
  printf( "---9---------------------------------------------------\n" );
  
  va.push( "one" );
  va.push( "two" );
  va.push( "tri" );
  va.push( "pet" );

  va.print();
  
  tr = va;
  
  tr.print();

  printf( "\n            ----\n" );

  va.push( tr );
  
  va.print();
  
  WArray va2;
  va2.push( "1" );
  va2.push( "2" );
  va2.push( "3" );
  va2.push( "4" );

  va2.unshift( "0" );
  va2.unshift( va );
  va2.push( va );

  va2.print();
  
}
*/

void print_vpath( VPath& vp )
{
  wprintf( L"pp = %ls\n", WString( vp.data() ).data() );
}

void test10()
{
  WArray va;
  WTrie  tr;
  
    wint_t x = 5;
    wchar_t name[] = L"GEEKS";
    wprintf( L"x = %d \n", x);
    wprintf( L"HELLO %ls \n", name);

  fwide( stdout, 0 );
  printf( "--- WIDE CHAR TESTS -----------------------------------\n" );

  fwide( stdout, 1 );
  
  wchar_t t[256];
  wcscpy( t, L"Това е проста проба щеш не щеш" );
  
  wprintf( L"%ls\n", L"Това е проста проба щеш не щеш" );
  
  printf( "--- test 10 ends --------------------------------------\n" );

  WString ws( "щото това е конвертиране" );
  wprintf( L"%ls\n", ws.data() );
  
  ws = "и това е пак също ковертиране";
  wprintf( L"%ls\n", ws.data() );

  VString vv = ws;
  WString ww = vv;
  ww += L" x2";
  wprintf( L"%ls\n", ww.data() );

  VPath pp = ws;
  pp = str_dot_reduce( pp.data(), 11 );
  vv = 123;
  print_vpath( vv );
  
  str_dot_reduce( ww, 16 );
}

void test11()
{

  ASSERT( sfn_match( "vf*", "vfudir"      ) == 0 );
  ASSERT( sfn_match( "vf*r", "vfudir"     ) == 0 );
  ASSERT( sfn_match( "vf*t", "vfudir"     ) != 0 );
  ASSERT( sfn_match( "vf[you]*", "vfudir" ) == 0 );

  ASSERT( sfn_match( "vf\\*d[g-k]?z", "vf*dirz" ) == 0 );
  ASSERT( sfn_match( "v*[c-e]*", "vfudirtest.txt" ) == 0 );
  ASSERT( sfn_match( "v*xt**", "vfudirtest.txt" ) == 0 );
  ASSERT( sfn_match( "v*xt**?", "vfudirtest.txt" ) != 0 );

  ASSERT( sfn_match( "vf*i*r", "vfudir"     ) == 0 );
  ASSERT( sfn_match( "vf*x*r", "vfudir"     ) != 0 );

  ASSERT( sfn_match( "vf*\\?*r", "vfutest?xdir"     ) == 0 );

  ASSERT( sfn_match( "??*", "vfutest"  ) == 0 );
  ASSERT( sfn_match( "??*", "vf"       ) == 0 );
  ASSERT( sfn_match( "??*", "v"        ) != 0 );
  ASSERT( sfn_match( "*vfu*?", "vfuz"  ) == 0 );

  ASSERT( sfn_match( "vf[^you]*", "vfudir" ) != 0 );

  ASSERT( sfn_match( "vf\\u*", "vfudir", SFN_NOESCAPE ) != 0 );
  ASSERT( sfn_match( "vf\\u*", "vf\\udir", SFN_NOESCAPE ) == 0 );

  ASSERT( sfn_match( "vf\\U*", "Vf\\udir", SFN_NOESCAPE ) != 0 );
  ASSERT( sfn_match( "vf\\U*", "Vf\\udir", SFN_NOESCAPE | SFN_CASEFOLD ) == 0 );
  ASSERT( sfn_match( "vF\\*d[g-k]?z", "Vf*dIrz", SFN_CASEFOLD ) == 0 );


  ASSERT( sfn_match( L"vf\\*d[g-k]?z", L"vf*dirz"  ) == 0 );
  ASSERT( sfn_match( L"vf*\\?*r", L"vfutest?xdir"  ) == 0 );
  ASSERT( sfn_match( L"vF\\*d[g-k]?z", L"Vf*dIrz", SFN_CASEFOLD ) == 0 );

  ASSERT( sfn_match( "*ing*", "vstring.txt"  ) == 0 );
}

int main( void )
{
  setlocale( LC_ALL, "" );
  #if 0
  char t[256] = "123456----------------------------------------9999999999999";
  char T[256] = "123456----------------------------------------9999999999999";
  str_trim_left( t, 3 );
  printf( "%ls\n", t );

  for( long z; z < 300000000; z++ )
  {
  //str_copy( t+10, t,    0, 15 ); // check for overlapping borders, begin of str
  //str_copy( t+10, t+20, 0, 15 ); // check for overlapping borders, end   of str
  //memmove( T, t, 222 );
  //memcpy( T, t, 222 );
  //str_copy( T, t, 0, 222 ); // check for overlapping borders, begin of str
  }
  #endif

  wchar_t t[92] = L"this is simple test";
  wchar_t r[92] = L"1111111111111111111";
  str_word( t, L" ", r );
  ASSERT( wcscmp( t, L"is simple test" ) == 0 );
  ASSERT( wcscmp( r, L"this" ) == 0 );

  str_set( t, L"   opa" );
  str_cut_left( t, L" " );
  ASSERT( wcscmp( t, L"opa" ) == 0 );

  str_set( t, L"opa   " );
  str_cut_right( t, L" " );
  ASSERT( wcscmp( t, L"opa" ) == 0 );

  str_set( t, L"this is good" );
  str_ins( t, 8, L"not " );
  ASSERT( wcscmp( t, L"this is not good" ) == 0 );

  str_del( t, 8, 4 );
  ASSERT( wcscmp( t, L"this is good" ) == 0 );

  str_set( t, L"more" );
  str_mul( t, 6 );
  ASSERT( wcscmp( t, L"moremoremoremoremoremore" ) == 0 );
  WString s = L"more";
  str_mul( s, 6 );
  ASSERT( s == L"moremoremoremoremoremore" );
  ASSERT( str_len( s ) == str_len((const wchar_t*)s) );

  str_copy( t+10, t,    0, 15 ); // check for overlapping borders, begin of str
  str_copy( t+10, t+20, 0, 15 ); // check for overlapping borders, end   of str

  str_set( t, L"despicable me" );
  str_word( t, L" ", r );
  ASSERT( wcscmp( r, L"despicable" ) == 0 );
  str_word( t, L" ", r );
  ASSERT( wcscmp( r, L"me" ) == 0 );
  ASSERT( t[0] == 0 );

  #if 1
  test1();
  test2();
  test3();
  test4();
  #if 0
  test5();
  test6();
  test7();
  test8();
  test9();
  #endif
  test10();
  test11();

  #endif
  return 0;
}
