#include <assert.h>
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
