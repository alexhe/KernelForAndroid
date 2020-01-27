/*
* Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
* foss@huawei.com
*
* If distributed as part of the Linux kernel, the following license terms
* apply:
*
* * This program is free software; you can redistribute it and/or modify
* * it under the terms of the GNU General Public License version 2 and
* * only version 2 as published by the Free Software Foundation.
* *
* * This program is distributed in the hope that it will be useful,
* * but WITHOUT ANY WARRANTY; without even the implied warranty of
* * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* * GNU General Public License for more details.
* *
* * You should have received a copy of the GNU General Public License
* * along with this program; if not, write to the Free Software
* * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
*
* Otherwise, the following license terms apply:
*
* * Redistribution and use in source and binary forms, with or without
* * modification, are permitted provided that the following conditions
* * are met:
* * 1) Redistributions of source code must retain the above copyright
* *    notice, this list of conditions and the following disclaimer.
* * 2) Redistributions in binary form must reproduce the above copyright
* *    notice, this list of conditions and the following disclaimer in the
* *    documentation and/or other materials provided with the distribution.
* * 3) Neither the name of Huawei nor the names of its contributors may
* *    be used to endorse or promote products derived from this software
* *    without specific prior written permission.
*
* * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*/

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#ifndef RF0_TYPE_ASIC_HI6365
#define RF0_TYPE_ASIC_HI6365
#endif 
#include "AtMtInterface.h"
#include "AtTypeDef.h"
#include "MnClient.h"
#include "ATCmdProc.h" 
#include "AtMtMsgProc.h"
#include "AtMtCommFun.h"



/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_AT_MT_MSG_PROC_C


/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
#if(FEATURE_ON == FEATURE_UE_MODE_NR)
const AT_PROC_BBIC_MSG_STRU g_astAtProcBbicMsgTab[]=
{
    {ID_BBIC_AT_CAL_MSG_CNF               , AT_ProcTxRxCnf},
    {ID_BBIC_AT_CAL_RF_DEBUG_TX_RESULT_IND, AT_ProcPowerDetCnf},
    {ID_BBIC_AT_MIPI_READ_CNF             , AT_RcvBbicCalMipiRedCnf},
    {ID_BBIC_AT_MIPI_WRITE_CNF            , AT_RcvBbicCalMipiWriteCnf},
    {ID_BBIC_AT_PLL_QRY_CNF               , At_RcvBbicPllStatusCnf},
    {ID_BBIC_AT_CAL_RF_DEBUG_RX_RSSI_IND  , At_RcvBbicRssiInd},
    {ID_BBIC_AT_DPDT_CNF                  , AT_RcvBbicCalSetDpdtCnf},
    {ID_BBIC_AT_TEMP_QRY_CNF              , AT_RcvBbicCalQryFtemprptCnf},
    {ID_BBIC_AT_DCXO_CNF                  , At_RcvBbicCalDcxoCnf},
};


const AT_PROC_CBT_MSG_STRU g_astAtProcCbtMsgTab[]=
{
    {ID_CCBT_AT_SET_WORK_MODE_CNF, AT_ProcSetWorkModeCnf},
};
#endif

#if(FEATURE_ON == FEATURE_UE_MODE_NR)
extern AT_MT_INFO_STRU                         g_stMtInfoCtx;
#else
extern AT_DEVICE_CMD_CTRL_STRU                 g_stAtDevCmdCtrl;
#endif

#if(FEATURE_OFF == FEATURE_UE_MODE_NR)
/*****************************************************************************
  3 �ɺ���ʵ��
*****************************************************************************/


