#include <stdlib.h>
#include <string.h>
#include "tctest.h"
#include "fixpoint.h"

// Test fixture: defines some fixpoint_t instances
// that can be used by test functions
typedef struct {
  fixpoint_t zero;
  fixpoint_t one;
  fixpoint_t one_half;
  fixpoint_t max;
  fixpoint_t neg_three_eighths;
  fixpoint_t min;
  fixpoint_t one_and_one_half;
  fixpoint_t one_hundred;
  fixpoint_t neg_eleven;
  fixpoint_t neg_one;
  fixpoint_t neg_two;
  fixpoint_t neg_max;
  fixpoint_t neg_min;
  fixpoint_t ten_and_quarter;
  fixpoint_t ten_and_half;
  fixpoint_t nine_and_half;
  fixpoint_t neg_nine_and_half;
  fixpoint_t nine_and_quarter;
  fixpoint_t neg_nine_and_quarter;
  fixpoint_t whole_max;
  fixpoint_t neg_whole_max;
  fixpoint_t neg_ten_and_half;
  fixpoint_t neg_ten_and_quarter;
} TestObjs;

// Functions to create and destroy the text fixture
// (each test function gets a "fresh" instance of the
// test fixture)
TestObjs *setup( void );
void cleanup( TestObjs *objs );

// The setup() function uses this macro to initialize
// the fixpoint_t objects in the test fixture, to avoid
// being dependent on the fixpoint_init() function.
#define TEST_FIXPOINT_INIT( val, w, f, n ) \
do { \
  (val)->frac = f; \
  (val)->whole = w; \
  (val)->negative = n; \
} while ( 0 )

// Macro to check two fixpoint_t instances for exact equality
#define TEST_EQUAL( val1, val2 ) \
do { \
  ASSERT( (val1)->whole == (val2)->whole ); \
  ASSERT( (val1)->frac == (val2)->frac ); \
  ASSERT( (val1)->negative == (val2)->negative ); \
} while ( 0 )

// Convenience macro to turn a string literal into a const pointer
// to a temporary instance of fixpoint_str_t
#define FIXPOINT_STR( strlit ) &( ( fixpoint_str_t ) { .str = (strlit) } )

// Prototypes for test functions
void test_init( TestObjs *objs );
void test_get_whole( TestObjs *objs );
void test_get_frac( TestObjs *objs );
void test_is_negative( TestObjs *objs );
void test_negate( TestObjs *objs );
void test_add( TestObjs *objs );
void test_sub( TestObjs *objs );
void test_mul( TestObjs *objs );
void test_compare( TestObjs *objs );
void test_format_hex( TestObjs *objs );
void test_parse_hex( TestObjs *objs );
// Additional comprehensive test functions
void test_add_2( TestObjs *objs );
void test_sub_2( TestObjs *objs );
void test_mul_2( TestObjs *objs );
void test_compare_2( TestObjs *objs );
void test_negate_2( TestObjs *objs );
void test_format_hex_2( TestObjs *objs );
void test_parse_hex_2( TestObjs *objs );
void test_negate_2( TestObjs *objs );
void test_is_negative_2( TestObjs *objs );

int main( int argc, char **argv ) {
  if ( argc > 1 )
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  TEST( test_init );
  TEST( test_get_whole );
  TEST( test_get_frac );
  TEST( test_is_negative );
  TEST( test_negate );
  TEST( test_add );
  TEST( test_sub );
  TEST( test_mul );
  TEST( test_compare );
  TEST( test_format_hex );
  TEST( test_parse_hex );
  // Additional comprehensive tests
  TEST( test_is_negative_2 );
  TEST( test_add_2 );
  TEST( test_sub_2 );
  TEST( test_mul_2 );
  TEST( test_compare_2 );
  TEST( test_negate_2 );
  TEST( test_format_hex_2 );
  TEST( test_parse_hex_2 );

  TEST_FINI();
}

