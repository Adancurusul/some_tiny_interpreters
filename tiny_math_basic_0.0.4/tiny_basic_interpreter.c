/*
author : Adancurusul
email  :chen.yuheng@nexuslink.cn
version：0.0.4   2020/4/8
介绍：
这是一个可以嵌入任何只要支持printf函数的系统或单片机中的基于basic语法扩展了数学方面使用和移植性的语言解释器；
内存开销也极小
完成了一个基本的词法分析器对语句拆分
代码量仅千行，但已经支持basic中let,print,if,then,else,for,to,next,goto,gosub,return,call,end
peek ,poke语句
0.0.4新增pow,powf,sqrt,exp,log,ln,sin,cos,tan,arctan,artsin,arccos,sinh,consh,tanh
库及其综合运算（见use.c（3 iaa = pow(2,3)\n50 t = sqrt(iaa*3)\n60 print \"sqrt(pow(2,3)*3) is:\",t+20\n）
*******************************************************************************************
使用只用传入程序的字符串数组到interpreter_init
然后do_interpretation即可
interpreter_finished作为结束标志
目标平台支持printf即可
*******************************************************************************************
0.0.4相对0.0.3支持了math库部分功能
下一步完善数学库和矩阵运算的支持
本解释器目标是作为一种高移植性，能进行科学运算的语言。
敬请期待后续

*/


/////
#define  MY_PRINT(...) printf(__VA_ARGS__) //如果目标平台通过其他方式输出，修改此行
/////




#include <stdio.h> //MY_PRINT
#include "math_dict/mymath.h"
#include "tiny_basic_interpreter.h"
#define MAX_GOSUB_DEPTH 20//gosub 语句最多20次嵌套
#define MAX_STR_LENGTHSTR_LENGTH 50//string长度最多50字节
#define MAX_FOR_DEPTHMAX_FOR_DEPTH 6//for循环最多嵌套次数5
#define MAX_VARNUM 40 //最多储存40个变量
#define MAX_NUMLEN 20  //number最大长度
#define isdigit(c) ((c) >= '0' && (c) <= '9') //检测是否是数字
#define CHANGE_LOWER 'A' - 'a'  //大写转小写

/////
//#define  MY_PRINT(...) printf(__VA_ARGS__) //如果目标平台通过其他方式输出，修改此行
/////


static char string[MAX_STR_LENGTHSTR_LENGTH]; //全局字符串，为防止指针滥用造成内存问题部分参数使用这个传递
static int gosub_stack[MAX_GOSUB_DEPTH]; //gosub的栈
static int gosub_stack_ptr; //gosub 的指针
static char const *program_ptr, *ptr, *nextptr, *startptr;//用于词法分析的指针
typedef struct for_state
{
    int line_after_for;
    STR for_variable;
    int to;
} FOR_STATE;//for循环使用


//typedef char STR[128];

static VAR_NAME search_index[MAX_VARNUM];//为寻找变量对应储存区实现的一个静态数组

static VARIANT empty = {var_null, 0}; //索引variable为检测到值的返回
static VARIANT var_mem[MAX_VARNUM]; //储存变量
int var_mem_ptr = 0;//初始化指针

static FOR_STATE for_stack[MAX_FOR_DEPTHMAX_FOR_DEPTH];//for循环栈
static int for_stack_ptr;//for 栈指针
//static char variables[MAX_VARNUM];
static int ended;  
static VARIANT expr(void);
static void line_handler(void);
static void handler(void);
int search_finished(void);

