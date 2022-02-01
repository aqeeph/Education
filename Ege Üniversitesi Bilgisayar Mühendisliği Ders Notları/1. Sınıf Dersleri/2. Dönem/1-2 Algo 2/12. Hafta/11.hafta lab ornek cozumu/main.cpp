#include <iostream>

using std::cout;
using std::cin;
using std::endl;

#include "karmasik.h"

int main()
{
    Karmasik x, y( 4.3, 8.2 ), z( 3.3, 1.1 );

    cout<<"x: "<<x<<endl<<"y: "<<y<<endl<<"z: "<<z<<endl;

    x = y + z;
    cout << endl << "x = y + z isleminden sonra x: " << x;

    x = y - z;
    cout << endl << "x = y - z isleminden sonra x: " << x;

    x = x + y + z;
    cout << endl << "x = x + y + z isleminden sonra x: " << x;

    x = y * z;
    cout << endl << "x = y * z isleminden sonra x: " << x;

    x = x * z + y * (z - x);
    cout << endl << "x = x * z + y * (z - x) isleminden sonra x: " << x;

    cout << endl << "Bir karmasik sayi girisi:" << endl;
    cin >> y;
    cout << endl << "Diger bir karmasik sayi girisi:" << endl;
    cin >> z;
    if (y == z)
        cout << "Esitler..." << endl;
    if (y != z)
        cout << "Esit degiller..." << endl;

    return 0;
}