TestObjs *setup( void ) {
  TestObjs *objs = (TestObjs *) malloc( sizeof( TestObjs ) );

  TEST_FIXPOINT_INIT( &objs->zero, 0, 0, false );
  TEST_FIXPOINT_INIT( &objs->one, 1, 0, false );
  TEST_FIXPOINT_INIT( &objs->one_half, 0, 0x80000000, false );
  TEST_FIXPOINT_INIT( &objs->max, 0xFFFFFFFF, 0xFFFFFFFF, false );
  TEST_FIXPOINT_INIT( &objs->neg_three_eighths, 0, 0x60000000, true );
  TEST_FIXPOINT_INIT( &objs->min, 0, 1, false );
  TEST_FIXPOINT_INIT( &objs->one_and_one_half, 1, 0x80000000, false );
  TEST_FIXPOINT_INIT( &objs->one_hundred, 100, 0, false );
  TEST_FIXPOINT_INIT( &objs->neg_eleven, 11, 0, true );

  // TODO: initialize additional fixpoint_t instances
  TEST_FIXPOINT_INIT( &objs->neg_one, 1, 0, true);
  TEST_FIXPOINT_INIT( &objs->neg_two, 2, 0, true);
  TEST_FIXPOINT_INIT( &objs->neg_max, 0xFFFFFFFF, 0xFFFFFFFF, true );
  TEST_FIXPOINT_INIT( &objs->neg_min, 0, 1, true);
  TEST_FIXPOINT_INIT( &objs->ten_and_quarter, 0x0000000A, 0x40000000, false );
  TEST_FIXPOINT_INIT( &objs->ten_and_half, 0x0000000A, 0x80000000, false );
  TEST_FIXPOINT_INIT( &objs->nine_and_half, 0x00000009, 0x80000000, false);
  TEST_FIXPOINT_INIT( &objs->nine_and_quarter, 0x00000009, 0x40000000, false);
  TEST_FIXPOINT_INIT( &objs->whole_max, 0xFFFFFFFF, 0x00000000, false);
  TEST_FIXPOINT_INIT( &objs->neg_whole_max, 0xFFFFFFFF, 0x00000000, true);
  TEST_FIXPOINT_INIT( &objs->neg_ten_and_half, 0x0000000A, 0x80000000, true );
  TEST_FIXPOINT_INIT( &objs->neg_ten_and_quarter, 0x0000000A, 0x40000000, true );
  TEST_FIXPOINT_INIT( &objs->neg_nine_and_half, 0x00000009, 0x80000000, true);
  TEST_FIXPOINT_INIT( &objs->neg_nine_and_quarter, 0x00000009, 0x40000000, true);


  return objs;
}

void cleanup( TestObjs *objs ) {
  free( objs );
}

void test_init( TestObjs *objs ) {
  // Note: don't modify the provided test functions.
  // Instead, add new test functions containing your new tests.

  fixpoint_t val;

  fixpoint_init( &val, 0, 0, false );
  ASSERT( val.whole == 0 );
  ASSERT( val.frac == 0 );
  ASSERT( val.negative == false );

  fixpoint_init( &val, 0xad2b55b1, 0xcf5f4470, true );
  ASSERT( val.whole == 0xad2b55b1 );
  ASSERT( val.frac == 0xcf5f4470 );
  ASSERT( val.negative == true );
}

void test_get_whole( TestObjs *objs ) {
  // Note: don't modify the provided test functions.
  // Instead, add new test functions containing your new tests.

  ASSERT( fixpoint_get_whole( &objs->zero ) == 0 );
  ASSERT( fixpoint_get_whole( &objs->one ) == 1 );
  ASSERT( fixpoint_get_whole( &objs->one_half ) == 0 );
  ASSERT( fixpoint_get_whole( &objs->max ) == 0xFFFFFFFF );
  ASSERT( fixpoint_get_whole( &objs->neg_three_eighths ) == 0 );
  ASSERT( fixpoint_get_whole( &objs->min ) == 0 );
  ASSERT( fixpoint_get_whole( &objs->one_and_one_half ) == 1 );
  ASSERT( fixpoint_get_whole( &objs->one_hundred ) == 100 );
  ASSERT( fixpoint_get_whole( &objs->neg_eleven ) == 11 );
}

