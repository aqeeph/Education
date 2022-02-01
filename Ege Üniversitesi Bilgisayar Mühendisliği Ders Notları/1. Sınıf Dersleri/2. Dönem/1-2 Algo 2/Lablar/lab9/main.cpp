#include <iostream>

using namespace std;
using std::cout;
using std::endl;

#include "Daire.h"

int main()
{
    Kare d1;
    Kare d2;

    cout <<"karenin alani ve cevresi:"<<d1.alanhesapla()<<endl<<d1.cevrehesapla()<<endl;
     d1.kenarayarla(5);
    cout<<"karenin alani ve çevresi:"<<d1.alanhesapla()<<endl <<d1.cevrehesapla()<<endl;
    d1.kareciz();
    d2.kenarayarla(10);
    cout<<"karenin alani ve cevresi:"<<d2.alanhesapla()<<endl<<d2.cevrehesapla()<<endl;
    d2.kareciz();
    d2.kenarayarla(0.0);
    cout<<"karenin alani ve cevresi:"<<d2.alanhesapla()<<d2.cevrehesapla()<<endl;
    d2.kareciz();



    return 0;
}
