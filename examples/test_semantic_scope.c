// SEMANTIC ERROR TEST 3: Scope and Symbol Resolution

int globalVar = 42;

void processData()
{
    int x = 10;

    {
        int x = 20;           // Valid: redefine in inner scope
        int y = x + localVar; // Error: localVar is undefined
    }

    int z = undefinedFunc(x); // Error: undefinedFunc not defined
    int missing = unknownVar; // Error: unknownVar not defined
}

int main()
{
    int result = globalVar;

    processData();

    int result = 100;                   // Error: result already defined
    int undefined_use = notDefined + 5; // Error: notDefined undefined

    return 0;
}