void test_get_frac( TestObjs *objs ) {
  // Note: don't modify the provided test functions.
  // Instead, add new test functions containing your new tests.

  ASSERT( fixpoint_get_frac( &objs->zero ) == 0 );
  ASSERT( fixpoint_get_frac( &objs->one ) == 0 );
  ASSERT( fixpoint_get_frac( &objs->one_half ) == 0x80000000 );
  ASSERT( fixpoint_get_frac( &objs->max ) == 0xFFFFFFFF );
  ASSERT( fixpoint_get_frac( &objs->neg_three_eighths ) == 0x60000000 );
  ASSERT( fixpoint_get_frac( &objs->min ) == 1 );
  ASSERT( fixpoint_get_frac( &objs->one_and_one_half ) == 0x80000000 );
  ASSERT( fixpoint_get_frac( &objs->one_hundred ) == 0 );
  ASSERT( fixpoint_get_frac( &objs->neg_eleven ) == 0 );
}

void test_is_negative( TestObjs *objs ) {
  // Note: don't modify the provided test functions.
  // Instead, add new test functions containing your new tests.

  ASSERT( fixpoint_is_negative( &objs->zero ) == false );
  ASSERT( fixpoint_is_negative( &objs->one ) == false );
  ASSERT( fixpoint_is_negative( &objs->one_half ) == false );
  ASSERT( fixpoint_is_negative( &objs->max ) == false );
  ASSERT( fixpoint_is_negative( &objs->neg_three_eighths ) == true );
  ASSERT( fixpoint_is_negative( &objs->min ) == false );
  ASSERT( fixpoint_is_negative( &objs->one_and_one_half ) == false );
  ASSERT( fixpoint_is_negative( &objs->one_hundred ) == false );
  ASSERT( fixpoint_is_negative( &objs->neg_eleven ) == true );
}

void test_negate( TestObjs *objs ) {
  // Note: don't modify the provided test functions.
  // Instead, add new test functions containing your new tests.

  fixpoint_t result;

  // Negating 0 shouldn't cause it to become negative
  result = objs->zero;
  ASSERT( false == result.negative );
  fixpoint_negate( &result );
  ASSERT( result.whole == objs->zero.whole );
  ASSERT( result.frac == objs->zero.frac );
  ASSERT( false == result.negative );

  // Non-zero values should have their sign flip when negated,
  // but the magnitude should stay the same

  result = objs->one;
  fixpoint_negate( &result );
  ASSERT( result.whole == objs->one.whole );
  ASSERT( result.frac == objs->one.frac );
  ASSERT( true == result.negative );

  result = objs->max;
  fixpoint_negate( &result );
  ASSERT( result.whole == objs->max.whole );
  ASSERT( result.frac == objs->max.frac );
  ASSERT( true == result.negative );

  result = objs->neg_three_eighths;
  fixpoint_negate( &result );
  ASSERT( result.whole == objs->neg_three_eighths.whole );
  ASSERT( result.frac == objs->neg_three_eighths.frac );
  ASSERT( false == result.negative );
}

void test_add( TestObjs *objs ) {
  // Note: don't modify the provided test functions.
  // Instead, add new test functions containing your new tests.
  fixpoint_t result;

  ASSERT( fixpoint_add( &result, &objs->zero, &objs->zero ) == RESULT_OK );
  ASSERT( 0 == result.frac );
  ASSERT( 0 == result.whole );
  ASSERT( false == result.negative );

  ASSERT( fixpoint_add( &result, &objs->zero, &objs->one ) == RESULT_OK );
  ASSERT( 0 == result.frac );
  ASSERT( 1 == result.whole );
  ASSERT( false == result.negative );

  ASSERT( fixpoint_add( &result, &objs->max, &objs->one ) == RESULT_OVERFLOW );

  ASSERT( fixpoint_add( &result, &objs->max, &objs->min ) == RESULT_OVERFLOW );

  ASSERT( fixpoint_add( &result, &objs->zero, &objs->neg_three_eighths ) == RESULT_OK );
  ASSERT( 0x60000000 == result.frac );
  ASSERT( 0 == result.whole );
  ASSERT( true == result.negative );

  fixpoint_t neg_max = objs->max;
  neg_max.negative = true;

  fixpoint_t neg_min = objs->min;
  neg_min.negative = true;

  ASSERT( fixpoint_add( &result, &neg_max, &neg_min ) == RESULT_OVERFLOW );
}

