

/************************************************************************
Copyright   : 2005-2008, Huawei Tech. Co., Ltd.
File name   : SimApi.c
Author      : H59254
Version     : V200R001
Date        : 2008-10-18
Description : ���ļ�������usimģ���ṩ��APP/AT��SIM���ӿڵ�ʵ��

Description : SIM��APP/AT�Ľӿ�
History     :

1.��    ��  : 2008��10��14��
  ��    ��  : H59254
  �޸�����  : Create
************************************************************************/

#include "vos.h"
#include "si_pih.h"
#include "si_pb.h"
#include "sitypedef.h"
#include "product_config.h"
#include "NasNvInterface.h"
#include "NVIM_Interface.h"
#include "AtOamInterface.h"



/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/
#define    THIS_FILE_ID PS_FILE_ID_PIH_API_C


/*****************************************************************************
�� �� ��  :SI_PIH_IsSvlte
��������  :�жϵ�������ӿ��Ƿ���SVLTE��
�������  :��
�������  :��
�޶���¼  :
1. ��    ��   : 2014��1��2��
   ��    ��   : h59254
   �޸�����   : Creat
*****************************************************************************/
VOS_UINT32 SI_PIH_IsSvlte(VOS_VOID)
{
    SVLTE_SUPPORT_FLAG_STRU             stSvlteFlag = {0};

    if (VOS_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_SVLTE_FLAG, &stSvlteFlag, sizeof(SVLTE_SUPPORT_FLAG_STRU)))
    {
        return VOS_FALSE;
    }

    return (VOS_UINT32)stSvlteFlag.ucSvlteSupportFlag;

}

/*****************************************************************************
�� �� ��  : SI_PIH_GetReceiverPid
��������  : FDN����
�������  : ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2008��10��18��
  ��    ��  : H59254
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_GetReceiverPid(
    MN_CLIENT_ID_T                      ClientId,
    VOS_UINT32                          *pulReceiverPid)
{
    MODEM_ID_ENUM_UINT16    enModemID;

    /* ���ýӿڻ�ȡModem ID */
    if(VOS_OK != AT_GetModemIdFromClient(ClientId,&enModemID))
    {
        return VOS_ERR;
    }

    if(MODEM_ID_1 == enModemID)
    {
        *pulReceiverPid = I1_MAPS_PIH_PID;
    }
    else
    {
        *pulReceiverPid = I0_MAPS_PIH_PID;
    }

    if (VOS_TRUE == SI_PIH_IsSvlte())
    {
        *pulReceiverPid = I0_MAPS_PIH_PID;
    }

    return VOS_OK;
}


