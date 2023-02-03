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

/***************************************************************************
**
** GLOBALS
**
****************************************************************************/

#define VARRAY_DEFAULT_BLOCK_SIZE   1024
#define VSTRING_DEFAULT_BLOCK_SIZE   256

/* forward */
class VS_STRING_CLASS;
class VS_STRING_CLASS_R;
class VS_ARRAY_CLASS;
class VS_TRIE_CLASS;

/* using casual names... */
#define VHash   VS_TRIE_CLASS

/****************************************************************************
**
** aux functions
**
****************************************************************************/

  ssize_t str_len( const VS_CHAR *s );

/****************************************************************************
**
** VSTRING BOX
**
****************************************************************************/

class VS_STRING_BOX: public VRef
{
public:

  int   sl;   // string buffer length
  int   size; // internal buffer size
  VS_CHAR* s;    // internal buffer

  int   block_size; // current block size
  int   compact;

  VS_STRING_BOX() { s = NULL; sl = size = compact = 0; block_size = VSTRING_DEFAULT_BLOCK_SIZE; resize_buf( 0 ); };
  virtual ~VS_STRING_BOX();

  VS_STRING_BOX* clone();

  void resize_buf( int new_size );
  void undef() { resize_buf( 0 ); sl = 0; };
  void set_block_size( int new_block_size );
};


/****************************************************************************
**
** VSTRING
**
****************************************************************************/

VS_STRING_CLASS& str_copy ( VS_STRING_CLASS& target, const VS_CHAR* source, int pos = 0, int len = -1 ); // returns `len' VS_CHARs from `pos'
VS_STRING_CLASS& str_pad  ( VS_STRING_CLASS& target, int len, VS_CHAR ch = VS_CHAR_L(' ') );
VS_STRING_CLASS& str_comma( VS_STRING_CLASS& target, VS_CHAR delim = VS_CHAR_L('\'') );

class VS_STRING_CLASS
{
  VS_STRING_BOX* box;
  VS_CHAR retch; // used to return VS_CHAR& for off-range VS_CHAR index

  void detach();

public:

  VS_STRING_CLASS( const VS_STRING_CLASS& str )
    {
    box = str.box;
    box->ref();
    };

  VS_STRING_CLASS()                      {  box = new VS_STRING_BOX(); };
  VS_STRING_CLASS( const void*    nu  )  {  box = new VS_STRING_BOX(); nu = nu;  };
  VS_STRING_CLASS( const VS_CHAR* ps  )  {  box = new VS_STRING_BOX(); set( ps); };
  VS_STRING_CLASS( const int      n   )  {  box = new VS_STRING_BOX(); i(n);     };
  VS_STRING_CLASS( const long     n   )  {  box = new VS_STRING_BOX(); l(n);     };
  VS_STRING_CLASS( const double   n   )  {  box = new VS_STRING_BOX(); f(n);     };
  VS_STRING_CLASS( VS_STRING_CLASS_R  rs  );
  ~VS_STRING_CLASS() { box->unref(); };

  void compact( int a_compact ) // set this != 0 for compact (memory preserving) behaviour
        { box->compact = a_compact; }; //FIXME: detach() first?

  void set_block_size( int new_block_size ) 
        { if ( box ) box->set_block_size( new_block_size ); };

  void resize( int new_size )
        { detach(); box->resize_buf( new_size ); };

  void undef()
        { box->unref(); box = new VS_STRING_BOX(); };

  const VS_STRING_CLASS& operator  = ( const VS_STRING_CLASS& str )
        {
        box->unref();
        box = str.box;
        box->ref();
        return *this;
        };

  const VS_STRING_CLASS& operator  = ( const void*    nu   ) { nu = nu; undef(); return *this; };
  const VS_STRING_CLASS& operator  = ( const VS_CHAR* ps   ) { set(ps); return *this; };
  const VS_STRING_CLASS& operator  = ( const int      n    ) { i(n);    return *this; };
  const VS_STRING_CLASS& operator  = ( const long     n    ) { l(n);    return *this; };
  const VS_STRING_CLASS& operator  = ( const double   n    ) { f(n);    return *this; };

  const VS_STRING_CLASS& operator += ( const VS_STRING_CLASS& str )  { cat( str.box->s ); return *this; };
  const VS_STRING_CLASS& operator += ( const VS_CHAR*  ps    )       { cat( ps ); return *this; };
  const VS_STRING_CLASS& operator += ( const int    n     )          { VS_STRING_CLASS tmp = n; cat(tmp); return *this; };
  const VS_STRING_CLASS& operator += ( const long   n     )          { VS_STRING_CLASS tmp = n; cat(tmp); return *this; };
  const VS_STRING_CLASS& operator += ( const double n     )          { VS_STRING_CLASS tmp = n; cat(tmp); return *this; };

