import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;
import java.util.Random;
import java.util.Scanner;

public class MatrixChain {
	public static int N[][];

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		System.out.println("How many matrices do you want to multiply?");
		Scanner inp = new Scanner(System.in);
		int matrixCount = inp.nextInt();
		N = new int[matrixCount][matrixCount];
		String outfile = "Matrix Sizes: ";
		int result = 0;
		int[][][] p = new int[matrixCount][][];

		Random rnd = new Random();
		int[] dimension = new int[matrixCount + 1];
		for (int i = 0; i < matrixCount + 1; i++) { // oluþturma

			dimension[i] = 1 + rnd.nextInt(20);

		}
		for (int i = 0; i < matrixCount; i++) {
			for (int j = 0; j < matrixCount; j++) {
				if (i == j)
					N[i][j] = 0;
				else
					N[i][j] = 1000000000;
			}
		}
		// ////////////////////////////////Dynamic programming /////////////////
		for (int i = 0; i < matrixCount; i++) {
			int A[][] = new int[dimension[i]][dimension[i + 1]];
			p[i] = A;
		}

		int[][] m = new int[p.length][p.length];

		int[][] s = new int[p.length][p.length];

		for (int i = 0; i < matrixCount; i++) {
			outfile += "A" + (i + 1) + " " + dimension[i] + " x "
					+ dimension[i + 1] + " ";
			if (i != matrixCount - 1) {
				outfile += "; ";
			}
		}
		long basla = System.currentTimeMillis();
		for (int b = 1; b < matrixCount; b++) {
			result += dimension[0] * dimension[b] * dimension[b + 1];
		}
		long bitir = System.currentTimeMillis();
		// ////////////////////////////Outputs//////////////////////////////////////////////

		outfile += System.getProperty("line.separator");
		outfile += System.getProperty("line.separator");
		outfile += "In-order multiplication";
		outfile += System.getProperty("line.separator");
		outfile += "Number of multiplication: " + result;
		outfile += System.getProperty("line.separator");
		outfile += "Run time(msec) : " + (bitir - basla);
		outfile += System.getProperty("line.separator");
		outfile += System.getProperty("line.separator");
		outfile += System.getProperty("line.separator");
		outfile += "Multiplication with Dynamic Programming";
		outfile += System.getProperty("line.separator");
		outfile += System.getProperty("line.separator");
		outfile += "Table for Dynamic Programming";
		outfile += System.getProperty("line.separator");
		outfile += System.getProperty("line.separator");

		matrixChain(p, m, s);

		for (int i = 0; i < m.length; i++) {
			for (int j = 0; j < m[0].length; j++) {
				System.out.print(m[i][j] + " ");
				outfile += " " + m[i][j];
			}
			System.out.println();
			outfile += System.getProperty("line.separator");
		}

		for (int i = 0; i < m.length; i++) {
			for (int j = 0; j < m[0].length; j++) {
				System.out.print(s[i][j] + " ");
				outfile += " " + s[i][j];
			}
			System.out.println();
			outfile += System.getProperty("line.separator");
		}

		// p dimension, s matris,0 soldaki boþlarýn karakteri, matrixsayisi
		System.out.println();
		outfile += System.getProperty("line.separator");
		outfile += "Matrices with paranthesis: ";
		outfile += printMatrix(p, s, 0, p.length - 1);
		outfile += System.getProperty("line.separator");
		long start = System.currentTimeMillis();
		int result3 = minMCM(dimension, matrixCount);
		long end = System.currentTimeMillis();
		outfile += "Number of multiplication: " + result3;
		outfile += System.getProperty("line.separator");
		outfile += "Run time(msec) : " + (end - start);

		Writer fw = null;
		try {
			fw = new FileWriter(
					"C:/Users/Asus/Desktop/AnalysisFiles/part3_outfile.txt");
			fw.write(outfile);

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

	// //////////////////////////////////////// End of main
	// ///////////////////////////////

	static void print(int mat[][], int c) {

		int row = 1;
		int col = c;
		int i = row, j = col;
		while (true) {
			System.out.print(mat[i][j] + "\t");
			if (j == col) {
				System.out.println();
				j = col - i;
				i = row;
			} else {
				i++;
				j++;
			}
			if (i == col)
				break;

		}

	}

	public static void matrixChain(int[][][] p, int[][] m, int[][] s) {
		int n = p.length;
		for (int i = 0; i < n; i++)
			m[i][i] = 0;

		for (int l = 2; l <= n; l++) {
			for (int i = 0; i < n - l + 1; i++) {
				int j = i + l - 1;
				m[i][j] = Integer.MAX_VALUE;
				for (int k = i; k < j; k++) {
					int q = m[i][k] + m[k + 1][j] + p[i].length
							* p[k][0].length * p[j][0].length;
					if (q < m[i][j]) {
						m[i][j] = q;
						s[i][j] = k;
					}
				}

			}
		}
	}

	public static String printMatrix(int[][][] p, int[][] s, int i, int j) {
		String outfile = "";
		if (i == j) {
			System.out.print("p[" + i + "]");
			outfile += "p[" + i + "]";
		} else {
			System.out.print("(");
			outfile += "(";
			outfile += printMatrix(p, s, i, s[i][j]);
			outfile += printMatrix(p, s, s[i][j] + 1, j);
			System.out.print(")");
			outfile += ")";
		}
		return outfile;
	}

	public static int minMCM(int dimension[], int matrixCount) {
		for (int i = 1; i <= matrixCount; i++) {
			for (int j = 0; j <= matrixCount - i; j++) {
				for (int k = j; k < j + i - 1; k++) {
					if (N[j][j + i - 1] > N[j][k] + N[k + 1][j + i - 1]
							+ dimension[j] * dimension[k + 1]
							* dimension[i + j]) {
						N[j][j + i - 1] = N[j][k] + N[k + 1][j + i - 1]
								+ dimension[j] * dimension[k + 1]
								* dimension[i + j];
					}
				}
			}
		}

		return N[0][matrixCount - 1];
	}
}
