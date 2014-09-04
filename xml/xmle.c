/************************************************************************
��Ȩ����:���ݺ������ӹɷ����޹�˾
��Ŀ����:������Ŀ
��    ��:V1.0
����ϵͳ:AIX4.2,SCO5
�ļ�����:xmle.c
�ļ�����:��XML������������չ����
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:�м�ҵ���Ʒ�з���
��������:2001��09��11��
�޸�����:2002��05��12��
************************************************************************/
/*
�޸ļ�¼
�޸�����:
�޸�����:����ֱ��XML�ļ����뵼��������
�޸���:������

�޸�����:2002.07.18
�޸�����:����ʽ��Դxml�����ݵ�Ŀ��xml��:���� xml_TreeUpdata
		 ������ýӿڲμ�����˵��
�޸���:�Ʒ�

�޸�����:2005-08-09 14:51
�޸�����:���Ӻ�����
	   xml_XCopy			���ڽ��ĸ���(��������)
	   xml_node_copy	����Ҷ�ӽڵ�ĸ���(��������)
	   xml_Conv2to1		���ڽ�XML 2.0��ʽתΪ XML 1.0��ʽ
�޸���:�ﻶ��

*/
#include  "xmlnode.c"

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xml.h"
#include "xmle.h"

PXMLNODE xml_GetNode( HXMLTREE hXML, const char *szElementName);


#define  MAX_NAMEPATH 1024
/******************************************************************************
����:xml_GetElemntE
����:��XML���ݷ��ʷ�װ,����·�����ʽڵ������
����:
    hXML
                ָ��XML�豸���
        szElementName

              �ڵ�·����ʾ:

                ָ����XML�ṹҪ��ȡ��Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
                "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
                NMAE    ָ��ÿһ��Ԫ����,
                IDX             ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
                        ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
                        ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
                        ȱʡΪ1
              ����·����ʾ:
                ���ɽڵ�ָ������ָ����XML�ṹ�еĽڵ㡣
                ��ͨ��@����ָ���ڵ�����ԡ�
                ����"/PRIVATE|2/��ѵ���@��λ
                "/PRIVATE|2/��ѵ���"ָ���ڵ�·����"@��λ"ָ���ڵ����ԡ�
        pucElementBuff
                ���ջ�ȡԪ��ֵ�Ļ�����ָ��
        iElementBuffSize
                ��������С
����ֵ:
        �ɹ�    ����SUCC
        ʧ��    ����FAIL
******************************************************************************/
int xml_GetElementE(HXMLTREE hXML,
                   const char *szElementName,
                   char *pucElementBuff,
                   const size_t uiElementBuffSize )
{
char *ptmp;
char stmp[MAX_NAMEPATH];
  memset(stmp,0,sizeof(stmp));
  memset(pucElementBuff,0,uiElementBuffSize);
  if((ptmp=strchr(szElementName,'@'))!=NULL)
  {
      memcpy(stmp,szElementName,ptmp-szElementName);
      return xml_GetElementAttr( hXML,stmp,ptmp+1, \
                        pucElementBuff,uiElementBuffSize);
  }
  else
    return xml_GetElement(hXML,szElementName,pucElementBuff,uiElementBuffSize);
}

/******************************************************************************
����:xml_SetElemntE
����:��XML���ݷ��ʷ�װ,����·�����ýڵ������ֵ
����:
*      ����:    ��XML��������һ��Ҷ�ӽڵ��ĳһ�ڵ��ϵ�����
                Ҷ�ӽڵ�(���ݽڵ�)�����ظ�����
                ����ýڵ㲻����, ��Ӹýڵ�;
                        ���·���ڵ㲻����, ����ͬ��Щ�ڵ�һ�����
                ����ýڵ��Ѵ���, �޸ĸýڵ�����;
 *      ����:
        hXML
                ָ��XML�豸���
        szElementName
              �ڵ�·����ʾ:

                ָ����XML�ṹҪ��ȡ��Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
                "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
                NMAE    ָ��ÿһ��Ԫ����,
                IDX             ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
                        ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
                        ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
                        ȱʡΪ1
              ����·����ʾ:
                ���ɽڵ�ָ������ָ����XML�ṹ�еĽڵ㡣
                ��ͨ��@����ָ���ڵ�����ԡ�
                ����"/PRIVATE|2/��ѵ���@��λ
                "/PRIVATE|2/��ѵ���"ָ���ڵ�·����"@��λ"ָ���ڵ����ԡ�
        szElementValue
                �����õ�Ԫ��ֵָ��
                �ڵ��Ϊ���ݽڵ�����Խڵ�

����ֵ:
        �ɹ�    ����SUCC
        ʧ��    ����FAIL
*****************************************************************************/
int xml_SetElementE( HXMLTREE hXML,
                   const char *szElementName,
                   const char *szElementValue )
{
char *ptmp;
char stmp[MAX_NAMEPATH];
    memset(stmp,0,sizeof(stmp));
    if((ptmp=strchr(szElementName,'@'))!=NULL)
    {
        memcpy(stmp,szElementName,ptmp-szElementName);
        /*  Ŀ�Ľڵ㲻���� */
        if(xml_ElementExist(hXML,stmp )==0)
        {
            if(xml_AddElement(hXML,stmp,"") == FAIL)
              return FAIL;
        }
        return xml_SetElementAttr( hXML,stmp,ptmp+1, \
                          szElementValue);
    }
    else return xml_SetElement(hXML,szElementName,szElementValue);
}

