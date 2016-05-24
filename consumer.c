#include<stdio.h>
#include<fcntl.h>
#include<assert.h>
#include<string.h>
#define ENDING_CONDITION 1
int main()
{
	char buf[100];
	int i=0;
	int errorFlag = 0;
	int fp = open("/dev/mypipe",O_RDONLY);
	if(fp<0)
		printf("device not opened succesfully\n");
	else
	{
		while(i<ENDING_CONDITION)
		{
			errorFlag = read(fp,buf,1);
			if(errorFlag<0)
				printf("Read not worked\n");	
			else
			{				
				printf("String returned  by the char device driver %s\n",buf);
				i++;
			}
		}
	}
		close(fp);
	return 0;
}	
