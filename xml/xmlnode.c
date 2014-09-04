/************************************************************************
��Ȩ���У����ݺ������ӹɷ����޹�˾
��Ŀ���ƣ�������Ŀ
��    ����V1.0
����ϵͳ��AIX4.2,SCO5
�ļ����ƣ�xmlnode.c
�ļ�������XML�ڵ����������XML�ṹ�ڲ�ʹ�ã�
�� Ŀ �飺�м�ҵ���Ʒ�з���
�� �� Ա���м�ҵ���Ʒ�з���
�������ڣ�2001��09��11��
�޸����ڣ�2002��05��12��
************************************************************************/
/*
    �޸ļ�¼��
    Tianhc :����
    sl_NodeClear
    sl_AttrClear
                --�������ָ���ڵ������,���ýڵ㱾��ɾ��,����ͬ���ڵ��衰ռ�����λ��,�����ݲ�Ҫ��
    xn_ExportXMLString_Format
                --���ڡ���ʽ�����������ַ���
    �޸�
      xn_strtokdata
                --���ں�TAB���ո���ַ���XML �ַ�������ʱ�����ڵ������ݺ���  
    ���ӣ�
      xn_Setquot()����,������ʾָ��"����Ϊ&quot;��ʽ��ȱʡ�Ļ�������Ϊ&quote;(�����,���Ǿɵ��ڲ���׼)                      
    �޸�:2006-11-15 22:10
      xn_importXmlString��xn_strtok����,�ײ�ֱ��֧��XML��д��ʽ(<node/>)  
*/


#ifndef __XMLNODE_C__
#define __XMLNODE_C__

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include	"xml.h"
#include  "trace_.h"

#ifdef	_WIN32
#include	<direct.h>
#define		FILE_SFLAG	'\\'
#include	<winsock2.h>
#include	<io.h>
#define		snprintf	_snprintf
#else
#define		FILE_SFLAG	'/'
#include	<dirent.h>
#include	<netinet/in.h>
/* #include	<inttypes.h> */
/* #pragma pack(1) SUN���������� */
#endif

/* #define	_TRACE_LOG_ */

#ifdef          _TRACE_LOG_
#include        "trace_.h"
#endif
#ifdef			_DEBUG_
#include        "debug.h"
#endif

extern int g_xmlErrno;
extern char g_szErrEx[128];
extern int g_xmlquot;

#define		XML_LOGCFGFILE	"XML_LOGCONFIG"

