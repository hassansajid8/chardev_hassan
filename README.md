# A sample character device driver for the linux kernel
## Submission of assignment for Vicharak LLP

### Submitted by Hassan Sajid

- [Video Submission Link](https://drive.google.com/file/d/1SHpKdeKiMskuXmDgeoscOSiBCd996Mh6/view?usp=drive_link) 
    - *This video does not contain the complete duration of me developing the driver. There was some error with the inbuilt screen recorder in ubuntu and it stopped showing up. I made a lot of changes and fixed a lot of errors after the end of this video. Took me around 3-4 hours total to successfully compile, run and test the driver.*

- The driver implements a dynamic circular queue and ensures thread safety by locking resources. 
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


