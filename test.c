#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "tensor.h"

void print_data(tensor_fp32 *t);
bool check_equals(float* a, float* b, int size);

/*
 * Test functions
 */
bool maxpool2d_stride1_padding0();
bool maxpool2d_stride2_padding0();
bool maxpool2d_stride1_padding1();

bool linear_layer_test();

int main(){
    srand(42);
    printf("Running tests...\n");
    printf("maxpool2d_stride1_padding0: %s\n", maxpool2d_stride1_padding0() ? "PASSED" : "FAILED");
    printf("maxpool2d_stride2_padding0: %s\n", maxpool2d_stride2_padding0() ? "PASSED" : "FAILED");
    printf("maxpool2d_stride1_padding1: %s\n", maxpool2d_stride1_padding1() ? "PASSED" : "FAILED");
    printf("linear_layer_test: %s\n", linear_layer_test() ? "PASSED" : "FAILED");
    printf("Done.\n");
}

/**
 * Linear layer tests
 */

bool linear_layer_test(){
    int input_shape[2] = {1, 4};
    float input_data[4] = {1,2,3,4};
    tensor_fp32 *input = init_with_data(2, input_shape, input_data);

    int weight_shape[2] = {2, 4};
    float weight_data[8] = {1,2,3,4,5,6,7,8};
    tensor_fp32 *weight = init_with_data(2, weight_shape, weight_data);

    tensor_fp32 *out = op_fp32linear(input, weight, NULL);
    printf("linear out\n");
    print_data(out);

    float expected[2] = {30, 70};
    bool passed = true;

    if (!check_equals(out->data, expected, 2)){
        passed = false;
    }

    free_tensor(input);
    free_tensor(weight);
    free_tensor(out);

    return passed;
}


/*
 * Maxpool2d tests
 */
bool maxpool2d_stride1_padding0(){
    int image_shape[4] = {1, 1, 4, 4};
    float image_data[16] = { 
        1,2,3,4,
        2,3,4,1,
        3,4,2,1,
        4,1,2,3
    };
    float expected_stride1[9] = { 
        3,4,4,
        4,4,4,
        4,4,3
    };

    tensor_fp32 *a = init_with_data(4, image_shape, image_data);
    tensor_fp32* out = op_fp32maxpool2d(a, 2,2, 1, 0);

    bool passed = true;

    if (!check_equals(out->data, expected_stride1, 9)){
        passed = false;
    }


    free_tensor(a);
    free_tensor(out);

    return passed;
}

bool maxpool2d_stride2_padding0(){
    int image_shape[4] = {1, 1, 4, 4};
    float image_data[16] = { 
        1,2,3,4,
        2,3,4,1,
        3,4,2,1,
        4,1,2,3
    };

    float expected_stride2[4] = { 
        3,4,
        4,3
    };

    bool passed = true;

    tensor_fp32 *a = init_with_data(4, image_shape, image_data);
    tensor_fp32* out = op_fp32maxpool2d(a, 2,2, 2, 0);

    if (!check_equals(out->data, expected_stride2, 4)){
        passed = false;
    }
    
    free_tensor(a);
    free_tensor(out);

    return passed;
}

bool maxpool2d_stride1_padding1(){
    int image_shape[4] = {1, 1, 4, 4};
    float image_data[16] = { 
        1,2,3,4,
        2,3,4,1,
        3,4,2,1,
        4,1,2,3
    };

    float expected_stride1[25] = { 
        1,2,3,4,4,
        2,3,4,4,4,
        3,4,4,4,1,
        4,4,4,3,3,
        4,4,2,3,3
    };

    bool passed = true;

    tensor_fp32 *a = init_with_data(4, image_shape, image_data);
    tensor_fp32* out = op_fp32maxpool2d(a, 2,2, 1, 1);

    if (!check_equals(out->data, expected_stride1, 16)){
        passed = false;
    }
    
    free_tensor(a);
    free_tensor(out);

    return passed;
}


bool check_equals(float* a, float* b, int size){
    for(int i=0; i<size; i++){
        if(a[i] != b[i]){
            return false;
        }
    }
    return true;
}


void print_data(tensor_fp32 *t){
    int size = 1;
    for(int i=0; i<t->ndims; i++){
    size *= t->dims[i];
    }
    for(int i=0; i<size; i++){
    printf("%f ", t->data[i]);
    }
    printf("\n");
}