#ifdef __cplusplus
extern "C" {
#endif /* end of __cplusplus */

#define		FAIL	    (-1)
#define     SUCC 	    (0)

#define		LIST_INCRE		8 /* ���α�ȱʡ���Ӵ�С */
#define		SL_ELEMTYPE		void*


struct xml_node;
typedef struct	wj_seq_list{ /* ˳�����Ա�ṹ */
	SL_ELEMTYPE		*m_lpBase;	/* �洢�ռ��ַ */
	unsigned int	m_nLen;     /* ��ǰ���� */
	unsigned int	m_nListSize;/* ǰ����Ĵ洢����(��sizeof(SL_ELEMTYPE)Ϊ��λ */
}WSEQLIST, *PWSEQLIST;

#define	sl_Length(pList)	((pList)->m_nLen)

typedef struct xml_node_attr{ /* �ڵ�Ԫ���������� */
	unsigned char	*m_pszName;	/* ������ */
	unsigned char	*m_pszData;	/* ����ֵ */
}XMLNODEATTR, *PXMLNODEATTR;

typedef struct xml_node{ /* �ڵ�Ԫ�ؽṹ */
	WSEQLIST	m_DownList;	/* �ӽڵ�� */
	WSEQLIST	m_AttrList;	/* ���Ա� */

	unsigned int m_id;		/* �ýڵ�ı�ʶ */
	unsigned char	*m_pszName;		/* �ýڵ������ */ 
	unsigned char	*m_pszData;		/* �ýڵ������ */
	unsigned int m_iDataLen;/* ���ݳ��� */

}XMLNODE, *PXMLNODE;


#ifdef	__XML_TRACE__
#define		XL_INDSR		0x00000001 /* ����DSR���ݺ��� */
#define		XL_TODSR		0x00000002 /* ����DSR���ݺ��� */
#define		XL_INXML		0x00000004 /* ����XML�ִ����� */
#define		XL_TOXML		0x00000008 /* ����XML�ִ����� */
#define		XL_INFMT		0x00000100 /* ����ָ����ʽ�ִ����� */
#define		XL_TOFMT		0x00000200 /* ����ָ����ʽ�ִ����� */
#define		XL_ERROR		0x00000010 /* ��ϸ���� */
#define		XL_DATA			0x00000020 /* ��ϸ���� */
#define		XL_EXCHANGE		0x00000040 /* �������ݺ��� */
#define		XL_NODE			0x00010000 /* �ڵ�������� */
#define		XL_NODEATTR		0x00020000 /* �ڵ����Բ������� */
#define		XL_COUNT		0x00030000 /* ͳ�Ʋ��� */
#define		XL_ALL			0xFFFFFFFF /* ���в��� */
#define		XL_NORMAL		0x0000003F /* ���ò��� */
#endif

typedef struct xml_struct{ /* XML�ṹ */
	PXMLNODE	m_pRoot;	/* ���ڵ� */
#ifdef	__XML_TRACE__
	unsigned int m_dwTrcType;/* TRACE���� */
	int			m_hTrace;	 /* TRACE */
#endif
}XMLSTRUCT,*PXMLSTRUCT;


static void xn_Destroy( PXMLNODE pNode );
static void xna_Destroy( PXMLNODEATTR pAttr );

/*********************************************************
 *	����:	sl_NodeDestroy
 *	����:	����˳�����Ա�
 *	����һ:	˳�����Ա�ָ��
 *	����ֵ:	�ɹ�,����0��ʧ��,����-1
 *********************************************************/
static void sl_NodeDestroy( PWSEQLIST lpList )
{
SL_ELEMTYPE *pNodeP;
unsigned int i;

	if ( lpList->m_nLen )
	{
		pNodeP=lpList->m_lpBase;
		i = lpList->m_nLen;

		for ( ; i; pNodeP++,i-- )
			xn_Destroy(*pNodeP);
	}
  
	if ( lpList->m_lpBase )
		free((void*)lpList->m_lpBase );
}

static void sl_NodeClear( PWSEQLIST lpList )
{
SL_ELEMTYPE *pNodeP;
unsigned int i;

	if ( lpList->m_nLen )
	{
		pNodeP=lpList->m_lpBase;
		i = lpList->m_nLen;

		for ( ; i; pNodeP++,i-- )
			xn_Destroy(*pNodeP);
	}
	if ( lpList->m_lpBase )
		free((void*)lpList->m_lpBase );
  lpList->m_nLen = 0;   /*Tianhc ��������ڵ�����ʱ(��ɾ��)*/
}
/*********************************************************
 *	����:	sl_AttrDestroy
 *	����:	����˳�����Ա�
 *	����һ:	˳�����Ա�ָ��
 *	����ֵ:	�ɹ�,����0��ʧ��,����-1
 *********************************************************/
static void sl_AttrDestroy( PWSEQLIST lpList )
{
SL_ELEMTYPE *pNodeP;
unsigned int i;

	pNodeP=lpList->m_lpBase;
	i = lpList->m_nLen;

	if ( lpList->m_nLen )
	{
		for ( ; i; pNodeP++,i-- )
			xna_Destroy(*pNodeP);
	}

	if ( lpList->m_lpBase )
		free((void*)lpList->m_lpBase );
}

/*********************************************************
 *	����:	sl_AttrClear
 *	����:	����˳�����Ա�,
 *        ��������ַFree,��������ڵ��������������
 *	����һ:	˳�����Ա�ָ��
 *	����ֵ:	�ɹ�,����0��ʧ��,����-1
 *  Tianhc 2005-11-23 20:52
 *********************************************************/
static void sl_AttrClear( PWSEQLIST lpList )
{
SL_ELEMTYPE *pNodeP;
unsigned int i;

	pNodeP=lpList->m_lpBase;
	i = lpList->m_nLen;

	if ( lpList->m_nLen )
	{
		for ( ; i; pNodeP++,i-- )
			xna_Destroy(*pNodeP);
	}

	if ( lpList->m_lpBase )
		free((void*)lpList->m_lpBase );

	lpList->m_nLen = 0;
}

/*********************************************************
 *	����:	sl_OwnAppeMem
 *	����:	���Ӵ洢�ռ�
 *	����һ:	˳�����Ա�ָ��
 *	����ֵ:	�ɹ�,����0;ʧ��,����-1
 *********************************************************/
static int sl_OwnAppeMem( PWSEQLIST lpList, int iAppSize )
{
SL_ELEMTYPE *lpNewBase;

	if ( iAppSize<=0 )
		iAppSize =  LIST_INCRE;

	if ( lpNewBase=(SL_ELEMTYPE*)realloc(lpList->m_lpBase,
		(lpList->m_nListSize+iAppSize)*sizeof(SL_ELEMTYPE)) )
	{
		memset((void*)(lpNewBase+lpList->m_nLen),'\0',iAppSize*sizeof(SL_ELEMTYPE));
		lpList->m_lpBase=lpNewBase;
		lpList->m_nListSize+=iAppSize;
		return	0;
	}

	if ( !(lpNewBase=(SL_ELEMTYPE*)malloc((lpList->m_nListSize+iAppSize)*sizeof(SL_ELEMTYPE)) ) )
	{
		return	(-1);
	}
	memset((void*)(lpNewBase+lpList->m_nLen),'\0',iAppSize*sizeof(SL_ELEMTYPE));
	memcpy((void*)lpNewBase,(void*)lpList->m_lpBase,lpList->m_nLen*sizeof(SL_ELEMTYPE));
	free((void*)lpList->m_lpBase);
	lpList->m_lpBase=lpNewBase;
	lpList->m_nListSize+=iAppSize;
	return	0;
}


/***************************************************************
 *	����:	sl_GetElem
 *	����:	�������Ա�ָ��λ�õ�����Ԫ��
 *	����һ:	˳�����Ա���
 *	������:	ָ��λ��
 *	������:	����ȡ����Ԫ��ָ��
 *	����ֵ:	�ɹ�,����0��ʧ��,����-1
 **************************************************************/
static int sl_GetElem( PWSEQLIST lpList, const int nGetPos,SL_ELEMTYPE* lpElem)
{

	if ( nGetPos< 1 || nGetPos> (int)(lpList->m_nLen) )
	{ /* nPosλ�ò��Ϸ� */
		return (-1);
	}

	memcpy((void*)lpElem,(void*)&(lpList->m_lpBase[nGetPos-1]),sizeof(SL_ELEMTYPE));
	return (0);
}


/***************************************************************
 *	����:	sl_Append
 *	����:	��˳�����Ա�β���һԪ��
 *	����һ:	˳�����Ա�ָ��
 *	������:	����ӵ�Ԫ�ص�ָ��
 *	����ֵ:	�ɹ�,����0;ʧ��,����(-1)
 **************************************************************/
static int sl_Append( PWSEQLIST lpList, SL_ELEMTYPE pApeNode)
{
	if ( lpList->m_nLen >=lpList->m_nListSize )
	{ /* ��ǰ�洢�ռ����������ӷ��� */
		if ( sl_OwnAppeMem(lpList,0)==(-1) ) 
			return (-1);
	}

	lpList->m_lpBase[lpList->m_nLen]=pApeNode;
	++(lpList->m_nLen);
	return (0);
}

/***************************************************************
 *	����:	sl_Delete
 *	����:	ɾ��ָ��λ��Ԫ��
 *	����һ:	˳�����Ա���
 *	������:	ָ��λ��
 *	������:	���ܱ�ɾ����Ԫ��ָ��(����Ҫ�ÿ�)
 *	����ֵ:	�ɹ�,����0��ʧ��,����-1
 **************************************************************/
static int sl_Delete( PWSEQLIST lpList,const int nDelPos,
                       SL_ELEMTYPE* lpElem )
{
SL_ELEMTYPE* lpBase;
SL_ELEMTYPE* p;
SL_ELEMTYPE* q;

	if ( nDelPos< 1 || nDelPos> (int)(lpList->m_nLen) ) /* nPosλ�ò��Ϸ� */
	{
		return (-1);
	}
	lpBase = lpList->m_lpBase;
	p=&(lpBase[nDelPos-1]);
	if ( lpElem )
		memcpy((void*)lpElem,(void*)p,sizeof(SL_ELEMTYPE));
	q=&(lpBase[lpList->m_nLen-1]);
	for ( ++p; p<=q; ++p )
		memcpy((void*)(p-1),(void*)p,sizeof(SL_ELEMTYPE));
	memset((void*)q,'\0',sizeof(SL_ELEMTYPE));
	--(lpList->m_nLen);
   return (0);
}



static unsigned int getstrid( const unsigned char *szStr, unsigned int len )
{
unsigned int data;
unsigned char *p;

	for ( data=0, p=(unsigned char*)szStr; *p&&len; p++,len-- )
		data = 17*data + *p;
/*		data = 17*data + (islower(*p)?*p-32:*p); */

	return	data;
}

static int _memicmp_( unsigned char *p1, unsigned char *p2, size_t n )
{

	for ( ; n; p1++, p2++,n-- ) {
		if ( *p1 == *p2 ||
			 islower(*p1) && ( *p1-*p2==32 ) ||
			 isupper(*p1) && ( *p2-*p1==32 ) )
			continue;
		else
			return	( ( *p1 > *p2 ) ? 1:-1 );
	}
	return	(0);
}

static int _memicmp( unsigned char *p1, unsigned char *p2, size_t n )
{
  if ( *p1 != *p2 )
        return  ( ( *p1 > *p2 ) ? 1:-1 );
  return memcmp(p1,p2,n );

}
/*
static int _memicmp_( const unsigned char *pData1, const unsigned char *pData2, size_t nCount )
{
unsigned char *pBuf1,*pBuf2;

	pBuf1 = (unsigned char*)pData1;
	pBuf2 = (unsigned char*)pData2;

	for ( ; nCount; pBuf1=pBuf1+1, pBuf2 =pBuf2+1,nCount-- ) {
		if ( islower(*pBuf1) && ( *pBuf1-*pBuf2==32 ) )
			continue;
		if ( isupper(*pBuf1) && ( *pBuf2-*pBuf1==32 ) )
			continue;

		if ( *pBuf1 == *pBuf2 )
			continue;
		else
			return	( ( *pBuf1 > *pBuf2 ) ? 1:-1 );
	}
	return	(0);
}
*/
#ifdef	_CAP_
#define	memicmp_	_memicmp_	/* XML�ڵ㲻���ִ�Сд */
#else
#define	memicmp_	_memicmp	/* XML�ڵ����ִ�Сд */
#endif

/***************************************************************
 *	����:	sl_FindAttr
 *	����:	�����Ա��в���ָ����������
 *	����һ:	����˳�����Ա���
 *	������:	�����ҵ���������;
 *	������:	����λ�õ�����ָ��
 *	����ֵ:	�ɹ�,����Ԫ��ָ�룻ʧ��,����NULL
 **************************************************************/
static PXMLNODEATTR sl_FindAttr( PWSEQLIST lpList, unsigned char *szName, int *pnPos )
{
PXMLNODEATTR *lpElemP,*lpEndP;

	if ( !lpList->m_lpBase )
		return	NULL;

	lpEndP = (PXMLNODEATTR*)lpList->m_lpBase+lpList->m_nLen;
	lpElemP = (PXMLNODEATTR*)lpList->m_lpBase;

	for ( ; lpElemP<lpEndP; lpElemP++ )
	{
		if ( !memicmp_((*lpElemP)->m_pszName,szName,strlen((char*)szName)) )
		{
			if ( pnPos )
				*pnPos = ((char*)lpElemP-(char*)lpList->m_lpBase)/sizeof(SL_ELEMTYPE)+1;
			return *lpElemP;
		}
	}
	return	NULL;
}


/***************************************************************
 *	����:	sl_FindNode
 *	����:	�ڽڵ���в���ָ�����ƽڵ�
 *	����һ:	�ڵ�˳�����Ա���
 *	������:	�����ҵĽڵ�����;
 *	������:	���Ƴ����ֽ�;
 *	������:	����,ָ���ڼ��η������������� 0,Ϊ���λ��
 *	������:	����λ�õ�����ָ��
 *	����ֵ:	�ɹ�,����Ԫ��ָ�룻ʧ��,����NULL
 **************************************************************/
static PXMLNODE sl_FindNode( PWSEQLIST lpList,
				unsigned char *pcName, unsigned int len, int nIdx, int *pnPos )
{
PXMLNODE *lpElemP,*lpEndP;
unsigned int	id;

	if ( !lpList->m_lpBase )
		return	NULL;

	id = getstrid( pcName, len );

	if ( nIdx )
	{
		lpEndP=(PXMLNODE*)lpList->m_lpBase+lpList->m_nLen;
		lpElemP = (PXMLNODE*)lpList->m_lpBase;
		for ( ; lpElemP<lpEndP; lpElemP++ )
		{
			if ( id==(*lpElemP)->m_id && !memicmp_((*lpElemP)->m_pszName,pcName,len) )
			{
				nIdx--;
				if ( !nIdx )
				{
					if ( pnPos )
						*pnPos = ((char*)lpElemP-(char*)lpList->m_lpBase)/sizeof(SL_ELEMTYPE)+1;
					return *lpElemP;
				}
			}
		}
	}
	else
	{
		lpElemP=(PXMLNODE*)lpList->m_lpBase+lpList->m_nLen-1;
		lpEndP = (PXMLNODE*)lpList->m_lpBase;
		for ( ; lpElemP>=lpEndP; lpElemP-- )
		{
			if ( id==(*lpElemP)->m_id && !memicmp_((*lpElemP)->m_pszName,pcName,len) )
			{
				if ( pnPos )
					*pnPos = ((char*)lpElemP-(char*)lpList->m_lpBase)/sizeof(SL_ELEMTYPE)+1;
				return *lpElemP;
			}
		}
	}

	return (NULL);
}


/***************************************************************
 *	����:	sl_CountNode
 *	����:	�ڽڵ����ͳ��ָ�����ƽڵ�ĸ���
 *	����һ:	�ڵ�˳�����Ա���
 *	������:	��ʼͳ�Ƶ�λ��
 *	������:	�����ҵĽڵ�����;
 *	������:	�ڵ����Ƴ���
 *	����ֵ:	�ɹ�,����Ԫ��ָ�룻ʧ��,����NULL
 **************************************************************/
static int sl_CountNode( PWSEQLIST lpList, int nStPos, unsigned char *pcName, unsigned int len )
{
PXMLNODE *lpElemP,*lpEndP;
int		num;
unsigned id;

	id = getstrid( pcName, len );

	lpEndP=(PXMLNODE*)lpList->m_lpBase+lpList->m_nLen;
	lpElemP = (PXMLNODE*)lpList->m_lpBase+nStPos;
	for ( num=0; lpElemP<lpEndP; lpElemP++ )
	{
		if ( id==(*lpElemP)->m_id && !memicmp_((*lpElemP)->m_pszName,pcName,len) )
		{
			num++;
		}
	}

	return	num;
}


static int xn_explain( unsigned char *szBuf, unsigned char *pStr, unsigned int len );

/**********************************************************************
 *	������	xna_Build
 *	����:	����һ���ڵ�����
 *	����:
	szName
		���Ե����ơ�
	len
		�������Ƴ���
	szData
		����ֵ��
 *	����ֵ:
	 �ɹ�	���ع���Ľڵ�����ָ��
	 ʧ��	����NULL
 **********************************************************************/
static PXMLNODEATTR xna_Build( const unsigned char *szName, const int len, const unsigned char *szAttr )
{
PXMLNODEATTR	pAttr;
unsigned int	n;
	if ( !szName || !*szName || !len )
		return	NULL;

	if ( !(pAttr=(PXMLNODEATTR)calloc(1,sizeof(XMLNODEATTR))) )
	{
		return	NULL;
	}

	if ( szName )
	{
		if ( !(pAttr->m_pszName=(unsigned char*)malloc(len+1)) )
		{
			free(pAttr);
			return	NULL;
		}
		memcpy(pAttr->m_pszName,szName,len);
		pAttr->m_pszName[len]='\0';
	}

	if ( szAttr )
	{
		n = strlen((char*)szAttr);
		if ( !(pAttr->m_pszData=(unsigned char*)malloc(n+1)) )
		{
			free(pAttr->m_pszName);
			free(pAttr);
			return	NULL;
		}
/*		xn_explain( pAttr->m_pszData, (unsigned char*)szAttr, n );*/
		memcpy(pAttr->m_pszData,szAttr,n); 
		*(pAttr->m_pszData+n)='\0';
	}

	return	pAttr;
}

/**********************************************************************
 *	������	xna_Destroy
 *	����:	����һ������
 *	����:
	pAttr
          �ڵ����Խṹָ�롣
 *	����ֵ:	 ��
 **********************************************************************/
static void xna_Destroy( PXMLNODEATTR pAttr )
{
	if ( pAttr->m_pszData )
		free(pAttr->m_pszData);
	if ( pAttr->m_pszName )
		free(pAttr->m_pszName);

	free(pAttr);
}



/**********************************************************************
 *	������	xn_Build
 *	����:	����һ��Ԫ�ؽڵ�
 *	����:
	 szName
		Ԫ�ص����ơ�
	 uiNameLen
		Ԫ�����Ƶĳ��ȡ�
	 szData
		Ԫ��ֵ��
	 uiDataLen
		Ԫ��ֵ�ĳ��ȡ�
 *	����ֵ:
	 �ɹ�	���ع���Ľڵ�ָ��
	 ʧ��	����NULL
	 �޸ļ�¼��
	    ������ת�崦��, Tianhc 2009-12-29 9:46:31
 **********************************************************************/
static PXMLNODE xn_Build( const unsigned char *szName, unsigned int uiNameLen,
						 const unsigned char *szData, unsigned int uiDataLen )
{
PXMLNODE	pNode;

	if ( !szName || !*szName || !uiNameLen )
		return	NULL;

	if ( !(pNode=(PXMLNODE)calloc(1,sizeof(XMLNODE))) )
	{
		return	NULL;
	}

	if ( !(pNode->m_pszName=(unsigned char*)malloc(uiNameLen+1)) )
	{
		free(pNode);
		return	NULL;
	}
	memcpy(pNode->m_pszName,szName,uiNameLen);
	pNode->m_pszName[uiNameLen]='\0';

	if ( szData && *szData && uiDataLen )
	{
		if ( !(pNode->m_pszData=(unsigned char*)malloc(uiDataLen+1)) )
		{
			free(pNode->m_pszName);
			free(pNode);
			return	NULL;
		}
/*		uiDataLen = xn_explain( pNode->m_pszData, (unsigned char*)szData, uiDataLen );*/
		memcpy(pNode->m_pszData,szData,uiDataLen); 
		pNode->m_pszData[uiDataLen]='\0';
		pNode->m_iDataLen = uiDataLen;
	}
	pNode->m_id = getstrid(szName,uiNameLen);

	return	pNode;
}

/**********************************************************************
 *	������	xn_Destroy
 *	����:	����һ��Ԫ�ؽڵ�
 *	����:
	pNode
          �ڵ�ṹָ�롣
 *	����ֵ:	 ��
 **********************************************************************/
static void xn_Destroy( PXMLNODE pNode )
{
	if ( pNode->m_pszData )
		free(pNode->m_pszData);
	if ( pNode->m_pszName )
		free(pNode->m_pszName);
	sl_AttrDestroy(&pNode->m_AttrList);
	sl_NodeDestroy(&pNode->m_DownList);
	free(pNode);
}

#ifdef	__XML_TRACE__
/***************************************************************
 *	����:	getstr_
 *	����:	��ȡ*.ini�����ļ�����ѡ��
 *	����:
	- psFile���ļ����ִ�
	- psItem��ָ�������ִ�
	- psName��ָ������
	- szDataBuf�������ִ����ݻ�����
	- iBufLen��������szDataBuf�ĳ���
 *	����ֵ:
	- �ɹ�	�����ִ�����ָ��
	- ʧ��	����NULL
 **************************************************************/
static char* getstr_( const char* psFile, const char* psItem, const char* psName,char* szDataBuf, int iBufLen)
{
FILE* pFileStream;
unsigned char	szBuf[256],*p,*q;
int		i;
char	*pBuf,bItemFlag='\0';
int		iLen=0;


	if ( !psFile || !psItem || !psName || !szDataBuf )
	{
		return	(NULL);
	}
	

	if ( ! (pFileStream=fopen(psFile,"rt") ) )
	{
		return	(NULL);
	}

	for(;;)	{
		/* ��ȡһ������ */
		if ( !fgets((char*)szBuf,255,pFileStream) )
			break;
		p = szBuf;
		while ( *p==32 || *p==9 ) /* �˳��ո���Ʊ�� */
			p++;

		/* ע�ͻ�س��� */
		if ( !*p || *p=='#' || ( *p=='/' && *(p+1)=='/') || *p==13 || *p==10 )
			continue;

		if ( bItemFlag ) { /* �Ѿ�ƥ��Item */
			if ( *p == '[' ) /* ����δ�ҵ�ָ��Name */
				break;
			
			q=(unsigned char*)psName;

			while ( (*p==*q) && (*p!='=') && *q ) {
				p++;
				q++;
			}
			while ( *p==32 || *p==9 ) /* �˳��ո���Ʊ�� */
				p++;

			if ( ( *p!='=' ) || *q  ) /* ��ƥ��Name */
				continue;

			p++;
			while ( *p==32 || *p==9 ) /* �˳��ո���Ʊ�� */
				p++;
			
			pBuf = szDataBuf;
			i = 0;
			while ( !( !*p || *p=='#' || *p==9 || ( *p=='/' && *(p+1)=='/') || *p==13 || *p==10 )) {
				if ( *p==32 ) /* �����ַ���ո�� */
					i++;
				else {
					while ( i ) {
						i--;
						iLen++;
						if(iLen>iBufLen)
						{
							fclose( pFileStream );
							return (NULL);
						}
						*pBuf++ = 32;
					}
					iLen++;
					if(iLen>iBufLen)
					{
						fclose( pFileStream );
						return (NULL);
					}
					*pBuf++ = *p;
				}
				p++;
			}
			iLen++;
			if(iLen>iBufLen)
			{
				fclose( pFileStream );
				return (NULL);
			}
			*pBuf = '\0';

			fclose( pFileStream );
			return	szDataBuf;
		}

		/* �����Ƿ���ָ��Item�� */
		else {
			if ( *p != '[' )
				continue;
			p++;
			while ( *p==32 || *p==9 ) /* �˳��ո���Ʊ�� */
				p++;

			q=(unsigned char*)psItem;

			while ( (*p==*q) && (*p!=']') && *q ) {
				p++;
				q++;
			}
			while ( *p==32 || *p==9 ) /* �˳��ո���Ʊ�� */
				p++;

			if ( ( *p==']' ) && !*q  ) /* ƥ��Item */
				bItemFlag='9';
		} /* End of else */
	}

	fclose( pFileStream );
	return	NULL;
}

static int xmlo_InitTrace(char *szCfgFile, PXMLSTRUCT pXML )
{
int		hTrace;
int		len;
char	szBuf[256],szDir[128];

	if ( access(szCfgFile,0) == -1 )
	{
		g_xmlErrno = XML_ELOGFILENOTEXIST;
		snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szCfgFile);
		return	FAIL;
	}
	if ( access(szCfgFile,4) == -1 )
	{
		g_xmlErrno = XML_ELOGFILENOTREAD;
		snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szCfgFile);
		return	FAIL;
	}

	if ( getstr_(szCfgFile,"COMMON","TRACEFILE",szBuf,sizeof(szBuf)) )
	{
		len = strlen(szBuf);
		snprintf(szBuf+len,sizeof(szBuf)-len,"_%s",pXML->m_pRoot->m_pszName);
	}
	else
	{
		getcwd(szBuf,sizeof(szBuf));
		len = strlen(szBuf);
		snprintf(szBuf+len,sizeof(szBuf)-len,"%cxml_log_%s",
			FILE_SFLAG,pXML->m_pRoot->m_pszName);
	}

	if ( !getstr_(szCfgFile,"COMMON","BAKDIR",szDir,sizeof(szDir)) )
	{
		getcwd(szDir,sizeof(szDir));
		len = strlen(szDir);
		snprintf(szDir+len,sizeof(szDir)-len,"%ctrace",FILE_SFLAG);
	}

	if ( (hTrace=trc_Build( szBuf, szDir))==FAIL )
	{
		g_xmlErrno = XML_EBUILDTRACE;
		return	FAIL;
	}

	if ( getstr_(szCfgFile,"COMMON","FLUSH",szBuf,sizeof(szBuf)) )
		trc_SetAttr( hTrace, TFS_FLUSH, atoi(szBuf));

	if ( getstr_(szCfgFile,"COMMON","ALWAYSOPEN",szBuf,sizeof(szBuf)) )
		trc_SetAttr( hTrace, TFS_ALWAYSOPEN, atoi(szBuf));

	if ( getstr_(szCfgFile,"COMMON","MAXSIZE",szBuf,sizeof(szBuf)) )
		trc_SetAttr( hTrace, TFS_MAXSIZE, atoi(szBuf));

	if ( getstr_(szCfgFile,"COMMON","MAXHOUR",szBuf,sizeof(szBuf)) )
		trc_SetAttr( hTrace, TFS_MAXHOUR, atoi(szBuf));

	if ( getstr_(szCfgFile,"COMMON","DAYTIME",szBuf,sizeof(szBuf)) )
		trc_SetAttr( hTrace, TFS_DAYTIME, atoi(szBuf));

	if ( getstr_(szCfgFile,"COMMON","WEEKDAY",szBuf,sizeof(szBuf)) )
		trc_SetAttr( hTrace, TFS_WEEKDAY, atoi(szBuf));

	if ( getstr_(szCfgFile,"COMMON","MONTHDAY",szBuf,sizeof(szBuf)) )
		trc_SetAttr( hTrace, TFS_MONTHDAY, atoi(szBuf));

	if ( getstr_(szCfgFile,"COMMON","DATA",szBuf,sizeof(szBuf)) )
	{
		pXML->m_dwTrcType = (unsigned int)atoi(szBuf);
	}
	else
	{
		if ( getstr_(szCfgFile,"COMMON","D_IMPORTDSR",szBuf,sizeof(szBuf)) )
		{
			if ( atoi(szBuf) )
				pXML->m_dwTrcType |= XL_INDSR;
		}
		if ( getstr_(szCfgFile,"COMMON","D_EXPORTDSR",szBuf,sizeof(szBuf)) )
		{
			if ( atoi(szBuf) )
				pXML->m_dwTrcType |= XL_TODSR;
		}
		if ( getstr_(szCfgFile,"COMMON","D_IMPORTXML",szBuf,sizeof(szBuf)) )
		{
			if ( atoi(szBuf) )
				pXML->m_dwTrcType |= XL_INXML;
		}
		if ( getstr_(szCfgFile,"COMMON","D_EXPORTXML",szBuf,sizeof(szBuf)) )
		{
			if ( atoi(szBuf) )
				pXML->m_dwTrcType |= XL_TOXML;
		}
		if ( getstr_(szCfgFile,"COMMON","D_IMPORTFMT",szBuf,sizeof(szBuf)) )
		{
			if ( atoi(szBuf) )
				pXML->m_dwTrcType |= XL_INFMT;
		}
		if ( getstr_(szCfgFile,"COMMON","D_EXPORTFMT",szBuf,sizeof(szBuf)) )
		{
			if ( atoi(szBuf) )
				pXML->m_dwTrcType |= XL_TOFMT;
		}
		if ( getstr_(szCfgFile,"COMMON","D_ERROR",szBuf,sizeof(szBuf)) )
		{
			if ( atoi(szBuf) )
				pXML->m_dwTrcType |= XL_ERROR;
		}
		if ( getstr_(szCfgFile,"COMMON","D_DATA",szBuf,sizeof(szBuf)) )
		{
			if ( atoi(szBuf) )
				pXML->m_dwTrcType |= XL_DATA;
		}
		if ( getstr_(szCfgFile,"COMMON","D_NODE",szBuf,sizeof(szBuf)) )
		{
			if ( atoi(szBuf) )
				pXML->m_dwTrcType |= XL_NODE;
		}
		if ( getstr_(szCfgFile,"COMMON","D_NODEATTR",szBuf,sizeof(szBuf)) )
		{
			if ( atoi(szBuf) )
				pXML->m_dwTrcType |= XL_NODEATTR;
		}
		if ( getstr_(szCfgFile,"COMMON","D_COUNT",szBuf,sizeof(szBuf)) )
		{
			if ( atoi(szBuf) )
				pXML->m_dwTrcType |= XL_COUNT;
		}
		if ( getstr_(szCfgFile,"COMMON","D_EXCHANGE",szBuf,sizeof(szBuf)) )
		{
			if ( atoi(szBuf) )
				pXML->m_dwTrcType |= XL_EXCHANGE;
		}
	}
	return	hTrace;
}
#endif

