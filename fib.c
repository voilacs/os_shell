#include <stdio.h>
#include <stdlib.h>

int fibonacci(int n) {
    // Create an array to store Fibonacci numbers
    int fib_table[n + 1];

    // Base cases
    fib_table[0] = 0;
    fib_table[1] = 1;

    // Calculate Fibonacci numbers from the bottom up
    for (int i = 2; i <= n; i++) {
        fib_table[i] = fib_table[i - 1] + fib_table[i - 2];
    }

    return fib_table[n];
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    if (n < 0) {
        printf("Fibonacci number is undefined for negative integers.\n");
    } else {
        int result = fibonacci(n);
        printf("The %d-th Fibonacci number is %d\n", n, result);
    }

    return 0;
}
