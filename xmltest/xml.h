/************************************************************************
��Ȩ����:���ݺ������ӹɷ����޹�˾
��Ŀ����:������Ŀ
��    ��:V1.0
����ϵͳ:AIX4.2,SCO5
�ļ�����:xml.h
�ļ�����:��XML����������
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
#ifndef __XML_H__
#define __XML_H__

#include <stdio.h>
#include <sys/types.h>

#define    FAIL      (-1)
#define     SUCC       (0)

typedef long    HXMLTREE;

#define    IO_TYPE_NORMAL    1  /* ������ָ������� */
#define    IO_TYPE_8583    2  /* 8583���� */
#define    IO_TYPE_XML      3  /* XML���� */
#define    IO_TYPE_DSR      4  /* DSR�������� */


/**********************************************************************
 *  ����:  xml_Create
 *  ����:  ��ʼ��XML�ṹ
 *  ����:
  szRootName
          ָ��XML�ṹ��Ԫ�ص����ơ�
 *  ����ֵ:
  �ɹ�  ����XML�ṹ���
  ʧ��  ����FAIL
 **********************************************************************/
HXMLTREE xml_Create( const char *szRootName);


/**********************************************************************
 *  ����:  xml_Destroy
 *  ����:  ɾ��XML�ṹ
 *  ����:
  hXML
          ��ɾ����XML�豸�����
 *  ����ֵ:
  �ɹ�  �����ֽ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_Destroy( HXMLTREE hXML );

/**********************************************************************
 *  ����:  xml_Clear
 *  ����:  ���XML�ṹ���������ݣ�ֻ�������ڵ�
 *  ����:
  hXML
          �������XML�豸�����
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_Clear( HXMLTREE hXML );

/**********************************************************************
 *  ����:  xml_AddElement
 *  ����:  ��XML�������һ��Ҷ�ӽڵ�
    Ҷ�ӽڵ�(���ݽڵ�)�����ظ�����
    ����ýڵ㲻����, ��Ӹýڵ�; 
    ����ýڵ��Ѵ���, ���ͬ���ڵ�;
    ���ʱ���·���ڵ㲻����, ����ͬ��Щ�ڵ�һ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ��ӵ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  �ڸò��ϵĽڵ�Ԫ������
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾ��������
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ0
  szElementValue
    ����ӵ�Ԫ��ֵָ��
    �ڵ��Ϊ���ݽڵ�����Խڵ�
    ����ò������ִ�ֵ����ýڵ�Ϊ���ݽڵ㣬
    ����ò���ֵΪ""��NULL����ýڵ����Ϊ���Խڵ㣬�Ժ�ýڵ����������ԡ�
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_AddElement( HXMLTREE hXML,
       const char *szElementName,
       const char *szElementValue );

/**********************************************************************
 *  ����:  xml_SetElement
 *  ����:  ��XML��������һ��Ҷ�ӽڵ�
    Ҷ�ӽڵ�(���ݽڵ�)�����ظ�����
    ����ýڵ㲻����, ��Ӹýڵ�; 
      ���·���ڵ㲻����, ����ͬ��Щ�ڵ�һ�����
    ����ýڵ��Ѵ���, �޸ĸýڵ�����;
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ��ӵ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  �ڸò��ϵĽڵ�Ԫ������
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾ��������
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
  szElementValue
    �����õ�Ԫ��ֵָ��
    �ڵ��Ϊ���ݽڵ�����Խڵ�
    ����ò������ִ�ֵ����ýڵ�Ϊ���ݽڵ㣬
    ����ò���ֵΪ""��NULL����ýڵ����Ϊ���Խڵ㣬�Ժ�ýڵ����������ԡ�
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_SetElement( HXMLTREE hXML,
       const char *szElementName,
       const char *szElementValue );

/**********************************************************************
 *  ����:  xml_GetElement
 *  ����:  ��ȡXML����ָ���ڵ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ��ȡ��Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
  pucElementBuff
    ���ջ�ȡԪ��ֵ�Ļ�����ָ��
  iElementBuffSize
    ��������С
 *  ����ֵ:
  �ɹ�  ����ʵ���ֽ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_GetElement( HXMLTREE hXML,
       const char *szElementName,
       char *pucElementBuff,
       const size_t uiElementBuffSize );


/**********************************************************************
 *  ����:  xml_ModifyElement
 *  ����:  �޸�XML����ָ���ڵ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ�޸ĵ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
  szElementValue
    Ԫ���µ�ֵָ��
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ModifyElement( HXMLTREE hXML,
       const char *szElementName,
       const char *szElementValue );


/**********************************************************************
 *  ����:  xml_DelElement
 *  ����:  ɾ��XML����ָ���ڵ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪɾ����Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_DelElement( HXMLTREE hXML, const char *szElementName );



/**********************************************************************
 *  ����:  xml_ElementExist
 *  ����:  �ж�XML����ָ���ڵ��Ƿ����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪɾ����Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
 *  ����ֵ:
  �ɹ�  ����,����1; ������,����0
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ElementExist( HXMLTREE hXML, const char *szElementName );

/**********************************************************************
 *  ����:  xml_IsLeafNode
 *  ����:  �ж�XML����ָ���ڵ��Ƿ�ΪҶ�ӽڵ�(���ݽڵ�)
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ�жϵ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
 *  ����ֵ:
  �ɹ�  ��Ҷ�ӽڵ�,����1; ����,����0
  ʧ��  ����FAIL
 **********************************************************************/