/**********************************************************************
 *	������	xmlo_Build
 *	����:	����һ��XML�ṹ
 *	����:
	 szRoot
		Ԫ�صĸ����ơ�
 *	����ֵ:
	 �ɹ�	���ع���Ľṹ��ʶ
	 ʧ��	����NULL
 **********************************************************************/
static PXMLSTRUCT xmlo_Build( char *szRootName )
{
PXMLSTRUCT	pXML;
char	*p;

	if ( !(pXML=(PXMLSTRUCT)calloc(1,sizeof(XMLSTRUCT))) )
	{
		g_xmlErrno = XML_ENOMEM;
		return	NULL;
	}

	if ( !(pXML->m_pRoot=xn_Build( (unsigned char*)szRootName, strlen(szRootName),NULL,0)) )
	{
		g_xmlErrno = XML_ENOMEM;
		free(pXML);
		return	NULL;
	}
#ifdef	__XML_TRACE__
{
/*char	szName[128],szDir[128];
	getcwd(szDir,sizeof(szDir));
	snprintf(szName,sizeof(szName),"%s%ctrace%cxml_log_%s",szDir,FILE_SFLAG,FILE_SFLAG,szRootName);
	snprintf(szDir+strlen(szDir),sizeof(szDir)-strlen(szDir),"%ctrace",FILE_SFLAG);
	if ( (pXML->m_hTrace=trc_Build( szName, szDir))==FAIL )
	{
		xn_Destroy(pXML->m_pRoot);
		free(pXML);
		return	NULL;
	}
	pXML->m_dwTrcType = XL_ALL;
	trc_SetAttr( pXML->m_hTrace, TFS_FLUSH, 1);
*/
	if ( (p=getenv(XML_LOGCFGFILE)) && *p )
	{
		if ( (pXML->m_hTrace=xmlo_InitTrace(p,pXML))==FAIL )
		{
			xn_Destroy(pXML->m_pRoot);
			free(pXML);
			return	NULL;
		}
		if ( pXML->m_dwTrcType & XL_NODE )
		{
			trc_Write(pXML->m_hTrace,"==================== ��ʼXML���ݼ�¼[%t] ====================\n\n",0);
			trc_Write(pXML->m_hTrace,">>> %t <<<  xml_Create('%s').\n\n",0,szRootName);
		}
	}
}
#endif
	return	pXML;
}

/**********************************************************************
 *	������	xmlo_Destroy
 *	����:	����XML�ṹ
 *	����:
	 pXML
		XML�ṹָ�롣
 *	����ֵ:	��
 **********************************************************************/
static void xmlo_Destroy( PXMLSTRUCT pXML )
{
	if ( pXML->m_pRoot )
		xn_Destroy(pXML->m_pRoot);
#ifdef	__XML_TRACE__
	if ( pXML->m_dwTrcType & XL_NODE )
	{
		trc_Write(pXML->m_hTrace,">>> %t <<<  xml_Destroy().\n\n",0);
		trc_Write(pXML->m_hTrace,"==================== ����XML���ݼ�¼[%t] ====================\n\n\n\n",0);
		trc_Destroy(pXML->m_hTrace);
	}
#endif
	free(pXML);
}


/**********************************************************************
 *	������	xn_SetData
 *	����:	�������ýڵ�ֵ
 *	����:
	pNode
		�ڵ�ṹָ�롣
	szData
		�µĽڵ�ֵ
	len
		�µĽڵ�ֵ����
 *	����ֵ:
	�ɹ�	����SUCC
	ʧ��	����FAIL
	�޸ļ�¼��
	   ������ת�崦��,Tianhc 2009-12-29 9:28:07
 **********************************************************************/
static int xn_SetData( PXMLNODE pNode, unsigned char *szData, unsigned int len )
{
unsigned char	*p;

	if ( szData && *szData && len>0 )
	{ /* ���� */
		if ( !(p=realloc(pNode->m_pszData,len+1)) )
		{
			g_xmlErrno = XML_ENOMEM;
			return	FAIL;
		}
		pNode->m_pszData = p;
/*		len = xn_explain( pNode->m_pszData, szData, len );*/
		memcpy(pNode->m_pszData,szData,len); 
		*(pNode->m_pszData+len)='\0';
		pNode->m_iDataLen = len;
	}
	else
	{ /* ��� */
		if ( pNode->m_pszData )
		{
			free(pNode->m_pszData);
			pNode->m_pszData = NULL;
		}
		pNode->m_iDataLen=0;
	}
	return	SUCC;
}


/**********************************************************************
 *	������	xn_SetDataWithConvert
 *	����:	�������ýڵ�ֵ(ֻ�ṩ��xm_ImportXMLStringʹ��)
 *	����:
	pNode
		�ڵ�ṹָ�롣
	szData
		�µĽڵ�ֵ
	len
		�µĽڵ�ֵ����
 *	����ֵ:
	�ɹ�	����SUCC
	ʧ��	����FAIL
	˵����
	   ��xn_SetData()�����ǽ���xn_explain����
	   ��Ϊ��ֻ��Importʱ������Ҫ����xn_explainת�崦�����಻Ӧ�ý��д���
 **********************************************************************/
