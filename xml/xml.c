/************************************************************************
��Ȩ���У����ݺ������ӹɷ����޹�˾
��Ŀ���ƣ�������Ŀ
��    ����V1.0
����ϵͳ��AIX4.2,SCO5
�ļ����ƣ�xmle.c
�ļ���������XML����������
�� Ŀ �飺�м�ҵ���Ʒ�з���
�� �� Ա���м�ҵ���Ʒ�з���
�������ڣ�2001��09��11��
�޸����ڣ�2002��05��12��
************************************************************************/
/**********************************************************************
  ת���ַ�&;
  &lt;    &gt;        &apos;      &quote;     &amp;
  <       >           '           "           &
 **********************************************************************/

/*
   �޸ļ�¼��
   Tianhc 2005-11-21 21:17
   ���ӣ�
    xml_ExportXMLStringHEAD()  ָ��XMLͷ����Ϣ
    xml_node_strlen            ���㵼�����ݵĳ��ȣ�����Exportǰ�����ڴ�
    xml_NodeMove               ��������ƶ�
*/


int g_xmlErrno=0; /* ������� */
char g_szErrEx[128];/* ������չ��Ϣ */
int g_xmlquot=1;

#include  "xmlnode.c"

#ifdef __cplusplus
extern "C" {
#endif /* end of __cplusplus */

PXMLNODE xml_GetNode( HXMLTREE hXML, const char *szElementName);


/**********************************************************************
 *  ������  xml_Create
 *  ����: ��ʼ��XML�ṹ
 *  ����:
  szRootName
          ָ��XML�ṹ��Ԫ�ص����ơ�
 *  ����ֵ:
  �ɹ�  ����XML�ṹ���
  ʧ��  ����FAIL
 **********************************************************************/
HXMLTREE xml_Create( const char *szRootName)
{
PXMLSTRUCT pXML;

  if ( !szRootName || !*szRootName )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  if ( !(pXML=xmlo_Build((char*)szRootName)) )
  {
    return  FAIL;
  }

  g_xmlErrno = XML_NOERROR;

  return  (HXMLTREE)pXML;
}

/**********************************************************************
 *  ������  xml_Destroy
 *  ����: ɾ��XML�ṹ
 *  ����:
  hXML
          ��ɾ����XML�豸�����
 *  ����ֵ:
  �ɹ�  �����ֽ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_Destroy( HXMLTREE hXML )
{
  if ( !hXML )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  xmlo_Destroy( (PXMLSTRUCT)hXML );
  g_xmlErrno = XML_NOERROR;
  return  SUCC;
}

/**********************************************************************
 *  ������  xml_Clear
 *  ����: ���XML�ṹ���������ݣ�ֻ�������ڵ�
 *  ����:
  hXML
          �������XML�豸�����
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_Clear( HXMLTREE hXML )
{
PXMLNODE *lpElemP,*lpEndP;
PXMLNODEATTR *lpElemAttrP,*lpEndAttrP;
PXMLSTRUCT  pXML;

  if ( !hXML )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }
  pXML = (PXMLSTRUCT)hXML;

#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_Clear().\n",0);
#endif

  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }


  if ( sl_Length(&pXML->m_pRoot->m_DownList) )
  {
    lpEndP=(PXMLNODE*)pXML->m_pRoot->m_DownList.m_lpBase+pXML->m_pRoot->m_DownList.m_nLen;
    lpElemP = (PXMLNODE*)pXML->m_pRoot->m_DownList.m_lpBase;
    for ( ; lpElemP<lpEndP; lpElemP++ )
    {
      xn_Destroy(*lpElemP);
    }
    pXML->m_pRoot->m_DownList.m_nLen=0;
  }

  if ( sl_Length(&pXML->m_pRoot->m_AttrList) )
  {
    lpEndAttrP=(PXMLNODEATTR*)pXML->m_pRoot->m_AttrList.m_lpBase+pXML->m_pRoot->m_DownList.m_nLen;
    lpElemAttrP = (PXMLNODEATTR*)pXML->m_pRoot->m_AttrList.m_lpBase;
    for ( ; lpElemAttrP<lpEndAttrP; lpElemAttrP++ )
    {
      xna_Destroy(*lpElemAttrP);
    }
    pXML->m_pRoot->m_AttrList.m_nLen=0;
  }

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  SUCC;
}

/**********************************************************************
 *  ������  xml_AddElement
 *  ����: ��XML�������һ��Ҷ�ӽڵ�
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
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
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
       const char *szElementValue )
{
PXMLSTRUCT pXML;
int   len;

  if ( !hXML || !szElementName || !*szElementName )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
  {
    if ( szElementValue )
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_AddElement(...,'%s','%s').\n",0,szElementName,szElementValue);
    else
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_AddElement(...,'%s',NULL).\n",0,szElementName);
  }
#endif

  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( szElementValue )
    len = strlen(szElementValue);
  else
    len = 0;

  if ( !xn_AddNode( pXML->m_pRoot, (char*)szElementName, (char*)szElementValue, len, pXML ) )
  {
    return  FAIL;
  }

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  SUCC;
}

/**********************************************************************
 *  ������  xml_SetElement
 *  ����: ��XML��������һ��Ҷ�ӽڵ�
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
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
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
       const char *szElementValue )
{
PXMLSTRUCT pXML;
PXMLNODE  lpNode;
int   len;

  if ( !hXML || !szElementName || !*szElementName )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
  {
    if ( szElementValue )
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_SetElement(...,'%s','%s').\n",0,szElementName,szElementValue);
    else
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_SetElement(...,'%s',NULL).\n",0,szElementName);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( szElementValue )
    len = strlen(szElementValue);
  else
    len = 0;

  if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
  {
    if ( g_xmlErrno!=XML_ENONODE )
      return  FAIL;

    /* �����ڸýڵ���� */
    if ( !xn_AddNode( pXML->m_pRoot, (char*)szElementName, (char*)szElementValue, len, pXML ) )
    {
      return  FAIL;
    }
  }
  else
  { /* ���ڸýڵ� */
    if ( sl_Length(&(lpNode->m_DownList)) )
    { /* �ýڵ㲻Ϊ���ݽڵ� */
      g_xmlErrno = XML_ENOTDATANODE;
      snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szElementName);
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(XML_ENOTDATANODE),szElementName);
#endif
      return  FAIL;
    }

    if ( xn_SetData(lpNode,(unsigned char*)szElementValue,len)==FAIL )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR )
        trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
      return  FAIL;
    }
  }

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  SUCC;
}

/**********************************************************************
 *  ������  xml_GetElement
 *  ����: ��ȡXML����ָ���ڵ�����
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
       const size_t uiElementBuffSize )
{
PXMLSTRUCT pXML;
PXMLNODE  lpNode;

  if ( !hXML || !szElementName || !*szElementName || !pucElementBuff || !uiElementBuffSize )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }
  memset( pucElementBuff,0,uiElementBuffSize);
  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_GetElement(...,'%s',0x%p,%u).\n",0,szElementName,pucElementBuff,uiElementBuffSize);
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
  {
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
      trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
    return  FAIL;
  }

  if ( lpNode->m_pszData && *lpNode->m_pszData )
  {
    if( uiElementBuffSize<= strlen((char*)lpNode->m_pszData) )
    {
      g_xmlErrno = XML_DFENOSPC;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_DFENOSPC));
#endif
      return  FAIL;
    }
    else
      memcpy(pucElementBuff,lpNode->m_pszData,lpNode->m_iDataLen);
  }
  else
    *pucElementBuff='\0';

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  lpNode->m_iDataLen;
}



/**********************************************************************
 *  ������  xml_ModifyElement
 *  ����: �޸�XML����ָ���ڵ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ�޸ĵ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
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
       const char *szElementValue )
{
PXMLSTRUCT pXML;
PXMLNODE  lpNode;
int   len;

  if ( !hXML || !szElementName || !*szElementName )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
  {
    if ( szElementValue )
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ModifyElement(...,'%s','%s').\n",0,szElementName,szElementValue);
    else
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ModifyElement(...,'%s',NULL).\n",0,szElementName);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
  {
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
      trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
    return  FAIL;
  }

  if ( sl_Length(&(lpNode->m_DownList)) )
  { /* �ýڵ㲻Ϊ���ݽڵ� */
    g_xmlErrno = XML_ENOTDATANODE;
    snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szElementName);
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(XML_ENOTDATANODE),szElementName);
#endif
    return  FAIL;
  }

  if ( szElementValue )
    len = strlen(szElementValue);
  else
    len = 0;

  if ( xn_SetData(lpNode,(unsigned char*)szElementValue,len)==FAIL )
  {
    return  FAIL;
  }
  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  SUCC;
}



