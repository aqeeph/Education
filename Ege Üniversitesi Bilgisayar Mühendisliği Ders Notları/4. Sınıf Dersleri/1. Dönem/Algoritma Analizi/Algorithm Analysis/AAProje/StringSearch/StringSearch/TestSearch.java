/**
 * TestSearch.java
 * 
 * Tests implementations of Horspool an Boyer-Moore on binary strings.
 */
package StringSearch;
import java.util.Scanner;

/**
 * Modify this code to compare timings for the string search algorithms
 * @author hhh
 *
 */
public class TestSearch {
	// builds a random binary string of the specified length
	private static String buildBinaryString (int length) {
		String pattern = "";
		while (pattern.length() < length) {
			long number = (long)(Math.random()*Long.MAX_VALUE);
			pattern = pattern + Long.toBinaryString(number);
		}
		pattern = pattern.substring(0, length);
		return pattern;
	}
	
	// runs all the string search algorithms on the specified text
	// Precondition: the pattern should be set when the algorithms are created
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
		Scanner keyboard = new Scanner(System.in);
		
		// set up a random binary pattern and a random binary text
		System.out.println("How long would you like your binary pattern: ");
		int patternLength = keyboard.nextInt();
		String pattern = buildBinaryString(patternLength);
		System.out.println("Your pattern is: " + pattern);
		System.out.println("How long would you like your binary text to be searched: ");
		int textLength = keyboard.nextInt();
		System.out.println("How many texts would you like to search for the binary pattern: ");
		int times = keyboard.nextInt();
		
		Search[] algorithm = new Search[3];
		algorithm[0] = new BruteForce(pattern);
		algorithm[1] = new Horspool(pattern);
		algorithm[2] = new BoyerMoore(pattern);
		
		for (int i=0; i<times; i++) {
			String text = buildBinaryString(textLength);
			testStringSearches(text, algorithm);
			System.out.println();
		}
				
	}
}
