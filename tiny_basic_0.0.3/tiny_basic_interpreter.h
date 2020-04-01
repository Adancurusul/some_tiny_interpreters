typedef enum
{
    ERROR = 1,
    ENDINPUT,
    NUMBER,
    STRING,
    VARIABLE,

    K_LET,
    K_PRINT,
    K_IF,
    K_THEN,
    K_ELSE,
    K_FOR,
    K_TO,
    K_NEXT,
    K_GOTO,
    K_GOSUB,
    K_RETURN,
    K_CALL,
    K_END,
    K_POKE,
    K_PEEK,

    COMMA,
    SEMICOLON,
    PLUS,
    MINUS,
    AND,
    OR,
    ASTRISK,
    SLASH,
    PERCENT,
    LEFTBRACKET,
    RIGHTBRACKET,

    LIGHTER,
    GREATER,
    EQUAL,
    CR,
} CORE_DATA;



void interpreter_init(char pro[]);
void do_interpretation(void);
int interpreter_finished(void);
typedef unsigned long long size_t;
typedef char STR[128];



int atoi(const char *src);
void *memcpy(void *dest, const void *src, int count);
char *strchr(char *str, const char c);
unsigned int strlen(const char *str);
char *strncpy(char *dest, const char *str, int count);
int strncmp(const char *str1, const char *str2, int count);
int strcmp(const char *str1, const char *str2);
char *itoa(int num, char *str, int radix);
char *strcpy(char *strDest, const char *strSrc);
void * memset(void *s, int c, size_t n);
//void dic_put(char *key, char *value);

//char *dic_value_for_key(char *key);


