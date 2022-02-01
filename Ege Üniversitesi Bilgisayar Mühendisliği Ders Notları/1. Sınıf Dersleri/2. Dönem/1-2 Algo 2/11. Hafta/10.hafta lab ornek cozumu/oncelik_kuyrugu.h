#ifndef ONCELIK_KUYRUGU_H_INCLUDED
#define ONCELIK_KUYRUGU_H_INCLUDED

class Oncelik_Kuyrugu {
private:
    Aday *ilk;
    int bekleyen_sayisi;
public:
    Oncelik_Kuyrugu();

    void ekle(Aday *);
    Aday *cikar();
    bool bos_mu();
    int bekleyen_sayisiOku();
    void listele();
};

#endif // ONCELIK_KUYRUGU_H_INCLUDED
