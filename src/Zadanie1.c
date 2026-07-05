#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double silnia(int n) { // Zwraca silnię
    double wynik = 1.0;

    for (int i = 2; i <= n; i++) {
        wynik *= i;
    }

    return wynik;
}

double potega(double x, int n) { // Zwraca silnię
    double wynik = 1.0;

    for (int i = 0; i < n; i++) {
        wynik *= x;
    }

    return wynik;
}

double aabs(double x) { // Zwraca wartość bezwzględną
    if(x >= 0.0){
        return x;
    }
    else {
        return -x;
    }
}

double reduce_angle(double x) {

    const double PI = 3.141592653589793;
    const double TWO_PI = 2.0 * PI;

    while(x > PI)
        x -= TWO_PI;

    while(x < -PI)
        x += TWO_PI;

    return x;
}

double sin_taylor_v1(double x, int n) { 
    
    double suma = 0.0;

    for(int i = 0; i < n; i++) {
        double wyraz = potega(-1, i) * potega(x, 2*i+1) / silnia(2*i+1);
        suma += wyraz;
    }

    return suma;
}

double sin_taylor_v2(double x, int n) {
    
    double suma = 0.0;

    for(int i = n-1; i >= 0; i--) {
        double wyraz = potega(-1, i) * potega(x, 2*i+1) / silnia(2*i+1);
        suma += wyraz;
    }

    return suma;
}

double sin_taylor_v3(double x, int n) {
    
    double suma = 0.0;
    double wyraz = x;

    for(int i = 0; i < n; i++) {
        suma += wyraz;
        wyraz *= -x*x / ((2*i+2)*(2*i+3));
    }

    return suma;
}

double sin_taylor_v4(double x, int n) {
    double wyrazy[n];

    wyrazy[0] = x;

    for(int i = 1; i < n; i++) {
        wyrazy[i] = wyrazy[i-1] * (-x*x) / ((2*(i-1)+2)*(2*(i-1)+3));
    }

    double suma = 0.0;

    for(int i = n-1; i >= 0; i--) {
        suma += wyrazy[i];
    }

    return suma;
}

int main() {
    
    const int N_full = 1000000;   // pełna liczba punktów
    const int N_plot = 1000;     //  punkty do wykresu
    const int n = 30;             // liczba składników szeregu

    double avg_v1 = 0.0, avg_v2 = 0.0, avg_v3 = 0.0, avg_v4 = 0.0;
    double max_v1 = 0.0, max_v2 = 0.0, max_v3 = 0.0, max_v4 = 0.0;

    FILE *plik = fopen("wyniki.csv", "w");
    fprintf(plik,"x,v1,v2,v3,v4,roznicav12,roznicav34,roznica13,roznica24\n");
    //na razie bez tego, żeby łatwiej kopiować do excela

    const double eps = 1e-16;

    for(int i = 0; i < N_full; i++) {
        double x = -20.0 + 40.0 * i / (double)N_full;
        double xr = reduce_angle(x);
        double lib = sin(x);
        //liczenie wartości Taylorem
        double v1 = sin_taylor_v1(xr,n);
        double v2 = sin_taylor_v2(xr,n);
        double v3 = sin_taylor_v3(xr,n);
        double v4 = sin_taylor_v4(xr,n);
        //liczenie błędów
        double b1 = aabs(v1 - lib);
        double b2 = aabs(v2 - lib);
        double b3 = aabs(v3 - lib);
        double b4 = aabs(v4 - lib);

        avg_v1 += b1; if(b1 > max_v1) max_v1 = b1;
        avg_v2 += b2; if(b2 > max_v2) max_v2 = b2;
        avg_v3 += b3; if(b3 > max_v3) max_v3 = b3;
        avg_v4 += b4; if(b4 > max_v4) max_v4 = b4;

        double r12 = aabs(v1 - v2);
        double r34 = aabs(v3 - v4);
        double r13 = aabs(v1 - v3);
        double r24 = aabs(v2 - v4);

        double diff12 = b1 - b2; // diff > 0 V2 lepsze diff < 0 V1 lepsze
        double diff34 = b3 - b4; // diff > 0 V4 lepsze diff < 0 V3 lepsze
        double diff13 = b1 - b3; // diff > 0 V3 lepsze diff < 0 V1 lepsze
        double diff24 = b2 - b4; // diff > 0 V4 lepsze diff < 0 V2 lepsze

        // zapisujemy do CSV tylko co N_full/N_plot punkt
        if(i % (N_full / N_plot) == 0) {
            fprintf(plik,"%.8f,%.20f,%.20f,%.20f,%.20f,%.20f,%.20f,%.20f,%.20f\n",
                    x,
                    log10(b1+eps),
                    log10(b2+eps),
                    log10(b3+eps),
                    log10(b4+eps),
                    diff12+eps,
                    diff34+eps,
                    diff13+eps,
                    diff24+eps);
        }
    }

    fclose(plik);

    avg_v1 /= N_full;
    avg_v2 /= N_full;
    avg_v3 /= N_full;
    avg_v4 /= N_full;

    printf("Liczba argumentow: %d\n\n", N_full);

    printf("Sredni blad:\n");
    printf("V1: %e\n", avg_v1);
    printf("V2: %e\n", avg_v2);
    printf("V3: %e\n", avg_v3);
    printf("V4: %e\n\n", avg_v4);

    printf("Maksymalny blad:\n");
    printf("V1: %e\n", max_v1);
    printf("V2: %e\n", max_v2);
    printf("V3: %e\n", max_v3);
    printf("V4: %e\n\n", max_v4);

    printf("Dane zapisano do pliku wyniki.csv (tylko %d punktow do wykresu)\n", N_plot);

    return 0;
}