/**
 * @(#)Araba.java
 *
 *
 * @author 
 * @version 1.00 2009/3/23
 */


public class Araba {
private String marka, renk, plaka;


    public Araba(String m, String r, String p) {
    	marka=m;
    	renk=r;
    	plaka=p;
    }
    
    public String toString() {
    	return "marka="+marka+" renk="+renk+" plaka="+plaka;
    }
    
}