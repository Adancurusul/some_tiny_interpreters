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

    MATH,
} CORE_DATA;

typedef enum{
SQRT = 1,
EXP,
POW,
POWF,
LOG,
LN,
SIN,
COS,
TAN,
ARCSIN,
ARCCOS,
ARCTAN,
SINH,
COSH,
TANH,
FABS,
PI,
NOTHING


}OP_MATH;

typedef enum
{
    var_null = 0,
    var_double,
    var_string
} variant_type;


typedef char STR[128];
typedef struct
{
    variant_type type;
    union {
        int i;
        double d;
        STR s;
    } U;
} VARIANT;

typedef struct varname
{
    STR name ;
    int name_ptr;
} VAR_NAME;

void interpreter_init(char pro[]);
void do_interpretation(void);
int interpreter_finished(void);





int atoi(const char *src);
void *memcpy(void *dest, const void *src, int count);
char *strchr(char *str, const char c);
unsigned int strlen(const char *str);
char *strncpy(char *dest, const char *str, int count);
int strncmp(const char *str1, const char *str2, int count);
int strcmp(const char* str1, const char* str2);
char* itoa(int num,char* str,int radix);
double math_handler(OP_MATH name );

//void dic_put(char *key, char *value);

//char *dic_value_for_key(char *key);


