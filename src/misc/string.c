// Добавить к строке строку
int strcat_i;
void strcat(char* s, const char* t){
    strcat_i = 0;

    while (s[strcat_i] != '\0')
        strcat_i++;
    while (*t != '\0')
        s[strcat_i++] = *t++;
    s[strcat_i] = '\0';
}

// Получить длину строки
int strlen(char* str){
    int i = 0;
    while(str[i] == '\0'){ i++; }

    return i;
}

// Сравнить строки
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

    // один из этих пунктов == 0
    return *str1 - *str2;
}
// Развернуть строку
void reverse(char s[]){
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

// int to char
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

void* memset(void* ptr, int value, unsigned int num) {
   unsigned char* ptr_byte = (unsigned char*)ptr;

   for (unsigned int i = 0; i < num; ptr_byte[i] = (unsigned char)value, i++);
   return ptr;
}
