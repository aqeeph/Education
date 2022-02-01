DOMAINS
    sec = integer
    isim = symbol
    soyad = symbol
    gorev = symbol
    telno = real
    ders_liste = symbol*

DATABASE
    calisan(isim, soyad, gorev, telno, ders_liste)
    
PREDICATES
    secilen(sec)
    yazliste(ders_liste)

CLAUSES

	calisan("Musa","Milli", "Asistan", 123456,["1- Algoritma Analizi","2- Yapay Zeka","3- Nesne Tabanli Programlama"]).
	calisan("Betul","Canik", "Profes�r", 8946201,["1- G�rsel Programlama","2- Veri Tabani","3- Web Programlama"]).
	calisan("Hakan","Bal", "Yar.Do�", 7748302,["1- Veri Iletisimi", "2- Bilgisayar Aglari"]).
	calisan("Mehmet","Milli", "Do�ent",7363309,["1- Fizik 1", "2- Matematik 1", "3- Difarensiyel Denklermler"]).
	calisan("Musa","Demir", "Profes�r", 454490,["1- Optimizasyon", "2- Yapay Sinir Aglari"]).
		
		
		
	yazliste ([]).                
	yazliste ([Bas|Kuyruk]):-write(Bas), nl,
				yazliste (Kuyruk).



    secilen(1) :-
        	write("Isimle Arama : "),
        	readln(Isim),
        	calisan(Isim, Soyad, Gorev, TelNo, Ders_liste), nl,
        	write(Isim), write(" "), write(Soyad), write("\t"),
        	write(Gorev), write("\t\t"),  write(TelNo), write(" "), nl,
        	yazliste(Ders_liste), write("\n"),
        	fail.

    secilen(2) :-
        	write("Soyisimle Arama : "),
        	readln(Soyad),
        	calisan(Isim, Soyad, Gorev, TelNo, Ders_liste), nl,
        	write(Isim), write(" "), write(Soyad), write("\t"),
        	write(Gorev), write("\t\t"),  write(TelNo), write(" "), nl,
        	yazliste(Ders_liste), write("\n"),
        	fail.
        
    secilen(3) :-
        	write("Goreviyle Arama : "),
        	readln(Gorev),
        	calisan(Isim, Soyad, Gorev, TelNo, Ders_liste), nl,
        	write(Isim), write(" "), write(Soyad), write("\t"),
        	write(Gorev), write("\t\t"),  write(TelNo), write(" "), nl,
        	yazliste(Ders_liste), write("\n"),
        	fail.
        	
     secilen(4) :-
        	calisan(Isim, Soyad, Gorev, TelNo, Ders_liste), nl,
        	write(Isim), write(" "), write(Soyad), write("\t"),
        	write(Gorev), write("\t\t"),  write(TelNo), write(" "), nl,
        	yazliste(Ders_liste), write("\n"),
        	fail.

    secilen(0) :- exit.

GOAL

        assert(calisan("Canan","Gurbuz", "Asistan", 123456,["1- Bulanik Mantik", "2- Veri Madenciligi", "3- Veri Yapilari", "4- Bilgisayar Grafikleri"])),
        assert(calisan("Levent","Atabek", "Yar.Do�", 234567,["Algoritmalar", "C Programlama", ""])),
        asserta(calisan("Murat","Yaman", "Profes�r", 345678,["1- Java Programlama", "2- Ileri Bilgisayar Grafikleri"])),
        assertz(calisan("Melis","Derman", "Do�ent", 2209871,["1- Elektrik Devre Temelleri", "2- Elektrik - Elektronik"])),


        write("1. Isimle Arama :"),
        write("\n2. Soyisimle Arama :"),
        write("\n3. G�revle Arama :"),
        write("\n4. Hepsini Goruntule :"),
        write("\n0. Cikis"),
        write("\n\nSecim Yapin : "),
        readint(Sec),
        secilen(Sec).