/**********************************************************************
 *  ������  xml_DelElement
 *  ����: ɾ��XML����ָ���ڵ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪɾ����Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
 *  ����ֵ:
  �ɹ�  ����SUCC
  ʧ��  ����FAIL
 **********************************************************************/
int xml_DelElement( HXMLTREE hXML, const char *szElementName )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
int   nPos;

  if ( !hXML || !szElementName || !*szElementName )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
  {
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_DelElement(...,'%s').\n",0,szElementName);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,&nPos,pXML )) )
  {
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
      trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
    return  FAIL;
  }

  if ( sl_Delete(&lpNode->m_DownList,nPos,(void*)&lpNode)==-1 )
  {
    g_xmlErrno = XML_EINVDATAPOS;
    return  FAIL;
  }

  xn_Destroy(lpNode);

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  SUCC;
}


/**********************************************************************
 *  ������  xml_ElementExist
 *  ����: �ж�XML����ָ���ڵ��Ƿ����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪɾ����Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
 *  ����ֵ:
  �ɹ�  ����,����1; ������,����0
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ElementExist( HXMLTREE hXML, const char *szElementName )
{
PXMLSTRUCT  pXML;
int   ret;

  if ( !hXML || !szElementName || !*szElementName )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
  {
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ElementExist(...,'%s').\n",0,szElementName);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML ) )
  {
    g_xmlErrno = XML_NOERROR;
    ret = 1;
  }
  else if ( g_xmlErrno==XML_ENONODE )
  {
    g_xmlErrno = XML_NOERROR;
    ret = 0;
  }
  else
    ret = FAIL;

#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  ret;
}

/**********************************************************************
 *  ������  xml_IsLeafNode
 *  ����: �ж�XML����ָ���ڵ��Ƿ�ΪҶ�ӽڵ�(���ݽڵ�)
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ�жϵ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
 *  ����ֵ:
  �ɹ�  ��Ҷ�ӽڵ�,����1; ����,����0
  ʧ��  ����FAIL
 **********************************************************************/
int xml_IsLeafNode( HXMLTREE hXML, const char *szElementName )
{
PXMLSTRUCT pXML;
PXMLNODE  lpNode;
int     ret;

  if ( !hXML || !szElementName || !*szElementName )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_COUNT )
  {
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_IsLeafNode(...,'%s').\n",0,szElementName);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
  {
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
      trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
    return  FAIL;
  }

  g_xmlErrno = XML_NOERROR;

  ret = !sl_Length(&(lpNode->m_DownList));
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_COUNT )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  ret;
}



/**********************************************************************
 *  ������  xml_ChildElementCount
 *  ����: ��ȡXML����ָ���ڵ���ӽڵ����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹ�е�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
 *  ����ֵ:
  �ɹ�  �����ӽڵ����
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ChildElementCount( HXMLTREE hXML,
       const char *szElementName )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;


  if ( !hXML || !szElementName || !*szElementName )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_COUNT )
  {
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ChildElementCount(...,'%s').\n",0,szElementName);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
  {
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
      trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
    return  FAIL;
  }

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_COUNT )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  sl_Length(&lpNode->m_DownList);
}



/**********************************************************************
 *  ������  xml_GetChildElementName
 *  ����: ��ȡXML���ڵ���ָ��λ���ӽڵ���
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
       const size_t uiNameBuffSize )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;

  if ( !hXML || !szElementName || !*szElementName || !iPos || !pucNameBuff || !uiNameBuffSize )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
  {
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_GetChildElementName(...,'%s',%d,0x%p,%u).\n",
      0,szElementName,iPos,pucNameBuff,uiNameBuffSize);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
  {
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
      trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
    return  FAIL;
  }

  if ( sl_GetElem(&lpNode->m_DownList,iPos,(void*)&lpNode)==(-1) )
  {
    g_xmlErrno = XML_EINVDATAPOS;
    return  FAIL;
  }

  if ( lpNode->m_pszName && *lpNode->m_pszName )
  {
    if( uiNameBuffSize<= strlen((char*)lpNode->m_pszName) )
    {
      g_xmlErrno = XML_DFENOSPC;
      return  FAIL;
    }
    else
      strcpy(pucNameBuff,(char*)lpNode->m_pszName);
  }
  else
    *pucNameBuff='\0';

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  SUCC;
}



/**********************************************************************
 *  ������  xml_AddElementAttr
 *  ����: ��XML�������һ���ڵ�����
    ���Խڵ㲻�����ظ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ������Ե�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ��ʾȡ���һ��Ԫ�أ�
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
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
       const char *szAttributeValue )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
PXMLNODEATTR pAttr;

  if ( !hXML || !szAttributeName ||!*szAttributeName || !szAttributeValue )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
  {
    if ( szAttributeValue )
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_AddElementAttr(...,'%s','%s','%s').\n",
        0,szElementName,szAttributeName,szAttributeValue);
    else
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_AddElementAttr(...,'%s','%s',NULL).\n",
        0,szElementName,szAttributeName);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !szElementName || !*szElementName )
  {
    lpNode=pXML->m_pRoot;
  }
  else
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
      return  FAIL;
    }
  }


  if ( lpNode->m_pszData && *(lpNode->m_pszData) && sl_Length(&lpNode->m_DownList) )
  {
    g_xmlErrno = XML_EDATANODE;
    snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szElementName);
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(g_xmlErrno),szElementName);
#endif
    return  FAIL;
  }


  if ( sl_FindAttr(&(lpNode->m_AttrList),(unsigned char*)szAttributeName,NULL) )
  {
    g_xmlErrno = XML_EATTREXIST;
    snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szAttributeName);
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s - [%s: %s]\n",xml_StringError(g_xmlErrno),szElementName,szAttributeName);
#endif
    return  FAIL;
  }

  if( !(pAttr=xna_Build( (unsigned char*)szAttributeName, strlen(szAttributeName),(unsigned char*)szAttributeValue )) )
  {
    g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(g_xmlErrno));
#endif
    return  FAIL;
  }

  if ( sl_Append(&(lpNode->m_AttrList),pAttr)==-1 )
  {
    xna_Destroy(pAttr);
    g_xmlErrno = XML_ENOMEM;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(g_xmlErrno));
#endif
    return  FAIL;
  }

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  SUCC;
}



/**********************************************************************
 *  ������  xml_SetElementAttr
 *  ����: ��XML��������һ���ڵ�����
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
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
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
       const char *szAttributeValue )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
PXMLNODEATTR lpAttr;

  if ( !hXML || !szAttributeName ||!*szAttributeName || !szAttributeValue )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
  {
    if ( szAttributeValue )
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_SetElementAttr(...,'%s','%s','%s').\n",
        0,szElementName,szAttributeName,szAttributeValue);
    else
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_SetElementAttr(...,'%s','%s',NULL).\n",
        0,szElementName,szAttributeName);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !szElementName || !*szElementName )
  {
    lpNode=pXML->m_pRoot;
  }
  else
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
      return  FAIL;
    }
  }


  if ( lpNode->m_pszData && *(lpNode->m_pszData) && sl_Length(&lpNode->m_DownList) )
  {
    g_xmlErrno = XML_EDATANODE;
    snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szElementName);
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(XML_EDATANODE),szElementName);
#endif
    return  FAIL;
  }


  if ( lpAttr=sl_FindAttr(&(lpNode->m_AttrList),(unsigned char*)szAttributeName,NULL) )
  { /* ����, �޸���ֵΪ������ֵ */
    if ( xna_SetData(lpAttr,(unsigned char*)szAttributeValue,strlen(szAttributeValue))==FAIL )
    {
      g_xmlErrno = XML_ENOMEM;
      return  FAIL;
    }
  }
  else
  { /* ������, ������� */
    if( !(lpAttr=xna_Build( (unsigned char*)szAttributeName, strlen(szAttributeName),(unsigned char*)szAttributeValue )) )
    {
      g_xmlErrno = XML_ENOMEM;
      return  FAIL;
    }

    if ( sl_Append(&(lpNode->m_AttrList),lpAttr)==-1 )
    {
      xna_Destroy(lpAttr);
      g_xmlErrno = XML_ENOMEM;
      return  FAIL;
    }
  }

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  SUCC;
}



