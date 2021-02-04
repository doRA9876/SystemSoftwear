#include<stdio.h>
#include<math.h>
#define N 3
#define EPS 0.0001

double a[N][N + 1] = {
	{ 3, 1, 1, 10 },
	{ 1, 5, 2, 21 },
	{ 1, 2, 5, 30 }
};

int main(void){

	double x[N], y[N];
	double sum, q;
	int i, j, k;

	for (i = 0; i < N; i++) {
		x[i] = 1.0;
		y[i] = x[i];
	}

	for (k = 1; k <= 30; k++){
		for (i = 0; i < N; i++){
			sum = 0;

			for (j = 0; j < N; j++){
				if (i != j){
					sum += a[i][j] * x[j];
				}
			}
			x[i] = (a[i][N] - sum) / a[i][i];
		}
		q = 0;

		for (i = 0; i < N; i++){
			q += abs(x[i] - y[i]);
			y[i] = x[i];
		}
		if (q < EPS){
			for (i = 0; i < N; i++){
				printf("x%d=%f\n", i, x[i]);
			}
			break;
		}
	}

	if (k > 30){
		printf("Žû‘©‚¹‚¸\n");
	}

	return 0;
}