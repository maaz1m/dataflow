#include <iostream>
using namespace std;

void foo(int x)
{
	int y = x+3;
	int z = x + y +3;
}

int main() 
{
    cout << "Hello, World!"<<endl;
    int x = 3+2;
    int *ptr = &x;
    ptr++;
    x++;
    x= 5;
    foo(x);
    return 0;
}