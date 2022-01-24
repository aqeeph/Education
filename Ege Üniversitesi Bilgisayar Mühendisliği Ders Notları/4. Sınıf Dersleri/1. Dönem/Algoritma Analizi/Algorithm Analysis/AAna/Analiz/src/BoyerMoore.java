class BoyerMoore

{
	  int CHAR_MAX = 256;
                int bmMatch(String text,String pattern)
                {  
                	  char[] needle = pattern.toCharArray(); //aradýðýmz þey
                      char[] haystack = text.toCharArray(); //içinde aradýðýmz büyük kýsým
                      
                      /* good suffix shift see goodshift(char[], int) below */

                      int[] good_shift = new int[needle.length];

                      for(int i = 0; i < needle.length; i++){
                          good_shift[i] = goodshift(needle, i);
                       
                          System.out.println("Needle: "+ needle[i] +"  Good Þift: "  + good_shift[i]);
                      } 
                      
                      /* bad shift section same argument as simpleBadShift */

                      int[] bad_shift = new int[CHAR_MAX];

                      for(int i = 0; i < CHAR_MAX; i++){
                          bad_shift[i] = needle.length;
                          //System.out.println("Bed Þift: "  + bad_shift[i]);
                      }

                      // offset at which match was found
                      int offset = 0; 
                      int scan = 0;
                      int son = needle.length - 1;
                      int maxoffset = haystack.length - needle.length;


                      for(int i = 0; i < son; i++){
                          bad_shift[needle[i]] = son - i;
                      }


                      // needle can still be inside haystack
                      while(offset <= maxoffset){

                          // start at end of pattern and match backwards
                          for(scan = son; needle[scan] == haystack[scan+offset]; scan--){

                              // we have a match
                              if(scan == 0){
                                  return offset;
                              }
                          }


                          // shift as much as possible based on the good and bad shift
                          offset += Math.max(bad_shift[haystack[offset + son]],
                                             good_shift[son - scan]);
                      
                      }
                      
                                int last[] = buildLast(pattern);
                                int n = text.length(); 
                                int m = pattern.length();
                                int i = m-1; 
                                if (i > n-1)  
                                                return -1; // no match  
                                int j = m-1;
                                do
                                {   
                                if (pattern.charAt(j) == text.charAt(i)) 
                                                if (j == 0)   
                                                                return i; // match     
                                                else
                                                {
                                                                // looking-glass technique 
                                                                i--;   
                                                                j--;     
                                                } 
                                else
                                {
                                                // character jump technique   
                                                int lo = last[text.charAt(i)]; 
                                                //last occ     
                                                i = i + m - Math.min(j, 1+lo);  
                                                j = m - 1;  
                                }  
                                }
                                while (i <= n-1);
                                                return -1; // no match
                }// end of bmMatch()

                int goodshift(char[] needle, int matches){

                    if(matches >= needle.length || matches < 0){
                        return -1;             
                    }                          

                    if(matches == 0){ return 1; } // leave it for badshift

                    int max = needle.length - 1;
                    int offset = max;
                    int last = matches - 1;

                    
                    // keep going as long as we have needle left..
                    while(offset >= 1){

                        --offset;

                        // keep on comparing the elements as long as we got any needle
                        // left, and when you have located matches for the entire suffix
                        // make sure that the next element is a missmatch.
                        for(int i = 0; (offset - i >= 0) && needle[(max - i)] == needle[(offset - i)] ; i++){

                            if( (offset - i) == 0 ){
                                return max - offset;
                            }
             
                            if(i == last){
                                // next char must be missmatch for this to count
                                if( needle[(max - matches)] != needle[(offset - matches)] ){
                                    return max - offset;
                                }else{
                                    break;
                                }
                            }
                        }
                    }

                    // if no suffix found return entire needle.length 
                    // which is legal shift value
                    return needle.length;
                }

                int[] buildLast(String pattern)
                /* Return array storing index of last    occurrence of each ASCII char in pattern. */
                {  
                                int last[] = new int[128];
                                // ASCII char set  
                                for(int i=0; i < 128; i++)  
                                                last[i] = -1;
                                // initialize array  
                                for (int i = 0; i < pattern.length(); i++)
                                                last[pattern.charAt(i)] = i;  
                                return last;
                } // end of buildLast()
}

