#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <sys/types.h>
#include <errno.h>
#include	"xml.h"


int main(int argc, char **argv)
{
	struct timeval starttime,endtime;
	HXMLTREE hXml;
	int iret;
	int i,j=0;
	char sbuf[402400]={0};
	char snode1[1024]={0};
	char snode2[1024]={0};
	char sval[1024]={0};
	
	gettimeofday(&starttime,0);
	
	if((hXml = xml_Create("root"))==FAIL )
		exit(-1);
		
	for(i=0;i<100;i++)
	{
		sprintf(snode1,"/prv/node%d|%d",i+1,i+1);
		xml_AddElement(hXml,snode1,NULL);
		for(j=0;j<100;j++)
		{
			sprintf(snode2,"%s/node%d|%d",snode1,j+1,j+1);
			sprintf(sval,"%d,%d",i+1,j+1);
			xml_AddElement(hXml,snode2,sval);
			memset(snode2,0x00,sizeof(snode2));
			memset(sval,0x00,sizeof(sval));
		}
		memset(snode1,0x00,sizeof(snode1));
	}
	iret = xml_ExportXMLString(hXml,sbuf,sizeof(sbuf),NULL,0);
	
	printf("Len:%d\n",iret);
	
	xml_Destroy(hXml);
	
	gettimeofday(&endtime,0);
	
	double timeuse=1000000 * (endtime.tv_sec-starttime.tv_sec)+endtime.tv_usec-starttime.tv_usec;
	
	printf("timeuse:%f\n",timeuse/1000);

	return 0;
}