/***************************************************************************
����:����xml����Ŀֵ��ת��ʵ��ASCII�����������ַ�hex�洢��
����:  HXMLTREE hXML   xml�ľ��ֵ
    const char *szElementName �ڵ�·����ʾ
    const *szElementValue,  ԭֵ����
    int szElementLen    ԭֵ����
����ֵ:
        �ɹ�    ����SUCC
        ʧ��    ����FAIL
***************************************************************************/
int xml_SetBinElement(HXMLTREE hXML,
           const char *szElementName,
           char *szElementValue,
           int szElementLen)
{
  char * ptmp;
  if((ptmp=(char *)malloc(szElementLen*2+1))==NULL)
    return FAIL;
  memset(ptmp,0,szElementLen*2+1);
  if(xml_asctohex(ptmp,szElementLen*2,szElementValue,szElementLen)==0)
  {
    free(ptmp);
    return FAIL;
  }
  if(xml_SetElement(hXML,szElementName,ptmp)== FAIL)
  {
    free(ptmp);
    return FAIL;
  }
  free(ptmp);
  return SUCC;
}

/****************************************************************************
����:��xml�����ַ�hex�洢������ת��Ϊʵ�ʵ�ASCII��������
����:  HXMLTREE hXML   XML���
    const *szElementName  �ڵ�·����ʾ
    char *pucElementBuff, ȡֵ����
    const size_t uiElementBuffSize  ���峤��
����ֵ:
        >=0 ȡ��ֵ,�����뻺��ĳ���
        ʧ��    ����FAIL
*****************************************************************************/
int xml_GetBinElement(HXMLTREE hXML,
           const char *szElementName,
           char *pucElementBuff,
                     const size_t uiElementBuffSize )
{
  char * ptmp;
  int ilen;
  memset( pucElementBuff,0,uiElementBuffSize);
  if((ptmp=(char *)malloc(uiElementBuffSize*2+1))==NULL)
    return FAIL;
  memset(ptmp,0,uiElementBuffSize*2+1);
  ilen=uiElementBuffSize*2;
  if(xml_GetElement(hXML,szElementName,ptmp,ilen)== FAIL)
  {
    free(ptmp);
    return FAIL;
  }
  ilen=strlen(ptmp);
  if((ilen=xml_hextoasc(pucElementBuff,uiElementBuffSize,ptmp,ilen)) == 0)
  {
    free(ptmp);
    return FAIL;
  }
  free(ptmp);
  return ilen;
}
/******************************************************************************
����:������xml���������ݵ�ָ���ڵ㣬����ýڵ㲻����������
����:  HXMLTREE hXML   XML���
      const char *szImportXML,
      const char *szImportName,
      int bReplace ����ֵ:>=0  ȡ��ֵ,�����뻺��ĳ���
����:
        �ɹ�    ����SUCC
        ʧ��    ����FAIL
*******************************************************************************/
int xml_ImportXMLStringE( HXMLTREE hXML,
      const char *szImportXML,
      const char *szImportName)
{
char sPath[MAX_NAMEPATH];
int icount;
    if((icount=xml_ElementCount(hXML,szImportName)) == FAIL)
      return FAIL;
    sprintf(sPath,"%s|%d",szImportName,icount+1);
    /*  ������Ŀ�Ľڵ�  */
    if(xml_AddElement(hXML,sPath,"") == FAIL)
      return FAIL;
    if(xml_ImportXMLString(hXML,szImportXML,sPath,0) == FAIL)
      return FAIL;
    return SUCC;
}
/******************************************************************************
����:��������Ĵ��е�XML��Ŀ����ȷ��ֵ�����滻��
����:  HXMLTREE hXML   XML���
      const char *pstrbuf, ԭ���봮����
      int isize;�����С��
    ���'[' ����ָ�ڵ㲻�����򷵻ء���.����'\'Ϊ'[',']'��ת�����
����:
        �ɹ�    ����SUCC
        ʧ��    ����FAIL
*******************************************************************************/
int xml_ParseXMLString(HXMLTREE hXML,char * sexp,int isize)
{
  int istatu=0;
  int ilenth,i,j=0,k=0,imax;
  char *stmp,*sresult,clast=0;

  imax=isize;
  ilenth=strlen(sexp);
  if((stmp=(char *)malloc(isize))==NULL)
    return FAIL;
  memset(stmp,0,isize);
  if((sresult=(char *)malloc(isize))==NULL)
  {
    free(stmp);
    return FAIL;
  }
  memset(sresult,0,isize);

  for(i=0;i<ilenth;i++)
  {
    if((sexp[i]=='[')&&( clast!='\\'))
    {
      if(istatu==0)
      {
        memset(stmp,0,isize);
        j=0;
      }
      else
        stmp[j++]=sexp[i];
      istatu++;
    }
    else if((sexp[i]==']' )&&(clast!='\\'))
    {
      istatu--;
      if(istatu==0)
      {
        if((xml_ItemToString(hXML,stmp,isize)) == FAIL)
        {
          free(sresult);
          free(stmp);
          return FAIL;
        }
        k+=strlen(stmp);
        if(k>isize){
          if( realloc(sresult,k+isize)==NULL)
          {
            free(sresult);
            free(stmp);
            return FAIL;
          }
          if( realloc(stmp,k+isize)==NULL)
          {
            free(sresult);
            free(stmp);
            return FAIL;
          }
          isize=k+isize;
        }
        strcat(sresult,stmp);
      }
      else stmp[j++]=sexp[i];
    }
    else
    {
      if(((sexp[i]=='[')&&(clast=='\\'))||((sexp[i]==']')&&(clast=='\\')))
      {
        if(istatu!=0)
          stmp[j-1]=sexp[i];
        else sresult[k-1]=sexp[i];
      }
      else{
        if(istatu!=0)
          stmp[j++]=sexp[i];
        else sresult[k++]=sexp[i];
      }
      if(k>=isize){
        if( realloc(sresult,k+isize)==NULL)
        {
          free(sresult);
          free(stmp);
          return FAIL;
        }
        if( realloc(stmp,k+isize)==NULL)
        {
          free(sresult);
          free(stmp);
          return FAIL;
        }
        isize=k+isize;
      }
    }
    sresult[k]=0;
    clast=sexp[i];
    if(istatu<0)
    {
      free(sresult);
      free(stmp);
      return FAIL;
    }
  }
  if(istatu!=0)
  {
    free(sresult);
    free(stmp);
    return FAIL;
  }
  strncpy(sexp,sresult,imax-1);
  free(sresult);
  free(stmp);
  return SUCC;
}

