
//Read from the files...

int get_min(int one,int two,int three)
{
	if(one <= two && one <= three)
	{
		return one;
	}
	else if (two <= one && two <= three)
	{
		return two;
	}	
	return three;
}



int my_read(int fd, char* buf, int nbytes)
{
	OFT* place = running->fd[fd];
	if(place == 0)
	{
		printf("Nothing to read\n");
		return;
	}
	//Assume open for read mode
	MINODE* mip = place->minodePtr;
	int count = 0, avail = mip->INODE.i_size - place->offset;
	char *cq = buf;
	int lbk = 0, startByte = 0, blk = 0;
	char readbuf[BLKSIZE];
	int indirect[256];
	
	while(nbytes && avail)
	{
		lbk = place->offset / BLKSIZE; // logical block number
		startByte = place->offset % BLKSIZE;
		
		if(lbk < 12) //direct
		{
			blk = mip->INODE.i_block[lbk]; // turn lbk into physical block number
		}
		else if (lbk >= 12 && lbk < 256 + 12) // indirect
		{ 
			get_block(dev,mip->INODE.i_block[12],indirect);//get the block of ints
			blk = indirect[lbk-12];//get the block from the int block
		}
		else // double indirect
		{	
			// mailmans alg
			get_block(dev,mip->INODE.i_block[13],indirect);//get the block of ints from 13
			lbk -= (12 + 256);
			int i = lbk / 256;
			int j = lbk % 256;
			get_block(dev,indirect[i],indirect);
			blk = indirect[j];//get the block from the int block
		
		}
		
		//optimize the reading function....
		get_block(dev,blk,readbuf);
		char* cp = readbuf + startByte;
		int remain = BLKSIZE - startByte;
		
		char copying[BLKSIZE];
		
		while(remain > 0)
		{
			// instead of reading one by one we read the minimun (remain left to read, offset, and nbytes is given)
			// remain is blocksize - startByte
			// startByte = place->offset % BLKSIZE;
			int read_count = get_min(avail, nbytes, remain);
			strncpy(cq,cp,read_count);
			cq += read_count;
			cp += read_count;
			place->offset += read_count;
			count += read_count;
			avail -= read_count;
			nbytes -= read_count ;
			remain -= read_count;
			
			if(nbytes <= 0 || avail <= 0)
			{
				break;
			}	
		
		}
	
	} 
	return count;
}










