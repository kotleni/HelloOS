int strlen(char* str){
    int i = 0;
    while(str[i] == '\0'){ i++; }

    return i;
}

// Сравнить строки
int strcmp(const char * str1, const char * str2) {
  while (*str1 != 0 && *str2 != 0) {
    int diff = *str1 - *str2;
    if (diff != 0) {
      return diff;
    }

    str1++;
    str2++;
  }

  // One of these points to 0
  return *str1 - *str2;
}

char* append(char* a, char* b){
    char* n = (char*) malloc(sizeof(char*));

    int i = 0;
    while(a[i] != '\0'){
        n[i] = a[i];
        i++;
    }

    int x = 0;
    while(b[x] != '\0'){
        n[i] = b[x];
        i++;
        x++;
    }

    return n;
}

// void strcat(char* s1, char* s2){
//     uint8_t x, i, j = 0;
//     char *t1 = s1;
//     char *t2 = s2;

//     while (*t1) { t1++; i++; }
//     while (*t2) { t2++; j++; }

//     for (x=0; x<j+1; x++)
//         *(s1+i+x) = *(s2+x);
// }

char* strcat(char *dest, const char *src)
{
    size_t i,j;
    for (i = 0; dest[i] != '\0'; i++)
        ;
    for (j = 0; src[j] != '\0'; j++)
        dest[i+j] = src[j];
    dest[i+j] = '\0';
    return dest;
}

// Взято из википедии
void reverse(char s[]){
    int i, j;
    char c;
 
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}


void itoa(int n, char* s){
    int i, sign;
 
    if ((sign = n) < 0)  /* записываем знак */
        n = -n;          /* делаем n положительным числом */
    i = 0;
    do {       /* генерируем цифры в обратном порядке */
        s[i++] = n % 10 + '0';   /* берем следующую цифру */
    } while ((n /= 10) > 0);     /* удаляем */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

char* int2str(int i){
    char* n = malloc(sizeof(char*));
    itoa(i, n);
    return n;
}

int strsplit(char* str, char delim){
	int n = 0;
	int i = 0;
	while(str[i]){
		if(str[i] == delim){
			str[i] = 0;
			n++;
		}
		i++;
	}
	n++;
	return n;
}

void *memcpy(void *dest, const void *src, unsigned int count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

void *memset(void *dest, char val, unsigned int count)
{
    char *temp = (char *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, unsigned int count)
{
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}