///////////////////////////////////////////////
//////////////////////////////////////////////
//下面是string的替代，如目标平台支持下面几个函数可去掉
VARIANT get_variable( char *name);
int atoi(const char *src);
void *memcpy(void *dest, const void *src, int count);
char *strchr(char *str, const char c);
unsigned int strlen(const char *str);
char *strncpy(char *dest, const char *str, int count);
int strncmp(const char *str1, const char *str2, int count);
int strcmp(const char *str1, const char *str2);
char *itoa(int num, char *str, int radix);
char *strcpy(char *strDest, const char *strSrc);
void* memset(void* dst,int val, int count);
void* memset(void* dst,int val, int count)
{
    void* ret = dst;
    while(count--)
    {
        *(char*)dst = (char)val;
        dst = (char*)dst + 1; 
    }
    return ret;
}
char *itoa(int num, char *str, int radix)
{
    char index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; //索引表
    unsigned unum;                                         //存放要转换的整数的绝对值,转换的整数可能是负数
    int i = 0, j, k;                                       //i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；转换后顺序是逆序的，有正负的情况，k用来指示调整顺序的开始位置;j用来指示调整顺序时的交换。

    //获取要转换的整数的绝对值
    if (radix == 10 && num < 0) //要转换成十进制数并且是负数
    {
        unum = (unsigned)-num; //将num的绝对值赋给unum
        str[i++] = '-';        //在字符串最前面设置为'-'号，并且索引加1
    }
    else
        unum = (unsigned)num; //若是num为正，直接赋值给unum

    //转换部分，注意转换后是逆序的
    do
    {
        str[i++] = index[unum % (unsigned)radix]; //取unum的最后一位，并设置为str对应位，指示索引加1
        unum /= radix;                            //unum去掉最后一位

    } while (unum); //直至unum为0退出循环

    str[i] = '\0'; //在字符串最后添加'\0'字符，c语言字符串以'\0'结束。

    //将顺序调整过来
    if (str[0] == '-')
        k = 1; //如果是负数，符号不用调整，从符号后面开始调整
    else
        k = 0; //不是负数，全部都要调整

    char temp;                         //临时变量，交换两个值时用到
    for (j = k; j <= (i - 1) / 2; j++) //头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1
    {
        temp = str[j];               //头部赋值给临时变量
        str[j] = str[i - 1 + k - j]; //尾部赋值给头部
        str[i - 1 + k - j] = temp;   //将临时变量的值(其实就是之前的头部值)赋给尾部
    }

    return str; //返回转换后的字符串
}

char *strncpy(char *dest, const char *str, int count)
{
    //assert(dest != NULL&&str != NULL);
    char *ret = dest;
    while (count-- && (*dest++ = *str++))
    {
        ;
    }
    if (count > 0) //当上述判断条件不为真时并且count未到零，在dest后继续加/0；
    {
        while (count--)
        {
            *dest++ = '/0';
        }
    }
    return ret;
}

int strcmp(const char *str1, const char *str2)
{
    int ret = 0;
    while (!(ret = *(unsigned char *)str1 - *(unsigned char *)str2) && *str1)
    {
        str1++;
        str2++;
    }
    if (ret < 0)
    {
        return -1;
    }
    else if (ret > 0)
    {
        return 1;
    }
    return 0;
}

char *strcpy(char *strDest, const char *strSrc)
{
    char *p = NULL;
    if (strDest == NULL || strSrc == NULL)
    {
        return NULL;
    }
    p = strDest;
    while ((*strDest++ = *strSrc++) != '\0'){
        //MY_PRINT("ch\n");
    }
        ;
    return p;
}

int strncmp(const char *str1, const char *str2, int count)
{

    if (!count)
        return 0;
    while (--count && *str1 && *str1 == *str2)
    {
        str1++;
        str2++;
    }
    return (*str1 - *str2);
}

void *memcpy(void *dest, const void *src, int count)
{
    if (dest == NULL || src == NULL)
    {
        return NULL;
    }
    char *pdest = (char *)dest;
    char *psrc = (char *)src;
    while (count--)
    {
        *pdest++ = *psrc++;
    }
    return dest;
}

char *strchr(char *str, const char c)
{

    while (*str != '\0' && *str != c)
    {
        str++;
    }

    return (*str == c ? str : NULL);
}

unsigned int strlen(const char *str)
{

    unsigned length = 0;
    while (*str != '\0')
    {
        length++;
        str++;
    }
    return length;
}

char * strtrim(char *s) {
    char *p = s;
    char *q = s;
    char *end = s;
    while (*p==' ' || *p=='\t') ++p;
    while (*q = *p) {
        if (*q!=' ' && *q!='\t') end = q+1;
        ++q, ++p;
    }
    *end = '\0';

    return s;
}

////////////////////////////////////////////
///////////////////////////////////////////
///////////////////////////////////////////

