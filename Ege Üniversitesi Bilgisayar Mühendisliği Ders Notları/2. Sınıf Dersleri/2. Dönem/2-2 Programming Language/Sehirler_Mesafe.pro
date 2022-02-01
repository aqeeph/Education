DOMAINS

sehir           = symbol

mesafe      = integer

PREDICATES

nondeterm yol(sehir,sehir,mesafe)

nondeterm guzergah(sehir,sehir,mesafe)

CLAUSES

yol(mardin,sanliurfa,190).

yol(gaziantep,mardin,320).

yol(sanliurfa,gaziantep,146).

yol(sanliurfa,elazig,348).

yol(gaziantep,elazig,540).

guzergah(_1_Sehir,_2_Sehir,Mesafe):-

yol(_1_Sehir,_2_Sehir, Mesafe).

guzergah(_1_Sehir,_2_Sehir,Mesafe):-

yol(_1_Sehir,X,Mesafe1),

guzergah(X,_2_Sehir,Mesafe2),

Mesafe=Mesafe1+Mesafe2, !.

GOAL guzergah(sanliurfa,elazig, Toplam_Mesafe).