VOS_UINT32  At_SendContinuesWaveOnToHPA(
    VOS_UINT16                          usPower,
    VOS_UINT8                           ucIndex
)
{
    AT_HPA_RF_CFG_REQ_STRU              *pstMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usMask;
    VOS_UINT8                           ucCtrlType;

    ucCtrlType = WDSP_CTRL_TX_ONE_TONE;

    /* �򿪵��� */
    if (AT_DSP_CLT_ENABLE == g_stAtDevCmdCtrl.enCltEnableFlg)
    {
        ucCtrlType = WDSP_CTRL_TX_THREE_TONE;
    }

    /* ����AT_HPA_RF_CFG_REQ_STRU��Ϣ */
    ulLength = sizeof(AT_HPA_RF_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg = (AT_HPA_RF_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_SendContinuesWaveOnToHPA: alloc msg fail!");
        return AT_FAILURE;
    }

    TAF_MEM_SET_S(pstMsg, sizeof(AT_HPA_RF_CFG_REQ_STRU), 0x00, sizeof(AT_HPA_RF_CFG_REQ_STRU));

    /* ��д��Ϣͷ */
    pstMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = AT_GetDestPid(ucIndex, I0_DSP_PID_WPHY);
    pstMsg->ulLength        = ulLength;

    /* ��д��Ϣ�� */
    pstMsg->usMsgID = ID_AT_HPA_RF_CFG_REQ;

    /* Tx Cfg */
    usMask =  W_RF_MASK_TX_TXONOFF | W_RF_MASK_TX_POWER;
    pstMsg->stRfCfgPara.sTxPower = (VOS_INT16)usPower;

    /* ��λ��ʶ�������� */
    pstMsg->stRfCfgPara.usMask      = usMask;
    pstMsg->stRfCfgPara.usTxAFCInit = W_AFC_INIT_VALUE;

    /* �򿪵����ź� */
    pstMsg->stRfCfgPara.usTxEnable = ucCtrlType;

    if ( VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_WARN_LOG("At_SendContinuesWaveOnToHPA: Send msg fail!");
        return AT_FAILURE;
    }
    return AT_SUCCESS;
}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_UINT32  At_SendContinuesWaveOnToCHPA(
    VOS_UINT8                           ucCtrlType,
    VOS_UINT16                          usPower
)
{
    AT_CHPA_RF_CFG_REQ_STRU            *pstMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usMask;

    /* ����AT_HPA_RF_CFG_REQ_STRU��Ϣ */
    ulLength = sizeof(AT_CHPA_RF_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg = (AT_CHPA_RF_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_SendContinuesWaveOnToCHPA: alloc msg fail!");
        return AT_FAILURE;
    }

    TAF_MEM_SET_S(pstMsg, sizeof(AT_CHPA_RF_CFG_REQ_STRU), 0x00 ,sizeof(AT_CHPA_RF_CFG_REQ_STRU));

    /* ��д��Ϣͷ */
    pstMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = UPHY_PID_CSDR_1X_CM;
    pstMsg->ulLength        = ulLength;

    /* ��д��Ϣ�� */
    pstMsg->usMsgID = ID_AT_CHPA_RF_CFG_REQ;

    /* Tx Cfg */
    usMask =  W_RF_MASK_TX_TXONOFF | W_RF_MASK_TX_POWER;
    pstMsg->stRfCfgPara.sTxPower = (VOS_INT16)usPower;

    /* ��λ��ʶ�������� */
    pstMsg->stRfCfgPara.usMask      = usMask;
    pstMsg->stRfCfgPara.usTxAFCInit = W_AFC_INIT_VALUE;

    /* �򿪵����ź� */
    pstMsg->stRfCfgPara.usTxEnable = ucCtrlType;

    if ( VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_WARN_LOG("At_SendContinuesWaveOnToCHPA: Send msg fail!");
        return AT_FAILURE;
    }
    return AT_SUCCESS;
}


VOS_UINT32  At_SendTxOnOffToCHPA(VOS_UINT8 ucSwitch)
{
    AT_CHPA_RF_CFG_REQ_STRU            *pstMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usMask;

    /* ����AT_HPA_RF_CFG_REQ_STRU��Ϣ */
    ulLength = sizeof(AT_CHPA_RF_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg = (AT_CHPA_RF_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_SendTxOnOffToCHPA: alloc msg fail!");
        return AT_FAILURE;
    }

    TAF_MEM_SET_S(pstMsg, sizeof(AT_CHPA_RF_CFG_REQ_STRU), 0x00, sizeof(AT_CHPA_RF_CFG_REQ_STRU));

    /* ��д��Ϣͷ */
    pstMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = UPHY_PID_CSDR_1X_CM;
    pstMsg->ulLength        = ulLength;

    /* ��д��Ϣ�� */
    pstMsg->usMsgID = ID_AT_CHPA_RF_CFG_REQ;

    /* Tx Cfg */
    usMask =  W_RF_MASK_AFC | W_RF_MASK_TX_ARFCN \
             | W_RF_MASK_TX_TXONOFF | W_RF_MASK_TX_PAMODE |W_RF_MASK_TX_POWDET;

    usMask                       = usMask | W_RF_MASK_TX_POWER;
    pstMsg->stRfCfgPara.sTxPower = (VOS_INT16)g_stAtDevCmdCtrl.usPower;

    /* Tx Cfg */
    pstMsg->stRfCfgPara.usMask      = usMask;                             /* ��λ��ʶ�������� */
    pstMsg->stRfCfgPara.usTxAFCInit = W_AFC_INIT_VALUE;              /* AFC */
    pstMsg->stRfCfgPara.usTxBand    = 0;                             /* g_stAtDevCmdCtrl.stDspBandArfcn.usDspBand;     1,2,3...,Э���е�band���,ע�ⲻ��BandId */
    pstMsg->stRfCfgPara.usTxFreqNum = g_stAtDevCmdCtrl.stDspBandArfcn.usUlArfcn; /* Arfcn */
    pstMsg->stRfCfgPara.usTxPAMode  = g_stAtDevCmdCtrl.ucPaLevel;

    if (AT_DSP_RF_SWITCH_ON == ucSwitch)
    {
        pstMsg->stRfCfgPara.usTxEnable = WDSP_CTRL_TX_ON;               /* �򿪷���TX */
    }
    else
    {
        pstMsg->stRfCfgPara.usTxEnable = WDSP_CTRL_TX_OFF;               /* �رշ���TX */
    }

    if ( VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_WARN_LOG("At_SendTxOnOffToCHPA: Send msg fail!");
        return AT_FAILURE;
    }
    return AT_SUCCESS;
}


VOS_UINT32 At_SendRxOnOffToCHPA(
    VOS_UINT32                          ulRxSwitch
)
{
    AT_CHPA_RF_CFG_REQ_STRU            *pstMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usMask;

    /* ����AT_HPA_RF_CFG_REQ_STRU��Ϣ */
    ulLength = sizeof(AT_CHPA_RF_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg   = (AT_CHPA_RF_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_SendRxOnOffToCHPA: alloc msg fail!");
        return AT_FAILURE;
    }

    TAF_MEM_SET_S(pstMsg, sizeof(AT_CHPA_RF_CFG_REQ_STRU), 0x00, sizeof(AT_CHPA_RF_CFG_REQ_STRU));

    /* ��д��Ϣͷ */
    pstMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = UPHY_PID_CSDR_1X_CM;
    pstMsg->ulLength        = ulLength;

    /* ��д��Ϣ�� */
    pstMsg->usMsgID = ID_AT_CHPA_RF_CFG_REQ;

    usMask = W_RF_MASK_RX_ARFCN | W_RF_MASK_RX_ANTSEL \
             | W_RF_MASK_RX_RXONOFF;

    /* ��λ��ʶ�������� */
    pstMsg->stRfCfgPara.usMask      = usMask;

    /* 1,2,3...,Э���е�band��� */
    pstMsg->stRfCfgPara.usRxBand    = 0;  /* g_stAtDevCmdCtrl.stDspBandArfcn.usDspBand;*/
    pstMsg->stRfCfgPara.usRxFreqNum = g_stAtDevCmdCtrl.stDspBandArfcn.usUlArfcn;

    pstMsg->stRfCfgPara.usRxAntSel = ANT_ONE;

    if (AT_DSP_RF_SWITCH_ON == ulRxSwitch)
    {      
        /* CPU���ƴ���ƵоƬ,����1 */
        pstMsg->stRfCfgPara.usRxEnable = DSP_CTRL_RX_ANT1_ON;
    }
    else
    {
        /* �رս���RX */
        pstMsg->stRfCfgPara.usRxEnable = DSP_CTRL_RX_OFF;
    }

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_WARN_LOG("At_SendRxOnOffToCHPA: Send msg fail!");
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}

#endif


TAF_UINT32  At_SendTxOnOffToHPA(
    TAF_UINT8                           ucSwitch,
    TAF_UINT8                           ucIndex
)
{
    AT_HPA_RF_CFG_REQ_STRU             *pstMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usMask;

    /* ����AT_HPA_RF_CFG_REQ_STRU��Ϣ */
    ulLength    = sizeof(AT_HPA_RF_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (AT_HPA_RF_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);
    usMask      = 0;

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_SendTxOnOffToHPA: alloc msg fail!");
        return AT_FAILURE;
    }

    TAF_MEM_SET_S(pstMsg, sizeof(AT_HPA_RF_CFG_REQ_STRU), 0x00, sizeof(AT_HPA_RF_CFG_REQ_STRU));

    /* ��д��Ϣͷ */
    pstMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = AT_GetDestPid(ucIndex, I0_DSP_PID_WPHY);
    pstMsg->ulLength        = ulLength;

    /* ��д��Ϣ�� */
    pstMsg->usMsgID = ID_AT_HPA_RF_CFG_REQ;

    /* Tx Cfg */
    usMask = W_RF_MASK_AFC | W_RF_MASK_TX_TXONOFF | W_RF_MASK_TX_PAMODE | W_RF_MASK_TX_POWDET;

    /*AT^FDAC���õ�FDACֵ��AT^FWAVE���õ�powerֵ��ʾ�ĺ�����ͬ��ȡ�����õ�ֵ*/
    if ( VOS_TRUE == g_stAtDevCmdCtrl.bFdacFlag )
    {
        usMask                      = usMask | W_RF_MASK_TX_AGC;
        pstMsg->stRfCfgPara.usTxAGC = g_stAtDevCmdCtrl.usFDAC;
    }
    else
    {
        usMask                       = usMask | W_RF_MASK_TX_POWER;
        pstMsg->stRfCfgPara.sTxPower = (VOS_INT16)g_stAtDevCmdCtrl.usPower;
    }

    /* Tx Cfg */
    pstMsg->stRfCfgPara.usMask      = usMask;                             /* ��λ��ʶ�������� */
    pstMsg->stRfCfgPara.usTxAFCInit = W_AFC_INIT_VALUE;              /* AFC */
    pstMsg->stRfCfgPara.usTxBand    = g_stAtDevCmdCtrl.stDspBandArfcn.usDspBand;    /* 1,2,3...,Э���е�band���,ע�ⲻ��BandId */
    pstMsg->stRfCfgPara.usTxFreqNum = g_stAtDevCmdCtrl.stDspBandArfcn.usUlArfcn; /* Arfcn */
    pstMsg->stRfCfgPara.usTxPAMode  = g_stAtDevCmdCtrl.ucPaLevel;

    if (AT_DSP_RF_SWITCH_OFF == ucSwitch)
    {
        pstMsg->stRfCfgPara.usTxEnable  = WDSP_CTRL_TX_OFF;                     /* �رշ���TX */
    }
    else
    {
        pstMsg->stRfCfgPara.usTxEnable  = WDSP_CTRL_TX_ON;                      /* �򿪷���TX */
        pstMsg->stRfCfgPara.usMask      |= W_RF_MASK_TX_ARFCN;

        /* ֻ����^FTXON=1 ʱ���·� */
        if (AT_DSP_CLT_ENABLE == g_stAtDevCmdCtrl.enCltEnableFlg)
        {
            pstMsg->stRfCfgPara.usMask |= W_RF_MASK_TX_CLT;
            pstMsg->stRfCfgPara.usMask &= ~(W_RF_MASK_TX_POWDET);
        }
        if ( AT_DCXOTEMPCOMP_ENABLE == g_stAtDevCmdCtrl.enDcxoTempCompEnableFlg)
        {
            pstMsg->stRfCfgPara.usMask |= W_RF_MASK_TX_DCXOTEMPCOMP;
            pstMsg->stRfCfgPara.usMask &= ~(W_RF_MASK_AFC);
        }
    }

    if ( VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_WARN_LOG("At_SendTxOnOffToHPA: Send msg fail!");
        return AT_FAILURE;
    }
    return AT_SUCCESS;
}


VOS_UINT32  At_SendTxOnOffToGHPA(
    VOS_UINT8                           ucIndex,
    VOS_UINT8                           ucSwitch
)
{
    AT_GHPA_RF_TX_CFG_REQ_STRU          *pstTxMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usMask;

    /* ����AT_GHPA_RF_TX_CFG_REQ_STRU��Ϣ */
    ulLength = sizeof(AT_GHPA_RF_TX_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    /*lint -save -e830 */
    pstTxMsg = (AT_GHPA_RF_TX_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);
    /*lint -restore */
    if (VOS_NULL_PTR == pstTxMsg)
    {
        AT_WARN_LOG("At_SendRfCfgReqToGHPA: alloc Tx msg fail!");
        return AT_FAILURE;
    }

    TAF_MEM_SET_S(pstTxMsg, sizeof(AT_GHPA_RF_TX_CFG_REQ_STRU), 0x00, sizeof(AT_GHPA_RF_TX_CFG_REQ_STRU));

    /* ��д��Ϣͷ */
    pstTxMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstTxMsg->ulSenderPid     = WUEPS_PID_AT;
    pstTxMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstTxMsg->ulReceiverPid   = AT_GetDestPid(ucIndex, I0_DSP_PID_GPHY);
    pstTxMsg->ulLength        = ulLength;

    /* Tx Cfg */

    /* ��д��Ϣ�� */
    pstTxMsg->usMsgID = ID_AT_GHPA_RF_TX_CFG_REQ;
    usMask            = G_RF_MASK_TX_AFC;

    /*AT^FDAC���õ�FDACֵ��AT^FWAVE���õ�powerֵ��ʾ�ĺ�����ͬ��ȡ�����õ�ֵ*/
    /* ������Ʒ�ʽ��
        0��GMSK��ѹ����,�˷�ʽ��usTxVpaҪ���ã�
        1�����ʿ���,�˷�ʽ��usTxPowerҪ���ã�
        2��8PSK PaVbias��ѹ&DBB Atten&RF Atten���ƣ�
        usPAVbias��usRfAtten,sDbbAtten����������Ҫ���ã�*/
    if ( VOS_TRUE == g_stAtDevCmdCtrl.bFdacFlag )
    {
        pstTxMsg->enCtrlMode = AT_GHPA_RF_CTRLMODE_TYPE_GMSK;
        pstTxMsg->usTxVpa = g_stAtDevCmdCtrl.usFDAC;  /* DAC���� */
    }
    else
    {
        pstTxMsg->enCtrlMode = AT_GHPA_RF_CTRLMODE_TYPE_TXPOWER;
        pstTxMsg->usTxPower = g_stAtDevCmdCtrl.usPower;
    }

    /* Tx Cfg */
    pstTxMsg->usMask = usMask;              /* ��λ��ʶ�������� */
    pstTxMsg->usAFC =  0;                   /* AFC */
    pstTxMsg->usFreqNum =
                (VOS_UINT16)((g_stAtDevCmdCtrl.stDspBandArfcn.usDspBand << 12) \
                            | g_stAtDevCmdCtrl.stDspBandArfcn.usUlArfcn);
                                            /* (Band << 12) | Arfcn */
    pstTxMsg->usTxMode = g_stAtDevCmdCtrl.usTxMode;    /* 0:burst����; 1:�������� */
    pstTxMsg->usModType = G_MOD_TYPE_GMSK;  /* ������Ʒ�ʽ:0��ʾGMSK����;1��ʾ8PSK���Ʒ�ʽ */

    if (AT_DSP_RF_SWITCH_ON == ucSwitch)
    {
        pstTxMsg->usTxEnable = GDSP_CTRL_TX_ON;
    }
    else
    {
        pstTxMsg->usTxEnable = GDSP_CTRL_TX_OFF;
    }

    if ( VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstTxMsg))
    {
        AT_WARN_LOG("At_SendTxOnOffToGHPA: Send msg fail!");
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}


VOS_UINT32  At_SendRxOnOffToGHPA(
    VOS_UINT8                           ucIndex,
    VOS_UINT32                          ulRxSwitch
)
{
    AT_GHPA_RF_RX_CFG_REQ_STRU          *pstRxMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usMask;

    /* ����AT_GHPA_RF_RX_CFG_REQ_STRU��Ϣ */
    ulLength = sizeof(AT_GHPA_RF_RX_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstRxMsg = (AT_GHPA_RF_RX_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstRxMsg)
    {
        AT_WARN_LOG("At_SendRxOnOffToGHPA: alloc Rx msg fail!");
        return AT_FAILURE;
    }

    /* Rx Cfg */
    pstRxMsg->ulReceiverPid   = AT_GetDestPid(ucIndex, I0_DSP_PID_GPHY);
    pstRxMsg->ulLength        = ulLength;
    pstRxMsg->usMsgID         = ID_AT_GHPA_RF_RX_CFG_REQ;
    pstRxMsg->usRsv           = 0;
    pstRxMsg->usRsv2          = 0;

    usMask = G_RF_MASK_RX_ARFCN | G_RF_MASK_RX_AGCMODE \
            | G_RF_MASK_RX_AGCGAIN | G_RF_MASK_RX_MODE;

    /* Reference MASK_CAL_RF_G_RX_* section */
    pstRxMsg->usMask = usMask;

    /* (Band << 12) | Arfcn */
    pstRxMsg->usFreqNum =
                    (VOS_UINT16)((g_stAtDevCmdCtrl.stDspBandArfcn.usDspBand << 12) \
                                | g_stAtDevCmdCtrl.stDspBandArfcn.usUlArfcn);

    /* 0:ֹͣ; 1:��������; */
    if ( TRUE == ulRxSwitch )
    {
        pstRxMsg->usRxMode  = AT_GDSP_RX_MODE_CONTINOUS_BURST;
    }
    else
    {
        pstRxMsg->usRxMode  = AT_GDSP_RX_MODE_STOP;
    }

    /* 1:Fast AGC,0:Slow AGC */
    pstRxMsg->usAGCMode = AT_GDSP_RX_SLOW_AGC_MODE;

    /* AGC��λ�����ĵ�,ȡֵΪ0-3*/
    pstRxMsg->usAgcGain = g_stAtDevCmdCtrl.ucLnaLevel;

    if ( VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstRxMsg))
    {
        AT_WARN_LOG("At_SendRxOnOffToGHPA: Send msg fail!");
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}


VOS_UINT32 At_SendRxOnOffToHPA(
    VOS_UINT32                          ulRxSwitch,
    VOS_UINT8                           ucIndex
)
{
    AT_HPA_RF_CFG_REQ_STRU              *pstMsg;
    VOS_UINT32                          ulLength;
    VOS_UINT16                          usMask;

    /* ����AT_HPA_RF_CFG_REQ_STRU��Ϣ */
    ulLength = sizeof(AT_HPA_RF_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg   = (AT_HPA_RF_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_SendRxOnOffToHPA: alloc msg fail!");
        return AT_FAILURE;
    }

    TAF_MEM_SET_S(pstMsg, sizeof(AT_HPA_RF_CFG_REQ_STRU), 0x00, sizeof(AT_HPA_RF_CFG_REQ_STRU));

    /* ��д��Ϣͷ */
    pstMsg->ulSenderCpuId   = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid     = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid   = AT_GetDestPid(ucIndex, I0_DSP_PID_WPHY);
    pstMsg->ulLength        = ulLength;

    /* ��д��Ϣ�� */
    pstMsg->usMsgID = ID_AT_HPA_RF_CFG_REQ;

    usMask = W_RF_MASK_RX_ARFCN | W_RF_MASK_RX_ANTSEL \
             | W_RF_MASK_RX_RXONOFF;

    /* ��λ��ʶ�������� */
    pstMsg->stRfCfgPara.usMask      = usMask;

    /* 1,2,3...,Э���е�band��� */
    pstMsg->stRfCfgPara.usRxBand    = g_stAtDevCmdCtrl.stDspBandArfcn.usDspBand;
    pstMsg->stRfCfgPara.usRxFreqNum = g_stAtDevCmdCtrl.stDspBandArfcn.usDlArfcn;

    /* DSPƵ��Band1-Band9��ʽת��Ϊ
       W Rf support
    -------------------------------------------------------------------------------
            bit8       bit7      bit6     bit5    bit4     bit3      bit2     bit1
    -------------------------------------------------------------------------------
    WCDMA   900(VIII)  2600(VII) 800(VI)  850(V)  1700(IV) 1800(III) 1900(II) 2100(I) oct1
            spare      spare     spare    spare   spare    spare     spare   J1700(IX)oct2
    */

    if (AT_RX_DIV_ON == g_stAtDevCmdCtrl.ucPriOrDiv)
    {
        pstMsg->stRfCfgPara.usRxAntSel = ANT_TWO;
    }
    else
    {
        pstMsg->stRfCfgPara.usRxAntSel = ANT_ONE;
    }

    /* LNAģʽ���� */
    pstMsg->stRfCfgPara.usRxLNAGainMode = g_stAtDevCmdCtrl.ucLnaLevel;

    if (AT_DSP_RF_SWITCH_ON == ulRxSwitch)
    {
        if (ANT_TWO == pstMsg->stRfCfgPara.usRxAntSel)
        {
            /* CPU���ƴ���ƵоƬ,����1��2 */
            pstMsg->stRfCfgPara.usRxEnable = DSP_CTRL_RX_ALL_ANT_ON;
        }
        else
        {
            /* CPU���ƴ���ƵоƬ,����1 */
            pstMsg->stRfCfgPara.usRxEnable = DSP_CTRL_RX_ANT1_ON;
        }
    }
    else
    {
        /* �رս���RX */
        pstMsg->stRfCfgPara.usRxEnable = DSP_CTRL_RX_OFF;
    }

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_WARN_LOG("At_SendRxOnOffToHPA: Send msg fail!");
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}


VOS_UINT32 At_SendRfCfgAntSelToHPA(
    VOS_UINT8                           ucDivOrPriOn,
    VOS_UINT8                           ucIndex
)
{
    AT_HPA_RF_CFG_REQ_STRU              *pstMsg;
    VOS_UINT32                           ulLength;
    VOS_UINT16                           usMask;

    /* ����AT_HPA_RF_CFG_REQ_STRU��Ϣ */
    ulLength = sizeof(AT_HPA_RF_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    /*lint -save -e830 */
    pstMsg   = (AT_HPA_RF_CFG_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);
    /*lint -restore */
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_WARN_LOG("At_SendRfCfgAntSelToHPA: alloc msg fail!");
        return AT_FAILURE;
    }

    TAF_MEM_SET_S(pstMsg, sizeof(AT_HPA_RF_CFG_REQ_STRU), 0x00, sizeof(AT_HPA_RF_CFG_REQ_STRU));


    /* ��д��Ϣͷ */
    pstMsg->ulSenderCpuId      = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid        = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId    = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid      = AT_GetDestPid(ucIndex, I0_DSP_PID_WPHY);
    pstMsg->ulLength           = ulLength;


    /* ��д��Ϣ�� */
    pstMsg->usMsgID            = ID_AT_HPA_RF_CFG_REQ;
    usMask                     = W_RF_MASK_RX_ARFCN | W_RF_MASK_RX_ANTSEL \
                                 | W_RF_MASK_RX_RXONOFF;

    /* ��λ��ʶ�������� */
    pstMsg->stRfCfgPara.usMask = usMask;

    if (AT_RX_DIV_ON == ucDivOrPriOn)
    {
        pstMsg->stRfCfgPara.usRxAntSel = ANT_TWO;
        pstMsg->stRfCfgPara.usRxEnable = DSP_CTRL_RX_ALL_ANT_ON;
    }
    else
    {
        pstMsg->stRfCfgPara.usRxAntSel = ANT_ONE;
        pstMsg->stRfCfgPara.usRxEnable = DSP_CTRL_RX_ANT1_ON;
    }

    pstMsg->stRfCfgPara.usRxBand       = g_stAtDevCmdCtrl.stDspBandArfcn.usDspBand;
    pstMsg->stRfCfgPara.usRxFreqNum    = g_stAtDevCmdCtrl.stDspBandArfcn.usDlArfcn;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_WARN_LOG("At_SendRfCfgAntSelToHPA: Send msg fail!");
        return AT_FAILURE;
    }

    return AT_SUCCESS;
}


VOS_UINT32 AT_RcvDrvAgentSetFchanRsp(VOS_VOID *pMsg)
{
    DRV_AGENT_MSG_STRU                 *pRcvMsg;
    DRV_AGENT_FCHAN_SET_CNF_STRU       *pstFchanSetCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulError;

    /* ��ʼ�� */
    pRcvMsg          = (DRV_AGENT_MSG_STRU *)pMsg;
    pstFchanSetCnf   = (DRV_AGENT_FCHAN_SET_CNF_STRU *)pRcvMsg->aucContent;

    /* ͨ��clientid��ȡindex */
    if (AT_FAILURE == At_ClientIdToUserId(pstFchanSetCnf->stAtAppCtrl.usClientId,&ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetFchanRsp:WARNING:AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentSetFchanRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_FCHAN_SET */
    if (AT_CMD_FCHAN_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* �����д���Ĵ��� */
    ulError =  AT_SetFchanRspErr(pstFchanSetCnf->enResult);
    if(DRV_AGENT_FCHAN_SET_NO_ERROR != ulError)
    {
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, ulError);
        return VOS_OK;
    }

    /* �����޴���Ĵ��� */
    g_stAtDevCmdCtrl.bDspLoadFlag    = VOS_TRUE;
    g_stAtDevCmdCtrl.ucDeviceRatMode = (VOS_UINT8)pstFchanSetCnf->stFchanSetReq.ucDeviceRatMode;
    g_stAtDevCmdCtrl.ucDeviceAtBand  = (VOS_UINT8)pstFchanSetCnf->stFchanSetReq.ucDeviceAtBand;
    g_stAtDevCmdCtrl.stDspBandArfcn  = pstFchanSetCnf->stFchanSetReq.stDspBandArfcn;
    g_stAtDevCmdCtrl.usFDAC          = 0;                                       /* FDAC���㣬��ֹG/W��Χ���� */

    /* ����AT_FormATResultDATa���������� */
    gstAtSendData.usBufLen = 0;
    At_FormatResultData(ucIndex,AT_OK);
    return VOS_OK;
}


VOS_VOID  At_HpaRfCfgCnfProc(
    HPA_AT_RF_CFG_CNF_STRU              *pstMsg
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucIndex;

    /*��ȡ���ر�����û�����*/
    ucIndex = g_stAtDevCmdCtrl.ucIndex;

    if (AT_HPA_RSLT_FAIL == pstMsg->usErrFlg)
    {
        AT_INFO_LOG("At_HpaRfCfgCnfProc: set rfcfg err");
        ulRslt = At_RfCfgCnfReturnErrProc(ucIndex);
        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex, ulRslt);
    }
    else
    {
        ulRslt = AT_OK;
        At_RfCfgCnfReturnSuccProc(ucIndex);

        /* ^FRSSI?��GDSP LOAD������յ�ID_HPA_AT_RF_CFG_CNF��Ϣ,�����ϱ�,���յ�
           ID_HPA_AT_RF_RX_RSSI_IND��Ϣʱ���ϱ� */
        if (AT_CMD_QUERY_RSSI != gastAtClientTab[ucIndex].CmdCurrentOpt)
        {
            AT_STOP_TIMER_CMD_READY(ucIndex);
            At_FormatResultData(ucIndex, ulRslt);
        }
    }
    return;
}

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)

VOS_VOID  At_CHpaRfCfgCnfProc(
    CHPA_AT_RF_CFG_CNF_STRU             *pstMsg
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucIndex;

    /*��ȡ���ر�����û�����*/
    ucIndex = g_stAtDevCmdCtrl.ucIndex;

    if (AT_HPA_RSLT_FAIL == pstMsg->usErrFlg)
    {
        AT_INFO_LOG("At_CHpaRfCfgCnfProc: set rfcfg err");
        ulRslt = At_RfCfgCnfReturnErrProc(ucIndex);
        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex, ulRslt);
    }
    else
    {
        ulRslt = AT_OK;
        At_RfCfgCnfReturnSuccProc(ucIndex);

        AT_STOP_TIMER_CMD_READY(ucIndex);
        At_FormatResultData(ucIndex, ulRslt);
    }
    return;
}
#endif


VOS_UINT32 AT_RcvDrvAgentTseLrfSetRsp(VOS_VOID *pMsg)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucIndex;
    DRV_AGENT_TSELRF_SET_CNF_STRU      *pstEvent;
    DRV_AGENT_MSG_STRU                 *pstRcvMsg;

    /* ��ʼ�� */
    pstRcvMsg              = (DRV_AGENT_MSG_STRU *)pMsg;
    pstEvent               = (DRV_AGENT_TSELRF_SET_CNF_STRU *)pstRcvMsg->aucContent;

    /* ͨ��clientid��ȡindex */
    if (AT_FAILURE == At_ClientIdToUserId(pstEvent->stAtAppCtrl.usClientId, &ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentTseLrfSetRsp: AT INDEX NOT FOUND!");
        return VOS_ERR;
    }

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvDrvAgentTseLrfSetRsp : AT_BROADCAST_INDEX.");
        return VOS_ERR;
    }

    /* ATģ���ڵȴ�TSELRF��������Ľ���¼��ϱ� */
    if (AT_CMD_TSELRF_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* ʹ��AT_STOP_TIMER_CMD_READY�ָ�AT����ʵ��״̬ΪREADY״̬ */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* �����ѯ��� */
    gstAtSendData.usBufLen = 0;
    if (DRV_AGENT_TSELRF_SET_NO_ERROR == pstEvent->enResult)
    {
        /* ���ô�����ΪAT_OK */
        ulRet                            = AT_OK;
        g_stAtDevCmdCtrl.bDspLoadFlag    = VOS_TRUE;
        g_stAtDevCmdCtrl.ucDeviceRatMode = pstEvent->ucDeviceRatMode;
        g_stAtDevCmdCtrl.usFDAC          = 0;

    }
    else
    {
        /* ��ѯʧ�ܷ���ERROR�ַ��� */
        ulRet                            = AT_ERROR;
    }

    /* 4. ����At_FormatResultData������ */
    At_FormatResultData(ucIndex, ulRet);
    return VOS_OK;
}


VOS_VOID At_RfFpowdetTCnfProc(PHY_AT_POWER_DET_CNF_STRU *pstMsg)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;

    /* ��ȡ���ر�����û����� */
    ucIndex = g_stAtDevCmdCtrl.ucIndex;

    if (AT_CMD_FPOWDET_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("At_RfFPOWDETCnfProc: CmdCurrentOpt is not AT_CMD_FPOWDET_QRY!");
        return;
    }

    /* ��λAT״̬ */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* Ӧ�����Ҫ���������ֵΪ0x7FFF��Ϊ��Чֵ�����ѯ�߷���ERROR */
    if(0x7FFF == pstMsg->sPowerDet)
    {
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_ERROR);
    }
    else
    {
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR *)pgucAtSndCodeAddr,
                                           "%s: %d",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           pstMsg->sPowerDet);

        gstAtSendData.usBufLen = usLength;

        At_FormatResultData(ucIndex, AT_OK);
    }

    return;
}


VOS_UINT32 AT_RcvMtaPowerDetQryCnf(VOS_VOID *pMsg)
{
    /* ����ֲ����� */
    AT_MTA_MSG_STRU                    *pstMtaMsg;
    MTA_AT_POWER_DET_QRY_CNF_STRU      *pstPowerDetQryCnf;
    PHY_AT_POWER_DET_CNF_STRU           stPowerNetMsg;

    /* ��ʼ����Ϣ���� */
    pstMtaMsg           = (AT_MTA_MSG_STRU *)pMsg;
    pstPowerDetQryCnf   = (MTA_AT_POWER_DET_QRY_CNF_STRU *)pstMtaMsg->aucContent;

    TAF_MEM_SET_S(&stPowerNetMsg, sizeof(stPowerNetMsg), 0x00, sizeof(PHY_AT_POWER_DET_CNF_STRU));

    if (MTA_AT_RESULT_NO_ERROR == pstPowerDetQryCnf->enResult)
    {
        stPowerNetMsg.sPowerDet = pstPowerDetQryCnf->sPowerDet;
    }
    else
    {
        stPowerNetMsg.sPowerDet = 0x7FFF;
    }

    At_RfFpowdetTCnfProc(&stPowerNetMsg);

    return VOS_OK;
}


VOS_VOID At_RfPllStatusCnfProc(PHY_AT_RF_PLL_STATUS_CNF_STRU *pstMsg)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;

    /* ��ȡ���ر�����û����� */
    ucIndex = g_stAtDevCmdCtrl.ucIndex;

    if (AT_CMD_FPLLSTATUS_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_WARN_LOG("At_RfPllStatusCnfProc: CmdCurrentOpt is not AT_CMD_FPLLSTATUS_QRY!");
        return;
    }

    /* ��λAT״̬ */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: %d,%d",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       pstMsg->usTxStatus,
                                       pstMsg->usRxStatus);

    gstAtSendData.usBufLen = usLength;

    At_FormatResultData(ucIndex, AT_OK);

    return;
}


