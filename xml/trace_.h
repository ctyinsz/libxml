/************************************************************************
��Ȩ����:���ݺ������ӹɷ����޹�˾
��Ŀ����:������Ŀ
��    ��:V1.0
����ϵͳ:AIX4.2,SCO5
�ļ�����:trace.h
�ļ�����:��XML���������⸨�����ٺ���
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:�м�ҵ���Ʒ�з���
��������:2001��09��11��
��    ��:�м�ҵ���Ʒ�з���
�޸�����:2002��05��12��
************************************************************************/
/*
�޸ļ�¼
�޸�����:
�޸�����:
�޸���:
*/
#ifndef __TRACE_H__
#define __TRACE_H__

#include  <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* end of __cplusplus */


typedef int  HTRACE;


/* ����TRACE�ṹ */
HTRACE trc_Build( char* pszFile, char *pszBakDir );

/* ����TRACE�ṹ */
int trc_Destroy( HTRACE hTrace );

/* ���û�ȡ���� */
#define  TFS_ALWAYSOPEN  1 /* �ļ��Ƿ�һֱ�� */
#define  TFS_FLUSH    2 /* �ļ��Ƿ��޻��壨����ˢ�£�*/
#define  TFS_MAXSIZE    3 /* �ļ���¼��󳤶� */
#define  TFS_MAXHOUR    4 /* �ļ���¼���Сʱ�� */
#define  TFS_DAYTIME    5 /* ��¼�ļ�ÿ��ת��ʱ�� */
#define  TFS_WEEKDAY    6 /* ��¼�ļ�ÿ��ת��ʱ�� */
#define  TFS_MONTHDAY  7 /* ��¼�ļ�ÿ��ת��ʱ�� */
int trc_SetAttr( HTRACE hTrace, int iAttrType, unsigned int uiAttrData );
int trc_GetAttr( HTRACE hTrace, int iAttrType, unsigned int *puiAttrData );

/* ��¼TRACE */
int trc_Write( HTRACE hTrace, char *pszFmt, ... );


/* ǿ�����TRACE�ļ��ƶ� */
int trc_MoveFile( HTRACE hTrace );

/* ��ȡ�汾�� */
unsigned int trc_GetVerion();

#ifdef __cplusplus
}
#endif /* end of __cplusplus */



#endif /* end of __TRACE_H__ */