double atof(const char *str)
{
    const char *p = str;
    int sign = 1;
    while (*p == ' ')
        ++p;       //忽略前置空格
    if (*p == '-') //考虑是否有符号位
    {
        sign = -1;
        ++p;
    }
    else if (*p == '+')
        ++p;
    int hasDot = 0, hasE = 0;
    double integerPart = 0.0, decimalPart = 0.0;
    //遇到'e'或'.'字符则退出循环,设置hasE和hasDot。
    for (; *p; ++p)
    {
        if (isdigit(*p)) //若p指向的字符为数字则计算当前整数部分的值
            integerPart = 10 * integerPart + *p - '0';
        else if (*p == '.')
        {
            hasDot = 1;
            p++;
            break;
        }
        else if (*p == 'e' || *p == 'E')
        {
            hasE = 1;
            p++;
            break;
        }
        else //如果遇到非法字符,则截取合法字符得到的数值,返回结果。
            return integerPart;
    }

    //上一部分循环中断有三种情况,一是遍历完成,这种情况下一部分的循环会自动跳过；其次便是是遇到'.'或'e',两种hasE和hasDot只可能一个为真,若hasDot为真则计算小数部分,若hasE为真则计算指数部分。
    int decimalDigits = 1;
    int exponential = 0;
    for (; *p; p++)
    {
        if (hasDot && isdigit(*p))
            decimalPart += (*p - '0') / pow(10, decimalDigits++);
        else if (hasDot && (*p == 'e' || *p == 'E'))
        {
            integerPart += decimalPart;
            decimalPart = 0.0;
            hasE = 1;
            ++p;
            break;
        }
        else if (hasE && isdigit(*p))
            exponential = 10 * exponential + *p - '0';
        else
            break;
    }
    //上一部分较难理解的就是else if (hasDot && (*p == 'e' || *p == 'E')) 这一特殊情况,对于合法的浮点数,出现'.'字符后,仍然有可能是科学计数法表示,但是出现'e'之后,指数部分不能为小数(这符合<string.h>对atof()的定义)。这种情况变量IntegerPart和decimalPart都是科学计数法的基数,因此有integerPart += decimalPart(这使得IntergerPart的命名可能欠妥,BasePart可能是一种好的选择)。
    //上一部分循环结束一般情况下就能返回结果了,除非遇到前文所述的特殊情况，对于特殊情况需要继续计算指数。
    if (hasE && hasDot)
        for (; *p; p++)
            if (isdigit(*p))
                exponential = 10 * exponential + *p - '0';
    return sign * (integerPart * pow(10, exponential) + decimalPart);
}

int atoi(const char *src)
{
    //assert(NULL != src);
    int _num = 0;
    int _sign = 0;
    while ('0' == *src || ' ' == *src || '\n' == *src || '-' == *src || '+' == *src) //如果有空,空格或者换行跳过去
    {
        if (*src == '-')
            _sign = 1;

        src++;
    }

    while (*src >= '0' && *src <= '9')
    {
        _num = _num * 10 + *src - '0';
        src++;
    }

    if (_sign == 1)
        return -_num;
    else
        return _num;
}

typedef struct keyword_token
{
    char *keyword;
    CORE_DATA token;
} KEYS;//获取token的结构体
typedef struct
{
    char *math_keyword;
    OP_MATH math_name;
} MATH_KEYS;//获取数学库的结构体

OP_MATH op_now = NOTHING;//初始化
static CORE_DATA token_now = ERROR;//初始化

const MATH_KEYS math_keywords[] = {
    /*SQRT = 1,EXP,POW,POWF,LOG,LN,SIN,COS,TAN,ARCSIN,ARCCOS,ARCTAN,SINH,CONSH,TANH,NOTHING*/
    {"sqrt", SQRT},
    {"exp", EXP},
    {"pow", POW},
    {"powf", POWF},
    {"log", LOG},
    {"ln", LN},
    {"sin", SIN},
    {"cos", COS},
    {"tan", TAN},
    {"arcsin", ARCSIN},
    {"arccos", ARCCOS},
    {"arctan", ARCTAN},
    {"sinh", SINH},
    {"consh", COSH},
    {"tanh", TANH},
    {NULL, NOTHING},
};//用于匹配math
const KEYS keywords[] = {
    {"let", K_LET},
    {"print", K_PRINT},
    {"if", K_IF},
    {"then", K_THEN},
    {"else", K_ELSE},
    {"for", K_FOR},
    {"to", K_TO},
    {"next", K_NEXT},
    {"goto", K_GOTO},
    {"gosub", K_GOSUB},
    {"return", K_RETURN},
    {"call", K_CALL},
    {"end", K_END},
    {"poke", K_POKE},
    {"peek", K_PEEK},
    {NULL, ERROR}};//用于匹配keys

static CORE_DATA if_one_char(void) //如果搜索到为单个字符时的处理函数
{
    switch (*ptr)
    {
    case '\n':
        return CR;
        break;
    case ',':
        return COMMA;
        break;
    case ';':
        return SEMICOLON;
        break;
    case '+':
        return PLUS;
        break;
    case '-':
        return MINUS;
        break;
    case '&':
        return AND;
        break;
    case '|':
        return OR;
        break;
    case '/':
        return SLASH;
        break;
    case '*':
        return ASTRISK;
        break;
    case '(':
        return LEFTBRACKET;
        break;
    case ')':
        return RIGHTBRACKET;
        break;
    case '<':
        return LIGHTER;
        break;
    case '>':
        return GREATER;
        break;

    case '=':
        return EQUAL;
        break;
    default:
        return 0;
        break;
    }
}

