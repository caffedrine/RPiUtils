## Create a clone card with DD

```bash
sudo dd if=/dev/sdc of=~/Desktop/raspbian_stretch_xinit.img bs=1M count=3000

```

Assuming that media device have a total of 3.0 GB, tell DD to copy only 3000 blocks of 1MB each