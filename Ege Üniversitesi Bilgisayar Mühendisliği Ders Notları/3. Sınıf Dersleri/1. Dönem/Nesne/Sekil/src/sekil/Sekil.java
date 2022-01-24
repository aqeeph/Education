/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package sekil;

/**
 *
 * @author Egebilmuh
 */
public abstract class Sekil {

    private String sekilAdi;

    public Sekil(String sekilAdi) {
        this.sekilAdi = sekilAdi;
    }

    @Override
    public String toString() {
        return "Sekil{" + "sekilAdi=" + sekilAdi + '}';
    }
    
    public abstract int alanHesapla();
    
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        
        
        
    }
}
