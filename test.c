#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "tensor.h"

void print_data(tensor_fp32 *t);

/*
 * Test functions
 */
bool maxpool2d_test();

int main(){
    srand(42);
    int kernel_shape[3] = {1, 3,3};
    int image_shape[4] = {1, 1, 5, 5};

    tensor_fp32 *a = init_with_random(4, image_shape);

    // create a 3x3 mean kernel
    tensor_fp32 *k = init_with_zeros(3, kernel_shape);
    scalarop_inplace_fp32add(k, 1);
    scalarop_inplace_fp32mul(k, (float) 1/9);

    // float data[] = {
    //     1,1,1,1,1,1,1,1,1,
    //     2,2,2,2,2,2,2,2,2,
    //     3,3,3,3,3,3,3,3,3,
    // };
    // tensor_fp32* k = init_with_data(3, kernel_shape, data) ;

    printf("Input array:\n");
    print_2d(a);

    // printf("3x3 Kernel array:\n");
    // print_data(k);

    // tensor_fp32* out = op_fp32conv2d(a, k, 1, 1);
    tensor_fp32* out = op_fp32maxpool2d(a, 2,2, 2, 1);

    printf("Output array:\n");
    print_2d(out);
    free(a); free(k); free(out);
    return 0;
}

// TODO: Add more tests
// int main(){
//     srand(42);
//     printf("Running tests...\n");
//     if(!maxpool2d_test()){
//         printf("maxpool2d test failed\n");
//         return 1;
//     }
// }
//
// bool maxpool2d_test(){
//     int image_shape[4] = {1, 1, 5, 5};
//     tensor_fp32 *a = init_with_random(4, image_shape);
//     tensor_fp32* out = op_fp32maxpool2d(a, 2,2, 2, 1);
//     // free(a); free(out);
//     print_2d(a);
//     print_2d(out);
//     free_tensor(a);
//     free_tensor(out);
//     return true;
// }
//
// void print_data(tensor_fp32 *t){
//     int size = 1;
//     for(int i=0; i<t->ndims; i++){
//     size *= t->dims[i];
//     }
//     for(int i=0; i<size; i++){
//     printf("%f ", t->data[i]);
//     }
//     printf("\n");
// }
