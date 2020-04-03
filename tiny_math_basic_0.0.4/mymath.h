#define pi 3.14159265358979323846
#define e  2.7182818284590452354
#define ln_2 0.69314718055994530942
#define ln_10 2.30258509299404568402
#define fabs(a) ((a)>0?(a):(-(a)))


double sqrt(double x);
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





//实数的整数次方
double pow(double a,int n)
{
    if(n<0) return 1/pow(a,-n);
    double res = 1.0;
    while(n)
    {
        if(n&1) res *= a;
        a *= a;
        n >>= 1;
    }
    return res;
}


//较高精度的e**x
//return 1 + x + x*x/2 + pow(x,3)/6 + pow(x,4)/24 + pow(x,5)/120;
//单次精度不够因为该展开式是在x=0的，越靠近0就越精确。而exp(x)=exp**2*(x/2),故我们可以递归地进行这样的代换，直到x足够小。vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
double eee(double x)//e指数的小数
{
    if(x>1e-3)
    {
        double ee = eee(x/2);
        return ee*ee;
    }
    return 1 + x + x*x/2 + pow(x,3)/6 + pow(x,4)/24 + pow(x,5)/120;
}
double exp(double x){
    if(x<0) return 1/exp(-x);
    int n = (int)x;
    x -= n;
    double e1 = pow(e,n);
    double e2 = eee(x);
    return e1*e2;
}


double sqrt(double x)
{
    if(x>100) return 10.0*sqrt(x/100);
    double t = x/8 + 0.5 + 2*x/(4+x);
    int c = 10;
    while(c--)
    {
        t = (t+x/t)/2;
    }
    return t;
}

//辛普森公式的ln
double F2(double x)
{
    return 1/sqrt(1-x*x);
}
double F1(double x)
{
    return 1/x;
}

double simpson(double a, double b,int flag)
{
    double c = a + (b-a)/2;
    if(flag==1)
        return (F1(a)+4*F1(c)+F1(b))*(b-a)/6;
    if(flag==2)
        return (F2(a)+4*F2(c)+F2(b))*(b-a)/6;
}

double asr(double a, double b, double eps, double A,int flag)
{
    double c = a + (b-a)/2;
    double L = simpson(a, c,flag), R = simpson(c, b,flag);
    if(fabs(L+R-A) <= 15*eps) return L+R+(L+R-A)/15.0;
    return asr(a, c, eps/2, L,flag) + asr(c, b, eps/2, R,flag);
}

double asr4(double a, double b, double eps,int flag)
{
    return asr(a, b, eps, simpson(a, b,flag),flag);
}

double ln(double x)
{
    return asr4(1,x,1e-8,1);
}

//log
double log(double a,double N)
{
    return ln(N)/ln(a);
}

//a**x = exp(xlna)实数的实数次方
double powf(double a,double x)
{
    return exp(x*ln(a));
}


//
/*三角函数*/
//利用周期性，奇偶性，诱导公式把变成（0，4/】
//

double sin(double x)
{
    double fl = 1;
    if(x>2*pi || x<-2*pi) x -= (int)(x/(2*pi))*2*pi;
    if(x>pi) x -= 2*pi;
    if(x<-pi) x += 2*pi;
    if(x>pi/2)
    {
        x -= pi;
        fl *= -1;
    }
    if(x<-pi/2)
    {
        x += pi;
        fl *= -1;
    }
    if(x>pi/4) return cos(pi/2-x);
    else return fl*(x - pow(x,3)/6 + pow(x,5)/120 - pow(x,7)/5040 +pow(x,9)/362880);
}

double cos(double x)
{
    double fl = 1;
    if(x>2*pi || x<-2*pi) x -= (int)(x/(2*pi))*2*pi;
    if(x>pi) x -= 2*pi;
    if(x<-pi) x += 2*pi;
    if(x>pi/2)
    {
        x -= pi;
        fl *= -1;
    }
    if(x<-pi/2)
    {
        x += pi;
        fl *= -1;
    }
    if(x>pi/4) return sin(pi/2-x);
    else return fl*(1 - pow(x,2)/2 + pow(x,4)/24 - pow(x,6)/720 + pow(x,8)/40320);
}
double tan(double x)
{
    return sin(x)/cos(x);
}


//arcsin


double arcsin(double x)
{
    if(fabs(x)>1) return -1;
    double fl = 1.0;
    if(x<0) {fl*=-1;x*=-1;}
    if(fabs(x-1)<1e-7) return pi/2;
    return (fl*asr4(0,x,1e-8,2));
    //return x + pow(x,3)/6 + pow(x,5)*3/40 +pow(x,7)*5/112 + pow(x,9)*35/1152 + pow(x,11)*315/1408;
}

double arccos(double x)
{
    if(fabs(x)>1) return -1;
    return pi/2 - arcsin(x);
}

//某大神写的，不大懂，应该是迭代到泰勒级数的范围
double arctan(double x)
{
    if(x<0) return -arctan(-x);
    if(x>1) return pi/2 - arctan(1/x);
    if(x>1e-3) return 2*arctan((sqrt(1+x*x)-1)/x);
    return x - pow(x,3)/3 + pow(x,5)/5 - pow(x,7)/7 + pow(x,9)/9;
}
//双曲
double sinh(double x)
{
    return (exp(x)-exp(-x))/2;
}

double cosh(double x)
{
    return (exp(x) + exp(-x))/2;
}

double tanh(double x)
{
    double ext = exp(x);
    return 1 - 2/(ext*ext+1);
}