static CORE_DATA get_next_token(void) //词法分析器核心部分：词法拆分
{
    KEYS const *kt;
    MATH_KEYS const *mk;
    int i;

    if (*ptr == 0)
    {
        return ENDINPUT;
    }

    if (isdigit(*ptr))
    {
        for (i = 0; i < MAX_NUMLEN; ++i)
        {
            if (!isdigit(ptr[i]) && ptr[i] != '.' && ptr[i] != 'E' && ptr[i] != 'e')
            {

                if (ptr[i] != '.' && ptr[i] != 'E' && ptr[i] != 'e')
                {
                    if (i > 0)
                    {
                        nextptr = ptr + i;
                        //MY_PRINT("numbernow :%d\n",ptr[i]);
                        return NUMBER;
                    }
                    else
                    {
                        MY_PRINT("get_next_token: error due to too short number\n");
                        return ERROR;
                    }
                }
            }
            if (!isdigit(ptr[i]) && ptr[i] != '.' && ptr[i] != 'E' && ptr[i] != 'e')
            {
                if (ptr[i] != '.')
                {
                    MY_PRINT("get_next_token: error due to malformed number\n");
                    return ERROR;
                }
            }
        }
        MY_PRINT("get_next_token: error due to too long number\n");
        return ERROR;
    }
    else if (if_one_char())//单个字符
    {
        nextptr = ptr + 1;
        return if_one_char();
    }
    else if (*ptr == '"')
    {
        nextptr = ptr;
        do
        {
            ++nextptr;
        } while (*nextptr != '"');
        ++nextptr;
        return STRING;
    }
    else
    {
        for (kt = keywords; kt->keyword != NULL; ++kt)//寻找是否为token
        {
            if (strncmp(ptr, kt->keyword, strlen(kt->keyword)) == 0)
            {
                nextptr = ptr + strlen(kt->keyword);
                return kt->token;
            }
        }
        for (mk = math_keywords; mk->math_keyword != NULL; ++mk)//寻找是否为数学库
        {
            if (strncmp(ptr, mk->math_keyword, strlen(mk->math_keyword)) == 0)
            {
                nextptr = ptr + strlen(mk->math_keyword);
                op_now = mk->math_name;
                
                
                return MATH;
            }
        }
    }
    int status = 0;
    startptr = ptr;
    while (*ptr >= 'a' && *ptr <= 'z')//查找变量
    {
        status = 1;
        nextptr = ptr + 1;
        //MY_PRINT("%c:ptrnow\n",*ptr);
        ++ptr;
    }
    if (status)
    {
        return VARIABLE;
    }

    return ERROR;
}

void search_init(const char *program)//初始化查找
{
    ptr = program;
    token_now = get_next_token();
}

CORE_DATA search_token(void)
{
    return token_now;
}

void search_next(void)
{

    if (search_finished())
    {
        return;
    }

    ptr = nextptr;
    while (*ptr == ' ')
    {
        ++ptr;
    }
    token_now = get_next_token();
    return;
}

double search_num(void)
{
    return atof(ptr);
}

void search_string(char *dest, int len)//查找字符串
{
    char *string_end;
    register int string_len;

    if (search_token() != STRING)
    {
        return;
    }
    string_end = strchr(ptr + 1, '"');
    if (string_end == NULL)
    {
        return;
        
    }
    string_len = string_end - ptr - 1;
    if (len < string_len)
    {
        string_len = len;
    }
    memcpy(dest, ptr + 1, string_len);
    dest[string_len] = 0;
}

void search_error_print(void)
{
    MY_PRINT("search_error_print: '%s'\n", ptr);
}

int search_finished(void)
{
    return *ptr == 0 || token_now == ENDINPUT;
}

char *variable_now(void)
{
    //return *ptr - 'a';
    //STR str_now;
    char *st;
    char *a;
    int i = 0;
    //MY_PRINT("%c:start\n",*startptr);
    while ('a' <= *ptr && 'z' >= *ptr)
    {
        //MY_PRINT("before%c",*ptr);
        if (*ptr != ' ')
        {
            //MY_PRINT("%c:now",*ptr);
            st[i] = *ptr;
            i++;
            ++ptr;
        }
    }
    //MY_PRINT("num of value %d\n",ptr-startptr);
    //memcpy(str_now,0,50);//清空
    
    memset(string,0,MAX_VARNUM);
    memcpy(string, startptr, ptr - startptr);
    //str_now[ptr-startptr] = 0; 
    //MY_PRINT("%c%c\n",*startptr ,*(startptr+1));
    //a = st;
    //st = str_now;
    st = string;
    //MY_PRINT("strnow:'%s'\n",st);
    //st = strtrim(st);
    //st[ptr-startptr]=0;
    return st;
}
/*
char lower(char pro[]){
    
    int length = strlen(pro);
    MY_PRINT("%d\n",length);
    
    for (int i=0;i<length;i++){
        if (pro[i]>='A'&&pro[i]<='Z'){
            pro[i] += CHANGE_LOWER;
        }
    }
    MY_PRINT("%s",pro);
    char *p = pro;
    return pro;

}
*/
void interpreter_init(char pro[])
{
    register int length = strlen(pro);
    //MY_PRINT("%d\n",length);

    for (int i = 0; i < length; i++)
    {
        if (pro[i] >= 'A' && pro[i] <= 'Z')
        {
            pro[i] -= CHANGE_LOWER;
            //MY_PRINT("%c",pro[i]);
            //while (1){

            //}
        }
    }
    //MY_PRINT("%s",pro);
    char *program = pro;

    //char *program = pro;
    //MY_PRINT("the%s",program);
    program_ptr = program;
    for_stack_ptr = gosub_stack_ptr = 0;
    search_init(program);
    ended = 0;
}

