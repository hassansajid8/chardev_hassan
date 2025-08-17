#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define DRIVER "/dev/hassan0"
#define SET_SIZE_CALL _IOW('a', 'a', int *)

int main(void)
{
      int fd = open(DRIVER, O_WRONLY);
      if(fd < 0){
            perror("open");
            return 1;
      }

      int size = 100;
      int ret = ioctl(fd, SET_SIZE_CALL, &size);
      if(ret < 0)
            perror("ioctl");

      printf("SET_SIZE_CALL returned %d\n", ret);
      close(fd);
      return ret;
}