/**********************************************************************
 *  ������  xml_GetElementAttr
 *  ����: ��ȡXML����ָ���ڵ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ��ȡ���Ե�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
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
  �ɹ�  ����ʵ���ֽ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_GetElementAttr( HXMLTREE hXML,
       const char *szElementName,
       const char *szAttributeName,
       char *pucAttributeBuff,
       const size_t uiAttrBuffSize )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
PXMLNODEATTR lpAttr;

  if ( !hXML || !szAttributeName || !*szAttributeName || !pucAttributeBuff || !uiAttrBuffSize )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }
  memset( pucAttributeBuff,0,uiAttrBuffSize);

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
  {
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_GetElementAttr(...,'%s','%s',0x%p,%u).\n",
      0,szElementName,szAttributeName,pucAttributeBuff,uiAttrBuffSize);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }


  if ( !szElementName || !*szElementName )
  {
    lpNode=pXML->m_pRoot;
  }
  else
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName, NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s- [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
      return  FAIL;
    }
  }

  if ( !(lpAttr=sl_FindAttr(&(lpNode->m_AttrList),(unsigned char*)szAttributeName,NULL)) )
  {
    g_xmlErrno = XML_ENOATTR;
    snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szAttributeName);
    return  FAIL;
  }

  if ( lpAttr->m_pszData && *lpAttr->m_pszData )
  {
    if( uiAttrBuffSize<= strlen((char*)lpAttr->m_pszData) )
    {
      g_xmlErrno = XML_DFENOSPC;
      return  FAIL;
    }
    else
      strcpy(pucAttributeBuff,(char*)lpAttr->m_pszData);
  }
  else
    *pucAttributeBuff = '\0';

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  strlen(pucAttributeBuff);
}



/**********************************************************************
 *  ������  xml_DelElementAttr
 *  ����: ɾ��XML����ָ���ڵ�����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪɾ�����Ե�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
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
       const char *szAttributeName)
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
PXMLNODEATTR lpAttr;
int   nPos;

  if ( !hXML || !szAttributeName || !*szAttributeName )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
  {
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_DelElementAttr(...,'%s','%s').\n",
      0,szElementName,szAttributeName);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !szElementName || !*szElementName )
  {
    lpNode=pXML->m_pRoot;
  }
  else
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
      return  FAIL;
    }
  }

  if ( !(lpAttr=sl_FindAttr(&(lpNode->m_AttrList),(unsigned char*)szAttributeName,&nPos)) )
  {
    g_xmlErrno = XML_ENOATTR;
    snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szAttributeName);
    return  FAIL;
  }

  if ( sl_Delete(&lpNode->m_AttrList,nPos,(void*)&lpAttr)==-1 )
  {
    g_xmlErrno = XML_EINVDATAPOS;
    return  FAIL;
  }

  xna_Destroy(lpAttr);

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  SUCC;
}


/**********************************************************************
 *  ������  xml_ElementAttrExist
 *  ����: �ж�XML����ָ���ڵ������Ƿ����
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ�ж����Ե�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
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
       const char *szAttributeName)
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
int     ret;

  if ( !hXML || !szAttributeName || !*szAttributeName )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
  {
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ElementAttrExist(...,'%s','%s').\n",
      0,szElementName,szAttributeName);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !szElementName || !*szElementName )
  {
    lpNode=pXML->m_pRoot;
  }
  else
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
      return  FAIL;
    }
  }

  if ( sl_FindAttr(&(lpNode->m_AttrList),(unsigned char*)szAttributeName,NULL) )
    ret = 1;
  else
    ret = 0;

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  ret;
}



/**********************************************************************
 *  ������  xml_ModifyElementAttr
 *  ����: �޸�XML����ָ���ڵ���������
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ��ȡ���Ե�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
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
       const char *szAttributeValue )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
PXMLNODEATTR lpAttr;

  if ( !hXML || !szAttributeName || !*szAttributeName || !*szAttributeName )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
  {
    if ( szAttributeValue )
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ModifyElementAttr(...,'%s','%s','%s').\n",
        0,szElementName,szAttributeName,szAttributeValue);
    else
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ModifyElementAttr(...,'%s','%s',NULL).\n",
        0,szElementName,szAttributeName);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !szElementName || !*szElementName )
  {
    lpNode=pXML->m_pRoot;
  }
  else
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName, NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
      return  FAIL;
    }
  }

  if ( !(lpAttr=sl_FindAttr(&(lpNode->m_AttrList),(unsigned char*)szAttributeName,NULL)) )
  {
    g_xmlErrno = XML_ENOATTR;
    snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szAttributeName);
    return  FAIL;
  }

  if ( xna_SetData(lpAttr,(unsigned char*)szAttributeValue,strlen(szAttributeValue))==FAIL )
  {
    g_xmlErrno = XML_ENOMEM;
    return  FAIL;
  }
  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  SUCC;
}



/**********************************************************************
 *  ������  xml_AttributeCount
 *  ����: ��ȡXML����ָ���ڵ�����Ը���
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹ�е�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
 *  ����ֵ:
  �ɹ�  ���ظýڵ���Ԫ�ظ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_AttributeCount( HXMLTREE hXML,
       const char *szElementName )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;

  if ( !hXML )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_COUNT )
  {
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_AttributeCount(...,'%s').\n",
      0,szElementName);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !szElementName || !*szElementName )
  {
    lpNode=pXML->m_pRoot;
  }
  else
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
      return  FAIL;
    }
  }

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_COUNT )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  sl_Length(&lpNode->m_AttrList);
}




/**********************************************************************
 *  ������  xml_GetAttributeName
 *  ����: ��ȡXML���ڵ���ָ��λ��������
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
       const size_t uiAttrNameBuffSize )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
PXMLNODEATTR  lpAttr;

  if ( !hXML || !iPos || !pucAttrNameBuff || !uiAttrNameBuffSize )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
  {
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_GetAttributeName(...,'%s',%d,0x%p,%u).\n",
      0,szElementName,iPos,pucAttrNameBuff,uiAttrNameBuffSize);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !szElementName || !*szElementName )
  {
    lpNode=pXML->m_pRoot;
  }
  else
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
      return  FAIL;
    }
  }

  if ( sl_GetElem(&lpNode->m_AttrList,iPos,(void*)&lpAttr)==(-1) )
  {
    g_xmlErrno = XML_EINVDATAPOS;
    return  FAIL;
  }

  if ( lpAttr->m_pszName && *lpAttr->m_pszName )
  {
    if( uiAttrNameBuffSize<= strlen((char*)lpAttr->m_pszName) )
    {
      g_xmlErrno = XML_DFENOSPC;
      return  FAIL;
    }
    else
      strcpy(pucAttrNameBuff,(char*)lpAttr->m_pszName);
  }
  else
    *pucAttrNameBuff='\0';

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODEATTR )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  SUCC;
}




/**********************************************************************
 *  ������  xml_ElementCount
 *  ����: ͳ��XML����ָ���ڵ�Ԫ��ͬһ���ϵĸ���
 *  ����:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪͳ�Ƶ�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn/FINDNAME"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
 *  ����ֵ:
  �ɹ�  ���ظ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ElementCount( HXMLTREE hXML,
    const char *szElementName )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
int   n;
char  *pName;

  if ( !hXML || !szElementName || !*szElementName )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_COUNT )
  {
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ElementCount(...,'%s').\n",
      0,szElementName);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,&n,pXML )) )
    return  0;

  pName = strrchr(szElementName,'/');
  if ( !pName )
    pName = (char*)szElementName;
  else if ( *pName=='/' )
    pName++;

  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_COUNT )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  sl_CountNode( &lpNode->m_DownList, n, (unsigned char*)pName, strlen(pName) )+1;
}


/**********************************************************************
 *  ������  xml_ExportXMLStringKVL
 *  ����: ����KVL�ִ�����
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
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
    ���ΪNULL��""���򵼳���������
  bNodeSelf
    �Ƿ�����ڵ���������
    ��0   ����ʱ�����ýڵ�Ԫ�ء�
    0   ����ʱ�����ýڵ������ӽڵ����ݣ��������ýڵ㱾��
 *  ����ֵ:
  �ɹ�  ���ص������ֽ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ExportXMLStringKVL( HXMLTREE hXML,
      char *pucXMLBuff, size_t uiXMLBuffLen,
      const char *szExportName, int bNodeSelf )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
int num,n;

  if ( !hXML || !pucXMLBuff || !uiXMLBuffLen )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_TOXML )
  {
    if ( szExportName )
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ExportXMLString(...,0x%p,%u,%s).\n",0,pucXMLBuff,uiXMLBuffLen,szExportName);
    else
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ExportXMLString(...,0x%p,%u,NULL).\n",0,pucXMLBuff,uiXMLBuffLen);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( szExportName && *szExportName )
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szExportName,NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s] \n",xml_StringError(xml_GetLastError()),szExportName);
#endif
      return  FAIL;
    }
  }
  else
  {
    lpNode = pXML->m_pRoot;
  }

  num = 0;
  if ( uiXMLBuffLen<=0 )
  {
    g_xmlErrno = XML_DFENOSPC;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }
  /*
  memcpy(pucXMLBuff,"<?xml version='1.0' encoding=\"GB2312\"?>\n",40);
  */

  if ( (n=xn_ExportXMLStringKVL(lpNode,(char*)pucXMLBuff+num,uiXMLBuffLen-num,bNodeSelf))==FAIL)
  {
    g_xmlErrno = XML_DFENOSPC;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }
  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType&XL_TOXML && pXML->m_dwTrcType&XL_DATA )
    trc_Write(pXML->m_hTrace,"export xml string length = [%d]\n%h%m%H\n",num+n,pucXMLBuff,num+n);
