// SEMANTIC ERROR TEST 1: Type Mismatch
// Assigning incompatible types in variable initialization

int main()
{
    int x = "hello"; // Type mismatch: string to int
    float y = 42;    // Type mismatch: int to float (in some type systems)
    char c = 3.14;   // Type mismatch: float to char

    return 0;
}
