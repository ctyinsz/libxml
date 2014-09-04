/************************************************************************
��Ȩ���У����ݺ������ӹɷ����޹�˾
��Ŀ���ƣ�������Ŀ
��    ����V1.0
����ϵͳ��AIX4.2,SCO5
�ļ����ƣ�trace.c
�ļ���������XML���������⸨�����ٺ���
�� Ŀ �飺�м�ҵ���Ʒ�з���
�� �� Ա���м�ҵ���Ʒ�з���
�������ڣ�2001��09��11��
�޸����ڣ�2002��05��12��
************************************************************************/
/**********************************************************************
			�ɸ���ָ���ļ����ȡ�ÿ���ض�ʱ�䡢ÿ���ض����ڡ�
			ÿ���ض����ڡ��ļ���¼��ʱ����Ҫ������Զ�����

 **********************************************************************/

#include	"trace_.h"

#include	<ctype.h>
#include	<string.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<time.h>

#ifdef  SUN
#ifndef __STDC__
#define __STDC__    /* SunOs5.8 stdarg.h*/
#endif
#endif
#include    <stdarg.h>

#include	<sys/types.h>
#include	<sys/stat.h>


#ifdef _WIN32
#include	<direct.h>
#include	<io.h>
#define		FILE_SFLAG	'\\'
#define mkdir_(a,b)	mkdir(a)
#define	snprintf	_snprintf
#else
#include	<dirent.h>
#define		FILE_SFLAG	'/'
#define mkdir_(a,b)	mkdir(a,b)
#endif