int xml_IsLeafNode( HXMLTREE hXML, const char *szElementName );



/**********************************************************************
 *  ����:  xml_ChildElementCount
 *  ����:  ��ȡXML����ָ���ڵ���ӽڵ����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹ�е�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ChildElementCount( HXMLTREE hXML,
       const char *szElementName );



/**********************************************************************
 *  ����:  xml_GetChildElementName
 *  ����:  ��ȡXML���ڵ���ָ��λ���ӽڵ���
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ��ȡ��Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
  iPos
    ָ��λ��
  pucNameBuff
    ���ջ�ȡԪ�����Ļ�����ָ��
  uiNameBuffSize
    ��������С
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_GetChildElementName( HXMLTREE hXML,
       const char *szElementName,
       const int iPos,
       char *pucNameBuff,
       const size_t uiNameBuffSize );


/**********************************************************************
 *  ����:  xml_AddElementAttr
 *  ����:  ��XML�������һ���ڵ�����
    ���Խڵ㲻�����ظ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ������Ե�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ��ʾȡ���һ��Ԫ�أ�
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾ��������
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
  szAttributeName
    ����ӵ�������ָ��
  szAttributeValue
    ����ӵ�����ֵָ��
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_AddElementAttr( HXMLTREE hXML,
       const char *szElementName,
       const char *szAttributeName,
       const char *szAttributeValue );


/**********************************************************************
 *  ����:  xml_SetElementAttr
 *  ����:  ��XML��������һ���ڵ�����
    ���Խڵ㲻�����ظ�����
    ����ڵ����Բ����ڣ������һ���ڵ����ԣ�
    ����ڵ����Դ��ڣ����޸���ֵΪ��ֵ
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ�������Ե�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ��ʾȡ���һ��Ԫ�أ�
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾ��������
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
  szAttributeName
    �����õ�������ָ��
  szAttributeValue
    �����õ�����ֵָ��
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_SetElementAttr( HXMLTREE hXML,
       const char *szElementName,
       const char *szAttributeName,
       const char *szAttributeValue );


/**********************************************************************
 *  ����:  xml_GetElementAttr
 *  ����:  ��ȡXML����ָ���ڵ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ��ȡ���Ե�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
  szAttributeName
    ������ָ��
  pucAttributeBuff
    ���ջ�ȡԪ������ֵ�Ļ�����ָ��
  iElementBuffSize
    ��������С
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_GetElementAttr( HXMLTREE hXML,
       const char *szElementName,
       const char *szAttributeName,
       char *pucAttributeBuff,
       const size_t uiAttrBuffSize );


/**********************************************************************
 *  ����:  xml_DelElementAttr
 *  ����:  ɾ��XML����ָ���ڵ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪɾ�����Ե�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
  szAttributeName
    ������ָ��
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_DelElementAttr( HXMLTREE hXML,
       const char *szElementName,
       const char *szAttributeName);



/**********************************************************************
 *  ����:  xml_ElementAttrExist
 *  ����:  �ж�XML����ָ���ڵ������Ƿ����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ�ж����Ե�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
  szAttributeName
    ������ָ��
 *  ����ֵ:
  �ɹ�  ����,����1; ������,����0
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ElementAttrExist( HXMLTREE hXML,
       const char *szElementName,
       const char *szAttributeName);


/**********************************************************************
 *  ����:  xml_ModifyElementAttr
 *  ����:  �޸�XML����ָ���ڵ���������
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ��ȡ���Ե�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
  szAttributeName
    ������ָ��
  szAttributeValue
    ����ӵ�����ֵָ��
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ModifyElementAttr( HXMLTREE hXML,
       const char *szElementName,
       const char *szAttributeName,
       const char *szAttributeValue );



/**********************************************************************
 *  ����:  xml_GetAttributeName
 *  ����:  ��ȡXML���ڵ���ָ��λ��������
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ��ȡ��Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
  iPos
    ָ��λ��
  pucNameBuff
    ���ջ�ȡԪ���������Ļ�����ָ��
  uiNameBuffSize
    ��������С
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_GetAttributeName( HXMLTREE hXML,
       const char *szElementName,
       const int iPos,
       char *pucAttrNameBuff,
       const size_t uiAttrNameBuffSize );



/**********************************************************************
 *  ����:  xml_AttributeCount
 *  ����:  ��ȡXML����ָ���ڵ�����Ը���
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹ�е�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
 *  ����ֵ:
  �ɹ�  ���ظýڵ���Ԫ�ظ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_AttributeCount( HXMLTREE hXML,
       const char *szElementName );



/**********************************************************************
 *  ����:  xml_ElementCount
 *  ����:  ͳ��XML����ָ���ڵ�Ԫ��ͬһ���ϵĸ���
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪͳ�Ƶ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn/FINDNAME"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
 *  ����ֵ:
  �ɹ�  ���ظ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ElementCount( HXMLTREE hXML,
       const char *szElementName );




/**********************************************************************
 *  ����:  xml_ExportXMLString
 *  ����:  ����XML�ִ�����
 *  ����:
  hXML
    ָ��XML�豸���
  pucXMLBuff
    ���յ���ȡ�ִ��Ļ�����ָ��
  uiXMLBuffLen
    ��������С
  szExportName
    ָ��XML�ṹ����λ�õ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
    ���ΪNULL��""���򵼳���������
  bNodeSelf
    �Ƿ�����ڵ���������
    ��0    ����ʱ�����ýڵ�Ԫ�ء�
    0    ����ʱ�����ýڵ������ӽڵ����ݣ��������ýڵ㱾��
  *  ����ֵ:
  �ɹ�  ���ص������ֽ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ExportXMLString( HXMLTREE hXML,
      char *pucXMLBuff,
      size_t uiXMLBuffLen,
      const char *szExportName,
      int bNodeSelf );



/**********************************************************************
 *  ����:  xml_ImportXMLString
 *  ����:  ��XML��ʽ�ַ������뵽XML�ṹָ���ڵ���
      XML�ִ����32��
 *  ����:
  hXML
    ָ�����յ���XML�豸���
  szImportXMLString
    XML��ʽ�ִ�ָ��
  szImportName
    ָ����XML�ṹ���յ����λ�õ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
    ���ΪNULL��""�������ڸ��ڵ���
  bReplace
    �Ƿ��滻szImportName�ڵ�
    >0 �滻 ��XML�ִ��е�һ���ϴ�����˳��ָ��λ�ýڵ��滻
    <0 �滻 ��XML�ִ��е�һ���ϴ��ҵ���˳��ָ��λ�ýڵ��滻
    0  ���滻,�����������뵽�ýڵ���
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ImportXMLString( HXMLTREE hXML,
      const char *szImportXML,
      const char *szImportName,
      int bReplace );



/**********************************************************************
 *  ����:  xml_ExportDSRString
 *  ����:  ����DSR��ʽ�ִ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szMapFile
    DSR <=> XML ӳ���ļ�
  pucDSRBuff
    ���յ���ȡ�ִ��Ļ�����ָ��
  uiDSRBuffLen
    ��������С
 *  ����ֵ:
  �ɹ�  ���ص������ֽ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ExportDSRString( HXMLTREE hXML,
      const char *szMapFile,
      char *pucDSRBuff,
      size_t uiDSRBuffLen );



/**********************************************************************
 *  ����:  xml_ImportDSRString
 *  ����:  ��DSR��ʽ�ַ������뵽XML�ṹָ���ڵ���
 *  ����:
  hXML
    ָ�����յ���XML�豸���
  szMapFile
    DSR <=> XML ӳ���ļ�
  szImportDSRString
    DSR��ʽ�ִ�ָ��
  size
    DSR��ʽ���ݳ���
  szImportName
    ָ����XML�ṹ���յ����λ�õ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
    ���ΪNULL��""�������ڸ��ڵ���
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ImportDSRString( HXMLTREE hXML,
      const char *szMapFile,
      const char *szImportDSRString,
      size_t  size,
      const char *szImportName );

/**********************************************************************
 *  ����:  xml_ExchangeWithDSR
 *  ����:  ��DSR���ػ���������
 *  ����:
  hXML
    ָ���������ػ����ݵ�XML�豸���
  szMapFile
    DSR <=> XML ӳ���ļ�
  lLUType
    LU����
  hRecvXML
    �������ػ����ݵ�XML�豸���
  szImportName
    ָ���ڽ���XML�ṹ��λ�õ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
    ���ΪNULL��""�������ڸ��ڵ���
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ExchangeWithDSR( HXMLTREE hXML,
      const char *szMapFile,
      long    lLUType,
      HXMLTREE  hRecvXML,
      const char *szImportName );


/**********************************************************************
 *  ����:  xml_ExportFmtString
 *  ����:  ����ָ����ʽ�ִ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szMapFile
    XML <=> ָ����ʽ ӳ���ļ�
  pucBuff
    ���յ���ȡ�ִ��Ļ�����ָ��
  uiBuffLen
    ��������С
  iType
    ����������
 *  ����ֵ:
  �ɹ�  ���ص������ֽ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ExportFmtString( HXMLTREE hXML,
      const char *szMapFile,
      char *pucBuff,
      size_t uiBuffLen,
      int    iType );

/**********************************************************************
 *  ����:  xml_ImportFmtString
 *  ����:  ��ָ����ʽ�ַ������뵽XML�ṹָ���ڵ���
 *  ����:
  hXML
    ָ�����յ���XML�豸���
  szMapFile
    XML <=> ָ����ʽ ӳ���ļ�
  szImportString
    ָ����ʽ�ִ�ָ��
  size
    ָ����ʽ���ݳ���
  szImportName
    ָ����XML�ṹ���յ����λ�õ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX    ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
    ���ΪNULL��""�������ڸ��ڵ���
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ImportFmtString( HXMLTREE hXML,
      const char *szMapFile,
      const char *szImportString,
      size_t  uiStrLen,
      const char *szImportName,
      int    iType );


int xml_Save( HXMLTREE hXML, int iType, char szName );

int xml_Restore( HXMLTREE hXML, int iType, char szName, int iId );


#define  xml_GetLastErrorString()  xml_String(xml_GetLastError())

/**********************************************************************
 *  ����:  xml_GetLastError
 *  ����:  ��ȡ���һ�β����������
 *  ����:  ��
 *  ����ֵ: ������
 **********************************************************************/
