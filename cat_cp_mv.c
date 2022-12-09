
//Files contained here:
//cat cp mv

int my_cat(char* filename) {
    char mybuf[1024], dummy = 0;  // a null char at end of mybuf[ ]
    int n;

    int fd = my_open(filename, 0); //int fd = open filename for READ;

    while( n = my_read(fd, mybuf, 1024)){
        mybuf[n] = 0;             // as a null terminated string
        printf("%s",mybuf);   //<=== THIS works but not good
       //spit out chars from mybuf[ ] but handle \n properly;

    }
    printf("\n");
    my_close(fd);

    return 0;
}

//from <fcntl.h>:
//O_RDONLY = 0
//O_WRONLY = 1
//O_CREAT = 64
//O_WRONLY|O_CREAT = 65
int my_cp(char* src, char* dst) {  // src is source, dst is destination
    
    int sfile = getino(src);//get the src file
	if(sfile == 0)//Doesn't exist
	{
		printf("Error, source file does not exist\n");
		return;
	}

    int dfile = getino(dst);//get the dst
	if(dfile == 0)//Doesn't exist
	{
		creats(dst);
	}
    
    int fd = my_open(src, 0); // fd = open src for READ
    int gd = my_open(dst, 1);
    int n;
    char mybuf[BLKSIZE]; //went with 1024, unsure if another number would fit better

    while( n = my_read(fd, mybuf, BLKSIZE)){
        my_write(gd, mybuf, n);
    }

    my_close(fd);
    my_close(gd);
    return 0;
}

int my_mv(char* src, char* dst) {

    int srcfile = getino(src);//get the src file

    //verify that src exists
	if(srcfile == 0)//Doesn't exist
	{
		printf("Error, source file does not exist\n");
		return;
	}

    int srcDev = dev;

    MINODE* file = iget(dev,srcfile);//get the INODE

    if (srcDev == dev) {
        links(src, dst);
        unlinks(src);
        //unlink
    } else {
        my_cp(src, dst);
        unlinks(src);
    }
}