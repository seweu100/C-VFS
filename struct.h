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

int read_files(struct datei *strucarr, unsigned long s, FILE *fp){
	if(fp==NULL)
		exitc(2,"Can't open structure file!");
			
	long lSize=get_file_size(fp);
	char *buffer;
	int i=0;
	char name[256];
	unsigned long size=0;
	unsigned long pos=0;
	char *carr;

	// speicher für buffer reservieren
	buffer = calloc (1, sizeof(char)*lSize+1);
	
	if (buffer == NULL)
		exitc(2,"Can't alloc mem!");

	read_file_2_buffer(buffer,fp,0,-1);

	// in Zeilen aufspalten	
	carr=strtok(buffer,"\n");
	

	for(i;i<s;i++){
		carr = strtok (NULL, "\n");
		sscanf(carr,"%lu:%lu:%s",&size,&pos,&name);
		strucarr[i].size = size;
		strucarr[i].pos	= pos;
		strcpy(strucarr[i].name,name);
	}
	
	// aufräumen
	free (buffer);
	rewind( fp );					
	return 0;

}

int read_head(FILE *fp)				
{	
	if(fp==NULL)
		exitc(2,"Can't open structure file!");	

	unsigned long block_size=0;
	unsigned long block_cnt=0;
	unsigned long file_cnt=0;
	
	rewind(fp);
	fscanf(fp,"Size: %lux%lu Bytes - Files: %lu",&block_cnt,&block_size,&file_cnt);
	rewind(fp);
	arch.block_size=block_size;
	arch.block_cnt=block_cnt;
	arch.file_cnt=file_cnt;
	arch.size=block_cnt*block_size;
	
	return 0;
}

unsigned long used_size(FILE *fp){
	unsigned long used=0;
	struct datei filearr[arch.file_cnt];
	read_files(filearr,arch.file_cnt,fp);
	for(int i=0;i<arch.file_cnt;i++)	used=used+filearr[i].size;
	return used;
}

int add_2_struct(FILE *fp, unsigned long size, unsigned long pos,char *name)				
{	
	if(fp==NULL)	exitc(2,"Can't open structure file!");	
	rewind(fp);
	fprintf(fp,"Size: %lux%lu Bytes - Files: %lu",arch.block_cnt,arch.block_size,arch.file_cnt);
	fseek(fp,0,SEEK_END);
	fprintf(fp,"\n%010lu:%010lu:%s",size,pos,name);
	
	return 0;
}

void upd_struct(FILE *fpstruc,struct datei *filearr){
	fprintf(fpstruc,"Size: %lux%lu Bytes - Files: %lu",arch.block_cnt,arch.block_size,arch.file_cnt);
	qsort (filearr, arch.file_cnt, sizeof (struct datei), critter_cmp);
	for(int u=0;u<arch.file_cnt;u++)
		fprintf(fpstruc,"\n%010lu:%010lu:%s",filearr[u].size,filearr[u].pos,filearr[u].name);
}