static void accept_token(int token)
{
    if (token != search_token())
    {
        search_error_print();
    }
    //MY_PRINT("token%d\n",token);
    search_next();
}

static VARIANT varfactor(void)
{
    register double r;
    
    
    //char  *str;

    VARIANT t;

    char const *st = variable_now();
    
    //STR str_ow;
    //MY_PRINT("set_name_now:'%s'\n",string);
    t = get_variable(string);
    
    if(t.type ==var_null){
        MY_PRINT("error :wrong variable");
    }
    //MY_PRINT("ooooaa%d\n",r);

    accept_token(VARIABLE);
    return t;
}
    //st[ptr-startptr] = '\0';
    //strcpy(str,st);
    //memcpy(str,st,ptr-startptr+1);
    //sMY_PRINT(str,"%s",st);
    //str[ptr-startptr] = 0;
    //MY_PRINT("set_name_now:'%s'\n",st);
    //MY_PRINT("return: st %s\n",str_ow);
static VARIANT factor(void)
{
    register double r;
    double math_ret;
    int type;
    VARIANT t;
    //puts("\ninside\n");
    switch (search_token())
    {
        
    case NUMBER:
        r = search_num();
        type = var_double;
        t.type = type;
        t.U.d = r;
        accept_token(NUMBER);
        break;
    case LEFTBRACKET:
        accept_token(LEFTBRACKET);
        t = expr();
        accept_token(RIGHTBRACKET);
        break;
    case MATH:
    //puts("okk");
        math_ret = math_handler(op_now);
        t.type = var_double;
        t.U.d = math_ret;
        break;
    default:
        t = varfactor();
        break;
    }
    // MY_PRINT("thenum :%g\n",r);
    return t;
}

static VARIANT term(void)
{
    register double f1, f2;
    register CORE_DATA op;
    VARIANT t1, t2;

    t1 = factor();
    switch (t1.type)
    {
    case (var_double):
        f1 = t1.U.d;

        //MY_PRINT("value in term:%g\n",f1);
        op = search_token();
        while (op == ASTRISK ||
               op == SLASH ||
               op == PERCENT)
        {
            search_next();
            t2 = factor();
            switch (t2.type)
            {
            case (var_double):
                f2 = t2.U.d;
                break;
            }
            switch (op)
            {
            case ASTRISK:
                f1 = f1 * f2;
                break;
            case SLASH:
                f1 = f1 / f2;
                break;
            case PERCENT:
                f1 = (int)f1 % (int)f2;
                break;
            }
            op = search_token();
        }

        t1.U.d = f1;
        return t1;
        break;
    }
}

static VARIANT expr(void)
{
    register double t1, t2;
    register CORE_DATA op;
    VARIANT v1, v2;

    v1 = term();
    switch (v1.type)
    {
    case (var_double):
        t1 = v1.U.d;

        //MY_PRINT("exprvalue : %g\n",t1);
        op = search_token();
        while (op == PLUS ||
               op == MINUS ||
               op == AND ||
               op == OR)
        {
            search_next();
            v2 = term();
            t2 = v2.U.d;
            switch (op)
            {
            case PLUS:
                t1 = t1 + t2;
                break;
            case MINUS:
                t1 = t1 - t2;
                break;
            case AND:
                t1 = (int)t1 & (int)t2;
                break;
            case OR:
                t1 = (int)t1 | (int)t2;
                break;
            }
            op = search_token();
        }
        v1.U.d = t1;
        return v1;
        break;
    }
}

