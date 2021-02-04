/****************************
 * はさみうち法
 *
 * 線形および非線形方程式 f(x)=0 を満たすxを求める手法の一つ(求根アルゴリズム)
 * 
 * f(x)=0 の形に直すと根は y=f(x) がx軸と交わる点となる。
 * したがって、根は f(a)*f(b) < 0
 * となる点a,bの間に存在するためa,bの間隔を狭めていけば根が出る。
 *****************************/

#include <iostream>
using namespace std;

// epsilon
#define EPS 0.0001

// f(x) = x^3+x-1
double func(double x) {
  return (x * x * x + x - 1);
}

int main() {
  double a, b, c;

  cout << "a:";
  cin >> a;

  cout << "b:";
  cin >> b;

  int count = 0;
  double before_c = 100;
  while (true) {
    ++count;
    double fa = func(a);
    double fb = func(b);
    c = a + (a - b) * fa / (fb - fa);
    double fc = func(c);

    if (abs(c - before_c) < EPS)
      break;
    else
      before_c = c;

    // faとfcの符号が一致(fa*fc>0)するならa=c,fbとfcの符号が一致(fb*fc>0)するならb=c
    if (fa * fc > 0) {
      a = c;
    } else {
      b = c;
    }
  }

  cout << "Result:" << c << endl;
  cout << "EPS:" << EPS << endl;
  cout << "Count:" << count << endl;
}
