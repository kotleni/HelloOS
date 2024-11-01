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

char *strncpy(char *dest, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }

    for (; i < n; i++) {
        dest[i] = '\0';
    }

    return dest;
}


char *strtok(char *str, const char *delim) {
    static char *next;
    if (str != NULL) {
        next = str;
    }
    if (next == NULL) {
        return NULL; // No more tokens
    }

    // Skip initial delimiters
    char *start = next;
    while (*start && strchr(delim, *start)) {
        start++;
    }

    if (*start == '\0') {
        next = NULL; // No more tokens
        return NULL;
    }

    // Find the end of the token
    char *end = start;
    while (*end && !strchr(delim, *end)) {
        end++;
    }

    if (*end == '\0') {
        next = NULL; // No more tokens after this one
    } else {
        *end = '\0'; // Null-terminate the token
        next = end + 1; // Move past the delimiter for the next call
    }

    return start;
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

char *strdup(const char *s) {
    char *news = malloc(strlen((char*)s) + 1);
    char *temp = news;
    while(*temp++ = *s++);
    return news;
}

/*
 * Integer to string
 */
static void print_dec(unsigned int value, unsigned int width, char * buf, int * ptr ) {
	unsigned int n_width = 1;
	unsigned int i = 9;
	while (value > i && i < UINT32_MAX) {
		n_width += 1;
		i *= 10;
		i += 9;
	}

	int printed = 0;
	while (n_width + printed < width) {
		buf[*ptr] = '0';
		*ptr += 1;
		printed += 1;
	}

	i = n_width;
	while (i > 0) {
		unsigned int n = value / 10;
		int r = value % 10;
		buf[*ptr + i - 1] = r + '0';
		i--;
		value = n;
	}
	*ptr += n_width;
}

/*
 * Hexadecimal to string
 */
void print_hex(unsigned int value, unsigned int width, char * buf, int * ptr) {
	int i = width;

	if (i == 0) i = 8;

	unsigned int n_width = 1;
	unsigned int j = 0x0F;
	while (value > j && j < UINT32_MAX) {
		n_width += 1;
		j *= 0x10;
		j += 0x0F;
	}

	while (i > (int)n_width) {
		buf[*ptr] = '0';
		*ptr += 1;
		i--;
	}

	i = (int)n_width;
	while (i-- > 0) {
		buf[*ptr + i] = "0123456789abcdef"[value & 0xF];
		value >>= 4;
	}
	*ptr += n_width;
}

void byte_to_hex(uint8_t byte, char *hex_str) {
    char hex_lookup[] = "0123456789ABCDEF";

    // Get the high and low nibbles (4 bits each)
    hex_str[0] = hex_lookup[(byte >> 4) & 0x0F]; // High nibble
    hex_str[1] = hex_lookup[byte & 0x0F];        // Low nibble
    hex_str[2] = '\0';                           // Null-terminate the string
}

size_t vasprintf(char * buf, const char *fmt, va_list args) {
	int i = 0;
	char *s;
	int ptr = 0;
	int len = strlen((char*)fmt);
	for ( ; i < len && fmt[i]; ++i) {
		if (fmt[i] != '%') {
			buf[ptr++] = fmt[i];
			continue;
		}
		++i;
		unsigned int arg_width = 0;
		while (fmt[i] >= '0' && fmt[i] <= '9') {
			arg_width *= 10;
			arg_width += fmt[i] - '0';
			++i;
		}
		/* fmt[i] == '%' */
		switch (fmt[i]) {
			case 's': /* String pointer -> String */
				s = (char *)va_arg(args, char *);
				while (*s) {
					buf[ptr++] = *s++;
				}
				break;
			case 'c': /* Single character */
				buf[ptr++] = (char)va_arg(args, int);
				break;
			case 'x': /* Hexadecimal number */
				print_hex((unsigned long)va_arg(args, unsigned long), arg_width, buf, &ptr);
				break;
			case 'd': /* Decimal number */
				print_dec((unsigned long)va_arg(args, unsigned long), arg_width, buf, &ptr);
				break;
			case '%': /* Escape */
				buf[ptr++] = '%';
				break;
			default: /* Nothing at all, just dump it */
				buf[ptr++] = fmt[i];
				break;
		}
	}
	/* Ensure the buffer ends in a null */
	buf[ptr] = '\0';
	return ptr;
}

int sprintf(
		char * buf,
		const char *fmt,
		...
	   ) {
	va_list args;
	va_start(args, fmt);
	int out = vasprintf(buf, fmt, args);
	va_end(args);
	return out;
}