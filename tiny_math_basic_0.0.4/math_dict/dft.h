#include <stdio.h>
#include <stdlib.h>
#include <math.h>			 
#define PI 3.1415926535897932384626433832795028841971
int N ;	//定于序列长度变量
double Input_Squence[100];	//输入的原始数据序列 
double Ampl[100] ;   	//存储幅值计算结果

typedef struct				//定义复数结构,下面通过欧拉公式运算
{
	double real,imag;   
}complex;
complex Result_Point[100];		

void DFT_Calculate_Point(int k)
{
	int n = 0;
	complex Sum_Point;
	complex One_Point[N];
	Sum_Point.real = 0;
	Sum_Point.imag = 0;
	for(n=0; n<N; n++)
	{
		One_Point[n].real = cos(2*PI/N*k*n)*Input_Squence[n];  //复数的实部
		One_Point[n].imag = -sin(2*PI/N*k*n)*Input_Squence[n]; //复数的虚部
		
		Sum_Point.real += One_Point[n].real;	//对实部求和
		Sum_Point.imag += One_Point[n].imag;	//对虚部求和		
	}
	Result_Point[k].real = Sum_Point.real;
	Result_Point[k].imag = Sum_Point.imag;
}

void DFT_Calculate()
{
	int i = 0;
	for(i=0; i<N; i++)
	{
		DFT_Calculate_Point(i);
		Ampl[i] = sqrt(Result_Point[i].real * Result_Point[i].real + Result_Point[i].imag * Result_Point[i].imag);  //计算幅值
	}
}

int main(int argc, char *argv[])
{
	N = atoi(argv[1]);  //atoi,将字符串转换为整数值。 
						//argv[ ]，用来存放指向你的字符串参数的指针
	int i = 0;
	for(i=0; i<N; i++)//产生输入序列 
	{
		Input_Squence[i] = i;
	}
	DFT_Calculate(); //进行DFT计算 
	for(i=0; i<N; i++)
	{
		printf("%d\t%lf\n",i,Ampl[i]); //输出计算结果
	}
	return 0;
}
