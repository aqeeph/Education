#ifndef KARMASIK_H_INCLUDED
#define KARMASIK_H_INCLUDED

#include <iostream>

using std::ostream;
using std::istream;

class Karmasik {
    //friend ostream &operator<<(ostream &, const Karmasik &);
    //friend istream &operator>>(istream &, Karmasik &);
    //Veri uyelerine erisim icin Oku ve Ayarla fonksiyonlari yazilirsa,
    //ustteki fonksiyonlar friend olmadan da yapilabilir, asagiya bakiniz...
private:
    double reel;
    double sanal;
public:
    Karmasik(double=0.0, double=0.0);//yapici

    Karmasik operator+(const Karmasik &) const;//toplama
    Karmasik operator-(const Karmasik &) const;//cikarma
    const Karmasik& operator=(const Karmasik &);//atama

    void reelAyarla(double);
    void sanalAyarla(double);
    double reelOku() const;
    double sanalOku() const;

    Karmasik operator*(const Karmasik &) const;//carpma
    bool operator==(const Karmasik &) const;//esitlik karsilastirma
    bool operator!=(const Karmasik &karsilastirilan) const//esitsizlik karsilastirma
        { return !(*this==karsilastirilan); }

};

ostream &operator<<(ostream &, const Karmasik &);
istream &operator>>(istream &, Karmasik &);

#endif // KARMASIK_H_INCLUDED
