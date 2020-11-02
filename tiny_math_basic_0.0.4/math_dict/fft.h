

#ifndef __FFT_H__
#define __FFT_H__

typedef struct complex //复数类型
{
    float real; //实部
    float imag; //虚部
} complex;
#include "mymath.h"
#define PI 3.1415926535897932384626433832795028841971

///
void conjugate_complex(int n, complex in[], complex out[]);
void c_plus(complex a, complex b, complex *c); //复数加
void c_mul(complex a, complex b, complex *c);  //复数乘
void c_sub(complex a, complex b, complex *c);  //复数减法
void c_div(complex a, complex b, complex *c);  //复数除法
void fft(int N, complex f[]);                  //傅立叶变换 输出也存在数组f中
void ifft(int N, complex f[]);                 // 傅里叶逆变换
void c_abs(complex f[], float out[], int n);   //复数数组取模

#endif