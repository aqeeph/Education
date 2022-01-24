public class Matrix_Chain
{
	public static void main(String[] args)
	{
		int[][] A0=new int[30][35];
		int[][] A1=new int[35][15];
		int[][] A2=new int[15][5];
		int[][] A3=new int[5][10];
		int[][] A4=new int[10][20];
		int[][] A5=new int[20][25];
		
		int[][][] p=new int[6][][];
		p[0]=A0;
		p[1]=A1;
		p[2]=A2;
		p[3]=A3;
		p[4]=A4;
		p[5]=A5;
		
		int[][] m=new int[p.length][p.length];
		
		int[][] s=new int[p.length][p.length];
		
		matrixChain(p,m,s);
		
		for(int i=0;i<m.length;i++)
	{
		for(int j=0;j<m[0].length;j++)
		{
				System.out.print(m[i][j]+" ");
			}
			System.out.println();
		}

		for(int i=0;i<m.length;i++)
		{
			for(int j=0;j<m[0].length;j++)
			{
				System.out.print(s[i][j]+" ");
			}
			System.out.println();
		}
					
		printMatrix(p,s,0,p.length-1);   //p dimension, s matris,0 soldaki boþlarýn karakteri, matrixsayisi
		System.out.println();
	}
	
	public static void matrixChain(int[][][] p,int[][] m,int[][] s)
	{
		int n=p.length;
		for(int i=0;i<n;i++)
			m[i][i]=0;
		
		for(int l=2;l<=n;l++)
		{
			for(int i=0;i<n-l+1;i++)
			{
				int j=i+l-1;
				m[i][j]=Integer.MAX_VALUE;
				for(int k=i;k<j;k++)
				{
					int q=m[i][k]+m[k+1][j]+p[i].length*p[k][0].length*p[j][0].length;
					if(q<m[i][j])
					{
						m[i][j]=q;
						s[i][j]=k;
					}
				}
					
			}	
		}
	}
	
	public static void printMatrix(int[][][] p,int[][] s,int i,int j) //bruteforce'un icine kopyala. cagirma kismi yukardaki comment.
	{
		if(i==j)
			System.out.print("p["+i+"]");
		else 
		{
			System.out.print("(");
			printMatrix(p,s,i,s[i][j]);
			printMatrix(p,s,s[i][j]+1,j);
			System.out.print(")");
		}
	}
}