void test_sub( TestObjs *objs ) {
  // Note: don't modify the provided test functions.
  // Instead, add new test functions containing your new tests.

  fixpoint_t result;

  ASSERT( fixpoint_sub( &result, &objs->one, &objs->zero ) == RESULT_OK );
  ASSERT( 1 == result.whole );
  ASSERT( 0 == result.frac );
  ASSERT( false == result.negative );

  ASSERT( fixpoint_sub( &result, &objs->zero, &objs->one ) == RESULT_OK );
  ASSERT( 1 == result.whole );
  ASSERT( 0 == result.frac );
  ASSERT( true == result.negative );

  fixpoint_t neg_min = objs->min;
  fixpoint_negate( &neg_min );
  ASSERT( fixpoint_sub( &result, &neg_min, &objs->max ) == RESULT_OVERFLOW );
}

void test_mul( TestObjs *objs ) {
  // Note: don't modify the provided test functions.
  // Instead, add new test functions containing your new tests.

  fixpoint_t result;

  ASSERT( fixpoint_mul( &result, &objs->one, &objs->zero ) == RESULT_OK );
  ASSERT( 0 == result.whole );
  ASSERT( 0 == result.frac );
  ASSERT( false == result.negative );

  ASSERT( fixpoint_mul( &result, &objs->one_and_one_half, &objs->one_hundred ) == RESULT_OK );
  ASSERT( 150 == result.whole );
  ASSERT( 0 == result.frac );
  ASSERT( false == result.negative );
}

void test_compare( TestObjs *objs ) {
  // Note: don't modify the provided test functions.
  // Instead, add new test functions containing your new tests.

  ASSERT( 0 == fixpoint_compare( &objs->zero, &objs->zero ) );
  ASSERT( 1 == fixpoint_compare( &objs->one, &objs->zero ) );
  ASSERT( -1 == fixpoint_compare( &objs->zero, &objs->one ) );
  ASSERT( -1 == fixpoint_compare( &objs->neg_three_eighths, &objs->one_half ) );
  ASSERT( 1 == fixpoint_compare( &objs->one_half, &objs->neg_three_eighths ) );
}

void test_format_hex( TestObjs *objs ) {
  // Note: don't modify the provided test functions.
  // Instead, add new test functions containing your new tests.

  fixpoint_str_t s;

  fixpoint_format_hex( &s, &objs->zero );
  ASSERT( 0 == strcmp( "0.0", s.str ) );

  fixpoint_format_hex( &s, &objs->one );
  ASSERT( 0 == strcmp( "1.0", s.str ) );

  fixpoint_format_hex( &s, &objs->one_half );
  ASSERT( 0 == strcmp( "0.8", s.str ) );

  fixpoint_format_hex( &s, &objs->max );
  ASSERT( 0 == strcmp( "ffffffff.ffffffff", s.str ) );

  fixpoint_format_hex( &s, &objs->neg_three_eighths );
  ASSERT( 0 == strcmp( "-0.6", s.str ) );

  fixpoint_format_hex( &s, &objs->min );
  ASSERT( 0 == strcmp( "0.00000001", s.str ) );

  fixpoint_format_hex( &s, &objs->one_and_one_half );
  ASSERT( 0 == strcmp( "1.8", s.str ) );

  fixpoint_format_hex( &s, &objs->one_hundred );
  ASSERT( 0 == strcmp( "64.0", s.str ) );

  fixpoint_format_hex( &s, &objs->neg_eleven );
  ASSERT( 0 == strcmp( "-b.0", s.str ) );
}

void test_parse_hex( TestObjs *objs ) {
  // Note: don't modify the provided test functions.
  // Instead, add new test functions containing your new tests.

  fixpoint_t val;

  ASSERT( true == fixpoint_parse_hex( &val, FIXPOINT_STR( "0.0" ) ) );
  TEST_EQUAL( &objs->zero, &val );

  ASSERT( true == fixpoint_parse_hex( &val, FIXPOINT_STR( "1.0" ) ) );
  TEST_EQUAL( &objs->one, &val );

  ASSERT( true == fixpoint_parse_hex( &val, FIXPOINT_STR( "0.8" ) ) );
  TEST_EQUAL( &objs->one_half, &val );

  ASSERT( true == fixpoint_parse_hex( &val, FIXPOINT_STR( "ffffffff.ffffffff" ) ) );
  TEST_EQUAL( &objs->max, &val );

  ASSERT( true == fixpoint_parse_hex( &val, FIXPOINT_STR( "-0.6" ) ) );
  TEST_EQUAL( &objs->neg_three_eighths, &val );

  ASSERT( true == fixpoint_parse_hex( &val, FIXPOINT_STR( "0.00000001" ) ) );
  TEST_EQUAL( &objs->min, &val );

  ASSERT( true == fixpoint_parse_hex( &val, FIXPOINT_STR( "1.8" ) ) );
  TEST_EQUAL( &objs->one_and_one_half, &val );

  ASSERT( true == fixpoint_parse_hex( &val, FIXPOINT_STR( "1.8" ) ) );
  TEST_EQUAL( &objs->one_and_one_half, &val );

  ASSERT( true == fixpoint_parse_hex( &val, FIXPOINT_STR( "64.0" ) ) );
  TEST_EQUAL( &objs->one_hundred, &val );

  ASSERT( true == fixpoint_parse_hex( &val, FIXPOINT_STR( "-b.0" ) ) );
  TEST_EQUAL( &objs->neg_eleven, &val );

  // Note: this test function doesn't have any tests for invalid
  // hex strings. You should add tests for invalid strings in one of
  // your own test functions.
}

