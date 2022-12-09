
//-----------------functions in this file-----------------
// mkdir,kmkdir,tokenize_dir,enter_name,creat

//Look over and test all the new functions...
//Deallocating the wrong inode will give 9 after next

char gpaths[128];
char *names[64];
int numbers;

int tokenize_dir(char* path)
{
   int i;
  char *s;

  strcpy(gpaths, path);
  numbers = 0;

  s = strtok(gpaths, "/");
  while(s){
    names[numbers] = s;
    numbers++;
    s = strtok(0, "/");
  }
  names[numbers] = 0;
}

//split pathname and directory to be created...
int mkdirs(char* pathname)
{
	tokenize_dir(pathname);
	pathname[strlen(pathname)-strlen(names[numbers-1])-1] = 0;//look over
	//	/a/b/c  dirname: /a/b    basename: c
	int pino = getino(pathname);//will where the directory a/b is not c because of the -2
	MINODE* pmip;
	// find the dirname (/a/b of /a/b/c)
	if(numbers == 1)//only the one
	{
		pmip = running->cwd;
	}
	else if (pino != 0)
	{
	  	pmip = iget(dev,pino);//pathname
	}
	else
	{
		printf("Not found\n");
		return;
	}
	
	//Not a directory
	if(!S_ISDIR(pmip->INODE.i_mode))
	{
		printf("Not a dir\n");
		return;
	}
	if(search(pmip,names[numbers-1]))
	{
		printf("Already created\n");
		return;
	}
	
	kmkdir(pmip,names[numbers-1]); // creates the DIR
	pmip->INODE.i_links_count++;
	pmip->dirty = 1;
	pmip->INODE.i_atime = pmip->INODE.i_ctime = pmip->INODE.i_mtime = time(0L);//reset the
	if(pmip != running->cwd) 
	{
		iput(pmip); // puts inode back disk
	}
}


int kmkdir(MINODE* pmip,char* base)
{
	int ino = ialloc(dev);
	int blk = balloc(dev);
	MINODE* mip = iget(dev,ino);//get the new inode
	INODE* ip = &mip->INODE;//pointer to the inode in the block
	ip->i_mode = 0x41ed; // 040775: DIR type and permissions 	
	ip->i_uid = running->uid; // owner uid
	ip->i_gid = running->gid; // group id
	ip->i_size = BLKSIZE; // size in bytes
	ip->i_links_count = 2; // links count 2 because of . and ..
	time_t seconds = time(0L);
	ip->i_atime = ip->i_ctime = ip->i_mtime = seconds;
	ip->i_blocks = 2; // LINUX: Blocks count in 512-byte chunks
	ip->i_block[0] = blk; // new DIR has one data block
	for(int t = 1; t < 15; t++){ // from 1 -> 14 set block to 0
	ip->i_block[t] = 0;
	}
	mip->dirty = 1; // mark minode dirty
	iput(mip);//write INODE back to disk
	//now for the dir to contain the .. and . values
	char buf[BLKSIZE];
	bzero(buf,BLKSIZE); // clears the buf to zero
	DIR* dp = (DIR *)buf;
	// make . entry
	dp->inode = ino;//the current directory
	dp->rec_len = 12;
	dp->name_len = 1;
	dp->name[0] = '.';
	// make .. entry
	dp = (char *)dp + 12;
	dp->inode = pmip->ino;//parent
	dp->rec_len = BLKSIZE -12; // rec_len spans block
	dp->name_len = 2;
	dp->name[0] = dp->name[1] = '.';
	put_block(dev,blk,buf);//write the block to the data (write to disk)
	enter_name(pmip, ino,base);//enter the name into the parent directory
}

int enter_name(MINODE* pip, int ino, char* name)
{
	char buf[BLKSIZE];
	bzero(buf,BLKSIZE);
	get_block(pip->dev,pip->INODE.i_block[0],buf);
	//got the block of the parent
	DIR* dp = (DIR *) buf;
	char* cp = buf;
	while(cp + dp->rec_len < buf + BLKSIZE) // KC code, finds last record
	{
		cp += dp->rec_len;
		dp = (DIR *) cp;
		printf("%d \n",dp->rec_len);
	}
	//dp points at the last record (last entry in the block)
	int ideal_length = 0;
	int need_length  = 0;
	// We are only worrying about inode block 0, if we were however at the end of a block,
	// and ideal length is less than need length we would need to allocate a new block
 	 ideal_length = (8 + dp->name_len + 3)/4;//ideal length of last entry (lowest possible value we can use in a slot)
	 need_length = (8 + strlen(name) + 3)/4;//what needs to be added (how much new file will need)
	 ideal_length *= 4;
	 need_length *=4;
	int remain = dp->rec_len - ideal_length; // last entry len - ideal len
	dp->rec_len = ideal_length;//last one needs to be this value for ideal
	if(remain >= need_length)//needed to place the new last
	{
		dp = (char *) dp + ideal_length;
		dp->inode = ino;//child
		dp->rec_len = remain;//remains
		dp->name_len = strlen(name);//length of string
		strcpy(dp->name,name);
	}
	put_block(pip->dev,pip->INODE.i_block[0],buf); // writes back to the disk with the new file/dir added to parent
}

int creats(char *pathname)
{
	tokenize_dir(pathname);
	pathname[strlen(pathname)-strlen(names[numbers-1])-1] = 0;//look over
	//	a/b/c
	int pino = getino(pathname);//will where the directory a/b is not c because of the -2
	MINODE* pmip;
	if(numbers == 1)//only the one
	{
		pmip = running->cwd;
	}
	else if (pino != 0)
	{
	  	pmip = iget(dev,pino);//pathname
	}
	else
	{
		printf("Not found\n");
		return;
	}
	
	//Not a directory
	if(!S_ISDIR(pmip->INODE.i_mode))
	{
		printf("Not a dir\n");
		return;
	}
	if(search(pmip,names[numbers-1]))
	{
		printf("Already created\n");
		return;
	}
	
	kcreat(pmip,names[numbers-1]);
	pmip->dirty = 1;
	pmip->INODE.i_atime = pmip->INODE.i_ctime = pmip->INODE.i_mtime = time(0L);//reset the
	if(pmip != running->cwd)
	{
		iput(pmip);
	}
	
}

int kcreat(MINODE* pmip, char* base)
{
	int ino = ialloc(dev);
	MINODE* mip = iget(dev,ino);//get the new inode
	INODE* ip = &mip->INODE;//pointer to the inode in the block
	ip->i_mode = 0644 + 040000 + 040000;
	ip->i_uid = running->uid;
	for(int blocks = 0; blocks < 15; blocks++)
	{
		ip->i_block[blocks] = 0;
	}
	ip->i_gid = running->gid;
	ip->i_size = 0;
	ip->i_links_count = 1;
	time_t seconds = time(0L);
	ip->i_atime = ip->i_ctime = ip->i_mtime = seconds;
	mip->dirty = 1;
	iput(mip);//write back to disk
	//now for the dir to contain the .. and . values
	enter_name(pmip, ino,base);//enter the name into the parent directory
}
