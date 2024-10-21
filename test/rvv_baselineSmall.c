#include <stdio.h>
#include <stdint.h>
#include <riscv_vector.h>

#define VECTOR_SIZE 8

void print_vector_int(const char* name, int32_t *vec, size_t size) {
    printf("%s: ", name);
    for (size_t i = 0; i < size; i++) {
        printf("%d ", vec[i]);
    }
    printf("\n");
}

void print_vector_float(const char* name, float *vec, size_t size) {
    printf("%s: ", name);
    for (size_t i = 0; i < size; i++) {
        int32_t fixed_point = (int32_t)(vec[i] * 100);  // 소수점 둘째자리까지, 100을 곱함
        printf("%d.%02d ", fixed_point / 100, abs(fixed_point % 100));
    }
    printf("\n");
}

void example_opivx() {
    int32_t vec_a[VECTOR_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};
    int32_t vec_result[VECTOR_SIZE];
    int32_t scalar = 10;

    // Vector addition with scalar (vadd.vx)
    asm volatile (
        "vsetvli t0, %0, e32\n\t"
        "vle32.v v1, (%1)\n\t"
        "vadd.vx v2, v1, %2\n\t"
        "vse32.v v2, (%3)\n\t"
        :
        : "r" (VECTOR_SIZE), "r" (vec_a), "r" (scalar), "r" (vec_result)
        : "t0", "v1", "v2"
    );
    print_vector_int("vec_a       ", vec_a, VECTOR_SIZE);
    printf("Scalar      : %d \n", scalar);

    print_vector_int("OPIVX result", vec_result, VECTOR_SIZE);
}

void example_opfvf() {
    float vec_a[VECTOR_SIZE] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
    float vec_result[VECTOR_SIZE];
    float scalar = 1.0;

    // Floating-point addition with scalar (vfadd.vf)
    asm volatile (
        "vsetvli t0, %0, e32\n\t"
        "vle32.v v1, (%1)\n\t"
        "vfadd.vf v2, v1, %2\n\t"
        "vse32.v v2, (%3)\n\t"
        :
        : "r" (VECTOR_SIZE), "r" (vec_a), "f" (scalar), "r" (vec_result)
        : "t0", "v1", "v2"
    );
    print_vector_float("vec_a       ", vec_a, VECTOR_SIZE);
    printf("Scalar      : %lf \n", scalar);
    print_vector_float("OPFVF result", vec_result, VECTOR_SIZE);
}

int main() {
    printf("\nOPIVX Example : vadd.vx\n");
    example_opivx();

    printf("\nOPFVF Example : vfadd.vf\n");
    example_opfvf();

    return 0;
}

