/* HelloOS */

#include <misc/string.h>

// add string ot string
int strcat_i;
void strcat(char* s, const char* t){
    strcat_i = 0;

    while (s[strcat_i] != '\0')
        strcat_i++;
    while (*t != '\0')
        s[strcat_i++] = *t++;
    s[strcat_i] = '\0';
}

// get string len
int strlen(char* str){
    int i = 0;
    while(str[i] != '\0'){ i++; }

    return i;
}

// compare strings
int strcmp(const char * str1, const char * str2) {
    int diff;
    while (*str1 != 0 && *str2 != 0) {
        diff = *str1 - *str2;
        if (diff != 0) {
            return diff;
        }

        str1++;
        str2++;
    }

    return *str1 - *str2;
}

// reverse string
void reverse(char s[]){
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

// int to string
char* itoa(int n){
    char* s = (char*) malloc(sizeof(char) * 32);
    int i, sign;

    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
        s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

// memset
void* memset(void* ptr, int value, unsigned int num) {
   unsigned char* ptr_byte = (unsigned char*)ptr;

   for (unsigned int i = 0; i < num; ptr_byte[i] = (unsigned char)value, i++);
   return ptr;
}

char *strchr(const char *p, int ch)
{
	char c;

	c = ch;
	for (;; ++p) {
		if (*p == c)
			return ((char *)p);
		if (*p == '\0')
			return (NULL);
	}
	// NOTREACHED
}

char* toupper(char* string)
{
    for(char* p=string; *p != '\0'; p++)
    {
        if(*p >= 'a' && *p <= 'z') 
          *p -= 32;
    }
    return string;
}

int isspace(int c)
{
	return (c == '\t' || c == '\n' ||
	    c == '\v' || c == '\f' || c == '\r' || c == ' ' ? 1 : 0);
}

char *trim(char *s) {
    char *ptr;
    if (!s)
        return NULL;   // handle NULL string
    if (!*s)
        return s;      // handle empty string
    for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
    ptr[1] = '\0';
    return s;
}

void parse_args(const char *argsString, int *argc, char *argv[]) {
    *argc = 0;

    // Iterate through the string
    for (int i = 0; argsString[i] != '\0';) {
        // Skip leading spaces
        while (argsString[i] == ' ') {
            ++i;
        }

        // Check for the end of the string
        if (argsString[i] == '\0') {
            break;
        }

        // Find the end of the current argument
        int start = i;
        while (argsString[i] != ' ' && argsString[i] != '\0') {
            ++i;
        }
        int end = i;

        // Allocate memory for the argument and copy the substring
        argv[*argc] = malloc(end - start + 1);
        for (int j = start; j < end; ++j) {
            argv[*argc][j - start] = argsString[j];
        }
        argv[*argc][end - start] = '\0';
        ++(*argc);
    }
}

/* Align a value by rounding down to closest size.
   e.g. Using size of 4096, we get this behavior:
	{4095, 4096, 4097} = {0, 4096, 4096}.  */
#define ALIGN_DOWN(base, size)	((base) & -((__typeof__ (base)) (size)))

/* Same as ALIGN_DOWN(), but automatically casts when base is a pointer.  */
#define PTR_ALIGN_DOWN(base, size) \
  ((__typeof__ (base)) ALIGN_DOWN ((uintptr_t) (base), (size)))

# define __glibc_unlikely(cond)	(cond)
# define __glibc_likely(cond)	(cond)

char *strchrnul(const char *s, int c_in) {
    const unsigned char *char_ptr;
  const unsigned long int *longword_ptr;
  unsigned long int longword, magic_bits, charmask;
  unsigned char c;
  c = (unsigned char) c_in;
  /* Handle the first few characters by reading one character at a time.
     Do this until CHAR_PTR is aligned on a longword boundary.  */
  for (char_ptr = (const unsigned char *) s;
       ((unsigned long int) char_ptr & (sizeof (longword) - 1)) != 0;
       ++char_ptr)
    if (*char_ptr == c || *char_ptr == '\0')
      return (void *) char_ptr;
  /* All these elucidatory comments refer to 4-byte longwords,
     but the theory applies equally well to 8-byte longwords.  */
  longword_ptr = (unsigned long int *) char_ptr;
  /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
     the "holes."  Note that there is a hole just to the left of
     each byte, with an extra at the end:
     bits:  01111110 11111110 11111110 11111111
     bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD
     The 1-bits make sure that carries propagate to the next 0-bit.
     The 0-bits provide holes for carries to fall into.  */
  magic_bits = -1;
  magic_bits = magic_bits / 0xff * 0xfe << 1 >> 1 | 1;
  /* Set up a longword, each of whose bytes is C.  */
  charmask = c | (c << 8);
  charmask |= charmask << 16;
  if (sizeof (longword) > 4)
    /* Do the shift in two steps to avoid a warning if long has 32 bits.  */
    charmask |= (charmask << 16) << 16;
  if (sizeof (longword) > 8)
    abort ();
  /* Instead of the traditional loop which tests each character,
     we will test a longword at a time.  The tricky part is testing
     if *any of the four* bytes in the longword in question are zero.  */
  for (;;)
    {
      /* We tentatively exit the loop if adding MAGIC_BITS to
	 LONGWORD fails to change any of the hole bits of LONGWORD.
	 1) Is this safe?  Will it catch all the zero bytes?
	 Suppose there is a byte with all zeros.  Any carry bits
	 propagating from its left will fall into the hole at its
	 least significant bit and stop.  Since there will be no
	 carry from its most significant bit, the LSB of the
	 byte to the left will be unchanged, and the zero will be
	 detected.
	 2) Is this worthwhile?  Will it ignore everything except
	 zero bytes?  Suppose every byte of LONGWORD has a bit set
	 somewhere.  There will be a carry into bit 8.  If bit 8
	 is set, this will carry into bit 16.  If bit 8 is clear,
	 one of bits 9-15 must be set, so there will be a carry
	 into bit 16.  Similarly, there will be a carry into bit
	 24.  If one of bits 24-30 is set, there will be a carry
	 into bit 31, so all of the hole bits will be changed.
	 The one misfire occurs when bits 24-30 are clear and bit
	 31 is set; in this case, the hole at bit 31 is not
	 changed.  If we had access to the processor carry flag,
	 we could close this loophole by putting the fourth hole
	 at bit 32!
	 So it ignores everything except 128's, when they're aligned
	 properly.
	 3) But wait!  Aren't we looking for C as well as zero?
	 Good point.  So what we do is XOR LONGWORD with a longword,
	 each of whose bytes is C.  This turns each byte that is C
	 into a zero.  */
      longword = *longword_ptr++;
      /* Add MAGIC_BITS to LONGWORD.  */
      if ((((longword + magic_bits)
	    /* Set those bits that were unchanged by the addition.  */
	    ^ ~longword)
	   /* Look at only the hole bits.  If any of the hole bits
	      are unchanged, most likely one of the bytes was a
	      zero.  */
	   & ~magic_bits) != 0
	  /* That caught zeroes.  Now test for C.  */
	  || ((((longword ^ charmask) + magic_bits) ^ ~(longword ^ charmask))
	      & ~magic_bits) != 0)
	{
	  /* Which of the bytes was C or zero?
	     If none of them were, it was a misfire; continue the search.  */
	  const unsigned char *cp = (const unsigned char *) (longword_ptr - 1);
	  if (*cp == c || *cp == '\0')
	    return (char *) cp;
	  if (*++cp == c || *cp == '\0')
	    return (char *) cp;
	  if (*++cp == c || *cp == '\0')
	    return (char *) cp;
	  if (*++cp == c || *cp == '\0')
	    return (char *) cp;
	  if (sizeof (longword) > 4)
	    {
	      if (*++cp == c || *cp == '\0')
		return (char *) cp;
	      if (*++cp == c || *cp == '\0')
		return (char *) cp;
	      if (*++cp == c || *cp == '\0')
		return (char *) cp;
	      if (*++cp == c || *cp == '\0')
		return (char *) cp;
	    }
	}
    }
  /* This should never happen.  */
  return NULL;
}

size_t strcspn(const char *str, const char *reject) {
    if (__glibc_unlikely(reject[0] == '\0') || __glibc_unlikely(reject[1] == '\0'))
    return strchrnul (str, reject [0]) - str;
  /* Use multiple small memsets to enable inlining on most targets.  */
  unsigned char table[256];
  unsigned char *p = memset (table, 0, 64);
  memset (p + 64, 0, 64);
  memset (p + 128, 0, 64);
  memset (p + 192, 0, 64);
  unsigned char *s = (unsigned char*) reject;
  unsigned char tmp;
  do
    p[tmp = *s++] = 1;
  while (tmp);
  s = (unsigned char*) str;
  if (p[s[0]]) return 0;
  if (p[s[1]]) return 1;
  if (p[s[2]]) return 2;
  if (p[s[3]]) return 3;
  s = (unsigned char *) PTR_ALIGN_DOWN (s, 4);
  unsigned int c0, c1, c2, c3;
  do
    {
      s += 4;
      c0 = p[s[0]];
      c1 = p[s[1]];
      c2 = p[s[2]];
      c3 = p[s[3]];
    }
  while ((c0 | c1 | c2 | c3) == 0);
  size_t count = s - (unsigned char *) str;
  return (c0 | c1) != 0 ? count - c0 + 1 : count - c2 + 3;
}

size_t strspn(const char *str, const char *accept) {
    if (__glibc_unlikely(accept[0] == '\0'))
    return 0;
  if (__glibc_unlikely(accept[1] == '\0'))
    {
      const char *a = str;
      for (; *str == *accept; str++);
      return str - a;
    }
  /* Use multiple small memsets to enable inlining on most targets.  */
  unsigned char table[256];
  unsigned char *p = memset (table, 0, 64);
  memset (p + 64, 0, 64);
  memset (p + 128, 0, 64);
  memset (p + 192, 0, 64);
  unsigned char *s = (unsigned char*) accept;
  /* Different from strcspn it does not add the NULL on the table
     so can avoid check if str[i] is NULL, since table['\0'] will
     be 0 and thus stopping the loop check.  */
  do
    p[*s++] = 1;
  while (*s);
  s = (unsigned char*) str;
  if (!p[s[0]]) return 0;
  if (!p[s[1]]) return 1;
  if (!p[s[2]]) return 2;
  if (!p[s[3]]) return 3;
  s = (unsigned char *) PTR_ALIGN_DOWN (s, 4);
  unsigned int c0, c1, c2, c3;
  do {
      s += 4;
      c0 = p[s[0]];
      c1 = p[s[1]];
      c2 = p[s[2]];
      c3 = p[s[3]];
  } while ((c0 & c1 & c2 & c3) != 0);
  size_t count = s - (unsigned char *) str;
  return (c0 & c1) == 0 ? count + c0 : count + c2 + 2;
}

char *strtok_r(char *s, const char *delim, char **save_ptr) {
    char *end;
  if (s == NULL)
    s = *save_ptr;
  if (*s == '\0')
    {
      *save_ptr = s;
      return NULL;
    }
  /* Scan leading delimiters.  */
  s += strspn (s, delim);
  if (*s == '\0')
    {
      *save_ptr = s;
      return NULL;
    }
  /* Find the end of the token.  */
  end = s + strcspn (s, delim);
  if (*end == '\0')
    {
      *save_ptr = end;
      return s;
    }
  /* Terminate the token and make *SAVE_PTR point past it.  */
  *end = '\0';
  *save_ptr = end + 1;
  return s;
}

char *strdup(const char *s) {
    char *news = malloc(strlen(s) + 1);
    char *temp = news;
    while(*temp++ = *s++);
    return news;
}