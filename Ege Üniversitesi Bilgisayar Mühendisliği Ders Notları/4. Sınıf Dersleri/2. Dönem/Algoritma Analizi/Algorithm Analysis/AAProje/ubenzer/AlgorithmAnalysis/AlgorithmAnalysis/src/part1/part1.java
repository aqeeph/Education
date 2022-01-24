package part1;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;
import util.MyReader;
import util.MyWriter;

public class part1 {

    private static String logString="";
    public static void execute(String inputFile, String outputFile, long samples) {
        int size = 0;
        int[] myArray = null;
        MyReader reader = new MyReader(inputFile);
        try {
            size = reader.getSize();
            myArray = reader.getArrayBound(size);
        } catch (Exception ex) {
            log("Reading file error: " + ex.getMessage());
            System.exit(-1);
        }

        for (int i = 0; i < myArray.length; i++) {
            int[] unsortedArray = new int[myArray[i]];
            int[] afterBubble= new int[myArray[i]];
            int[] afterQuick= new int[myArray[i]];

            /* Random Fill */
            Random rand = new Random();
            for (int j = 0; j < myArray[i]; j++) {
                unsortedArray[j] = rand.nextInt(999999);
            }

            System.out.println("Bubble Sort algorithm is in process. Please wait...");

            BubbleSort bs = new BubbleSort();
            bs.execute(unsortedArray, samples);
            afterBubble = bs.getSortedArray();

            System.out.println("Quick Sort benchmark is in process. Please wait...");

            
            QuickSort qs = new QuickSort();
            qs.execute(unsortedArray, samples);            
            afterQuick = qs.getSortedArray();

            log();

            java.text.DecimalFormat nft = new java.text.DecimalFormat("0000000000");
            nft.setDecimalSeparatorAlwaysShown(false);

            log("*** RESULTS ***");
            log("---------------");
            log("Algorithm        :  BubbleSort   QuickSort");
            log("ArraySize        :  " + nft.format(myArray[i]) + "  " + nft.format(myArray[i]));
            log("Running Time (ns):  " + nft.format(bs.getRunningTime()) + "  " + nft.format(qs.getRunningTime()));
            log("No. of key comp. :  " + nft.format(bs.getNumberOfKeyComparasions()) + "  " + nft.format(qs.getNumberOfKeyComparasions()));
            log("No. of samples   :  " + nft.format(bs.getNumberOfSamples()) + "  " + nft.format(qs.getNumberOfSamples()));
            log();
            nft = new java.text.DecimalFormat("000000");
            log("Unsor.  Bu.So.  Qui.So.");
            log("------  ------  -------");
            for (int a = 0; a < unsortedArray.length; a++) {
                log(nft.format(unsortedArray[a]) + "  " + nft.format(afterBubble[a]) + "  " + nft.format(afterQuick[a]));
            }
            log();
            log();

            MyWriter write= new MyWriter(outputFile,"UTF-8");
            try {
                write.write(getLog());
            } catch (IOException ex) {
                System.out.println("Error writing file: " + ex.getMessage());
                System.exit(-1);
            }
        }
    }
    private static void log(String in) {
        logString += in + "\n";
        System.out.println(in);
    }
    private static void log() {
        logString += "\n";
        System.out.println();
    }
    private static String getLog(){
        return logString;
    }

    public static void benchmark(String outputFile, long samples, int fromItem, int toItem, int stepSize) {
        ArrayList<int[]> dizi = new ArrayList<int[]>();
        for(int i = fromItem; i <= toItem; i = i + stepSize) {
            int[] unsortedArray = new int[i];

            /* Random Fill */
            Random rand = new Random();
            for (int j = 0; j < i; j++) {
                unsortedArray[j] = rand.nextInt(999999);
            }
            dizi.add(unsortedArray);
        }

        log("From " + fromItem + " to " + toItem + " Item with step size " + stepSize + "." );
        log("Sample count: " + samples);
        log();

        log("BUBBLE SORT");
        log("-----------");
        log("Array Si.  Time (ns)  N.of.K.C.");
        log("---------  ---------  ---------");
        int sayac = 0;
        for(int i = fromItem; i <= toItem; i = i + stepSize) {
            int[] unsortedArray = dizi.get(sayac);
            sayac++;

            BubbleSort bs = new BubbleSort();
            bs.execute(unsortedArray, samples);

            java.text.DecimalFormat nft = new java.text.DecimalFormat("000000000");
            nft.setDecimalSeparatorAlwaysShown(false);

            log(nft.format(i) + "  " + nft.format(bs.getRunningTime()) + "  " + nft.format(bs.getNumberOfKeyComparasions()));           
        }

        log();
        log();
        log("QUICK SORT");
        log("-----------");
        log("Array Si.  Time (ns)  N.of.K.C.");
        log("---------  ---------  ---------");
        sayac = 0;
        for(int i = fromItem; i <= toItem; i = i + stepSize) {
            int[] unsortedArray = dizi.get(sayac);
            sayac++;
            
            QuickSort qs = new QuickSort();
            qs.execute(unsortedArray, samples);

            java.text.DecimalFormat nft = new java.text.DecimalFormat("000000000");
            nft.setDecimalSeparatorAlwaysShown(false);

            log(nft.format(i) + "  " + nft.format(qs.getRunningTime()) + "  " + nft.format(qs.getNumberOfKeyComparasions()));
        }

        MyWriter write= new MyWriter(outputFile,"UTF-8");
        try {
            write.write(getLog());
        } catch (IOException ex) {
            System.out.println("Error writing file: " + ex.getMessage());
            System.exit(-1);
        }
    }
}