int xml_ItemToString(HXMLTREE hXML,char * sexp,int isize)
{
  int istatu=0;
  int ilenth,i,j=0,k=0,imax;
  char *sresult,*stmp,*stmpres, clast=0;

  imax=isize;
  ilenth=strlen(sexp);
  if((stmp=malloc(isize))==NULL)
    return FAIL;
  memset(stmp,0,isize);
  if((sresult=malloc(isize))==NULL)
  {
    free(stmp);
    return FAIL;
  }
  memset(sresult,0,isize);
  if((stmpres=malloc(isize))==NULL)
  {
    free(sresult);
    free(stmp);
    return FAIL;
  }
  memset(stmpres,0,isize);

  for(i=0;i<ilenth;i++)
  {
    if((sexp[i]=='[')&&(clast!='\\'))
    {
      if(istatu==0)
      {
        memset(stmp,0,sizeof(stmp));
        j=0;
      }
      else
        stmp[j++]=sexp[i];
      istatu++;
    }
    else if((sexp[i]==']')&&(clast!='\\'))
    {
      istatu--;
      if(istatu==0)
      {
        if((xml_ItemToString(hXML,stmp,isize))== FAIL)
        {
          free(stmpres);
          free(sresult);
          free(stmp);
          return FAIL;
        }
        k+=strlen(stmp);
        if(k>=isize)
        {
          if( realloc(sresult,k+isize)==NULL)
          {
            free(stmpres);
            free(sresult);
            free(stmp);
            return FAIL;
          }
          if( realloc(stmp,k+isize)==NULL)
          {
            free(stmpres);
            free(sresult);
            free(stmp);
            return FAIL;
          }
          isize=k+isize;
        }
        strcat(sresult,stmp);
      }
      else stmp[j++]=sexp[i];
    }
    else
    {
      if(((sexp[i]=='[')&&(clast=='\\'))||((sexp[i]==']')&&(clast=='\\')))
      {
        if(istatu!=0)
          stmp[j-1]=sexp[i];
        else sresult[k-1]=sexp[i];
      }
      else
      {
        if(istatu!=0)
          stmp[j++]=sexp[i];
        else sresult[k++]=sexp[i];
      }
    }
    if(k>=isize)
    {
      if( realloc(sresult,k+isize)==NULL)
      {
        free(stmpres);
        free(sresult);
        free(stmp);
        return FAIL;
      }
      if( realloc(stmp,k+isize)==NULL)
      {
        free(stmpres);
        free(sresult);
        free(stmp);
        return FAIL;
      }
      isize=k+isize;
    }
    sresult[k]=0;
    clast=sexp[i];
    if(istatu<0)
    {
      free(stmpres);
      free(sresult);
      free(stmp);
      return FAIL;
    }
  }
  if(istatu!=0)
  {
    free(stmpres);
    free(sresult);
    free(stmp);
    return FAIL;
  }
  if((xml_GetElementE(hXML,sresult,stmpres,isize)== FAIL)
     &&(xml_GetLastError()!=XML_ENONODE))
  {
    free(stmpres);
    free(sresult);
    free(stmp);
    return FAIL;
  }
  strncpy(sexp,stmpres,imax-1);
  free(stmpres);
  free(sresult);
  free(stmp);
  return SUCC;
}