static double relation(void)
{
    register double t1, t2;
    register CORE_DATA op;
    int r1;
    VARIANT v1, v2;

    v1 = expr();

    switch (v1.type)
    {
    case (var_double):
        t1 = v1.U.d;
        op = search_token();
        while (op == LIGHTER ||
               op == GREATER ||
               op == EQUAL)
        {
            search_next();
            v2 = expr();
            t2 = v2.U.d;
            switch (op)
            {
            case LIGHTER:
                r1 = t1 < t2;
                break;
            case GREATER:
                r1 = t1 > t2;
                break;
            case EQUAL:
                r1 = t1 == t2;
                break;
            }
            op = search_token();
        }
        return r1;
        break;
    }
}

static void jump_linenum(int linenum)
{
    search_init(program_ptr);
    while ((int)search_num() != linenum)
    {
        do
        {
            do
            {
                search_next();
            } while (search_token() != CR &&
                     search_token() != ENDINPUT);
            if (search_token() == CR)
            {
                search_next();
            }
        } while (search_token() != NUMBER);
    }
}

static void goto_handler(void)
{
    accept_token(K_GOTO);
    jump_linenum((int)search_num()); //强制转为int
}

static void print_handler(void)
{
    accept_token(K_PRINT);
    do
    {
        
        if (search_token() == STRING)
        {
            search_string(string, sizeof(string));
            MY_PRINT("%s", string);
            search_next();
        }
        else if (search_token() == COMMA)
        {
            
            MY_PRINT(" ");
            search_next();
        }
        else if (search_token() == SEMICOLON)
        {
            search_next();
        }
        else if (search_token() == VARIABLE ||
                 search_token() == NUMBER)
        {
            //MY_PRINT("getit");
            MY_PRINT("%f", expr().U.d);
        }
        else
        {
            break;
        }
    } while (search_token() != CR &&
             search_token() != ENDINPUT);
    MY_PRINT("\n");
    search_next();
}

static void if_handler(void)
{
    register int r;

    accept_token(K_IF);

    r = relation();
    accept_token(K_THEN);
    if (r)
    {
        handler();
    }
    else
    {
        do
        {
            search_next();
        } while (search_token() != K_ELSE &&
                 search_token() != CR &&
                 search_token() != ENDINPUT);
        if (search_token() == K_ELSE)
        {
            search_next();
            handler();
        }
        else if (search_token() == CR)
        {
            search_next();
        }
    }
}

static void let_handler(void)
{
    register VARIANT var;
STR s_now ;
    char *st_now = variable_now();
     strcpy(s_now ,st_now);
    //MY_PRINT("varnum:%s and ",s_now);

    accept_token(VARIABLE);
    accept_token(EQUAL);
    var = expr();
    //MY_PRINT("varnum:%s and ",s_now);
    //MY_PRINT("valuenum:%g\n", var.U.d);
    set_variable(s_now, var);
    accept_token(CR);
}

static void gosub_handler(void)
{
    register int linenum;
    accept_token(K_GOSUB);
    linenum = search_num();
    accept_token(NUMBER);
    accept_token(CR);
    if (gosub_stack_ptr < MAX_GOSUB_DEPTH)
    {
        gosub_stack[gosub_stack_ptr] = search_num();
        gosub_stack_ptr++;
        jump_linenum(linenum);
        //MY_PRINT("jump");
    }
    else
    {
    }
}

static void return_handler(void)
{
    accept_token(K_RETURN);
    if (gosub_stack_ptr > 0)
    {
        gosub_stack_ptr--;
        jump_linenum(gosub_stack[gosub_stack_ptr]);
    }
    else
    {
    }
}

static void next_handler(void)
{
    STR var;
    
    accept_token(K_NEXT);
    strcpy(var,variable_now());
    //MY_PRINT("%d::::%d\n",strlen(var),strlen(for_stack[for_stack_ptr - 1].for_variable));
    accept_token(VARIABLE);
    if (for_stack_ptr > 0 &&
        !strcmp(var, for_stack[for_stack_ptr - 1].for_variable))
    {
        //MY_PRINT("into");
        VARIANT v = get_variable(var);
        
        double t0 = v.U.d;
        v.U.d = t0 + 1;
        //MY_PRINT("varfor:'%g'\n",v.U.d);
        int t = (int)t0;

        set_variable(var,v);
        //MY_PRINT("varforend:'%g'\n",v.U.d);
        if (t <= for_stack[for_stack_ptr - 1].to-1)
        {
            jump_linenum(for_stack[for_stack_ptr - 1].line_after_for);
        }
        else
        {
            for_stack_ptr--;
            accept_token(CR);
        }
    }
    else
    {
        accept_token(CR);
    }
}

