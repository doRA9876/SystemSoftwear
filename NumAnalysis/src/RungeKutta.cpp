/****************************

ルンゲ・クッタ法
f(x, y)は微分方程式

******************************/

#include<stdio.h>
#include<math.h>

double func(double x, double y);

int main(void){
  double a, b, x, y, y0, h;
  double k1, k2, k3, k4;
  double N;

  for(int i = 0; i < 11; i++){
    N = exp2((double)i);

    y0 = 1;
    a = 1;
    b = 2;

    h = (b - a) / N;

    x = a;
    y = y0;

    while(x < b){
      k1 = func(x, y);
      k2 = func(x + h / 2.0, y + k1 * h / 2.0);
      k3 = func(x + h / 2.0, y + k2 * h / 2.0);
      k4 = func(x + h, y + k3 * h);

      y = y + h * (k1 + 2.0*k2 + 2.0*k3 + k4) / 6;
      x += h;
    }

    printf("N=%d,  y=%f,  %f,  %f\n", (int)N, y, 3.375 - y, (double)(3.375-y)/y);
  }

  return(0);
}

double func(double x, double y){
  return (3 * y / (1 + x));
}