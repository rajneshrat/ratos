// apt-get install libelfg0-dev
#include <err.h>
#include <fcntl.h>
#include <sysexits.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <libelf.h>

int main(int argc, char * argv[]) {
    Elf *elf_file, *elf;
    Elf_Kind  elf_kind_file;
    int file_descriptor;

    if((argc!=2)) 
        printf("file name not found\n");

    file_descriptor = open(argv[1], O_RDONLY, 0);
    if( file_descriptor )
	printf("File is OK\n");
    
   if (elf_version(EV_CURRENT) == EV_NONE) {
		(void) fprintf(stderr,
			"elf_version() failed: %s\n", elf_errmsg(0));
		exit(1);
	}
    elf_file = elf_begin(file_descriptor, ELF_C_READ, NULL);
    if( elf_file )
	printf( "File is ELF\n");
    
    (void) fprintf(stderr, "elf_begin() failed: %s\n",
			    elf_errmsg(0));

    elf = elf_begin(file_descriptor, ELF_C_READ, elf_file);
    //int a = elf32_getehdr(elf);
   // printf("return = %d\n",a);
    if ((elf32_getehdr(elf)) != 0)
   	{
	   printf("Header is OK\n");
   	}
   
     elf_kind_file = elf_kind(elf_file);

    elf_end(elf_file);
    close(file_descriptor);


    return 0;
}

