## Create a clone SDcard with DD

#### 1. Identify the SD card:
```bash
$ lsblk
NAME   MAJ:MIN RM   SIZE RO TYPE MOUNTPOINT

sr0     11:0    1  1024M  0 rom  
sdc      8:32   1  28,8G  0 disk 
├─sdc2   8:34   1  28,8G  0 part /media/user/rootfs
└─sdc1   8:33   1  43,1M  0 part /media/user/boot
```

#### 2. Check the size of the SDcard data:

```bash
$ df -h
Filesystem      Size  Used Avail Use% Mounted on
/dev/sdc1        43M   22M   21M  52% /media/user/boot
/dev/sdc2        29G  2,9G   25G  11% /media/user/rootfs
```

#### 3. Resize the SDcard using GParted 

Shrink the card size to it's data size. IF the data have 3.8GB then shring card from it's maximum size to 4Gb


#### 4.0 Use DD utility to copy blocks
```bash
$ sudo dd if=/dev/sdc of=~/Desktop/raspbian_stretch_xinit.img bs=1M count=4000
```

Assuming that media device have a total of 4.0 GB, tell DD to copy only 4000 blocks of 1MB each

#### or

#### 4.1 Use DD utility to copy blocks and show progress

```bash
$ sudo apt-get install pv
$ sudo dd if=/dev/sdc | pv | of=~/Desktop/raspbian_stretch_xinit.img bs=1M count=4000
```