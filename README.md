# EXT2-Filesystem
Linux EXT2 Filesystem that needs to be ran on a linux system. I recommend using wsl or something similar if you need to use a virtual machine.

To run this program you need to use the command ./mk while in the linux terminal
From there you need to ./a.out one of two files:
either ./a.out mydisk OR ./a.out disk2

mydisk is an empty file system while disk2 is a filesystem filled with files of various sizes (good for if you want to test cp, cat, link, symlink, etc)

Regarding the actual program, the code supports a good number of linux commands, a full list can be seen below:

ls
cd
pwd
mkdir
creat
rmdir
link
unlink
symlink
open
close
pfd
read
write
cat
cp
lseek
quit
