/**
 * TestSimpleSearch.java
 * 
 * Simple test cases for checking your string search algorithms work.
 * 
 * @author Greg Gagne and Helen Hu
 *
 */

package StringSearch;

public class TestSimpleSearch {
	private static void testStringSearches(String text, Search[] algorithm) {
		for (int i=0; i<algorithm.length; i++) {
			int found = algorithm[i].search(text);
			System.out.print(algorithm[i].getClass().getSimpleName() + ": ");
			found = algorithm[i].search(text);
			if (found == -1)
				System.out.println("not found");
			else
				System.out.println("found at " + found);
		}
	}

	public static void main(String[] args) {

		String pattern = "BARBER";
		
		Search[] algorithm = new Search[3];
		algorithm[0] = new BruteForce(pattern);
//		algorithm[1] = new Horspool(pattern);
//		algorithm[2] = new BoyerMoore(pattern);
		
		// example from page 259
		// should be found at 16
		String text = "JIM_SAW_ME_IN_A_BARBERSHOP";
		testStringSearches(text, algorithm);
		System.out.println();
		
		// now repeat for a text that doesn't include BARBER
		// should NOT be found
		text = "JIM_SAW_ME_IN_A_BASBERSHOP";
		testStringSearches(text, algorithm);
		System.out.println();

		// repeat for the example on page 263
		// should be found at 16 again
		for (int i=0; i<algorithm.length; i++) 
			algorithm[i].setString("BAOBAB");
		text = "BESS_KNEW_ABOUT_BAOBABS";
		testStringSearches(text, algorithm);	
		System.out.println();
	}

}
