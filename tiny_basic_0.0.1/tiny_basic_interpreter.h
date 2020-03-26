void interpreter_init(char pro[]);
void do_interpretation(void);
int interpreter_finished(void);

typedef char STR[128];



int atoi(const char *src);
void *memcpy(void *dest, const void *src, int count);
char *strchr(char *str, const char c);
unsigned int strlen(const char *str);
char *strncpy(char *dest, const char *str, int count);
int strncmp(const char *str1, const char *str2, int count);
int strcmp(const char* str1, const char* str2);
char* itoa(int num,char* str,int radix);

void dic_put(char *key, char *value);

char *dic_value_for_key(char *key);