static void for_handler(void)
{
    char *for_variable;
    int to;
    STR for_v;

    accept_token(K_FOR);
    for_variable = variable_now();
    strcpy(for_v,for_variable);
    //MY_PRINT("for_now:%s\n",for_v);
    accept_token(VARIABLE);
    accept_token(EQUAL);
    VARIANT t = expr();
    
    set_variable(for_v, t);
    
    accept_token(K_TO);
    VARIANT v = expr();
    //MY_PRINT("to:%g\n",v.U.d);
    to = (int)v.U.d;
    accept_token(CR);
    
    if (for_stack_ptr < MAX_FOR_DEPTHMAX_FOR_DEPTH)
    {
        for_stack[for_stack_ptr].line_after_for = (int)search_num();
        //MY_PRINT("now:%g\n",search_num());
        

        strcpy(for_stack[for_stack_ptr].for_variable,for_v) ;
        //MY_PRINT("now:var: \"%s\"'%s'",for_v,for_stack[for_stack_ptr].for_variable);
        for_stack[for_stack_ptr].to = to;

        for_stack_ptr++;
    }
    else
    {
    }
}

static void end_handler(void)
{
    accept_token(K_END);
    ended = 1;
}

static void peek_handler()
{
    char *var;
    VARIANT dst;
    accept_token(K_PEEK);
    var = variable_now();
    accept_token(VARIABLE);
    accept_token(COMMA);
    dst = expr();
    set_variable(var, dst);
}
static void poke_handler()
{
    int *dst;
    VARIANT var;

    accept_token(K_POKE);
    //MY_PRINT("okk");
    var = expr();
    dst = (int)var.U.d;
    accept_token(COMMA);

    var = expr();

    *dst = (int)var.U.d;

    accept_token(CR);
}
static void line_handler(void)
{

    accept_token(NUMBER);
    handler();
    return;
}

static void handler(void)
{

    register CORE_DATA token;

    token = search_token();
    //MY_PRINT("aaa%daaa\n",token);
    switch (token)
    {
    case K_PRINT:
        //MY_PRINT("print\n handler\n");
        print_handler();
        break;
    case K_IF:
        //MY_PRINT("if\n handler\n");
        if_handler();
        break;
    case K_GOTO:
        //MY_PRINT("goto\n handler\n");
        goto_handler();
        break;
    case K_GOSUB:
        //MY_PRINT("gosub\n handler\n");
        //MY_PRINT("gosub");
        gosub_handler();
        break;
    case K_RETURN:
        //MY_PRINT("return\n handler\n");
        return_handler();
        break;
    case K_FOR:

        for_handler();
        break;
    case K_NEXT:
        //MY_PRINT("next\n handler\n");
        next_handler();
        break;
    case K_END:
        //MY_PRINT("dne\n handler\n");
        end_handler();
        break;
    case K_LET:
        // MY_PRINT("let\n handler\n");
        accept_token(K_LET);
        /* Fall through. */
    case VARIABLE:
        //MY_PRINT("variable\n handler\n");
        let_handler();
        break;
    case K_POKE:
        poke_handler();
        break;
    case K_PEEK:
        peek_handler();
        break;
    default:
        break;
        MY_PRINT("!!!!!!!!!!error!!!!!!!!!!!\n");
    }
}

void do_interpretation(void)
{
    if (search_finished())
    {

        return;
    }

    line_handler();
}

int interpreter_finished(void)
{
    return ended || search_finished();
}

int if_variable_existed(char *name) //判断变量是否已经存在
{

    for (int i = 0; i < var_mem_ptr ; ++i)
    {
  
       
        if (!strcmp(name, search_index[i].name))
        {
            //MY_PRINT("indexnow:%d,namenow:%s,ptrnow:%d\n",i,search_index[i].name,search_index[i].name_ptr);
            int var_num_now = search_index[i].name_ptr;
            //MY_PRINT("\n,%s,%s,%s,%s\n", search_index[0].name, search_index[1].name, search_index[2].name, search_index[3].name);
            //MY_PRINT("\nxiagntong :%s,%d\n",name,var_num_now);
            return var_num_now;
            
        }
        
    }
    
    return -1;
}

