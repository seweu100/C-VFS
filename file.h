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

unsigned long count_lines_of_file(FILE *fp) {
	unsigned long line_count = 0;

	if(fp == NULL){
        	fclose(fp);
        	return 0;
	}
	if(feof(fp))rewind( fp );
	if(feof(fp))return 0;
	int ch;
	while ((ch=getc(fp))!=EOF)
    		if (ch=='\n')
        		++line_count;
	rewind(fp);
   	return line_count;	
}

long get_file_size(FILE *fp){
	long lSize;
	if (fp==NULL)	exitc(2,"Can't open file!");

	fseek (fp , 0 , SEEK_END);
	lSize = ftell (fp);
	rewind(fp);
	return lSize;	
}

size_t read_file_2_buffer(char *buf, FILE *fp, unsigned long off, long lSize){
	size_t res;
	if(lSize==-1)
		lSize=get_file_size(fp);
	  
	if (fp==NULL){
		free(buf);
		exitc(2,"Can't open file!");
	}

	if(off>0)
		fseek ( fp , off , SEEK_SET );

	// Inhalt in buffer schreiben:
	res = fread (buf,1,lSize,fp);
	
	if (res != lSize){
			printf("res: %u lS: %u\n",res,lSize);
			free(buf);
			fclose(fp);
			exitc(3,"Can't read file!");
	}
	return res;
}

int create_file(char *path, unsigned long blocksize, unsigned long blockcnt,enum ftype type){
	FILE *fp;
	fp = fopen(path, "r");
   	if(errno == ENOENT) {
		errno = ESUCCESS;
		switch(type)
		{
			case standard:
				fp = fopen (path, "wb");
				if(NULL == fp)	return 1;
				break;
			case structure:
				fp = fopen (path, "w");
				if(NULL == fp)	return 1;
				fprintf(fp,"Size: %ux%u Bytes - Files: 0",blockcnt,blocksize);
				break;
			case store:
				fp = fopen (path, "wb");
				if(NULL == fp)	return 1;
				fseek(fp, blockcnt*blocksize-1, SEEK_SET);
				fputc(EOF, fp);
				break; 
		}    
	}
	else{
		printf("File does already exist!\n");
		fclose(fp);
		return 3;
	}
	fclose(fp);
	return 0;
}
