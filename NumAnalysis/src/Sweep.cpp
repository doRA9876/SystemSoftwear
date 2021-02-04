/**************************
 * 掃出し法
 * 
 * 連立方程式の解法。 
 * 
 * 対角成分が１の行列(単位行列)になるように行基本変形を用いて変形を行う。
 **************************/

#include<iostream>
using namespace std;
#define N 3

double a[N][N + 1] = {
  {0, 2, 1, 10}, 
  {1, 5, 2, 21}, 
  {1, 2, 5, 30}
};

void swap(int n);

int main() {
  double pivot, a_ik;

  for (int i = 0; i < N; i++) {
    do {
      pivot = a[i][i];
      if (pivot == 0)
        swap(i);
    } while (pivot == 0);

    for (int j = i; j < N + 1; j++) {
      a[i][j] = a[i][j] / pivot;
    }

    for (int k = 0; k < N; k++) {
      if ((k - i) != 0) {
        a_ik = a[k][i];

        for (int j = i; j < N + 1; j++) {
          a[k][j] = a[k][j] - a_ik * a[i][j];
        }
      }
    }
  }

  for (int i = 0; i < N; i++) {
    cout << i + 1 << " : " << a[i][N] << endl;
  }

  return 0;
}

void swap(int n) {
  for (int j = n; j < N - 1; j++) {
    for (int i = 0; i < N + 1; i++) {
      double tmp = a[j][i];
      a[j][i] = a[j + 1][i];
      a[j + 1][i] = tmp;
    }
  }
}