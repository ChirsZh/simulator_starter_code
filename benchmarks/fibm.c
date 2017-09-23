/**
 * fibm.c
 *
 * Fibonacci Memory Benchmark
 *
 * This benchmark uses the memory-based version of the iterative implementation
 * of the Fibonacci function. Each intermediate computation is stored in array
 * in memory. This benchmark tests how well the processor handles memory
 * instructions and resolves dependencies between instructions and memory
 * instructions.
 **/

// The maximum Fibonacci number to compute in the sequence.
#define MAX_N       15

// The number of iterations to run the Fibonacci function.
#define NUM_ITERS   (27 * (MAX_N + 1))

int fibm(int n)
{
    // The base case for the Fibonacci sequence
    if (n == 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    }

    // Declare the array to store the Fibonacci sequence of numbers.
    static int fib_seq[MAX_N+1];
    fib_seq[0] = 0;
    fib_seq[1] = 1;

    // Iteratively compute the nth Fibonacci number
    for (int i = 2; i <= n; i++)
    {
        fib_seq[i] = fib_seq[i-1] + fib_seq[i-2];
    }

    return fib_seq[n];
}

int main()
{
    /* Run the iterative Fibonacci function NUM_ITERS times, cycling between
     * computing the Fibonacci numbers between [0, MAX_N]. */
    int i = 0;
    int sum = 0;
    for (int iter = 0; iter < NUM_ITERS; iter++)
    {
        sum += fibm(i);
        i = (i + 1 > MAX_N) ? 0 : i + 1;
    }

    return sum;
}