int xml_GetLastError(void);

/**********************************************************************
 *  ����:  xml_StringErrno
 *  ����:  ����������ִ���
 *  ����һ:  �������
 *  ����ֵ: �����ִ�
 **********************************************************************/
char *xml_StringError(int iErrno);

/**********************************************************************
 *  �������
 **********************************************************************/
#define XML_NOERROR         0    /* �޴���(No error)                    */
#define XML_EINVAL         22    /* ��Ч����(Invalid argument)          */
#define XML_ENOMEM         12    /* ϵͳ�ڴ�ռ䲻��(Not enough memory)  */

#define XML_DFENOSPC       28    /* ָ���������ռ䲻��(No space left on device)  */

#define XML_ESYSTEM       200    /* ϵͳ����(system error)           */

#define XML_ENOROOT       201    /* �޸��ڵ�����(No root data)           */
#define XML_EINVNODENAME  202    /* ��Ч�ڵ������ִ�(Invalid node name)  */
#define XML_ENONODE       203    /* �����ڸýڵ�(No such node)           */
#define XML_ENOTDATANODE  204    /* �������ݽڵ�(Not data node)          */
#define XML_EDATANODE     205    /* �����ݽڵ�(Is data node)             */
#define XML_ENOATTR       206    /* �����ڸ�����(No such attribute)      */
#define XML_EATTREXIST    207    /* �������Ѵ���(Attribute exist already)*/
#define XML_EINVDATAPOS   208    /* ��Ч������λ��(Invalid data postion) */