  const VS_STRING_CLASS& operator *= ( const int    n     )          { return str_mul( *this, n ); };

  friend VS_STRING_CLASS operator + ( const VS_STRING_CLASS& str1, const VS_STRING_CLASS& str2 ) { VS_STRING_CLASS res = str1; res += str2; return res; };
  friend VS_STRING_CLASS operator + ( const VS_STRING_CLASS& str1, const VS_CHAR* ps )           { VS_STRING_CLASS res = str1; res += ps;   return res; };
  friend VS_STRING_CLASS operator + ( const VS_CHAR* ps, const VS_STRING_CLASS& str2 )           { VS_STRING_CLASS res = ps;   res += str2; return res; };

  friend VS_STRING_CLASS operator + ( const VS_STRING_CLASS& str1, const int    n )              { VS_STRING_CLASS res = str1; res +=    n; return res; };
  friend VS_STRING_CLASS operator + ( const int    n, const VS_STRING_CLASS& str2 )              { VS_STRING_CLASS res =    n; res += str2; return res; };
  friend VS_STRING_CLASS operator + ( const VS_STRING_CLASS& str1, const long   n )              { VS_STRING_CLASS res = str1; res +=    n; return res; };
  friend VS_STRING_CLASS operator + ( const long   n, const VS_STRING_CLASS& str2 )              { VS_STRING_CLASS res =    n; res += str2; return res; };
  friend VS_STRING_CLASS operator + ( const VS_STRING_CLASS& str1, const double n )              { VS_STRING_CLASS res = str1; res +=    n; return res; };
  friend VS_STRING_CLASS operator + ( const double n, const VS_STRING_CLASS& str2 )              { VS_STRING_CLASS res =    n; res += str2; return res; };

  friend int operator == ( const VS_STRING_CLASS& s1, const VS_STRING_CLASS& s2 ) { return VS_FN_STRCMP( s1, s2 ) == 0; };
  friend int operator == ( const VS_CHAR*    s1, const VS_STRING_CLASS& s2 ) { return VS_FN_STRCMP( s1, s2 ) == 0; };
  friend int operator == ( const VS_STRING_CLASS& s1, const VS_CHAR*    s2 ) { return VS_FN_STRCMP( s1, s2 ) == 0; };

  friend int operator != ( const VS_STRING_CLASS& s1, const VS_STRING_CLASS& s2 ) { return VS_FN_STRCMP( s1, s2 ) != 0; };
  friend int operator != ( const VS_CHAR*    s1, const VS_STRING_CLASS& s2 ) { return VS_FN_STRCMP( s1, s2 ) != 0; };
  friend int operator != ( const VS_STRING_CLASS& s1, const VS_CHAR*    s2 ) { return VS_FN_STRCMP( s1, s2 ) != 0; };

  friend int operator >  ( const VS_STRING_CLASS& s1, const VS_STRING_CLASS& s2 ) { return VS_FN_STRCMP( s1, s2 ) >  0; };
  friend int operator >  ( const VS_CHAR*    s1, const VS_STRING_CLASS& s2 ) { return VS_FN_STRCMP( s1, s2 ) >  0; };
  friend int operator >  ( const VS_STRING_CLASS& s1, const VS_CHAR*    s2 ) { return VS_FN_STRCMP( s1, s2 ) >  0; };

  friend int operator >= ( const VS_STRING_CLASS& s1, const VS_STRING_CLASS& s2 ) { return VS_FN_STRCMP( s1, s2 ) >= 0; };
  friend int operator >= ( const VS_CHAR*    s1, const VS_STRING_CLASS& s2 ) { return VS_FN_STRCMP( s1, s2 ) >= 0; };
  friend int operator >= ( const VS_STRING_CLASS& s1, const VS_CHAR*    s2 ) { return VS_FN_STRCMP( s1, s2 ) >= 0; };

  friend int operator <  ( const VS_STRING_CLASS& s1, const VS_STRING_CLASS& s2 ) { return VS_FN_STRCMP( s1, s2 ) <  0; };
  friend int operator <  ( const VS_CHAR*    s1, const VS_STRING_CLASS& s2 ) { return VS_FN_STRCMP( s1, s2 ) <  0; };
  friend int operator <  ( const VS_STRING_CLASS& s1, const VS_CHAR*    s2 ) { return VS_FN_STRCMP( s1, s2 ) <  0; };