VOS_VOID  At_RfRssiIndProc(
    HPA_AT_RF_RX_RSSI_IND_STRU          *pstMsg
)
{
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucIndex;
    VOS_UINT16                          usLength;
    VOS_INT32                           lRssi;

    /*��ȡ���ر�����û�����*/
    ucIndex  = g_stAtDevCmdCtrl.ucIndex;

    if (AT_DSP_RF_AGC_STATE_ERROR == pstMsg->sAGCGain)  /* ���� */
    {
        AT_WARN_LOG("AT_RfRssiIndProc err");
        ulRslt = AT_FRSSI_OTHER_ERR;
    }
    else
    {

        gstAtSendData.usBufLen = 0;

        /*����RSSI����ֵ��λ0.125dBm��Ϊ������������*1000.*/
        lRssi = pstMsg->sRSSI * AT_DSP_RSSI_VALUE_MUL_THOUSAND;

        /*��ȡ��RSSIֵ��������ֵ�ϱ�����ȷ��0.1dBm����ֵ��Ϣ�������ǰ��RSSI
          ֵΪ-85.1dBm������ֵΪ851. ����֮ǰ��1000�����Ծ�ȷ��0.1dBm����Ҫ��100*/
        if (lRssi < 0 )
        {
            lRssi = (-1*lRssi)/100;
        }
        else
        {
            lRssi = lRssi/100;
        }

        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR*)pgucAtSndCodeAddr, "%s:%d",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           lRssi);

        gstAtSendData.usBufLen = usLength;
        ulRslt = AT_OK;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRslt);

    return;
}


