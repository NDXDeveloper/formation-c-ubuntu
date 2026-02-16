#include <stdio.h>
int main(void) {
    // Test various GNU extensions
    typeof(int) x = 5;
    printf("%d\n", x);
    
    // Test zero-length array
    // struct { int n; int data[0]; } s;
    
    return 0;
}