/************************************************************************
��������:HXMLTREE xml_CreateXMLFromFile(char *sFileName)
��������:���ļ���������XML�ṹ
�������:XML��ʽ�ļ�����
�������:
          >0--�ɹ� XML ���
          <0--ʧ��
          -2--�ļ�Ϊ��
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:������
��������:2000��01��01��
�޸�����:2002��04��01��
************************************************************************/
HXMLTREE xml_CreateXMLFromFile(char *sFileName)
{
  /*�������÷���*/
  int iResult;

  /*XML�ṹ���*/
  HXMLTREE iHandleXML;

  /*�ļ�ָ��*/
  FILE *pfFile;

  caddr_t sFileHead;
  struct stat struStat;

  /******************************������ʼ*******************************/
  /*��XML��ʽ���ļ�*/
  pfFile = fopen(sFileName, "r");
  if (pfFile == NULL)
    return FAIL;

  /*���ļ�ӳ�䵽�ڴ�*/
  fstat(fileno(pfFile), &struStat);
  if ( struStat.st_size == 0 )
    return FAIL;

  if ((sFileHead = mmap(NULL, (size_t)struStat.st_size, PROT_READ, MAP_SHARED,
      fileno(pfFile), (off_t)0)) == (caddr_t) - 1)
  {
    munmap(sFileHead, (size_t)struStat.st_size);
    fclose(pfFile);
    return FAIL;
  }

  /*��ʼ��XML�ṹ*/
  iHandleXML = xml_Create("root");
  if (iHandleXML == -1)
  {
    munmap(sFileHead, (size_t)struStat.st_size);
    fclose(pfFile);
    return FAIL;
  }

  /*���ļ����ݵ���XML�ṹ��*/
  iResult = xml_ImportXMLString(iHandleXML, sFileHead, NULL, 1);
  if (iResult == FAIL)
  {
    munmap(sFileHead, (size_t)struStat.st_size);
    fclose(pfFile);
    return FAIL;
  }

  munmap(sFileHead, (size_t)struStat.st_size);

  /*�ر��ļ�*/
  fclose(pfFile);

  /*��������*/
  return iHandleXML;
}

