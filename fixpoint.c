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

//TODO: add a helper for checkstyle
result_t
fixpoint_add( fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right ) {
  //if opposite signs, negates the negative one and calls sub
  if (left->negative ^ right->negative) {
    if(left->negative) {
      fixpoint_t newLeft = *left;
      fixpoint_negate(&newLeft);
      result_t to_return = fixpoint_sub(result, &newLeft, right);
      //negates if left was negative because should be neg after
      fixpoint_negate(result);
      return to_return;
    }
    if(right->negative) {
      fixpoint_t newRight = *right;
      fixpoint_negate(&newRight);
      return fixpoint_sub(result, left, &newRight);
    }
  }

  //executes addition
  result->whole = left->whole + right->whole;
  result->frac = left->frac + right->frac;
  result->negative = left->negative;
  //if overflow in the fraction occurs, add one to the whole
  if (result->frac < left->frac || result->frac < right->frac) {
    result->whole += 1;
  }
  //if overflow occurs
  if (result->whole < left->whole || result->whole < right->whole) {
    return RESULT_OVERFLOW;
  }
  return RESULT_OK;
}

//TODO: add a helper for checkstyle
result_t
fixpoint_sub( fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right ) {
  //if opposite signs, negates the negative one and calls add
  if (left->negative ^ right->negative) {
    if(left->negative) {
      fixpoint_t newLeft = *left;
      fixpoint_negate(&newLeft);
      result_t res = fixpoint_add(result, &newLeft, right);
      fixpoint_negate(result);
      return res;
    }
    if(right->negative) {
      fixpoint_t newRight = *right;
      fixpoint_negate(&newRight);
      return fixpoint_add(result, left, &newRight);
    }
  }

  //takes into account which whole is bigger and subtracts/sets negative
  if (left->whole > right->whole) {
    result->whole = left->whole - right->whole;
    result->negative = left->negative;
  }
  else if (right->whole > left->whole) {
    result->whole = right->whole - left->whole;
    result->negative = !left->negative;
  }
  else {
    result->whole = 0;
    result->negative = false;
  }

  //takes into account which fraction is bigger and subtracts
  if (left->frac > right->frac) {
    result->frac = left->frac - right->frac;
  }
  else if (right->frac > left->frac) {
    //if at 0, flips sign and goes into the negatives
    if(result->whole == 0) {
      result->negative = !result->negative;
      result->frac = right->frac - left->frac;
    }
    //if not subtracts 1 from whole
    else {
      result->whole -= 1;
      result->frac = 0xFFFFFFFF - right->frac + left->frac;
    }
  }
  else {
    result->frac = 0;
  }
  return RESULT_OK;
}

result_t
fixpoint_mul( fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right ) {
  //gets both parts of both fixpoints
  uint32_t a0 = left->frac;     
  uint32_t a1 = left->whole;      
  uint32_t b0 = right->frac;      
  uint32_t b1 = right->whole;       
  
  //multiplies both parts first number by both parts second and stores in 64 bit
  uint64_t p0 = (uint64_t)a0 * b0;  
  uint64_t p1 = (uint64_t)a0 * b1;  
  uint64_t p2 = (uint64_t)a1 * b0;  
  uint64_t p3 = (uint64_t)a1 * b1;  
  
  //handles adding together the parts and truncating the bits
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
  //if both are negative returns opposite
  if(left->negative) return toReturn *-1;
  //if both positive, return regular
  return toReturn;
}

void
fixpoint_format_hex( fixpoint_str_t *s, const fixpoint_t *val ) {
  int cx = 0;
  //adds - for negative
  if (val->negative) {
    s->str[0] = '-';
    cx = 1;
  }

  //adds hexstring and '.' for whole
  cx += snprintf(s->str + cx, FIXPOINT_STR_MAX_SIZE - cx, "%x.", val->whole);

  //if just 0, adds 0 to frac, if not, adds hexstring then removes all trailing 0s
  if (val->frac == 0) {
    cx += snprintf(s->str + cx, FIXPOINT_STR_MAX_SIZE - cx, "%d", 0);
  } else {
    cx += snprintf(s->str + cx, FIXPOINT_STR_MAX_SIZE - cx, "%08x", val->frac);
    while (s->str[cx - 1] == '0') {
      s->str[cx - 1] = '\0';
      cx--;
    }
  }
}

//TODO: add helper for checkstyle
bool
fixpoint_parse_hex( fixpoint_t *val, const fixpoint_str_t *s ) {
  //buffer for parsing making sure everything is right
  int cx = 0;
  int digitsInFrac = 0;
  //allow adding trailing 0s by modifying base string
  char newStr [FIXPOINT_STR_MAX_SIZE];
  strcpy(newStr, s->str);

  //determing if negative
  if (newStr[0] == '-') {
    val->negative = true;
    cx++;
  } else {
    val->negative = false;
  }

  //determine validity of string, and find how many fraction digits, fail if not valid
  //whole part of string
  while (newStr[cx] != '.') {
    char c = newStr[cx];
    if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
      return 0;
    }
    cx++;
  }
  cx++;
  if (((cx > 10 || cx < 3) && val->negative) || ((cx > 9 || cx < 2) && !val->negative)) {
    return 0;
  }

  //fraction part of string
  while (newStr[cx] != '\0') {
    digitsInFrac++;
    char c = newStr[cx];
    if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
      return 0;
    }
    cx++;
  }
  if (digitsInFrac > 8 || digitsInFrac < 1) {
    return 0;
  }

  //add 0s to end of string
  int len = strlen(newStr);
  while (digitsInFrac < 8) {
      newStr[len++] = '0';
      digitsInFrac++;
  }
  newStr[len] = '\0';

  //put values in correct spots
  if(!val->negative) sscanf (newStr, "%x%*c%x", &val->whole, &val->frac);
  if(val->negative) sscanf (newStr, "%*c%x%*c%x", &val->whole, &val->frac);

  //success
  return 1;
}
