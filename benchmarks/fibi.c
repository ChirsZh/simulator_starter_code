/**
 * fibi.c
 *
 * Fibonacci Iterative Benchmark
 *
 * This benchmark uses the iterative implementation of the Fibonacci function.
 * The benchmark tests how well the processor handles backwards branches and
 * resolves dependencies between instructions.
 **/

// The maximum Fibonacci number to compute in the sequence.
#define MAX_N       15

// The number of iterations to run the Fibonacci function.
#define NUM_ITERS   (28 * (MAX_N + 1))

int fibi(int n)
{
    // The base case for the Fibonacci sequence
    if (n == 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    }

    // Declare the variables for fib(n), fib(n-1), and fib(n-2).
    int fib_n2 = 0;
    int fib_n1 = 1;
    int fib_n = -1;

    // Iteratively compute the nth Fibonacci number
    for (int i = 2; i <= n; i++)
    {
        fib_n = fib_n1 + fib_n2;
        fib_n2 = fib_n1;
        fib_n1 = fib_n;
    }

    return fib_n;
}

int main()
{
    /* Run the iterative Fibonacci function NUM_ITERS times, cycling between
     * computing the Fibonacci numbers between [0, MAX_N]. */
    int i = 0;
    int sum = 0;
    for (int iter = 0; iter < NUM_ITERS; iter++)
    {
        sum += fibi(i);
        i = (i + 1 > MAX_N) ? 0 : i + 1;
    }

    return sum;
}
