#include <stdio.h>
#include <stdlib.h>

int main()
{
    int ogrenci_no,lab_notu,arasinav_notu,final_notu;
    float donem_sonu_notu;

    printf("ogrenci numarasini giriniz:");
    scanf("%d",&ogrenci_no);
    printf("lab, arasinav ve final notlarini birer bosluk birakarak giriniz:");
    scanf("%d %d %d",&lab_notu,&arasinav_notu,&final_notu);

    donem_sonu_notu=0.2*lab_notu+0.3*arasinav_notu+0.5*final_notu;

    printf("Ogrenci no:%d\n",ogrenci_no);
    printf("Lab notu:%d\n",lab_notu);
    printf("Arasinav notu:%d\n",arasinav_notu);
    printf("Final notu:%d\n",final_notu);
    printf("Donem sonu notu:%f\n",donem_sonu_notu);

    return 0;
}