static int xn_SetDataWithConvert( PXMLNODE pNode, unsigned char *szData, unsigned int len )
{
unsigned char	*p;

	if ( szData && *szData && len>0 )
	{ /* ���� */
		if ( !(p=realloc(pNode->m_pszData,len+1)) )
		{
			g_xmlErrno = XML_ENOMEM;
			return	FAIL;
		}
		pNode->m_pszData = p;
		len = xn_explain( pNode->m_pszData, szData, len );
/*		memcpy(pNode->m_pszData,szData,len); */
		*(pNode->m_pszData+len)='\0';
		pNode->m_iDataLen = len;
	}
	else
	{ /* ��� */
		if ( pNode->m_pszData )
		{
			free(pNode->m_pszData);
			pNode->m_pszData = NULL;
		}
		pNode->m_iDataLen=0;
	}
	return	SUCC;
}

/**********************************************************************
 *	������	xna_SetData
 *	����:	������������ֵ
 *	����:
	pNode
		���Խṹָ�롣
	szData
		�µ�����ֵ
	len
		�µ�����ֵ����
 *	����ֵ:
	�ɹ�	����SUCC
	ʧ��	����FAIL
	�޸ļ�¼��
	  ��Ϊ������ת�崦��,Tianhc 2009-12-29 9:04:56
 **********************************************************************/
static int xna_SetData( PXMLNODEATTR pAttr, unsigned char *szData, unsigned int len )
{
unsigned char	*p;

	if ( szData && *szData && len )
	{ /* ���� */
		if ( !(p=realloc(pAttr->m_pszData,len+1)) )
		{
			g_xmlErrno = XML_ENOMEM;
			return	FAIL;
		}
		pAttr->m_pszData = p;
/*		len = xn_explain( pAttr->m_pszData, szData, len );*/
		memcpy(pAttr->m_pszData,szData,len); 
		*(pAttr->m_pszData+len)='\0';
	}
	else
	{ /* ��� */
		if ( pAttr->m_pszData )
		{
			free(pAttr->m_pszData);
			pAttr->m_pszData = NULL;
		}
	}
	return	SUCC;
}

/**********************************************************************
 *	������	xna_SetDataWithConvert
 *	����:	������������ֵ(ֻ�ṩ��xn_ImportXMLStringʹ��)
 *	����:
	pNode
		���Խṹָ�롣
	szData
		�µ�����ֵ
	len
		�µ�����ֵ����
 *	����ֵ:
	�ɹ�	����SUCC
	ʧ��	����FAIL
	�޸ļ�¼��
	  ����ת�崦��,Tianhc 2009-12-29 9:04:56
 **********************************************************************/
static int xna_SetDataWithConvert( PXMLNODEATTR pAttr, unsigned char *szData, unsigned int len )
{
unsigned char	*p;

	if ( szData && *szData && len )
	{ /* ���� */
		if ( !(p=realloc(pAttr->m_pszData,len+1)) )
		{
			g_xmlErrno = XML_ENOMEM;
			return	FAIL;
		}
		pAttr->m_pszData = p;
		len = xn_explain( pAttr->m_pszData, szData, len );
/*		memcpy(pAttr->m_pszData,szData,len); */
		*(pAttr->m_pszData+len)='\0';
	}
	else
	{ /* ��� */
		if ( pAttr->m_pszData )
		{
			free(pAttr->m_pszData);
			pAttr->m_pszData = NULL;
		}
	}
	return	SUCC;
}

/**********************************************************************
 *	������	xn_Copy
 *	����:	����һ���ڵ�
		��ɺ󣬷���һ�������Ľڵ�
 *	����:
	lpNode
		Դ�ڵ�
 *	����ֵ:
	�ɹ�	Ŀ��ڵ�
	ʧ��  NULL
  �޸ļ�¼��
 **********************************************************************/
static PXMLNODE xn_Copy(PXMLNODE lpNode)
{
	PXMLNODE pNode = NULL;
	PXMLNODE *lpElemP,*lpEndP;
	PXMLNODEATTR *lpAttrP, *lpEndAP;
	PWSEQLIST lpList;
  if (lpNode == NULL)
  	return NULL;
  
  pNode = xn_Build((unsigned char*)lpNode->m_pszName, strlen(lpNode->m_pszName),
                    lpNode->m_pszData, lpNode->m_iDataLen);
  if (!pNode)
	{
		g_xmlErrno = XML_ENOMEM;
		return	NULL;
	}
	lpList = (PWSEQLIST) &(lpNode->m_DownList);
	lpEndP=(PXMLNODE*)lpList->m_lpBase+lpList->m_nLen;
	lpElemP = (PXMLNODE*)lpList->m_lpBase;
	for ( ; lpElemP<lpEndP; lpElemP++ )
	{
		PXMLNODE tpNode = xn_Copy(*lpElemP);
		if (!tpNode)
		{
			xn_Destroy(pNode);
			return NULL;
		}
		
		sl_Append(&(pNode->m_DownList), tpNode);
	}
	lpList = (PWSEQLIST) &(lpNode->m_AttrList);
	lpEndAP=(PXMLNODEATTR*)lpList->m_lpBase+lpList->m_nLen;
	lpAttrP = (PXMLNODEATTR*)lpList->m_lpBase;
	for ( ; lpAttrP<lpEndAP; lpAttrP++ )
	{
		PXMLNODEATTR pAttr = xna_Build((*lpAttrP)->m_pszName, strlen((*lpAttrP)->m_pszName), (*lpAttrP)->m_pszData);
		if (!pAttr)
		{
			g_xmlErrno = XML_ENOMEM;
			xn_Destroy(pNode);
			return NULL;
		}
		sl_Append(&(pNode->m_AttrList), pAttr);
	}
	
	return pNode;
}

/**********************************************************************
 *	������	xn_Move
 *	����:	��ָ���ڵ������滻
		��ɺ󣬽������ݽڵ��ԭ�����ݽ�ɾ�������ƶ����ݽڵ㽫��ʧ
 *	����:
	pNode
		�����������ݵĽڵ�
	pMoveNode
		���ƶ����ݵĽڵ�
 *	����ֵ:
	�ɹ�	����SUCC
	ʧ��	����FAIL
  �޸ļ�¼��
 **********************************************************************/
static int xn_Move( PXMLNODE pNode, PXMLNODE pMoveNode)
{
	sl_NodeDestroy( &pNode->m_DownList );
	sl_AttrDestroy( &pNode->m_AttrList );
	if ( pNode->m_pszName )
		free( pNode->m_pszName );
	if ( pNode->m_pszData )
		free( pNode->m_pszData );

	memcpy(pNode, pMoveNode, sizeof(XMLNODE));

	free(pMoveNode);
	return	SUCC;
}


#define	XML_NAMEFLAG	'/'	/* ���ƺ�����֮��ķָ��� */
#define	XML_POSFLAG		'|' /* ���ƺ�λ��֮��ķָ��� */

/**********************************************************************
 *	������	xn_LocateNode
 *	����:	��ȡXML������ָ�����ƵĽڵ�
 *	����:
	pRootNode
		���ڵ�ṹָ��
	szName
		Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
		"/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
		NMAE	��ʾȡ���һ��Ԫ�أ�
		IDX		ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
			���iIndexΪ0����ʾ��������
			���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
			ȱʡΪ1
	pnUpPos
		��һ��λ��ָ��
	pXML
		XML�ṹָ��
 *	����ֵ:
	�ɹ�	���ػ�ȡ�Ľڵ�ָ��
	ʧ��	����NULL
 **********************************************************************/
static PXMLNODE xn_LocateNode( PXMLNODE pRootNode, char *szName, int *pnUpPos, PXMLSTRUCT pXML )
{
unsigned char	*pName,*p;
PXMLNODE	lpNode,lpUp;
int iIndex,len;

	p=(unsigned char*)szName;
	lpNode = pRootNode;
	if ( *p=='/' )
		p++;

	while ( *p )
	{
		pName = p;
		while ( *p && !(*p==XML_NAMEFLAG||*p==XML_POSFLAG) )
			p++;

		if ( *p == XML_POSFLAG )
		{
			iIndex = 0;
			len = p++-pName;
			if ( !len || len==1 && !*pName )
			{ /* �ִ����ƴ��� */
				g_xmlErrno = XML_EINVNODENAME;
				snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szName);
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: %s �ִ��ָ����ݴ��� - %s\n",szName, xml_StringError(xml_GetLastError()));
#endif
				return	NULL;
			}
			while ( *p && isdigit(*p) )
			{
				iIndex = iIndex*10+*p-'0';
				p++;
			}
			if ( !*p )
			{
				p--;
			}
			else if ( *p != XML_NAMEFLAG )
			{ /* �ִ��ָ����ݴ��� */
				g_xmlErrno = XML_EINVNODENAME;
				snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szName);
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: %s �ִ��ָ����ݴ��� - %s\n",szName, xml_StringError(xml_GetLastError()));
#endif
				return	NULL;
			}
		}
		else
		{
			iIndex = 1; /* ȱʡΪ��һ�� */
			len = p-pName;
			if ( !len || len==1 && !*pName )
			{ /* �ִ����ƴ��� */
				g_xmlErrno = XML_EINVNODENAME;
				snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szName);
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: %s �ִ��ָ����ݴ��� - %s\n",szName, xml_StringError(xml_GetLastError()));
#endif
				return	NULL;
			}
			if ( !*p )
			{
				p--;
			}
		}

		lpUp = lpNode;
		if ( !(lpNode=sl_FindNode( &lpNode->m_DownList, pName, len, iIndex, pnUpPos )) )
		{ /* ������ */
			g_xmlErrno = XML_ENONODE;
			snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szName);
			return	NULL;
		}
		p++;
	}

	if ( pnUpPos )
		return	lpUp;
	return	lpNode;
}


/**********************************************************************
 *	������	xn_AddNode
 *	����:	��XML�������һ���ڵ�
 *	����:
	pRootNode
		���ڵ�ṹָ��
	szName
		ָ����XML�ṹҪ��ӵ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
		"/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
		NMAE	��ʾȡ���һ��Ԫ�أ�
		IDX		ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
			���iIndexΪ0����ʾ��������
			���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
			ȱʡΪ0
	szData
		����ӵ�Ԫ��ֵָ��
	pXML
		XML�ṹָ��
 *	����ֵ:
	�ɹ�	������ӵĽڵ�ָ��
	ʧ��	����NULL
 **********************************************************************/
static PXMLNODE xn_AddNode( PXMLNODE pRootNode, char *szName, char *szData, int iDataLen, PXMLSTRUCT pXML )
{
unsigned char	*pName,*p;
PXMLNODE	lpNode,pTpNode;
int iIndex,len;
int	bFlag=0;

	p=(unsigned char*)szName;
	lpNode = pRootNode;

	if ( *p=='/' )
		p++;

	while ( *p )
	{
		iIndex = 0;
		pName = p;
		while ( *p && !(*p==XML_NAMEFLAG||*p==XML_POSFLAG) )
			p++;

		if ( *p == XML_POSFLAG )
		{
			len = p++-pName;
			if ( !len || len==1 && !*pName )
			{ /* �ִ����ƴ��� */
				g_xmlErrno = XML_EINVNODENAME;
				snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szName);
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: %s - %s\n",szName, xml_StringError(xml_GetLastError()));
#endif
				return	NULL;
			}

			while ( *p && isdigit(*p) )
			{
				iIndex = iIndex*10+*p-'0';
				p++;
			}
			if ( !*p )
			{
				p--;
			}
			else if ( *p!=XML_NAMEFLAG )
			{ /* �ִ��ָ����ݴ��� */
				g_xmlErrno = XML_EINVNODENAME;
				snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szName);
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: %s �ִ��ָ����ݴ��� - %s\n",szName, xml_StringError(xml_GetLastError()));
#endif
				return	NULL;
			}
		}
		else
		{
			len = p-pName;
			if ( !len || len==1 && !*pName )
			{ /* �ִ����ƴ��� */
				g_xmlErrno = XML_EINVNODENAME;
				snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szName);
#ifdef __XML_TRACE__
					if ( pXML->m_dwTrcType&XL_ERROR )
						trc_Write(pXML->m_hTrace,"error: %s - %s\n",szName, xml_StringError(xml_GetLastError()));
#endif
				return	NULL;
			}

			if ( !*p )
			{
				p--;
			}
		}

		if ( !*(p+1) || bFlag || !(pTpNode=sl_FindNode( &lpNode->m_DownList, pName, len, iIndex, NULL )) || (pTpNode->m_pszData&&*pTpNode->m_pszData)  )
		{ /* �����ڻ������� */
			if ( !bFlag )
				bFlag = 1;
			if ( *(p+1) )
				pTpNode=xn_Build(pName,len,NULL,0 );
			else
				pTpNode=xn_Build(pName,len,(unsigned char*)szData,iDataLen);
			if ( !pTpNode )
			{
				g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ����ڵ�'%m'ʧ�� - %s\n",pName,len,xml_StringError(xml_GetLastError()));
#endif
				return	NULL;
			}

			if ( sl_Append(&(lpNode->m_DownList),pTpNode)==-1 )
			{
				g_xmlErrno = XML_ENOMEM;
				xn_Destroy(pTpNode);
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ��ӽڵ�ʧ�� - %s\n",xml_StringError(xml_GetLastError()));
#endif
				return	NULL;
			}
		}
		lpNode = pTpNode;
		p++;
	}

	return	pTpNode;
}