#define XML_EXMLSYNTAX    209    /* XML�ִ��﷨����(Xml string syntax error)    */
#define XML_EXMLMAXLEV    210    /* XML�ִ���������������(Xml string max lev error)    */
#define XML_EXMLNONODE    211    /* XML�ִ��в����ڽ����滻�Ľڵ�(Xml string special node not exist)    */

#define XML_EOPENMAPFILE  221    /* ��DSRӳ���ļ�����(Open map file error)*/
#define XML_EFILENOTEXIST 222    /* ӳ���ļ�������(Map file not exists)*/
#define XML_EFILENOTREAD  223    /* ӳ���ļ����ɶ�(Map file can not read)*/
#define XML_EFILECONT     224    /* ӳ���ļ����ݴ���(Map file content error)*/
#define XML_EFILECONT_FOR 225    /* ӳ���ļ�����ѭ����ƥ��(Map file content loop error)*/
#define XML_EFILECONT_FORLVL  226 /* ӳ���ļ����ݲ�֧�ֶ���ѭ��(Map file content loop error)*/
#define XML_EFILECONT_FORCOMM  227 /* DSRӳ���ļ�����ѭ���в�֧�ֶ���ͨѶ��(Map file content loop error)*/
#define XML_EFILECONT_FORNUM  228 /* ӳ���ļ�����ѭ����������(Map file content loop num error)*/
#define XML_EFILENOTENG      229 /* ӳ���ļ�������DSR�������ݲ���(Map file and DSR content error)*/
#define XML_EFILECONT_XMLNODE   230 /* ӳ���ļ�����XML�ڵ�������(Map file XML node name too long)*/
#define XML_EFILEMULDSR      231 /* DSRӳ���ļ������������˶��DSR�ļ�(Map file dsr file too much)*/
#define XML_EFILENODSR      232 /* DSRӳ���ļ�������ȱ��DSR�ļ�����(Map file need dsr file)*/
#define XML_EDSRSTRERR      233 /* DSR�ִ�����(Dsr content error)       */

