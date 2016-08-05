#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ONCE_SIZE  48
#define FILE1_START  0
#define FILE1_END  1
#define FILE2_END  2
#define FILE_END   3
#define TYPE_32    4
#define TYPE_16    2
#define TYPE_8     1

static unsigned int print_len = TYPE_8;
static unsigned int start_addr = 0;
static unsigned int cmp_lenght = 0;

static void usage(char *app)
{
	printf("%s file1 file2 [type] [start addr] [lenght]\n", app);
	printf("type:1 2 4 for char short int mode,default 1\n");
}

static void print_hex(const char *buf, int type)
{
	unsigned int *p32 = NULL;
	unsigned short *p16 = NULL;
	unsigned char *p8 = NULL;
	int j = 0;

	for(j = 0; j < 16; j += type)
	{
		switch(type)
		{
			case TYPE_8:
				p8 = (unsigned char *) & (buf[j]);
				printf("%02x ", *p8);
				break;

			case TYPE_16:
				p16 = (unsigned short *) & (buf[j]);
				printf("%04x ", *p16);
				break;

			case TYPE_32:
			default:
				p32 = (unsigned int *) & (buf[j]);
				printf("%08x ", *p32);
				break;
		}
	}
}

static void print_char(const char *buf, int type)
{
	char *pchar = NULL;
	int j = 0, m = 0;

	for(j = 0; j < 16; j += type)
	{
		for(m = type - 1; m >= 0; m--)
		{
			pchar = (char *) & (buf[j + m]);

			if(isprint(*pchar))
			{
				printf("%c ", *pchar);
				//exit(0);   /*for test*/
			}
			else
			{
				printf("%c ", ' ');
			}
		}

		printf(" ");
	}
}

static void print_data(int addr, const char *buf1, const char *buf2)
{
	int i = 0;
	unsigned int *pitem = NULL;
	char *pchar = NULL;

	for(i = 0; i < ONCE_SIZE; i += 16)
	{
		printf("0x%08x  ", addr + i);
		print_hex(buf1 + i, print_len);
		printf("        ");
		print_hex(buf2 + i, print_len);
		printf("\n");

		printf("            ");
		print_char(buf1 + i, print_len);
		printf("        ");
		print_char(buf2 + i, print_len);
		printf("\n");
	}
}

static void file_compare(const char *file1, const char *file2)
{
	FILE *fd1 = NULL;
	FILE *fd2 = NULL;
	char buf1[128], buf2[128];
	int loopflag = 0;
	unsigned addr = 0;
	unsigned int len1 = 0, len2 = 0;

	fd1 = fopen(file1, "rb");

	if(NULL == fd1)
	{
		perror(file1);
		exit(-1);
	}

	fd2 = fopen(file2, "rb");

	if(NULL == fd2)
	{
		perror(file2);
		exit(-1);
	}

	if(start_addr > 0)
		addr = start_addr;

	if(0 != fseek(fd1, addr, SEEK_SET))
	{
		printf("%s fseek 0x08x error%", file1, addr);
		exit(-1);
	}

	if(0 != fseek(fd2, addr, SEEK_SET))
	{
		printf("%s fseek 0x08x error%", file2, addr);
		exit(-1);
	}

	while(!loopflag)
	{
		memset(buf1, 0, 128);
		memset(buf2, 0, 128);
		len1 = fread(buf1, 1, ONCE_SIZE, fd1);
		len2 = fread(buf2, 1, ONCE_SIZE, fd2);

		if(len1 < ONCE_SIZE || len2 < ONCE_SIZE)
			loopflag = FILE_END;

		if(len2 > len1)
			loopflag = FILE1_END;

		if(len2 < len1)
			loopflag = FILE2_END;

		if(memcmp(buf1, buf2, ONCE_SIZE))
		{
			print_data(addr, buf1, buf2);
		}
		else
		{
			printf("0x%08x same\n", addr);
		}

		switch(loopflag)
		{
			case FILE1_END:
				printf("file %s is shorter than %s\n", file1, file2);
				break;

			case FILE2_END:
				printf("file %s is larger than %s\n", file1, file2);
				break;

			case FILE_END:
				printf("file %s same size with %s\n", file1, file2);
				break;
		}

		addr += ONCE_SIZE;

		if(cmp_lenght > 0)
		{
			if(addr > (start_addr + cmp_lenght))
				break;
		}
	}

	fclose(fd1);
	fclose(fd2);
}

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		usage(argv[0]);
		exit(-1);
	}

	if(argc > 3)
	{
		print_len = atoi(argv[3]);

		if(print_len != TYPE_8 && print_len != TYPE_16 && print_len != TYPE_32)
		{
			printf("type error\n");
			exit(-1);
		}
	}

	if(argc > 5)
	{
		start_addr = strtoul(argv[4], NULL, 0);
		cmp_lenght = strtoul(argv[5], NULL, 0);
	}

	file_compare(argv[1], argv[2]);

	return 0;
}
