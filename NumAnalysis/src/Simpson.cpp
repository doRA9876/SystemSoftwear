//シンプソン法

#include<stdio.h>
#include<math.h>
#define N 32

double array[11] = {
	4.000, 3.960, 3.846, 3.670, 3.448, 3.200,
	2.941, 2.685, 2.439, 2.210, 2.000
};

/*
double func(double x);

int main(){
	int i;
	double s, s1, s2, h, x;
	double a = 0;
	double b = 1;

	h = (b - a) / (2 * N);
	s1 = 0;

	for (i = 1; i <= 2 * N - 1; i += 2){
		x = a + h*i;
		s1 += 4 * func(x);
	}
	s2 = 0;

	for (i = 2; i <= 2 * N - 2; i += 2){
		x = a + h*i;
		s2 += 2 * func(x);
	}

	s = (s1 + s2 + func(a) + func(b))*h / 3;

	printf("%lf\n", s);
}

double func(double x){
	return exp(x)*x*x*x;
}
*/

int main(void){
	double sam = 0;
	double height = 0.1;

	for (int i = 1; i < 11; i += 2){
		sam += height*(array[i - 1] + 4 * array[i] + array[i + 1]) / 3;
	}

	printf("%f\n", sam);
}