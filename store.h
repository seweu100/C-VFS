/*
The MIT License (MIT)

Copyright (c) 2013 Sebastian Weuste

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

unsigned long get_next_free_pos(struct datei *strucarr, unsigned long size){
	if(arch.file_cnt==0)
		return 0;
	if(arch.file_cnt==1)
		return (int)(strucarr[0].size/arch.block_size+1)*arch.block_size;
	
	unsigned long bs=arch.block_size;
	for(int i=0;i<arch.file_cnt-1;i++)
		if(strucarr[i+1].pos-(strucarr[i].pos+(int)(strucarr[i].size/bs+1)*bs)>=(int)(size/bs+1)*size)
			return strucarr[i].pos+(int)(strucarr[i].size/bs+1)*bs;

	return strucarr[arch.file_cnt-1].pos+(int)(strucarr[arch.file_cnt-1].size/bs+1)*bs;
}

void add_2_store(FILE *fpaf, char *store_path, unsigned long nfp, unsigned long fs){
	FILE *fpstore;
	size_t size;
	fpstore=fopen(store_path,"r+b");
	if(errno == ENOENT){
		exitc(2,"Store file does not exist!");
	}
	if(fpaf == NULL){
		exitc(2,"Store file exists but can't read!");
	}
	char *buffer;

	// speicher für buffer reservieren
	buffer = (char*)malloc (sizeof(char)*fs+1);
	if (buffer == NULL)
		exitc(2,"Can't alloc mem!");
	
	size = read_file_2_buffer(buffer,fpaf,0,-1);
	fseek ( fpstore , nfp , SEEK_SET );
	fwrite(buffer,1,size,fpstore);
	printf("%d\n",size);
	fseek ( fpstore , arch.block_cnt*arch.block_size-1, SEEK_SET);
	fputc(EOF, fpstore);
	fclose(fpstore);
	free(buffer);
}
void mv_2_pos(FILE *fpstore,struct datei *filearr,int i,unsigned long pos){
	size_t res;
	size_t size = filearr[i].size;
	char *buffer;
	// speicher für buffer reservieren
	buffer = (char*)malloc (sizeof(char)*filearr[i].size+1);
	
	if (buffer == NULL)
		exitc(2,"Can't alloc mem!");
			
	res = read_file_2_buffer(buffer,fpstore,filearr[i].pos,size);
	fseek ( fpstore , pos , SEEK_SET );
	fwrite( buffer , 1 , size , fpstore );
	free(buffer);
	filearr[i].pos=pos;
}