#endif
  return  num+n;
}

/**********************************************************************
 *  ������  xml_ExportXMLString
 *  ����: ����XML�ִ�����
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
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
    ���ΪNULL��""���򵼳���������
  bNodeSelf
    �Ƿ�����ڵ���������
    ��0   ����ʱ�����ýڵ�Ԫ�ء�
    0   ����ʱ�����ýڵ������ӽڵ����ݣ��������ýڵ㱾��
 *  ����ֵ:
  �ɹ�  ���ص������ֽ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ExportXMLString( HXMLTREE hXML,
      char *pucXMLBuff, size_t uiXMLBuffLen,
      const char *szExportName, int bNodeSelf )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
int num,n;

  if ( !hXML || !pucXMLBuff || !uiXMLBuffLen )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_TOXML )
  {
    if ( szExportName )
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ExportXMLString(...,0x%p,%u,%s).\n",0,pucXMLBuff,uiXMLBuffLen,szExportName);
    else
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ExportXMLString(...,0x%p,%u,NULL).\n",0,pucXMLBuff,uiXMLBuffLen);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( szExportName && *szExportName )
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szExportName,NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s] \n",xml_StringError(xml_GetLastError()),szExportName);
#endif
      return  FAIL;
    }
  }
  else
  {
    lpNode = pXML->m_pRoot;
  }

  num = 40;
  if ( uiXMLBuffLen<=40 )
  {
    g_xmlErrno = XML_DFENOSPC;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }
  memcpy(pucXMLBuff,"<?xml version='1.0' encoding=\"GB2312\"?>\n",40);


  if ( (n=xn_ExportXMLString(lpNode,(char*)pucXMLBuff+num,uiXMLBuffLen-num,bNodeSelf))==FAIL)
  {
    g_xmlErrno = XML_DFENOSPC;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }
  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType&XL_TOXML && pXML->m_dwTrcType&XL_DATA )
    trc_Write(pXML->m_hTrace,"export xml string length = [%d]\n%h%m%H\n",num+n,pucXMLBuff,num+n);
#endif
  return  num+n;
}


/**********************************************************************
 *  ������  xml_ExportXMLStringEh
 *  ����: ����XML�ִ�����(��ǿ����)
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
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
    ���ΪNULL��""���򵼳���������
  bNodeSelf
    �Ƿ�����ڵ���������
    ��0   ����ʱ�����ýڵ�Ԫ�ء�
    0   ����ʱ�����ýڵ������ӽڵ����ݣ��������ýڵ㱾��
  int iLineMode             XMLͷ������ģʽ,0 ������ 1 ����
  int iParaCount           XML����ָ�����
  ...                      XML����ָ���ַ����б�
��    ��:XML����ָ���ַ����б���ֻ����ʹ���ַ����������������

 *  ����ֵ:
  �ɹ�  ���ص������ֽ���
  ʧ��  ����FAIL
 **********************************************************************/
int xml_ExportXMLStringEh( HXMLTREE hXML,
      char *pucXMLBuff, size_t uiXMLBuffLen,
      const char *szExportName, int bNodeSelf ,
      int iLineMode,int iParaCount,...
      )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
int num,n,i;
va_list vaParas;
char *p;

  if ( !hXML || !pucXMLBuff || !uiXMLBuffLen )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_TOXML )
  {
    if ( szExportName )
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ExportXMLString(...,0x%p,%u,%s).\n",0,pucXMLBuff,uiXMLBuffLen,szExportName);
    else
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ExportXMLString(...,0x%p,%u,NULL).\n",0,pucXMLBuff,uiXMLBuffLen);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( szExportName && *szExportName )
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szExportName,NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s] \n",xml_StringError(xml_GetLastError()),szExportName);
#endif
      return  FAIL;
    }
  }
  else
  {
    lpNode = pXML->m_pRoot;
  }

  num = 40;
  if ( uiXMLBuffLen<=40 )
  {
    g_xmlErrno = XML_DFENOSPC;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }
  strcpy(pucXMLBuff,"<?xml ");

  va_start(vaParas,iParaCount);
  for(i=0;i<iParaCount;++i)
  {
    p=va_arg(vaParas,char *);
    if ( (long )p <= 0 )
      break;
    strcat(pucXMLBuff,p);
    strcat(pucXMLBuff," ");

  }
  if ( iLineMode )
    strcat(pucXMLBuff,">\n");
  else
    strcat(pucXMLBuff,">");

  va_end(vaParas);


  if ( (n=xn_ExportXMLString(lpNode,(char*)pucXMLBuff+num,uiXMLBuffLen-num,bNodeSelf))==FAIL)
  {
    g_xmlErrno = XML_DFENOSPC;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }
  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType&XL_TOXML && pXML->m_dwTrcType&XL_DATA )
    trc_Write(pXML->m_hTrace,"export xml string length = [%d]\n%h%m%H\n",num+n,pucXMLBuff,num+n);
#endif
  return  num+n;
}


/*
  xml_ExportXMLStringEh�е㿴�����������ã���Ϊ��Ϊ��һ��ͷ
  Tianhc 2005-10-20 14:58
*/
int xml_ExportXMLStringHEAD( HXMLTREE hXML,
      char *pucXMLBuff, size_t uiXMLBuffLen,
      const char *szExportName, int bNodeSelf ,
      int iLineMode,char *shead)
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
int num,n,i;
va_list vaParas;
char *p;

  if ( !hXML || !pucXMLBuff || !uiXMLBuffLen )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_TOXML )
  {
    if ( szExportName )
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ExportXMLString(...,0x%p,%u,%s).\n",0,pucXMLBuff,uiXMLBuffLen,szExportName);
    else
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ExportXMLString(...,0x%p,%u,NULL).\n",0,pucXMLBuff,uiXMLBuffLen);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( szExportName && *szExportName )
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szExportName,NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s] \n",xml_StringError(xml_GetLastError()),szExportName);
#endif
      return  FAIL;
    }
  }
  else
  {
    lpNode = pXML->m_pRoot;
  }

