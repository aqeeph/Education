/**
 * Horspool.java
 * 
 * An implmentation of Horspool's Algorithm.
 * 
 * @author
 */
package StringSearch;

public class Horspool extends Search {

	public Horspool(String searchString) {
		super(searchString);
	}
	
	public void setString(String pattern) {
		super.setString(pattern);
		this.buildShiftTable();
		if (DEBUG) 
			this.printShiftTable();
	}
	
	// implement the pseudocode given on page 258 of your textbook
	// you may ONLY use the String methods: charAt, equals, length, and substring
	public int search(String text) {
		int m = pattern.length();
		int i = m-1;
		while (i < text.length()) {
			int k=0;
			if (DEBUG) {
				System.out.println("Comparing " + pattern.charAt(m-1-k));
				System.out.println("\tto " + text.charAt(i-k));
			}

			// complete remaining code

		}
		return -1;
	}

}
