
// Dugum Sýnýfý
class Dugum
{
	public int sayi;
	
	public Dugum sonraki;
	public Dugum onceki;
	public Dugum(int s)
	{ sayi = s; }
}

// Liste Sýnýfý
class DListe
{
	private Dugum bas;
	private Dugum son;

	public DListe()
	{ bas = null; son = null; }

	public boolean IsEmpty()
	{ return bas==null; }

	public void basaEkle(int yeniVeri)
	{
		Dugum yeniDugum = new Dugum(yeniVeri);

		if(IsEmpty())
			son = yeniDugum;
		else
			bas.onceki = yeniDugum;
		yeniDugum.sonraki = bas;
		bas = yeniDugum;
	}

	public void sonaEkle(int yeniVeri)
	{
		Dugum yeniDugum = new Dugum(yeniVeri);

		if(IsEmpty())
			bas = yeniDugum;
		else
		{
			son.sonraki = yeniDugum;
			yeniDugum.onceki = son;
		}
		son = yeniDugum;
	}

	// Boþ Liste Kontrolü yok
	public Dugum bastanSil()
	{
		Dugum temp = bas;
		if(bas.sonraki==null)
			son = null;
		else
			bas.sonraki.onceki = null;
		bas = bas.sonraki;
		return temp;
	}

	// Boþ Liste Kontrolü yok
	public Dugum sondanSil()
	{
		Dugum temp=son;
		if(bas.sonraki == null) 
			bas = null;
		else
			son.onceki.sonraki = null;
		son = son.onceki;
		return temp;
	}



	public void Yazdir()
	{
		if(IsEmpty())
		{ System.out.println("Boþ Liste"); return; };
		Dugum etkin = bas;
		while(etkin!=null)
		{
			System.out.print(etkin.sayi+" ");
			etkin=etkin.sonraki;
		}
		System.out.println("\n");
	}

	public Dugum bul(int aranan)
	{
		Dugum etkin = bas;
		while(etkin.sayi != aranan)
			if(etkin.sonraki == null)
				return null;
			else
				etkin = etkin.sonraki;
		return etkin;
	}

}



// ListeTest Sýnýfý
class ListeTest
{
	public static void main(String[] args)
	{

		
		DListe liste = new DListe();
		for(int i=1; i<5; i++)
			liste.basaEkle(i);
		liste.Yazdir();
		for(int i=5; i<10; ++i)
			liste.sonaEkle(i);
		liste.Yazdir();

		
		Dugum temp = liste.bul(9);
		if(temp==null) 
			System.out.println ("Bulunamadý");
		else 
			System.out.println ("Bulundu");

		liste.bastanSil();
		liste.Yazdir();

		//		while(!liste.IsEmpty()) liste.bastanSil();


		for(int i=0; i<8; ++i) 
			if((temp=liste.bastanSil())!=null)  
				System.out.println (temp.sayi+" Silindi");
		
//		liste.sil(5);
//		liste.bastanSil();
		liste.Yazdir();

	}
}