void test_is_negative_2 (TestObjs *objs) {
  fixpoint_t result;

  //getting is_negative on negative 0 should be true
  result = objs->zero;
  result.negative = true;
  ASSERT( fixpoint_is_negative( &result ) == true );
}

void test_add_2( TestObjs *objs ) {
  fixpoint_t result;
  
  // Test adding two negative numbers
  ASSERT( fixpoint_add(&result, &objs->neg_one, &objs->neg_two) == RESULT_OK );
  ASSERT( result.whole == 3 );
  ASSERT( result.frac == 0 );
  ASSERT( result.negative == true );
  
  // Test adding positive and negative that results in positive
  fixpoint_add(&result, &objs->one_hundred, &objs->neg_eleven);
  ASSERT( result.whole == 89 );
  ASSERT( result.frac == 0 );
  ASSERT( result.negative == false );
  
  // Test adding positive and negative that results in negative
  fixpoint_add(&result, &objs->neg_eleven, &objs->one);
  ASSERT( result.whole == 10 );
  ASSERT( result.frac == 0 );
  ASSERT( result.negative == true );

  //Test negative overflow with fraction remaining
  fixpoint_add(&result, &objs->neg_max, &objs->neg_one);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0xFFFFFFFF );
  ASSERT( result.negative == true );

  //make sure it works flipped
  fixpoint_add(&result, &objs->neg_one, &objs->neg_max);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0xFFFFFFFF );
  ASSERT( result.negative == true );

  //test when left is positive and subtracting a negative smaller
  fixpoint_add(&result, &objs->max, &objs->neg_one);
  ASSERT( result.whole == 0xFFFFFFFE );
  ASSERT( result.frac == 0xFFFFFFFF );
  ASSERT( result.negative == false );

  //test when right is positive and subtracting a negative smaller
  fixpoint_add(&result, &objs->neg_one, &objs->max);
  ASSERT( result.frac == 0xFFFFFFFF );
  ASSERT( result.whole == 0xFFFFFFFE );
  ASSERT( result.negative == false );

  //test when left is negative and subtracting a positive smaller
  fixpoint_add(&result, &objs->neg_max, &objs->one);
  ASSERT( result.whole == 0xFFFFFFFE );
  ASSERT( result.frac == 0xFFFFFFFF );
  ASSERT( result.negative == true );

  //test when right is negative and subtracting a positive smaller
  fixpoint_add(&result, &objs->one, &objs->neg_max);
  ASSERT( result.whole == 0xFFFFFFFE );
  ASSERT( result.frac == 0xFFFFFFFF );
  ASSERT( result.negative == true );

  //test when adding (subtracting, different sign) same whole and different frac that sign flips when needed
  //and fraction part remaining is right sign (next 4 tests)
  fixpoint_add(&result, &objs->ten_and_quarter, &objs->neg_ten_and_half);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0x40000000 );
  ASSERT( result.negative == true );

  fixpoint_add(&result, &objs->ten_and_half, &objs->neg_ten_and_quarter);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0x40000000 );
  ASSERT( result.negative == false );

  fixpoint_add(&result, &objs->neg_ten_and_half, &objs->ten_and_quarter);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0x40000000 );
  ASSERT( result.negative == true );

  fixpoint_add(&result, &objs->neg_ten_and_quarter, &objs->ten_and_half);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0x40000000 );
  ASSERT( result.negative == false );

}

