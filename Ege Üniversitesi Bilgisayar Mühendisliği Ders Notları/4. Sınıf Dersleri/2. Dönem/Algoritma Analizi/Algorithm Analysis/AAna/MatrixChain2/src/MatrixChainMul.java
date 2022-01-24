import java.util.Scanner;


public class MatrixChainMul {
    public static void main(String[] args) {
        int m[][];
        int s[][];
        int mat_count;
        Scanner scan = new Scanner(System.in);
        System.out.println("Enter how many no of matrices to calculate");
        mat_count = scan.nextInt();
        mat_count++;
        int r[] = new int[mat_count];
        int c[] = new int[mat_count];
        m = new int[mat_count][mat_count];
        s = new int[mat_count][mat_count];
    
        for(int i = 1;i < mat_count;i++){
            System.out.println("Enter "+i+" matrix rows and cols");
            r[i] = scan.nextInt();
            c[i] = scan.nextInt();
        }
        
        
        c[0] = r[1];
        
        for(int i = 1; i < mat_count;i++){
            for(int j = 1; j < mat_count; j++)
            m[i][j] = 0;
        }
        
        
        int count = mat_count - 1;
        int a = 1;
        int temp = 0;
        
        while(count > 0){
            for(int i = 1; i < count; i++){
                int j = i + a;
                for(int k = i; k < j; k++){
                    temp = m [i][k] + m[k+1][j] + (c[i-1] * c[k] * c[j]);
                    if(m[i][j] == 0 || m[i][j] > temp){
                        m[i][j] = temp;
                        s[i][j] = k;
                    }
                }
                
            }
                count--;
                a++;
        }
        System.out.println("\n------------M Matrix------------");
        print(m, mat_count - 1);
        System.out.println("\n------------S Matrix------------");
        print(s, mat_count-1); 
        
    }    
    
    
    static void print(int mat[][], int c) {
        
        int row = 1;
        int col = c;
        int i = row,j= col;
        while(true){
            System.out.print(mat[i][j]+"\t");
            if(j == col){
                System.out.println();
                j = col - i;
                i = row;
            }else{
                i++;
                j++;
            }
            if(i == col)
                break;
            
        }
        
    }
    

}