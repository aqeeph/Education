class StudentYeniTest {

 public static void main(String[] args) {
    Araba a1;
    StudentYeni s1;
    a1=new Araba("Rover","Siyah","06 ABC 99");
    s1=new StudentYeni("ahmet","demir","bornova","gida",2911,a1);
    s1.print();
    System.out.println(s1);
 }
}