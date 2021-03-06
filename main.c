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

#define VER 0001
#define ESUCCESS 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
#include <errno.h>

#include "misc.h"
struct archiv arch;

#include "file.h"
#include "struct.h"
#include "store.h"


int create_arch(char *archpath,unsigned long int blocksize,unsigned long int blockcnt){
	char fname1[256],fname2[256];
	int ret=0;
	strcpy(fname1,archpath);
	strcat(fname1,".structure");
	strcpy(fname2,archpath);
	strcat(fname2,".store");
	
	ret=create_file(fname1,blocksize,blockcnt,structure);
	if(ret==0)ret=create_file(fname2,blocksize,blockcnt,store);
	if(ret==0)printf("vfs erfolgreich erstellt!\n");
	return ret;
}



int add(char *archpath,char *path, char *name){
	FILE *fpstruc;
	FILE *fpaf;
	char struc_path[256],store_path[256];
	strcpy(struc_path,archpath);
	strcat(struc_path,".structure");
	strcpy(store_path,archpath);
	strcat(store_path,".store");

	fpaf=fopen(path,"r");
	if(errno == ENOENT)
		exitc(13,"File does not exist!");
	
	if(fpaf == NULL)
		exitc(1,"File exists but can't read!");
	

	fpstruc=fopen(struc_path,"r+");
	read_head(fpstruc);

	unsigned long nfp=0;
	unsigned long bs=arch.block_size;
	unsigned long fs=get_file_size(fpaf);
	
	if(arch.file_cnt>0){
		struct datei filearr[arch.file_cnt];
		read_files(filearr,arch.file_cnt,fpstruc);
		qsort (filearr, arch.file_cnt, sizeof (struct datei), critter_cmp);
		for(int i=0;i<arch.file_cnt;i++)
			if(strcmp(filearr[i].name,name)==0)
				exitc(11,"File name does already exist!");
			
		nfp=get_next_free_pos(filearr,fs);
	}
 
	if(nfp+(int)(fs/bs)*bs>=arch.size)
		exitc(12,"Not enough space!");

	add_2_store(fpaf,store_path,nfp,fs);
	arch.file_cnt++;
	add_2_struct(fpstruc,fs,nfp,name);
	fclose(fpaf);
	fclose(fpstruc);
	return 0;
}

int get(char *archpath,int i, char *path){
	FILE *fpstruc;
	FILE *fpgf;
	FILE *fp;
	char struc_path[256],store_path[256];
	strcpy(struc_path,archpath);
	strcat(struc_path,".structure");
	strcpy(store_path,archpath);
	strcat(store_path,".store");

	
	fp=fopen(struc_path,"r");
	read_head(fp);

	if(arch.file_cnt==0 || i<0 || i>=arch.file_cnt){
		fclose(fp);
		exitc(21,"File doesn't exist.");
	}

	
	struct datei filearr[arch.file_cnt];
	read_files(filearr,arch.file_cnt,fp);
	fclose(fp);
	printf("get %s pos %lu size %lu\n",filearr[i].name,filearr[i].pos,filearr[i].size);
	FILE *fpstore;
	fpstore=fopen(store_path,"rb");

	char *buffer;
	// speicher für buffer reservieren
	buffer = (char*)malloc (sizeof(char)*filearr[i].size+1);
	
	if (buffer == NULL)
		exitc(2,"Can't alloc mem!");
		
	size_t res;
	res = read_file_2_buffer(buffer,fpstore,filearr[i].pos,filearr[i].size);
	fclose(fpstore);	
	printf("%d\n",res);

	if(create_file(path,0,0,standard)==1)
		exitc(30,"Couldn't create file!");

	fpgf=fopen(path,"wb");
	fwrite(buffer,filearr[i].size,1,fpgf);
	fclose(fpgf);
	free(buffer);

	return 0;
}

int del(char *archpath,int i){
	FILE *fp;
	char struc_path[256];
	strcpy(struc_path,archpath);
	strcat(struc_path,".structure");

	fp=fopen(struc_path,"r");
	read_head(fp);

	if(arch.file_cnt==0 || i<0 || i>=arch.file_cnt){
		fclose(fp);
		exitc(21,"File doesn't exist.");
	}

	struct datei *filearr;
	filearr=(struct datei*)malloc(sizeof(struct datei)*arch.file_cnt);
	read_files(filearr,arch.file_cnt,fp);
	--arch.file_cnt;
	for(i;i<arch.file_cnt;i++)
		filearr[i]=filearr[i+1];
	fclose(fp);
	fp=fopen(struc_path,"w");
	upd_struct(fp,filearr);
	fclose(fp);
	free(filearr);
	return 0;
}

