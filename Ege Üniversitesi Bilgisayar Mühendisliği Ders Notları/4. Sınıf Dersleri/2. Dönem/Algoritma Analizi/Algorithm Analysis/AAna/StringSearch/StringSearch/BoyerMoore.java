/**
 * BoyerMoore.java
 * 
 * An implementation of the Boyer-Moore algorithm.
 * 
 * @author
 */
package StringSearch;

public class BoyerMoore extends Search {
	
    // good suffix table
	protected int[] goodSuffixShift;  
	
	public BoyerMoore(String searchString) {
		super(searchString);
	}
	
	// resets the search string
	public void setString(String pattern) {
		super.setString(pattern);
		this.buildShiftTable();
		this.buildGoodSuffixTable();
		
		if (DEBUG) {
			this.printShiftTable();
			this.printGoodSuffixTable();
		}
	}
	
	// finds the longest prefix that matches the suffix of the same size
	// as described on pages 261;
	// returns the shift amount for the matched prefix, 
	// or the pattern length if no matching prefix is found
	private int findLongestPrefix(String suffix) {
		for (int i=suffix.length()-1, j=1; i>0; i--, j++) {
			String prefix = pattern.substring(0,i);
//			System.out.println("trying prefix " + prefix);
//			System.out.println("trying suffix " + suffix.substring(j));
			if (prefix.equals(suffix.substring(j))) {
//				System.out.println("found prefix");
				return pattern.length()-suffix.length()+j;
			}
		} 
		return pattern.length();
	}

	// should build the good-suffix table for the Boyer-Moore algorithm
	// as described on pages 260-261
	private void buildGoodSuffixTable() {
		goodSuffixShift = new int[pattern.length()];
		goodSuffixShift[0] = 0;
		int k=1;
		String suffix = "";
		// search for the exact occurrence of the suffix
		// start at the back of the string, and move forward
		for (int start = pattern.length()-1; start > 0; start--) {
			suffix = pattern.substring(start);
			int position = start-1;
			int shift = 1;
			boolean found = false;
			//			System.out.println("searching for " + suffix);
			while (position >= 0 && !found) {
				//				System.out.println("trying " + pattern.substring(position, position+suffix.length()));
				if (suffix.equals(pattern.substring(position, position+suffix.length()))) {
					//					System.out.println("found  at " + position);
					// check that the character before the suffix isn't the same
					if (position == 0 || pattern.charAt(start-1) != pattern.charAt(position-1))
						found = true;
				}
				if (!found) {
					position--;
					shift++;
				}
			}
			// if exact suffix is found, that's our shift amount
			if (found) {
				goodSuffixShift[k] = shift;
			}
			// otherwise, search for the longest prefix
			// if prefix is found, that's our shift amount
			// otherwise the shift amount is the length of the pattern
			else {
				goodSuffixShift[k] = findLongestPrefix(suffix);
			}
			k++;
		}
	}
	

	// for debugging
    private void printGoodSuffixTable() {
	    if (goodSuffixShift != null) {
		System.out.println("good suffix table");
		for (int i=1; i<goodSuffixShift.length; i++) {
			System.out.println(i + "\t shift by: " + goodSuffixShift[i]);
		}
	    }
	}

	// implement the pseudocode given on page 262 of your textbook
	// the beginning of the method is very, very similar to the Horspool algorithm
	// you may use the String methods: charAt, equals, length, and substring ONLY
	public int search(String text) {
	    // please include similar DEBUGGING information as in the Horspool search method
	    return -1;
	}

	// to help you debug - turn DEBUG flag on (in Search class)
	public static void main(String[] args) {
		// example from page 261
		BoyerMoore bm = new BoyerMoore("ABCBAB");
		
		// example from page 263
		bm.setString("BAOBAB");
		System.out.println(bm.search("BESS_KNEW_ABOUT_BAOBABS"));
	}
}
