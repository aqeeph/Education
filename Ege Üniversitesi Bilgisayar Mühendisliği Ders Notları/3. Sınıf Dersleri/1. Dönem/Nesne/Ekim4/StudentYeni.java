public class StudentYeni {
   // instance deðiþkenleri 
   protected String ad;
   protected String soyad;
   protected String adres;
   protected String bolum;
   protected int numara;
   protected Araba oto;
   // class deðiþkeni
   static protected int say=0;
   
   public StudentYeni() { // Parametresiz Constructor
    this.ad="";
    this.soyad="";
    this.adres="";
    this.bolum="";
    this.numara=0;
    oto=null;
   }   

   public StudentYeni(String ad, String soyad, String adres, String bolum, int numara, Araba araba)
   { // constructor metod
   this.ad=ad;
   this.soyad=soyad;
   this.adres=adres;
   this.bolum=bolum;
   this.numara=numara;
   this.oto=araba;
   say++;
   }

   public int nesneSay() {
     return say;
   }

   public void setAdSoyad(String yeniad, String yenisoyad) { //setter metod
     ad=yeniad;
     soyad=yenisoyad;
   }

   public int getNumara() { //getter metod
     return numara;   
   }
 
   public String isim_dondur() {
     return ad;
  }

   public Araba araba_dondur() {
    return oto;
  }

   protected void print( ) {
     // Bu sýnýf baþka bir Java uygulamasý tarafýndan kullanýldýðýnda,
     //  bu metod ile bilgiler ekrana yazýlabilir.
    System.out.println("AD:"+ad+" SOYAD:"+soyad+" NUMARA:"+numara);
   }
  
   public String toString() {
     return "Ad Soyad:"+ad+" "+soyad+"\n"+"Numara:"+Integer.toString(numara)
     +"\n"+"Adres:"+adres+"\n"+"Bolum:"+bolum+"\n"+ (oto.toString());
  }

} // Student class sonu