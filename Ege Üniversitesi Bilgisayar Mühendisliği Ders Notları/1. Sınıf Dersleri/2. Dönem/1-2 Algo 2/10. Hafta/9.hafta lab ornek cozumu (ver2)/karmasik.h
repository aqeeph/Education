#ifndef KARMASIK_H
#define KARMASIK_H

class Karmasik {
private:
    double reel;
    double sanal;
public:
    Karmasik(double=0, double=0);

    void reelAyarla(double);
    void sanalAyarla(double);

    double reelOku();
    double sanalOku();

    void topla(Karmasik);
    void cikar(Karmasik);
    void goster();
};

#endif
