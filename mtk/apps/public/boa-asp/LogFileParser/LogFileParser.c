#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LOG_LV_EMERGENCY		"[Emergency]"
#define LOG_LV_ALERT 			"[Alert]"
#define LOG_LV_CRITICAL			"[Critical]"
#define LOG_LV_ERROR			"[Error]"
#define LOG_LV_WARNING			"[Warning]"
#define LOG_LV_NOTICE			"[Notice]"
#define LOG_LV_INFORMATIONAL		"[Informational]"
#define LOG_LV_DEBUG			"[Debug]"
#define LOG_FILE_PATH			"/var/log/currLogFile"

int isDisplayCurLine(char str[],int level)
{
	
	switch(level)
	{
		case 0:
		{
			if (strstr(str,LOG_LV_EMERGENCY)) 
				return 1;
				break;

		}
		case 1:
		{
			if (strstr(str,LOG_LV_EMERGENCY) || strstr(str,LOG_LV_ALERT)) 
				return 1;
				break;

		}
		case 2:
		{
			if (strstr(str,LOG_LV_EMERGENCY) || strstr(str,LOG_LV_ALERT) ||
			    strstr(str,LOG_LV_CRITICAL))
				return 1;
				break;
		}
		case 3:
		{
			if (strstr(str,LOG_LV_EMERGENCY) || strstr(str,LOG_LV_ALERT) ||
			    strstr(str,LOG_LV_CRITICAL)  || strstr(str,LOG_LV_ERROR))
				return 1;
				break;
		}
		case 4:
		{
			if (strstr(str,LOG_LV_EMERGENCY) || strstr(str,LOG_LV_ALERT) ||
			    strstr(str,LOG_LV_CRITICAL)  || strstr(str,LOG_LV_ERROR) ||
			    strstr(str,LOG_LV_WARNING))
				return 1;
				break;
		}
		case 5:
		{
			if (strstr(str,LOG_LV_EMERGENCY) || strstr(str,LOG_LV_ALERT) ||
			    strstr(str,LOG_LV_CRITICAL)  || strstr(str,LOG_LV_ERROR) ||
			    strstr(str,LOG_LV_WARNING)   || strstr(str,LOG_LV_NOTICE))
				return 1;
				break;
		}
		case 6:
		{
			if (strstr(str,LOG_LV_EMERGENCY) || strstr(str,LOG_LV_ALERT)  ||
			    strstr(str,LOG_LV_CRITICAL)  || strstr(str,LOG_LV_ERROR)  ||
			    strstr(str,LOG_LV_WARNING)   || strstr(str,LOG_LV_NOTICE) ||
			    strstr(str,LOG_LV_INFORMATIONAL))
				return 1;
				break;
		}
		case 7:
		{
			if (strstr(str,LOG_LV_EMERGENCY) || strstr(str,LOG_LV_ALERT)  ||
			    strstr(str,LOG_LV_CRITICAL)  || strstr(str,LOG_LV_ERROR)  ||
			    strstr(str,LOG_LV_WARNING)   || strstr(str,LOG_LV_NOTICE) ||
			    strstr(str,LOG_LV_DEBUG)     || strstr(str,LOG_LV_INFORMATIONAL))
				return 1;
				break;
		}
	}
	return 0;
}


int main(int argc ,char* argv[])
{
	if(argc != 2) 
	{
		printf("Input parm error!\n");
		return 0;
	}
	
	FILE *fp = NULL;
	char buf[512];
	fp=fopen(LOG_FILE_PATH, "r");

	if(fp != NULL)
	{
		while(fgets(buf,512,fp))
		{
			if(isDisplayCurLine(buf,atoi(argv[1])))
			{
#if !defined(TCSUPPORT_CT_NEWGUI) 
				printf("%s", buf);
#endif
	}
		}
	}
	else
	{
		return 0;
	}

	fclose(fp);
	return 1;
}
