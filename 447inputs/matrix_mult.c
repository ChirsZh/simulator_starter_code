/**
 * matrix_mult.c
 *
 * Matrix Multiplication Test
 *
 * This test contains a naive implementation of matrix multiplication. This test
 * is also used to demonstrate how to write C test files.
 **/

/**
 * For C test files, the only thing that is required is a main function, like is
 * the case for typical C applications that you would write. The build system
 * sets up the test so that the _start function in 447runtime/crt0.S is called.
 * This function simply calls main.
 *
 * When main returns, the _start function puts the lower 32-bits of the return
 * value in x2 and the upper 32-bits of the return value in x3. This done
 * because the RISC-V ABI permits doubleword return values. The function then
 * invokes ECALL to terminate simulation.
 *
 * While the processor does not support floating point or integer multiplication
 * instructions, you can still use them within C test files. This is because the
 * code is linked against GCC's library, which provides software implementations
 * of these instructions that faithfully emulate them.
 *
 * Note that none of the C standard library functions or any external functions
 * are available to use. You will only be able to call functions that appear in
 * this file.
 **/

/*----------------------------------------------------------------------------
 * Internal Definitions
 *----------------------------------------------------------------------------*/

// Macro that gets the length of a statically allocated array
#define array_len(A) (sizeof(A) / sizeof((A)[0]))

// The matrices used for matrix multiplication
static const float A[4][5] = {
    {-6.416,  0.315, -9.039, -0.686, -8.869,},
    { 6.641, -9.336, -1.902,  8.933,  9.999,},
    { 8.235, -3.692, -8.778, -0.195,  2.708,},
    { 5.810, -5.911,  8.022, -8.100, -4.310,},
};
static const float B[5][3] = {
    {-7.362, -8.212,  8.562,},
    {-1.741, -1.678,  5.200,},
    {-1.867, -6.416, -9.140,},
    { 5.467,  3.763, -2.192,},
    { 5.628, -8.104, -7.084,},
};
float C[array_len(A)][array_len(B[0])];

/*----------------------------------------------------------------------------
 * Functions
 *----------------------------------------------------------------------------*/

/* Performs matrix-matrix multiplication of A and B, storing the result in the
 * matrix C. */
static void matrix_mult(int A_rows, int A_cols, int B_cols,
        const float A[A_rows][A_cols], const float B[A_cols][B_cols],
        float C[A_rows][B_cols])
{
    for (int output_row = 0; output_row < A_rows; output_row++)
    {
        for (int output_col = 0; output_col < B_cols; output_col++)
        {
            C[output_row][output_col] = 0.0f;
            for (int input_dim = 0; input_dim < A_cols; input_dim++)
            {
                C[output_row][output_col] += A[output_row][input_dim] *
                        B[input_dim][output_col];
            }
        }
    }
}

// Sums all the elements in the given matrix together
static float matrix_add_reduce(int rows, int cols, float M[rows][cols])
{
    float sum = 0.0f;
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            sum += M[row][col];
        }
    }

    return sum;
}

// Main method for the program
int main()
{
    matrix_mult(array_len(A), array_len(A[0]), array_len(B[0]), A, B, C);

    /* Sum the output matrix and return the binary representation of the
     * floating-point sum (it is not converted to an integer). */
    float sum = matrix_add_reduce(array_len(C), array_len(C[0]), C);
    return *(int *)&sum;
}
