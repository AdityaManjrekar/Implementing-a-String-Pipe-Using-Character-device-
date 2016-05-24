#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<assert.h>
#include<string.h>
#include <time.h>
#define BUFFER_SIZE 10
#define ENDING_CONDITION 1
static void *randomString(char*, int);
int main()
{
	char buf[BUFFER_SIZE];
	char buffer[BUFFER_SIZE];
	int i=0;
	int fp=0;
	int errorFlag=0;
	int producerNumber=0;
	fp = open("/dev/mypipe",O_WRONLY);
	 srand(time(NULL));
	if(fp<0)
		printf("Device not opened succesfully\n");
	else
	{
		while(i<ENDING_CONDITION)
		{
				printf("Device Opened succesfully\n");	
				randomString(buf,BUFFER_SIZE-3);
	      producerNumber = rand() % 100;
	      sprintf(buffer,"%s_%d",buf,producerNumber);
				errorFlag = write(fp,buffer,1);
				printf("Producer generated string %s\n",buffer);
				if(errorFlag<0)
					printf("write not worked \n");
				else
					printf("Write worked\n");	
				sleep(3);
				i++;
		}
	}
	close(fp);
	return 0;
}	
static void *randomString(char *stringTemp, int size)
{
	const char characterArray[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int n=0;
  int index=0;
  for (n = 0; n < size; n++) 
  {
    index = rand() % (int)(sizeof(characterArray) - 1);
    stringTemp[n] = characterArray[index];
  }
  stringTemp[size] = '\0';
}
