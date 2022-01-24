/**
 * Implementierung des Horspool-Algorithmus.
 */
public class Horspool implements IStringSearch {
  
  /**
   * Erstellen der Verschiebe-Tabelle zu einem Suchstring.
   */
  private int[] createCharShiftTable (String searchString) {
    int[] charshift = new int[256]; 
    int len = searchString.length();
    // alle Felder mit der Laenge des Suchstrings belegen.
    for (int i = 0; i < charshift.length; i++) {
      charshift[i] = len;
    }
    // Felder für vorhandene Zeichen mit der Distanz ihres jeweils 
    // letzten Vorkommens zum Ende der Zeichenkette belegen. 
    // Das letzte Zeichen wird nicht berechnet.
    for (int i = 0; i < len - 1; i++) {
      charshift[searchString.charAt(i)] = len - i - 1;      
    }
    return charshift;
  }
  
  /**
   * Suche nach einer Zeichenkette im Text von Beginn an.
   */
  public int indexOf (String text, String searchString) {
    return indexOf(text, searchString, 0);
  }
  
  /**
   * Suche nach einer Zeichenkette im Text von Position startIndex an.
   */
  public int indexOf (String text, String searchString, int startIndex) {
    int index = -1;
    // Sicherstellen, dass startIndex nicht negativ
    startIndex = Math.max(startIndex, 0);
    if (searchString.length() == 0) { 
      index = startIndex;
    } else {
      int[] charshift = createCharShiftTable(searchString);
      int highSearchString = searchString.length() - 1;
      int indexText = highSearchString + startIndex;
      int indexSearchString = highSearchString;
      // Horspool-Suche
      while (indexText < text.length()) { 
        if (text.charAt(indexText) == searchString.charAt(indexSearchString)) {       
          if (indexSearchString == 0) {
            // Falls alle Zeichen erfolgreich abgeglichen wurden
            index = indexText;
            break;         
          } else {
            // sonst zum naechsten Zeichen gehen
            indexText--;
            indexSearchString--;
          }
        // Falls zu vergleichende Zeichen in Text und Suchstring ungleich sind...
        } else {
          // verschiebe mittels Verschiebe-Tabelle anhand des rechten Zeichens im Textfenster 
          indexText = indexText + charshift[text.charAt(indexText + highSearchString - indexSearchString)];         
          // setzt den Index wieder auf das Ende des Suchstrings.
          indexSearchString = highSearchString;
          
        }
      }
     
      for (int a=0;a<charshift.length;a++) {
    	  System.out.println("character c: " +(char)a +"  shift t(c): "+ charshift[a]);
      }
    }
    return index;
  }

}