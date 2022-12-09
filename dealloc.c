
int clr_bits(char* buf,int bit)
{
	buf[bit/8] &= ~(1 << (bit%8));
}

int incFreeInodes(int dev)
{
	char buf[BLKSIZE];
	get_block(dev,1,buf);
	sp = (SUPER *)buf;
	sp->s_free_inodes_count++;
	put_block(dev,1,buf);
	get_block(dev,2,buf);
	gp = (GD *)buf;
	gp->bg_free_inodes_count++;
	put_block(dev,2,buf);
}
//Look Over
int idalloc(int dev,int ino)
{
	int i;
	char buf[BLKSIZE];
	get_block(dev,imap,buf);
	clr_bits(buf,ino-1);
	put_block(dev,imap,buf);
	incFreeInodes(dev);
}

int incFreeBlocks(int dev)
{
	char buf[BLKSIZE];
	get_block(dev,1,buf);
	sp = (SUPER *)buf;
	sp->s_free_blocks_count++;
	put_block(dev,1,buf);
	
	get_block(dev,2,buf);
	gp = (GD *)buf;
	gp->bg_free_blocks_count++;
	put_block(dev,2,buf);
}
//Look Over
int bdalloc(int dev, int bno)
{
	int i;
	char buf[BLKSIZE];
	get_block(dev,bmap,buf);
	clr_bits(buf,bno-1);
	put_block(dev,bmap,buf);
	incFreeBlocks(dev);
}




