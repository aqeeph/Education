DOMAINS

hastane_ad = symbol                                            
uzman_konu = symbol
hastalik_ad = symbol
belirti = symbol 
Sikayet = symbol                                                                          

PREDICATES

nondeterm hastane(hastane_ad, uzman_konu)
nondeterm hastalik(hastalik_ad, belirti)
nondeterm teshis(symbol, symbol)


CLAUSES

hastane("Karsiyaka Devlet", "Seker Hastaligi").
hastane("Dokuz Eylul Uni Tip Fakultesi Hastanesi", "Tansiyon").
hastane("Ege Üni Tip Fakultesi Hastanesi", "Ortopedi").
hastane("Sifa Hastanesi", "Tansiyon").
hastane("Ekol Hastanesi", "KBB").
hastane("Dunya Goz Hastanesi", "Goz").
hastane("Behcet Uz Cocuk Hastaliklari", "Cocuk Hastaliklari").
hastane("Alsancak Hastanesi", "Tansiyon").
hastane("Metropol", "Seker Hastaligi").


hastalik("Seker Hastaligi", "Bas Donmesi").
hastalik("Tansiyon", "Tansiyon 13 ten büyük").


teshis(T, S):-!,
nl,
hastalik(T, S), 
write("Siz", T, "Hastasisiniz\n"),
fail.

Goal

write("Hastalik Girin :"), nl, 
readln(Sec),
teshis(X, Sec).


