#include <stdio.h>
#include <stdint.h>
#include <riscv_vector.h>

#define VECTOR_SIZE 4

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
        printf("%.2f ", vec[i]);
    }
    printf("\n");
}

void example_opivv() {
    int32_t vec_a[VECTOR_SIZE] = {1, 2, 3, 4};
    int32_t vec_b[VECTOR_SIZE] = {5, 6, 7, 8};
    int32_t vec_result[VECTOR_SIZE];

    // Vector Addition (vadd.vv)
    asm volatile (
        "vsetvli t0, %0, e32\n\t"
        "vle32.v v1, (%1)\n\t"
        "vle32.v v2, (%2)\n\t"
        "vadd.vv v3, v1, v2\n\t"
        "vse32.v v3, (%3)\n\t"
        :
        : "r" (VECTOR_SIZE), "r" (vec_a), "r" (vec_b), "r" (vec_result)
        : "t0", "v1", "v2", "v3"
    );
    print_vector_int("vec_a       ", vec_a, VECTOR_SIZE);
    print_vector_int("vec_b       ", vec_b, VECTOR_SIZE);
    print_vector_int("OPIVV result", vec_result, VECTOR_SIZE);
}

void example_opfvv() {
    float vec_a[VECTOR_SIZE] = {1.1, 2.2, 3.3, 4.4};
    float vec_b[VECTOR_SIZE] = {1.0, 2.0, 3.0, 4.0};
    float vec_result[VECTOR_SIZE];

    // Floating-point addition (vfadd.vv)
    asm volatile (
        "vsetvli t0, %0, e32\n\t"
        "vle32.v v1, (%1)\n\t"
        "vle32.v v2, (%2)\n\t"
        "vfadd.vv v3, v1, v2\n\t"
        "vse32.v v3, (%3)\n\t"
        :
        : "r" (VECTOR_SIZE), "r" (vec_a), "r" (vec_b), "r" (vec_result)
        : "t0", "v1", "v2", "v3"
    );
    print_vector_float("vec_a       ", vec_a, VECTOR_SIZE);
    print_vector_float("vec_b       ", vec_b, VECTOR_SIZE);
    print_vector_float("OPFVV result", vec_result, VECTOR_SIZE);
}

void example_opmvv() {
    int32_t vec_a[VECTOR_SIZE] = {1, 2, 3, 4};
    int32_t vec_result[VECTOR_SIZE];

    // Move vector (vmv.v.v)
    asm volatile (
        "vsetvli t0, %0, e32\n\t"
        "vle32.v v1, (%1)\n\t"
        "vmv.v.v v2, v1\n\t"
        "vse32.v v2, (%2)\n\t"
        :
        : "r" (VECTOR_SIZE), "r" (vec_a), "r" (vec_result)
        : "t0", "v1", "v2"
    );

    print_vector_int("OPMVV result", vec_result, VECTOR_SIZE);
}

void example_opivi() {
    int32_t vec_a[VECTOR_SIZE] = {1, 2, 3, 4};
    int32_t vec_result[VECTOR_SIZE];

    // Vector addition with immediate (vadd.vi)
    asm volatile (
        "vsetvli t0, %0, e32\n\t"
        "vle32.v v1, (%1)\n\t"
        "vadd.vi v2, v1, 10\n\t" // Immediate value directly specified
        "vse32.v v2, (%2)\n\t"
        :
        : "r" (VECTOR_SIZE), "r" (vec_a), "r" (vec_result)
        : "t0", "v1", "v2"
    );
    print_vector_int("vec_a       ", vec_a, VECTOR_SIZE);
    printf("imm         : 10 \n");
    print_vector_int("OPIVI result", vec_result, VECTOR_SIZE);
}

void example_opivx() {
    int32_t vec_a[VECTOR_SIZE] = {1, 2, 3, 4};
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
    float vec_a[VECTOR_SIZE] = {1.1, 2.2, 3.3, 4.4};
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

void example_opmvx() {
    int32_t vec_result[VECTOR_SIZE];
    int32_t scalar = 5;

    // Move scalar to vector (vmv.v.x)
    asm volatile (
        "vsetvli t0, %0, e32\n\t"
        "vmv.v.x v1, %1\n\t"
        "vse32.v v1, (%2)\n\t"
        :
        : "r" (VECTOR_SIZE), "r" (scalar), "r" (vec_result)
        : "t0", "v1"
    );
    printf("Scalar      : %d \n", scalar);
    print_vector_int("OPMVX result", vec_result, VECTOR_SIZE);
}

int main() {
    printf("OPIVV Example : vadd.vv\n");
    example_opivv();

    printf("\nOPFVV Example : vfadd.vv\n");
    example_opfvv();

    printf("\nOPIVI Example : vadd.vi\n");
    example_opivi();

    printf("\nOPIVX Example : vadd.vx\n");
    example_opivx();

    printf("\nOPFVF Example : vfadd.vf\n");
    example_opfvf();

    return 0;
}

