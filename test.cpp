/****************************************************************************
 *
 *  VSTRING Library
 *
 *  Copyright (c) 1996-2020 Vladi Belperchinov-Shabanski "Cade" 
 *
 *  http://cade.datamax.bg/  <cade@biscom.net> <cade@bis.bg> <cade@datamax.bg>
 *  Distributed under the GPL license, you should receive copy of GPL!
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
 ***************************************************************************/

#include <stdio.h>
#include "vstrlib.h"

void test1()
{
  VString str = "Hello";
  str += " World"; // str is `Hello World' now
  str_reverse( str ); // str is `dlroW olleH' now
  str_low( str ); // lower case

  VArray va = str_split( " +", str ); // array contains `dlroW' at pos 0 and `olleH' at 1

  va.reverse(); // array reversed: `dlroW' at pos 1 and `olleH' at 0

  int z;
  for( z = 0; z < va.count(); z++ )
    {
    str_reverse( va[z] ); // reverses each string element
    }


  str = str_join( va, " " ); // joins into temporary string

  printf( "************************ test 1 result is: %s\n", str.data() ); // this should print `hello world'
}

void test2()
{
  VArray va;
  va.push( "hello" ); // pos 0
  va.push( "world" ); // pos 1

  va.ins( 1, "your" ); // pos 1 shifted

  va[1] = "my"; // replaces `your'
  va[3] = "!";  // set outside the size, array is extended

  VString str = va.pop(); // pops last element, str is now `!'

  str = str_join( va, "-" ); // joins to given string

  str_tr( str, "-", " " ); // replaces dashes with spaces

  str_replace( str, " my ", " " ); // removes ` my '

  printf( "************************ test 2 result is: %s\n", str.data() ); // this should print `hello world'
}

void test3()
{
  VTrie tr; // hash-like
  VArray va;

  // inserting keys and values
  tr[ "tralala" ] = "data1";
  tr[ "opala"   ] = "data2";
  tr[ "keynext" ] =  "data3";

  // inserting elements into the array
  va.push( "this" );
  va.push( "just" );
  va.push( "test" );
  va.push( "simple" );

  // adding string to the first element of the array
  va[1] += " x2";

  // the array is converted to trie (hash) and merged into `tr'
  tr += va; // same as: tr.merge( &va );

  // clear the array--remove all elements
  va.undef();

  // take keys from `tr' as array and store them into va, returns count
  // i.e. i = tr.count();
  int i;
  va = tr.keys();

  printf( "keys count = %d\n", va.count() );

  // printing the array and trie data
  for( i = 0; i < va.count(); i++ )
    {
    printf( "%d -> %s (%s)\n", i, va[i].data(), tr[ va[i] ].data() );
    }

  VArray v1;

  printf( "--------------------\n" );
  v1 = tr;    // same as: v1.undef; v1.push( &tr );
  v1.print(); // print array data

  VRegexp re( "a([0-9]+)" ); // compiling new regexp
  printf( "regexp error: %s\n", re.error_str() );

  if( re.m( "tralala85.zz" ) ) // match against regexp
    {
    printf( "sub 0 = %s\n", re[0].data() ); // re[1] returns `85'
    printf( "sub 1 = %s\n", re[1].data() ); // re[1] returns `85'
    }

  VString vs;
  if( re.m( "tralala85.", "a(la)+" ) ) // match against regexp
    {
    printf( "sub 0 = %s\n", re[0].data() ); // `lala'
    printf( "sub 1 = %s\n", re[1].data() ); // `la'
    }

  printf( "--------------------\n" );
  v1 = str_split( ",", "*.tralala,opala and another   one" ); // splits on spaces
  v1.print();

  printf( "joined: %s\n", (const char*)str_join( v1, "---" ) ); // join the same data back
  VString m1 = v1[0];
  VString m2 = v1[1];
  printf( "1[%s] 2[%s]\n", m1.data(), m2.data() );

  printf( "--------------------\n" );
  v1 = str_split( " +", "tralala  opala and another   one", 3 ); // splits data on spaces up to 3 elements
  v1.print();

//exit(1);

  printf( "--------------------\n" );
  v1[1] = "hack this one here"; // set (overwrite) element 1
  str_sleft( v1[2], 11 ); // reset element 2 to the left 11 chars only
  v1[0] = 12345; // convert integer into string
  v1.print();

  printf( "--------------------\n" );
  VArray aa[3]; // array of arrays

  aa[0] = str_split( " ", "this is just a simple test" );
  aa[1] = str_split( " ", "never ending story" );
  aa[2] = str_split( " ", "star-wars rulez" );

  aa[0][1] = "was"; // first array, second element, replaces `is' with `was'
  aa[2][0] = "slackware"; // third array, first element, `star-wars' is now `slackware'

  // expands the array from 3 to 11 elements
  aa[1][10] = "king of the hill";

  for( i = 0; i < 3; i++ )
    {
    printf("---\n");
    aa[i].print();
    }

  printf( "---box test-----------------------------\n" );
  i = 20;
  while( i-- )
  {
  v1.push( "this" );
  v1.push( "just" );
  v1.push( "test" );
  v1.push( "simple" );
  }

  v1.print();
  VArray vv = v1; // this makes vv data aliased to the data of v1
  vv.print(); // actually print the v1's data which is shared right now
  vv.set( 0, "---" ); // vv makes own copy of the array data
  vv.print(); // vv's data is no more aliased to v1's



  VRegexp re_see( "^\\s*see\\s*=\\s*([^, \011]*)\\s*,(.*)$", "i" );
  if( re_see.m( "see=*.tgz,tralala" ) )
    {
    VString str;
    str = str + re_see[1] + re_see[2];
    printf( "VRegexp[1+2]=[%s]\n", str.data() );
    }

  printf( "************************ test 3 ends here\n" );
}

