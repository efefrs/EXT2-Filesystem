//Allocating the data and the inodes
//-----------------functions in this file-----------------
// tst_bit,set_bit,decFreeInodes,ialloc,balloc,decFreeBlocks

int tst_bit(char* buf, int bit)
{
	return buf[bit/8] & (1 << (bit%8));

}

int set_bit(char* buf, int bit)
{
	buf[bit/8] |= (1 << (bit % 8));
}


//Keep track of the nodes that are free
int decFreeInodes(int dev)
{
	char buf[BLKSIZE];
	get_block(dev,1,buf);
	sp = (SUPER*)buf;
	sp->s_free_inodes_count--;
	put_block(dev,1,buf);
	
	get_block(dev,2,buf);
	gp = (GD*)buf;
	gp->bg_free_inodes_count--;
	put_block(dev,2,buf);
}

int ialloc(int dev)
{
	int i;
	char buf[BLKSIZE];
	
	get_block(dev,imap,buf);
	for(i=0;i<ninodes;i++)
	{
		if(tst_bit(buf,i) == 0)
		{
			set_bit(buf,i);
			put_block(dev,imap,buf);
			decFreeInodes(dev);
			
			printf("Allocated ino = %d\n",i+1);
			return i+1;//inodes goes from 1 not 0
		}
	}
	
	return 0;
}


int balloc(int dev)
{
	int i;
	char buf[BLKSIZE];
	
	get_block(dev,bmap,buf);
	for(i=0; i<nblocks; i++)
	{
		if(tst_bit(buf,i) == 0)
		{
			set_bit(buf,i);
			put_block(dev,bmap,buf);
			decFreeBlocks(dev);
			
			printf("Allocated block = %d\n",i+1);
			return i+1;//inodes goes from 1 not 0
		}
	}
	
	return 0;
}

//Decrease the amount of free blocks
int decFreeBlocks(int dev)
{
	
	char buf[BLKSIZE];
	get_block(dev,1,buf);
	sp = (SUPER*)buf;
	sp->s_free_blocks_count--;
	put_block(dev,1,buf);
	
	get_block(dev,2,buf);
	gp = (GD*)buf;
	gp->bg_free_blocks_count--;
	put_block(dev,2,buf);
}














