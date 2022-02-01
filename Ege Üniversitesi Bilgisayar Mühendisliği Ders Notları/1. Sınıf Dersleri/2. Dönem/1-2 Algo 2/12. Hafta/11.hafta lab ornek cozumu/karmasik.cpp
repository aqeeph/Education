#include <iostream>

using std::cout;
using std::cin;
using std::endl;
using std::ostream;
using std::istream;

#include "karmasik.h"

//yapici
Karmasik::Karmasik(double r, double s)
    : reel(r), sanal(s) {}

//asiri yuklenmis toplama operatoru
Karmasik Karmasik::operator+(const Karmasik &toplanan) const
{
    return Karmasik(reel+toplanan.reel, sanal+toplanan.sanal);
}

//asiri yuklenmis cikarma operatoru
Karmasik Karmasik::operator-(const Karmasik &cikan) const
{
    return Karmasik(reel-cikan.reel, sanal-cikan.sanal);
}

//asiri yuklenmis atama operatoru
const Karmasik& Karmasik::operator=(const Karmasik &atanan)
{
    reel=atanan.reel;
    sanal=atanan.sanal;

    return *this;
}

void Karmasik::reelAyarla(double r)
{
    reel=r;
}

void Karmasik::sanalAyarla(double s)
{
    sanal=s;
}

double Karmasik::reelOku() const
{
    return reel;
}

double Karmasik::sanalOku() const
{
    return sanal;
}

//asiri yuklenmis carpma operatoru
Karmasik Karmasik::operator*(const Karmasik &carpan) const
{
    //z1=(a+ib) ve z2=(c+id) ise z1*z2=ac-bd+i(ad+bc) dir.(i'nin karesi -1'dir)
    return Karmasik(reel*carpan.reel-sanal*carpan.sanal, reel*carpan.sanal+sanal*carpan.reel);
    //asagidaki sekilde de yapilabilir
/*
    Karmasik carpim;

    carpim.reel=reel*carpan.reel-sanal*carpan.sanal;
    carpim.sanal=reel*carpan.sanal+sanal*carpan.reel;

    return carpim;
*/
}

//asiri yuklenmis esitlik karsilastirma operatoru
bool Karmasik::operator==(const Karmasik &karsilastirilan) const
{
    if (reel==karsilastirilan.reel && sanal==karsilastirilan.sanal)
        return true;

    return false;
}

/*
//asiri yuklenmis cikti operatoru(friend fonksiyon ile)
ostream &operator<<(ostream &cikti, const Karmasik &sayi)
{
    cikti << '(' << sayi.reel << ", " << sayi.sanal << ')';

    return cikti;
}
*/

//asiri yuklenmis cikti operatoru(friend olmayan fonksiyon ile)
ostream &operator<<(ostream &cikti, const Karmasik &sayi)
{
    cikti << '(' << sayi.reelOku() << ", " << sayi.sanalOku() << ')';

    return cikti;
}

/*
//asiri yuklenmis girdi operatoru(friend fonksiyon ile)
istream &operator>>(istream &girdi, Karmasik &sayi)
{
    cout<<"sayinin reel kismini giriniz:";
    girdi>>sayi.reel;
    cout<<"sayinin sanal kismini giriniz:";
    girdi>>sayi.sanal;

    return girdi;
}
*/

//asiri yuklenmis girdi operatoru(friend olmayan fonksiyon ile)
istream &operator>>(istream &girdi, Karmasik &sayi)
{
    double r,s;

    cout<<"sayinin reel kismini giriniz:";
    girdi>>r;
    sayi.reelAyarla(r);

    cout<<"sayinin sanal kismini giriniz:";
    girdi>>s;
    sayi.sanalAyarla(s);

    return girdi;
}