#ifdef __cplusplus
extern "C" {
#endif /* end of __cplusplus */

#ifdef SUCC
#undef SUCC
#endif
#ifdef FAIL
#undef FAIL
#endif

#define	SUCC	(0)
#define	FAIL	(-1)


typedef struct wj_file_trace_struct
{ /* �ļ�TRACE�ṹ */
	char	*m_pszFile;			/* ��TRACE���ļ� */
	char	*m_pszBakDir;		/* ת�Ƶı���Ŀ¼ */
	unsigned int m_dwType;		/* ���� */
	unsigned int m_unMaxByte;	/* �������ص�����ֽ��� */
	unsigned int m_unMaxHour;	/* �������ص�Сʱ�� */
	time_t	m_tmDayTime;		/* TRACE�ļ�ÿ��ת��ʱ�� */
	time_t	m_tmWeekDay;		/* TRACE�ļ�ÿ��ת��ʱ�� */
	time_t	m_tmMonthDay;		/* TRACE�ļ�ÿ��ת��ʱ�� */

	FILE	*m_fpOpen;			/* �򿪵��ļ����豸 */
	unsigned char m_unFileIdx;	/* ��ǰ�����ļ�˳��� */
	time_t	m_tmLastMove;		/* ���һ���ƶ���ʱ�� */
}FILETRACE,*PFILETRACE;


/* �ṹ�ڲ����� */
#define	TFA_ALWAYSOPEN	0x00000001 /* �ļ��Ƿ�һֱ�� */
#define	TFA_FLUSH		0x00000002 /* �ļ��Ƿ��޻��壨����ˢ�£�*/
#define	TFA_MAXSIZE		0x00000010 /* �ļ���¼��󳤶� */
#define	TFA_MAXHOUR		0x00000020 /* �ļ���¼���Сʱ�� */
#define	TFA_DAYTIME		0x00000040 /* ��¼�ļ�ÿ��ת��ʱ�� */
#define	TFA_WEEKDAY		0x00000080 /* ��¼�ļ�ÿ��ת��ʱ�� */
#define	TFA_MONTHDAY	0x00000100 /* ��¼�ļ�ÿ��ת��ʱ�� */



/***************************************************************
 *	����:	trc_getidxnum
 *	����:	�ҳ�ָ��Ŀ¼��ָ��ʱ���ض�����TRACE���������ֵ
 *	����:
	- pszDir		TRACE���ڵı���Ŀ¼
	- pszBakDir		TRACE��
	- ptm			ָ��ʱ��ָ��
 *	����ֵ:
	- �ɹ�	����������ֵ
 **************************************************************/
#ifdef _WIN32
static int trc_getidxnum( char *pszDir, char *pszFile, struct tm *ptm )
{
struct _finddata_t  fdata;
long	fd;
char	szDir[128],szFile[128],*p;
int		len;
int		idx,n;

	getcwd(szDir,sizeof(szDir));
	chdir(pszDir);


	if ( p=strrchr(pszFile,'.') )
	{
		len = p-pszFile;
		memcpy(szFile,pszFile,len);
		len+=snprintf(szFile+len,sizeof(szFile)-len,"_%04d%02d%02d_*",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday)-1;
	}
	else
	{
		len=snprintf(szFile,sizeof(szFile),"%s_%04d%02d%02d_*",pszFile,ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday)-1;
	}
	if ( (fd=_findfirst(szFile,&fdata))==-1 )
	{
		chdir(szDir);
		return	0;
	}

	if ( p=strrchr( szFile, FILE_SFLAG ) )
	{
		len = strlen(p+1);
	}

	idx=atoi(&fdata.name[len]);

	while ( !_findnext(fd,&fdata) )
	{
		if ( (n=atoi(&fdata.name[len])) > idx )
			idx=n;
	}
	_findclose(fd);
	chdir(szDir);
	return	idx;
}
#else
static int trc_getidxnum( char *pszDir, char *pszFile, struct tm *ptm )
{
DIR		*dp;
struct dirent *dirp;
int		len,idx,n;
char	szFile[128],*p,*pName;

	idx = 0;

	if ( p=strrchr(pszFile,'.') )
	{
		len = p-pszFile;
		memcpy(szFile,pszFile,len);
		len+=snprintf(szFile+len,sizeof(szFile)-len,"_%04d%02d%02d_",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday);
	}
	else
	{
		len=snprintf(szFile,sizeof(szFile),"%s_%04d%02d%02d_",pszFile,ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday);
	}

	if ( p=strrchr( szFile, FILE_SFLAG ) )
		len = strlen(++p);
	else
		p = szFile;

	if ( !(dp=opendir(pszDir)) )
		return	0;

	while ( dirp=readdir(dp) )
	{
		pName = dirp->d_name;
#ifdef  SUN
		pName = dirp->d_name-2;
#endif
/*		printf("readdir file name=[%s]\n",pName); */
		if ( !memcmp(pName,p,len) )
		{
/*			printf("file name=[%s]\n",pName); */
			if ( (n=atoi(&pName[len]))>idx )
				idx = n;
		}
	}
	closedir(dp);
	return	idx;
}
#endif

/***************************************************************
 *	����:	trc_Build
 *	����:	����TRACE�ṹ
 *	����:
	- pszFile		������־���ļ���
	- pszBakDir	����Ŀ¼
 *	����ֵ:
	- �ɹ�	�����豸���
	- ʧ��	����FAIL
 *	��ע:	ȱʡ����Ϊ�ļ�����,������������
 **************************************************************/
int trc_Build( char* pszFile, char *pszBakDir )
{
PFILETRACE	pTrace;
int		n;
char	*p;
char	szDir[128];

	if ( !pszFile || !*pszFile )
	{
		return	FAIL;
	}

	if ( !(pTrace=(PFILETRACE)calloc(1,sizeof(FILETRACE))) )
	{
		return	FAIL;
	}

	if ( !(pTrace->m_pszFile=strdup(pszFile) ) )
	{
		free(pTrace);
		return	FAIL;
	}

	if ( pszBakDir && *pszBakDir)
	{
		p = pszBakDir;
	}
	else
	{
		getcwd(szDir,sizeof(szDir));
		p = szDir;
	}
	if ( !(pTrace->m_pszBakDir=strdup(p) ) )
	{
		free(pTrace->m_pszFile);
		free(pTrace);
	}
	n = strlen(pTrace->m_pszBakDir);
	if ( pTrace->m_pszBakDir[n]==FILE_SFLAG )
		pTrace->m_pszBakDir[n]='\0';

	/* ���Ŀ¼�Ƿ���ڣ������ڽ��� */
	if ( access(pTrace->m_pszBakDir,0)==-1 )
	{
		if ( mkdir_(pTrace->m_pszBakDir,0700|0050|0005)==-1 )
		{
			free(pTrace->m_pszBakDir);
			free(pTrace->m_pszFile);
			free(pTrace);
			return	FAIL;
		}
	}

	pTrace->m_dwType = TFA_ALWAYSOPEN;
	if ( ! (pTrace->m_fpOpen=fopen(pszFile,"ab")) )
	{
		if ( pTrace->m_pszBakDir )
			free(pTrace->m_pszBakDir);
		free(pTrace->m_pszFile);
		free(pTrace);
		return FAIL;
	}

	pTrace->m_tmLastMove = time(NULL);
	pTrace->m_unFileIdx = trc_getidxnum(pTrace->m_pszBakDir,pTrace->m_pszFile,localtime(&pTrace->m_tmLastMove))+1;
	return	(HTRACE)pTrace;
}


/***************************************************************
 *	����:	trc_Destroy
 *	����:	����TRACE�ṹ
 *	����:
	- hTrace		TRACE�ṹ���
 *	����ֵ:	
	- �ɹ�	����SUCC
	- ʧ��	����FAIL
 **************************************************************/
int trc_Destroy( HTRACE hTrace )
{
PFILETRACE	pTrace;

	if ( !hTrace )
		return	FAIL;

	pTrace = (PFILETRACE)hTrace;

	if ( pTrace->m_pszFile )
	{
		free( pTrace->m_pszFile );
	}

	if ( pTrace->m_pszBakDir )
	{
		free( pTrace->m_pszBakDir );
	}

	if ( pTrace->m_fpOpen )
		fclose( pTrace->m_fpOpen );

	free(pTrace);
	return	SUCC;
}


/***************************************************************
 *	����:	trc_SetAttr
 *	����:	����TRACE�ṹ����
 *	����:
	- hTrace		TRACE�ṹ���
	- iAttrType		�������
		TFS_ALWAYSOPEN	�ļ��Ƿ�һֱ��
			��0, ��;  0,����
		TFS_FLUSH		�ļ��Ƿ��޻��壨����ˢ�£�
			��0, ��;	0,��
		TFS_MAXSIZE		�ļ���¼��󳤶�(���ֽڼ���)
			��Χ 1-4294967295(unsigned int�����ֵ)
		TFS_MAXDAY		�ļ���¼���Сʱ��
			��Χ 1-4294967295(unsigned int�����ֵ)
		TFS_DAYTIME		��¼�ļ�ÿ��ת��ʱ��
			��ʽΪ����hhmmss, ��: 130440��ʾ����1����4��40��
			��Χ 000001-235959
		TFS_WEEKDAY		��¼�ļ�ÿ��ת��ʱ��
			��Χ 1-7, 7��ʾ������
		TFS_MONTHDAY	��¼�ļ�ÿ��ת��ʱ��
			��Χ 1-31
	- uiAttrData	��������
		����ò�����Ϊ0, ������Ӧ�������
		����ò���Ϊ0, �������Ӧ�������
 *	����ֵ:	
	- �ɹ�	����SUCC
	- ʧ��	����FAIL
 **************************************************************/
int trc_SetAttr( HTRACE hTrace, int iAttrType, unsigned int uiAttrData )
{
PFILETRACE	pTrace;
struct tm *ptmWhen;
time_t		tmNow;
   
	if ( !hTrace )
		return	FAIL;

	pTrace = (PFILETRACE)hTrace;

	switch ( iAttrType )
	{
	case	TFS_ALWAYSOPEN: /* �ļ��Ƿ�һֱ�� */
		if ( pTrace->m_dwType&TFA_ALWAYSOPEN )
		{
			if ( uiAttrData )
				break;
			/* �رճ��򿪵����� */
			fclose(pTrace->m_fpOpen);
			pTrace->m_fpOpen=0;
			pTrace->m_dwType&=(~TFA_ALWAYSOPEN);
		}
		else
		{
			if ( !uiAttrData )
				break;
			if ( ! (pTrace->m_fpOpen=fopen(pTrace->m_pszFile,"ab")) )
			{
				return FAIL;
			}
			pTrace->m_dwType|=TFA_ALWAYSOPEN;
		}
		break;

	case	TFS_FLUSH:		/* �ļ��Ƿ��޻��壨����ˢ�£�*/
		if ( pTrace->m_dwType&TFA_FLUSH )
		{
			if ( uiAttrData )
				break;
			pTrace->m_dwType&=(~TFA_FLUSH);
		}
		else
		{
			if ( !uiAttrData )
				break;
			pTrace->m_dwType|=TFA_FLUSH;
		}
		break;

	case	TFS_MAXSIZE:	/* �ļ���¼��󳤶� */
		if ( pTrace->m_dwType&TFA_MAXSIZE )
		{
			if ( uiAttrData )
			{ /* �޸� */
				pTrace->m_unMaxByte=uiAttrData;
			}
			else
			{ /* ��� */
				pTrace->m_dwType&=(~TFA_MAXSIZE);
				pTrace->m_unMaxByte=0;
			}
		}
		else
		{
			if ( !uiAttrData )
				break;
			pTrace->m_unMaxByte = uiAttrData;
			pTrace->m_dwType|=TFA_MAXSIZE;
		}
		break;

	case	TFS_MAXHOUR:		/* �ļ���¼���Сʱ�� */
		if ( pTrace->m_dwType&TFA_MAXHOUR )
		{
			if ( uiAttrData )
			{ /* �޸� */
				pTrace->m_unMaxHour = uiAttrData;
			}
			else
			{ /* ��� */
				pTrace->m_dwType&=(~TFA_MAXHOUR);
				pTrace->m_unMaxHour = 0;
			}
		}
		else
		{
			if ( !uiAttrData )
				break;
			pTrace->m_unMaxHour = uiAttrData;
			pTrace->m_dwType|=TFA_MAXHOUR;
		}
		break;

	case	TFS_DAYTIME:	/* ��¼�ļ�ÿ��ת��ʱ�� */
		if ( uiAttrData>235959 )
			return	FAIL;
		if ( pTrace->m_dwType&TFA_DAYTIME )
		{
			if ( !uiAttrData )
			{ /* ��� */
				pTrace->m_dwType&=(~TFA_DAYTIME);
				pTrace->m_tmDayTime = 0;
				return	SUCC;
			}
		}
		else
		{
			if ( !uiAttrData )
				return	SUCC;
			pTrace->m_dwType|=TFA_DAYTIME;
		}

		/* ����ÿ��ת��ʱ�� */
		tmNow = time(NULL);
		ptmWhen = localtime(&tmNow);
		ptmWhen->tm_hour=uiAttrData/10000;
		ptmWhen->tm_min = uiAttrData%10000/100;
		ptmWhen->tm_sec = uiAttrData%100;
		pTrace->m_tmDayTime = mktime(ptmWhen);
		if ( tmNow > pTrace->m_tmDayTime )
			pTrace->m_tmDayTime+=86400;
		break;

	case	TFS_WEEKDAY:	/* ��¼�ļ�ÿ��ת��ʱ�� */
		if ( uiAttrData>7 )
			return	FAIL;
		if ( pTrace->m_dwType&TFA_WEEKDAY )
		{
			if ( !uiAttrData )
			{ /* ��� */
				pTrace->m_dwType&=(~TFA_WEEKDAY);
				pTrace->m_tmWeekDay = 0;
			}
		}
		else
		{
			if ( !uiAttrData )
				return	SUCC;
			pTrace->m_dwType|=TFA_WEEKDAY;
		}

		/* ����ÿ��ת������ */
		if ( uiAttrData==7 )
			uiAttrData=0;
		tmNow = time(NULL);
		ptmWhen = localtime(&tmNow);
		if ( ptmWhen->tm_wday >= (int)uiAttrData )
			ptmWhen->tm_mday+=7;
		ptmWhen->tm_mday += uiAttrData-ptmWhen->tm_wday;
		ptmWhen->tm_hour = 0;
		ptmWhen->tm_min  = 0;
		ptmWhen->tm_sec  = 0;
		pTrace->m_tmWeekDay = mktime(ptmWhen);
		break;

	case	TFS_MONTHDAY:	/* ��¼�ļ�ÿ��ת��ʱ�� */
		if ( uiAttrData>31 )
			return	FAIL;
		if ( pTrace->m_dwType&TFA_MONTHDAY )
		{
			if ( !uiAttrData )
			{ /* ��� */
				pTrace->m_dwType&=(~TFA_MONTHDAY);
				pTrace->m_tmMonthDay = 0;
			}
		}
		else
		{
			if ( !uiAttrData )
				return	SUCC;
			pTrace->m_dwType|=TFA_MONTHDAY;
		}
		/* ����ÿ��ת������ */
		tmNow = time(NULL);
		ptmWhen = localtime(&tmNow);
		if ( ptmWhen->tm_mday >= (int)uiAttrData )
			ptmWhen->tm_mon++;
		ptmWhen->tm_mday = uiAttrData;
		ptmWhen->tm_hour = 0;
		ptmWhen->tm_min  = 0;
		ptmWhen->tm_sec  = 0;
		pTrace->m_tmMonthDay = mktime(ptmWhen);
		break;

	default:
		return	FAIL;
	}
	return	SUCC;
}


/***************************************************************
 *	����:	trc_GetAttr
 *	����:	��ȡTRACE�ṹ����
 *	����:
	- hTrace		TRACE�ṹ���
	- iAttrType		�������
		TFS_ALWAYSOPEN �ļ��Ƿ�һֱ��
		TFS_FLUSH		�ļ��Ƿ��޻��壨����ˢ�£�
		TFS_MAXSIZE		�ļ���¼��󳤶�
		TFS_MAXDAY		�ļ���¼���Сʱ��
		TFS_DAYTIME		��¼�ļ�ÿ��ת��ʱ��
		TFS_WEEKDAY		��¼�ļ�ÿ��ת��ʱ��
		TFS_MONTHDAY	��¼�ļ�ÿ��ת��ʱ��
	- uiAttrData	������������ָ��
		����ò�������Ϊ0, ��˵��û��������Ӧ�������
 *	����ֵ:	
	- �ɹ�	����SUCC
	- ʧ��	����FAIL
 **************************************************************/
int trc_GetAttr( HTRACE hTrace, int iAttrType, unsigned int *puiAttrData )
{
PFILETRACE	pTrace;
   
	if ( !hTrace && !puiAttrData )
		return	FAIL;

	pTrace = (PFILETRACE)hTrace;

	switch ( iAttrType )
	{
	case	TFS_ALWAYSOPEN: /* �ļ��Ƿ�һֱ�� */
		*puiAttrData = pTrace->m_dwType&TFA_ALWAYSOPEN?1:0;
		break;

	case	TFS_FLUSH:		/* �ļ��Ƿ��޻��壨����ˢ�£�*/
		*puiAttrData = pTrace->m_dwType&TFA_FLUSH?1:0;
		break;

	case	TFS_MAXSIZE:	/* �ļ���¼��󳤶� */
		*puiAttrData = pTrace->m_unMaxByte;
		break;

	case	TFS_MAXHOUR:		/* �ļ���¼���Сʱ�� */
		*puiAttrData = pTrace->m_unMaxHour;
		break;

	case	TFS_DAYTIME:	/* ��¼�ļ�ÿ��ת��ʱ�� */
		{struct tm *ptm;
		ptm = localtime(&pTrace->m_tmMonthDay);
		*puiAttrData = ptm->tm_hour*10000+ptm->tm_min*100+ptm->tm_sec;
		}
		break;

	case	TFS_WEEKDAY:	/* ��¼�ļ�ÿ��ת��ʱ�� */
		if ( !(*puiAttrData=localtime(&pTrace->m_tmWeekDay)->tm_wday) )
			*puiAttrData=7;
		break;

	case	TFS_MONTHDAY:	/* ��¼�ļ�ÿ��ת��ʱ�� */
		*puiAttrData = localtime(&pTrace->m_tmMonthDay)->tm_mday;
		break;

	default:
		return	FAIL;
	}
	return	SUCC;
}

/***************************************************************
 *	����:	trc_MoveFile
 *	����:	ǿ�����TRACE�ļ��ƶ�
 *	����:
	- hTrace		TRACE�ṹ���
 *	����ֵ:	
	- �ɹ�	���ؼ�¼���ֽ���
	- ʧ��	����FAIL
 **************************************************************/
int trc_MoveFile( HTRACE hTrace )
{
PFILETRACE	pTrace;
char	szNewFile[256];
char	*p;
int		n;
time_t	tmNow;
struct tm *ptm;

	if ( !hTrace )
		return	FAIL;
	pTrace = (PFILETRACE)hTrace;

	tmNow=time(NULL);
	if ( tmNow/86400 != pTrace->m_tmLastMove/86400 )
	{
		pTrace->m_tmLastMove = tmNow;
		pTrace->m_unFileIdx = 1;
	}
	ptm = localtime(&tmNow);

	if ( pTrace->m_fpOpen )
		fclose(pTrace->m_fpOpen);

	if (p=strrchr(pTrace->m_pszFile,'.') )
	{
		n = snprintf(szNewFile,sizeof(szNewFile),"%s%c",pTrace->m_pszBakDir,FILE_SFLAG);
		memcpy(szNewFile+n,pTrace->m_pszFile,p-pTrace->m_pszFile);
		n += (p-pTrace->m_pszFile);
		n += snprintf(szNewFile+n,sizeof(szNewFile)-n,"_%04d%02d%02d_%03d",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,pTrace->m_unFileIdx);
		strcpy(szNewFile+n,p);
	}
	else
	{
		p = strrchr( pTrace->m_pszFile, FILE_SFLAG );
		sprintf(szNewFile,"%s%s_%04d%02d%02d_%03d",pTrace->m_pszBakDir,p,ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,pTrace->m_unFileIdx);
	}
	
	rename(pTrace->m_pszFile,szNewFile);

	if ( pTrace->m_dwType&TFA_ALWAYSOPEN )
	{ /* ���ļ� */
		if ( ! (pTrace->m_fpOpen=fopen(pTrace->m_pszFile,"ab")) )
		{
			pTrace->m_fpOpen=0;
			return	FAIL;
		}
	}

	pTrace->m_unFileIdx++;

	return	SUCC;
}

static int vfprintf_( FILE *stream, char *szFormat, va_list pArg );

/***************************************************************
 *	����:	trc_Write
 *	����:	��¼TRACE
 *	����:
	- hTrace		TRACE�ṹ���
	- pszFmt		��¼��ʽ
	- ...			��ʽ��Ӧ����
 *	����ֵ:	
	- �ɹ�	���ؼ�¼���ֽ���
	- ʧ��	����FAIL
 **************************************************************/
int trc_Write( HTRACE hTrace, char *pszFmt, ... )
{
PFILETRACE	pTrace;
FILE	*fp;
struct stat fs;

va_list	pArg;
int		num;
struct tm *ptmWhen;

	if ( !hTrace )
		return	FAIL;

	pTrace = (PFILETRACE)hTrace;

	if ( stat( pTrace->m_pszFile, &fs )==(-1) )
	{
		return	FAIL;
	}

	if ( fs.st_size )
	{
		if ( pTrace->m_dwType&TFA_MAXHOUR
			&& (unsigned int)(time(NULL)-(unsigned int)fs.st_ctime)/3600 > pTrace->m_unMaxHour )
		{ /* �������Сʱ��,�ƶ�TRACE�ļ� */
			trc_MoveFile(hTrace);
		}
		else if ( pTrace->m_dwType&TFA_DAYTIME
			&& time(NULL) > pTrace->m_tmDayTime )
		{ /* �ﵽÿ��ת��ʱ��*/
			pTrace->m_tmDayTime+=86400;
			trc_MoveFile(hTrace);
		}
		else if ( pTrace->m_dwType&TFA_WEEKDAY
			&& time(NULL) > pTrace->m_tmWeekDay )
		{ /* �ﵽÿ��ת��ʱ��*/
			ptmWhen = localtime(&pTrace->m_tmWeekDay);
			ptmWhen->tm_mday+=7;
			pTrace->m_tmWeekDay = mktime(ptmWhen);
			trc_MoveFile(hTrace);
		}
		else if ( pTrace->m_dwType&TFA_MONTHDAY
			&& time(NULL) > pTrace->m_tmMonthDay )
		{ /* �ﵽÿ��ת��ʱ��*/
			ptmWhen = localtime(&pTrace->m_tmMonthDay);
			ptmWhen->tm_mon++;
			pTrace->m_tmMonthDay = mktime(ptmWhen);
			trc_MoveFile(hTrace);
		}

	}

	if ( pTrace->m_fpOpen )
		fp = pTrace->m_fpOpen;
	else if ( !(fp=fopen(pTrace->m_pszFile,"ab")) )
	{
		return	FAIL;
	}

	va_start( pArg, pszFmt );
		num = vfprintf_( fp, pszFmt, pArg );
	va_end( pArg );


	if ( pTrace->m_dwType & TFA_FLUSH )
		fflush(fp);

	if ( !pTrace->m_fpOpen )
		fclose(fp);

	/* ����С */
	if ( pTrace->m_dwType&TFA_MAXSIZE &&
		(unsigned int)fs.st_size+num > pTrace->m_unMaxByte )
	{ /* ��������ֽ���,�ƶ�TRACE�ļ� */
		trc_MoveFile(hTrace);
	}

	return	num;
}









/*************************************************************************************
 *	������	trc_GetVerion
 *	����:	��ȡ�汾��
 *	����:	��
 *	����ֵ: �汾�� 1.0
		���ֽ�Ϊ��汾��
		���ֽ�Ϊ˳���
 *************************************************************************************/
#define MAKELONG_(a, b)      ((unsigned int)(((unsigned short)(a)) | ((unsigned int)((unsigned short)(b))) << 16))
unsigned int trc_GetVerion()
{
	return	MAKELONG_(1,0);
}

/*************************************************************************************
 *	������	trc_GetVerion
 *	����:	��ȡ���汾����
 *	����:	��
 *	����ֵ: �汾���ڴ�, ��ʽΪ "yyyy-mm-dd"
  *************************************************************************************/
char *trc_GetLastVerData()
{
	return	"2001-09-17";
}


typedef struct _HeadFmt_{ /* ��ʽͷ�ṹ */
	char	m_szFmt[64]; /* ͷ��ʽ�ִ�,����sprintf��ʽ�ִ����� */
	int		m_nStartRow; /* ��ʼ��,����sprintf��һ���ɱ���� */
	int		m_nHeadBytes;/* ��ʽ���Ӵ����ͷ�ֽ��� */
}_HEADFMT_,*_PHEADFMT_;

/**********************************************************************
 *	����:	_mkhead_(�ڲ�ʹ��)
 *	����:	�ֽ�ת��ͷ��ַ��ʽ�ִ�
 *	����һ:	����ת�����ʽ��ͷ���ṹָ��
 *	������:	Դ��ַ��ʽ�ִ� [ֱ����ʾ�ַ�][��ַλ��][.��ʼ����][ֱ����ʾ�ַ�]
		NULL	�����ַ
		��:		"[8.0]" 8λ��ַ��ʽ��ͷ��ʼ
 *	����ֵ:	����0
 **********************************************************************/
static int _mkhead_( _PHEADFMT_ pHead, char* psInpData )
{
int	num=0;
int	size=64;
char	*p;

	if ( !pHead )
		return	(-1);

	p = pHead->m_szFmt;
	pHead->m_nStartRow = 0;

	while ( size && *psInpData && ( *psInpData<'0'||*psInpData>'9') ) {
		*p++ = *psInpData++;
		size--;	num++;
	}

	if ( !*psInpData || size<2 ) {
		*p='\0';
		return	num;
	}

	*p++ = '%';
	*p++ = '0';
	size -= 2;
	num += atoi(psInpData);

	while ( size && *psInpData>='0' && *psInpData<='9' ){
		*p++ = *psInpData++;
		size--;
	}
	if ( size ) {
		*p++ ='X';
		size--;
	}

	if ( *psInpData == '.' ) { /* ָ����ʼ��ֵַ */
		psInpData++;
		pHead->m_nStartRow = atoi(psInpData);
		while ( *psInpData>='0' && *psInpData<='9' )
			psInpData++;
	}

	while( size && *psInpData ) {
		*p++ = *psInpData++;
		size--;num++;
	}
	if ( !size ) {
		p--;
		num--;
	}
	*p = '\0';
	pHead->m_nHeadBytes = num;
	return	0;
}

/**********************************************************************
 *	����:	_filedout_(�ڲ�ʹ��)
 *	����:	�� 16 ���ƺ��ַ����ֺϲ���ʽ��ʽ��һ�黺�������ݲ�������ļ���
 *	����һ:	�򿪵��ļ���ָ��
 *	������:	����ָ��
 *	������:	�������ݴ�С
 *	������:	ͷ���ṹָ��
 *	����ֵ:	�ɹ�,����������ֽ���; ʧ��,����-1
 **********************************************************************/
static int _filedout_( FILE *stream, const void *pData, size_t nLen, _PHEADFMT_ pHead )
{
unsigned char *pt,*p;
int	nStep;
int	num;
int	nAddrSt;

	num = 0;
	if ( pHead && pHead->m_nHeadBytes ) {
		nAddrSt = pHead->m_nStartRow<< 4;
		nAddrSt -= 16;
	}

	p = pt = (unsigned char*)pData;
	while ( nLen ) {

		/* ǰ��ַ */
		nStep=16;
		if ( pHead && pHead->m_nHeadBytes )
			num += fprintf( stream, pHead->m_szFmt, nAddrSt+=16 );

		/* ǰ16���ַ� */
		for ( ; nLen && nStep>8; nLen--,nStep--,p++ )
			num += fprintf( stream, "%02X ", *p );

		for ( ; nStep > 8; nStep-- )
			num += fprintf( stream, "   " );

		num += fprintf( stream, " " );

		/* ��16���ַ� */
		for ( ; nLen && nStep; nLen--,nStep--,p++ )
			num += fprintf( stream, "%02X ", *p );

		for ( ; nStep; nStep-- )
			num += fprintf( stream, "   " );

		num += fprintf( stream, "  " );

		/* ����ı��ַ� */
/*		nStep = p-pt; */
		for ( ; pt < p; pt++ ) {
			if( isprint( *pt ) || ( *pt&0x80 ) && p-pt!=1 )
				num += fprintf( stream, "%c", *pt );
			else
				num += fprintf( stream, "." );
		}

/*		if ( nStep==16 ) */
			num += fprintf( stream, "\n" );
			fflush(stream);
	}
/*	fflush( stream ); */
	return	num;
}


/**********************************************************************
 *	����:	_bufoutfile_(�ڲ�ʹ��)
 *	����:	����������ݵ��ļ���
 *	����һ:	�򿪵�����ļ���ָ��
 *	������:	����ָ��
 *	������:	�������ݳ���ָ��
 *	������:	��������
 *	������:	���ݳ���
 *	������:	ͷ���ṹָ��(�����Զ�����)
 *	����ֵ:	����ʵ��������ֽ���
 **********************************************************************/
static int _bufoutfile_( FILE *stream, char *szOut, int *pnOut, char *pData, int nData, _PHEADFMT_ pHead )
{
int	n=0;

	if ( *pnOut ) { /* ��ʣ���ַ� */
		if ( nData+*pnOut <16 ){ /* ���㿽�뻺���� */
			memcpy( szOut+*pnOut, pData, nData );
			*pnOut += nData;
			return	0;
		}
		else {
			int	num = 16-*pnOut;
			memcpy( szOut+*pnOut, pData, num );
			n = _filedout_(stream,pData,16,pHead);
			pHead->m_nStartRow++;
			pData += num;
			nData -= num;
		}
	}
	*pnOut = nData%16;
	if ( nData >= 16 )
		n += _filedout_(stream,pData,nData-*pnOut,pHead);

	memcpy( szOut, pData+(nData-*pnOut), *pnOut );
	pHead->m_nStartRow += (nData/16);

	return	n;
}

/**********************************************************************
 *	����:	vfprintf_
 *	����:	ʹ�ø�ʽ�����������ʽ�����ݵ��ļ���
 *	����һ:	�򿪵�����ļ���ָ��
 *	������:	���ݸ�ʽ�ִ�
 *	������:	��ʽ������ָ��
 *	����ֵ:	�ɹ�,����������ֽ���; ʧ��,����0
 **********************************************************************/
static int vfprintf_( FILE *stream, char *szFormat, va_list pArg )
{
_HEADFMT_ head;
char	szBuf[128];
char	*p,*q;
size_t	n;
char	szOut[16]; /* ��������� */
int		nOut=0;    /* ��������ݸ��� */
int		num=0;     /* ��������ֽ��� */
char	ucHexFlag=0;/* �Ƿ�ʮ������������ */

	p = szFormat;

	while ( *szFormat ) {
		if ( *szFormat != '%' ){
			szFormat++;
			continue;
		}

		if ( szFormat != p ) {
			if ( ucHexFlag ) /* ʮ��������ʽ */
				num += _bufoutfile_( stream, szOut, &nOut, p, szFormat-p, &head );
			else {
				num += (szFormat-p);
				while( p<szFormat )
					fprintf( stream, "%c",*p++ );
			}
		}

		szFormat++;
		switch ( *szFormat ) {
			case	't': /* ����ʱ�� */
			case	'T': {
				struct	tm *ptm;
				time_t	tm;

				tm = va_arg( pArg, time_t );
				if ( !tm )
					time(&tm);
				ptm = localtime( &tm );

				if ( *(szFormat+1)=='\'' ) { /* ��ʼ��ʽ */
					char szTmp[128];
					szFormat += 2;
					q = szTmp; n=sizeof(szTmp)-1;
					while ( *szFormat && *szFormat!='\'' && n ) {
						*q++ = *szFormat++;
						n-- ;
					}
					*q = '\0';
					n = strftime( szBuf,sizeof(szBuf),szTmp,ptm );
				}
				else
					n=snprintf( szBuf,sizeof(szBuf),"%04d-%02d-%02d %02d:%02d:%02d",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec);

				if ( ucHexFlag ) /* ʮ��������ʽ */
					num += _bufoutfile_( stream, szOut, &nOut, szBuf, n, &head );
				else
					num += fprintf( stream,szBuf );
				}
				szFormat++;
				break;
		
			case	'm': /* �ڴ����� */
			case	'M':
				if ( !(p = va_arg( pArg, char* )) )
					break;

				if ( !(n = va_arg( pArg, int )) )
					break;

				if ( ucHexFlag ) /* ʮ��������ʽ */
					num += _bufoutfile_( stream, szOut, &nOut, p, n, &head );
				else
					num += fprintf( stream,p );
				szFormat++;
				break;

			case	's': /* �ַ��� */
			case	'S':
				if ( !(p = va_arg( pArg, char* )) )
					break;

				if ( ucHexFlag ) /* ʮ��������ʽ */
					num += _bufoutfile_( stream, szOut, &nOut, p, strlen(p), &head );
				else
					num += fprintf( stream,p );
				szFormat++;
				break;

			case	'h': /* ��ʼתΪʮ�����Ƶ��Ը�ʽ */
				if ( *(szFormat+1)=='\'' ) { /* ��ʼ��ʽ */
					szFormat += 2;
					q = szBuf; n=sizeof(szBuf)-1;
					while ( *szFormat && *szFormat!='\'' && n ) {
						*q++ = *szFormat++;
						n-- ;
					}
					*q = '\0';
					_mkhead_( &head,szBuf );
				}
				else {
					strcpy(head.m_szFmt,"[%08X]  " );
					head.m_nStartRow  = 0;
					head.m_nHeadBytes = 12;
				}
				ucHexFlag = 1;
				szFormat++;
				break;

			case	'H': /* ����ʮ�����Ƶ��Ը�ʽ */
				if ( !ucHexFlag ) /* ��ʮ��������ʽ */
					break;
				if ( nOut ) { /* ��ʣ���ַ� */
					num += _filedout_(stream,szOut,nOut,&head);
					nOut = 0;
				}
				ucHexFlag = 0;
				szFormat++;
				break;

			default:{
				char szFTmp[64];
				p = szFTmp;
				*p++ = '%';
				n=sizeof(szFTmp)-1;
				while ( *szFormat && *szFormat != '%' && *szFormat != '\\' && n-- )
					*p++ = *szFormat++;
				*p='\0';
				n = snprintf( szBuf,sizeof(szBuf),szFTmp, va_arg(pArg,int ) );

				if ( ucHexFlag ) /* ʮ��������ʽ */
					num += _bufoutfile_( stream, szOut, &nOut, szBuf, n, &head );
				else
					num += fprintf( stream,szBuf );
				}
				break;
		}
		p = szFormat;
	}

	if ( szFormat != p ) {
		n = szFormat - p;
		if ( ucHexFlag ) /* ʮ��������ʽ */
			num += _bufoutfile_( stream, szOut, &nOut, p, n, &head );
		else
			num += fprintf( stream,p );
	}

	if ( nOut ) /* ��ʣ���ַ� */
		num += _filedout_(stream,szOut,nOut,&head);

	return	num;
}


#ifdef __cplusplus
}
#endif /* end of __cplusplus */

