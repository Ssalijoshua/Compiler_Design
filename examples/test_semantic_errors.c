// Test file for SEMANTIC ERRORS
// These are errors caught during semantic analysis

int main() {
    int x = 10;
    
    // Using undefined variable - SEMANTIC ERROR at line 7
    y = x + 5;
    
    // Redefining variable - SEMANTIC ERROR at line 10
    int x = 20;
    
    // Calling undefined function - SEMANTIC ERROR at line 13
    result = undefined_function(x);
    
    return 0;
}
