/**
 * fib.c
 *
 * Fibonacci Test
 *
 * This test is the naive implementation of the Fibonacci function. This
 * generally tests a good subset of the instructions. This test also
 * demonstrates how to write C test files.
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
 * When writing C tests, you have to be careful and make sure you only use code
 * that will generate supported instructions. The code you write cannot use
 * floating point values. Also, note that none of the C standard library or any
 * external functions are available to use; you will only be able to call
 * functions that appear in this file.
 **/

int fib(int i)
{
    if (i == 0) {
        return 1;
    } else if (i == 1) {
        return 1;
    } else {
        return fib(i - 1) + fib(i - 2);
    }
}

int main()
{
    // fib(10) = 89. This value will appear in x2 (sp). x3 (gp) will be 0.
    return fib(10);
}