/***************************************************************
 *	����:	xn_explain
 *	����:	���ʹ���ת���ַ����ִ������仹ԭΪԭ�����ַ���
 *	����һ:	���ս��ͺ���ִ�����ָ��(�ɵ��ú�����֤�ռ��С)
 *	������:	�����͵Ĵ���ת���ַ����ִ�
 *	������:	�ִ�����
 *	����ֵ:	����ʵ�������ֽ���
 *	��ע��	����xn_ImportXMLString����ʹ��
 ת���ַ�Ϊ�����ַ���
	&lt;    &gt;        &apos;      &quote;     &amp;
	<       >           '           "           &
  ==>                             &quot;  --Tianhc 2006-6-15 17:54
 **************************************************************/
static int xn_explain( unsigned char *szBuf, unsigned char *pStr, unsigned int len )
{
unsigned char *p;
unsigned char *q;

	p = pStr;
	q = szBuf;

	while ( len--  )
	{
		if ( *p == '&' )
		{
			p++;
			if ( !_memicmp(p,"lt;",3) )
			{
				p+=3;
				len -= 3;
				*q++ = '<';
			}
			else if ( !_memicmp(p,"gt;",3) )
			{
				p+=3;
				len -= 3;
				*q++ = '>';
			}
			else if ( !_memicmp(p,"apos;",5) )
			{
				p+=5;
				len -= 5;
				*q++ = '\'';
			}
			else if (( !_memicmp(p,"quot;",5))  &&  (1==g_xmlquot))
			{
				p+=5;
				len -= 5;
				*q++ = '"';
			}
			else if (( !_memicmp(p,"quote;",6)) &&  (0==g_xmlquot))
			{
				p+=6;
				len -= 6;
				*q++ = '"';
			}
			else if ( !_memicmp(p,"amp;",4) )
			{
				p+=4;
				len -= 4;
				*q++ = '&';
			}
			else
			{ /* ��Ӧ���ֵ������ַ�����Ϊ�����Է���ԭ�ַ� */
				*q++='&';
			}
		}
		else
		{
			*q++ = *p++;
		}

	}

	return	q-szBuf;
}

/***************************************************************
 *	����:	xn_explace
 *	����:	���ַ����������ַ���ת���ַ��滻
 *	����һ:	����ת������ִ�����ָ��
 *	������:	�����С
 *	������:	������ת���Ĵ��������ַ����ִ�
 *	������:	�ִ�����
 *	����ֵ:	����ת��, ����д�뻺�������ֽ���; �������, ����0
 *	��ע��	����xn_ExportXMLString����ʹ��
 **************************************************************/
static int xn_explace( unsigned char *szBuf, unsigned int size, unsigned char *pStr, unsigned int len )
{
unsigned char *p,*q;
unsigned int num;

	p = pStr;
	q = szBuf;
	num = size;

	while ( len-- && num )
	{
		switch ( *p )
		{
		case	'<':
			if ( num<4 )
				return	FAIL;
			memcpy(q,"&lt;",4);
			q   += 4;
			num += 4;
			break;
		case	'>':
			if ( size<4 )
				return	FAIL;
			memcpy(q,"&gt;",4);
			q += 4;
			num += 4;
			break;
		case	'\'':
			if ( size<6 )
				return	FAIL;
			memcpy(q,"&apos;",6);
			q += 6;
			num += 6;
			break;
		case	'"':
      if (1==g_xmlquot)  /*" ==> &quot; Tianhc 2006-9-7 13:10*/
      {	 
				if ( size<6 )
					return	FAIL;
				memcpy(q,"&quot;",6);
				q += 6;
				num += 6;
				break;
		  } else
		  {
				if ( size<7 )    /*" ==> &quote; Ϊ�˼�����ǰ�Ĵ���!*/
					return	FAIL;
				memcpy(q,"&quote;",7);
				q += 7;
				num += 7;
				break;
		  }		
		case	'&':
			if ( size<5 )
				return	FAIL;
			memcpy(q,"&amp;",5);
			q += 5;
			num += 5;
			break;
		default:
			*q++ = *p;
			num--;
		}
		p++;
	}
	return	q-szBuf;
}

/***************************************************************
 *	����:	xn_explacelen
 *	����:	���ַ����������ַ���ת���ַ��滻,�����滻�󳤶�
 *	����һ:	������ת���Ĵ��������ַ����ִ�
 *	������:	�ִ�����
 *	����ֵ:	����ת��, ����д�뻺�������ֽ���; 
 *	��ע��	����xn_ExportXMLStringLen����ʹ��
 **************************************************************/
static int xn_explacelen( unsigned char *pStr, unsigned int len )
{
unsigned char *p,*q;
unsigned int num;

	p = pStr;
	num = 0;

	while ( len-- )
	{
		switch ( *p )
		{
		case	'<':
			num += 4;
			break;
		case	'>':
			num += 4;
			break;
		case	'\'':
			num += 6;
			break;
		case	'"':
			num += 6;
			break;
		case	'&':
			num += 5;
			break;
		default:
			num++;
		}
		p++;
	}
	return	num;
}

/***************************************************************
 *	����:	xn_ExportXMLStringKVL
 *	����:	����KVL��ʽ�ַ���, ר����KVL��ʽ����
 *	����һ:	�������Ľڵ�ָ��
 *	������:	�����ִ����ݵĻ�����ָ��
 *	������:	����������
 *	������:	�Ƿ������ýڵ�����
 *	����ֵ:	�ɹ�,����SUCC; ʧ��,����FAIL;
 **************************************************************/
static int xn_ExportXMLStringKVL(PXMLNODE lpNode, char *szBuf, size_t size, int bNodeSelf )
{
PWSEQLIST	lpList;
PXMLNODEATTR *lpElemP;
PXMLNODE *lpNodeP;
int		iNameLen,iDataLen,len1,len2;
int		i,n,num;
unsigned char	*p;

	num = size;
	p = (unsigned char*)szBuf;


	if ( bNodeSelf )
	{
		iNameLen=strlen((char*)lpNode->m_pszName);
		if ( num <= iNameLen+1 )
			return	FAIL;
		*p++ = '<';
		memcpy(p,lpNode->m_pszName,iNameLen);
		p	+= iNameLen;
		num	-= iNameLen;

		/* ������������ */
		lpList = &lpNode->m_AttrList;
		if ( i=sl_Length(lpList) )
		{
			lpElemP=(PXMLNODEATTR*)lpList->m_lpBase;
			for ( ; i; i--, lpElemP++ )
			{
				len1 = strlen((char*)(*lpElemP)->m_pszName);
				if ( num < len1+4 )
					return	FAIL;

				*p++ = ' ';
				memcpy(p,(*lpElemP)->m_pszName,len1);
				p += len1;
				*p++ = '=';
				*p++ = '\"';
				num -= (len1+4);

				if ( (*lpElemP)->m_pszData )
				{
					if ( (len2=xn_explace(p,num,(*lpElemP)->m_pszData,
							strlen((char*)(*lpElemP)->m_pszData)))==FAIL )
						return	FAIL;
					num -= len2;
					p   += len2;
				}
				*p++ = '\"';
			}
		}
		*p++ = '>';
		if ( num <= 1 )
			return	FAIL;
		num--;
	}
/* �����ӽڵ����� */
	lpList = &lpNode->m_DownList;
	if ( i=sl_Length(lpList) )
	{
		lpNodeP		= (PXMLNODE*)lpList->m_lpBase;
		for ( ; i; i--,lpNodeP++ )
		{
			if ( (n=xn_ExportXMLStringKVL((*lpNodeP),(char*)p,num,1))==FAIL )
				return	FAIL;
			num-=n;
			p+=n;
		}
	}

	if ( bNodeSelf )
	{
		/* �����ڵ�ֵ���� */
		if ( lpNode->m_pszData )
		{
			if ( (iDataLen=xn_explace(p,num,lpNode->m_pszData,lpNode->m_iDataLen))==FAIL )
				return	FAIL;
			num -= iDataLen;
			p += iDataLen;
		}

		if ( num < 4 )
			return	FAIL;

		*p++ = '<';
		*p++ = '/';
/*
		memcpy(p,lpNode->m_pszName,iNameLen);
		p+=iNameLen;
*/
		*p++ = '>';
		*p++ = '\0';
		num -= 4;
	}

	return	size-num;
}


/***************************************************************
 *	����:	xn_ExportXMLString
 *	����:	����XML��ʽ�ַ���
 *	����һ:	�������Ľڵ�ָ��
 *	������:	�����ִ����ݵĻ�����ָ��
 *	������:	����������
 *	������:	�Ƿ������ýڵ�����
 *	����ֵ:	�ɹ�,����SUCC; ʧ��,����FAIL;
 **************************************************************/
static int xn_ExportXMLString(PXMLNODE lpNode, char *szBuf, size_t size, int bNodeSelf )
{
PWSEQLIST	lpList;
PXMLNODEATTR *lpElemP;
PXMLNODE *lpNodeP;
int		iNameLen,iDataLen,len1,len2;
int		i,n,num;
unsigned char	*p;

	num = size;
	p = (unsigned char*)szBuf;


	if ( bNodeSelf )
	{
		iNameLen=strlen((char*)lpNode->m_pszName);
		if ( num <= iNameLen+1 )
			return	FAIL;
		*p++ = '<';
		memcpy(p,lpNode->m_pszName,iNameLen);
		p	+= iNameLen;
		num	-= iNameLen;

		/* ������������ */
		lpList = &lpNode->m_AttrList;
		if ( i=sl_Length(lpList) )
		{
			lpElemP=(PXMLNODEATTR*)lpList->m_lpBase;
			for ( ; i; i--, lpElemP++ )
			{
				len1 = strlen((char*)(*lpElemP)->m_pszName);
				if ( num < len1+4 )
					return	FAIL;

				*p++ = ' ';
				memcpy(p,(*lpElemP)->m_pszName,len1);
				p += len1;
				*p++ = '=';
				*p++ = '\"';
				num -= (len1+4);

				if ( (*lpElemP)->m_pszData )
				{
					if ( (len2=xn_explace(p,num,(*lpElemP)->m_pszData,
							strlen((char*)(*lpElemP)->m_pszData)))==FAIL )
						return	FAIL;
					num -= len2;
					p   += len2;
				}
				*p++ = '\"';
			}
		}
		*p++ = '>';
		if ( num <= 1 )
			return	FAIL;
		num--;
	}

	/* �����ӽڵ����� */
	lpList = &lpNode->m_DownList;
	if ( i=sl_Length(lpList) )
	{
		lpNodeP		= (PXMLNODE*)lpList->m_lpBase;
		for ( ; i; i--,lpNodeP++ )
		{
			if ( (n=xn_ExportXMLString((*lpNodeP),(char*)p,num,1))==FAIL )
				return	FAIL;
			num-=n;
			p+=n;
		}
	}

	if ( bNodeSelf )
	{
		/* �����ڵ�ֵ���� */
		if ( lpNode->m_pszData )
		{
			if ( (iDataLen=xn_explace(p,num,lpNode->m_pszData,lpNode->m_iDataLen))==FAIL )
				return	FAIL;
			num -= iDataLen;
			p += iDataLen;
		}

		if ( num < iNameLen+4 )
			return	FAIL;

		*p++ = '<';
		*p++ = '/';
		memcpy(p,lpNode->m_pszName,iNameLen);
		p+=iNameLen;
		*p++ = '>';
		*p++ = '\0';
		num -= (iNameLen+4);
	}

	return	size-num;
}


/***************************************************************
 *	����:	xn_ExportXMLString
 *	����:	����XML��ʽ�ַ���
 *	����һ:	�������Ľڵ�ָ��
 *	������:	�����ִ����ݵĻ�����ָ��
 *	������:	����������
 *	������:	�Ƿ������ýڵ�����
 *	����ֵ:	�ɹ�,����SUCC; ʧ��,����FAIL;
 **************************************************************/