VOS_UINT32  At_PdmCtrlCnfProc( HPA_AT_PDM_CTRL_CNF_STRU *pstMsg )
{
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucIndex;

    /*��ȡ���ر�����û�����*/
    ucIndex         = g_stAtDevCmdCtrl.ucIndex;

    if ( AT_CMD_PDM_CTRL != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
          return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (AT_HPA_RSLT_FAIL == pstMsg->ulResult)
    {
        AT_INFO_LOG("At_PdmCtrlCnfProc: read PdmCtrl err");
        ulRslt      = AT_ERROR;
    }
    else
    {
        ulRslt      = AT_OK;
    }

    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}


VOS_VOID  At_RfCfgCnfReturnSuccProc(
    VOS_UINT8                           ucIndex
)
{
    switch (gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        case AT_CMD_SET_FTXON:
            g_stAtDevCmdCtrl.ucTxOnOff = g_stAtDevCmdCtrl.ucTempRxorTxOnOff;

            /* ����Ǵ򿪷������������Ҫ��¼���һ��ִ�е��Ǵ򿪷�������Ǵ򿪽��ջ����� */
            if (AT_DSP_RF_SWITCH_ON == g_stAtDevCmdCtrl.ucTxOnOff)
            {
                g_stAtDevCmdCtrl.ucRxonOrTxon = AT_TXON_OPEN;
            }
            break;

        case AT_CMD_SET_FRXON:
            g_stAtDevCmdCtrl.ucRxOnOff = g_stAtDevCmdCtrl.ucTempRxorTxOnOff;

            /* ����Ǵ򿪽��ջ���������Ҫ��¼���һ��ִ�е��Ǵ򿪷�������Ǵ򿪽��ջ����� */
            if (AT_DSP_RF_SWITCH_ON == g_stAtDevCmdCtrl.ucRxOnOff)
            {
                g_stAtDevCmdCtrl.ucRxonOrTxon = AT_RXON_OPEN;
            }
            break;

        case AT_CMD_QUERY_RSSI:
            break;

        case AT_CMD_SET_RXDIV:
           if ((AT_OK == At_SaveRxDivPara(g_stAtDevCmdCtrl.usOrigBand, 1))
            && (AT_TMODE_FTM == g_stAtDevCmdCtrl.ucCurrentTMode))
           {
               g_stAtDevCmdCtrl.ucPriOrDiv = AT_RX_DIV_ON;
               g_stAtDevCmdCtrl.usRxDiv    = g_stAtDevCmdCtrl.usOrigBand;
           }
           break;

        case AT_CMD_SET_RXPRI:
            g_stAtDevCmdCtrl.ucPriOrDiv = AT_RX_PRI_ON;
            g_stAtDevCmdCtrl.usRxPri    = g_stAtDevCmdCtrl.usOrigBand;
            break;

        default:
            break;
    }

    return;

}


VOS_VOID  At_WTxCltIndProc(
    WPHY_AT_TX_CLT_IND_STRU            *pstMsg
)
{
    /* ��ʼ��ȫ�ֱ��� */
    TAF_MEM_SET_S(&g_stAtDevCmdCtrl.stCltInfo, sizeof(g_stAtDevCmdCtrl.stCltInfo), 0x0, sizeof(AT_TX_CLT_INFO_STRU));

    /* ����CLT��Ϣ��Ч��־ */
    g_stAtDevCmdCtrl.stCltInfo.ulInfoAvailableFlg   = VOS_TRUE;

    /* ��������ϱ�����Ϣ��¼��ȫ�ֱ����� */
    g_stAtDevCmdCtrl.stCltInfo.shwGammaReal             = pstMsg->shwGammaReal;                 /* ����ϵ��ʵ�� */
    g_stAtDevCmdCtrl.stCltInfo.shwGammaImag             = pstMsg->shwGammaImag;                 /* ����ϵ���鲿 */
    g_stAtDevCmdCtrl.stCltInfo.ushwGammaAmpUc0          = pstMsg->ushwGammaAmpUc0;              /* פ����ⳡ��0����ϵ������ */
    g_stAtDevCmdCtrl.stCltInfo.ushwGammaAmpUc1          = pstMsg->ushwGammaAmpUc1;              /* פ����ⳡ��1����ϵ������ */
    g_stAtDevCmdCtrl.stCltInfo.ushwGammaAmpUc2          = pstMsg->ushwGammaAmpUc2;              /* פ����ⳡ��2����ϵ������ */
    g_stAtDevCmdCtrl.stCltInfo.ushwGammaAntCoarseTune   = pstMsg->ushwGammaAntCoarseTune;       /* �ֵ����λ�� */
    g_stAtDevCmdCtrl.stCltInfo.ulwFomcoarseTune         = pstMsg->ulwFomcoarseTune;             /* �ֵ�FOMֵ */
    g_stAtDevCmdCtrl.stCltInfo.ushwCltAlgState          = pstMsg->ushwCltAlgState;              /* �ջ��㷨����״̬ */
    g_stAtDevCmdCtrl.stCltInfo.ushwCltDetectCount       = pstMsg->ushwCltDetectCount;           /* �ջ������ܲ��� */
    g_stAtDevCmdCtrl.stCltInfo.ushwDac0                 = pstMsg->ushwDac0;                     /* DAC0 */
    g_stAtDevCmdCtrl.stCltInfo.ushwDac1                 = pstMsg->ushwDac1;                     /* DAC1 */
    g_stAtDevCmdCtrl.stCltInfo.ushwDac2                 = pstMsg->ushwDac2;                     /* DAC2 */
    g_stAtDevCmdCtrl.stCltInfo.ushwDac3                 = pstMsg->ushwDac3;                     /* DAC3 */

    return;
}


VOS_UINT32 AT_RcvMtaRficSsiRdQryCnf(VOS_VOID *pMsg)
{
    /* ����ֲ����� */
    AT_MTA_MSG_STRU                    *pstMtaMsg;
    MTA_AT_RFICSSIRD_CNF_STRU          *pstRficSsiRdCnf;
    VOS_UINT8                           ucIndex;
    VOS_UINT32                          ulResult;

    /* ��ʼ����Ϣ���� */
    ucIndex                 = 0;
    pstMtaMsg               = ( AT_MTA_MSG_STRU * )pMsg;
    pstRficSsiRdCnf         = ( MTA_AT_RFICSSIRD_CNF_STRU * )pstMtaMsg->aucContent;

    ucIndex                 = g_stAtDevCmdCtrl.ucIndex;

    if (AT_IS_BROADCAST_CLIENT_INDEX(ucIndex))
    {
        AT_WARN_LOG("AT_RcvMtaRficSsiRdQryCnf: WARNING:AT_BROADCAST_INDEX!");
        return VOS_ERR;
    }

    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_RFICSSIRD_SET */
    if ( AT_CMD_RFICSSIRD_SET != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
        AT_WARN_LOG("AT_RcvMtaRficSsiRdQryCnf: WARNING:Not AT_CMD_REFCLKFREQ_READ!");
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* ����Ĵ�����ֵ */
    gstAtSendData.usBufLen  = (VOS_UINT16)At_sprintf( AT_CMD_MAX_LEN,
                                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                                     "%s: %d",
                                                     g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                     pstRficSsiRdCnf->ulRegValue );
    ulResult                = AT_OK;

    /* ����At_FormatResultData���������� */
    At_FormatResultData(ucIndex, ulResult);

    return VOS_OK;
}


VOS_UINT32  At_MipiRdCnfProc( HPA_AT_MIPI_RD_CNF_STRU *pstMsg )
{
    VOS_UINT32                          usRslt;
    VOS_UINT32                          usData;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    /* ��ʼ�����ر��� */
    usLength        = 0;

    /*��ȡ���ر�����û�����*/
    ucIndex         = g_stAtDevCmdCtrl.ucIndex;

    if (AT_CMD_MIPI_RD != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
          return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (AT_HPA_RSLT_FAIL == pstMsg->ulResult)
    {
        AT_INFO_LOG("At_MipiRdCnfProc: read mipi err");
        usRslt = AT_ERROR;
    }
    else
    {
        usRslt      = AT_OK;
        usData      = pstMsg->ulValue;

        usLength    = (VOS_UINT16)At_sprintf( AT_CMD_MAX_LEN,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              (VOS_CHAR *)pgucAtSndCodeAddr,
                                              "%s:%d",
                                              g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                              usData);
    }
    gstAtSendData.usBufLen  = usLength;
    At_FormatResultData(ucIndex, usRslt);
    return VOS_OK;
}


VOS_UINT32  At_MipiWrCnfProc( HPA_AT_MIPI_WR_CNF_STRU       *pstMsg )
{
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucIndex;

    /*��ȡ���ر�����û�����*/
    ucIndex = g_stAtDevCmdCtrl.ucIndex;

     if ( AT_CMD_MIPI_WR != gastAtClientTab[ucIndex].CmdCurrentOpt )
     {
          return VOS_ERR;
     }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (AT_HPA_RSLT_FAIL == pstMsg->usErrFlg)
    {
        AT_INFO_LOG("At_MipiWrCnfProc: set MipiCfg err");
        ulRslt  = AT_ERROR;
    }
    else
    {
        ulRslt  = AT_OK;
    }

    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}


VOS_UINT32  At_SsiWrCnfProc( HPA_AT_SSI_WR_CNF_STRU         *pstMsg )
{
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucIndex;

    /*��ȡ���ر�����û�����*/
    ucIndex     = g_stAtDevCmdCtrl.ucIndex;

    if (AT_CMD_SSI_WR != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
          return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);


    if (AT_HPA_RSLT_FAIL == pstMsg->usErrFlg)
    {
        AT_INFO_LOG("At_SsiWrCnfProc: set rfcfg err");
        ulRslt = AT_ERROR;
    }
    else
    {
        ulRslt  = AT_OK;
    }

    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}


VOS_UINT32  At_SsiRdCnfProc( HPA_AT_SSI_RD_CNF_STRU          *pstMsg )
{
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulData;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    /* ��ʼ�����ر��� */
    usLength        = 0;

    /*��ȡ���ر�����û�����*/
    ucIndex         = g_stAtDevCmdCtrl.ucIndex;

    if ( AT_CMD_SSI_RD != gastAtClientTab[ucIndex].CmdCurrentOpt )
    {
          return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (AT_HPA_RSLT_FAIL == pstMsg->ulResult)
    {
        AT_INFO_LOG("At_MipiRdCnfProc: read SSI err");
        ulRslt = AT_ERROR;
    }
    else
    {
        ulRslt      = AT_OK;
        ulData      = pstMsg->ulValue;

        usLength    = (VOS_UINT16)At_sprintf( AT_CMD_MAX_LEN,
                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                            (VOS_CHAR *)pgucAtSndCodeAddr,
                                            "%s:%d",
                                            g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                            ulData );
    }

    gstAtSendData.usBufLen  = usLength;
    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}


VOS_UINT32 AT_SetFchanRspErr(DRV_AGENT_FCHAN_SET_ERROR_ENUM_UINT32  enResult)
{
    if(DRV_AGENT_FCHAN_BAND_NOT_MATCH == enResult)
    {
        return AT_FCHAN_BAND_NOT_MATCH;
    }

    if(DRV_AGENT_FCHAN_BAND_CHANNEL_NOT_MATCH == enResult)
    {
        return AT_FCHAN_BAND_CHANNEL_NOT_MATCH;
    }

    if(DRV_AGENT_FCHAN_OTHER_ERR == enResult)
    {
        g_stAtDevCmdCtrl.bDspLoadFlag = VOS_FALSE;
        AT_WARN_LOG("AT_SetFChanPara: DSP Load fail!");
        return AT_FCHAN_OTHER_ERR;
    }

    return DRV_AGENT_FCHAN_SET_NO_ERROR;

}


VOS_UINT32  At_RfCfgCnfReturnErrProc(
    VOS_UINT8                           ucIndex
)
{
    VOS_UINT32                          ulRslt;

    switch (gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
       case AT_CMD_SET_FTXON:
           ulRslt = AT_FTXON_SET_FAIL;
           break;

       case AT_CMD_SET_FRXON:
           ulRslt = AT_FRXON_SET_FAIL;
           break;

       case AT_CMD_QUERY_RSSI:
           ulRslt = AT_FRSSI_OTHER_ERR;
           break;

       /* ���������ͷּ�ʱ�յ�DSP�ظ���������·��صĴ�������ͬ */
       case AT_CMD_SET_RXDIV:
       case AT_CMD_SET_RXPRI:
           ulRslt = AT_CME_RX_DIV_OTHER_ERR;
           break;

       default:
           ulRslt = AT_ERROR;
           break;
    }

   return ulRslt;
}


#else
/*****************************************************************************
  4 �º���ʵ��
*****************************************************************************/


VOS_UINT32 At_SndRxOnOffReq(VOS_VOID)
{
    BBIC_CAL_RF_DEBUG_RX_REQ_STRU      *pstRxReq = VOS_NULL_PTR;
    VOS_UINT32                          ulIndex;

    /* ������Ϣ�ռ� */
    pstRxReq = (BBIC_CAL_RF_DEBUG_RX_REQ_STRU*)PS_ALLOC_MSG(WUEPS_PID_AT,
                           sizeof(BBIC_CAL_RF_DEBUG_RX_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstRxReq)
    {
        return VOS_FALSE;
    }

    /* ��ʼ�� */
    AT_MT_CLR_MSG_ENTITY(pstRxReq);

    /* ��д��Ϣͷ */
    AT_CFG_MT_MSG_HDR(pstRxReq, DSP_PID_BBA_CAL, ID_AT_BBIC_CAL_RF_DEBUG_RX_REQ);

    /* ��д��Ϣ���� */
    pstRxReq->stPara.uhwRxEnable                = g_stMtInfoCtx.stAtInfo.enTempRxorTxOnOff;
    /* ʹ��RSSI�ϱ� */
    pstRxReq->stPara.ucWbRssiReport             = VOS_TRUE;
    pstRxReq->stPara.enRxCfgMode                = BBIC_CAL_CONFIG_MODE_GAIN_INDEX;

    /* �������� */
    /* MT��λ����һ��modem���ܸ������е����ߣ��ҵ�ǰ��������û��modem�ĸ��� */
    pstRxReq->stPara.stCommonInfo.enModemId     = MODEM_ID_0;
    pstRxReq->stPara.stCommonInfo.enRatMode     = g_stMtInfoCtx.stBbicInfo.enCurrtRatMode;
    pstRxReq->stPara.stCommonInfo.enBand        = g_stMtInfoCtx.stBbicInfo.stDspBandFreq.usDspBand;
    pstRxReq->stPara.stCommonInfo.enBandWith    = g_stMtInfoCtx.stBbicInfo.enBandWidth;
    
    if (AT_ANT_TYPE_MIMO == g_stMtInfoCtx.stBbicInfo.enRxAntType)
    {
        pstRxReq->stPara.stCommonInfo.uhwMimoType   = g_stMtInfoCtx.stBbicInfo.enRxMimoType;
        pstRxReq->stPara.ucAntCfgMap                = g_stMtInfoCtx.stBbicInfo.enRxMimoAntNum;
    }
    else
    {
        pstRxReq->stPara.stCommonInfo.uhwMimoType   = AT_SET_BIT32(1);
        pstRxReq->stPara.ucAntCfgMap                = g_stMtInfoCtx.stBbicInfo.enRxAntType;
    }
    
    if (MODU_TYPE_BUTT == g_stMtInfoCtx.stBbicInfo.enFwaveType)
    {
        pstRxReq->stPara.stCommonInfo.uhwSignalType = 0;                        /* ���� */
        pstRxReq->stPara.stCommonInfo.enModType     = MODU_TYPE_LTE_QPSK;

        if (AT_RAT_MODE_GSM == g_stMtInfoCtx.stAtInfo.enRatMode)
        {
            pstRxReq->stPara.stCommonInfo.enModType         = MODU_TYPE_GMSK;
        }

        if (AT_RAT_MODE_EDGE == g_stMtInfoCtx.stAtInfo.enRatMode)
        {
            pstRxReq->stPara.stCommonInfo.enModType         = MODU_TYPE_8PSK;
        }
        
    }
    else
    {
        pstRxReq->stPara.stCommonInfo.uhwSignalType = 1;                        /* ���� */
        pstRxReq->stPara.stCommonInfo.enModType     = g_stMtInfoCtx.stBbicInfo.enFwaveType;
    }

    if (RAT_MODE_NR == g_stMtInfoCtx.stBbicInfo.enCurrtRatMode)
    {
        pstRxReq->stPara.stCommonInfo.enWaveType    = WAVEFORM_TYPE_DFT_S_OFDM; /* �������û���������ã����Ʒ��ȷ��Ĭ��ֵΪDFT_S_OFDM */
    }

    /* RXֻ��Ҫ������ */
    pstRxReq->stPara.stCommonInfo.uwRxFreqInfo      = g_stMtInfoCtx.stBbicInfo.stDspBandFreq.ulDlFreq;

    /* ���ڲ�Ʒ��ÿ��ֻ��һ����һ�����������������ݶ����һ���� */
    for (ulIndex = 0; ulIndex < RFHAL_CAL_MAX_ANT_NUM; ulIndex++)
    {
        pstRxReq->stPara.aunAntInfo[ulIndex].stAgcGainIndex.uhwAgcTableIndex   = 0xFFFF;
        pstRxReq->stPara.aunAntInfo[ulIndex].stAgcGainIndex.uhwAgcGainIndex    = g_stMtInfoCtx.stAtInfo.ucAgcGainLevel;
        pstRxReq->stPara.aunAntInfo[ulIndex].stAgcGainIndex.enWorkType         = AGC_WORK_TYPE_NOBLOCK;
    }

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstRxReq))
    {
        return VOS_FALSE;
    }

    AT_PR_LOGH("At_SndRxOnOffReq Exit");

    return VOS_TRUE;
}


VOS_UINT32 At_SndTxOnOffReq(VOS_UINT32 ulPowerDetFlg)
{
    BBIC_CAL_RF_DEBUG_TX_REQ_STRU      *pstTxReq = VOS_NULL_PTR;

    /* ������Ϣ�ռ� */
    pstTxReq = (BBIC_CAL_RF_DEBUG_TX_REQ_STRU*)PS_ALLOC_MSG(WUEPS_PID_AT,
                           sizeof(BBIC_CAL_RF_DEBUG_TX_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstTxReq)
    {
        return VOS_FALSE;
    }

    /* ��ʼ�� */
    AT_MT_CLR_MSG_ENTITY(pstTxReq);

    /* ��д��Ϣͷ */
    AT_CFG_MT_MSG_HDR(pstTxReq, DSP_PID_BBA_CAL, ID_AT_BBIC_CAL_RF_DEBUG_TX_REQ);

    /* ���� */
    pstTxReq->stPara.uhwTxEnable = g_stMtInfoCtx.stAtInfo.enTempRxorTxOnOff;

    /* ������Ϣ���� */
    /* MT��λ����һ��modem���ܸ������е����ߣ��ҵ�ǰ��������û��modem�ĸ��� */
    pstTxReq->stPara.stCommonInfo.enModemId     = MODEM_ID_0;
    pstTxReq->stPara.stCommonInfo.enRatMode     = g_stMtInfoCtx.stBbicInfo.enCurrtRatMode;
    pstTxReq->stPara.stCommonInfo.enBand        = g_stMtInfoCtx.stBbicInfo.stDspBandFreq.usDspBand;
    pstTxReq->stPara.stCommonInfo.enBandWith    = g_stMtInfoCtx.stBbicInfo.enBandWidth;

    if (AT_ANT_TYPE_MIMO == g_stMtInfoCtx.stBbicInfo.enTxAntType)
    {
        pstTxReq->stPara.stCommonInfo.uhwMimoType   = g_stMtInfoCtx.stBbicInfo.enTxMimoType;
        pstTxReq->stPara.uhwAntMap                  = g_stMtInfoCtx.stBbicInfo.enTxMimoAntNum;
    }
    else
    {   
        /* Ĭ����1 TX */
        pstTxReq->stPara.stCommonInfo.uhwMimoType   = AT_SET_BIT32(0);
        pstTxReq->stPara.uhwAntMap                  = AT_MIMO_ANT_NUM_1;
    }

    if (MODU_TYPE_BUTT == g_stMtInfoCtx.stBbicInfo.enFwaveType)
    {
        pstTxReq->stPara.stCommonInfo.uhwSignalType     = 0;                    /* ���� */
        pstTxReq->stPara.stCommonInfo.enModType         = MODU_TYPE_LTE_QPSK;   /* ����Ĭ��дQPSK */

        if (AT_RAT_MODE_GSM == g_stMtInfoCtx.stAtInfo.enRatMode)
        {
            pstTxReq->stPara.stCommonInfo.enModType         = MODU_TYPE_GMSK;
        }

        if (AT_RAT_MODE_EDGE == g_stMtInfoCtx.stAtInfo.enRatMode)
        {
            pstTxReq->stPara.stCommonInfo.enModType         = MODU_TYPE_8PSK;
        }
        
        /* TX�źŲ��� */
        pstTxReq->stPara.stTxSignalPara.uhwSignalCnt    = 1;                    /* ����д1 */
    }
    else
    {
        pstTxReq->stPara.stCommonInfo.uhwSignalType     = 2;                    /* ���� */
        pstTxReq->stPara.stCommonInfo.enModType         = g_stMtInfoCtx.stBbicInfo.enFwaveType;
        /* TX�źŲ��� */
        pstTxReq->stPara.stTxSignalPara.uhwSignalCnt    = 0;                    /* ����д0 */
    }

    pstTxReq->stPara.stTxSignalPara.aswOffset[0]   = (VOS_INT32)At_GetBaseFreq(g_stMtInfoCtx.stBbicInfo.enCurrtRatMode);                    
    pstTxReq->stPara.stTxSignalPara.aswOffset[1]   = (VOS_INT32)At_GetBaseFreq(g_stMtInfoCtx.stBbicInfo.enCurrtRatMode);

    if (RAT_MODE_NR == g_stMtInfoCtx.stBbicInfo.enCurrtRatMode)
    {
        pstTxReq->stPara.stCommonInfo.enWaveType    = WAVEFORM_TYPE_DFT_S_OFDM; /* �������û���������ã����Ʒ��ȷ��Ĭ��ֵΪDFT_S_OFDM */

        /* �������� */
        pstTxReq->stPara.unTxRatPara.stNrModuPara.enScsType = g_stMtInfoCtx.stBbicInfo.enBbicScs;
        pstTxReq->stPara.unTxRatPara.stNrModuPara.uhwRbNum  = (UINT16)g_stMtInfoCtx.stBbicInfo.enBandWidthValue / (15 * 12);
    }

    /* TXֻ��Ҫ������ */
    pstTxReq->stPara.stCommonInfo.uwTxFreqInfo      = g_stMtInfoCtx.stBbicInfo.stDspBandFreq.ulUlFreq;

    /* TX���ʲ��� */
    pstTxReq->stPara.stTxPowerPara.enPowerCtrlMode          = POWER_CTRL_MODE_POWER;    /* MTĬ��дPower mode */
    pstTxReq->stPara.stTxPowerPara.stPaPara.ucTxPaMode      = g_stMtInfoCtx.stBbicInfo.enPaLevel;
    pstTxReq->stPara.stTxPowerPara.unPowerPara.shwTxPower   = g_stMtInfoCtx.stBbicInfo.sFwavePower;

    /* �Ƿ��ѯPower Det */
    if (VOS_TRUE == ulPowerDetFlg)
    {
        pstTxReq->stPara.stMRxPara.uhwMrxEanble                 = VOS_TRUE;
        pstTxReq->stPara.stMRxPara.enMrxEstMode                 = MRX_ESTIMATE_POWER_MODE;
        pstTxReq->stPara.stMRxPara.enMrxCfgCode                 = BBIC_CAL_CONFIG_MODE_GAIN_INDEX;
        pstTxReq->stPara.stMRxPara.unMrxConfig.uhwMrxGainIndex  = 0;            /* Ĭ��д0 */
    }

    /* �������� */
    if (RAT_MODE_LTE == g_stMtInfoCtx.stBbicInfo.enCurrtRatMode)
    {
        pstTxReq->stPara.unTxRatPara.stLteRbPara.ucRbStart = 0;            /* Ĭ��д0 */
        /* ����/(15KHZ * 12) */
        pstTxReq->stPara.unTxRatPara.stLteRbPara.ucRbNum   = \
                                       (VOS_UINT8)g_stMtInfoCtx.stBbicInfo.enBandWidthValue / (15 * 12);
    }

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstTxReq))
    {
        return VOS_FALSE;
    }

    AT_PR_LOGH("At_SndTxOnOffReq Exit");

    return VOS_TRUE;

}


VOS_UINT32  At_LoadPhy(VOS_VOID)
{
    AT_CCBT_LOAD_PHY_REQ_STRU          *pstLoadPhyReq = VOS_NULL_PTR;

    /* ������Ϣ�ռ� */
    pstLoadPhyReq = (AT_CCBT_LOAD_PHY_REQ_STRU*)PS_ALLOC_MSG(WUEPS_PID_AT,
                           sizeof(AT_CCBT_LOAD_PHY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstLoadPhyReq)
    {
        return VOS_FALSE;
    }

    /* ��ʼ�� */
    AT_MT_CLR_MSG_ENTITY(pstLoadPhyReq);

    /* ��д��Ϣͷ */
    AT_CFG_MT_MSG_HDR(pstLoadPhyReq, CCPU_PID_CBT, ID_AT_CCBT_SET_WORK_MODE_REQ);

    /* ��д��Ϣ���� */
    pstLoadPhyReq->usRat        = g_stMtInfoCtx.stBbicInfo.enLoadDspMode;
    pstLoadPhyReq->ucFtmMode    = AT_MT_LOAD_DSP_FTM_MODE;                                            /* CT����,ֵΪ5 */
    pstLoadPhyReq->ucBusiness   = 1;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstLoadPhyReq))
    {
        return VOS_FALSE;
    }

    AT_PR_LOGH("At_LoadPhy Exit");

    return VOS_TRUE;
}


VOS_VOID AT_ProcCbtMsg(VOS_VOID *pstMsg)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulMsgCnt;
    VOS_UINT32                          ulRst;
    VOS_UINT16                          usMsgId;


    ulMsgCnt = sizeof(g_astAtProcCbtMsgTab) / sizeof(AT_PROC_CBT_MSG_STRU);

    usMsgId  = ((AT_MT_MSG_HEADER_STRU *)pstMsg)->usMsgId;

    /* g_astAtProcCbtMsgTab���������Ϣ�ַ� */
    for (i = 0; i < ulMsgCnt; i++)
    {
        if (g_astAtProcCbtMsgTab[i].ulMsgType == usMsgId)
        {
            ulRst = g_astAtProcCbtMsgTab[i].pProcMsgFunc(pstMsg);

            if (VOS_ERR == ulRst)
            {
                AT_ERR_LOG("AT_ProcCbtMsg: Msg Proc Err!");
            }

            return;
        }
    }

    /*û���ҵ�ƥ�����Ϣ*/
    if (ulMsgCnt == i)
    {
        AT_ERR_LOG("AT_ProcCbtMsg: Msg Id is invalid!");
    }

    return;
}