void test4()
{
  // this is regression test, please ignore it...

  int i;
  int ii;

  VArray va;
  ii = 20;
  i = ii;
  while( i-- )
    {
    va = str_split( ",", "this is, just a simple. but fixed, nonsense test, voila :)" );
    printf( "%d%% va count = %d\n", (100*i)/ii, va.count() );
    }

  VString set;
  VString cat;
  VString setn;
  VString catn;
  VString sete;
  VString setp;

  i = 2000;

  while( i-- )
    {
    set.set( "this is, just a simple. but fixed, nonsense test, voila :)" );
    cat.cat( "this is, just a simple. but fixed, nonsense test, voila :)" );
    setn.setn( "this is, just a simple. but fixed, nonsense test, voila :)", 20 );
    catn.catn( "this is, just a simple. but fixed, nonsense test, voila :)", 20 );

    sete = "this is, just a simple. but fixed, nonsense test, voila :)";
    setp += "this is, just a simple. but fixed, nonsense test, voila :)";
    }

  printf( "set  = %d\n", str_len( set  ) );
  printf( "cat  = %d\n", str_len( cat  ) );
  printf( "setn = %d\n", str_len( setn ) );
  printf( "catn = %d\n", str_len( catn ) );
  printf( "sete = %d\n", str_len( sete ) );
  printf( "setp = %d\n", str_len( setp ) );

  printf( "--------------------\n" );

  i = 2000;
  while( i-- )
    {
    set = "this is, just a simple. but fixed, nonsense test, voila :)";
    setn = set;
    str_del( set, 20, 10 );
    str_ins( set, 30, "***opa***" );
    str_replace( setn, "i", "[I]" );
    }
  printf( "set  = %s\n", set.data() );
  printf( "setn = %s\n", setn.data() );

  printf( "---array sort-------\n" );
  va.undef();
  va = str_split( "[, \t]+", "this is, just a simple. but fixed, nonsense test, voila :)" );
  va.sort();
  va.print();
  printf( "--------------------\n" );
  va.sort( 1 );
  va.print();
  printf( "--------------------\n" );

}

void test5()
{
  VTrie tr; // hash-like
  VArray va;

  // inserting keys and values
  tr[ "key1" ] = "data1";
  tr[ "key2" ] = "data2";
  tr[ "key3" ] = "data3";

  tr.print();
  tr.reverse();
  tr.print();
  tr.reverse();
  tr.print();

  VCharSet cs;

  cs.push( 'a' );
  printf( "char_set: %d, %d\n", cs.in( 'a' ), cs.in( 'z' ) );
  cs.undef( 'a' );
  printf( "char_set: %d, %d\n", cs.in( 'a' ), cs.in( 'z' ) );
  cs.undef();

  int i = 2000;
  while( i-- )
    {
    cs.push( i );
    }
  cs.undef();


  printf( "************************ test 5 ends here\n" );
}

