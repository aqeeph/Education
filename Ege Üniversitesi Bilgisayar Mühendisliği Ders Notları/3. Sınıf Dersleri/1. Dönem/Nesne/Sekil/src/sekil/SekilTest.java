/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package sekil;

import java.lang.reflect.Field;
import java.util.ArrayList;

/**
 *
 * @author Egebilmuh
 */
public class SekilTest {
    
    public static void main(String[] args) {
        
        ArrayList list=new ArrayList();
        
        Sekil s=new Kare(5);
        System.out.println(s.toString()+" alan:"+s.alanHesapla());
        
        
        
        list.add(s);
        s=new Dikdortgen(5,10);
        
        list.add(s);
        
        for (int i=0;i<list.size();i++){
            Sekil sek=(Sekil)list.get(i);
            System.out.println(sek.alanHesapla());
        }
        
        Dikdortgen d1= new Dikdortgen(10,20);
        Dikdortgen d2=d1.clone();
        d2.getList().add("gfhgh");
        
        System.out.println(d1.getList().size());
        System.out.println(d2.getList().size());
        
        Kare k1=new Kare(12);
        Kare k2=(Kare)k1.clone();
        k2.getList().add("fghngn");
        
        System.out.println(k1.getList().size());
        System.out.println(k2.getList().size());
        
        Field[] f=Dikdortgen.class.getDeclaredFields();
        for (int i=0;i<f.length;i++){
            System.out.println(f[i].getName());
        }
        
        d1= new Dikdortgen(20,40);
        d2= new Dikdortgen(20,40);
        System.out.println(d1.equals(d2));
        
    }
    
}
