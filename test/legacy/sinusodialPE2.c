#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void sinPE(int pos, int d_model, double* Token);

// Sinusoidal Positional Encoding 벡터를 계산하고 Token 벡터와 더하는 함수
void sinPE(int pos, int d_model, double* Token) {
    double* PE_vector = (double*)malloc(d_model * sizeof(double));
    for (int i = 0; i < d_model; i += 2) {
        PE_vector[i] = sin(pos / pow(10000, ((2.0 * i) / d_model)));
        if (i + 1 < d_model) {
            PE_vector[i + 1] = cos(pos / pow(10000, ((2.0 * i) / d_model)));
        }
    }
    for (int i = 0; i < d_model; i++) {
        Token[i] += PE_vector[i];
    }
    free(PE_vector);
}

void computeSINPE() {
    int max_len = 50;  // 시퀀스의 최대 길이
    int d_model = 130;  // 모델의 차원
    int pos = 1;    // 몇번째 토큰인지

    double* Token = (double*)calloc(max_len * d_model, sizeof(double));
    // 초기 토큰 값 설정 (예: 모든 값을 0로 설정)
    for (int i = 0; i < max_len * d_model; i++) {
        Token[i] = 0.0;
    }

    sinPE(pos, d_model, &Token[pos * d_model]);

    // 결과 출력
    for (int i = 0; i < d_model; i++) {
        printf("%f ", Token[pos * d_model + i]);
    }
    printf("\n");

    free(Token);
    return 0;
}