  friend int operator <= ( const VS_STRING_CLASS& s1, const VS_STRING_CLASS& s2 ) { return VS_FN_STRCMP( s1, s2 ) <= 0; };
  friend int operator <= ( const VS_CHAR*    s1, const VS_STRING_CLASS& s2 ) { return VS_FN_STRCMP( s1, s2 ) <= 0; };
  friend int operator <= ( const VS_STRING_CLASS& s1, const VS_CHAR*    s2 ) { return VS_FN_STRCMP( s1, s2 ) <= 0; };

  operator const VS_CHAR* ( ) const { return (const VS_CHAR*)box->s; }
  const VS_CHAR* data() const       { return (const VS_CHAR*)box->s; }

  VS_CHAR& operator [] ( int n )
      {
      if ( n < 0 ) n = box->sl + n;
      if ( n < 0 || n >= box->sl )
        {
        retch = 0;
        return retch;
        }
      detach();
      return box->s[n];
      }

  void fixlen()
       { box->sl = str_len(box->s);
         ASSERT( box->sl < box->size ); }
  void fix()
       { box->sl = str_len(box->s);
         box->resize_buf(box->sl);
         ASSERT( box->sl < box->size ); }
  void fixbuf()
       { box->resize_buf(box->sl);
         ASSERT( box->sl < box->size ); }

  void   i( const int n );
  void   l( const long n );
  void   f( const double d );
  void   fi( const double d ); // sets double as int (w/o frac)

  int    i()  { return VS_FN_STRTOL( box->s ); }
  long   l()  { return VS_FN_STRTOL( box->s ); }
  double f()  { return VS_FN_STRTOD( box->s ); }
  double fi() { return VS_FN_STRTOD( box->s ); }

  void   set(  const VS_CHAR* ps );
  void   cat(  const VS_CHAR* ps );
  void   setn( const VS_CHAR* ps, int len );
  void   catn( const VS_CHAR* ps, int len );

  /* for debugging only */
  int check() 
      { 
      int len = str_len(box->s); 
      return ((len == box->sl)&&(len<box->size)); 
      }

  /****************************************************************************
  ** VS_STRING_CLASS Friend Functions (for class VS_STRING_CLASS)
  ****************************************************************************/

  inline friend ssize_t str_len( VS_STRING_CLASS& target ) { return target.box->sl; };
  inline friend VS_STRING_CLASS& str_set( VS_STRING_CLASS& target, const VS_CHAR* ps ) { target.set( ps ); return target; };

  friend VS_STRING_CLASS& str_mul    ( VS_STRING_CLASS& target, int n                  ); // multiplies the VS_STRING_CLASS n times, i.e. "1"*5 = "11111"
  friend VS_STRING_CLASS& str_del    ( VS_STRING_CLASS& target, int pos, int len       ); // deletes `len' VS_CHARs starting from `pos'
  friend VS_STRING_CLASS& str_ins    ( VS_STRING_CLASS& target, int pos, const VS_CHAR* s ); // inserts `s' in position `pos'
  friend VS_STRING_CLASS& str_ins_ch ( VS_STRING_CLASS& target, int pos, VS_CHAR ch       ); // inserts `ch' in position `pos'
  friend VS_STRING_CLASS& str_replace( VS_STRING_CLASS& target, const VS_CHAR* out, const VS_CHAR* in ); // replace `out' w. `in'

  friend VS_STRING_CLASS& str_copy  ( VS_STRING_CLASS& target, const VS_CHAR* source, int pos, int len ); // returns `len' VS_CHARs from `pos'
  friend VS_STRING_CLASS& str_left  ( VS_STRING_CLASS& target, const VS_CHAR* source, int len ); // returns `len' VS_CHARs from the left
  friend VS_STRING_CLASS& str_right ( VS_STRING_CLASS& target, const VS_CHAR* source, int len ); // returns `len' VS_CHARs from the right
  friend VS_STRING_CLASS& str_sleft ( VS_STRING_CLASS& target, int len                     ); // self-left -- just as 'str_left()' but works on `target'
  friend VS_STRING_CLASS& str_sright( VS_STRING_CLASS& target, int len                     ); // self-right -- just as 'str_right()' but works on `target'

  friend VS_STRING_CLASS& str_trim_left ( VS_STRING_CLASS& target, int len ); // trims `len' VS_CHARs from the beginning (left)
  friend VS_STRING_CLASS& str_trim_right( VS_STRING_CLASS& target, int len ); // trim `len' VS_CHARs from the end (right)

