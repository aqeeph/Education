
// Dugum Sınıfı
class Dugum
{
	public int sayi;
	public Dugum sonraki;
	public Dugum(int s)
	{ sayi = s; sonraki = null; }
}

// Liste Sınıfı
class SListe
{
	private Dugum bas;

	public SListe()
	{ bas = null; }

	public boolean IsEmpty()
	{ return bas==null; }

	public void ekle(int yeniVeri)
	{
		Dugum onceki = null;
		Dugum etkin = bas;

		Dugum newNode = new Dugum(yeniVeri);

		while(etkin!=null && yeniVeri > etkin.sayi)
		{
			onceki = etkin;
			etkin = etkin.sonraki; 
		}

		if(onceki==null)
			bas = newNode;
		else
			onceki.sonraki = newNode;
		newNode.sonraki = etkin;
	}






    public Dugum sil(int aranan)
	{
		Dugum onceki = null;
		Dugum etkin = bas;
		
		while(etkin!=null && aranan > etkin.sayi)
		{
			onceki = etkin;
			etkin = etkin.sonraki; 
		}
		
		if( (etkin==null) || (aranan!=etkin.sayi) ) 
		{ System.out.println(aranan + " Bulunamadı"); return null; }
		else
		if(onceki==null) 
		{ bas = bas.sonraki; return etkin; }
		else
		{ onceki.sonraki = etkin.sonraki; return etkin; }
	}
	

	public void yazdir()
	{
		if(IsEmpty())
		{ System.out.println("Boş Liste"); return; };
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



// ListeTest Sınıfı
class ListeTest
{
	public static void main(String[] args)
	{
		SListe liste = new SListe();
		for(int i=1; i<5; i++)
			liste.ekle(i);
		liste.yazdir();
		for(int i=5; i<10; ++i)
			liste.ekle(i);
		liste.yazdir();

		
		Dugum temp = liste.bul(9);
		if(temp==null) 
			System.out.println ("Bulunamadı");
		else 
			System.out.println ("Bulundu");

		liste.sil(9);
		liste.yazdir();

		for(int i=-2; i<8; ++i) 
			if(liste.sil(i)!=null)  
				System.out.println (i+" Silindi");
		liste.sil(5);
		liste.yazdir();



		System.out.println();


	}
}
