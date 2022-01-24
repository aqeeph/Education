/**
 * Search.java
 * 
 * This abstract class is used for building the shift table in 
 * Horspool and Boyer-Moore algorithm.
 * 
 * @author Greg Gagne and Helen Hu
 * 
 * October 2011
 */

package StringSearch;

import java.util.*;

public abstract class Search {
	// set the status of debugging
	protected final boolean DEBUG = false;
	
	// the shift table
	protected TreeMap<Character, ShiftNode> badSymbolTable = new TreeMap<Character, ShiftNode>();
	
	// the pattern we are matching
	protected String pattern;
	
	public Search(String searchString) {
		this.setString(searchString);
	}
	
	// returns the search string (the pattern)
	public String getString() {
		return pattern;
	}
	
	// resets the search string
	public void setString(String searchString) {
		this.pattern = searchString;
	}
	
	// should return -1 if the string is not found, or the location of the pattern if
	// the search string exists in the text
	public abstract int search(String text);
    
	/*
	 * Build a shift table as used by Horspool's and Boyer-Moore algorithm
	 * see page 257 in your textbook
	 * You only need to enter characters in the pattern into the table.
	 * All other characters in the alphabet will automatically be set to the pattern length.
	 */
	protected void buildShiftTable() {
		
	}
	
	// for debugging
	protected void printShiftTable() {
		System.out.println("bad symbol table: ");
		Collection <ShiftNode> values = badSymbolTable.values();
		for (ShiftNode node : values) 
			System.out.println(node);
		System.out.println("all other entries should be shifted " + getDefaultShift());
		System.out.println();
	}
	
	// returns the bad symbol table's entry for the character c
	protected int getShift(char c) {
		int shift = this.getDefaultShift();
		ShiftNode retrieved = this.badSymbolTable.get(new Character(c));
		if (retrieved != null) 
			shift = retrieved.getShift();
		return shift;
	}
		
	//  returns the length of the pattern
	protected int getDefaultShift() {
		return pattern.length();
	}
	
}