  friend VS_STRING_CLASS& str_cut_left ( VS_STRING_CLASS& target, const VS_CHAR* charlist ); // remove all VS_CHARs `charlist' from the beginning (i.e. from the left)
  friend VS_STRING_CLASS& str_cut_right( VS_STRING_CLASS& target, const VS_CHAR* charlist ); // remove all VS_CHARs `charlist' from the end (i.e. from the right)
  friend VS_STRING_CLASS& str_cut      ( VS_STRING_CLASS& target, const VS_CHAR* charlist ); // does `str_cut_right(charlist);str_cut_left(charlist);'
  friend VS_STRING_CLASS& str_cut_spc  ( VS_STRING_CLASS& target                       ); // does `str_cut(" ");'

  friend VS_STRING_CLASS& str_pad  ( VS_STRING_CLASS& target, int len, VS_CHAR ch );
  friend VS_STRING_CLASS& str_comma( VS_STRING_CLASS& target, VS_CHAR delim );

  // next 3 functions are safe! so if you get/set out of the VS_STRING_CLASS range!
  friend void str_set_ch( VS_STRING_CLASS& target, int pos, const VS_CHAR ch ); // sets `ch' VS_CHAR at position `pos'
  friend VS_CHAR str_get_ch( VS_STRING_CLASS& target, int pos                ); // return VS_CHAR at position `pos', -1 for the last VS_CHAR etc...
  friend void str_add_ch( VS_STRING_CLASS& target, const VS_CHAR ch          ); // adds `ch' at the end
  friend void str_add_ch_range( VS_STRING_CLASS &target, const VS_CHAR fr, const VS_CHAR to ); // adds all from `fr' to 'to' at the end

  friend VS_CHAR*  str_word( VS_STRING_CLASS& target, const VS_CHAR* delimiters, VS_CHAR* result );
  friend VS_CHAR*  str_rword( VS_STRING_CLASS& target, const VS_CHAR* delimiters, VS_CHAR* result );
  // check VS_ARRAY_CLASS::split() instead of word() funtions...

  //FIXME: TODO: str_sprintf() should return VS_STRING_CLASS!
  // this `sprintf'-like function works as follows:
  // 1. set `this.VS_STRING_CLASS' length to `init_size'
  // 2. call `sprintf' with `format' and `...'
  // NOTE: You have to supply enough `init_size'! sorry...
  friend int sprintf( int init_size, VS_STRING_CLASS& target, const VS_CHAR *format, ... );
  // this is equal to `printf( 1024, format, ... )', i.e. `init_size=1024'
  friend int sprintf( VS_STRING_CLASS& target, const VS_CHAR *format, ... );


  friend VS_STRING_CLASS& str_tr ( VS_STRING_CLASS& target, const VS_CHAR *from, const VS_CHAR *to );
  friend VS_STRING_CLASS& str_up ( VS_STRING_CLASS& target );
  friend VS_STRING_CLASS& str_low( VS_STRING_CLASS& target );
  friend VS_STRING_CLASS& str_flip_case( VS_STRING_CLASS& target );

  friend VS_STRING_CLASS& str_reverse( VS_STRING_CLASS& target                       ); // reverse the VS_STRING_CLASS: `abcde' becomes `edcba'
  friend VS_STRING_CLASS& str_squeeze( VS_STRING_CLASS& target, const VS_CHAR* sq_VS_CHARs ); // squeeze encountered repeating VS_CHARs to one only

  /* utilities */

  void print(); // print string data to stdout (console)

  /* conversions/reversed char type functions */
  
  VS_STRING_CLASS( const VS_CHAR_R* prs  )  {  box = new VS_STRING_BOX(); set( prs );  };

  const VS_STRING_CLASS& operator  = ( const VS_STRING_CLASS_R& rs  );
  const VS_STRING_CLASS& operator  = ( const VS_CHAR_R* prs   );

  void   set(  const VS_CHAR_R* prs );

  #ifdef _VSTRING_WIDE_
  int   set_failsafe( const char* mbs ); // convert from multi-byte string to wide string, returns error chars count
  #endif

}; /* end of VS_STRING_CLASS class */

/****************************************************************************
**
** VS_STRING_CLASS Functions (for class VS_STRING_CLASS)
**
****************************************************************************/

