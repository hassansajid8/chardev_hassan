#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#define DRIVER "/dev/hassan0"
#define WRITE_CALL _IOW('a', 'b', struct Data)

struct Data {
      int length;
      char data[256];
};

int main(void)
{
      int fd = open(DRIVER, O_WRONLY);
      if(fd < 0){
            perror("open");
            return 1;
      }

      struct Data d;
      d.length = 3;

      strcpy(d.data, "xyz");
      int ret = ioctl(fd, WRITE_CALL, &d);
      if(ret < 0){
            perror("ioctl");
      }

      printf("WRITE_CALL returned %d\n", ret);
      close(fd);
      return ret;
}
