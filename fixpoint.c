#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "fixpoint.h"

////////////////////////////////////////////////////////////////////////
// Helper functions
// Note that you can make these "visible" (not static)
// if you want to be able to write unit tests for them
////////////////////////////////////////////////////////////////////////

// TODO: add helper functions

////////////////////////////////////////////////////////////////////////
// Public API functions
////////////////////////////////////////////////////////////////////////

void
fixpoint_init( fixpoint_t *val, uint32_t whole, uint32_t frac, bool negative ) {
  val->whole = whole;
  val->frac = frac;
  val->negative = negative;
}

uint32_t
fixpoint_get_whole( const fixpoint_t *val ) {
  return val->whole;
}

uint32_t
fixpoint_get_frac( const fixpoint_t *val ) {
  return val->frac;
}

bool
fixpoint_is_negative( const fixpoint_t *val ) {
  return val->negative;
}

void
fixpoint_negate( fixpoint_t *val ) {
  // Special case: zero should never be negative
  if (val->whole == 0 && val->frac == 0) {
    val->negative = false;
  } else {
    // For non-zero values, flip the sign
    val->negative = !val->negative;
  }
}

result_t
fixpoint_add( fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right ) {
  //if signs differ
  if (left->negative ^ right->negative) {
    //if left greater than right
    if (left->whole > right->whole) {
      result->whole = left->whole - right->whole;
      //handle fraction subtraction when right fraction greater than left
      if (right->frac > left->frac) {
        result->whole -=1;
        result->frac = right->frac - left->frac;
      } else {
        result->frac = left->frac - right->frac;
      }
      result->negative = left->negative;
      //if right greater than left
    } else if (right->whole > left->whole){
      result->whole = right->whole - left->whole;
      if (left->frac > right->frac) {
        result->whole -=1;
        result->frac = left->frac - right->frac;
      } else {
        result->frac = right->frac - left->frac;
      }
      result->negative = right->negative;
      //if wholes are same and left frac greater than right frac
    } else if (left->frac > right->frac) {
      result->whole = 0;
      result->frac = left->frac - right->frac;
      result->negative = left->negative;
      //same wholes right frac greater than left
    } else if (right->frac > left->frac) {
      result->whole = 0;
      result->frac = right->frac - left->frac;
      result->negative = right->negative;
    }
    //if signs are the same
  } else {
    result->whole = left->whole + right->whole;
    result->frac = left->frac + right->frac;
    result->negative = left->negative;
    //if overflow in the fraction occurs, add one to the whole
    if (result->frac < left->frac || result->frac < right->frac)
      result->whole += 1;
  }
  //if overflow occurs
  if (result->whole < left->whole || result->whole < right->whole)
    return RESULT_OVERFLOW;
  return RESULT_OK;
}

result_t
fixpoint_sub( fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right ) {
  //can just add a negative
  fixpoint_t neg_right = *right;
  fixpoint_negate(&neg_right);
  return fixpoint_add(result, left, &neg_right);
}

result_t
fixpoint_mul( fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right ) {
  uint32_t a0 = left->frac;         // frac part of left
  uint32_t a1 = left->whole;        // whole part of left  
  uint32_t b0 = right->frac;        // frac part of right
  uint32_t b1 = right->whole;       // whole part of right
  
  uint64_t p0 = (uint64_t)a0 * b0;  // frac * frac
  uint64_t p1 = (uint64_t)a0 * b1;  // frac * whole
  uint64_t p2 = (uint64_t)a1 * b0;  // whole * frac
  uint64_t p3 = (uint64_t)a1 * b1;  // whole * whole
  
  uint64_t middle_sum = p1 + p2 + (p0 >> 32);
  result->whole = (uint32_t)p3 + (uint32_t)(middle_sum >> 32);
  result->frac = (uint32_t)middle_sum;
  
  //sign handling
  result->negative = left->negative ^ right->negative;
  if (result->whole == 0 && result->frac == 0) {
    result->negative = false;
  }
  
  // Check overflow/underflow
  result_t ret = RESULT_OK;
  if (p3 >> 32) ret |= RESULT_OVERFLOW;
  if (p0 & 0xFFFFFFFF) ret |= RESULT_UNDERFLOW; 
  
  return ret;
}

int
fixpoint_compare( const fixpoint_t *left, const fixpoint_t *right ) {
  //if differing signs the negative is smallest
  if (left->negative ^ right->negative) {
    if (left->negative) return -1;
    if (right->negative) return 1;
  }
  int toReturn = 0;
  //sets return value for if both are positive
  if (left->whole < right->whole) {
    toReturn = -1;
  } else if (left->whole > right->whole) {
    toReturn = 1;
  } else if (left->frac < right->frac) {
    toReturn = -1;
  } else if (left->frac > right->frac) {
    toReturn = 1;
  }
  //if same returns 0
  if(!toReturn) return toReturn;
  //if both positive, return regular
  return toReturn;
    //if both are negative returns opposite
  if(left->negative) return toReturn *-1;
}

void
fixpoint_format_hex( fixpoint_str_t *s, const fixpoint_t *val ) {
  char whole_str[16];
  char frac_str[16]; 
  
  if (val->whole == 0) {
    strcpy(whole_str, "0");
  } else {
    sprintf(whole_str, "%x", val->whole);  // lowercase hex, no leading zeros
  }
  

  if (val->frac == 0) {
    strcpy(frac_str, "0");
  } else {
    sprintf(frac_str, "%08x", val->frac);

    int len = strlen(frac_str);
    while (len > 1 && frac_str[len - 1] == '0') {
      frac_str[len - 1] = '\0';
      len--;
    }
  }
  
  if (val->negative) {
    sprintf(s->str, "-%s.%s", whole_str, frac_str);
  } else {
    sprintf(s->str, "%s.%s", whole_str, frac_str);
  }
}

bool
fixpoint_parse_hex( fixpoint_t *val, const fixpoint_str_t *s ) {
  //idk
}
