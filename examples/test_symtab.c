// Simple test to show symbol table

int global_var = 100;
float pi = 3.14;

int add(int a, int b)
{
    return a + b;
}

float multiply(float x, float y)
{
    return x * y;
}

int main()
{
    int local_x = 10;
    int local_y = 20;
    int result = local_x + local_y;

    return 0;
}
