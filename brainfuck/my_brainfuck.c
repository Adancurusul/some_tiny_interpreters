#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
>  指针加一 
<  指针减一 
+  指针指向字节加一
- 。。。减一
. 输出指针单元的ascii
，输出内容到指针指向单元
[ 该单元为0 后跳到]

] 不为零 前跳




*/

#define TOKEN "><+-.,[]"
#define DATA_SIZE 30000
#define STACK_SIZE 1000
#define CODE_SIZE 30000

typedef void (*callback)(void);

struct main_struct
{
    /* data */
    char code_part[CODE_SIZE];
    long instruction_pointer;
    char satck_part[STACK_SIZE];
    long stack_pointer;
    char data_part[DATA_SIZE];
    long base_pointer;
    callback function[128];
} core_data;

void forward() //>
{
    core_data.base_pointer = (core_data.base_pointer + 1) % DATA_SIZE;
}

void backward() //<
{
    core_data.base_pointer = (core_data.base_pointer - 1) % DATA_SIZE;
}

void increase()
{
    core_data.data_part[core_data.base_pointer]++;
    //printf("in");
}

void decrease()
{
    core_data.data_part[core_data.base_pointer]--;
}

void b_input()
{
    core_data.data_part[core_data.base_pointer] = getchar();
}

void b_output()
{
    putchar(core_data.data_part[core_data.base_pointer]);
}

void while_start()
{
    //printf("c");
    if (core_data.data_part[core_data.base_pointer])
    {
        core_data.satck_part[core_data.stack_pointer] = core_data.instruction_pointer - 1;
        core_data.stack_pointer++;
        //printf("a");
    }
    else
    {
        int v = 1;
        for (core_data.instruction_pointer++; core_data.code_part[core_data.instruction_pointer] && v; core_data.instruction_pointer++)
        {
            if (core_data.code_part[core_data.instruction_pointer] == '[')
            {
                v++;
            }
            else if (core_data.code_part[core_data.instruction_pointer] == ']')
            {
                v--;
            }
        }
        //printf("b");
    }
}
void change_line()
{
    printf("line");
}
void while_end()
{
    if (core_data.data_part[core_data.base_pointer])
    {
        core_data.stack_pointer--;
        core_data.instruction_pointer = core_data.satck_part[core_data.stack_pointer];
    }
}

void init_all()
{
    int v;
    int i;
    memset(&core_data, 0, sizeof(core_data));
    core_data.function[62] = forward;     //>
    core_data.function[60] = backward;    //<
    core_data.function[43] = increase;    //+
    core_data.function[45] = decrease;    //-
    core_data.function[46] = b_output;    //.
    core_data.function[44] = b_input;     //,
    core_data.function[91] = while_start; //[
    core_data.function[93] = while_end;   //]
    core_data.function[10] = change_line; //LF
    for (i = 0; (v = getchar()) != EOF;)
    {
        //printf("%d",v);
        if (strchr(TOKEN, v))
        {
            //printf("%d",v);
            core_data.code_part[i] = v;
            i++;
        }

        //printf("%d",core_data.code_part);
    }
    core_data.code_part[i + 1] = -8;
    //printf(core_data.code_part);
    //printf("%d",i);
}
void do_explaining()
{
    while (core_data.code_part[core_data.instruction_pointer] != -8)
    {
        core_data.function[core_data.code_part[core_data.instruction_pointer]]();
        core_data.instruction_pointer += 1;
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        freopen(argv[1], "r", stdin);
    }

    init_all();
    do_explaining();

    // printf("finish");
    return 0;
}