void test6()
{
  VRegexp re;
  VArray va;

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
    printf( "file is: %s\n", ps );
    printf( "path is: %s\n", (const char*)str_file_path( ps ) );
    printf( "name is: %s\n", (const char*)str_file_name( ps ) );
    printf( "ext  is: %s\n", (const char*)str_file_ext( ps ) );
    printf( "n+ex is: %s\n", (const char*)str_file_name_ext( ps ) );
    printf( "reduced path is: %s\n", (const char*)str_reduce_path( ps ) );
    printf( "dot reduce sample is: %s\n", (const char*)str_dot_reduce( ps, 10 ) );
    }

  va.fsave( "/tmp/a.aaa" );
  va.fload( "/tmp/a.aaa" );
  va.print();
}

void test7()
{
  VTrie tr; // hash-like
  VTrie tr2; // hash-like
  VArray va;

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
  VString v1;
  VString v2;

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

  printf( "copy 7,6: [%s]", (const char*)str_copy( v2, v1, 8, 6 ) );
  printf( "copy 10: [%s]", (const char*)str_copy( v2, v1, -10 ) );

  printf( "************************ test 5 ends here\n" );
}

void test9()
{
  VArray va;
  VTrie  tr;
  
  printf( "---9---------------------------------------------------\n" );
  
  va.push( "one" );
  va.push( "two" );
  va.push( "tri" );
  va.push( "pet" );
  
  tr = va;
  
  tr.print();

  va.push( &tr );
  
  va.print();
  
  VArray va2;
  va2.push( "1" );
  va2.push( "2" );
  va2.push( "3" );
  va2.push( "4" );

  va2.unshift( "0" );
  va2.unshift( &va );
  va2.push( &va );

  va2.print();
  
}

int main( int argc, char* argv[] )
{

  /*
  char t[256] = "123456----------------------------------------9999999999999";
  char T[256] = "123456----------------------------------------9999999999999";
  str_trim_left( t, 3 );
  printf( "%s\n", t );

  for( long z; z < 300000000; z++ )
  {
  //str_copy( t+10, t,    0, 15 ); // check for overlapping borders, begin of str
  //str_copy( t+10, t+20, 0, 15 ); // check for overlapping borders, end   of str
  //memmove( T, t, 222 );
  //memcpy( T, t, 222 );
  //str_copy( T, t, 0, 222 ); // check for overlapping borders, begin of str
  }
  /**/

  char t[92] = "this is simple test";
  char r[92] = "1111111111111111111";
  str_word( t, " ", r );
  ASSERT( strcmp( t, "is simple test" ) == 0 );
  ASSERT( strcmp( r, "this" ) == 0 );

  strcpy( t, "   opa" );
  str_cut_left( t, " " );
  ASSERT( strcmp( t, "opa" ) == 0 );

  strcpy( t, "opa   " );
  str_cut_right( t, " " );
  ASSERT( strcmp( t, "opa" ) == 0 );

  strcpy( t, "this is good" );
  str_ins( t, 8, "not " );
  ASSERT( strcmp( t, "this is not good" ) == 0 );

  str_del( t, 8, 4 );
  ASSERT( strcmp( t, "this is good" ) == 0 );

  strcpy( t, "more" );
  str_mul( t, 3 );
  ASSERT( strcmp( t, "moremoremore" ) == 0 );

  str_copy( t+10, t,    0, 15 ); // check for overlapping borders, begin of str
  str_copy( t+10, t+20, 0, 15 ); // check for overlapping borders, end   of str

  strcpy( t, "despicable me" );
  str_word( t, " ", r );
  ASSERT( strcmp( r, "despicable" ) == 0 );
  str_word( t, " ", r );
  ASSERT( strcmp( r, "me" ) == 0 );
  ASSERT( t[0] == 0 );


  /**/
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();
  //*/
  return 0;
}
