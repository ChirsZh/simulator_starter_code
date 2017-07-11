/**
 * fibr.c
 *
 * Fibonacci Recursive Benchmark
 *
 * This benchmark uses the recursive implementation of the Fibonacci function.
 * The benchmark tests how well the processor handles unconditional jumps and
 * branches.
 **/

// The Fibonacci number to compute.
#define N   15

int fibr(int n)
{
    if (n == 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    } else {
        return fibr(n-1) + fibr(n-2);
    }
}

int main()
{
    return fibr(N);
}