/************************************************************************
��������:int xml_CreateXMLFileFromTree(HXMLTREE iXmlhandle,char *sFileName)
��������:��XML�ṹ�����ļ�����
�������:XML�����XML��ʽ�ļ�����
�������:
          >0--�ɹ�
          -1--�ļ���ʧ��ʧ��
          -2--��������
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:���¸�
��������:2000��01��01��
�޸�����:2002��04��01��
************************************************************************/
int xml_CreateXMLFileFromTree(HXMLTREE iXmlhandle,char *sFileName)
{
  int iResult;
  FILE *pfFile;
  int iFileSize;
  if ( !*sFileName || !sFileName || !iXmlhandle)
    return FAIL;
  pfFile = fopen(sFileName, "w");
  if (pfFile == NULL)
    return FAIL;

  fclose(pfFile);

  return SUCC;
}


/**********************************************************************
 *  ������  xml_GetBinElementLen
 *  ����: ��ȡXML����ָ�������ƽڵ����ݳ���
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ��ȡ��Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
 *  ����ֵ:
  �ɹ�  ����ʵ���ֽ���
  ʧ��  ����FAIL
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:
��������:2000��01��01��
�޸�����:2002��04��01��
��:
 **********************************************************************/
int xml_GetBinElementLen( HXMLTREE hXML,const char *szElementName)
{
  return xml_GetElementLen( hXML,szElementName)/2;
}
/**************************************************************
 *  ������  xml_TreeUpdata
 *  ����: ��Դxml�������ݸ���Ŀ����xml�нڵ�����
 *  ����:
  sxml
    Դxml�����
  dxml
	Ŀ�������
  dpathname
    ָ��Ŀ��XML����Ҫ���¸��ڵ�����ƣ�����"/PRIVATE"
    "/NAME1|IDX1/NAME|2
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
ʹ��˵�������ݽڵ���¹���
		  ����ӽڵ�ͬ�����滻Ŀ�����нڵ������
		  ���Ŀ�����в������������ڵ�
		  ��Ӱ��Ŀ�����е������ڵ�
		���ƣ�����ͬĿ¼�µ�ͬ���ڵ���Ϊһ���ڵ㴦��
 *  ����ֵ:
        �ɹ�    ����SUCC
        ʧ��    ����FAIL
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:huangfeng
��������:2002��07��19��
�޸�����:2002��07��19��
��:
**************************************************************/
int xml_TreeUpdata(HXMLTREE sxml,HXMLTREE dxml,char *dpathname)
{
 char dcurDIR[101];
 char scurDIR[101];
 int icount,i;
 char sname[101];

 memset(dcurDIR,0,sizeof(dcurDIR));
 memset(scurDIR,0,sizeof(scurDIR));

 icount=xml_ChildElementCount(sxml,"/");
 if(icount== FAIL) return FAIL;
 for(i=1;i<=icount;i++)
 {
 	 memset(sname,0,sizeof(sname));
 	 if(xml_GetChildElementName(sxml,"/",i,sname,sizeof(sname))==FAIL)
 	   return FAIL;
 	 snprintf(scurDIR,sizeof(scurDIR),"/%s",sname);
 	 snprintf(dcurDIR,sizeof(dcurDIR),"%s/%s",dpathname,sname);
 	 if(xn_SubExchang(sxml,dxml,scurDIR,dcurDIR)== FAIL)
 	   return FAIL;
  }
  return SUCC;
}
/********************************************************
* ������xn_subExchang
* ���ܣ����ṩ��xml_TreeUpdata����,���нڵ�ĸ���ת��
* �򵥸��ƣ�����ͬ���ڵ�,���Ծ��޴���������ʹ�� By Tianhc
**********************************************************/
int xn_SubExchang( HXMLTREE sxml, HXMLTREE dxml,char *spathname,char *dpathname)
{
 int chlcount,i;
 char  scurDIR[101],dcurDIR[101];
 char databuf[2048];

 memset(databuf,0,sizeof(databuf));
 if(xml_IsLeafNode(sxml,spathname))
 {
 	if(xml_GetElement(sxml,spathname,databuf,sizeof(databuf)) == FAIL)
 		return FAIL;
 	if(xml_SetElement(dxml,dpathname,databuf) == FAIL)
 		return FAIL;
 }
 else
 {
   if((chlcount=xml_ChildElementCount(sxml,spathname))== FAIL)
   	  return FAIL;
   for(i=1;i<=chlcount;i++)
   {
 	  if(xml_GetChildElementName(sxml,spathname,i,databuf,sizeof(databuf))== FAIL)
 		return FAIL;
 	  snprintf(scurDIR,sizeof(scurDIR),"%s/%s",spathname,databuf);
 	  snprintf(dcurDIR,sizeof(dcurDIR),"%s/%s",dpathname,databuf);
 	  if(xn_SubExchang(sxml,dxml,scurDIR,dcurDIR)== FAIL)
 	  	return FAIL;
   }
 }
 return SUCC;
}
/************************************************************************
��������:
int xml_CopyElement(HXMLTREE hXML,
                    const char *szSrcElementName,
                    const char *szDecElementName)
��������:XMLԪ�ؿ���
��������XMLԪ������
�������:
         HXMLTREE hXML                  XML���
         const char *szSrcElementName   ԴԪ��·��
         const char *szDecElementName   Ŀ��Ԫ��·��
��    ��:
         0  �����ɹ�
         FAIL ����ʧ��
��    ��:
ֻ�ܿ�������Ҷ�ӽڵ�
������󳤶Ȳ�����4096�ֽ�

�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:���¸�
��������:2000��01��01��
�޸�����:2002��04��01��
��:
************************************************************************/
int xml_CopyElement(HXMLTREE hXML,
                    const char *szSrcElementName,
                    const char *szDecElementName)
{
char sTmp[4096];
 	if(xml_GetElement(hXML,szSrcElementName,sTmp,sizeof(sTmp))== FAIL)
 		return FAIL;
 	if(xml_SetElement(hXML,szDecElementName,sTmp) == FAIL)
 		return FAIL;
  return strlen(sTmp);

}


