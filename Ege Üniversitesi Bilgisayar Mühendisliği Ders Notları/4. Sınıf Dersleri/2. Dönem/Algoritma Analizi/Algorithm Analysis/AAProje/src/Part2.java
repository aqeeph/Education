import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;

public class Part2 implements IStringSearch {
	static int brutestringcounter;
	public static String boyer = "";
	public static String horspool = "";

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		// Pattern s= new Pattern();
		String outfile = "Brute-Force String Matching";
		outfile += System.getProperty("line.separator");

		System.out.println("--- Start String Matching Algorithms ---");

		String text = null;
		String pattern = null;

		try {
			BufferedReader reader = null;
			reader = new BufferedReader(new FileReader(
					"C:/Users/Asus/Desktop/AnalysisFiles/part2_infile.txt"));
			text = reader.readLine();
			pattern = reader.readLine();
			reader.close();
		} catch (final IOException e) {
		}

		System.out.println("Text: " + text);
		System.out.println("Pattern: " + pattern);

		long basla = System.currentTimeMillis();
		int posn = brute(text, pattern);
		long bitir = System.currentTimeMillis();
		if (posn == -1)
			System.out.println("Pattern not found");
		else
			System.out.println("Pattern starts at positionn " + (posn + 1));

		outfile += "Number of Key Comparison : " + brutestringcounter;
		outfile += System.getProperty("line.separator");
		outfile += "Run time(msec) : " + (bitir - basla);
		outfile += System.getProperty("line.separator");
		outfile += System.getProperty("line.separator");
		outfile += "Horspool’s algorithm";
		outfile += System.getProperty("line.separator");

		IStringSearch horspool = new Horspool();
		long baslaHorspool = System.currentTimeMillis();
		search(horspool, text, pattern);
		long bitirHorspool = System.currentTimeMillis();

		outfile += "Shift Table: ";
		outfile += System.getProperty("line.separator");
		outfile += "Number of Key Comparison: ";
		outfile += System.getProperty("line.separator");
		outfile += "Run time(msec) : " + (bitirHorspool - baslaHorspool);
		outfile += System.getProperty("line.separator");
		outfile += System.getProperty("line.separator");
		outfile += "Boyer-Moore algorithm";
		outfile += System.getProperty("line.separator");
		
		IStringSearch boyer = new BoyerMoore();
		long baslaBoyer = System.currentTimeMillis();
		search(boyer, text, pattern);
		long bitirBoyer = System.currentTimeMillis();
		
		outfile += "Bad Symbol Shift Table: ";
		outfile += System.getProperty("line.separator");
		outfile += "Good-Suffix Shift Table: ";
		outfile += System.getProperty("line.separator");
		outfile += "Number of Key Comparison: ";
		outfile += System.getProperty("line.separator");
		outfile += "Run time(msec) : " + (bitirBoyer - baslaBoyer);

		System.out.println("\n--- Finish ---");

		Writer fw = null;
		try {
			fw = new FileWriter(
					"C:/Users/Asus/Desktop/AnalysisFiles/part2_outfile.txt");
			fw.write(outfile);
			// fw.append(System.getProperty("line.seperator"));
		} catch (IOException e) {
			System.err.println("Dosya olusturulamadi");
		} finally {
			if (fw != null)
				try {
					fw.close();
				} catch (IOException e) {
				}
		}
	}

	// //////end of main/////////////////

	static int brute(String text, String pattern) {
		int n = text.length(); // n is the length of text
		int m = pattern.length(); // m is length of pattern
		int j;
		for (int i = 0; i <= (n - m); i++) {
			j = 0;
			while ((j < m) && (text.charAt(i + j) == pattern.charAt(j)))
				j++;
			brutestringcounter++;

			if (j == m) {
				brutestringcounter += (m - 1);
				System.out
						.println("BruteForce String Matching Comparison Count: "
								+ brutestringcounter);
				return i; // match at i
			}
		}

		return -1; // no match
	}

	private int[] createCharShiftTable(String searchString) {
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

	public int indexOf(String text, String searchString) {
		return indexOf(text, searchString, 0);
	}

	public int indexOf(String text, String searchString, int startIndex) {
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
				if (text.charAt(indexText) == searchString
						.charAt(indexSearchString)) {
					if (indexSearchString == 0) {
						// Falls alle Zeichen erfolgreich abgeglichen wurden
						index = indexText;
						break;
					} else {
						// sonst zum naechsten Zeichen gehen
						indexText--;
						indexSearchString--;
					}
					// Falls zu vergleichende Zeichen in Text und Suchstring
					// ungleich sind...
				} else {
					// verschiebe mittels Verschiebe-Tabelle anhand des rechten
					// Zeichens im Textfenster
					indexText = indexText
							+ charshift[text.charAt(indexText
									+ highSearchString - indexSearchString)];
					// setzt den Index wieder auf das Ende des Suchstrings.
					indexSearchString = highSearchString;

				}
			}

			for (int a = 0; a < charshift.length; a++) {
				System.out.println("character c: " + (char) a
						+ "  shift t(c): " + charshift[a]);
			}
		}
		return index;
	}

	public static void search(IStringSearch ss, String text, String searchString) {
		// en baþtan aratma. index 0 dan baþlayarak
		System.out.println("Searching pattern \"" + searchString + "\" ...\n");
		try {
			// Horspool-Suche vs.
			int index1 = ss.indexOf(text, searchString); // bu fonksiyon saðdan
															// itibaren ilk
															// occurence'ý
															// buluyor.
			// ... Java-String-Suche
			int index2 = text.indexOf(searchString);
			if (index1 == index2) {
				System.out.println("Found in " + (index1 + 1) + ". position.");
			} else {
				System.out.println("Not found  " + index1
						+ " ist nicht korrekt. \"" + searchString
						+ "\" steht an Index " + index2);
			}
		} catch (Exception e) {
			System.err.println("Bei der Suche nach \"" + searchString
					+ "\" ist ein Fehler aufgetreten!");
			e.printStackTrace();
		}
	}

}
