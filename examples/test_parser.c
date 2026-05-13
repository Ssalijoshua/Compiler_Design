/*
 * Comprehensive test file for parser
 * Tests various C language constructs
 */

// Global variable declaration
int global_var = 42;
float pi = 3.14159;

// Function with parameters
int add(int a, int b)
{
    int result = a + b;
    return result;
}

// Function with loop
void count_up(int max)
{
    int i = 0;
    while (i < max)
    {
        i = i + 1;
    }
}

// Function with for loop
int factorial(int n)
{
    int result = 1;
    int i;
    for (i = 1; i <= n; i = i + 1)
    {
        result = result * i;
    }
    return result;
}

// Function with if/else
int abs_value(int x)
{
    if (x < 0)
    {
        return -x;
    }
    else
    {
        return x;
    }
}

// Main function
int main()
{
    int x = 10;
    int y = 20;
    int sum = add(x, y);

    float value = 5.5;
    char name[100] = "Hello World";

    if (sum > 25)
    {
        sum = sum - 5;
    }

    count_up(10);

    int fact = factorial(5);

    int neg = -42;
    int absolute = abs_value(neg);

    return 0;
}