#define XML_ELOGFILENOTEXIST  234 /* TRACE�����ļ�������(TRACE config file not exists)*/
#define XML_ELOGFILENOTREAD    235 /* TRACE�����ļ����ɶ�(TRACE config file can not read)*/
#define  XML_EBUILDTRACE      236  /* ����TRACEʧ�ܣ�����TRACE�ļ�������Ŀ¼Ȩ��(Build Trace fail, please check trace file or bak directory) */

#define  XML_EDATA_NOTENOUGTH  237 /* �������ݳ��Ȳ���(import data not enougth) */
#define  XML_EIOTYPE        238 /* �����ӳ���ļ�����(unknow map file type) */
#define  XML_EFILE_TYPEFIELD    239  /* ӳ���ļ������������ִ�����(Map file datatype field content error) */
#define  XML_EFILE_ATTRFIELD    240  /* ӳ���ļ��������ִ�����(Map file attribute field content error) */
#define  XML_E8583NOTEXIST    241 /* ������ָ����8583��(Special 8583 Field not exist) */
#define  XML_E8583HEAD      242 /* 8583ͷ�����ݲ�ƥ��(8583 head data error) */
#define  XML_E8583POS      243 /* 8583������ƥ��(8583 field error) */

/**********************************************************************
 *  ����:  xml_asctohex
 *  ����:  ��ASCII�ִ�ת��Ϊ���ֽڵ�HEX�ִ�
 *  ����һ:  ����ת�����ʮ���������ݻ�����ָ��
 *  ������:  ��������С
 *  ������:  ��ת����ASCII����ָ��
 *  ������:  ���ݳ���
 *  ����ֵ:  �ɹ�,����ת���ַ�����;ʧ�ܷ���0
 **********************************************************************/
