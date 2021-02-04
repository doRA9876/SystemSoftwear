/****************************** 

オイラー法 
f(x, y)：微分方程式
今回は微分方程式 f(x, y) = x * y　を解く。

********************************/


#include <stdio.h>
#include<math.h>

double func(double x, double y);

int main(void)
{
  double N;
	double a, b, eta, h;
	double x, y;

  for(int i = 0; i < 11; i++){
    N = exp2((double)i);

    a = 0;
    b = 1;
    eta = 1;

    h = (b - a) / N;

    x = a;
    y = eta;

    while(x < b){
      y = y + h * func(x, y);
      x += h;
    }

    printf("N=%d,  y=%f,  %f\n", (int)N, y, exp((double)1 / 2) - y);
  }

	return 0;
}


double func(double x, double y){
  return x*y;
}