/**************************
 * ニュートン法
 * 
 * 線形および非線形方程式 f(x)=0 を満たすxを求める手法の一つ(求根アルゴリズム)
 * 
 * f(x)=0 になるようなxを探すとき、ある値x1におけるf(x)の接線とx軸との交点x2は
 * x1よりも根xに近づく。
 * これを繰り返すことで、根xを求める。
 **************************/

#include <iostream>
using namespace std;

#define EPS 0.0001

// f(x) = x^3+x-1
double func(double x) {
  return (x * x * x + x - 1);
}

// df(x)/dx = 3x^2+1 
double dfunc(double x) {
  return (3 * x * x + 1);
}

int main(void) {
  double x1;

  cout << "x1:";
  cin >> x1;

  int count = 0;
  while (true)
  {
    ++count;
    double fx1 = func(x1);
    double dx1 = dfunc(x1);
    double x2 = x1 - fx1 / dx1;
    double dx = abs(x1 - x2);

    if(dx < EPS)
      break;
    else
      x1 = x2;
  }

  cout << "EPS:" << EPS << endl;
  cout << "x1:" << x1 << endl;
  cout << "Count:" << count << endl;

  return 0;
}
