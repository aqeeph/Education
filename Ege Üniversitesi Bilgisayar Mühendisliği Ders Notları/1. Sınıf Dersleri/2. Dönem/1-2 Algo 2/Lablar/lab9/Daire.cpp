#include <iostream>

using std::cout;
using std::endl;

#include "Daire.h"


Kare::Kare(int r)
{
    kenarayarla(r);
    //ctor
}

void Kare::kenarayarla(int r)
{

    kenar = (kenar==0 || kenar<0 ) ? 1 :r ;
}

int Kare::kenaroku()
{
    return kenar;
}

int Kare::cevrehesapla()
{
    return 4*kenar;
}

int Kare::alanhesapla()
{
    return kenar*kenar;
}

void Kare::kareciz()
{
    for(int i=0; i<kenar; i++)
    {
        for(int j=0; j<kenar; j++)
            cout<<"*";
        cout<<"\n"<<endl;
    }
}
/*Daire::~Daire()
{
    //dtor
}*/