/****************************************************************************
**
** VS_STRING_CLASS Functions (for VS_CHAR*)
**
****************************************************************************/

  VS_CHAR* str_set( VS_CHAR* target, const VS_CHAR* ps );

  VS_CHAR* str_mul( VS_CHAR* target, int n ); // multiplies the VS_STRING_CLASS n times, i.e. "1"*5 = "11111"

  VS_CHAR* str_del    ( VS_CHAR* target, int pos, int len       ); // deletes `len' VS_CHARs starting from `pos'
  VS_CHAR* str_ins    ( VS_CHAR* target, int pos, const VS_CHAR* s ); // inserts `s' string in position `pos'
  VS_CHAR* str_ins_ch ( VS_CHAR* target, int pos, VS_CHAR ch       ); // inserts `ch' VS_CHAR in position `pos'
  VS_CHAR* str_replace( VS_CHAR* target, const VS_CHAR* out, const VS_CHAR* in ); // replace `out' w. `in'

  inline int __str_copy_calc_offsets( const VS_CHAR* source, int& pos, int& len );
  VS_CHAR* str_copy  ( VS_CHAR* target, const VS_CHAR* source, int pos = 0, int len = -1 ); // returns `len' VS_CHARs from `pos'
  VS_CHAR* str_left  ( VS_CHAR* target, const VS_CHAR* source, int len ); // returns `len' VS_CHARs from the left
  VS_CHAR* str_right ( VS_CHAR* target, const VS_CHAR* source, int len ); // returns `len' VS_CHARs from the right
  VS_CHAR* str_sleft ( VS_CHAR* target, int len                     ); // "self-left"  i.e. just as 'left'  but works on `target'
  VS_CHAR* str_sright( VS_CHAR* target, int len                     ); // "self-right" i.e. just as 'right' but works on `target'

  VS_CHAR* str_trim_left ( VS_CHAR* target, int len ); // trims `len' VS_CHARs from the beginning (left)
  VS_CHAR* str_trim_right( VS_CHAR* target, int len ); // trim `len' VS_CHARs from the end (right)

  // next 2 functions are safe. so if you get/set out of the VS_CHAR* length range.
  // note: that `VS_CHAR*' funcs are slower because of initial strlen() check
  void str_set_ch( VS_CHAR* target, int pos, const VS_CHAR ch ); // sets `ch' VS_CHAR at position `pos'
  VS_CHAR str_get_ch( VS_CHAR* target, int pos                ); // return VS_CHAR at position `pos', -1 for the last VS_CHAR etc...
  
  void str_add_ch( VS_CHAR* target, const VS_CHAR ch          ); // adds `ch' at the end
  void str_add_ch_range( VS_CHAR* target, const VS_CHAR fr, const VS_CHAR to ); // adds all from `fr' to 'to' at the end

  // return first `word' (will be stored to `resolt'), 
  // i.e. from pos 0 to first found delimiter VS_CHAR
  // after that deletes this `word' from the `target' string. 
  // returns NULL when no words left
  VS_CHAR* str_word ( VS_CHAR* target, const VS_CHAR* delimiters, VS_CHAR* result );
  // ...same but `last' word reverse/rear
  VS_CHAR* str_rword( VS_CHAR* target, const VS_CHAR* delimiters, VS_CHAR* result );

  VS_CHAR* str_cut_left ( VS_CHAR* target, const VS_CHAR* charlist ); // remove all VS_CHARs `charlist' from the beginning (i.e. from the left)
  VS_CHAR* str_cut_right( VS_CHAR* target, const VS_CHAR* charlist ); // remove all VS_CHARs `charlist' from the end (i.e. from the right)
  VS_CHAR* str_cut      ( VS_CHAR* target, const VS_CHAR* charlist ); // does `CutR(charlist);CutL(charlist);'
  VS_CHAR* str_cut_spc  ( VS_CHAR* target                       ); // does `str_cut(" ");'

  // expand align in a field, filled w. `ch', if len > 0 then right, else left
  VS_CHAR* str_pad( VS_CHAR* target, int len, VS_CHAR ch = VS_CHAR_L(' ') );

  // insert `commas' for 1000's delimiter or use another delimiter
  // VS_STRING_CLASS supposed to be a integer or real w/o `e' format
  VS_CHAR* str_comma( VS_CHAR* target, VS_CHAR delim = VS_CHAR_L('\'') );

  // translate VS_CHARs from `from' to `to'
  // length of `from' MUST be equal to length of `to'
  VS_CHAR* str_tr( VS_CHAR* target, const VS_CHAR *from, const VS_CHAR *to );

  VS_CHAR* str_up ( VS_CHAR* target );
  VS_CHAR* str_low( VS_CHAR* target );
  VS_CHAR* str_flip_case( VS_CHAR* target );

  VS_CHAR* str_reverse( VS_CHAR* target ); // reverse the VS_STRING_CLASS: `abcde' becomes `edcba'

  VS_CHAR* str_squeeze( VS_CHAR* target, const VS_CHAR* sq_VS_CHARs ); // squeeze repeating VS_CHARs to one only