/*
  Tianhc 2009-7-15 17:05:21
  ����<40�ֽڵ�XML����,�Ҳ���Ҫͷ���ĵ�����������
  if ( uiXMLBuffLen<=40 )
*/
  if (uiXMLBuffLen < strlen(shead))
  {
    g_xmlErrno = XML_DFENOSPC;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  /*Tianhc 2005-10-20 14:58*/
  num = strlen(shead);
  strcpy(pucXMLBuff,shead);
  if ( (iLineMode == 1) && (num > 0) )
  {
  	  strcat(pucXMLBuff,"\n");
  	  num++;
  }
  if ( (n=xn_ExportXMLString(lpNode,(char*)pucXMLBuff+num,uiXMLBuffLen-num,bNodeSelf))==FAIL)
  {
    g_xmlErrno = XML_DFENOSPC;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }
  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType&XL_TOXML && pXML->m_dwTrcType&XL_DATA )
    trc_Write(pXML->m_hTrace,"export xml string length = [%d]\n%h%m%H\n",num+n,pucXMLBuff,num+n);
#endif
  return  num+n;
}


int xml_ExportXMLStringFMT( HXMLTREE hXML,
      char *pucXMLBuff, size_t uiXMLBuffLen,
      const char *szExportName, int bNodeSelf ,
      int iLineMode,char *shead)
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
int num,n,i;
va_list vaParas;
char *p;

  if ( !hXML || !pucXMLBuff || !uiXMLBuffLen )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_TOXML )
  {
    if ( szExportName )
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ExportXMLString(...,0x%p,%u,%s).\n",0,pucXMLBuff,uiXMLBuffLen,szExportName);
    else
      trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ExportXMLString(...,0x%p,%u,NULL).\n",0,pucXMLBuff,uiXMLBuffLen);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( szExportName && *szExportName )
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szExportName,NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s] \n",xml_StringError(xml_GetLastError()),szExportName);
#endif
      return  FAIL;
    }
  }
  else
  {
    lpNode = pXML->m_pRoot;
  }

  if ( uiXMLBuffLen<=40 )
  {
    g_xmlErrno = XML_DFENOSPC;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  /*Tianhc 2005-10-20 14:58*/
  num = strlen(shead);
  strcpy(pucXMLBuff,shead);
  if ( (iLineMode == 1) && (num > 0) )
  {
  	  strcat(pucXMLBuff,"\n");
  	  num++;
  }
  if ( (n=xn_ExportXMLString_Format(lpNode,(char*)pucXMLBuff+num,uiXMLBuffLen-num,bNodeSelf,0))==FAIL)
  {
    g_xmlErrno = XML_DFENOSPC;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }
  g_xmlErrno = XML_NOERROR;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType&XL_TOXML && pXML->m_dwTrcType&XL_DATA )
    trc_Write(pXML->m_hTrace,"export xml string length = [%d]\n%h%m%H\n",num+n,pucXMLBuff,num+n);
#endif
  return  num+n;
}

/**********************************************************************
 *  ������  xml_ImportXMLString
 *  ����: ��XML��ʽ�ַ������뵽XML�ṹָ���ڵ���
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
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
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
      const char *szImportXMLString,
      const char *szImportName,
      int bReplace )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
int     ret;

  if ( !hXML || !szImportXMLString || !*szImportXMLString )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType&XL_INXML )
  {
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ImportXMLString(...,%s).\n",0,szImportName);
    if ( pXML->m_dwTrcType&XL_DATA )
      trc_Write(pXML->m_hTrace,"%h%s%H",szImportXMLString);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( szImportName && *szImportName )
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szImportName,NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szImportName);
#endif
      return  FAIL;
    }
  }
  else
  {
    lpNode = pXML->m_pRoot;
  }


  if ( lpNode->m_pszData && *(lpNode->m_pszData) && !bReplace )
  {
    g_xmlErrno = XML_EDATANODE;
    snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szImportName);
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(g_xmlErrno),szImportName);
#endif
    return  FAIL;
  }


  g_xmlErrno = XML_NOERROR;
  ret = xn_ImportXMLString(lpNode,(char*)szImportXMLString,bReplace,pXML);
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType&XL_INXML )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  ret;
}

/**********************************************************************
 *  ������  xml_ImportXMLStringKVL
 *  ����: ��KVL��ʽ�ַ������뵽XML�ṹָ���ڵ���
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
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
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
int xml_ImportXMLStringKVL( HXMLTREE hXML,
      const char *szImportXMLString,
      const char *szImportName,
      int bReplace )
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
int     ret;

  if ( !hXML || !szImportXMLString || !*szImportXMLString )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType&XL_INXML )
  {
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_ImportXMLString(...,%s).\n",0,szImportName);
    if ( pXML->m_dwTrcType&XL_DATA )
      trc_Write(pXML->m_hTrace,"%h%s%H",szImportXMLString);
  }
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( szImportName && *szImportName )
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szImportName,NULL,pXML )) )
    {
#ifdef __XML_TRACE__
      if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
        trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szImportName);
#endif
      return  FAIL;
    }
  }
  else
  {
    lpNode = pXML->m_pRoot;
  }


  if ( lpNode->m_pszData && *(lpNode->m_pszData) && !bReplace )
  {
    g_xmlErrno = XML_EDATANODE;
    snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szImportName);
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(g_xmlErrno),szImportName);
#endif
    return  FAIL;
  }


  g_xmlErrno = XML_NOERROR;
  ret = xn_ImportXMLStringKVL(lpNode,(char*)szImportXMLString,bReplace,pXML);
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType&XL_INXML )
    trc_Write(pXML->m_hTrace,"\n");
#endif
  return  ret;
}

/**********************************************************************
 *  ����: xml_asctohex
 *  ����: ��ASCII�ִ�ת��Ϊ���ֽڵ�HEX�ִ�
 *  ����һ: ����ת�����ʮ���������ݻ�����ָ��
 *  ������: ��������С
 *  ������: ��ת����ASCII����ָ��
 *  ������: ���ݳ���
 *  ����ֵ: �ɹ�,����ת���ַ�����;ʧ�ܷ���0
 **********************************************************************/
size_t xml_asctohex( char *szHexBuf, size_t size, char *szAsc, size_t num )
{
unsigned char *p,*q,*pEnd;

  if ( size < num*2 )
  {
    g_xmlErrno = XML_DFENOSPC;
    return  0;
  }


  pEnd = (unsigned char*)szAsc;
  p = pEnd+num-1;
  q = (unsigned char*)szHexBuf+num*2-1;

  for ( ; p>=pEnd; p-- )
  {

    *q = (*p%16)+'0';
    if ( *q>'9' )
      *q+=7;

    *--q = (*p/16)+'0';
    if ( *q>'9' )
      *q+=7;

    q--;
  }
  g_xmlErrno = XML_NOERROR;
  return  num*2;
}

/**********************************************************************
 *  ����: xml_hextoasc
 *  ����: �����ֽڵ�HEX�ִ�ת��ΪASCII�ִ�
 *  ����һ: ����ת�����ASCII���ݻ�����ָ��
 *  ������: ��������С
 *  ������: ��ת���Ķ��ֽڵ�HEX����ָ��
 *  ������: ���ݳ���
 *  ����ֵ: �ɹ�,����ת���ַ�����;ʧ�ܷ���0
 **********************************************************************/
size_t xml_hextoasc( char *szAscBuf, size_t size, char *szHex, size_t num )
{
unsigned char *p,*q;

  if ( size < num/2 )
  {
    g_xmlErrno = XML_DFENOSPC;
    return  0;
  }

  p = (unsigned char*)szHex;
  q = (unsigned char*)szAscBuf;

  for ( ; num; num-=2, q++ )
  {
    if ( *p>='0' && *p <= '9' )
    {
      *q = (*p-'0')*16;
    }
    else if ( *p>='A' && *p <= 'F' )
    {
      *q = (*p-55)*16;
    }
    else if ( *p>='a' && *p <= 'f' )
    {
      *q = (*p-87)*16;
    }
    else
      break;
    p++;
    if ( *p>='0' && *p <= '9' )
    {
      *q += (*p-'0');
    }
    else if ( *p>='A' && *p <= 'F' )
    {
      *q += (*p-55);
    }
    else if ( *p>='a' && *p <= 'f' )
    {
      *q += (*p-87);
    }
    else
      break;
    p++;
  }
  g_xmlErrno = XML_NOERROR;
  return  (char*)q-szAscBuf;
}

/**********************************************************************
 *  ������  xml_GetLastError
 *  ����: ��ȡ���һ�β����������
 *  ����: ��
 *  ����ֵ: ������
 **********************************************************************/
int xml_GetLastError(void)
{
  return  g_xmlErrno;
}