static int xn_ExportXMLString_Format(PXMLNODE lpNode, char *szBuf, size_t size, int bNodeSelf,int ilevel )
{
PWSEQLIST	lpList;
PXMLNODEATTR *lpElemP;
PXMLNODE *lpNodeP;
int		iNameLen,iDataLen,len1,len2;
int		i,n,num;
unsigned char	*p;

	num = size;
	p = (unsigned char*)szBuf;


	if ( bNodeSelf )
	{
		iNameLen=strlen((char*)lpNode->m_pszName);
		/*����TAB��*/
		for (i=1;i<=ilevel;i++)   
		{
		  /**p++ = 9;*/
		  *p++ = ' ';
      num--;
		}
		if ( num <= iNameLen+1 )
			return	FAIL;
		*p++ = '<';
		memcpy(p,lpNode->m_pszName,iNameLen);
		p	+= iNameLen;
		num	-= iNameLen;

		/* ������������ */
		lpList = &lpNode->m_AttrList;
		if ( i=sl_Length(lpList) )
		{
			lpElemP=(PXMLNODEATTR*)lpList->m_lpBase;
			for ( ; i; i--, lpElemP++ )
			{
				len1 = strlen((char*)(*lpElemP)->m_pszName);
				if ( num < len1+4 )
					return	FAIL;

				*p++ = ' ';
				memcpy(p,(*lpElemP)->m_pszName,len1);
				p += len1;
				*p++ = '=';
				*p++ = '\"';
				num -= (len1+4);

				if ( (*lpElemP)->m_pszData )
				{
					if ( (len2=xn_explace(p,num,(*lpElemP)->m_pszData,
							strlen((char*)(*lpElemP)->m_pszData)))==FAIL )
						return	FAIL;
					num -= len2;
					p   += len2;
				}
				*p++ = '\"';
			}
		}
		*p++ = '>';
		if ( num <= 1 )
			return	FAIL;
		num--;
	}

	/* �����ӽڵ����� */
	lpList = &lpNode->m_DownList;
	if ( i=sl_Length(lpList) )
	{
		lpNodeP		= (PXMLNODE*)lpList->m_lpBase;
		ilevel++;
		 /*���ӽڵ�*/
		*p++ = '\n';
		num--;
		for ( ; i; i--,lpNodeP++ )
		{
			if ( (n=xn_ExportXMLString_Format((*lpNodeP),(char*)p,num,1,ilevel))==FAIL )
				return	FAIL;
			num-=n;
			p+=n;
		}
		/*����TAB��*/
		for (i=1;i<ilevel;i++)   
		{
		  /**p++ = 9;*/
		  *p++ = ' ';
      num--;
		}
		
	}

	if ( bNodeSelf )
	{
		/* �����ڵ�ֵ���� */
		if ( lpNode->m_pszData )
		{
			if ( (iDataLen=xn_explace(p,num,lpNode->m_pszData,lpNode->m_iDataLen))==FAIL )
				return	FAIL;
			num -= iDataLen;
			p += iDataLen;
		}
    
		if ( num < iNameLen+5 )
			return	FAIL;

		*p++ = '<';
		*p++ = '/';
		memcpy(p,lpNode->m_pszName,iNameLen);
		p+=iNameLen;
		*p++ = '>';
		*p++ = '\n';  /*���ڵ�������ӻس�*/
		*p++ = '\0';
		num -= (iNameLen+5);
	}

	return	size-num;
}

/***************************************************************
 *	����:	xn_strtok
 *	����:	���XML��ʽ�ַ���
 *	����һ:	XML�ִ�ָ��
 *	������:	�����ִ����ݿ�ʼ��ַָ���ָ��
 *	������:	�����ִ����ȵ�ָ��
 *	������:	�����ִ��ֽ����λ�õ�ַָ���ָ��
 *	����ֵ:	���طֽ����ͼ�#define
 *	��ע��	����xn_ImportXMLString����ʹ��
 �޸ļ�¼��
    Tianhc 2006-11-15 19:07
    ����XML 2.0�Ĵ���(��д֧��),/>
    1.����/> ΪXT_SHORT
    2.�������������XT_SHORT���£�
      <node1/>
      <node1 name="xxxx"/>
      һ������XT_HEADLEFTʱ����
      ��һ��������ȡֵ������
 **************************************************************/
#define		XT_ERROR		(-1)/* ����				*/
#define		XT_END			0	/* ����				*/
#define		XT_HEADLEFT		1	/* ��ʼ���ʶ <		*/
#define		XT_TAILLEFT		2	/* �������ʶ </	*/
#define		XT_RIGHT		3	/* �ұ�ʶ >			*/
#define		XT_EQUALE		4	/* ���ڱ�ʶ =		*/
#define		XT_STRING		5	/* �ִ�����			*/
#define   XT_SHORT    9 /*  ��д�� />   */
static int xn_strtok( unsigned char *szXML, unsigned char**pStP, int *n, unsigned char**pEndP )
{
unsigned char	*p = szXML;

	while ( *p && isspace(*p) )
		p++;

	if ( !*p )
		return	XT_END;
/*mod by src */
/* if ( *p=='<'&& (*(p+1)=='?' || *(p+1)=='!') )*/
   if ( *p=='<'&& (*(p+1)=='?') )
	{char	ch;
		ch = *(p+1);
		p += 2;
		while ( *p && !(*p==ch&&*(p+1)=='>') )
		{
			p++;
		}
		if ( !*p )
			return	XT_ERROR;
		else
			p+=2;

		while ( *p && isspace(*p) )
			p++;
		if ( !*p )
			return	XT_END;
	}
    /*For omit the xml comment <!-- xxx --> add by src*/
    if (*p == '<' && (*(p + 1) == '!' && *(p + 2) == '-' && *(p + 3) == '-'))
    {
        char   ch;
        ch = *(p + 2);
        p += 4;
        while ( *p && !(*p == ch && *(p + 1) == '-' && *(p + 2) == '>'))
        {
            p++;
        }
        if ( !*p )
        {
            return  XT_ERROR;
        }
        else
        {
            p += 3;
        }

        while ( *p && isspace(*p) )
        {
            p++;
        }
        if ( !*p )
        {
            return  XT_END;
        }
    }
	if ( *p=='<' )
	{
		if ( *(p+1)=='/' )
		{
			*pEndP = p+2;
			return	XT_TAILLEFT;
		}
		else
		{
			*pEndP = p+1;
			return	XT_HEADLEFT;
		}
	}
	else if ( *p=='>' ) 
	{
		*pEndP = p+1;
		return	XT_RIGHT;
	}
	else if (( *p=='/' ) && ( *(p+1)=='>' ))
	{
		*pEndP = p+2;
		return	XT_SHORT;
	}	
	else if ( *p=='=' )
	{
		*pEndP = p+1;
		return	XT_EQUALE;
	}
	else
	{
		*pStP = p;
		while ( *p && !isspace(*p) && *p!='<' && *p!='>' && *p!='=' )
			p++;
		*pEndP = p;
		*n = p - *pStP;
		if (*p == '>' && *(p-1) == '/')   /*��ȡ�������ʱ����XT_SHORT*/
		{
			*n = *n - 1;
			*pEndP= p+1;
			return XT_SHORT;
		}	
		return	XT_STRING;
	}
}

/***************************************************************
 *	����:	xn_strtokdata
 *	����:	ר���ڲ��XML��ʽ�ַ����е�ĳ���ڵ������ֵ
 *	����һ:	XML�ִ�ָ��,���ַ�'<'��ʼ
 *	������:	�����ִ����ݿ�ʼ��ַָ���ָ��
 *	������:	�����ִ����ȵ�ָ��
 *	������:	�����ִ��ֽ����λ�õ�ַָ���ָ��,��'>'����
 *	����ֵ:	���طֽ����ͼ�#define
 *	��ע��	����xn_ImportXMLString����ʹ��
 **************************************************************/
static int xn_strtokdata( unsigned char *szXML, unsigned char**pStP, int *n, unsigned char**pEndP )
{
unsigned char	*p = szXML;
unsigned char	*p2;

  	while ( *p && *p=='\n')
	  	p++;

		p2=p;
	 	while ( *p2 && (*p2=='\n' || *p2==' ' || *p2==9 || *p2==13) ) 
	 	    p2++;
	  	
  
	if ( !*p || *p=='<' || *p=='>' )
		return	XT_ERROR;

	*pStP = p;
	while ( *p && *p!='<' && *p!='>' )
		p++;

	if ( !*p )
		return	XT_ERROR;
  
  /*�ڵ���ڵ�������������,�ǽڵ����� Tianhc 2005-12-01 13:45*/
  if ( *p=='<' && *(p+1)!='/' )
  {
  	 *pStP = p2;
  } 			
   
	*pEndP = p;
	*n = p - *pStP;
	return	XT_STRING;
}


/***************************************************************
 *	����:	xn_strtokattr
 *	����:	ר���ڲ��XML��ʽ�ַ����е�ĳ���ڵ������ֵ
 *	����һ:	XML�ִ�ָ��,���ַ�'='��ʼ
 *	������:	�����ִ����ݿ�ʼ��ַָ���ָ��
 *	������:	�����ִ����ȵ�ָ��
 *	������:	�����ִ��ֽ����λ�õ�ַָ���ָ��,��'"'����
 *	����ֵ:	���طֽ����ͼ�#define
 *	��ע��	����xn_ImportXMLString����ʹ��
 **************************************************************/
static int xn_strtokattr( unsigned char *szXML, unsigned char**pStP, int *n, unsigned char**pEndP )
{
unsigned char	*p = szXML;

	if ( *p != '"' )
		return	XT_ERROR;

	*pStP = p++;

	if ( !*p )
		return	XT_END;

	while ( *p && *p!='<' && *p!='>' && *p!='"' )
		p++;

	if ( *p != '"' )
		return	XT_ERROR;

	*pEndP = ++p;
	*n = p - *pStP;
	return	XT_STRING;
}


/***************************************************************
 *	����:	sl_Move
 *	����:	��һ�����ݱ��е�����Ԫ���ƶ�����һ����
 *	����һ:	��������˳�����Ա�ṹָ��
 *	������:	�Ƴ����ݵ�˳�����Ա�ṹָ��
 *	����ֵ:	�ɹ�,����0��ʧ��,����-1
 *	��ע��	����xn_ImportXMLString����ʹ��
 **************************************************************/
static int sl_Move( PWSEQLIST lpList, PWSEQLIST lpMoveList )
{
	if ( !sl_Length(lpMoveList) )
		return	SUCC;
	if ( lpList->m_nLen+lpMoveList->m_nLen >=lpList->m_nListSize )
	{ /* ��ǰ�洢�ռ����������ӷ��� */
		if ( sl_OwnAppeMem(lpList,lpMoveList->m_nLen)==(-1) ) 
			return FAIL;
	}
	memcpy( lpList->m_lpBase+lpList->m_nLen,
		lpMoveList->m_lpBase,
		lpMoveList->m_nLen*sizeof(SL_ELEMTYPE));

	lpList->m_nLen+=lpMoveList->m_nLen;
	lpMoveList->m_nLen = 0;
	return SUCC;
}

/***************************************************************
 *	����:	xn_ImportXMLString
 *	����:	��XML��ʽ�ַ������뵽XML�ṹָ���ڵ���
 *	����һ:	�����������ݵĽڵ�ָ��
 *	������:	�����XML�ִ�����ָ��
 *	������:	�Ƿ��滻�ýڵ�
  *	������:	XMLָ��
 *	����ֵ:	�ɹ�,����SUCC��ʧ��,����FAIL
 **************************************************************/