/****************************************************************************
**
** VS_STRING_CLASS Functions (for const VS_CHAR*)
**
****************************************************************************/

  VS_STRING_CLASS str_up ( const VS_CHAR* src );
  VS_STRING_CLASS str_low( const VS_CHAR* src );
  VS_STRING_CLASS str_flip_case( const VS_CHAR* src );

/****************************************************************************
**
** VS_STRING_CLASS Functions -- common (VS_STRING_CLASS class will pass transparently here)
**
****************************************************************************/

  int str_find ( const VS_CHAR* target, const VS_CHAR  c, int startpos = 0 ); // returns first zero-based position of VS_CHAR, or -1 if not found
  int str_rfind( const VS_CHAR* target, const VS_CHAR  c                   ); // returns last  zero-based position of VS_CHAR, or -1 if not found
  int str_find ( const VS_CHAR* target, const VS_CHAR* s, int startpos = 0 ); // returns first zero-based position of VS_STRING_CLASS, or -1 if not found
  int str_rfind( const VS_CHAR* target, const VS_CHAR* s                   ); // returns last  zero-based position of VS_STRING_CLASS, or -1 if not found

  int str_count(     const VS_CHAR* target, const VS_CHAR* charlist, int startpos = 0 ); // returns match count of all VS_CHARs from `charlist'
  int str_str_count( const VS_CHAR* target, const VS_CHAR* s,        int startpos = 0 ); // returns match count of `s' VS_STRING_CLASS into target

  int str_is_int   ( const VS_CHAR* target ); // check if VS_STRING_CLASS is correct int value
  int str_is_double( const VS_CHAR* target ); // check if VS_STRING_CLASS is correct double (w/o `e' format :( )

/***************************************************************************
**
** VARRAYBOX
**
****************************************************************************/

class VS_ARRAY_BOX : public VRef
{
public:

  VS_STRING_CLASS** _data;
  int       _size;
  int       _count;

  int   block_size; // current block size

  VS_ARRAY_BOX();
  ~VS_ARRAY_BOX();

  VS_ARRAY_BOX* clone();

  void resize( int new_size );
  void undef();
  void set_block_size( int new_block_size );
};

/***************************************************************************
**
** VARRAY
**
****************************************************************************/

class VS_ARRAY_CLASS
{
  VS_ARRAY_BOX *box;

  int       _fe; // foreach element index

  const VS_STRING_CLASS   _ret_empty; // return-empty-container
        VS_STRING_CLASS   _ret_str;   // return-container

  void detach();
  void q_sort( int lo, int hi, int (*q_strcmp)(const VS_CHAR *, const VS_CHAR *) );

  void new_pos( int n );
  void del_pos( int n );

  public:

  int compact;

  VS_ARRAY_CLASS();
  VS_ARRAY_CLASS( const VS_ARRAY_CLASS& arr );
  VS_ARRAY_CLASS( const VS_TRIE_CLASS& tr );
  ~VS_ARRAY_CLASS();

  int count() { return box->_count; } // return element count
  void set_block_size( int new_block_size ) { if ( box ) box->set_block_size( new_block_size ); };

  void ins( int n, const VS_CHAR* s ); // insert at position `n'
  void set( int n, const VS_CHAR* s ); // set/replace at position `n'
  void del( int n                   ); // delete at position `n'
  const VS_CHAR* get( int n ); // get at position `n'

  void undef() // clear the array (frees all elements)
      { box->unref(); box = new VS_ARRAY_BOX(); _ret_str = VS_CHAR_L(""); }

  int push( const VS_CHAR* s ); // add to the end of the array
  int push( VS_TRIE_CLASS *tr     ); // add to the end of the array
  int push( VS_ARRAY_CLASS *arr   ); // add to the end of the array
  const VS_CHAR* pop(); // get and remove the last element

  int unshift( const VS_CHAR* s ); // add to the beginning of the array
  int unshift( VS_TRIE_CLASS *tr     ); // add to the beginning of the array
  int unshift( VS_ARRAY_CLASS *arr   ); // add to the beginning of the array
  const VS_CHAR* shift(); // get and remove the first element