void test_sub_2( TestObjs *objs ) {
  fixpoint_t result;
  
  // Test subtracting same values (should result in zero)
  fixpoint_sub(&result, &objs->one, &objs->one);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0 );
  ASSERT( result.negative == false );
  
  //Test subtracting same negative values
  fixpoint_sub(&result, &objs->neg_one, &objs->neg_one);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0 );
  ASSERT( result.negative == false );

  //Test subtracting one from negative max getting overflow and neg 0
  ASSERT( RESULT_OVERFLOW == fixpoint_sub(&result, &objs->neg_whole_max, &objs->one));
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0 );
  ASSERT( result.negative == true );

  // Test subtracting larger from smaller (should be negative)
  fixpoint_sub(&result, &objs->one, &objs->one_hundred);
  ASSERT( result.whole == 99 );
  ASSERT( result.frac == 0 );
  ASSERT( result.negative == true );
  
  // Test subtracting negative from positive (addition)
  fixpoint_sub(&result, &objs->one, &objs->neg_eleven);
  ASSERT( result.whole == 12 );
  ASSERT( result.frac == 0 );
  ASSERT( result.negative == false );
  
  // Test subtracting positive from negative
  fixpoint_sub(&result, &objs->neg_eleven, &objs->one);
  ASSERT( result.whole == 12 );
  ASSERT( result.frac == 0 );
  ASSERT( result.negative == true );

  //test overflow when subtracting 1 from neg max
  fixpoint_sub(&result, &objs->neg_max, &objs->one);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0xFFFFFFFF );
  ASSERT( result.negative == true );

  //test overflow when subtracting -1 from max
  ASSERT( RESULT_OVERFLOW == fixpoint_sub(&result, &objs->max, &objs->neg_one));
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0xFFFFFFFF );
  ASSERT( result.negative == false );

  //test subtracting bigger whole smaller frac 
  fixpoint_sub(&result, &objs->ten_and_quarter, &objs->nine_and_half);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0xC0000000 );
  ASSERT( result.negative == false );

  //test subtracting bigger whole bigger frac both pos
  fixpoint_sub(&result, &objs->ten_and_half, &objs->nine_and_quarter);
  ASSERT( result.whole == 1 );
  ASSERT( result.frac == 0x40000000 );
  ASSERT( result.negative == false );

  //test subtracting bigger whole bigger frac both neg
  fixpoint_sub(&result, &objs->neg_ten_and_half, &objs->neg_nine_and_quarter);
  ASSERT( result.whole == 1 );
  ASSERT( result.frac == 0x40000000 );
  ASSERT( result.negative == true );


  //test subtracting smaller whole bigger frac
  fixpoint_sub(&result, &objs->nine_and_half, &objs->ten_and_quarter);
  ASSERT( result.whole == 1 );
  ASSERT( result.frac == 0x40000000 );
  ASSERT( result.negative == true );

  //test subtracting bigger whole smaller frac both neg
  fixpoint_sub(&result, &objs->neg_ten_and_quarter, &objs->neg_nine_and_half);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0xC0000000 );
  ASSERT( result.negative == true );

  //test subtracting smaller whole bigger frac both neg
  fixpoint_sub(&result, &objs->neg_nine_and_half, &objs->neg_ten_and_quarter);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0xC0000000 );
  ASSERT( result.negative == false );

  //test when both negative and right frac and whole bigger
  fixpoint_sub(&result, &objs->neg_one, &objs->neg_max);
  ASSERT( result.whole == 0xFFFFFFFE );
  ASSERT( result.frac == 0xFFFFFFFF );
  ASSERT( result.negative == false );

  //test when both negative and right frac and whole bigger
  fixpoint_sub(&result, &objs->neg_ten_and_quarter, &objs->neg_max);
  ASSERT( result.whole == 0xFFFFFFF5 );
  ASSERT( result.frac == 0xBFFFFFFF );
  ASSERT( result.negative == false );

  //test when both negative and left frac and whole bigger
  fixpoint_sub(&result, &objs->neg_max, &objs->neg_ten_and_quarter);
  ASSERT( result.whole == 0xFFFFFFF5 );
  ASSERT( result.frac == 0xBFFFFFFF );
  ASSERT( result.negative == true );

  //test subbing bigger whole bigger frac
  fixpoint_sub(&result, &objs->ten_and_half, &objs->nine_and_quarter);
  ASSERT( result.whole == 1 );
  ASSERT( result.frac == 0x40000000 );
  ASSERT( result.negative == false );

  //test when subtracting same whole and different frac that sign flips when needed
  //and fraction part remaining is right sign (next 4 tests)
  fixpoint_sub(&result, &objs->ten_and_quarter, &objs->ten_and_half);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0x40000000 );
  ASSERT( result.negative == true );

  fixpoint_sub(&result, &objs->ten_and_half, &objs->ten_and_quarter);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0x40000000 );
  ASSERT( result.negative == false );

  fixpoint_sub(&result, &objs->neg_ten_and_half, &objs->neg_ten_and_quarter);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0x40000000 );
  ASSERT( result.negative == true );

  fixpoint_sub(&result, &objs->neg_ten_and_quarter, &objs->neg_ten_and_half);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0x40000000 );
  ASSERT( result.negative == false );

}

