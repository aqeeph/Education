#include <iostream>

using std::cout;
using std::endl;

#include "karmasik.h"

Karmasik::Karmasik(double r, double s)
{
    reelAyarla(r);
    sanalAyarla(s);
}

void Karmasik::reelAyarla(double r)
{
    reel=r;
}

void Karmasik::sanalAyarla(double s)
{
    sanal=s;
}

double Karmasik::reelOku()
{
    return reel;
}

double Karmasik::sanalOku()
{
    return sanal;
}

void Karmasik::topla(Karmasik toplanan)
{
    reel=reel+toplanan.reelOku();
    sanal=sanal+toplanan.sanalOku();
}

void Karmasik::cikar(Karmasik cikan)
{
    reel=reel-cikan.reelOku();
    sanal=sanal-cikan.sanalOku();
}

void Karmasik::goster()
{
    cout << '(' << reel << ", " << sanal << ')' << endl;
}
