#include <iostream>
using namespace std ;

int main() {
    cout << "1000 36\n";
    for(int i = 0 ; i < 1000 ; ++i)
    {
        if(i % 36 == 1)
            cout << "UNDO\n";
        else
            cout << "NEW " << i << '\n';
    }
}