/**********************************************************************
 *  ������  xml_StringErrno
 *  ����: ����������ִ���
 *  ����һ: �������
 *  ����ֵ: �����ִ�
 **********************************************************************/
char *xml_StringError(int iErrno)
{
static char szErrInfo[256];

  if ( iErrno >=0 )
  {
    switch ( iErrno )
    {
    case  XML_NOERROR:
      return  "�޴���(No error)";
    case  XML_EINVAL:
      return  "��Ч����(Invalid argument)";
    case  XML_ENOMEM:
      return  "ϵͳ�ڴ�ռ䲻��(Not enough memory)";

    case  XML_DFENOSPC:
      return  "ָ���������ռ䲻��(No space left on device)";

    case  XML_ESYSTEM:
      snprintf(szErrInfo,sizeof(szErrInfo),"ϵͳ����(system error) - [%s]",strerror(errno));
      return  szErrInfo;

    case  XML_ENOROOT:
      return  "�޸��ڵ�����(No root data)";
    case  XML_EINVNODENAME:
      snprintf(szErrInfo,sizeof(szErrInfo),"��Ч�ڵ������ִ�(Invalid node name) - [%s]",g_szErrEx);
      return  szErrInfo;
    case  XML_ENONODE:
      snprintf(szErrInfo,sizeof(szErrInfo),"�����ڸýڵ�(No such node) - [%s]",g_szErrEx);
      return  szErrInfo;
    case  XML_ENOTDATANODE:
      snprintf(szErrInfo,sizeof(szErrInfo),"�������ݽڵ�(Not data node) - [%s]",g_szErrEx);
      return  szErrInfo;
    case  XML_EDATANODE:
      snprintf(szErrInfo,sizeof(szErrInfo),"�����ݽڵ�(Is data node) - [%s]",g_szErrEx);
      return  szErrInfo;
    case  XML_ENOATTR:
      snprintf(szErrInfo,sizeof(szErrInfo),"�����ڸ�����(No such attribute) - [%s]",g_szErrEx);
      return  szErrInfo;
    case  XML_EATTREXIST:
      snprintf(szErrInfo,sizeof(szErrInfo),"�������Ѵ���(Attribute exist already) - [%s]",g_szErrEx);
      return  szErrInfo;
    case  XML_EINVDATAPOS:
      return  "��Ч������λ��(Invalid data postion)";

    case  XML_EXMLSYNTAX:
      return  "XML�ִ��﷨����(Xml string syntax error)";
    case  XML_EXMLMAXLEV:
      return  "XML�ִ���������������(Xml string max lev error)";
    case  XML_EXMLNONODE:
      snprintf(szErrInfo,sizeof(szErrInfo),"XML�ִ��в����ڽ����滻�Ľڵ�(Xml string special node not exist) - [%s]",g_szErrEx);
      return  szErrInfo;
    case  XML_EOPENMAPFILE:
      return  "��ӳ���ļ�����(Open map file error)";
    case  XML_EFILENOTEXIST:
      snprintf(szErrInfo,sizeof(szErrInfo),"ӳ���ļ�������(Map file not exists) - [%s]",g_szErrEx);
      return  szErrInfo;
    case  XML_EFILENOTREAD:
      snprintf(szErrInfo,sizeof(szErrInfo),"ӳ���ļ����ɶ�(Map file can not read) - [%s]",g_szErrEx);
      return  szErrInfo;
    case  XML_EFILECONT:
      return  "ӳ���ļ����ݴ���(Map file content error)";
    case  XML_EFILECONT_FOR:
      return  "ӳ���ļ�����ѭ����ƥ��(Map file content loop error)";
    case  XML_EFILECONT_FORLVL:
      return  "ӳ���ļ����ݲ�֧�ֶ���ѭ��(Map file content loop error)";
    case  XML_EFILECONT_FORCOMM:
      return  "DSRӳ���ļ�����ѭ���в�֧�ֶ���ͨѶ��(Map file content loop error)";
    case  XML_EFILECONT_FORNUM:
      return  "ӳ���ļ�����ѭ����������(Map file content loop num error)";
    case  XML_EFILENOTENG:
      return  "ӳ���ļ�������DSR�������ݲ���(Map file and DSR content error)";
    case  XML_EFILECONT_XMLNODE:
      return  "ӳ���ļ�����XML�ڵ�������(Map file XML node name too long)";
    case  XML_EFILEMULDSR:
      return  "DSRӳ���ļ������������˶��DSR�ļ�(Map file dsr file too much)";
    case  XML_EFILENODSR:
      return  "DSRӳ���ļ�������ȱ��DSR�ļ�����(Map file need dsr file)";
    case  XML_EDSRSTRERR:
      return  "DSR�ִ�����(Dsr content error)";
    case  XML_ELOGFILENOTEXIST:
      snprintf(szErrInfo,sizeof(szErrInfo),"TRACE�����ļ�������(TRACE config file not exists) - [%s]",g_szErrEx);
      return  szErrInfo;
    case  XML_ELOGFILENOTREAD:
      snprintf(szErrInfo,sizeof(szErrInfo),"TRACE�����ļ����ɶ�(TRACE config file can not read) - [%s]",g_szErrEx);
      return  szErrInfo;
    case  XML_EBUILDTRACE:
      return  "����TRACEʧ�ܣ�����TRACE�ļ�������Ŀ¼Ȩ��(Build Trace fail, please check trace file and bak directory)";

    case  XML_EDATA_NOTENOUGTH:
      return  "�������ݳ��Ȳ���(import data not enougth)";
    case  XML_EIOTYPE:
      return  "�����ӳ���ļ�����(unknow map file type)";
    case  XML_EFILE_TYPEFIELD:
      return  "ӳ���ļ������������ִ�����(Map file datatype field content error)";
    case  XML_EFILE_ATTRFIELD:
      return  "ӳ���ļ��������ִ�����(Map file attribute field content error)";
    case  XML_E8583NOTEXIST:
      return  "������ָ����8583��(Special 8583 Field not exist)";
    case  XML_E8583HEAD:
      return  "8583ͷ�����ݲ�ƥ��(8583 head data error)";
    case  XML_E8583POS:
      return  "8583������ƥ��(8583 field error)";

    default:
      return  "δ֪�������(Unknow errno)";
    }
  }
  else
  {
    switch ( iErrno )
    {
    case -1:
      return  "ͨѶ�˿�д�� [socket write error]";
    case -2:
      return  "ͨѶ�˿ڶ��� [socket read error]";
    case -7:
      return  "���ݳ��ȴ��� [data length error]";
    case -8:
      return  "ͨѶЭ����� [commun procotol error]";
    case -9:
      return  "[server error]";
    case -10:
      return  "����У����� [data crc check error]";
    case -11:
      return  "���״���ʱ [trade deal timeout]";
    case -99:
      return  "ͨѶ��ʼ��ʧ�� [commun init failed]";
    default:
      if ( iErrno>-200 )
      {
        snprintf(szErrInfo,sizeof(szErrInfo),"�޷������Ĵ��� [unknown error],code=[%d]",iErrno);
        return  szErrInfo;
      }
      switch ( iErrno+200 )
      {
      case  -1:
        return  "����� [package overflow]";
      case  -2:
        return  "�������ʹ����� [package can not send to host]";
      case  -3:
        return  "�����ظ������� [host response error]";
      case  -4:
        return  "��������̫�� [package too large to host]";
      default:
        snprintf(szErrInfo,sizeof(szErrInfo),"DSR���ػ�����״̬����[server error],code=[%d]",iErrno+200);
        return  szErrInfo;
      }
    }
  }
}

#ifdef __XML_TRACE__
/* ��ȡTRACE��� */
int xml_GetTraceHandle( HXMLTREE hXML )
{
PXMLSTRUCT  pXML;

  if ( !hXML  )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }
  pXML =(PXMLSTRUCT)hXML;
  return  pXML->m_hTrace;
}
#endif

/**********************************************************************
 *  ������  xml_GetVerion
 *  ����: ��ȡ�汾��
 *  ����: ��
 *  ����ֵ: �汾��
    ���ֽ�Ϊ��汾��
    ���ֽ�Ϊ˳���
  **********************************************************************/
#define MAKELONG_(a, b)      ((unsigned int)(((unsigned short)(a)) | ((unsigned int)((unsigned short)(b))) << 16))
unsigned int xml_GetVerion()
{
  return  MAKELONG_(97,0);
}

