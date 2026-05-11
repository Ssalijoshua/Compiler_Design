/*
 * Advanced test cases for parser
 * Tests operators, expressions, and edge cases
 */

// Test various operators
int test_operators()
{
    int a = 10;
    int b = 3;
    
    int add_result = a + b;
    int sub_result = a - b;
    int mul_result = a * b;
    int div_result = a / b;
    int mod_result = a % b;
    
    // Logical operators
    int bool1 = 1;
    int bool2 = 0;
    
    int and_result = bool1 && bool2;
    int or_result = bool1 || bool2;
    
    // Comparison operators
    int eq = a == b;
    int ne = a != b;
    int lt = a < b;
    int gt = a > b;
    int le = a <= b;
    int ge = a >= b;
    
    return 0;
}

// Test nested expressions
int test_expressions()
{
    int x = 2 + 3 * 4;
    int y = (2 + 3) * 4;
    int z = 10 - 5 - 2;
    int w = 100 / 2 / 5;
    
    int nested = (1 + 2) * (3 + 4) - (5 * 6);
    
    return nested;
}

// Test array access
void test_arrays()
{
    int arr[10];
    arr[0] = 1;
    arr[5] = 10;
    
    float floats[20];
    floats[3] = 3.14;
    
    char str[100] = "array test";
    
    return;
}

// Test nested loops
void nested_loops()
{
    int i;
    int j;
    
    for (i = 0; i < 10; i = i + 1)
    {
        for (j = 0; j < 10; j = j + 1)
        {
            // nested loop body
        }
    }
    
    return;
}

// Test nested if statements
void nested_conditions()
{
    int x = 5;
    int y = 10;
    
    if (x > 0)
    {
        if (y > 0)
        {
            if (x == 5)
            {
                // nested condition
            }
        }
    }
    
    return;
}

// Main entry point
int main()
{
    test_operators();
    test_expressions();
    test_arrays();
    nested_loops();
    nested_conditions();
    
    return 0;
}