/*
     �ݹ����,���ı���
     ���Ʒ�ʽ:  1-���Ƿ�ʽ, 0-�����Ƿ�ʽ(����), 2-���
     �������Ե�֧��  Tianhc 2005-06-02 14:28
     Tianhc 2005-11-20 22:16
     ���ڶ�����������BUG����
     ������XMLʱ(>2M),Ч�ʽϵ�,�õײ�ĺ����ܼӿ��ٶ�?---��λ����,�����ݳ���>5000ʱ�������
*/
int xml_XCopy(HXMLTREE lXmlhandle , char *source, char *dest, int mode)
{
  int num,i,num1,num2,ret=0,j,k;
  char sname[255],sname1[255],sname2[255];
  char sname_add[255],sname_broth[255];
  int ibroth;
  int iret=0;

  num=xml_ChildElementCount( lXmlhandle, source);
  if (num<0) return -1;

  /*�ж��Ƿ�ΪҶ�ӽڵ�*/
  if (num == 0)
  {
     /*Ŀ��ڵ㲻���ڻ���Ŀ��ڵ������Ϊ��ӷ�ʽ*/
     if (xml_GetNode( lXmlhandle, dest) == NULL || mode == 2)
     {
        /*���ӿսڵ�*/
        if (xml_AddElement(lXmlhandle, dest, "") < 0)
            return -7;
        /*����+ֵһ����*/
        snprintf(sname_add, sizeof(sname_add), "%s|0", dest);
        if (xml_node_copy(lXmlhandle,source,sname_add) < 0)
            return -8;
     }
     else if (mode == 1) /*Ŀ�������Ϊ���Ƿ�ʽ*/
     {
         if (xml_node_copy(lXmlhandle, source, dest) < 0)
             return -11;
     }
     return 1;
  }

  for (i=1;i<=num;i++)
  {
    if ( xml_GetChildElementName( lXmlhandle, source , i, sname, sizeof(sname) )==-1 )
      return -2;
    /*�����п�������!Tianhc 2005-11-20 22:17*/
    ibroth=1;
    for (k=1;k<i;k++)
    {
         if (xml_GetChildElementName( lXmlhandle, source , k, sname_broth, sizeof(sname) )==-1)
              return -3;
         if (strcmp(sname,sname_broth)==0)
              ibroth++;
    }
    if (ibroth > 1)
    {
      snprintf(sname1,sizeof(sname1),"%s/%s|%d",source,sname,ibroth);
      if (strcmp(dest,"/")==0) /*tianhc 2006-6-1 20:48*/
         snprintf(sname2,sizeof(sname2),"/%s|%d",sname,ibroth);
      else
         snprintf(sname2,sizeof(sname2),"%s/%s|%d",dest,sname,ibroth);
    }
    else
    {
      snprintf(sname1,sizeof(sname1),"%s/%s",source,sname);
      if (strcmp(dest,"/")==0)  /*tianhc 2006-6-1 20:48*/
          snprintf(sname2,sizeof(sname2),"/%s",sname);
        else
          snprintf(sname2,sizeof(sname2),"%s/%s",dest,sname);
    }
    /*�ж��Ƿ�ΪҶ�ӽڵ�*/
    if (xml_IsLeafNode(lXmlhandle,sname1)==1)
    {
         iret++;
         /*�ڵ��Ƿ����*/
         if (xml_GetNode( lXmlhandle, sname2)!=NULL)  /*Ŀ���Ѵ���*/
         {
            if (mode==0) /*Ŀ�����,��������*/
               continue;
            if (mode==2) /*Ŀ�����,��ӷ�ʽ*/
            {
               /*���ӽڵ�,����ֵ*/
               snprintf(sname_add,sizeof(sname_add),"%s/%s", dest,sname);
               if (xml_AddElement( lXmlhandle, sname_add, "") == -1 )
                 return -7;
               /*����+ֵһ����-->���Ҷ�ӽڵ� Tianhc 2005-06-02 14:38*/
               snprintf(sname_add,sizeof(sname_add),"%s/%s|0", dest,sname);
               ret = xml_node_copy(lXmlhandle,sname1,sname_add);
               if ( ret < 0 )
                  return -8;
            } else
            {
                  /*����*/
               ret = xml_node_copy(lXmlhandle,sname1,sname2);
               if ( ret < 0 )
                  return ret;
            }
         } else
         {
                  /*Ŀ�겻����*/
               ret = xml_node_copy(lXmlhandle,sname1,sname2);
               if ( ret < 0 )
                  return ret;
         }
    } else
    {
         ret=xml_XCopy( lXmlhandle, sname1, sname2, mode);
         if (ret<0)
            return ret;
         iret+=ret;
    }
  }
  return iret;
}