int defrag(char *archpath){
	FILE *fpstruc;
	FILE *fpstore;
	char struc_path[256],store_path[256];
	strcpy(struc_path,archpath);
	strcat(struc_path,".structure");
	strcpy(store_path,archpath);
	strcat(store_path,".store");
	
	fpstruc=fopen(struc_path,"r+");
	read_head(fpstruc);

	if(arch.file_cnt>0){
		struct datei filearr[arch.file_cnt];
		read_files(filearr,arch.file_cnt,fpstruc);
		qsort (filearr, arch.file_cnt, sizeof (struct datei), critter_cmp);
		fpstore=fopen(store_path,"r+b");
		if(filearr[0].pos!=0)
			mv_2_pos( fpstore , filearr , 0 , 0 );
		unsigned long bs=arch.block_size;
		for(int i=1;i<arch.file_cnt;i++)
			if(filearr[i].pos>(filearr[i-1].pos+(int)(filearr[i-1].size/bs+1)*bs))
				mv_2_pos(fpstore , filearr , i , (filearr[i-1].pos+(int)(filearr[i-1].size/bs+1)*bs) );	
			
		fseek ( fpstore , arch.block_cnt*bs-1, SEEK_SET);
		fputc(EOF, fpstore);
		fclose(fpstore);
		upd_struct(fpstruc,filearr);	
	}
	fclose(fpstruc);
	return 0;
}


unsigned long used(char *archpath){
	unsigned long used=0;
	FILE *fp;
	char struc_path[256];
	strcpy(struc_path,archpath);
	strcat(struc_path,".structure");

	fp=fopen(struc_path,"r");
	read_head(fp);

	if(arch.file_cnt>0)	used=used_size(fp);
	fclose(fp);
	return used;	
}

unsigned long free_bytes(char *archpath){
	FILE *fp;
	char struc_path[256];
	strcpy(struc_path,archpath);
	strcat(struc_path,".structure");

	fp=fopen(struc_path,"r");
	read_head(fp);

	fclose(fp);
	
	return arch.size - used(archpath);	
}

int list(char *archpath){
	FILE *fp;
	char struc_path[256];
	strcpy(struc_path,archpath);
	strcat(struc_path,".structure");

	fp=fopen(struc_path,"r");
	read_head(fp);

	if(arch.file_cnt>0){
		struct datei filearr[arch.file_cnt];
		read_files(filearr,arch.file_cnt,fp);
		unsigned long bs=arch.block_size;
		for(int i=0;i<arch.file_cnt;i++){
			printf("%d %s,%lu,%lu",i,filearr[i].name,filearr[i].size,(int)(filearr[i].size/bs+1));
			for(int u=filearr[i].pos/bs;u<filearr[i].pos/bs+(int)(filearr[i].size/bs+1);u++)
				printf(",%d",u);
			printf("\n");
		}
		//printf("Name%24s%38s|%38s\n","|","Filesize in Bytes","Position in Bytes");
		//for(int i=0;i<arch.file_cnt;i++)
		//	printf("%27s|%38lu|%38lu\n",filearr[i].name,filearr[i].size,filearr[i].pos);
	}

	fclose(fp);
	return 0;
}


int main( int argc, char *argv[] )  
{
   	printf("VFS #%04d\n\n", VER);
	int ret=0;

	if( argc > 2 )
   	{
		check_arg_cnt( argc, argv );
      		if	(strcmp(argv[2],"create")==0)	ret=create_arch(argv[1],atol(argv[3]),atol(argv[4]));
		else if (strcmp(argv[2],"add")==0)	ret=add(argv[1],argv[3],argv[4]);
		else if (strcmp(argv[2],"get")==0)	ret=get(argv[1],atol(argv[3]),argv[4]);
		else if (strcmp(argv[2],"del")==0)	ret=del(argv[1],atol(argv[3]));
		else if (strcmp(argv[2],"defrag")==0)	ret=defrag(argv[1]);
		else if (strcmp(argv[2],"free")==0)	printf("Space left: %lu Bytes\n",free_bytes(argv[1]));
		else if (strcmp(argv[2],"list")==0)	ret=list(argv[1]);
		else if (strcmp(argv[2],"used")==0)	printf("Space used: %lu Bytes\n",used(argv[1]));
		else{
			printf("Unknown operation.\n\n");   
			printcmds();
			ret = 66;
		}
		

   	}
   	else
   	{
      		printf("At least 2 arguments expected!\n\n");
		printcmds();
		ret = 66;
   	}
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
	return ret;
}