/**********************************************************************
 *  ������  xml_GetLastVerDate
 *  ����: ��ȡ���汾����
 *  ����: ��
 *  ����ֵ: �汾���ڴ�, ��ʽΪ "yyyy-mm-dd"
  **********************************************************************/
char *xml_GetLastVerDate()
{
  return  "2001-12-13";
}

/**********************************************************************
 *  ����: FileFmtOutput
 *  ����: �� 16 ���ƺ��ַ����ֺϲ���ʽ���һ�黺��������
 *  ����һ: �򿪵�����ļ�ָ��
 *  ������: ������ָ��
 *  ������: ���ݴ�С
 *  ������: ��ַ��ʼֵ
 *  ����ֵ: ��
 **********************************************************************/
void FileFmtOutput( FILE *fpFile, const void *pBuf, size_t nLen, int nStart )
{
unsigned char *pt,*p;
int nStep;

  p = pt = (unsigned char*)pBuf;
  while ( nLen ) {

    /* ǰ��ַ */
    nStep=16;
    if ( nStart >= 0 )
      fprintf( fpFile, "[%07X0] ", nStart++ );

    /* ǰ16���ַ� */
    for ( ; nLen && nStep>8; nLen--,nStep--,p++ )
      fprintf( fpFile, "%02X ", *p );

    for ( ; nStep > 8; nStep-- )
      fprintf( fpFile, "   " );

    fprintf( fpFile, " " );

    /* ��16���ַ� */
    for ( ; nLen && nStep; nLen--,nStep--,p++ )
      fprintf( fpFile, "%02X ", *p );

    for ( ; nStep; nStep-- )
      fprintf( fpFile, "   " );

    fprintf( fpFile, "  " );

    /* ����ı��ַ� */
/*    nStep = p-pt; */
    for ( ; pt < p; pt++ ) {
      if( isprint( *pt ) || ( *pt & 0x80 ) )
        fprintf( fpFile, "%c", *pt );
      else
        fprintf( fpFile, "." );
    }

/*    if ( nStep==16 ) */
      fprintf( fpFile, "\n" );
  }
  fflush( fpFile );
}

#ifdef WIN32
int exchange(long tradetype,char* databuff,long datalen)
{
  return  0;
}
void setcfgfile(char *szFile)
{
}

#endif


/************************************************************************
��������:xml_GetElementAttrP
��������:��ȡXML����ָ���ڵ���������ָ��
�������:
  hXML
    ָ��XML�豸���
  szElementName
    ָ����XML�ṹҪ��ȡ���Ե�Ԫ�ص����ƣ�����"/PRIVATE|2/��ѵ���|0"
    "/NAME1|IDX1/NAME2|IDX2/.../NAMEn|IDXn"
    NMAE  ָ��ÿһ��Ԫ����,
    IDX   ָ��ÿһ��Ԫ����ͬ���ֵ�Ԫ���е�����λ�á�
      ���iIndexΪ0����ʾȡ���һ��Ԫ�أ�
      ���Ϊ1��2��3��������ʾȡ��Ӧ��ŵ�Ԫ�ء�
      ȱʡΪ1
  szAttributeName
    ������ָ��
��    ��:
  �ɹ�  �ڵ���������ָ��
  ʧ��  NULL
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:
��������:2000��01��01��
�޸�����:2002��04��01��
��:
 **********************************************************************/
char* xml_GetElementAttrP( HXMLTREE hXML,
       const char *szElementName,
       const char *szAttributeName)
{
PXMLSTRUCT  pXML;
PXMLNODE  lpNode;
PXMLNODEATTR lpAttr;

  if ( !hXML || !szAttributeName || !*szAttributeName   )
  {
    g_xmlErrno = XML_EINVAL;
    return  NULL;
  }

  pXML = (PXMLSTRUCT)hXML;

  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
    return  NULL;
  }


  if ( !szElementName || !*szElementName )
  {
    lpNode=pXML->m_pRoot;
  }
  else
  {
    if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName, NULL,pXML )) )
      return  NULL;
  }

  if ( !(lpAttr=sl_FindAttr(&(lpNode->m_AttrList),(unsigned char*)szAttributeName,NULL)) )
  {
    g_xmlErrno = XML_ENOATTR;
    snprintf(g_szErrEx,sizeof(g_szErrEx),"%s",szAttributeName);
    return  NULL;
  }

  g_xmlErrno = XML_NOERROR;
  return  (char*)lpAttr->m_pszData;
}

/************************************************************************
��������:
char* xml_GetElementP( HXMLTREE hXML, const char *szElementName)
��������:��ȡXML����ָ���ڵ�����ָ��
�������:
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
��    ��:
  �ɹ�  �ڵ�����ָ��
  ʧ��  NULL
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:
��������:2000��01��01��
�޸�����:2002��04��01��
��:
************************************************************************/
char* xml_GetElementP( HXMLTREE hXML, const char *szElementName)
{
PXMLSTRUCT pXML;
PXMLNODE  lpNode;

  if ( !hXML || !szElementName || !*szElementName  )
  {
    g_xmlErrno = XML_EINVAL;
    return  NULL;
  }

  pXML = (PXMLSTRUCT)hXML;
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
    return  NULL;
  }

  if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
    return  NULL;


  g_xmlErrno = XML_NOERROR;
  return  (char*)lpNode->m_pszData;
}

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
 **********************************************************************/
int xml_GetElementLen( HXMLTREE hXML,
       const char *szElementName)
{
PXMLSTRUCT pXML;
PXMLNODE  lpNode;

  if ( !hXML || !szElementName || !*szElementName )
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;

  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
    return  FAIL;
  }

  if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
  {
    return  FAIL;
  }

  g_xmlErrno = XML_NOERROR;
  return (lpNode->m_pszData == NULL ? 0 : strlen((char*)lpNode->m_pszData));
}


#ifdef __cplusplus
}
#endif /* end of __cplusplus */

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
HXMLTREE xml_CopyTree(HXMLTREE hXML, const char *szElementName)
{
	PXMLSTRUCT pXML, ptXML;
  PXMLNODE  lpNode;
  char *p;

  if ( !hXML || !szElementName || !*szElementName)
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_CopyTree(...,'%s').\n",0,szElementName);
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
  {
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR && g_xmlErrno==XML_ENONODE )
      trc_Write(pXML->m_hTrace,"error: %s - [%s]\n",xml_StringError(xml_GetLastError()),szElementName);
#endif
    return  FAIL;
  }
  
  if (!(ptXML=(PXMLSTRUCT)calloc(1,sizeof(XMLSTRUCT))) )
	{
		g_xmlErrno = XML_ENOMEM;
		return	FAIL;
	}
	
	if ( !(ptXML->m_pRoot=xn_Copy(lpNode)) )
	{
		free(ptXML);
		return FAIL;
	}
	
#ifdef	__XML_TRACE__
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
			trc_Write(pXML->m_hTrace,">>> %t <<<  xml_CopyTree('%s').\n\n",0,pXML->m_pRoot->m_pszName);
		}
	}
