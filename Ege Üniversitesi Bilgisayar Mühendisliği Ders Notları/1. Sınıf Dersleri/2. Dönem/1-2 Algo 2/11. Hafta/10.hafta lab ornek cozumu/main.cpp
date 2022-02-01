#include <iostream>

using namespace std;

#include "aday.h"
#include "oncelik_kuyrugu.h"

int menu_secim(void);

int main()
{
    Oncelik_Kuyrugu kuyruk;//Oncelik_Kuyrugu sinifindan bir kuyruk nesnesi turetiliyor
    int no,boy;
    Aday *birAday;

    int secenek;

	do {
        secenek=menu_secim();
        switch(secenek) {
            case 1:
                cout << "Aday numarasini giriniz:";
                cin >> no;
                cout << "Adayin boyunu giriniz:";
                cin >> boy;
                birAday=new Aday(no,boy);//yeni gelen aday icin Aday sinifindan bir nesne turetiliyor
                kuyruk.ekle(birAday);//aday, kuyruga boy sirasina gore yerlestiriliyor
                break;
            case 2:
                if (!kuyruk.bos_mu()) {//kuyruk bos degilse...
                    cout << "Sinava cagrilan aday:" << endl;
                    cout << "Aday No Boy" << endl;
                    cout << "------- ---" << endl;
                    birAday=kuyruk.cikar();//siradaki aday sinava cagiriliyor
                    birAday->yazdir();//cagrilan adayin bilgileri ekrana yazdiriliyor
                    delete birAday;//aday nesnesi yok ediliyor (programin devaminda kullanilacaksa yok edilmemeli
                }
                else
                    cout << "Bekleme salonu bos" << endl;
                break;
            case 3:
                cout << "Bekleme salonunda bekleyen aday sayisi:" << kuyruk.bekleyen_sayisiOku() << endl;
                break;
            case 4:
                if (!kuyruk.bos_mu()) {//kuyruk bos degilse...
                    cout << "Bekleme salonunda bekleyen adaylar:" << endl;
                    cout << "Aday No Boy" << endl;
                    cout << "------- ---" << endl;
                    kuyruk.listele();//o salonda bekleyen tum adaylar boy sirasina gore listeleniyor
                }
                else
                    cout << "Bekleme salonu bos" << endl;
                break;
        }
    } while (secenek!=5);
    return 0;
}

int menu_secim(void){
    int secim;

    cout << endl;
    cout << "********************MENU************************************" << endl;
    cout << "1. Bir adayin bekleme salonuna alinmasi" << endl;
    cout << "2. Bekleme salonundan siradaki adayin sinava cagrilmasi" << endl;
    cout << "3. Bekleme salonunda bekleyen aday sayisinin goruntulenmesi" << endl;
    cout << "4. Bekleme salonunda bekleyen adaylarin bilgilerinin listelenmesi" << endl;
    cout << "5. Cikis" << endl;
    do {
        cout << "Seciminizi Giriniz:  ";
        cin >> secim;
    }while(secim<1 || secim>5);

    return secim;
}