/*
   XMLҶ�ӽڵ㸴�ơ�
   ֻ���Ҷ�ӽڵ�,�������ݣ�����+ֵ
*/
int xml_node_copy(HXMLTREE lXmlhandle,char *sname_src,char *sname_dest)
{
	 char sbuf[5000],sattr[255];
	 int inum,i,iret;
	 PXMLNODE lpNode;

	 inum=xml_AttributeCount( lXmlhandle, sname_src);
	 if (inum < 0)
	   return -21;
   for (i=1; i<=inum ; i++)
	 {
	 	  memset(sattr,0,sizeof(sattr));
	 	  iret=xml_GetAttributeName(lXmlhandle, sname_src, i, sattr, sizeof(sattr));
	 	  if (iret<0)
	 	    return -22;
	    iret=xml_GetElementAttr( lXmlhandle,sname_src,sattr,sbuf, sizeof(sbuf));
	    if (iret < 0)
	      return -23;
	    /*�ڵ㲻����,������ Tianhc 2005-06-03 12:06*/
      if (xml_ElementExist ( lXmlhandle, sname_dest) == 0)
      {
        if (xml_SetElement( lXmlhandle, sname_dest, "") == -1)
          return -24;
	    }
	    iret=xml_SetElementAttr( lXmlhandle, sname_dest, sattr, sbuf);
	    if (iret < 0)
	      return -25;
	 }
	 /*���Լ����,�ӽڵ��ֵ,�ж��Ƿ�ΪҶ�ڵ�*/
	 if (xml_IsLeafNode( lXmlhandle, sname_src ) == 1 )
   {
	   /*if (xml_GetElement( lXmlhandle, sname_src, sbuf, sizeof(sbuf)) == -1)*/
	   lpNode=xml_GetNode(lXmlhandle,sname_src);
	   if ( lpNode== NULL )
	        return -25;
	   /*if (sbuf[0]!=0) */
	   if  ( lpNode->m_iDataLen != 0 )
	   {
	       if (xml_SetElement( lXmlhandle, sname_dest, lpNode->m_pszData) == -1)
	           return -26;
		 } else
		 {
	       if (xml_SetElement( lXmlhandle, sname_dest, "") == -1)
	           return -27;
		 }
	 }
	 return 0;
}


