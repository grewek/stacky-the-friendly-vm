#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * Description: length based string
 */
typedef struct {
  size_t length; // the complete length of the string
  const char *data; // pointer that points to the start of the data
} LString;


/*
 * Description: Generate a LString from an exsisting c string
 * @string-data - a valid pointer to a zero terminated c string
 * Returns: The generated LString
 */
LString lstring_from_cstring(const char *string_data) {
  assert(string_data != NULL);
  LString result = {
    .length= strlen(string_data),
    .data = string_data,
  };

  return result;
}

/*
 * Description: Generate a cstring from a given lstring
 * @lstring - the lstring that should be converted into a cstring
 *
 */
char *cstring_from_lstring(LString lstring) {
  assert(lstring.length > 0);
  assert(lstring.data != NULL);

  char *result = malloc(lstring.length);

  if(result == NULL) {
    fprintf(stderr, "malloc error: %s", strerror(errno));
    exit(1);
  }

  memcpy(result, lstring.data, lstring.length);

  return result;
}

/*
 * Description: Trim all whitespace characters from the left side of the string.
 * @source: The source string that should be trimmed
 * Returns: The trimmed string
 * __NOTE__: This Function modifies the position of the data pointer, always make sure that you treat the
 *           LString more as a view and not as the owner of the actual data.
 */
LString lstring_trim_left(LString source) {
  assert(source.data != NULL);

  size_t start = 0;

  for(size_t i = 0; i < source.length && isspace(source.data[i]); i++) {
    start = i;
  }

  LString result = {
    .length = source.length - start,
    .data = source.data + start,
  };

  return result;
}
/*
 * Description: Trim all whitespace characters from the right side of the string.
 * @source: The source string that should be trimmed
 * Returns: The trimmed string
 * __NOTE__: This Function modifies the position of the data pointer, always make sure that you treat the
 *           LString more as a view and not as the owner of the actual data.
 */
LString lstring_trim_right(LString source) {
  assert(source.data != NULL);

  size_t end = source.length;
  for(size_t i = 0; i < source.length && isspace(source.data[source.length - i]); i--) {
    end = i;
  }

  LString result = source;
  if(end > 0) {
    result = (LString) {
      .length = end,
      .data = source.data,
    };
  }
 
  return result;
}

LString lstring_trim(LString source) {
  return lstring_trim_left(lstring_trim_right(source));
}

LString lstring_split_by_delimiter(LString *source, const char delimiter) {

  bool found_delimiter = false;
  size_t split_position = source->length;

  *source = lstring_trim(*source);

  for(size_t i = 0; i < source->length && !found_delimiter; i++) {
    if(source->data[i] == delimiter) {
      found_delimiter = true;
      split_position = i;
    }
  }

  LString result = {
    .length = split_position,
    .data = source->data,
  };

  if(split_position < source->length) {
    source->length -= split_position + 1;
    source->data += split_position + 1;
  } else {
    source->length -= split_position;
    source->data += split_position;
  }

  return result;
}

bool lstring_to_integer_value(LString repr, int64_t *value_out) {
  assert(repr.data != NULL);
  int64_t value = 0;

  bool is_valid_numeric_value = true;
  for(size_t i = 0; i < repr.length && is_valid_numeric_value; i++) {
    char digit = repr.data[i];
    value *= 10;
    if(isdigit(digit)) {
      value += digit - '0';
    } else {
      is_valid_numeric_value = false;
    }
  }

  if(is_valid_numeric_value) {
    *value_out = value;
  } else {
    *value_out = 0;
  }

  return is_valid_numeric_value;
}


/*
 * Description: Compare a LString to a static const char*
 * a: The first LString to compare
 * b: The static const char* to compare
 * Returns: true if the strings are equal, false if they don't
 */
bool lstring_compare_to_cstring(const LString a, const char *b) {
  LString lstring_b = lstring_from_cstring(b);
    assert(lstring_b.length > 0 && lstring_b.data != NULL);

  bool result = true;

  if(a.length != lstring_b.length) {
    result = false;
  }

  for(size_t i = 0; i < a.length && result; i++) {
    if(a.data[i] != lstring_b.data[i]) {
      result = false;
    }
  }

  return result;
}
