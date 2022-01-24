/*

flags 0 < /dev/tty
flags 1 > temp.foo
cat temp.foo
flags 2 >>temp.foo
cat temp.foo 
*/

#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
 int accmode, val;

 if (argc !=2) 
   perror("usage: a.out <descriptor #>"),exit(-1);

 if ((val=fcntl(atoi(argv[1]), F_GETFL,0)) < 0)
   perror("fcntl error for fd %d", atoi(argv[1])), exit(-1);

 accmode=val & O_ACCMODE;
 if (accmode == O_RDONLY) printf("read only");
 else if (accmode == O_WRONLY) printf("write only");
      else if (accmode == O_RDWR) printf("read write");
      else printf(" unknown access mode"), exit(-1);

 if (val & O_APPEND) printf(", append");
 if (val & O_NONBLOCK) printf(", nonblocking");
 putchar("\n");
 exit(0);
}