VOS_VOID AT_ProcBbicMsg(VOS_VOID *pstMsg)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulMsgCnt;
    VOS_UINT32                          ulRst;
    VOS_UINT16                          usMsgId;


    ulMsgCnt = sizeof(g_astAtProcBbicMsgTab) / sizeof(AT_PROC_BBIC_MSG_STRU);

    usMsgId  = ((AT_MT_MSG_HEADER_STRU *)pstMsg)->usMsgId;

    /* g_astAtProcBbicMsgTab���������Ϣ�ַ� */
    for (i = 0; i < ulMsgCnt; i++)
    {
        if (g_astAtProcBbicMsgTab[i].ulMsgType == usMsgId)
        {
            ulRst = g_astAtProcBbicMsgTab[i].pProcMsgFunc(pstMsg);

            if (VOS_ERR == ulRst)
            {
                AT_ERR_LOG("AT_ProcBbicMsg: Msg Proc Err!");
            }

            return;
        }
    }

    /*û���ҵ�ƥ�����Ϣ*/
    if (ulMsgCnt == i)
    {
        AT_ERR_LOG("AT_ProcBbicMsg: Msg Id is invalid!");
    }

    return;
}


VOS_UINT32 AT_ProcSetWorkModeCnf(VOS_VOID *pstMsg)
{
    AT_CCBT_LOAD_PHY_CNF_STRU          *pstLoadCnf  = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;

    pstLoadCnf  = (AT_CCBT_LOAD_PHY_CNF_STRU *)pstMsg;
    ucIndex     = g_stMtInfoCtx.stAtInfo.ucIndex;

    AT_PR_LOGH("AT_ProcSetWorkModeCnf Enter");

    /* �±걣�� */
    if (ucIndex >= AT_MAX_CLIENT_NUM)
    {
        AT_ERR_LOG("AT_ProcSetWorkModeCnf: ulIndex err!");
        return VOS_ERR;
    }

    /* �жϵ�ǰ���������Ƿ�ΪAT_CMD_FCHAN_SET */
    if (AT_CMD_FCHAN_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_ERR_LOG("AT_ProcSetWorkModeCnf: Not AT_CMD_FCHAN_SET!");
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    /* ��0��ʾ���� */
    if (MT_OK != pstLoadCnf->ulErrorCode)
    {
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_FCHAN_LOAD_DSP_ERR);
        return VOS_OK;
    }

    gstAtSendData.usBufLen = 0;
    g_stMtInfoCtx.stAtInfo.bDspLoadFlag = VOS_TRUE;
    At_FormatResultData(ucIndex, AT_OK);
    return VOS_OK;
}