void test_mul_2( TestObjs *objs ) {
  fixpoint_t result;
    
  // Test multiplication by one
  ASSERT( fixpoint_mul(&result, &objs->one_hundred, &objs->one) == RESULT_OK );
  ASSERT( result.whole == 100 );
  ASSERT( result.frac == 0 );
  ASSERT( result.negative == false );
  
  // Test fractional multiplication
  ASSERT( fixpoint_mul(&result, &objs->one_half, &objs->one_half) == RESULT_OK );
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0x40000000 );  // 0.5 * 0.5 = 0.25
  ASSERT( result.negative == false );
  
  // Test negative multiplication
  ASSERT( fixpoint_mul(&result, &objs->neg_eleven, &objs->one) == RESULT_OK );
  ASSERT( result.whole == 11 );
  ASSERT( result.frac == 0 );
  ASSERT( result.negative == true );
  
  // Test negative * negative = positive
  fixpoint_t neg_two;
  fixpoint_init(&neg_two, 2, 0, true);
  ASSERT( fixpoint_mul(&result, &objs->neg_eleven, &neg_two) == RESULT_OK );
  ASSERT( result.whole == 22 );
  ASSERT( result.frac == 0 );
  ASSERT( result.negative == false );

  fixpoint_t highest_bit;
  fixpoint_init(&highest_bit, 0x10000000, 0, false);

  fixpoint_t neg_highest_bit;
  fixpoint_init(&neg_highest_bit, 0x10000000, 0, true);

  fixpoint_t highest_and_lowest;
  fixpoint_init(&highest_and_lowest, 0x10000000, 1, false);

  fixpoint_t neg_highest_and_lowest;
  fixpoint_init(&neg_highest_and_lowest, 0x10000000, 1, true);

  //make sure result overflow proccs and still positive
  ASSERT( RESULT_OVERFLOW == fixpoint_mul(&result, &highest_bit, &highest_bit) );
  ASSERT( result.negative == false );
  ASSERT( RESULT_OVERFLOW == fixpoint_mul(&result, &objs->max, &objs->neg_two) );
  ASSERT( result.negative == true );

  //make sure result underflow proccs and still positive or negative
  ASSERT( RESULT_UNDERFLOW == fixpoint_mul(&result, &objs->min, &objs->min) );
  ASSERT( result.negative == false );
  ASSERT( RESULT_UNDERFLOW == fixpoint_mul(&result, &objs->neg_min, &objs->max) );
  ASSERT( result.negative == true );

  //make sure when both overflow its both and still positive or negative
  ASSERT( (RESULT_OVERFLOW|RESULT_UNDERFLOW) == fixpoint_mul(&result, &objs->max, &objs->max) );
  ASSERT( result.negative == false );
  ASSERT( (RESULT_OVERFLOW|RESULT_UNDERFLOW) == fixpoint_mul(&result, &neg_highest_and_lowest, &highest_and_lowest) );
  ASSERT( result.negative == true );


}