  void ins( int n, const VS_STRING_CLASS& vs ); // insert at position `n'
  void set( int n, const VS_STRING_CLASS& vs ); // set/replace at position `n'
  int push( const VS_STRING_CLASS& vs ); // add to the end of the array
  int unshift( const VS_STRING_CLASS& vs ); // add to the beginning of the array

  void sort( int rev = 0, int (*q_strcmp)(const VS_CHAR *, const VS_CHAR *) = NULL ); // sort (optional reverse order)
  void reverse(); // reverse elements order
  void shuffle(); // randomize element order with Fisher-Yates shuffle

  VS_STRING_CLASS& operator []( int n )
    {
      if ( n < 0 ) { _ret_str = VS_CHAR_L(""); return _ret_str; }
      if ( n >= box->_count )
        set( n, VS_CHAR_L("") );
      else
        detach(); // I don't know if user will change returned VS_STRING_CLASS?!
      return *box->_data[n];
    }

  // FIXME: TODO: verify behaviour!
  const VS_STRING_CLASS& operator []( int n ) const 
    {
      if ( n < 0 || n >= box->_count ) { return _ret_empty; }
      return *box->_data[n];
    }

  const VS_ARRAY_CLASS& operator = ( const VS_ARRAY_CLASS& arr )
    {
    box->unref();
    box = arr.box;
    box->ref();
    return *this;
    };

  const VS_ARRAY_CLASS& operator = ( const VS_TRIE_CLASS& tr )
    { undef(); push( (VS_TRIE_CLASS*)&tr ); return *this; };
  const VS_ARRAY_CLASS& operator = ( const VS_STRING_CLASS& str )
    { undef(); push( str ); return *this; };
  const VS_ARRAY_CLASS& operator += ( const VS_ARRAY_CLASS& arr )
    { push( (VS_ARRAY_CLASS*)&arr ); return *this; };
  const VS_ARRAY_CLASS& operator += ( const VS_TRIE_CLASS& tr )
    { push( (VS_TRIE_CLASS*)&tr ); return *this; };
  const VS_ARRAY_CLASS& operator += ( const VS_STRING_CLASS& str )
    { push( str ); return *this; };

  /* utilities */

  void print(); // print array data to stdout (console)

  int fload( const char* fname ); // return 0 for ok
  int fsave( const char* fname ); // return 0 for ok
  int fload( FILE* f ); // return 0 for ok
  int fsave( FILE* f ); // return 0 for ok

  /* implement `foreach'-like interface */
  void reset() // reset position to beginning
    { _fe = -1; };
  const VS_CHAR* next() // get next item or NULL for the end
    { _fe++; return _fe < box->_count ? box->_data[_fe]->data() : NULL; };
  const VS_CHAR* current() // get latest item got from next() -- current one
    { return _fe < box->_count ? box->_data[_fe]->data() : NULL; };
  int current_index() // current index
    { return _fe < box->_count ? _fe : -1; };

  int max_len(); // return the length of the longest string in the array
  int min_len(); // return the length of the shortest string in the array
};

/***************************************************************************
**
** VTRIENODE -- INTERNAL!
**
****************************************************************************/

class VS_TRIE_NODE
{
public:

  VS_TRIE_NODE();
  ~VS_TRIE_NODE();

  VS_TRIE_NODE *next;
  VS_TRIE_NODE *down;
  VS_CHAR      c;
  VS_STRING_CLASS   *data;

  void detach() { next = down = NULL; }
  void del_node( const VS_CHAR *key, int branch = 0 );
  VS_TRIE_NODE* find_node( const VS_CHAR* key, int create = 0 );

  VS_TRIE_NODE *clone();
  void print();
};

/***************************************************************************
**
** VTRIEBOX -- INTERNAL!
**
****************************************************************************/

class VS_TRIE_BOX : public VRef
{
public:

  VS_TRIE_NODE *root;

  VS_TRIE_BOX()  { root = new VS_TRIE_NODE(); }
  ~VS_TRIE_BOX() { ASSERT( root ); delete root; }

  VS_TRIE_BOX* clone();
  void undef() { ASSERT( root ); delete root; root = new VS_TRIE_NODE(); };
};

/***************************************************************************
**
** VTRIE
**
****************************************************************************/

class VS_TRIE_CLASS
{
  VS_TRIE_BOX *box;

  void detach();
  void trace_node( VS_TRIE_NODE *node, VS_ARRAY_CLASS* keys, VS_ARRAY_CLASS *vals );

  VS_STRING_CLASS temp_key;

  public:

  int compact;