VOS_UINT32 AT_ProcTxRxCnf(VOS_VOID *pstMsg)
{
    BBIC_CAL_MSG_CNF_STRU              *pstCnf = VOS_NULL_PTR;
    VOS_UINT8                           ucIndex;

    ucIndex         = g_stMtInfoCtx.stAtInfo.ucIndex;
    pstCnf          = (BBIC_CAL_MSG_CNF_STRU *)pstMsg;

    AT_PR_LOGH("AT_ProcTxRxCnf Enter");

    if (ucIndex >= AT_MAX_CLIENT_NUM)
    {
        AT_ERR_LOG("AT_ProcTxRxCnf: ulIndex err!");
        return VOS_ERR;
    }

    if ((AT_CMD_SET_FTXON   != gastAtClientTab[ucIndex].CmdCurrentOpt)
     && (AT_CMD_SET_FRXON   != gastAtClientTab[ucIndex].CmdCurrentOpt)
     && (AT_CMD_FPOWDET_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt))
    {
        AT_ERR_LOG("AT_ProcTxRxCnf: Not Set FTXON,FRXON,FPOWDET QRY !");
        return VOS_ERR;
    }

    /* Power Det�ǽ���TXON����Ϣ���͵ģ�����ǲ�ѯpower det�����˻�������Ϣ����Ҫ�ظ� ID_BBIC_TOOL_CAL_RF_DEBUG_TX_RESULT_IND */
    if (AT_CMD_FPOWDET_QRY == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        if (MT_OK != pstCnf->stPara.uwErrorCode)
        {
            /* ��λAT״̬ */
            AT_STOP_TIMER_CMD_READY(ucIndex);
            gstAtSendData.usBufLen = 0;
            At_FormatResultData(ucIndex, AT_ERROR);
            return VOS_OK;
        }

        return VOS_OK;
    }

    if (AT_CMD_SET_FTXON == gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        /* ��λAT״̬ */
        AT_STOP_TIMER_CMD_READY(ucIndex);

        if (MT_OK == pstCnf->stPara.uwErrorCode)
        {
            g_stMtInfoCtx.stAtInfo.enTxOnOff = g_stMtInfoCtx.stAtInfo.enTempRxorTxOnOff;
            gstAtSendData.usBufLen = 0;
            At_FormatResultData(ucIndex, AT_OK);
            return VOS_OK;
        }
        else
        {
            gstAtSendData.usBufLen = 0;
            At_FormatResultData(ucIndex, AT_FTXON_SET_FAIL);
            return VOS_OK;
        }
    }

    /* ����BBICģ��ظ���FTXON��Ϣ */
    /* ��λAT״̬ */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (MT_OK == pstCnf->stPara.uwErrorCode)
    {
        g_stMtInfoCtx.stAtInfo.enRxOnOff    = g_stMtInfoCtx.stAtInfo.enTempRxorTxOnOff;
        gstAtSendData.usBufLen              = 0;
        At_FormatResultData(ucIndex, AT_OK);
    }
    else
    {
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_FRXON_SET_FAIL);
    }

    return VOS_OK;
}


