#include "tiny_basic_interpreter.h"
#include <stdio.h>



static  char program[] =
"30 ia = 119.6+20\n\
40 print ia-20\n\
45 t = 200\n\
50 print t \n\
60 print ia*2/3-200\n\
100 end";
/*
"3 GOSUB 100\n\
20 for i = 1 to 6\n\
30 print i\n\
40 next i\n\
45 print \"goto end ok\"\n\
60 end\n\
100 print \"gosub ok\"\n\
102 t = 10\n\
103 n= t + 9 * 2\n\
104 print n\n\
110 return\n";*/



int main(void)
{
  interpreter_init(program);

  do {
    do_interpretation();
  } while(!interpreter_finished());
while(1){}
  return 0;
}

/*
#define MAXLENGTH 1000
int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        char dest[MAXLENGTH];

        FILE *file;
        int pos, temp, i;
        file = fopen(argv[1], "r");

        if (NULL == file)
        {
            fprintf(stderr, "open %s error\n", argv[1]);
            return -1;
        }

        pos = 0;
        
        for (i = 0; i < MAXLENGTH - 1; i++)
        {
            temp = fgetc(file);
            if (EOF == temp)
                break;
            dest[pos++] = temp;
        }
        //关闭文件
        fclose(file);
        //在数组末尾加0
        dest[pos] = 0;
        interpreter_init(dest);

        do
        {
            do_interpretation();
        } while (!interpreter_finished());
        while (1)
        {
        }
    }
}
*/

/*



float sqrt(float x);
double eee(double x);
double exp(double x);
double pow(double a,int n);
double powf(double a,double x);
double log(double a,double N);
double ln(double x);
double sin(double x);
double cos(double x);
double tan(double x);
double arcsin(double x);
double arccos(double x);
double arctan(double x);
double sinh(double x);
double cosh(double x);
double tanh(double x);

*/