/*****************************************************************************
�� �� ��  : SI_PIH_FdnEnable
��������  : FDN����
�������  : ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2008��10��18��
  ��    ��  : H59254
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_FdnEnable (
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    VOS_UINT8                           *pPIN2)
{
    SI_PIH_FDN_ENABLE_REQ_STRU     *pMsg;
    VOS_UINT32                      ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_FdnEnable:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }


    pMsg = (SI_PIH_FDN_ENABLE_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_FDN_ENABLE_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        PIH_WARNING_LOG("SI_PIH_FdnEnable:WARNING AllocMsg FAILED.");
        return TAF_FAILURE;
    }

    pMsg->stMsgHeader.ulReceiverPid = ulReceiverPid;
    pMsg->stMsgHeader.ulMsgName     = SI_PIH_FDN_ENABLE_REQ;
    pMsg->stMsgHeader.usClient      = ClientId;
    pMsg->stMsgHeader.ucOpID        = OpId;
    pMsg->stMsgHeader.ulEventType   = SI_PIH_EVENT_FDN_CNF;

    if(VOS_NULL_PTR != pPIN2)
    {
        PAM_MEM_CPY_S(pMsg->aucPIN2, SI_PIH_PIN_CODE_LEN, pPIN2, SI_PIH_PIN_CODE_LEN);
    }

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PIH_WARNING_LOG("SI_PIH_FdnEnable:WARNING SendMsg FAILED.");
        return TAF_FAILURE;
    }

    return TAF_SUCCESS;

}

/*****************************************************************************
�� �� ��  : SI_PIH_FdnDisable
��������  : FDNȥ����
�������  : ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2008��10��18��
  ��    ��  : H59254
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_FdnDisable (
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    VOS_UINT8                           *pPIN2)
{
    SI_PIH_FDN_DISABLE_REQ_STRU     *pMsg;
    VOS_UINT32                      ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_FdnDisable:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }


    pMsg = (SI_PIH_FDN_DISABLE_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_FDN_DISABLE_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        PIH_WARNING_LOG("SI_PIH_FdnDisable:WARNING AllocMsg FAILED");
        return TAF_FAILURE;
    }

    pMsg->stMsgHeader.ulReceiverPid = ulReceiverPid;
    pMsg->stMsgHeader.ulMsgName     = SI_PIH_FDN_DISALBE_REQ;
    pMsg->stMsgHeader.usClient      = ClientId;
    pMsg->stMsgHeader.ucOpID        = OpId;
    pMsg->stMsgHeader.ulEventType   = SI_PIH_EVENT_FDN_CNF;

    if(VOS_NULL_PTR != pPIN2)
    {
        PAM_MEM_CPY_S(pMsg->aucPIN2, SI_PIH_PIN_CODE_LEN, pPIN2, SI_PIH_PIN_CODE_LEN);
    }

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PIH_WARNING_LOG("SI_PIH_FdnDisable:WARNING SendMsg FAILED");
        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_FdnBdnQuery
��������  : FDN״̬��ѯ
�������  : ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2008��10��20��
  ��    ��  : H59254
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_FdnBdnQuery(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    SI_PIH_QUERY_TYPE_ENUM_UINT32       enQueryType)
{
    SI_PIH_MSG_HEADER_STRU          *pMsg;
    VOS_UINT32                      ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_FdnBdnQuery:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    pMsg = (SI_PIH_MSG_HEADER_STRU *)VOS_AllocMsg(WUEPS_PID_AT,
                        sizeof(SI_PIH_MSG_HEADER_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        PIH_WARNING_LOG("SI_PIH_FdnDisable:WARNING AllocMsg FAILED");
        return TAF_FAILURE;
    }

    pMsg->ulReceiverPid = ulReceiverPid;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;

    if( SI_PIH_FDN_QUERY == enQueryType )
    {
        pMsg->ulEventType   = SI_PIH_EVENT_FDN_CNF;
        pMsg->ulMsgName     = SI_PIH_FDN_QUERY_REQ;
    }
    else
    {
        pMsg->ulEventType   = SI_PIH_EVENT_BDN_CNF;
        pMsg->ulMsgName     = SI_PIH_BDN_QUERY_REQ;
    }

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PIH_WARNING_LOG("SI_PIH_FdnDisable:WARNING SendMsg FAILED");
        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_GenericAccessReq
��������  : FDNȥ����
�������  : ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2008��10��18��
  ��    ��  : H59254
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_GenericAccessReq(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    SI_PIH_CSIM_COMMAND_STRU            *pstData)
{
    SI_PIH_GACCESS_REQ_STRU         *pMsg;
    VOS_UINT32                      ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_GenericAccessReq:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    if(pstData->ulLen == 0)
    {
        PIH_ERROR_LOG("SI_PIH_GenericAccessReq: Data Len is Error");
        return TAF_FAILURE;
    }

    if(  (pstData->ulLen > SI_PIH_APDU_HDR_LEN)
        &&(pstData->aucCommand[4] != pstData->ulLen-SI_PIH_APDU_HDR_LEN))
    {
        PIH_ERROR_LOG("SI_PIH_GenericAccessReq: Data Len is Not Eq P3");
        return TAF_FAILURE;
    }

    pMsg = (SI_PIH_GACCESS_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT,
                sizeof(SI_PIH_GACCESS_REQ_STRU)-VOS_MSG_HEAD_LENGTH+pstData->ulLen);

    if (VOS_NULL_PTR == pMsg)
    {
        PIH_WARNING_LOG("SI_PIH_GenericAccessReq: AllocMsg FAILED");
        return TAF_FAILURE;
    }

    pMsg->stMsgHeader.ulReceiverPid = ulReceiverPid;
    pMsg->stMsgHeader.ulMsgName     = SI_PIH_GACCESS_REQ;
    pMsg->stMsgHeader.usClient      = ClientId;
    pMsg->stMsgHeader.ucOpID        = OpId;
    pMsg->stMsgHeader.ulEventType   = SI_PIH_EVENT_GENERIC_ACCESS_CNF;
    pMsg->ulDataLen                 = pstData->ulLen;

    if(0 != pstData->ulLen)
    {
        PAM_MEM_CPY_S(pMsg->aucData, pstData->ulLen, pstData->aucCommand, pstData->ulLen);
    }

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PIH_WARNING_LOG("SI_PIH_FdnDisable:WARNING SendMsg FAILED");
        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_IsdbAccessReq
��������  : ISDB͸��APDU�ӿں���
�������  : ClientId:�û�ID
            OpId    :Op��ID
            pstData :���������ṹ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2012��08��28��
  ��    ��  : H59254
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_IsdbAccessReq(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    SI_PIH_ISDB_ACCESS_COMMAND_STRU    *pstData)
{
    SI_PIH_ISDB_ACCESS_REQ_STRU     *pstMsg;
    VOS_UINT32                      ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_IsdbAccessReq:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    if (0 == pstData->ulLen)
    {
        PIH_ERROR_LOG("SI_PIH_IsdbAccessReq: Data Len is Error");

        return TAF_FAILURE;
    }

    pstMsg  = (SI_PIH_ISDB_ACCESS_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT,
                                                   sizeof(SI_PIH_ISDB_ACCESS_REQ_STRU) - VOS_MSG_HEAD_LENGTH + pstData->ulLen);

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_WARNING_LOG("SI_PIH_IsdbAccessReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstMsg->stMsgHeader.ulReceiverPid   =   ulReceiverPid;
    pstMsg->stMsgHeader.ulMsgName       =   SI_PIH_ISDB_ACCESS_REQ;
    pstMsg->stMsgHeader.usClient        =   ClientId;
    pstMsg->stMsgHeader.ucOpID          =   OpId;
    pstMsg->stMsgHeader.ulEventType     =   SI_PIH_EVENT_ISDB_ACCESS_CNF;
    pstMsg->ulDataLen                   =   pstData->ulLen;

    if (0 != pstData->ulLen)
    {
        PAM_MEM_CPY_S(pstMsg->aucData, pstData->ulLen, pstData->aucCommand, pstData->ulLen);
    }

    if (VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pstMsg))
    {
        PIH_WARNING_LOG("SI_PIH_IsdbAccessReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_CchoSetReq
��������  : ���߼�ͨ������
�������  : ClientId:�û�ID
            OpId    :Op��ID
            pstCchoCmd :���������ṹ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2013��05��15��
  ��    ��  : g47350
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_CchoSetReq(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    SI_PIH_CCHO_COMMAND_STRU           *pstCchoCmd)
{
    SI_PIH_CCHO_SET_REQ_STRU           *pstMsg;
    VOS_UINT32                          ulReceiverPid;

    /* ������� */
    if ((0 == pstCchoCmd->ulAIDLen)
        || ((USIMM_AID_LEN_MAX*2) < pstCchoCmd->ulAIDLen))
    {
        PIH_ERROR_LOG("SI_PIH_CchoSetReq: AID length is incorrect.");

        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_CchoSetReq:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    /* ������Ϣ�ڴ� */
    pstMsg  = (SI_PIH_CCHO_SET_REQ_STRU*)VOS_AllocMsg(WUEPS_PID_AT,
                            (VOS_UINT32)(sizeof(SI_PIH_CCHO_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_WARNING_LOG("SI_PIH_CchoSetReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstMsg->stMsgHeader.ulReceiverPid   =   ulReceiverPid;
    pstMsg->stMsgHeader.ulMsgName       =   SI_PIH_CCHO_SET_REQ;
    pstMsg->stMsgHeader.usClient        =   ClientId;
    pstMsg->stMsgHeader.ucOpID          =   OpId;
    pstMsg->stMsgHeader.ulEventType     =   SI_PIH_EVENT_CCHO_SET_CNF;
    pstMsg->ulAIDLen                    =   pstCchoCmd->ulAIDLen;

    PAM_MEM_CPY_S(pstMsg->aucADFName, pstCchoCmd->ulAIDLen, pstCchoCmd->pucADFName, pstCchoCmd->ulAIDLen);

    if (VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pstMsg))
    {
        PIH_WARNING_LOG("SI_PIH_CchoSetReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;

}

/*****************************************************************************
�� �� ��  : SI_PIH_CchpSetReq
��������  : ���߼�ͨ������
�������  : ClientId:�û�ID
            OpId    :Op��ID
            pstCchpCmd :���������ṹ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2016��09��26��
  ��    ��  : z00377832
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_CchpSetReq(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    SI_PIH_CCHP_COMMAND_STRU           *pstCchpCmd)
{
    SI_PIH_CCHP_SET_REQ_STRU           *pstMsg;
    VOS_UINT32                          ulReceiverPid;

    /* ������� */
    if ((0 == pstCchpCmd->ulAIDLen)
      || ((USIMM_AID_LEN_MAX*2) < pstCchpCmd->ulAIDLen))
    {
        PIH_ERROR_LOG("SI_PIH_CchpSetReq: AID length is incorrect.");

        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_CchpSetReq:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    /* ������Ϣ�ڴ� */
    pstMsg  = (SI_PIH_CCHP_SET_REQ_STRU*)VOS_AllocMsg(WUEPS_PID_AT,
                            (VOS_UINT32)(sizeof(SI_PIH_CCHP_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_WARNING_LOG("SI_PIH_CchpSetReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstMsg->stMsgHeader.ulReceiverPid   =   ulReceiverPid;
    pstMsg->stMsgHeader.ulMsgName       =   SI_PIH_CCHP_SET_REQ;
    pstMsg->stMsgHeader.usClient        =   ClientId;
    pstMsg->stMsgHeader.ucOpID          =   OpId;
    pstMsg->stMsgHeader.ulEventType     =   SI_PIH_EVENT_CCHP_SET_CNF;
    pstMsg->ucAPDUP2                    =   pstCchpCmd->ucAPDUP2;
    pstMsg->ulAIDLen                    =   pstCchpCmd->ulAIDLen;

    PAM_MEM_CPY_S(pstMsg->aucADFName, pstCchpCmd->ulAIDLen, pstCchpCmd->pucADFName, pstCchpCmd->ulAIDLen);

    if (VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pstMsg))
    {
        PIH_WARNING_LOG("SI_PIH_CchpSetReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;

}


/*****************************************************************************
�� �� ��  : SI_PIH_CchcSetReq
��������  : �ر��߼�ͨ������
�������  : ClientId:�û�ID
            OpId    :Op��ID
            pstCchoCmd :���������ṹ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2013��05��15��
  ��    ��  : g47350
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_CchcSetReq(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    VOS_UINT32                          ulSessionID)
{
    SI_PIH_CCHC_SET_REQ_STRU           *pstMsg;
    VOS_UINT32                          ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_CchcSetReq:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    /* ������Ϣ�ڴ� */
    pstMsg  = (SI_PIH_CCHC_SET_REQ_STRU*)VOS_AllocMsg(WUEPS_PID_AT,
                        sizeof(SI_PIH_CCHC_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_WARNING_LOG("SI_PIH_CchcSetReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstMsg->stMsgHeader.ulReceiverPid   =   ulReceiverPid;
    pstMsg->stMsgHeader.ulMsgName       =   SI_PIH_CCHC_SET_REQ;
    pstMsg->stMsgHeader.usClient        =   ClientId;
    pstMsg->stMsgHeader.ucOpID          =   OpId;
    pstMsg->stMsgHeader.ulEventType     =   SI_PIH_EVENT_CCHC_SET_CNF;
    pstMsg->ulSessionID                 =   ulSessionID;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, pstMsg))
    {
        PIH_WARNING_LOG("SI_PIH_CchcSetReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;

}

/*****************************************************************************
�� �� ��  : SI_PIH_CglaSetReq
��������  : ͸���߼�ͨ��APDU�ӿں���
�������  : ClientId:�û�ID
            OpId    :Op��ID
            pstData :���������ṹ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2013��05��15��
  ��    ��  : g47350
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_CglaSetReq(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    SI_PIH_CGLA_COMMAND_STRU           *pstData)
{
    SI_PIH_CGLA_REQ_STRU               *pstMsg;
    VOS_UINT32                          ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_CglaSetReq:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    /* ������Ϣ�ڴ� */
    pstMsg  = (SI_PIH_CGLA_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT,
                                                   sizeof(SI_PIH_CGLA_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_WARNING_LOG("SI_PIH_CglaSetReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstMsg->stMsgHeader.ulReceiverPid   =   ulReceiverPid;
    pstMsg->stMsgHeader.ulMsgName       =   SI_PIH_CGLA_SET_REQ;
    pstMsg->stMsgHeader.usClient        =   ClientId;
    pstMsg->stMsgHeader.ucOpID          =   OpId;
    pstMsg->stMsgHeader.ulEventType     =   SI_PIH_EVENT_CGLA_SET_CNF;
    pstMsg->ulSessionID                 =   pstData->ulSessionID;
    pstMsg->ulDataLen                   =   pstData->ulLen;

    PAM_MEM_CPY_S(pstMsg->aucData, pstData->ulLen, pstData->pucCommand, pstData->ulLen);

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, pstMsg))
    {
        PIH_WARNING_LOG("SI_PIH_CglaSetReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_GetCardATRReq
��������  : ��ȡ����ATR����
�������  : ClientId:�û�ID
            OpId    :Op��ID
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2013��08��22��
  ��    ��  : g47350
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_GetCardATRReq(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId)
{
    SI_PIH_MSG_HEADER_STRU             *pstMsg;
    VOS_UINT32                          ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_CchoSetReq:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    /* ������Ϣ�ڴ� */
    pstMsg  = (SI_PIH_MSG_HEADER_STRU *)VOS_AllocMsg(WUEPS_PID_AT,
                                                   sizeof(SI_PIH_MSG_HEADER_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_WARNING_LOG("SI_PIH_CglaSetReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstMsg->ulReceiverPid   =   ulReceiverPid;
    pstMsg->ulMsgName       =   SI_PIH_CARD_ATR_QRY_REQ;
    pstMsg->usClient        =   ClientId;
    pstMsg->ucOpID          =   OpId;
    pstMsg->ulEventType     =   SI_PIH_EVENT_CARD_ATR_QRY_CNF;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, pstMsg))
    {
        PIH_WARNING_LOG("SI_PIH_CglaSetReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_HvSstSet
��������  : FDNȥ����
�������  : ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2013��03��18��
  ��    ��  : zhuli
  �޸�����  : Create
2.��    ��  : 2014��10��9��
  ��    ��  : zhuli
  �޸�����  : �������ɲ�ƷҪ�󣬸ýӿڲ��ܺ����
*****************************************************************************/

VOS_UINT32 SI_PIH_HvSstSet (
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    SI_PIH_HVSST_SET_STRU              *pstHvSStSet)
{
    SI_PIH_HVSST_REQ_STRU   *pMsg;
    VOS_UINT32               ulReceiverPid;

    if(VOS_NULL_PTR == pstHvSStSet)
    {
        PIH_WARNING_LOG("SI_PIH_HvSstSet:Parameter is Wrong");

        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_HvSstSet:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    pMsg = (SI_PIH_HVSST_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_HVSST_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        PIH_WARNING_LOG("SI_PIH_HvSstSet:WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pMsg->stMsgHeader.ulReceiverPid = ulReceiverPid;
    pMsg->stMsgHeader.usClient      = ClientId;
    pMsg->stMsgHeader.ucOpID        = OpId;
    pMsg->stMsgHeader.ulMsgName     = SI_PIH_HVSST_SET_REQ;
    pMsg->stMsgHeader.ulEventType   = SI_PIH_EVENT_HVSST_SET_CNF;

    PAM_MEM_CPY_S(&pMsg->stHvSSTData, sizeof(SI_PIH_HVSST_SET_STRU), pstHvSStSet, sizeof(SI_PIH_HVSST_SET_STRU));

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PIH_WARNING_LOG("SI_PIH_HvSstSet:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_HvSstQuery
��������  : HVSST�����ѯ����
�������  : ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2013��3��18��
  ��    ��  : zhuli
  �޸�����  : Create
2.��    ��  : 2014��10��9��
  ��    ��  : zhuli
  �޸�����  : �������ɲ�ƷҪ�󣬸ýӿڲ��ܺ����
*****************************************************************************/
VOS_UINT32 SI_PIH_HvSstQuery(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId)
{
    SI_PIH_MSG_HEADER_STRU *pMsg;
    VOS_UINT32              ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_HvSstQuery:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    pMsg = (SI_PIH_MSG_HEADER_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_MSG_HEADER_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        PIH_WARNING_LOG("SI_PIH_HvSstQuery:WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pMsg->ulReceiverPid = ulReceiverPid;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;
    pMsg->ulMsgName     = SI_PIH_HVSST_QUERY_REQ;
    pMsg->ulEventType   = SI_PIH_EVENT_HVSST_QUERY_CNF;

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PIH_WARNING_LOG("SI_PIH_HvSstQuery:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_SciCfgSet
��������  : SCICFG Set
�������  : ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2014��10��9��
  ��    ��  : zhuli
  �޸�����  : �������ɲ�ƷҪ������
*****************************************************************************/

VOS_UINT32 SI_PIH_SciCfgSet (
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    SI_PIH_CARD_SLOT_ENUM_UINT32        enCard0Slot,
    SI_PIH_CARD_SLOT_ENUM_UINT32        enCard1Slot,
    SI_PIH_CARD_SLOT_ENUM_UINT32        enCard2Slot
)
{
    SI_PIH_SCICFG_SET_REQ_STRU  *pstMsg;
    VOS_UINT32                  ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_SciCfgSet:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    pstMsg = (SI_PIH_SCICFG_SET_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_SCICFG_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_WARNING_LOG("SI_PIH_SciCfgSet:WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstMsg->stMsgHeader.ulReceiverPid   = ulReceiverPid;
    pstMsg->stMsgHeader.usClient        = ClientId;
    pstMsg->stMsgHeader.ucOpID          = OpId;
    pstMsg->stMsgHeader.ulMsgName       = SI_PIH_SCICFG_SET_REQ;
    pstMsg->stMsgHeader.ulEventType     = SI_PIH_EVENT_SCICFG_SET_CNF;

    pstMsg->enCard0Slot                 = enCard0Slot;
    pstMsg->enCard1Slot                 = enCard1Slot;
    pstMsg->enCard2Slot                 = enCard2Slot;

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pstMsg))
    {
        PIH_WARNING_LOG("SI_PIH_SciCfgSet:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_SciCfgQuery
��������  : SCICFG Query
�������  : ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2014��10��9��
  ��    ��  : zhuli
  �޸�����  : �������ɲ�ƷҪ������
*****************************************************************************/

VOS_UINT32 SI_PIH_SciCfgQuery (
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId)
{
    SI_PIH_MSG_HEADER_STRU *pstMsg;
    VOS_UINT32              ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_SciCfgQuery:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    pstMsg = (SI_PIH_MSG_HEADER_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_MSG_HEADER_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_WARNING_LOG("SI_PIH_SciCfgQuery:WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstMsg->ulReceiverPid = ulReceiverPid;
    pstMsg->usClient      = ClientId;
    pstMsg->ucOpID        = OpId;
    pstMsg->ulMsgName     = SI_PIH_SCICFG_QUERY_REQ;
    pstMsg->ulEventType   = SI_PIH_EVENT_SCICFG_QUERY_CNF;

    if(VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pstMsg))
    {
        PIH_WARNING_LOG("SI_PIH_SciCfgQuery:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_AcpuInit
��������  : HVTEE Set
�������  : ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2014��10��9��
  ��    ��  : zhuli
  �޸�����  : �������ɲ�ƷҪ������
*****************************************************************************/
VOS_VOID SI_PIH_AcpuInit(VOS_VOID)
{
    VOS_UINT8    aucUUID[] = {0x47,0x91,0xe8,0xab,
                                0x61,0xcd,
                                0x3f,0xf4,
                                0x71,0xc4,0x1a,0x31,0x7e,0x40,0x53,0x12};

    if (VOS_OK != TC_NS_RegisterServiceCallbackFunc((VOS_CHAR*)aucUUID,
                                                    SI_PIH_TEETimeOutCB,
                                                    VOS_NULL_PTR))
    {
        vos_printf("SI_PIH_AcpuInit: Reg TEE Timeout CB FUN Fail\r\n");
    }

    vos_printf("SI_PIH_AcpuInit: Reg TEE Timeout CB FUN\r\n");

    return;
}

/*****************************************************************************
�� �� ��  : SI_PIH_HvteeSet
��������  : HVTEE Set
�������  : ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2014��10��9��
  ��    ��  : zhuli
  �޸�����  : �������ɲ�ƷҪ������
*****************************************************************************/

VOS_UINT32 SI_PIH_HvteeSet(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    SI_PIH_HVTEE_SET_STRU               *pstHvtee)
{
    SI_PIH_HVTEE_SET_REQ_STRU           *pstMsg;
    VOS_UINT32                          ulReceiverPid;

    if (VOS_NULL_PTR == pstHvtee)
    {
        PIH_ERROR_LOG("SI_PIH_HvteeSet: Input Para Error.");

        return TAF_FAILURE;
    }

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_HvteeSet:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    pstMsg = (SI_PIH_HVTEE_SET_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_HVTEE_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_WARNING_LOG("SI_PIH_HvteeSet: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstMsg->stMsgHeader.ulReceiverPid = ulReceiverPid;
    pstMsg->stMsgHeader.ulMsgName     = SI_PIH_HVTEE_SET_REQ;
    pstMsg->stMsgHeader.usClient      = ClientId;
    pstMsg->stMsgHeader.ucOpID        = OpId;
    pstMsg->stMsgHeader.ulEventType   = SI_PIH_EVENT_HVTEE_SET_CNF;

    PAM_MEM_CPY_S(&pstMsg->stHvtee, sizeof(SI_PIH_HVTEE_SET_STRU), pstHvtee, sizeof(SI_PIH_HVTEE_SET_STRU));

    if(VOS_OK != VOS_SendMsg(WUEPS_PID_AT, pstMsg))
    {
        PIH_WARNING_LOG("SI_PIH_HvsContQuery:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_TEETimeOutCB
��������  : HVTEE Set
�������  : ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2014��10��9��
  ��    ��  : zhuli
  �޸�����  : �������ɲ�ƷҪ������
*****************************************************************************/

VOS_VOID SI_PIH_TEETimeOutCB (
    TEEC_TIMER_PROPERTY_STRU            *pstTimerData
)
{
    MN_APP_PIH_AT_CNF_STRU *pstMsg;

    pstMsg = (MN_APP_PIH_AT_CNF_STRU*)VOS_AllocMsg(MAPS_PIH_PID,
                                                sizeof(MN_APP_PIH_AT_CNF_STRU)-VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_ERROR_LOG("SI_PIH_TEETimeOutCB: Alloc Msg Failed!");

        return ;
    }

    pstMsg->stPIHAtEvent.EventType                  = SI_PIH_EVENT_TEETIMEOUT_IND;

    pstMsg->stPIHAtEvent.PIHError                   = TAF_ERR_NO_ERROR;

    pstMsg->stPIHAtEvent.PIHEvent.TEETimeOut.ulData = pstTimerData->time_type;

    pstMsg->ulReceiverPid                           = WUEPS_PID_AT;

    pstMsg->ulMsgId                                 = PIH_AT_EVENT_CNF;

    pstMsg->stPIHAtEvent.ClientId                   = (MN_CLIENT_ALL&AT_BROADCAST_CLIENT_ID_MODEM_0);

    (VOS_VOID)VOS_SendMsg(MAPS_PIH_PID, pstMsg);

    return ;
}

/*****************************************************************************
�� �� ��  : SI_PIH_HvCheckCardQuery
��������  : ���ݲ�ѯ����
�������  : ��
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2013��3��26��
  ��    ��  : g43750
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_HvCheckCardQuery(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId)
{
    SI_PIH_HVCHECKCARD_REQ_STRU         *pstMsg;
    VOS_UINT32                          ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_HvCheckCardQuery:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    pstMsg = (SI_PIH_HVCHECKCARD_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_HVCHECKCARD_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_WARNING_LOG("SI_PIH_HvCheckCardQuery: AllocMsg FAILED");
        return TAF_FAILURE;
    }

    pstMsg->stMsgHeader.ulReceiverPid = ulReceiverPid;
    pstMsg->stMsgHeader.ulMsgName     = SI_PIH_HVCHECKCARD_REQ;
    pstMsg->stMsgHeader.usClient      = ClientId;
    pstMsg->stMsgHeader.ucOpID        = OpId;
    pstMsg->stMsgHeader.ulEventType   = SI_PIH_EVENT_HVCHECKCARD_CNF;

    if(VOS_OK != VOS_SendMsg(WUEPS_PID_AT, pstMsg))
    {
        PIH_WARNING_LOG("SI_PIH_HvCheckCardQuery:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}


/*****************************************************************************
�� �� ��  : SI_PIH_UiccAuthReq
��������  : GBA��Ȩ������
�������  : ClientId:�û�ID
            OpId:OP id
            pstData:��Ȩ����
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2014��3��21��
  ��    ��  : zhuli
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_UiccAuthReq(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    SI_PIH_UICCAUTH_STRU                *pstData)
{
    SI_PIH_UICCAUTH_REQ_STRU        *pstMsg;
    VOS_UINT32                      ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_UiccAuthReq:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    if (VOS_NULL_PTR == pstData)
    {
        PIH_WARNING_LOG("SI_PIH_UiccAuthReq:Para Check Error");

        return TAF_FAILURE;
    }

    /* ������Ϣ�ڴ� */
    pstMsg  = (SI_PIH_UICCAUTH_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_UICCAUTH_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_WARNING_LOG("SI_PIH_UiccAuthReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstMsg->stMsgHeader.ulReceiverPid   =   ulReceiverPid;
    pstMsg->stMsgHeader.ulMsgName       =   SI_PIH_UICCAUTH_REQ;
    pstMsg->stMsgHeader.usClient        =   ClientId;
    pstMsg->stMsgHeader.ucOpID          =   OpId;
    pstMsg->stMsgHeader.ulEventType     =   SI_PIH_EVENT_UICCAUTH_CNF;

    PAM_MEM_CPY_S(&pstMsg->stAuthData, sizeof(SI_PIH_UICCAUTH_STRU), pstData, sizeof(SI_PIH_UICCAUTH_STRU));

    if (VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pstMsg))
    {
        PIH_WARNING_LOG("SI_PIH_UiccAuthReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_AccessUICCFileReq
��������  :
�������  : ClientId:�û�ID
            OpId:OP id
            pstData:��Ȩ����
�������  : ��
�� �� ֵ  : VOS_UINT32 ����ִ�н��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2014��3��21��
  ��    ��  : zhuli
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_AccessUICCFileReq(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    SI_PIH_ACCESSFILE_STRU              *pstData)
{
    SI_PIH_ACCESSFILE_REQ_STRU          *pstMsg;
    VOS_UINT32                          ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_AccessUICCFileReq:Get ulReceiverPid Error.");

        return TAF_FAILURE;
    }

    if (VOS_NULL_PTR == pstData)
    {
        PIH_WARNING_LOG("SI_PIH_AccessUICCFileReq:Para Check Error");

        return TAF_FAILURE;
    }

    /* ������Ϣ�ڴ� */
    pstMsg  = (SI_PIH_ACCESSFILE_REQ_STRU *)VOS_AllocMsg(WUEPS_PID_AT,
                    sizeof(SI_PIH_ACCESSFILE_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_WARNING_LOG("SI_PIH_AccessUICCFileReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstMsg->stMsgHeader.ulReceiverPid   =   ulReceiverPid;
    pstMsg->stMsgHeader.ulMsgName       =   SI_PIH_URSM_REQ;
    pstMsg->stMsgHeader.usClient        =   ClientId;
    pstMsg->stMsgHeader.ucOpID          =   OpId;
    pstMsg->stMsgHeader.ulEventType     =   SI_PIH_EVENT_URSM_CNF;

    PAM_MEM_CPY_S(&pstMsg->stCmdData, sizeof(SI_PIH_ACCESSFILE_STRU), pstData, sizeof(SI_PIH_ACCESSFILE_STRU));

    if (VOS_OK !=  VOS_SendMsg(WUEPS_PID_AT, pstMsg))
    {
        PIH_WARNING_LOG("SI_PIH_AccessUICCFileReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_CardTypeQuery
��������  :
�������  : ClientId:�û�ID
            OpId:OP id
�������  : ��
�� �� ֵ  : ��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2014��06��04��
  ��    ��  : w00180399
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_CardTypeQuery(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId)
{
    SI_PIH_MSG_HEADER_STRU *pMsg;
    VOS_UINT32              ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_CardTypeQuery: Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    pMsg = (SI_PIH_MSG_HEADER_STRU *)VOS_AllocMsg(WUEPS_PID_AT,
                        sizeof(SI_PIH_MSG_HEADER_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        PIH_WARNING_LOG("SI_PIH_CardTypeQuery: WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pMsg->ulReceiverPid = ulReceiverPid;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;
    pMsg->ulMsgName     = SI_PIH_CARDTYPE_QUERY_REQ;
    pMsg->ulEventType   = SI_PIH_EVENT_CARDTYPE_QUERY_CNF;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PIH_WARNING_LOG("SI_PIH_CardTypeQuery: WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_CrsmSetReq
��������  :
�������  : ClientId:�û�ID
            OpId:OP id
�������  : ��
�� �� ֵ  : ��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2014��06��04��
  ��    ��  : w00180399
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_CrsmSetReq(
    MN_CLIENT_ID_T                          ClientId,
    MN_OPERATION_ID_T                       OpId,
    SI_PIH_CRSM_STRU                       *pstCrsmPara
)
{
    SI_PIH_CRSM_SET_REQ_STRU           *pstCrsmMsg;
    VOS_UINT32                          ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_CrsmSetReq:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    /* ������Ϣ�ڴ� */
    pstCrsmMsg  = (SI_PIH_CRSM_SET_REQ_STRU*)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_CRSM_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstCrsmMsg)
    {
        PIH_WARNING_LOG("SI_PIH_CrsmSetReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstCrsmMsg->stMsgHeader.ulReceiverPid   =   ulReceiverPid;
    pstCrsmMsg->stMsgHeader.ulMsgName       =   SI_PIH_CRSM_SET_REQ;
    pstCrsmMsg->stMsgHeader.usClient        =   ClientId;
    pstCrsmMsg->stMsgHeader.ucOpID          =   OpId;
    pstCrsmMsg->stMsgHeader.ulEventType     =   SI_PIH_EVENT_CRSM_SET_CNF;

    PAM_MEM_CPY_S(&(pstCrsmMsg->stMsgContent), sizeof(SI_PIH_CRSM_STRU), pstCrsmPara, sizeof(SI_PIH_CRSM_STRU));

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, pstCrsmMsg))
    {
        PIH_WARNING_LOG("SI_PIH_CrsmSetReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_CrlaSetReq
��������  :
�������  : ClientId:�û�ID
            OpId:OP id
�������  : ��
�� �� ֵ  : ��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2014��06��04��
  ��    ��  : w00180399
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_CrlaSetReq(
    MN_CLIENT_ID_T                          ClientId,
    MN_OPERATION_ID_T                       OpId,
    SI_PIH_CRLA_STRU                       *pstCrlaPara
)
{
    SI_PIH_CRLA_SET_REQ_STRU           *pstCrlaMsg;
    VOS_UINT32                          ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_CrlaSetReq:Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    /* ������Ϣ�ڴ� */
    pstCrlaMsg  = (SI_PIH_CRLA_SET_REQ_STRU*)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_CRLA_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstCrlaMsg)
    {
        PIH_WARNING_LOG("SI_PIH_CrlaSetReq: AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstCrlaMsg->stMsgHeader.ulReceiverPid   =   ulReceiverPid;
    pstCrlaMsg->stMsgHeader.ulMsgName       =   SI_PIH_CRLA_SET_REQ;
    pstCrlaMsg->stMsgHeader.usClient        =   ClientId;
    pstCrlaMsg->stMsgHeader.ucOpID          =   OpId;
    pstCrlaMsg->stMsgHeader.ulEventType     =   SI_PIH_EVENT_CRLA_SET_CNF;

    PAM_MEM_CPY_S(&(pstCrlaMsg->stMsgContent), sizeof(SI_PIH_CRLA_STRU), pstCrlaPara, sizeof(SI_PIH_CRLA_STRU));

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, pstCrlaMsg))
    {
        PIH_WARNING_LOG("SI_PIH_CchcSetReq:WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_CardSessionQuery
��������  :
�������  : ClientId:�û�ID
            OpId:OP id
�������  : ��
�� �� ֵ  : ��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2014��06��04��
  ��    ��  : w00180399
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_CardSessionQuery(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
)
{
    SI_PIH_MSG_HEADER_STRU             *pMsg;
    VOS_UINT32                          ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_CardSessionQuery: Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    pMsg = (SI_PIH_MSG_HEADER_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_MSG_HEADER_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        PIH_WARNING_LOG("SI_PIH_CardSessionQuery: WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pMsg->ulReceiverPid = ulReceiverPid;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;
    pMsg->ulMsgName     = SI_PIH_SESSION_QRY_REQ;
    pMsg->ulEventType   = SI_PIH_EVENT_SESSION_QRY_CNF;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PIH_WARNING_LOG("SI_PIH_CardSessionQuery: WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_CimiSetReq
��������  :
�������  : ClientId:�û�ID
            OpId:OP id
�������  : ��
�� �� ֵ  : ��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2014��06��04��
  ��    ��  : w00180399
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_CimiSetReq(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
)
{
    SI_PIH_MSG_HEADER_STRU             *pstMsg;
    VOS_UINT32                          ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_CardSessionQuery: Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    pstMsg = (SI_PIH_MSG_HEADER_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_MSG_HEADER_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        PIH_WARNING_LOG("SI_PIH_CimiSetReq: WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pstMsg->ulReceiverPid = ulReceiverPid;
    pstMsg->usClient      = ClientId;
    pstMsg->ucOpID        = OpId;
    pstMsg->ulMsgName     = SI_PIH_CIMI_QRY_REQ;
    pstMsg->ulEventType   = SI_PIH_EVENT_CIMI_QRY_CNF;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, pstMsg))
    {
        PIH_WARNING_LOG("SI_PIH_CimiSetReq: WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}

/*****************************************************************************
�� �� ��  : SI_PIH_CCimiSetReq
��������  :
�������  : ClientId:�û�ID
            OpId:OP id
�������  : ��
�� �� ֵ  : ��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 2014��06��04��
  ��    ��  : w00180399
  �޸�����  : Create
*****************************************************************************/
VOS_UINT32 SI_PIH_CCimiSetReq(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
)
{
    SI_PIH_MSG_HEADER_STRU             *pMsg;
    VOS_UINT32                          ulReceiverPid;

    if (VOS_OK != SI_PIH_GetReceiverPid(ClientId, &ulReceiverPid))
    {
        PIH_ERROR_LOG("SI_PIH_CardSessionQuery: Get ulReceiverPid Error.");
        return TAF_FAILURE;
    }

    pMsg = (SI_PIH_MSG_HEADER_STRU *)VOS_AllocMsg(WUEPS_PID_AT, sizeof(SI_PIH_MSG_HEADER_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pMsg)
    {
        PIH_WARNING_LOG("SI_PIH_CardSessionQuery: WARNING AllocMsg FAILED");

        return TAF_FAILURE;
    }

    pMsg->ulReceiverPid = ulReceiverPid;
    pMsg->usClient      = ClientId;
    pMsg->ucOpID        = OpId;
    pMsg->ulMsgName     = SI_PIH_CCIMI_QRY_REQ;
    pMsg->ulEventType   = SI_PIH_EVENT_CCIMI_QRY_CNF;

    if (VOS_OK != VOS_SendMsg(WUEPS_PID_AT, pMsg))
    {
        PIH_WARNING_LOG("SI_PIH_CardSessionQuery: WARNING SendMsg FAILED");

        return TAF_FAILURE;
    }

    return TAF_SUCCESS;
}







