

public interface List { 

  public int size();
 
  public boolean isEmpty();
  
  public Object element(int i);
  
  public Object head();
  
  public Object last();
  
  public void insert(Object item, int i);
  
  public void insertHead(Object item);
  
  public void insertLast(Object item);
  
  public Object remove(int i);
  
  public Object removeHead();
  
  public Object removeLast();

}
