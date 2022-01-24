/*
  hole
  ls -l file.hole
  od -c file.hole
*/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> /* bunu cikar SEEK_SET hatasi verecek */
char buf1[]="abcdefghij";
char buf2[]="ABCDEFGHIJ";

int main(void)
{
 int fd;

 if ( (fd = creat("file.hole",S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
  perror("creat error"), exit(-1);

 if(write(fd, buf1,10) != 10)
  perror("buf1 write error"), exit(-1);

 if (lseek(fd, 40, SEEK_SET) ==-1) 
  perror("lseek error"), exit(-1);

 if (write(fd, buf2,10) != 10)
  perror("buf2 write error"), exit(-1);

 exit(0);
}