void test_compare_2( TestObjs *objs ) {
  // Test comparing equal values
  ASSERT( 0 == fixpoint_compare(&objs->zero, &objs->zero) );
  ASSERT( 0 == fixpoint_compare(&objs->one, &objs->one) );
  
  // Test comparing fractions
  ASSERT( -1 == fixpoint_compare(&objs->min, &objs->one_half) );
  ASSERT( 1 == fixpoint_compare(&objs->one_half, &objs->min) );
  
  // Test comparing negative values
  fixpoint_t neg_small;
  fixpoint_init(&neg_small, 0, 0x40000000, true);  // -0.25
  ASSERT( -1 == fixpoint_compare(&objs->neg_eleven, &neg_small) );
  ASSERT( 1 == fixpoint_compare(&neg_small, &objs->neg_eleven) );
  
  // Test comparing mixed signs with same magnitude
  fixpoint_t pos_eleven;
  fixpoint_init(&pos_eleven, 11, 0, false);
  ASSERT( -1 == fixpoint_compare(&objs->neg_eleven, &pos_eleven) );
  ASSERT( 1 == fixpoint_compare(&pos_eleven, &objs->neg_eleven) );
}

void test_negate_2( TestObjs *objs ) {
  fixpoint_t result;
  
  // Test negating negative zero (from overflow)
  fixpoint_init(&result, 0, 0, true);
  ASSERT( result.whole == 0 );
  ASSERT( result.frac == 0 );
  ASSERT( result.negative == true );
  
  
  // Test double negation returns to original
  result = objs->one_and_one_half;
  fixpoint_negate(&result);
  fixpoint_negate(&result);
  TEST_EQUAL(&objs->one_and_one_half, &result);
}

void test_format_hex_2( TestObjs *objs ) {
  fixpoint_str_t s;
  
  // Test formatting large values
  fixpoint_t large_val;
  fixpoint_init(&large_val, 0xABCDEF12, 0x34567890, false);
  fixpoint_format_hex(&s, &large_val);
  ASSERT( 0 == strcmp("abcdef12.3456789", s.str) );
  
  // Test formatting negative fractional
  fixpoint_t neg_frac;
  fixpoint_init(&neg_frac, 0, 0x12345678, true);
  fixpoint_format_hex(&s, &neg_frac);
  ASSERT( 0 == strcmp("-0.12345678", s.str) );
  
  // Test formatting with trailing zeros in fraction with negative
  fixpoint_t trailing_zeros;
  fixpoint_init(&trailing_zeros, 15, 0x12340000, true);
  fixpoint_format_hex(&s, &trailing_zeros);
  ASSERT( 0 == strcmp("-f.1234", s.str) );
}

void test_parse_hex_2( TestObjs *objs ) {
  // NOTE: These tests will be useful when parse_hex is implemented
  fixpoint_t val;
  
  // Test parsing invalid formats (should return false)
  ASSERT( false == fixpoint_parse_hex(&val, FIXPOINT_STR("")) );       //nothing
  ASSERT( false == fixpoint_parse_hex(&val, FIXPOINT_STR(".")) );      //only .
  ASSERT( false == fixpoint_parse_hex(&val, FIXPOINT_STR("1")) );      // No decimal
  ASSERT( false == fixpoint_parse_hex(&val, FIXPOINT_STR("1.")) );     // No frac digits
  ASSERT( false == fixpoint_parse_hex(&val, FIXPOINT_STR(".5")) );     // No whole digits
  ASSERT( false == fixpoint_parse_hex(&val, FIXPOINT_STR("1.g")) );    // Invalid frac
  ASSERT( false == fixpoint_parse_hex(&val, FIXPOINT_STR("z.5")) );    // Invalid whole
  ASSERT( false == fixpoint_parse_hex(&val, FIXPOINT_STR("1.123456789ABCDEF")) );   //invalid fraction
  ASSERT( false == fixpoint_parse_hex(&val, FIXPOINT_STR("1003bd34542.8")) );       //invalid whole


  // Test parsing uppercase hex
  ASSERT( true == fixpoint_parse_hex(&val, FIXPOINT_STR("ABC.DEF")) );
  ASSERT( val.whole == 0xABC );
  ASSERT( val.frac == 0xDEF00000 );
  ASSERT( val.negative == false );
  
  // Test parsing mixed case
  ASSERT( true == fixpoint_parse_hex(&val, FIXPOINT_STR("aBc.DeF")) );
  ASSERT( val.whole == 0xABC );
  ASSERT( val.frac == 0xDEF00000 );
  ASSERT( val.negative == false );
}