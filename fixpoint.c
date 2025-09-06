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
  //if opposite signs, negates the negative one and calls sub
  if (left->negative ^ right->negative) {
    if(left->negative) {
      fixpoint_t newLeft = *left;
      fixpoint_negate(&newLeft);
      return(fixpoint_sub(result, &newLeft, right));
    }
    if(right->negative) {
      fixpoint_t newRight = *right;
      fixpoint_negate(&newRight);
      return(fixpoint_sub(result, left, &newRight));
    }
  }

  //executes addition
  result->whole = left->whole + right->whole;
  result->frac = left->frac + right->frac;
  result->negative = left->negative;
  //if overflow in the fraction occurs, add one to the whole
  if (result->frac < left->frac || result->frac < right->frac)
      result->whole += 1;
  //if overflow occurs
  if (result->whole < left->whole || result->whole < right->whole)
    return RESULT_OVERFLOW;
  return RESULT_OK;
}

result_t
fixpoint_sub( fixpoint_t *result, const fixpoint_t *left, const fixpoint_t *right ) {
  //if opposite signs, negates the negative one and calls add
  if (left->negative ^ right->negative) {
    if(left->negative) {
      fixpoint_t newLeft = *left;
      fixpoint_negate(&newLeft);
      return(fixpoint_add(result, &newLeft, right));
    }
    if(right->negative) {
      fixpoint_t newRight = *right;
      fixpoint_negate(&newRight);
      return(fixpoint_add(result, &newRight, right));
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
  result->whole = left->whole * right->whole;
  result->frac = left->frac * right->frac;
  result->negative = left->negative || right->negative;
  return RESULT_OK;
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
  char whole_str[16];  // Buffer for whole part
  char frac_str[16];   // Buffer for fractional part
  
  // Convert whole part to hex (remove leading zeros, keep at least 1)
  if (val->whole == 0) {
    strcpy(whole_str, "0");
  } else {
    sprintf(whole_str, "%x", val->whole);  // lowercase hex, no leading zeros
  }
  
  // Convert fractional part to hex (remove trailing zeros, keep at least 1)
  if (val->frac == 0) {
    strcpy(frac_str, "0");
  } else {
    sprintf(frac_str, "%08x", val->frac);  // 8 digits with leading zeros
    
    // Remove trailing zeros from fractional part
    int len = strlen(frac_str);
    while (len > 1 && frac_str[len - 1] == '0') {
      frac_str[len - 1] = '\0';
      len--;
    }
  }
  
  // Combine the parts with optional minus sign
  if (val->negative) {
    sprintf(s->str, "-%s.%s", whole_str, frac_str);
  } else {
    sprintf(s->str, "%s.%s", whole_str, frac_str);
  }
}

bool
fixpoint_parse_hex( fixpoint_t *val, const fixpoint_str_t *s ) {
  // TODO: implement
  return false;
}