size_t xml_asctohex( char *szHexBuf, size_t size, char *szAsc, size_t num );

/**********************************************************************
 *  ����:  xml_hextoasc
 *  ����:  �����ֽڵ�HEX�ִ�ת��ΪASCII�ִ�
 *  ����һ:  ����ת�����ASCII���ݻ�����ָ��
 *  ������:  ��������С
 *  ������:  ��ת���Ķ��ֽڵ�HEX����ָ��
 *  ������:  ���ݳ���
 *  ����ֵ:  �ɹ�,����ת���ַ�����;ʧ�ܷ���0
 **********************************************************************/
size_t xml_hextoasc( char *szAscBuf, size_t size, char *szHex, size_t num );

int xml_GetTraceHandle( HXMLTREE hXML );

/**********************************************************************
 *  ����:  xml_GetVerion
 *  ����:  ��ȡ�汾��
 *  ����:  ��
 *  ����ֵ: �汾��
 **********************************************************************/
#define LOWORD_(l)           ((unsigned short)(l))
#define HIWORD_(l)           ((unsigned short)(((unsigned int)(l) >> 16) & 0xFFFF))
unsigned int xml_GetVerion();

/**********************************************************************
 *  ����:  xml_GetLastVerDate
 *  ����:  ��ȡ���汾����
 *  ����:  ��
 *  ����ֵ: �汾���ڴ�, ��ʽΪ "yyyy-mm-dd"
  **********************************************************************/
char *xml_GetLastVerDate();


/*use commun swap data call outlook*/
int xml_exchange(long lLUType,char* databuff,long datalen);


void FileFmtOutput( FILE *fpFile, const void *pBuf, size_t nLen, int nStart );

/************************************************************************
��������:xml_GetElementAttrP
��������:��ȡXML����ָ���ڵ���������ָ��
�������: 
	hXML
		ָ��XML�豸���
	szElementName
		ָ����XML�ṹҪ��ȡ���Ե�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
		"/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
		NMAE	ָ��ÿһ��Ԫ����,
		IDX		ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
			���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
			���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
			ȱʡΪ1
	szAttributeName
		������ָ��
��    ��: 
	�ɹ�	�ڵ���������ָ��
	ʧ��	NULL
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:
��������:2000��01��01��
�޸�����:2002��04��01��
��:
 **********************************************************************/