void set_variable(char * name, VARIANT value) //
{
    if (var_mem_ptr >= 0 && var_mem_ptr <= MAX_VARNUM) //在变量数量范围内
    //MY_PRINT("input val:%g\n",value.U.d);
    {
        int t = if_variable_existed(name); //变量是否已经存在
        //MY_PRINT("tnow:%d",t);
        if (t == -1)
        {
            VAR_NAME v_n;
            VARIANT val;
            char value_str[MAX_NUMLEN];
            //MY_PRINT("valuename :%s and ",name);
            //MY_PRINT("value_to_set :%g\n",value.U.d);
            //val.type = var_double;
            val = value;
            v_n.name_ptr = var_mem_ptr;
            //MY_PRINT("ptr %d\n" ,v_n.name_ptr);
            //itoa(value, value_str, 10);
            //search_index[var_mem_ptr].name=name;
            strcpy(search_index[var_mem_ptr].name, name);
            search_index[var_mem_ptr].name_ptr = var_mem_ptr;
            var_mem[var_mem_ptr] = val;
            // MY_PRINT("now i :%d\n",var_mem_ptr);
            //MY_PRINT("\nthings:%g,%g,%g,%g,%g\n",var_mem[0].U.d,var_mem[1].U.d,var_mem[2].U.d,var_mem[3].U.d);
            var_mem_ptr++;
        }
        else
        {
            
            VAR_NAME v_n;
            VARIANT val;
            char value_str[MAX_NUMLEN];

            val.type = var_double;
            val = value;
            //MY_PRINT("\nnow val :%d\n",val.U.d);
            v_n.name_ptr = t;
            //MY_PRINT("\nt:%d\n",t);
            //itoa(value, value_str, 10);
            //search_index[var_mem_ptr].name=name;
            strcpy(search_index[t].name, name);
            //search_index[t].name_ptr = var_mem_ptr;
            var_mem[t] = val;
            // MY_PRINT("now i :%d\n",var_mem_ptr);
            //MY_PRINT("now giaogiao :%g,t:%d\n",var_mem[t].U.d,t);
            //MY_PRINT("\nthings:%g,%g,%g,%g,%g\n",var_mem[0].U.d,var_mem[1].U.d,var_mem[2].U.d,var_mem[3].U.d);
        }
    }
}

VARIANT get_variable(char *name) //取出变量并返回
{
name = string;
//MY_PRINT("hello\n");
    for (int i = 0; i < var_mem_ptr + 1; i++)
    {
//MY_PRINT("/////\nnow ::::%g\nnext:::%g\nnenext::%g\n////////\n",
//var_mem[search_index[i].name_ptr].U.d,var_mem[search_index[i+1].name_ptr].U.d,var_mem[search_index[i+2].name_ptr].U.d);
        //MY_PRINT("namenow: '%s' \nname search:'%s'",name,search_index[i].name);
        int a = strcmp(name, search_index[i].name);

        if (!a)
        {
            int var_num_now = search_index[i].name_ptr;
            //MY_PRINT("var_get : %g  type: %d\n",var_mem[var_num_now].U.d,var_mem[var_num_now].type);
            return var_mem[var_num_now];
        }
        else {
            //MY_PRINT("\nnothing\n");
        }
    }
    //MY_PRINT("\n\nempty:%s\n",string);
    return empty;
}

double math_handler(OP_MATH name)
{
    double param1, param2, param3;
    double result;
    VARIANT v1, v2, v3;
    //puts("inside");
    accept_token(MATH);
    /*SQRT = 1,EXP,POW,POWF,LOG,LN,SIN,COS,TAN,ARCSIN,ARCCOS,ARCTAN,SINH,CONSH,TANH,NOTHING*/
    accept_token(LEFTBRACKET);
    switch (name)
    {
    case (SQRT):
        v1 = expr();
        result = sqrt(v1.U.d);
        break;

    case (EXP):
        v1 = expr();
        result = exp(v1.U.d);
        break;

    case (POW):
        v1 = expr();
        accept_token(COMMA);
        v2 = expr();
        result = pow(v1.U.d, (int)v2.U.d);
        break;

    case (POWF):
        v1 = expr();
        accept_token(COMMA);
        v2 = expr();
        result = powf(v1.U.d, v2.U.d);
        break;
        /*SQRT = 1,EXP,POW,POWF,LOG,LN,SIN,COS,TAN,ARCSIN,ARCCOS,ARCTAN,SINH,CONSH,TANH,NOTHING*/
    case (LOG):
        v1 = expr();
        accept_token(COMMA);
        v2 = expr();
        result = log(v1.U.d, v2.U.d);
        break;

    case (LN):
        v1 = expr();
        result = ln(v1.U.d);
        break;

    case (SIN):
        v1 = expr();
        result = sin(v1.U.d);
        break;

    case (COS):
        v1 = expr();
        result = cos(v1.U.d);
        break;

    case (TAN):
        v1 = expr();
        result = tan(v1.U.d);
        break;

    case (ARCTAN):
        v1 = expr();
        result = arctan(v1.U.d);
        break;

    case (ARCSIN):
        v1 = expr();
        result = arcsin(v1.U.d);
        break;

    case (ARCCOS):
        v1 = expr();
        result = arccos(v1.U.d);
        break;

    case (SINH):
        v1 = expr();
        result = sinh(v1.U.d);
        break;

    case (COSH):
        v1 = expr();
        result = cosh(v1.U.d);
        break;

    case (TANH):
        v1 = expr();
        result = tanh(v1.U.d);
        break;

    }
    accept_token(RIGHTBRACKET);
    return result;
}
