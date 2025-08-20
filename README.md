# A sample character device driver for the linux kernel

- The driver implements a dynamic circular queue that can be updated using ioctl calls from userpace.
- Three ioctl calls for initialisation, enqueue (write) and dequeue (read) are defined.
- Program ensures thread safety by locking resources. 
- Showcases blocking behaviour using a wait queue.

### Instructions

- Compile the driver program with make

- Register the device driver using insmod command 
```bash
sudo insmod hassan.ko 
```

- Verify the registration by checking in /proc/devices file
```bash 
grep hassan /proc/devices 
```

- Manually create a device using mknod command
```bash
sudo mknod /dev/hassan0 c <major_number> 0 
```

- tests/ directory contains userspace programs for testing ioctl calls. Compile the programs using make
    - configurator.c initializes queue with capacity of 100
    - filler.c enqueues an item onto the queue 
    - reader.c dequeues and prints the dequeued item 

- to unregister the device, use rmmod command
```bash
sudo rmmod hassan
```


