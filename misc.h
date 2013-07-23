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

enum ftype {standard, structure, store};

struct datei
{
	char name[256];
	unsigned long size;
	unsigned long pos;
};

struct archiv
{
	unsigned long block_size;
	unsigned long block_cnt;
	unsigned long file_cnt;
	unsigned long size;
};


void printcmds(){
	printf(	"try: \t ./vfs ARCHIVE OPERATION [PARAMETERS]\n"
			"ARCHIVE \t\t Pfad und Dateiname (ohne Endung) zu den Dateien, die das\n"
			"\t\t\t VFS enthalten/verwalten\n"
			"OPERATION:\n"
			"\t create [BLOCKSIZE] [BLOCKCOUNT] \t ein neues VFS anlegen\n"
			"\t add [SOURCE] [TARGET] \t\t\t eine Datei mit Namen [TARGET] zum VFS hinzufugen\n"
			"\t del [TARGET] \t\t\t\t Datei mit dem Index [TARGET] aus dem VFS loschen\n"
			"\t defrag \t\t\t\t Defragmentierung des VFS\n"
			"\t free \t\t\t\t\t den freien Speicher (in Byte) des VFS ausgeben\n"
			"\t get [SOURCE] [OUTPUT] \t\t\t Datei mit dem Index [SOURCE] aus dem VFS lesen\n"
			"\t list \t\t\t\t\t eine Liste der gespeicherten Dateien ausgeben\n"
			"\t used \t\t\t\t\t den belegten Speicher (in Byte) des VFS ausgeben\n"
			"PARAMETERS \t\t Parameter, die von der ausgewahlten Operation benotigt werden\n");
}

void exitc(int err_code, char *err_msg){
	printf("%s\n",err_msg);	
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
	exit(err_code);
}

int critter_cmp (const void *v1, const void *v2){
	const struct datei *c1 = v1;
	const struct datei *c2 = v2;
	return c1->pos-c2->pos;
}

void check_arg_cnt( int argc, char *argv[] ){
	if(argc<5 && ( !strcmp(argv[2],"create") || !strcmp(argv[2],"add") || !strcmp(argv[2],"get") )){
		printf("%s needs more arguments.\n",argv[2]);
		printcmds();
		exitc(66,"");
	}
	if(argc<4 && !strcmp(argv[2],"delete")){
		printf("%s needs more arguments.\n",argv[2]);
		printcmds();
		exitc(66,"");
	}
		
}