VOS_UINT32 AT_ProcPowerDetCnf(VOS_VOID *pstMsg)
{
    BBIC_CAL_RF_DEBUG_TX_RESULT_IND_STRU           *pstTxResultInd = VOS_NULL_PTR;
    VOS_UINT16                                      usLength;
    VOS_UINT8                                       ucIndex;
    VOS_INT32                                       lPowerValue;

    ucIndex         = g_stMtInfoCtx.stAtInfo.ucIndex;
    usLength        = 0;
    pstTxResultInd  = (BBIC_CAL_RF_DEBUG_TX_RESULT_IND_STRU *)pstMsg;

    AT_PR_LOGH("AT_ProcPowerDetCnf Enter");

    if (ucIndex >= AT_MAX_CLIENT_NUM)
    {
       AT_ERR_LOG("AT_ProcPowerDetCnf: ulIndex err !");
       return VOS_ERR;
    }

    if (AT_CMD_FPOWDET_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (MT_OK != pstTxResultInd->stPara.uwErrorCode)
    {
        gstAtSendData.usBufLen = 0;
        At_FormatResultData(ucIndex, AT_ERROR);
        return VOS_OK;
    }

    if ((RAT_MODE_LTE == g_stMtInfoCtx.stBbicInfo.enCurrtRatMode)
     || (RAT_MODE_NR == g_stMtInfoCtx.stBbicInfo.enCurrtRatMode))
    {
        /* LTE��NR DSP�ϱ�����Ϊ0.125 */
        lPowerValue = pstTxResultInd->stPara.shwAntPower * 10 / 8;             /* �ϱ�����0.1dB */
    }
    else
    {   
        /* GUC�ϱ�����Ϊ0.1 */
        lPowerValue = pstTxResultInd->stPara.shwAntPower; 
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "^FPOWDET:%d",
                                        lPowerValue);

    gstAtSendData.usBufLen = usLength;
    At_FormatResultData(ucIndex, AT_OK);

    return VOS_OK;
}


VOS_UINT32 AT_SndBbicCalMipiReadReq(
    VOS_UINT32                          ulMipiPortSel,
    VOS_UINT32                          ulSlaveId,
    VOS_UINT32                          ulRegAddr,
    VOS_UINT32                          ulByteCnt
)
{
    BBIC_CAL_RF_DEBUG_READ_MIPI_REQ_STRU       *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                                  ulLength;

    /* ����BBIC_CAL_RF_DEBUG_READ_MIPI_REQ_STRU��Ϣ */
    ulLength    = sizeof(BBIC_CAL_RF_DEBUG_READ_MIPI_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (BBIC_CAL_RF_DEBUG_READ_MIPI_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("AT_SndBbicCalMipiReadReq: alloc msg fail!");
        return AT_FAILURE;
    }

     /* ��ʼ�� */
    AT_MT_CLR_MSG_ENTITY(pstMsg);

    /* ��д��Ϣͷ */
    AT_CFG_MT_MSG_HDR(pstMsg, DSP_PID_BBA_CAL, ID_AT_BBIC_MIPI_READ_REQ);

    /* ���Ʒ��ȷ�ϣ�ÿ��ֻ��дһ��MIPI */
    pstMsg->stPara.uwMipiNum                = 1;
    pstMsg->stPara.astCMD[0].bitMipiPortSel = ulMipiPortSel;
    pstMsg->stPara.astCMD[0].bitSlaveId     = ulSlaveId;
    pstMsg->stPara.astCMD[0].bitRegAddr     = ulRegAddr;
    pstMsg->stPara.astCMD[0].bitByteCnt     = ulByteCnt;

    if ( VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_ERR_LOG("AT_SndBbicCalMipiReadReq: Send msg fail!");
        return AT_FAILURE;
    }

    AT_PR_LOGH("AT_SndBbicCalMipiReadReq Exit");

    return AT_SUCCESS;
}


VOS_UINT32 AT_SndBbicCalMipiWriteReq(
    VOS_UINT32                          ulMipiPortSel,
    VOS_UINT32                          ulSlaveId,
    VOS_UINT32                          ulRegAddr,
    VOS_UINT32                          ulByteCnt,
    VOS_UINT32                          ulValue
)
{
    BBIC_CAL_RF_DEBUG_WRITE_MIPI_REQ_STRU      *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                                  ulLength;

    /* ����BBIC_CAL_RF_DEBUG_READ_MIPI_REQ_STRU��Ϣ */
    ulLength    = sizeof(BBIC_CAL_RF_DEBUG_WRITE_MIPI_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (BBIC_CAL_RF_DEBUG_WRITE_MIPI_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("AT_SndBbicCalMipiWriteReqMsg: alloc msg fail!");
        return AT_FAILURE;
    }

    /* ��ʼ�� */
    AT_MT_CLR_MSG_ENTITY(pstMsg);

    /* ��д��Ϣͷ */
    AT_CFG_MT_MSG_HDR(pstMsg, DSP_PID_BBA_CAL, ID_AT_BBIC_MIPI_WRITE_REQ);

    pstMsg->stPara.uwMipiNum                        = 1;
    pstMsg->stPara.astData[0].stCmd.bitMipiPortSel  = ulMipiPortSel;
    pstMsg->stPara.astData[0].stCmd.bitSlaveId      = ulSlaveId;
    pstMsg->stPara.astData[0].stCmd.bitRegAddr      = ulRegAddr;
    pstMsg->stPara.astData[0].stCmd.bitByteCnt      = ulByteCnt;
    pstMsg->stPara.astData[0].stData.bitByte0       = ulValue;

    if ( VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_ERR_LOG("AT_SndBbicCalMipiWriteReq: Send msg fail!");
        return AT_FAILURE;
    }

    AT_PR_LOGH("AT_SndBbicCalMipiWriteReq Exit");

    return AT_SUCCESS;
}


VOS_UINT32 AT_RcvBbicCalMipiRedCnf(VOS_VOID *pstMsg)
{
    BBIC_CAL_RF_DEBUG_READ_MIPI_IND_STRU    *pstRedCnf = VOS_NULL_PTR;
    VOS_UINT32                               ulRslt;
    VOS_UINT32                               ulByteCnt;
    VOS_UINT16                               usLength;
    VOS_UINT8                                ucIndex;

    ulRslt      = AT_OK;
    usLength    = 0;

    /*��ȡ���ر�����û�����*/
    ucIndex     = g_stMtInfoCtx.stAtInfo.ucIndex;
    pstRedCnf   = (BBIC_CAL_RF_DEBUG_READ_MIPI_IND_STRU *)pstMsg;

    AT_PR_LOGH("AT_RcvBbicCalMipiRedCnf Enter");

    if (AT_MAX_CLIENT_NUM <= ucIndex)
    {
        AT_ERR_LOG("AT_RcvBbicCalMipiRedCnf: ulIndex err !");
        return VOS_ERR;
    }

    if (AT_CMD_MIPIOPERATE_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_ERR_LOG("AT_RcvBbicCalMipiRedCnf: CmdCurrentOpt is not AT_CMD_MIPIOPERATE_SET!");
        return VOS_ERR;
    }

    if (MT_OK == pstRedCnf->uwResult)
    {
        ulRslt   = AT_OK;
        ulByteCnt = pstRedCnf->stPara.astData[0].stCmd.bitByteCnt;

        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   "%s: %d",
                                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                                   pstRedCnf->stPara.astData[0].stData.bitByte0);

        if (1 < ulByteCnt)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN - usLength,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   ",%d",
                                                   pstRedCnf->stPara.astData[0].stData.bitByte1);
        }

        if (2 < ulByteCnt)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN - usLength,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   ",%d",
                                                   pstRedCnf->stPara.astData[0].stData.bitByte2);
        }

        if (3 < ulByteCnt)
        {
            usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN - usLength,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr,
                                                   (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                                   ",%d",
                                                   pstRedCnf->stPara.astData[0].stData.bitByte3);
        }

        gstAtSendData.usBufLen  = usLength;
    }
    else
    {
        AT_INFO_LOG("AT_RcvBbicCalMipiRedCnfMsg: read mipi err");
        ulRslt = AT_ERROR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}


VOS_UINT32 AT_RcvBbicCalMipiWriteCnf(VOS_VOID *pstMsg)
{
    BBIC_CAL_RF_DEBUG_WRITE_MIPI_IND_STRU      *pstWriteCnf = VOS_NULL_PTR;
    VOS_UINT32                                  ulRslt;
    VOS_UINT8                                   ucIndex;


    /*��ȡ���ر�����û�����*/
    ulRslt      = AT_OK;
    ucIndex     = g_stMtInfoCtx.stAtInfo.ucIndex;
    pstWriteCnf = (BBIC_CAL_RF_DEBUG_WRITE_MIPI_IND_STRU *)pstMsg;

    AT_PR_LOGH("AT_RcvBbicCalMipiWriteCnf Enter");

    if (AT_MAX_CLIENT_NUM <= ucIndex)
    {
        AT_ERR_LOG("AT_RcvBbicCalMipiWriteCnf: ulIndex err !");
        return VOS_ERR;
    }

    if (AT_CMD_MIPIOPERATE_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_ERR_LOG("AT_RcvBbicCalMipiWriteCnf: CmdCurrentOpt is not AT_CMD_MIPIOPERATE_SET!");
        return VOS_ERR;
    }

    if (MT_OK == pstWriteCnf->uwResult)
    {
        ulRslt = AT_OK;
    }
    else
    {
        ulRslt = AT_ERROR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}


VOS_UINT32 AT_SndBbicPllStatusReq(VOS_VOID)
{
    BBIC_CAL_PLL_QRY_REQ_STRU          *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    /* ����BBIC_CAL_PLL_QRY_REQ_STRU��Ϣ */
    ulLength = sizeof(BBIC_CAL_PLL_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg   = (BBIC_CAL_PLL_QRY_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("AT_SndBbicPllStatusReq: Alloc msg fail!");
        return AT_FAILURE;
    }

    /* ��ʼ�� */
    AT_MT_CLR_MSG_ENTITY(pstMsg);

    /* ��д��Ϣͷ */
    AT_CFG_MT_MSG_HDR(pstMsg, DSP_PID_BBA_CAL, ID_AT_BBIC_PLL_QRY_REQ);

    pstMsg->stPara.uhwBand   = g_stMtInfoCtx.stBbicInfo.stDspBandFreq.usDspBand;
    pstMsg->stPara.enModType = MODU_TYPE_BUTT;                                  /* GSM  ���������������GSM�������ģʽ */
    pstMsg->stPara.enRatMode = g_stMtInfoCtx.stBbicInfo.enCurrtRatMode;

    /* ���ӦPHY������Ϣ */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_ERR_LOG("AT_SndBbicPllStatusReq: Send msg fail!");
        return AT_FAILURE;
    }

    AT_PR_LOGH("AT_SndBbicPllStatusReq Exit");

    return AT_SUCCESS;
}

VOS_UINT32 At_RcvBbicPllStatusCnf(VOS_VOID *pstMsg)
{
    BBIC_CAL_PLL_QRY_IND_STRU          *pstQryCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;


    /*��ȡ���ر�����û�����*/
    ulRslt      = AT_OK;
    usLength    = 0;
    ucIndex     = g_stMtInfoCtx.stAtInfo.ucIndex;
    pstQryCnf   = (BBIC_CAL_PLL_QRY_IND_STRU *)pstMsg;

    AT_PR_LOGH("At_RcvBbicPllStatusCnf Enter");

    if (AT_MAX_CLIENT_NUM <= ucIndex)
    {
        AT_ERR_LOG("At_RcvBbicPllStatusCnf: ulIndex err !");
        return VOS_ERR;
    }

    if (AT_CMD_FPLLSTATUS_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_ERR_LOG("At_RcvBbicPllStatusCnf: CmdCurrentOpt is not AT_CMD_FPLLSTATUS_QRY!");
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (MT_OK == pstQryCnf->stPara.uwErrorCode)
    {
        ulRslt   = AT_OK;
        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: %d,%d",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                       pstQryCnf->stPara.uwTxPllStatus,
                                       pstQryCnf->stPara.uwRxPllStatus);
    }
    else
    {
        ulRslt = AT_ERROR;
    }

    gstAtSendData.usBufLen = usLength;

    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}


VOS_UINT32 AT_SndBbicRssiReq(VOS_VOID)
{
    BBIC_CAL_RF_DEBUG_RSSI_REQ_STRU          *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                                ulLength;

    /* ����BBIC_CAL_RF_DEBUG_RSSI_REQ_STRU��Ϣ */
    ulLength = sizeof(BBIC_CAL_RF_DEBUG_RSSI_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg   = (BBIC_CAL_RF_DEBUG_RSSI_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("AT_SndBbicRssiReq: alloc msg fail!");
        return AT_FRSSI_OTHER_ERR;
    }

    /* ��ʼ�� */
    AT_MT_CLR_MSG_ENTITY(pstMsg);

    /* ��д��Ϣͷ */
    AT_CFG_MT_MSG_HDR(pstMsg, DSP_PID_BBA_CAL, ID_AT_BBIC_CAL_RF_DEBUG_RSSI_REQ);

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_ERR_LOG("AT_SndBbicRssiReq: Send msg fail!");
        return AT_FRSSI_OTHER_ERR;
    }

    AT_PR_LOGH("AT_SndBbicRssiReq Exit");

    return AT_SUCCESS;
}


VOS_UINT32  At_RcvBbicRssiInd(VOS_VOID *pstMsg)
{
    BBIC_CAL_RF_DEBUG_RX_RSSI_IND_STRU *pstRssiIndMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_INT32                           lRssi;
    VOS_UINT16                          usLength;
    VOS_UINT8                           ucIndex;

    /*��ȡ���ر�����û�����*/
    lRssi           = 0;
    ulRslt          = AT_OK;
    usLength        = 0;
    ucIndex         = g_stMtInfoCtx.stAtInfo.ucIndex;
    pstRssiIndMsg   = (BBIC_CAL_RF_DEBUG_RX_RSSI_IND_STRU *)pstMsg;

    AT_PR_LOGH("At_RcvBbicRssiInd Enter");

    if (AT_MAX_CLIENT_NUM <= ucIndex)
    {
        AT_ERR_LOG("At_RcvBbicRssiInd: ulIndex err !");
        return VOS_ERR;
    }

    if (AT_CMD_QUERY_RSSI != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_ERR_LOG("At_RcvBbicRssiInd: CmdCurrentOpt Not Query Rssi !");
        return VOS_ERR;
    }

    /* MIMO������1��3��5��7ȡashwRssi[0]�� 2��4��6��8ȡashwRssi[1] */
    if ((AT_ANT_TYPE_PRI == g_stMtInfoCtx.stBbicInfo.enRxAntType)
     || ((1 == g_stMtInfoCtx.stAtInfo.enRxMimoAntNum % 2 )
      && (AT_ANT_TYPE_MIMO == g_stMtInfoCtx.stBbicInfo.enRxAntType)))
    {
        /*����RSSI����ֵ��λ0.125dBm��Ϊ������������*1000.*/
        lRssi = pstRssiIndMsg->stPara.ashwRssi[0] * AT_DSP_RSSI_VALUE_MUL_THOUSAND;
    }
    else
    {
        /*����RSSI����ֵ��λ0.125dBm��Ϊ������������*1000.*/
        lRssi = pstRssiIndMsg->stPara.ashwRssi[1] * AT_DSP_RSSI_VALUE_MUL_THOUSAND;
    }

    if (MT_OK != pstRssiIndMsg->stPara.uwErrorCode)
    {
        AT_ERR_LOG("At_RcvBbicRssiIndProc err");
        ulRslt = AT_ERROR;
    }
    else
    {
        /*��ȡ��RSSIֵ��������ֵ�ϱ�����ȷ��0.01dBm����ֵ��Ϣ�������ǰ��RSSI
          ֵΪ-85.1dBm������ֵΪ8510. ����֮ǰ��1000�����Ծ�ȷ��0.01dBm����Ҫ��10*/
        if (lRssi < 0 )
        {
            lRssi = (-1*lRssi)/100;
        }
        else
        {
            lRssi = lRssi/100;
        }

        usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr,
                                           (VOS_CHAR*)pgucAtSndCodeAddr,
                                           "%s:%d",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                           lRssi);

        gstAtSendData.usBufLen = usLength;
        ulRslt = AT_OK;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);
    
    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}