  VS_TRIE_CLASS();
  VS_TRIE_CLASS( const VS_ARRAY_CLASS& arr );
  VS_TRIE_CLASS( const VS_TRIE_CLASS& tr );
  ~VS_TRIE_CLASS();

  void set( const VS_CHAR* key, const VS_CHAR* data ); // set data, same as []=
  void del( const VS_CHAR* key                   ); // remove data associated with `key'
  const VS_CHAR* get( const VS_CHAR* key            ); // get data by `key', same as []

  int exists( const VS_CHAR* key ); // return != 0 if key exist (i.e. is used)

  void undef() // delete all key+data pairs
    { box->unref(); box = new VS_TRIE_BOX(); }

  void keys_and_values( VS_ARRAY_CLASS *keys, VS_ARRAY_CLASS *values );

  VS_ARRAY_CLASS keys();   // returns VS_ARRAY_CLASS with keys currently used
  VS_ARRAY_CLASS values(); // returns VS_ARRAY_CLASS with keys' values

  void reverse(); // reverse keys <-> values

  void merge( VS_TRIE_CLASS  *tr  ); // adds keys+values (or modify existing keys)
  void merge( VS_ARRAY_CLASS *arr ); // adds keys+values (by VS_ARRAY_CLASS pair values)

  //void print_nodes() { print_node( root ); }; // for debug only
  void print(); // print trie data to stdout (console)

  int fload( const char* fname ); // return 0 for ok
  int fsave( const char* fname ); // return 0 for ok
  int fload( FILE* f ); // return 0 for ok
  int fsave( FILE* f ); // return 0 for ok

  VS_STRING_CLASS& operator []( const VS_CHAR* key )
    {
    detach(); // I don't know if user will change returned VS_STRING_CLASS?!
    VS_TRIE_NODE *node = box->root->find_node( key, 1 );
    ASSERT( node );
    if ( ! node->data )
      node->data = new VS_STRING_CLASS();
    return *(node->data);
    }

  const VS_TRIE_CLASS& operator = ( const VS_TRIE_CLASS& tr )
    {
    box->unref();
    box = tr.box;
    box->ref();
    return *this;
    };

  const VS_TRIE_CLASS& operator = ( const VS_ARRAY_CLASS& arr )
    { undef(); merge( (VS_ARRAY_CLASS*)&arr ); return *this; };
  const VS_TRIE_CLASS& operator += ( const VS_ARRAY_CLASS& arr )
    { merge( (VS_ARRAY_CLASS*)&arr ); return *this; };
  const VS_TRIE_CLASS& operator += ( const VS_TRIE_CLASS& tr )
    { merge( (VS_TRIE_CLASS*)&tr ); return *this; };
};

/****************************************************************************
**
** VS_STRING_CLASS Utility functions
**
****************************************************************************/

// str_chop() removes last VS_CHAR from a VS_STRING_CLASS (perl-like)
inline VS_CHAR*    str_chop( VS_CHAR* target    ) { return str_trim_right( target, 1 ); }
inline VS_STRING_CLASS& str_chop( VS_STRING_CLASS& target ) { return str_trim_right( target, 1 ); }

/* reduces VS_STRING_CLASS to the given width using dots:
   "this is long line" -> "this...ine"
   `s' can be NULL, then target will be reduced */
VS_STRING_CLASS str_dot_reduce( const VS_CHAR* s, int width );

/****************************************************************************
**
** VS_STRING_CLASS file names utilities -- functions and classes
** NOTE: does not use any external (outside this library) function calls!
**
****************************************************************************/

// adds trailing '/' if not exist
VS_CHAR* str_fix_path( VS_CHAR* s, int slashtype = VS_CHAR_L('/') );
const VS_STRING_CLASS& str_fix_path( VS_STRING_CLASS& s, int slashtype = VS_CHAR_L('/') );

VS_STRING_CLASS str_file_ext(      const VS_CHAR *ps ); // `ext'
VS_STRING_CLASS str_file_name(     const VS_CHAR *ps ); // `filename'
VS_STRING_CLASS str_file_name_ext( const VS_CHAR *ps ); // `filename.ext'
VS_STRING_CLASS str_file_path(     const VS_CHAR *ps ); // `/path/'

/* removes "/../"s, `path' can be NULL, then dest is fixed */
VS_STRING_CLASS str_reduce_path( const VS_CHAR* path );

/****************************************************************************
**
** VS_STRING_CLASS Conversions
**
****************************************************************************/

long hex2long( const VS_CHAR* s ); // hex to long

/***************************************************************************
**
** EOF
**
****************************************************************************/