/****************************************************************
��������ConvXmlTwo
�������ܣ���XML2.0��ʽ������ת��ΪXML1.0��ʽ��<xxxx/>��ʽת��Ϊ<xxxx></xxxx>��ʽ��
�������:
	 char *str :XML�ַ���
	 int ilen  :����,һ��Ϊsizeof(str),���ص����ݱ���������ݳ�.
ע��
   ���޸ĵײ㺯��,Ŀǰ�ײ�xn_ImportXMLString����ֱ��֧��<xxx/>��XML 2.0��д��ʽ,
   ��������XML�ϴ�ʱ(>1M)Ч�ʽϵͣ�����!
   Tianhc 2006-11-15 22:03
*****************************************************************/
int xml_Conv2to1(char *str,int ilen)
{
    char *buffer;
    int i=0,j=0,k=0,itmp=0,jtmp=0;
    int flag=0;
    if (  str == NULL )
      return -1;
    return 0;

    if((buffer=malloc(ilen*2))==NULL)
       return -2;
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] =='>')
           flag=1;
        else if (str[i] =='<')
        {
           j=k;
           flag=0;
        }
/*
        if (flag==1 && str[i] ==' ' && str[i] != '\0')
            continue;
*/
        if (str[i]=='/'&&str[i+1]=='>') /*��<xxxx/>��ʾ�սڵ�����ת��Ϊ<xxxx></xxxx>��ʽ*/
        {
            jtmp = k;
            buffer[k++] = '>';
            buffer[k++] = '<';
            buffer[k++] = '/';
            for(itmp = j+1; itmp<= jtmp;itmp++)
            {
            	/*
            	  Tianhc 2005-06-03 14:52
            	  ���<ATTRIBUTE NAME="1" />�����,��Ϊ�пո����Ǵ����ԵĽ��
            	*/
            	if (buffer[itmp] == ' ')
            	{
            	   buffer[k++]='>';
            		 break;
            	}
            	buffer[k++] = buffer[itmp];
            }
	          i++;
            continue;
        }
        buffer[k++] = str[i];
    }
    buffer[k] = '\0';
    if (k>ilen)   /*������������*/
    {
        free(buffer);
    	  return -3;
    }
    strcpy(str , buffer);
    return 0;
}

/************************************************************************
��������:HXMLTREE xml_CreateXMLFromFile_OPTROOT(char *sFileName, int ioptroot)
��������:���ļ���������XML�ṹ
�������:sFileName      XML��ʽ�ļ�����
         ioptroot       �Ƿ��滻���ڵ�
            >0 �滻 ��XML�ִ��е�һ���ϴ�����˳��ָ��λ�ýڵ��滻
            <0 �滻 ��XML�ִ��е�һ���ϴ��ҵ���˳��ָ��λ�ýڵ��滻
            0  ���滻,�����������뵽�ýڵ���

�������:
          >0--�ɹ� XML ���
          <0--ʧ��
          -2--�ļ�Ϊ��
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:SRC
************************************************************************/
HXMLTREE xml_CreateXMLFromFile_OPTROOT(char *sFileName, int ioptroot)
{
  /*�������÷���*/
  int iResult;

  /*XML�ṹ���*/
  HXMLTREE iHandleXML;

  /*�ļ�ָ��*/
  FILE *pfFile;

  caddr_t sFileHead;
  struct stat struStat;

  /******************************������ʼ*******************************/
  /*��XML��ʽ���ļ�*/
  pfFile = fopen(sFileName, "r");
  if (pfFile == NULL)
    return FAIL;

  /*���ļ�ӳ�䵽�ڴ�*/
  fstat(fileno(pfFile), &struStat);
  if ( struStat.st_size == 0 )
    return FAIL;

  if ((sFileHead = mmap(NULL, (size_t)struStat.st_size, PROT_READ, MAP_SHARED,
      fileno(pfFile), (off_t)0)) == (caddr_t) - 1)
  {
    munmap(sFileHead, (size_t)struStat.st_size);
    fclose(pfFile);
    return FAIL;
  }

  /*��ʼ��XML�ṹ*/
  iHandleXML = xml_Create("root");
  if (iHandleXML == -1)
  {
    munmap(sFileHead, (size_t)struStat.st_size);
    fclose(pfFile);
    return FAIL;
  }

  /*���ļ����ݵ���XML�ṹ��*/
  iResult = xml_ImportXMLString(iHandleXML, sFileHead, NULL, ioptroot);
  if (iResult == FAIL)
  {
    munmap(sFileHead, (size_t)struStat.st_size);
    fclose(pfFile);
    return FAIL;
  }

  munmap(sFileHead, (size_t)struStat.st_size);

  /*�ر��ļ�*/
  fclose(pfFile);

  /*��������*/
  return iHandleXML;
}