#endif
	return (HXMLTREE) ptXML;
}

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
int xml_LoadTree(HXMLTREE hXML, const char *szElementName, HXMLTREE *hLoadXML)
{
	PXMLSTRUCT pXML, ptXML;
  PXMLNODE  lpNode;
  char *p;

  if ( !hXML || !hLoadXML || !szElementName || !*szElementName)
  {
    g_xmlErrno = XML_EINVAL;
    return  FAIL;
  }

  pXML = (PXMLSTRUCT)hXML;
#ifdef __XML_TRACE__
  if ( pXML->m_dwTrcType & XL_NODE )
    trc_Write(pXML->m_hTrace,">>> %t <<<  xml_LoadTree(...,'%s').\n",0,szElementName);
#endif
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( pXML->m_dwTrcType&XL_ERROR )
      trc_Write(pXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }

  if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
  {
    if ( g_xmlErrno!=XML_ENONODE )
      return  FAIL;

    /* �����ڸýڵ���� */
    lpNode = xn_AddNode( pXML->m_pRoot, (char*)szElementName, "", 0, pXML );
    if ( !lpNode )
    {
      return  FAIL;
    }
  }
  
  ptXML = *(PXMLSTRUCT *)hLoadXML;
#ifdef __XML_TRACE__
  if ( ptXML->m_dwTrcType & XL_NODE )
    trc_Write(ptXML->m_hTrace,">>> %t <<<  xml_LoadTree(...,'%s').\n",0,szElementName);
#endif
  if ( !ptXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
#ifdef __XML_TRACE__
    if ( ptXML->m_dwTrcType&XL_ERROR )
      trc_Write(ptXML->m_hTrace,"error: %s\n",xml_StringError(XML_ENOROOT));
#endif
    return  FAIL;
  }
  
  sl_NodeDestroy( &lpNode->m_DownList );
	sl_AttrDestroy( &lpNode->m_AttrList );
	if ( lpNode->m_pszData )
		free( lpNode->m_pszData );
  
  memcpy(&lpNode->m_DownList, &ptXML->m_pRoot->m_DownList, sizeof(WSEQLIST));
  memcpy(&lpNode->m_AttrList, &ptXML->m_pRoot->m_AttrList, sizeof(WSEQLIST));
  lpNode->m_pszData = ptXML->m_pRoot->m_pszData;
  if (ptXML->m_pRoot->m_pszName)
  	free(ptXML->m_pRoot->m_pszName);
  free(ptXML->m_pRoot);
  free(ptXML);
  *hLoadXML = 0;
	return SUCC;
}


/************************************************************************
��������:
PXMLNODE xml_GetNode( HXMLTREE hXML, const char *szElementName)
��������:��ȡXML����ָ���ڵ�Nodeָ��
�������:
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
��    ��:
  �ɹ�  �ڵ�ָ��
  ʧ��  NULL
�� Ŀ ��:�м�ҵ���Ʒ�з���
�� �� Ա:
��������:2000��01��01��
�޸�����:2005-08-18 13:11
��:
************************************************************************/
PXMLNODE xml_GetNode( HXMLTREE hXML, const char *szElementName)
{
PXMLSTRUCT pXML;
PXMLNODE  lpNode;

  if ( !hXML || !szElementName || !*szElementName  )
  {
    g_xmlErrno = XML_EINVAL;
    return  NULL;
  }

  pXML = (PXMLSTRUCT)hXML;
  if ( !pXML->m_pRoot )
  {
    g_xmlErrno = XML_ENOROOT;
    return  NULL;
  }

  if ( !(lpNode=xn_LocateNode( pXML->m_pRoot, (char*)szElementName,NULL,pXML )) )
    return  NULL;

  return lpNode;
}


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
int xml_node_strlen(HXMLTREE lXmlhandle,char *snode)
{
   PXMLNODE  lpNode;
   int iret=0;

	 lpNode = xml_GetNode( lXmlhandle,snode);
	 if (lpNode == NULL )
	 	  return -1;
	 iret = xml_node_explen(lXmlhandle,lpNode);
	 return iret;
}


int xml_node_explen(HXMLTREE lXmlhandle,PXMLNODE lpNode)
{
PWSEQLIST	lpList;
PXMLNODEATTR *lpElemP;
PXMLNODE *lpNodeP;
int		iNameLen,iDataLen,len1,len2;
int		i,n,num;
unsigned char	*p;

	num = 0;


	if ( 1 ) /*����ڵ�*/
	{
		iNameLen=strlen((char*)lpNode->m_pszName);
		num	+= iNameLen;

		/* ������������ */
		lpList = &lpNode->m_AttrList;
		if ( i=sl_Length(lpList) )
		{
			lpElemP=(PXMLNODEATTR*)lpList->m_lpBase;
			for ( ; i; i--, lpElemP++ )
			{
				len1 = strlen((char*)(*lpElemP)->m_pszName);
				num += (len1+4);

				if ( (*lpElemP)->m_pszData )
				{
					len2=prv_execlen((*lpElemP)->m_pszData,strlen((char*)(*lpElemP)->m_pszData));
					if (len2 < 0)
						return	FAIL;
					num += len2;
				}
			}
		}
		num++;
	}

	/* �����ӽڵ����� */
	lpList = &lpNode->m_DownList;
	if ( i=sl_Length(lpList) )
	{
		lpNodeP		= (PXMLNODE*)lpList->m_lpBase;
		for ( ; i; i--,lpNodeP++ )
		{
			n=xml_node_explen(lXmlhandle,(*lpNodeP));
			if (n<0)
				return	n;
			num+=n;
		}
	}

	if ( 1 ) /*����ڵ�*/
	{
		/* �����ڵ�ֵ���� */
		if ( lpNode->m_pszData )
		{
			if ( (iDataLen=prv_execlen(lpNode->m_pszData,lpNode->m_iDataLen))==FAIL )
				return	FAIL;
			num += iDataLen;
		}

		num += (iNameLen+4);
	}

	return	num;
}

/*
   Tianhc 2006-9-12 11:08
     --ԭxml_NodeMove������,��д,��ɵĺ����㷨��ͬ
   Ŀ������ڣ��������Ŀ��ڵ�(����ɾ��,����ͬ���ڵ��޷�����)
         �����ڣ���һ���յĽڵ㣻
   ����sl_Delete() + xn_Move()�ƶ� (���ԭ�ڵ����丸�ڵ��ϵĵǼ�+�ƶ�+free)
*/
int xml_NodeMove( HXMLTREE hXML, const char *snode_dest,const char *snode_src)
{
  PXMLSTRUCT pXML;
  PXMLNODE  lpNode_src,lpNode_dest,lpNode_src_parent;
	char sname_save[255];
	int i,pnUpPos,m_id_save;
  pXML = (PXMLSTRUCT)hXML;
  if ( !pXML->m_pRoot )
     return -99;
	lpNode_src  = xml_GetNode(hXML, snode_src);
  if (lpNode_src==NULL)
  	 return -1;
  if ( !strcmp(snode_src,"/") || !strcmp(snode_dest,"/") )
  	 return -2;
  if ((lpNode_src_parent=xn_LocateNode( pXML->m_pRoot, (char*)snode_src, &pnUpPos, pXML )) == NULL )
  	 return -3;
	if (sl_Delete(&lpNode_src_parent->m_DownList,pnUpPos,NULL) != 0)
		 return -4;
	lpNode_dest  = xml_GetNode(hXML, snode_dest);
  if (lpNode_dest == NULL)  /*������������һ���յĽ��*/
  {
  	  if (xml_SetElement(hXML,snode_dest,"") < 0)
         return -5;
  	  lpNode_dest  = xml_GetNode(hXML, snode_dest);
  }
  snprintf(sname_save,sizeof(sname_save),lpNode_dest->m_pszName);
  m_id_save = lpNode_dest->m_id;
  if (xn_Move(lpNode_dest,lpNode_src) != 0)
  	 return -6;
  /*�ڵ����ΪĿ����*/
  if (xn_Rename(lpNode_dest,sname_save) != 0)
  	 return -7;
  lpNode_dest->m_id=m_id_save;
  return 0;
}

int prv_execlen(char *content,int isize)
{
	  char *pstr;
	  int ilen2=0,i;

	  ilen2 = isize;
	   for (i=0,pstr=content; i<isize; i++,pstr++)
	   {
	   	  if (*pstr == '<' || *pstr == '>')
	   	  	 ilen2 += 3;
	   	  if (*pstr == '&')
	   	  	 ilen2 += 4;
	   	  if (*pstr == '\'')
	   	  	 ilen2 += 5;
	   	  if (*pstr == '"')
	   	  {
	   	  	if (g_xmlquot == 1)
	   	  	 ilen2 += 5;
	   	  	else
	   	  	 ilen2 += 6;
	   	  }
	   }
	 return ilen2;
}


/**********************************************************************
 *	����:	����xn_Setquot
 *	����:	�����ڲ���"�ַ���ת����׼,1-����Ϊ&quot;(XML��׼),0-����Ϊ&quote;��ϵͳ����ı�׼
 *	����һ:	ģʽ
 *	��ע��	ֻΪ���ݾ�ģʽ��һ������
 *       �����Ϊ�±�׼�����ϵͳ��ϲ����±���,����ASPK+IFI����Ӧ��,�нϴ�������׳�����
 *       xn_����Ϊ�ڲ�����,����������,�������ٰ�һ��
 **********************************************************************/
void xml_Setquot(int imode)
{
	 xn_Setquot(imode);
}

