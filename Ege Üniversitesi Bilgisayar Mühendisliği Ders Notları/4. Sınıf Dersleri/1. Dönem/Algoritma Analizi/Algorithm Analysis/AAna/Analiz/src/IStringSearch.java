/**
 * Das Interface IStringSearch implementieren Klassen, welche die Funktion anbieten,
 * in einem String die Position einer gesuchten Zeichenkette zu bestimmen.
 */
public interface IStringSearch {
  
  /**
   * Sucht das erste Vorkommen von einer Zeichenkette in einem Text.
   * @param text der zu durchsuchende Text
   * @param searchString die zu suchende Zeichenkette
   * @return Index des ersten Vorkommens der gesuchten Zeichenkette,
   *         -1 falls die Zeichenkette nicht vorhanden ist
   */
  public int indexOf (String text, String searchString);
  
  /**
   * Sucht das erste Vorkommen von einer Zeichenkette in einem Text 
   * beginnend bei einer vorgegebenen Startposition.
   * @param text der zu durchsuchende Text
   * @param searchString die zu suchende Zeichenkette
   * @param startIndex Position im Text, bei der die Suche beginnt
   * @return Index des ersten Vorkommens der gesuchten Zeichenkette,
   *         -1 falls die Zeichenkette nicht vorhanden ist
   */
  public int indexOf (String text, String searchString, int startIndex);

}