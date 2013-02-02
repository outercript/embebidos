#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

typedef union{
    float f;
    uint32_t i;
    struct {
        unsigned int Mantissa: 23;
        unsigned int Exponent: 8;
        unsigned int Sign: 1;
    } My;
}Float32;

void print_headers(){
printf(
    "// Copyright 2005, Google Inc.                                             \n"
    "// All rights reserved.                                                    \n"
    "//                                                                         \n"
    "// Redistribution and use in source and binary forms, with or without      \n"
    "// modification, are permitted provided that the following conditions are  \n"
    "// met:                                                                    \n"
    "//                                                                         \n"
    "//     * Redistributions of source code must retain the above copyright    \n"
    "// notice, this list of conditions and the following disclaimer.           \n"
    "//     * Redistributions in binary form must reproduce the above           \n"
    "// copyright notice, this list of conditions and the following disclaimer  \n"
    "// in the documentation and/or other materials provided with the           \n"
    "// distribution.                                                           \n"
    "//     * Neither the name of Google Inc. nor the names of its              \n"
    "// contributors may be used to endorse or promote products derived from    \n"
    "// this software without specific prior written permission.                \n"
    "//                                                                         \n"
    "// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     \n"
    "// \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     \n"
    "// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   \n"
    "// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT    \n"
    "// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   \n"
    "// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        \n"
    "// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   \n"
    "// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   \n"
    "// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     \n"
    "// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   \n"
    "// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    \n"
    "                                                                           \n"
    "#include <gtest/gtest.h>                                                   \n"
    "#include \"../main.h\"                                                 \n\n\n");
}

void print_macros(){
printf(
    "#define FLOAT32_MULTIPLY(TEST_NAME, x, y, z)               \\\n"
    "    TEST(Multiplication, TEST_NAME){                       \\\n"
    "        EXPECT_FALSE(float32_multiply_check(x, y, z));     \\\n"
    "    }\n");

printf(
    "#define FLOAT32_ADDITION(TEST_NAME, x, y, z)                \\\n"
    "    TEST(ADDITION, TEST_NAME){                              \\\n"
    "        Float32 a, b, c;                                    \\\n"
    "        a.fword = x;                                        \\\n"
    "        b.fword = y;                                        \\\n"
    "        c = float32_addition_substraction(a, b, 0);         \\\n"
    "        ASSERT_FLOAT_EQ(z, c.fword);                        \\\n"
    "    }                                                       \n\n");

printf(
    "#define FLOAT32_SUBSTRACTION(TEST_NAME, x, y, z)            \\\n"
    "    TEST(SUBSTRACTION, TEST_NAME){                          \\\n"
    "        Float32 a, b, c;                                    \\\n"
    "        a.fword = x;                                        \\\n"
    "        b.fword = y;                                        \\\n"
    "        c = float32_addition_substraction(a, b, 1);         \\\n"
    "        ASSERT_FLOAT_EQ(z, c.fword);                        \\\n"
    "    }                                                       \n\n");

printf(
    "#define FLOAT32_DIVIDE(TEST_NAME, x, y, z)                  \\\n"
    "    TEST(Division, TEST_NAME){                              \\\n"
    "        EXPECT_FALSE(float32_divide_check(x, y, z));        \\\n"
    "    }                                                       \n\n");
}


uint8_t float32_is_special(Float32 a){
    if(a.My.Exponent == 0 || a.My.Exponent == 0xFF)
        return 1;
    else
        return 0;
}

uint8_t float32_validate_values(Float32 a, Float32 b, Float32 c){
    return float32_is_special(a) || float32_is_special(b) || float32_is_special(c);
}

float float32_rand(float min, float max){
    float random = ((float) rand()) / (float) RAND_MAX;
    return min + (random * (max - min));
}

void float32_multiply(float min, float max, char *testcase, uint16_t limit){
    uint16_t index;
    Float32 A, B, result;

    printf("\n\n// %s Test Cases\n", testcase);
    for(index = 0; index < limit; index++){
        do{
            A.f = float32_rand(min, max);
            B.f = float32_rand(min, max);
            result.f = A.f * B.f;
        }while(float32_validate_values(A,B,result));

        printf("FLOAT32_MULTIPLY(TC_%s_%03d, "
               "%.8f, %.8f, %.8f);\n",
               testcase, index, A.f, B.f, result.f);
    }
}

void float32_addition(float min, float max, char *testcase, uint16_t limit){
    uint16_t index;
    Float32 A, B, result;

    printf("\n\n// %s Test Cases\n", testcase);
    for(index = 0; index < limit; index++){
        A.f = float32_rand(min, max);
        B.f = float32_rand(min, max);
        result.f = A.f + B.f;

        printf("FLOAT32_ADDITION(TC_%s_%03d, "
               "%.8f, %.8f, %.8f);\n",
               testcase, index, A.f, B.f, result.f);
    }
}

void float32_substraction(float min, float max, char *testcase, uint16_t limit){
    uint16_t index;
    Float32 A, B, result;

    printf("\n\n// %s Test Cases\n", testcase);
    for(index = 0; index < limit; index++){
        A.f = float32_rand(min, max);
        B.f = float32_rand(min, max);
        result.f = A.f - B.f;

        printf("FLOAT32_SUBSTRACTION(TC_%s_%03d, "
               "%.8f, %.8f, %.8f);\n",
               testcase, index, A.f, B.f, result.f);
    }
}


void float32_divide(float min, float max, char *testcase, uint16_t limit){
    uint16_t index;
    Float32 A, B, result;

    printf("\n\n// %s Test Cases\n", testcase);
    for(index = 0; index < limit; index++){

        do{
            A.f = float32_rand(min, max);
            B.f = float32_rand(min, max);
            result.f = A.f / B.f;
        }while(float32_validate_values(A,B,result));

        printf("FLOAT32_DIVIDE(TC_%s_%03d, "
               "%.8f, %.8f, %.8f);\n",
               testcase, index, A.f, B.f, result.f);
    }
}


int main() {

    // Init the random seed
    srand(time(NULL));

    // Print headers and macros
    print_headers();
    print_macros();

    //float32_addition(-1500,1500, "Addition", 2500);
    //float32_substraction(-1000,1000, "substraction", 1000);

    //float32_multiply(1,100, "PositiveNumbers_Small", 20);
    //float32_multiply(-100,100, "NegativeNumbers_Small", 20);
    //float32_multiply(500,5000, "PositiveNumbers_Medium", 20);
    float32_multiply(-50000000,50000000, "NegativeNumbers_Medium", 3500);

    //float32_divide(1,100, "PositiveNumbers_Small", 2000);
    //float32_divide(-100,100, "NegativeNumbers_Small", 20);
    //float32_divide(500,5000, "PositiveNumbers_Medium", 20);
    //float32_divide(-5000,5000, "NegativeNumbers_Medium", 20);



    return 0;
}
