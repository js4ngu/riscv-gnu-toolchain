#include <stdio.h>
#include <stdint.h>
#include <riscv_vector.h>

#define VECTOR_SIZE 4

void print_vector(const char* name, int32_t *vec, size_t size) {
    printf("%s: ", name);
    for (size_t i = 0; i < size; i++) {
        printf("%d ", vec[i]);
    }
    printf("\n");
}

void print_mask(const char* name, uint8_t *mask, size_t size) {
    printf("%s: ", name);
    for (size_t i = 0; i < size; i++) {
        printf("%d ", mask[i]);
    }
    printf("\n");
}

int main() {
    int32_t vec_a[VECTOR_SIZE] = {6, 7, 8, 9};
    int32_t vec_b[VECTOR_SIZE] = {15, 16, 17, 18};
    int32_t scalar = 3;
    int32_t vec_result[VECTOR_SIZE] = {0, 0, 0, 0};

    print_vector("vec A", vec_a, VECTOR_SIZE);
    print_vector("vec B", vec_b, VECTOR_SIZE);
    printf("Scalar %d\n", scalar);

    // Vector Addition (vadd.vv)
    asm volatile (
        "vsetvli t0, %0, e32\n\t"
        "vle32.v v1, (%1)\n\t"
        "vle32.v v2, (%2)\n\t"
        "vmod.vv v3, v1, v2\n\t"
        "vse32.v v3, (%3)\n\t"
        :
        : "r" (VECTOR_SIZE), "r" (vec_a), "r" (vec_b), "r" (vec_result)
        : "t0", "v1", "v2", "v3"
    );
    print_vector("Vector mod (vmod.vv)", vec_result, VECTOR_SIZE);

    // Vector Addition with Immediate (vadd.vi)
    asm volatile (
        "vsetvli t0, %0, e32\n\t"
        "vle32.v v1, (%1)\n\t"
        "vmod.vi v3, v1, 3\n\t" // Immediate value 10
        "vse32.v v3, (%2)\n\t"
        :
        : "r" (VECTOR_SIZE), "r" (vec_b), "r" (vec_result)
        : "t0", "v1", "v3"
    );
    print_vector("Vector mod with Immediate(3) (vmod.vi)", vec_result, VECTOR_SIZE);

    // Vector Addition with Scalar (vadd.vx)
    asm volatile (
        "vsetvli t0, %0, e32\n\t"
        "vle32.v v1, (%1)\n\t"
        "vmod.vx v3, v1, %2\n\t" // Scalar value
        "vse32.v v3, (%3)\n\t"
        :
        : "r" (VECTOR_SIZE), "r" (vec_b), "r" (scalar), "r" (vec_result)
        : "t0", "v1", "v3"
    );
    print_vector("Vector mod with Scalar (vmod.vx)", vec_result, VECTOR_SIZE);
    
    return 0;
}
