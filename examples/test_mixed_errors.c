// Test file with BOTH syntax and semantic errors

int add(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;
    int y = 20
    // Missing semicolon above - SYNTAX ERROR at line 9
    
    z = x + y;  // Undefined variable z - SEMANTIC ERROR
    
    int w = add(x y);  // Missing comma in function call - SYNTAX ERROR
    
    return x
}