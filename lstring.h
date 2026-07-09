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

  if(start < source.length) {
    start += 1;
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

  size_t end = source.length - 1;
  for(size_t i = end; i > 0 && isspace(source.data[i]); i--) {
    end = i - source.length;
    assert(end > 0);
  }

  if(end > 0) {
    end -= 1;
  }

  LString result = {
    .length = source.length - end,
    .data = source.data,
  };

  return result;
}

LString lstring_trim(LString source) {
  return lstring_trim_left(lstring_trim_right(source));
}

LString lstring_split_by_delimiter(LString *source, const char delimiter) {

}
