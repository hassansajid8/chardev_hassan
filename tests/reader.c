#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>


#define DRIVER "/dev/hassan0"
#define READ_CALL _IOR('a', 'c', struct Data)

struct Data {
      int length;
      char data[256];
};

int main(void)
{
      int fd = open(DRIVER, O_RDWR);
      if(fd < 0){
            perror("open");
            return 1;
      }

      struct Data d;
      d.length = 3;
      int ret = ioctl(fd, READ_CALL, &d);
      if(ret < 0){
            perror("ioctl");
      }

      printf("READ_CALL returned %d\n", ret);
      printf("data.length = %d\n", d.length);
      printf("data = %s\n", d.data);
      close(fd);
      return ret;
}
