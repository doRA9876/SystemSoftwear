//台形法

#include<stdio.h>
#include<math.h>
#define K 5

double array[11] = {
	4.000, 3.960, 3.846, 3.670, 3.448, 3.200,
	2.941, 2.685, 2.439, 2.210, 2.000
};

/*
double func(double x);

int main(void){
	double n = pow((double)2, K);
	double a = 0;
	double b = 1;
	double s = 0, x = 0;
	double h = (b - a) / n;
	int i = 0;

	for (i = 1; i < n; i++)
	{
		x = a + h*i;
		s += 2 * func(x);
	}
	s = 0.5*(s + func(a) + func(b))*h;

	printf("%lf\n", s);
}

double func(double x){
	return exp(x)*x*x*x;
}
*/

int main(void){
	double sam = 0;
	double height = 0.1;

	for (int i = 0; i < 10; i++){
		sam += (array[i]+array[i+1])*height/2;
	}

	printf("%f\n", sam);
}