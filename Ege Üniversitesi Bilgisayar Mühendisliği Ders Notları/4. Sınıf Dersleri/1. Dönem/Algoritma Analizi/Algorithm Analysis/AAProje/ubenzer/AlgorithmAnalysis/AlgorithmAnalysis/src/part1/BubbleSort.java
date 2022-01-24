package part1;

public class BubbleSort {
    private long runningTime = 0;
    private long count = 0;
    private long samples = 0;
    private int[] sortedArray;

    public void execute(int[] unsortedArray) {
        this.execute(unsortedArray, 1);
    }
    public void execute(int[] unsortedArray, long samples) {        
        this.samples = samples;
        long startTime = System.nanoTime();
        for (int i=0; i < samples; i++) {
            sortedArray=unsortedArray.clone();
            bubbleSort(sortedArray);
        }
        long estimatedTime = System.nanoTime() - startTime;
        this.runningTime = estimatedTime;        
    }
    public long getRunningTime() {
        return runningTime / samples;
    }
    private void bubbleSort(int[] unsortedArray) {
        int temp, counter, index;
        for (counter = 0; counter < unsortedArray.length - 1; counter++) {
            for (index = 0; index < unsortedArray.length - 1 - counter; index++) {
                if (unsortedArray[index] > unsortedArray[index + 1]) {
                    temp = unsortedArray[index];
                    unsortedArray[index] = unsortedArray[index + 1];
                    unsortedArray[index + 1] = temp;                    
                }
                count++;
            }
        }
        this.sortedArray = unsortedArray;
    }
    public long getNumberOfKeyComparasions() {
        return this.count / samples;
    }
    public long getNumberOfSamples() {
        return this.samples;
    }
    public int[] getSortedArray() {
        return this.sortedArray;
    }
}