char* xml_GetElementAttrP( HXMLTREE hXML,
		   const char *szElementName,
		   const char *szAttributeName);
		   
/************************************************************************
��������:xml_GetElementP
��������:��ȡXML����ָ���ڵ�����ָ��
�������: 
	hXML
		ָ��XML�豸���
	szElementName
		ָ����XML�ṹҪ��ȡ��Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
		"/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
		NMAE	ָ��ÿһ��Ԫ����,
		IDX		ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
			���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
			���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
			ȱʡΪ1
��    ��: 
	�ɹ�	�ڵ�����ָ��
	ʧ��	NULL
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:
��������:2000��01��01��
�޸�����:2002��04��01��
��:
************************************************************************/
char* xml_GetElementP( HXMLTREE hXML, const char *szElementName);
/**********************************************************************
 *  ������  xml_GetElementLen
 *  ����: ��ȡXML����ָ���ڵ����ݳ���
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
int xml_GetElementLen( HXMLTREE hXML,const char *szElementName);

/*
    ����ָ���ڵ㵼������(string)ʱ���õĳ���
    һ������ExportToStringʱ�����仺����
    ilen1  �������*2+5(<></>)
    ilen2 +��������
    ilen3 +������+1+2+1(="" )+
    ilen4 +����ֵ ... 
    ֻ�����ý�㼰���������ӽ��������ַ�������.������
    <?xml version="1.0" encoding="GB2312"?> +40
*/
int xml_node_strlen(HXMLTREE lXmlhandle,char *snode);

/**********************************************************************
 *	����:	����xn_Setquot
 *	����:	�����ڲ���"�ַ���ת����׼,1-����Ϊ&quot;(XML��׼),0-����Ϊ&quote;��ϵͳ����ı�׼
 *	����һ:	ģʽ
 *	��ע��	ֻΪ���ݾ�ģʽ��һ������
 *       �����Ϊ�±�׼�����ϵͳ��ϲ����±���,����ASPK+IFI����Ӧ��,�нϴ�������׳�����
 *       xn_����Ϊ�ڲ�����,����������,�������ٰ�һ��
 **********************************************************************/
void xml_Setquot(int imode);

/*
   Tianhc 2006-9-12 11:08
     --ԭxml_NodeMove������,��д,��ɵĺ����㷨��ͬ
   Ŀ������ڣ��������Ŀ��ڵ�(����ɾ��,����ͬ���ڵ��޷�����)
         �����ڣ���һ���յĽڵ㣻
   ����sl_Delete() + xn_Move()�ƶ� (���ԭ�ڵ����丸�ڵ��ϵĵǼ�+�ƶ�+free)     
*/
int xml_NodeMove( HXMLTREE hXML, const char *snode_dest,const char *snode_src);
/************************************************************************
��������:
HXMLTREE xml_CopyTree(HXMLTREE hXML, const char *szElementName)
��������:�Ӿ���п���һ���������γ�һ���µ�xml��
�������:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ������Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
��    ��:
  �ɹ�  XML���
  ʧ��  FAIL
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:
��������:2000��01��01��
�޸�����:2011-07-28 13:11
��:
************************************************************************/
HXMLTREE xml_CopyTree(HXMLTREE hXML, const char *szElementName);

/************************************************************************
��������:
int xml_LoadTree(HXMLTREE hXML, const char *szElementName, HXMLTREE hLoadXML)
��������:װ��һ��XML������γ�һ����Ϊһ����������װ�ص�XML������ͷ�
�������:
  hXML
    ָ��XML�豸���
  szElementName
    װ�ص�λ�ã�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
  hLoadXML ��װ�ص�XML��ָ��
��    ��:
  �ɹ�  SUCC
  ʧ��  FAIL
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:
��������:2000��01��01��
�޸�����:2011-07-28 13:11
��:
************************************************************************/
int xml_LoadTree(HXMLTREE hXML, const char *szElementName, HXMLTREE *hLoadXML);

#endif /* end of  __XML_H__ */
