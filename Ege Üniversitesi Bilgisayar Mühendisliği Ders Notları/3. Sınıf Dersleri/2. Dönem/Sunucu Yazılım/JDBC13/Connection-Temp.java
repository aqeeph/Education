package coreservlets;

// TO DO: Delete this *class file* when done testing!

public class Connection {
  private static int totalCount = 0;
  public int count;
  private boolean flag = false;
  
  public Connection() {
    count = totalCount++;
    System.out.println("Created " + toString());
    if (Math.random() < 0.33) {
      flag = true;
    }
  }

  public boolean wasClosed() {
    return(flag);
  }
  
  public String toString() {
    return("Connection " + count);
  }
}