#define	XML_LEV	32	/* �����XML�ִ������� */
static int xn_ImportXMLString(PXMLNODE lpOwnNode, char *szImportXML, int bReplace, PXMLSTRUCT pXML )
{
unsigned char	*p,*pStr,*pTp;
int		len,nType,bSetAttr;
PXMLNODE	lpRootNode,lpTpNode;
PXMLNODEATTR lpAttr;

PXMLNODE	lpNode[XML_LEV];
int		lev,lev_save;

	lev=0;
	lev_save = 0;
	p = (unsigned char*)szImportXML;

	/* ������ʱ�ڵ㣬������������ݽڵ�����ʱ���ڸýڵ��� */
	if ( !(lpRootNode=xn_Build((unsigned char*)"__tp__",4,NULL,0)) )
	{
		g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
		if ( pXML->m_dwTrcType&XL_ERROR )
			trc_Write(pXML->m_hTrace,"error: ������ʱ�ڵ�ʧ�� - %s\n",xml_StringError(xml_GetLastError()));
#endif
		return	FAIL;
	}

	lpNode[lev]=lpRootNode;
	while (1) {
		nType = xn_strtok(p,&pStr,&len,&p);
		switch ( nType )
		{
		case	XT_END: /* ����	*/
			if ( lev )
				goto	error_proc;

			if ( bReplace )
			{
				len = sl_Length(&lpRootNode->m_DownList);
				if ( bReplace<0 )
					bReplace = len+1+bReplace;
				if ( !len || len<bReplace )
				{
					g_xmlErrno = XML_EXMLNONODE;
					snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",lpOwnNode->m_pszName);
#ifdef __XML_TRACE__
					if ( pXML->m_dwTrcType&XL_ERROR )
						trc_Write(pXML->m_hTrace,"error: �滻�ڵ�[%s]ʧ�� - %s\n",lpOwnNode->m_pszName,xml_StringError(xml_GetLastError()));
#endif
					goto	error_proc;
				}
				sl_Delete(&lpRootNode->m_DownList,bReplace,(void**)&lpTpNode);
				/* �滻�ڵ� */
				xn_Move(lpOwnNode,lpTpNode);
			}
			else
			{
				/* ��������ȫ����ɺ������ʱ�ڵ��µ����нڵ�����ת�Ƶ���ʽ�ڵ��� */
				if ( sl_Move(&lpOwnNode->m_DownList,&lpRootNode->m_DownList)==FAIL )
				{
					g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
					if ( pXML->m_dwTrcType&XL_ERROR )
						trc_Write(pXML->m_hTrace,"error: ��ʱ�ڵ�����ת��ʧ�� - %s\n",xml_StringError(xml_GetLastError()));
#endif
					goto	error_proc;
				}
			}
			xn_Destroy(lpRootNode);
			return	SUCC;

		case	XT_HEADLEFT: /* ��ʼ���ʶ < */
			bSetAttr = 0;
			nType=xn_strtok(p,&pStr,&len,&p);
			if (( nType!= XT_STRING ) && ( nType!= XT_SHORT ))
			{
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ��%d�ֽ�ǰ����ӦΪ��ʼ��ʶ�ִ� - %s\n",(char*)p-szImportXML,xml_StringError(XML_EXMLSYNTAX));
#endif
				goto	error_proc;
			}

			if ( !(lpTpNode=xn_Build(pStr,len,NULL,0)) )
			{
				g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ����ڵ�'%m'ʧ�� - %s\n",pStr,len,(char*)p-szImportXML,xml_StringError(XML_ENOMEM));
#endif
				goto	error_proc;
			}

			if ( sl_Append(&lpNode[lev]->m_DownList,(void*)lpTpNode)==-1 )
			{
				g_xmlErrno = XML_ENOMEM;
				xn_Destroy(lpTpNode);
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ��ӽڵ�'%s'ʧ�� - %s\n",lpTpNode->m_pszName,(char*)p-szImportXML,xml_StringError(XML_ENOMEM));
#endif
				goto	error_proc;
			}
  
			if ( ++lev>XML_LEV )
			{/* ���������� */
				g_xmlErrno = XML_EXMLMAXLEV;
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ��%d�ֽڿ�ʼ���� - %s\n",(char*)p-szImportXML,xml_StringError(XML_EXMLMAXLEV));
#endif
				goto	error_proc;
			}
			lpNode[lev]=lpTpNode;

      if (nType == XT_SHORT)   /*���������ֻ��ӽ�㼴��*/
      {
      	lev--;
      	break;
      }		

			while (1)
			{  /* �ֽ����� */
				nType = xn_strtok(p,&pStr,&len,&p);
				if ( nType==XT_RIGHT )
				{ /* ���������� */
					break;
				}
				else if ( nType==XT_SHORT )  /*����/>--XML2.0*/
				{
					 break;
				}	
				else if ( nType==XT_STRING )
				{ /* ������ */
					if ( !bSetAttr )
						bSetAttr = 1;

					if ( !(lpAttr=xna_Build(pStr,len,NULL)) )
					{
						g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
						if ( pXML->m_dwTrcType&XL_ERROR )
							trc_Write(pXML->m_hTrace,"error: ��������'%m'ʧ�� - %s\n",pStr,len,(char*)p-szImportXML,xml_StringError(XML_ENOMEM));
#endif
						goto	error_proc;
					}
					if ( sl_Append(&lpNode[lev]->m_AttrList,(void*)lpAttr)==-1 )
					{
						g_xmlErrno = XML_ENOMEM;
						xna_Destroy(lpAttr);
#ifdef __XML_TRACE__
						if ( pXML->m_dwTrcType&XL_ERROR )
							trc_Write(pXML->m_hTrace,"error: �������'%s'ʧ�� - %s\n",lpAttr->m_pszName,(char*)p-szImportXML,xml_StringError(XML_ENOMEM));
#endif
						goto	error_proc;
					}

					if ( xn_strtok(p,&pStr,&len,&p)!=XT_EQUALE )
					{ /* ��ֵ��'=' */
#ifdef __XML_TRACE__
					if ( pXML->m_dwTrcType&XL_ERROR )
						trc_Write(pXML->m_hTrace,"error: ��%d�ֽ�ǰ����ӦΪ���Ը�ֵ����'=' - %s\n",(char*)p-szImportXML,xml_StringError(XML_EXMLSYNTAX));
#endif
						goto	error_proc;
					}
					if ( xn_strtokattr(p,&pStr,&len,&p)!=XT_STRING || *pStr!='\"' || *(pStr+len-1)!='\"' )
					{ /* ����ֵ */
#ifdef __XML_TRACE__
						if ( pXML->m_dwTrcType&XL_ERROR )
							trc_Write(pXML->m_hTrace,"error: ��%d�ֽ� ����ֵ����'%m'ȱ��˫���� - %s\n",(char*)pStr-szImportXML,pStr,len,xml_StringError(XML_EXMLSYNTAX));
#endif
						goto	error_proc;
					}

					if ( xna_SetDataWithConvert(lpAttr,pStr+1,len-2)==FAIL )
					{ /* ��������ֵ */
						g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
						if ( pXML->m_dwTrcType&XL_ERROR )
							trc_Write(pXML->m_hTrace,"error: ��������'%s' ֵ '%m' ʧ�� - %s\n",lpAttr->m_pszName,pStr+1,len-2,(char*)p-szImportXML,xml_StringError(XML_ENOMEM));
#endif
						goto	error_proc;
					}
				}
				else
				{
#ifdef __XML_TRACE__
					if ( pXML->m_dwTrcType&XL_ERROR )
						trc_Write(pXML->m_hTrace,"error1: ��%d�ֽ�ǰ���ݲ��ܽ��� - %s\n",(char*)p-szImportXML,xml_StringError(XML_EXMLSYNTAX));
#endif
					goto	error_proc;
				}
			}
      if ( nType==XT_SHORT )  /*������ദ��*/
      {
      	 lev--;
      	 break;
      }		

			/* �������ֵ */
			if ( xn_strtokdata(p,&pStr,&len,&pTp) == XT_STRING )
			{ /* ��������ֵ */
				p = pTp;
#ifdef __ContrlAttr__ /* ������ */
				if ( bSetAttr )
				{ /* �ýڵ��Ѿ��������ԣ�������Ϊ���ݽڵ� */
#ifdef __XML_TRACE__
					if ( pXML->m_dwTrcType&XL_ERROR )
						trc_Write(pXML->m_hTrace,"error: ��%d�ֽ� ����'%m' �Ѿ������������� - %s\n",(char*)pStr-szImportXML,pStr,len,xml_StringError(XML_EXMLSYNTAX));
#endif
					goto	error_proc;
				}
#endif

				/* ����ת�������� */
				if ( xn_SetDataWithConvert(lpNode[lev],pStr,len)==FAIL )
				{
					g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
					if ( pXML->m_dwTrcType&XL_ERROR )
						trc_Write(pXML->m_hTrace,"error: ���ýڵ�'%s' ֵ '%m' ʧ�� - %s\n",lpNode[lev]->m_pszName,pStr,len,(char*)p-szImportXML,xml_StringError(XML_ENOMEM));
#endif
					goto	error_proc;
				}
				    lev_save = lev;

			}
			break;

		case	XT_TAILLEFT: /* �������ʶ </ */
			if ( xn_strtok(p,&pStr,&len,&p)!= XT_STRING )
			{
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ��%d�ֽ�ǰ����ӦΪ������ʶ�ִ� - %s\n",(char*)p-szImportXML,xml_StringError(XML_EXMLSYNTAX));
#endif
				goto	error_proc;
			}

			if ( memicmp_(lpNode[lev]->m_pszName,pStr,len) )
			{
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ��%d�ֽ�����'%m'����ƥ�����,ӦΪ'%s' - %s\n",(char*)pStr-szImportXML,pStr,len,lpNode[lev]->m_pszName,xml_StringError(XML_EXMLSYNTAX));
#endif
				goto	error_proc;
			}

			if ( xn_strtok(p,&pStr,&len,&p)!= XT_RIGHT )
			{
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ��%d�ֽ�ǰ����ӦΪ������ʶ'>' - %s\n",(char*)p-szImportXML,xml_StringError(XML_EXMLSYNTAX));
#endif
				goto	error_proc;
			}
			lev--;
			break;

		default:
			g_xmlErrno = XML_EXMLSYNTAX;
#ifdef __XML_TRACE__
			if ( pXML->m_dwTrcType&XL_ERROR )
				trc_Write(pXML->m_hTrace,"error2: [%d]��%d�ֽ�ǰ���ݲ��ܽ��� - %s\n",nType,(char*)p-szImportXML,xml_StringError(XML_EXMLSYNTAX));
#endif
error_proc:
#ifdef __XML_TRACE__
			if ( pXML->m_dwTrcType&XL_ERROR )
				trc_Write(pXML->m_hTrace,"��������λ��Ϊ�����Ѿ�ɨ����ַ�ǰ��\n%h%m%H",szImportXML,(char*)p-szImportXML);
#endif
			xn_Destroy(lpRootNode);
			return	FAIL;	
		}
	}

/*	return	SUCC; */
}


/***************************************************************
 *	����:	xn_ImportXMLStringKVL
 *	����:	��KVL��ʽ�ַ������뵽XML�ṹָ���ڵ���
 *	����һ:	�����������ݵĽڵ�ָ��
 *	������:	�����XML�ִ�����ָ��
 *	������:	�Ƿ��滻�ýڵ�
  *	������:	XMLָ��
 *	����ֵ:	�ɹ�,����SUCC��ʧ��,����FAIL
 **************************************************************/