VOS_UINT32 AT_SndBbicCalSetDpdtReq(
    BBIC_DPDT_OPERTYPE_ENUM_UINT16      enOperType,
    VOS_UINT32                          ulValue,
    VOS_UINT32                          ulWorkType
)
{
    BBIC_CAL_DPDT_REQ_STRU             *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    /* ����BBIC_CAL_DPDT_REQ_STRU��Ϣ */
    ulLength = sizeof(BBIC_CAL_DPDT_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg   = (BBIC_CAL_DPDT_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("At_SndBbicCalDpdtReq: alloc msg fail!");
        return AT_FAILURE;
    }

    /* ��ʼ�� */
    AT_MT_CLR_MSG_ENTITY(pstMsg);

    /* ��д��Ϣͷ */
    AT_CFG_MT_MSG_HDR(pstMsg, DSP_PID_BBA_CAL, ID_AT_BBIC_DPDT_REQ);

    pstMsg->stPara.enRatMode                = g_stMtInfoCtx.stBbicInfo.enDpdtRatMode;
    pstMsg->stPara.enOperType               = enOperType;
    pstMsg->stPara.uwValue                  = ulValue;
    pstMsg->stPara.uhwModemWorkType         = (VOS_UINT16)ulWorkType;

    if ( VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_ERR_LOG("At_SndBbicCalDpdtReq: Send msg fail!");
        return AT_FAILURE;
    }

    
    AT_PR_LOGH("AT_SndBbicCalSetDpdtReq Exit");

    return AT_SUCCESS;
}



VOS_UINT32 AT_RcvBbicCalSetDpdtCnf(VOS_VOID *pstMsg)
{
    BBIC_CAL_DPDT_IND_STRU             *pstDpdtCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucIndex;

    ulRslt          = AT_ERROR;

    /* ��ȡ���ر�����û����� */
    ucIndex = g_stMtInfoCtx.stAtInfo.ucIndex;
    pstDpdtCnf = (BBIC_CAL_DPDT_IND_STRU *)pstMsg;

    AT_PR_LOGH("AT_RcvBbicCalSetDpdtCnf Enter");

    if (AT_MAX_CLIENT_NUM <= ucIndex)
    {
        AT_ERR_LOG("AT_RcvBbicCalSetDpdtCnf: ulIndex err !");
        return VOS_ERR;
    }

    if ((AT_CMD_DPDT_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
     && (AT_CMD_DPDTQRY_SET != gastAtClientTab[ucIndex].CmdCurrentOpt))
    {
        AT_ERR_LOG("AT_RcvBbicCalSetDpdtCnf: CmdCurrentOpt is not AT_CMD_DPDT_SET or AT_CMD_DPDTQRY_SET!");
        return VOS_ERR;
    }

    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (MT_OK != pstDpdtCnf->stPara.uwErrorCode)
    {
        AT_ERR_LOG1("AT_RcvBbicCalSetDpdtCnf: set dpdt error, ErrorCode is ", pstDpdtCnf->stPara.uwErrorCode);
        ulRslt = AT_ERROR;
    }
    else
    {
        /* ����������״̬ΪGetʱ���ϱ���ѯDpdt��� */
        if (BBIC_DPDT_OPERTYPE_GET == pstDpdtCnf->stPara.unOperType)
        {
            ulRslt = AT_OK;
            gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               (VOS_CHAR *)pgucAtSndCodeAddr,
                                               "%s: %d",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                               pstDpdtCnf->stPara.uwValue);
        }
        /* ����������״̬ΪSetʱ���������óɹ� */
        else
        {
            ulRslt = AT_OK;
            gstAtSendData.usBufLen = 0;
        }
    }

    /* ����At_FormatResultData���������� */
    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}




VOS_UINT32 AT_SndBbicCalQryFtemprptReq(INT16 lChannelNum)
{
    BBIC_CAL_TEMP_QRY_REQ_STRU         *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    /* ����BBIC_CAL_TEMP_QRY_REQ_STRU��Ϣ */
    ulLength    = sizeof(BBIC_CAL_TEMP_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;
    pstMsg      = (BBIC_CAL_TEMP_QRY_REQ_STRU *)PS_ALLOC_MSG(WUEPS_PID_AT, ulLength);

    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("AT_SndBbicCalQryFtemprptReq: alloc msg fail!");
        return AT_FAILURE;
    }

    /* ��ʼ�� */
    AT_MT_CLR_MSG_ENTITY(pstMsg);

    /* ��д��Ϣͷ */
    AT_CFG_MT_MSG_HDR(pstMsg, DSP_PID_BBA_CAL, ID_AT_BBIC_TEMP_QRY_REQ);

    pstMsg->stPara.enChannelType  = g_stMtInfoCtx.stBbicInfo.enCurrentChannelType;
    pstMsg->stPara.hwChannelNum   = lChannelNum;

    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_ERR_LOG("AT_SndBbicCalQryFtemprptReq: Send msg fail!");
        return AT_FAILURE;
    }

    AT_PR_LOGH("AT_SndBbicCalQryFtemprptReq Exit");

    return AT_SUCCESS;
}



VOS_UINT32 AT_RcvBbicCalQryFtemprptCnf(VOS_VOID *pstMsg)
{
    BBIC_CAL_TEMP_QRY_IND_STRU         *pstFtemprptCnf = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucIndex;

    ulRslt   = AT_ERROR;

    /*��ȡ���ر�����û�����*/
    ucIndex = g_stMtInfoCtx.stAtInfo.ucIndex;
    pstFtemprptCnf = (BBIC_CAL_TEMP_QRY_IND_STRU *)pstMsg;

    AT_PR_LOGH("AT_RcvBbicCalQryFtemprptCnf Enter");

    if (ucIndex >= AT_MAX_CLIENT_NUM)
    {
        AT_ERR_LOG("AT_RcvBbicCalQryFtemprptCnf: ulIndex err !");
        return VOS_ERR;
    }

    if (AT_CMD_FTEMPRPT_QRY != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_ERR_LOG("AT_RcvBbicCalQryFtemprptCnf: CmdCurrentOpt is not AT_CMD_OPT_READ_CMD!");
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (MT_OK != pstFtemprptCnf->stPara.uwErrorCode)
    {
        ulRslt = AT_ERROR;
        AT_ERR_LOG1("AT_RcvBbicCalQryFtemprptCnf: qry Ftemprpt error, ErrorCode is ", pstFtemprptCnf->stPara.uwErrorCode);
        gstAtSendData.usBufLen = 0;
    }
    else
    {
        ulRslt = AT_OK;
        gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                     (VOS_CHAR *)pgucAtSndCodeAddr,
                                      "%s: %d,%d,%d",
                                      g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                      pstFtemprptCnf->stPara.enChannelType,
                                      pstFtemprptCnf->stPara.hwChannelNum,
                                      pstFtemprptCnf->stPara.wTemperature);
    }

    /* ����At_FormatResultData���������� */
    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}


VOS_UINT32 At_SndDcxoReq(VOS_VOID)
{
    BBIC_CAL_DCXO_REQ_STRU             *pstDcxoReq = VOS_NULL_PTR;

    /* ������Ϣ�ռ� */
    pstDcxoReq = (BBIC_CAL_DCXO_REQ_STRU*)PS_ALLOC_MSG(WUEPS_PID_AT,
                           sizeof(BBIC_CAL_DCXO_REQ_STRU) - VOS_MSG_HEAD_LENGTH);
    if (VOS_NULL_PTR == pstDcxoReq)
    {
        return VOS_FALSE;
    }

    /* ��ʼ�� */
    AT_MT_CLR_MSG_ENTITY(pstDcxoReq);

    /* ��д��Ϣͷ */
    AT_CFG_MT_MSG_HDR(pstDcxoReq, DSP_PID_BBA_CAL, ID_AT_BBIC_DCXO_REQ);

    pstDcxoReq->stPara.enSetType    = g_stMtInfoCtx.stBbicInfo.enDcxoTempCompEnableFlg;
    pstDcxoReq->stPara.ulTxFreq     = g_stMtInfoCtx.stBbicInfo.stDspBandFreq.ulUlFreq;
    pstDcxoReq->stPara.usBand       = g_stMtInfoCtx.stBbicInfo.stDspBandFreq.usDspBand;
    
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstDcxoReq))
    {
       return VOS_FALSE;
    }

    AT_PR_LOGH("At_SndDcxoReq Exit");

    return VOS_TRUE;
    
}


VOS_UINT32 At_RcvBbicCalDcxoCnf(VOS_VOID *pstMsg)
{
    BBIC_CAL_DCXO_IND_STRU             *pstDcxoInd = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;
    VOS_UINT8                           ucIndex;

    /*��ȡ���ر�����û�����*/
    ucIndex     = g_stMtInfoCtx.stAtInfo.ucIndex;
    pstDcxoInd  = (BBIC_CAL_DCXO_IND_STRU*)pstMsg;
    ulRslt      = AT_OK;

    AT_PR_LOGH("At_RcvBbicCalDcxoCnf Enter");

    if (ucIndex >= AT_MAX_CLIENT_NUM)
    {
        AT_ERR_LOG("At_RcvBbicCalDcxoCnf: ulIndex err !");
        return VOS_ERR;
    }

    if (AT_CMD_DCXOTEMPCOMP_SET != gastAtClientTab[ucIndex].CmdCurrentOpt)
    {
        AT_ERR_LOG("At_RcvBbicCalDcxoCnf: CmdCurrentOpt is not AT_CMD_DCXOTEMPCOMP_SET!");
        return VOS_ERR;
    }

    /* ��λAT״̬ */
    AT_STOP_TIMER_CMD_READY(ucIndex);

    if (MT_OK != pstDcxoInd->ulErrorCode)
    {
        ulRslt = AT_ERROR;
        AT_ERR_LOG1("At_RcvBbicCalDcxoCnf: ErrorCode is ", pstDcxoInd->ulErrorCode);
        gstAtSendData.usBufLen = 0;
    }
    else
    {
        ulRslt = AT_OK;
        gstAtSendData.usBufLen = 0;
        g_stMtInfoCtx.stAtInfo.enDcxoTempCompEnableFlg = (AT_DCXOTEMPCOMP_ENABLE_ENUM_UINT8)g_stMtInfoCtx.stBbicInfo.enDcxoTempCompEnableFlg;
    }

    /* ����At_FormatResultData���������� */
    At_FormatResultData(ucIndex, ulRslt);

    return VOS_OK;
}


#endif


