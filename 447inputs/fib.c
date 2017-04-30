/**
 * fib.c
 *
 * Fibonacci Test
 *
 * This test is the naive implementation of the Fibonacci function. This
 * generally tests a good subset of the instructions.
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
    return fib(10);     // fib(10) = 89. This value will appear in x2 (sp).
}