static int xn_ImportXMLStringKVL(PXMLNODE lpOwnNode, char *szImportXML, int bReplace, PXMLSTRUCT pXML )
{
unsigned char	*p,*pStr,*pTp;
int		len,nType,bSetAttr;
PXMLNODE	lpRootNode,lpTpNode;
PXMLNODEATTR lpAttr;

PXMLNODE	lpNode[XML_LEV];
int		lev,lev_save;

	lev=0;
	lev_save = 0;
	p = (unsigned char*)szImportXML;

	/* ������ʱ�ڵ㣬������������ݽڵ�����ʱ���ڸýڵ��� */
	if ( !(lpRootNode=xn_Build((unsigned char*)"__tp__",4,NULL,0)) )
	{
		g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
		if ( pXML->m_dwTrcType&XL_ERROR )
			trc_Write(pXML->m_hTrace,"error: ������ʱ�ڵ�ʧ�� - %s\n",xml_StringError(xml_GetLastError()));
#endif
		return	FAIL;
	}

	lpNode[lev]=lpRootNode;
	while (1) {
		nType = xn_strtok(p,&pStr,&len,&p);
		switch ( nType )
		{
		case	XT_END: /* ����	*/
			if ( lev )
				goto	error_proc;

			if ( bReplace )
			{
				len = sl_Length(&lpRootNode->m_DownList);
				if ( bReplace<0 )
					bReplace = len+1+bReplace;
				if ( !len || len<bReplace )
				{
					g_xmlErrno = XML_EXMLNONODE;
					snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",lpOwnNode->m_pszName);
#ifdef __XML_TRACE__
					if ( pXML->m_dwTrcType&XL_ERROR )
						trc_Write(pXML->m_hTrace,"error: �滻�ڵ�[%s]ʧ�� - %s\n",lpOwnNode->m_pszName,xml_StringError(xml_GetLastError()));
#endif
					goto	error_proc;
				}
				sl_Delete(&lpRootNode->m_DownList,bReplace,(void**)&lpTpNode);
				/* �滻�ڵ� */
				xn_Move(lpOwnNode,lpTpNode);
			}
			else
			{
				/* ��������ȫ����ɺ������ʱ�ڵ��µ����нڵ�����ת�Ƶ���ʽ�ڵ��� */
				if ( sl_Move(&lpOwnNode->m_DownList,&lpRootNode->m_DownList)==FAIL )
				{
					g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
					if ( pXML->m_dwTrcType&XL_ERROR )
						trc_Write(pXML->m_hTrace,"error: ��ʱ�ڵ�����ת��ʧ�� - %s\n",xml_StringError(xml_GetLastError()));
#endif
					goto	error_proc;
				}
			}
			xn_Destroy(lpRootNode);
			return	SUCC;

		case	XT_HEADLEFT: /* ��ʼ���ʶ < */
			bSetAttr = 0;
			nType=xn_strtok(p,&pStr,&len,&p);
			if (( nType!= XT_STRING ) && ( nType!= XT_SHORT ))
			{
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ��%d�ֽ�ǰ����ӦΪ��ʼ��ʶ�ִ� - %s\n",(char*)p-szImportXML,xml_StringError(XML_EXMLSYNTAX));
#endif
				goto	error_proc;
			}

			if ( !(lpTpNode=xn_Build(pStr,len,NULL,0)) )
			{
				g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ����ڵ�'%m'ʧ�� - %s\n",pStr,len,(char*)p-szImportXML,xml_StringError(XML_ENOMEM));
#endif
				goto	error_proc;
			}

			if ( sl_Append(&lpNode[lev]->m_DownList,(void*)lpTpNode)==-1 )
			{
				g_xmlErrno = XML_ENOMEM;
				xn_Destroy(lpTpNode);
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ��ӽڵ�'%s'ʧ�� - %s\n",lpTpNode->m_pszName,(char*)p-szImportXML,xml_StringError(XML_ENOMEM));
#endif
				goto	error_proc;
			}
  
			if ( ++lev>XML_LEV )
			{/* ���������� */
				g_xmlErrno = XML_EXMLMAXLEV;
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ��%d�ֽڿ�ʼ���� - %s\n",(char*)p-szImportXML,xml_StringError(XML_EXMLMAXLEV));
#endif
				goto	error_proc;
			}
			lpNode[lev]=lpTpNode;

      if (nType == XT_SHORT)   /*���������ֻ��ӽ�㼴��*/
      {
      	lev--;
      	break;
      }		

			while (1)
			{  /* �ֽ����� */
				nType = xn_strtok(p,&pStr,&len,&p);
				if ( nType==XT_RIGHT )
				{ /* ���������� */
					break;
				}
				else if ( nType==XT_SHORT )  /*����/>--XML2.0*/
				{
					 break;
				}	
				else if ( nType==XT_STRING )
				{ /* ������ */
					if ( !bSetAttr )
						bSetAttr = 1;

					if ( !(lpAttr=xna_Build(pStr,len,NULL)) )
					{
						g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
						if ( pXML->m_dwTrcType&XL_ERROR )
							trc_Write(pXML->m_hTrace,"error: ��������'%m'ʧ�� - %s\n",pStr,len,(char*)p-szImportXML,xml_StringError(XML_ENOMEM));
#endif
						goto	error_proc;
					}
					if ( sl_Append(&lpNode[lev]->m_AttrList,(void*)lpAttr)==-1 )
					{
						g_xmlErrno = XML_ENOMEM;
						xna_Destroy(lpAttr);
#ifdef __XML_TRACE__
						if ( pXML->m_dwTrcType&XL_ERROR )
							trc_Write(pXML->m_hTrace,"error: �������'%s'ʧ�� - %s\n",lpAttr->m_pszName,(char*)p-szImportXML,xml_StringError(XML_ENOMEM));
#endif
						goto	error_proc;
					}

					if ( xn_strtok(p,&pStr,&len,&p)!=XT_EQUALE )
					{ /* ��ֵ��'=' */
#ifdef __XML_TRACE__
					if ( pXML->m_dwTrcType&XL_ERROR )
						trc_Write(pXML->m_hTrace,"error: ��%d�ֽ�ǰ����ӦΪ���Ը�ֵ����'=' - %s\n",(char*)p-szImportXML,xml_StringError(XML_EXMLSYNTAX));
#endif
						goto	error_proc;
					}
					if ( xn_strtokattr(p,&pStr,&len,&p)!=XT_STRING || *pStr!='\"' || *(pStr+len-1)!='\"' )
					{ /* ����ֵ */
#ifdef __XML_TRACE__
						if ( pXML->m_dwTrcType&XL_ERROR )
							trc_Write(pXML->m_hTrace,"error: ��%d�ֽ� ����ֵ����'%m'ȱ��˫���� - %s\n",(char*)pStr-szImportXML,pStr,len,xml_StringError(XML_EXMLSYNTAX));
#endif
						goto	error_proc;
					}

					if ( xna_SetDataWithConvert(lpAttr,pStr+1,len-2)==FAIL )
					{ /* ��������ֵ */
						g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
						if ( pXML->m_dwTrcType&XL_ERROR )
							trc_Write(pXML->m_hTrace,"error: ��������'%s' ֵ '%m' ʧ�� - %s\n",lpAttr->m_pszName,pStr+1,len-2,(char*)p-szImportXML,xml_StringError(XML_ENOMEM));
#endif
						goto	error_proc;
					}
				}
				else
				{
#ifdef __XML_TRACE__
					if ( pXML->m_dwTrcType&XL_ERROR )
						trc_Write(pXML->m_hTrace,"error1: ��%d�ֽ�ǰ���ݲ��ܽ��� - %s\n",(char*)p-szImportXML,xml_StringError(XML_EXMLSYNTAX));
#endif
					goto	error_proc;
				}
			}
      if ( nType==XT_SHORT )  /*������ദ��*/
      {
      	 lev--;
      	 break;
      }		

			/* �������ֵ */
			if ( xn_strtokdata(p,&pStr,&len,&pTp) == XT_STRING )
			{ /* ��������ֵ */
				p = pTp;
#ifdef __ContrlAttr__ /* ������ */
				if ( bSetAttr )
				{ /* �ýڵ��Ѿ��������ԣ�������Ϊ���ݽڵ� */
#ifdef __XML_TRACE__
					if ( pXML->m_dwTrcType&XL_ERROR )
						trc_Write(pXML->m_hTrace,"error: ��%d�ֽ� ����'%m' �Ѿ������������� - %s\n",(char*)pStr-szImportXML,pStr,len,xml_StringError(XML_EXMLSYNTAX));
#endif
					goto	error_proc;
				}
#endif

				/* ����ת�������� */
				if ( xn_SetDataWithConvert(lpNode[lev],pStr,len)==FAIL )
				{
					g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
					if ( pXML->m_dwTrcType&XL_ERROR )
						trc_Write(pXML->m_hTrace,"error: ���ýڵ�'%s' ֵ '%m' ʧ�� - %s\n",lpNode[lev]->m_pszName,pStr,len,(char*)p-szImportXML,xml_StringError(XML_ENOMEM));
#endif
					goto	error_proc;
				}
				    lev_save = lev;

			}
			break;

		case	XT_TAILLEFT: /* �������ʶ </ */
			if ( xn_strtok(p,&pStr,&len,&p)!= XT_RIGHT )
			{
#ifdef __XML_TRACE__
				if ( pXML->m_dwTrcType&XL_ERROR )
					trc_Write(pXML->m_hTrace,"error: ��%d�ֽ�ǰ����ӦΪ������ʶ'>' - %s\n",(char*)p-szImportXML,xml_StringError(XML_EXMLSYNTAX));
#endif
				goto	error_proc;
			}
			lev--;
			break;

		default:
			g_xmlErrno = XML_EXMLSYNTAX;
#ifdef __XML_TRACE__
			if ( pXML->m_dwTrcType&XL_ERROR )
				trc_Write(pXML->m_hTrace,"error2: [%d]��%d�ֽ�ǰ���ݲ��ܽ��� - %s\n",nType,(char*)p-szImportXML,xml_StringError(XML_EXMLSYNTAX));
#endif
error_proc:
#ifdef __XML_TRACE__
			if ( pXML->m_dwTrcType&XL_ERROR )
				trc_Write(pXML->m_hTrace,"��������λ��Ϊ�����Ѿ�ɨ����ַ�ǰ��\n%h%m%H",szImportXML,(char*)p-szImportXML);
#endif
			xn_Destroy(lpRootNode);
			return	FAIL;	
		}
	}

/*	return	SUCC; */
}


/**********************************************************************
 *	����:	splitstr
 *	����:	��һ���ַ��������ݿո������������
 *	����һ:	�ַ�������ָ��
 *	������:	�������ַ�����ָ������
 *	������:	ָ�������С
 *	����ֵ:	�ɹ�,����ʵ�ʷ�����������;ʧ�ܷ���0
 *	��ע��	����xn_ExportDSRString,xn_ImportDSRString����ʹ��
 **********************************************************************/
static int splitstr( unsigned char *szData, unsigned char *szField[], size_t num )
{
size_t		n=(int)num;
unsigned char *pszStr = (unsigned char*)szData;

	if( !pszStr || !pszStr[0] || !szField || !n )
		return	0;

	*szField++ = pszStr;
	while ( *pszStr && n ) {
		if ( *pszStr<127 && isspace(*pszStr) )
		{
			*pszStr++ = '\0';
			while ( isspace(*pszStr) )
				pszStr++;
			if ( --n )
				*szField++ = pszStr;
		}
		else
			pszStr++;
	}
	*pszStr='\0';

	return	num-n;
}


/**********************************************************************
 *	����:	strchg_
 *	����:	�����ַ������������е�ת���ַ�
 *	����һ:	�ַ���ָ��
 *	������:	����������ַ����Ļ�����ָ��
 *	������:	��������С
 *	����ֵ:	�ɹ�,�����ַ�����;ʧ�ܷ���-1
 **********************************************************************/
static int strchg_( char *pStr, char *pBuf, size_t size )
{
char    *p,*q;
size_t	n;
int		i,data;

    p = pStr;
	q = pBuf;
	n = size;
    
/*	while ( *p && n > 1 ) */
	while ( *p && n )
	{
		if ( *p == '\\' )
		{
			if ( p[1]=='\r' && p[2]=='n' )
			{	/* �س����� */
				p+=3;
			}
			else if ( p[1]=='\n' )
			{ /* ���� */
				p+=2;
			}
			else
			{
				if ( p[1]=='n' || p[1]=='N' )
				{
					p+=2; *q++ = '\n';
				}
				else if ( p[1]=='t' || p[1]=='T' )
				{
					p+=2; *q++ = '\t';
				}
				else if ( p[1]=='v' || p[1]=='V' )
				{
					p+=2; *q++ = '\v';
				}
				else if ( p[1]=='r' || p[1]=='R' )
				{
					p+=2; *q++ = '\r';
				}
				else if ( p[1]=='0' && (p[2]=='x'||p[2]=='X') )
				{	/* 16���������� */
					p+=2;data=0;
					for ( p++,data=i=0; i<2; i++,p++ )
					{
						if ( isdigit(*p) )
						{
							data = data*16 + *p-'0';
						}
						else if ( *p>='A' && *p<='F' )
						{
							data = data*16 + *p - 55;
						}
						else if ( *p>='a' && *p<='f' )
						{
							data = data*16 + *p - 87;
						}
						else
						{
							break;
						}
					}
					if ( i )
					{
						*q++=data;
					}
				}
				else
				{/* 10���������� */
					for ( p++,data=i=0; i<3; i++,p++ )
					{
						if ( isdigit(*p) )
						{
							data = data*10+*p-'0';
						}
						else
						{
							break;
						}
					}
					if ( !i )
					{
						*q++ = *p++;
					}
					else if (data>256)
					{
						*q++ = data/10;
						p--;
					}
					else
					{
						*q++ = data;
					}
				}
				n--;
			}
		}
		else
		{
			*q++ = *p++;
			n--;
		}
	}
	*q = '\0';
	return	size-n;
}

/**********************************************************************
 *	����:	xn_Setquot
 *	����:	�����ڲ���"�ַ���ת����׼,1-����Ϊ&quot;(XML��׼),0-����Ϊ&quote;��ϵͳ����ı�׼
 *	����һ:	ģʽ
 *	��ע��	ֻΪ���ݾ�ģʽ��һ������
 *       �����Ϊ�±�׼�����ϵͳ��ϲ����±���,����ASPK+IFI����Ӧ��,�нϴ�������׳�����
 **********************************************************************/
static void xn_Setquot( int imode )
{
  g_xmlquot = imode;
}

#ifdef __cplusplus
}
#endif /* end of __cplusplus */


#endif /* end of  __XMLNODE_C__ */


/**********************************************************************
 *	������	xn_Rename
 *	����:	�ڵ����
 *	����:
	pNode
		�ڵ�ṹָ�롣
	sname_new
		�µĽڵ�����
 *	����ֵ:
	�ɹ�	����SUCC
	ʧ��	����FAIL
 **********************************************************************/
static int xn_Rename( PXMLNODE pNode, char *sname_new)
{
	if ((pNode == NULL ) || (sname_new[0] == 0))
		 return -1;
	free( pNode->m_pszName );
	if ( !(pNode->m_pszName=(unsigned char*)malloc(strlen(sname_new)+1)) )
     return -2;
	strcpy(pNode->m_pszName,sname_new);
	pNode->m_pszName[strlen(sname_new)]='\0';
  return 0;   	
}	
