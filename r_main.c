/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_main.c
* Version      : CodeGenerator for RL78/G13 V2.05.03.01 [12 Nov 2018]
* Device(s)    : R5F100LG
* Tool-Chain   : CA78K0R
* Description  : This file implements main function.
* Creation Date: 2019-10-25
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */

/**************************************/
/* section                            */
/**************************************/
/*  */
#ifdef __78K0R_SMALL__
  #pragma section @@CODE  SAM_CODE
#else
  #pragma section @@CODEL SAM_CODE
#endif

/*  */
#ifdef __78K0R_LARGE__
  #pragma section @@CNSTL SAM_CNST
#else
  #pragma section @@CNST  SAM_CNST
#endif

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_intc.h"
#include "r_cg_serial.h"
#include "r_cg_adc.h"
#include "r_cg_timer.h"
#include "r_cg_wdt.h"
/* Start user code for include. Do not edit comment generated here */

/* FDL,EEL library header files  */
#include "fdl.h"
#include "fdl_descriptor.h"
#include "eel.h"
#include "eel_descriptor.h"
#include "eel_user_types.h"
#include "M150k_Table.h"
#include "M199k_Table.h"
#include "LPF_filter.h"
#include "u_dbg_uart.h"

/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */



#define register_test_psw_enabled
#define register_test_enabled
#define register_test_stack_enabled
#define register_test_cs_enabled
#define register_test_es_enabled

/* Flash EEPROM */
#define SAM_EEL_MODE_ENFORCED   0xFF    /* Enforced mode (0xFF)         */
#define SAM_EEL_MODE_TIMEOUT    0x10    /* Timeout  mode (0x01?0xFE)   */
#define SAM_EEL_MODE_POLLING    0x00    /* Polling  mode (0x00)         */
#define SAM_EEL_MAINTE_END      0x04


// Saving to Memory
//uint8_t WriteFlashEnable = 0;
//unsigned int Writecnt = 0;
unsigned int IniMemoryCheck = 0;

fdl_status_t    fdlStatus = 0;
eel_request_t   dtyEelReq;


/***********************************************************************************************************************
ADC Struct Value & Table.
***********************************************************************************************************************/

typedef struct
{
	uint16_t Adc_Data;
	int16_t	Temp;					///< ADC Data Value, Convert Temp Value.
}stTable;

typedef struct
{
	const uint8_t 	st_Size;		///< 온도 테이블 처리 사이즈.
	uint8_t 		st_Index;		///< 온도 테이블 처리 인덱스.
	stTable 		*pst;			///< 센서 온도센서 사양서 테이블.
}stSensorTable;


/*
typedef union
{
	uint16_t i;
	uint8_t b[2];
}byte2;

typedef union
{
	uint16_t i;
	struct{
		uint16_t a:8;
		uint16_t b:8;
	}b;
}byte_2;

byte2 test1;
byte_2 test2;
*/


const stTable astTemp_table[25] = {
	854, 0, 	// 0
	821, 500,	// 1
	784, 1000,	// 2
	744, 1500,	// 3
	701, 2000,	// 4
	657, 2500,	// 5
	611, 3000,	// 6
	565, 3500,	// 7
	520, 4000,	// 8
	476, 4500,	// 9
	433, 5000,	// 10
	389, 5500,	// 11
	355, 6000,	// 12
	319, 6500,	// 13
	287, 7000,	// 14
	258, 7500,	// 15
	231, 8000,	// 16
	207, 8500,	// 17
	185, 9000,	// 18
	166, 9500,	// 19
	148, 10000, // 20
	133, 10500, // 21
	119, 11000, // 22
	107, 11500, // 23
	96,  12000	// 24
};

const stTable astTitle_table[25] = {
	896 , 0,	// 0
	865 , 500,	// 1
	829 , 1000, // 2
	789 , 1500, // 3
	744 , 2000, // 4
	697 , 2500, // 5
	646 , 3000, // 6
	595 , 3500, // 7
	543 , 4000, // 8
	492 , 4500, // 9
	443 , 5000, // 10
	397 , 5500, // 11
	353 , 6000, // 12
	313 , 6500, // 13
	277 , 7000, // 14
	244 , 7500, // 15
	215 , 8000, // 16
	189 , 8500, // 17
	166 , 9000, // 18
	146 , 9500, // 19
	129 , 10000, // 20
	114 , 10500, // 21
	100 , 11000, // 22
	88 , 11500, // 23
	78 , 12000	// 24

};

stSensorTable stTempTb = {25, 0, astTemp_table};
stSensorTable stTitleTb = {25, 0, astTitle_table};
T_fTEMP mstTemp = {0,};
uint16_t munPumpOffDelay=0;

T_STD_RC mstStdRc={
				{false, false, false, false},
				false,
				false,
				false,
				false
				};

T_FAN_AGM_INFO mstFnA={
		false, 0,
	    false,0,0,0,0,0,0,	// tbNum.

		0,					// targetPwm.
		0,					// targetAgm.
		0, 0,				// cutRpm, addCutRpm.
		0,					// addMaxCal.

		{
		{0,},
		{0,},
		{0,},
		{1150,1120,1100,1050,950,850,800,700,500,300,0},
		{0,},

		{{0,}, }
		}
	};



/***********************************************************************************************************************
***********************************************************************************************************************/


volatile uint8_t	EELclean_cnt = 0;
//

//IWM Stepping Motor
const uint8_t 	IWM_table[4] = {0x0a,0x09,0x11,0x12};
volatile uint8_t	IWM_on = 0;
volatile uint8_t 	IWM_table_cnt = 0;

volatile unsigned int 	IWM_CurrPoint = 0;
volatile unsigned int 	IWM_GallPoint = 0;
volatile unsigned int 	BF_IWM_GallPoint = 0;
volatile bool 			IWM_ForcedOpen=false;	// 강제 MIN 시 해제는 Full OPEN 하고 진행.
//volatile unsigned int 	IWM_percent = 0;

volatile uint8_t 	IWM_Time1ms = 0;
//volatile unsigned int 	IWMspeedTime = 0;

volatile uint8_t	IWM_CloseTest_en = 0;
volatile uint8_t	IWM_CloseTest_cnt = 0;
volatile uint8_t	err_IWM_ZeroClose = 0;
volatile uint8_t	IWM_OpenTest_en = 0;
volatile uint8_t	IWM_OpenTest_cnt = 0;
volatile uint8_t	err_IWM_ZeroOpen = 0;

volatile uint8_t	IWM_Refreeze1sec_cnt = 0;
volatile unsigned int	IWM_Refreeze360Min_cnt = 0;	// 2초마다 1카우트.
volatile uint8_t	IWM_intial_en = 0;
volatile unsigned int	IWM_intial_cnt = 0;

volatile uint8_t	IWMdriver_en = 0;
volatile unsigned int	IWMdriver_cnt = 0;
volatile uint8_t	IWM_gall_en = 0;
volatile unsigned int	IWM_gall_cnt = 0;

volatile uint8_t	IWMuse = 0;
volatile uint8_t	IWM_intial = 0;

volatile uint8_t	IWM_enable = 0;
volatile uint8_t	IWMset_en = 0;
volatile unsigned int	IWMset_cnt = 0;
volatile uint8_t	IWMOPset_en = 0;
volatile unsigned int	IWMOPset_cnt = 0;


//DHM
// Time
const uint8_t 	DHM_table[4] = {0x14,0x12,0x0a,0x0c};
//SHIN
//const uint8_t 	DHM_table[4] = {0x14,0x0c,0x0a,0x12};
volatile uint8_t	DHM_on = 0;
volatile uint8_t 	DHM_table_cnt = 0;

volatile unsigned int 	DHM_CurrPoint = 0;
volatile unsigned int 	DHM_GallPoint = 0;
//volatile unsigned int 	DHM_percent = 0;

volatile uint8_t 	DHM_Time1ms = 0;
volatile uint8_t 	DHM_stopTime_cnt = 0;

volatile uint8_t	DHM_CloseTest_en = 0;
volatile uint8_t	DHM_CloseTest_cnt = 0;
volatile uint8_t	err_DHM_ZeroClose = 0;
volatile uint8_t	DHM_OpenTest_en = 0;
volatile uint8_t	DHM_OpenTest_cnt = 0;
volatile uint8_t	err_DHM_ZeroOpen = 0;
volatile uint8_t	DHM_ZeroOpen_state = 0;

volatile uint8_t	DHM_Refreeze1sec_cnt = 0;
volatile unsigned int	DHM_Refreeze360Min_cnt = 0;
volatile uint8_t	DHM_intial_en = 0;
volatile unsigned int	DHM_intial_cnt = 0;
volatile uint8_t	DHM_intial_OK = 0;

volatile uint8_t	DHM_enable = 0;
volatile uint8_t	DHMset_en = 0;
volatile unsigned int	DHMset_cnt = 0;
volatile uint8_t	DHMOPset_en = 0;
volatile unsigned int	DHMOPset_cnt = 0;




//AGM
const uint8_t AGM_table[4] = {0xa0,0x90,0x50,0x60};
volatile T_AGM_INFO mstAgm={
				false,	// init.
				false,	// stepping.

				0,		// tbCnt.
				0,		// tmr1ms;
				0,		// tmrRefreeze1sec.
				0,		// tmrRefreeze360min.

				0, 0,	// curr/gall point.

				AGM_table
			};


// Common Ext
volatile bool 			CommonEXT = false;	// 역류 방지 팬 가동.
volatile unsigned int 	munCcComVentPwm = 0;
volatile unsigned int 	munFollowComVentPWM = 0;



//*********************************************
// PT Setting
//*********************************************
volatile unsigned int 	RoomPW = 0;
volatile uint8_t 	ManualReset = 0;

volatile uint8_t 	Error_ok = 0;

volatile bool 	bRoomRunDemand = false;


//*********************************************
// PT time
//*********************************************
volatile unsigned int	PT_time  = 0;
volatile uint8_t	PT_step = 0;


//*****************************************************
// Fan Operation
//*****************************************************

volatile unsigned int 	FanCtrl_Value = 0;
volatile unsigned int 	FanMonitor = 0;
volatile float 	MaxFancut  = 0;



volatile unsigned long	FanPwmPulse_cnt = 0;
volatile unsigned long	FanPwmStop = 0;
volatile uint8_t 	Fan_ON = 0;
volatile uint8_t 	PwmOff = 0;

// Fan RPM Sensor
//*********************************************
volatile unsigned int	Rpm_cnt = 0;
volatile unsigned int 	RpmCnt_Result = 0;
volatile uint8_t	RpmCnt_Repeat = 0;
volatile float			Rpm_Hz = 0;
volatile unsigned int	FanRpm = 0;

volatile uint8_t	FanRpm_time = 0;
volatile float			RpmHzSum = 0;
volatile uint8_t	FanRpm_cnt = 0;

volatile unsigned int	FanRpmNG_cnt = 0;
volatile uint8_t	FanRun_NG = 0;
volatile uint8_t	err_FanRpm = 0;
volatile uint8_t	err_FanRpm_CHK_en = 0;
volatile uint8_t	err_FanRpm_CHK_cnt = 0;

//volatile unsigned int	FanSpeedChk = 0;
//volatile unsigned int	FanTDR03Chk = 0;



/* NCP-A DATA-ID table */
volatile unsigned int NCP_tab[NCP_TOTALNO] = {0};
//volatile unsigned int rx_DataId[1] = {0};

//*****************************************************
// ADC Variable
//*****************************************************
volatile unsigned int 	temp_average[6] = {0,0,0,0,0,0};
volatile uint8_t 	adchk_cnt = 0;

volatile unsigned int  	ADNo_2CurrDegree = 0;

volatile unsigned int  	ADNo_CurrAvg = 0;

volatile uint8_t  ad0chk_ok = 0;
volatile uint8_t  ad1chk_ok = 0;
volatile uint8_t  ad2chk_ok = 0;
volatile uint8_t  ad3chk_ok = 0;
volatile uint8_t  ad4chk_ok = 0;
volatile uint8_t  ad5chk_ok = 0;

// Flame Check
volatile unsigned short	Flame_ad =0;
volatile uint8_t 	Fire_flag = 0;

volatile uint8_t 	FireOff_delay_en = 0;
volatile uint8_t 	FireOff_delay_cnt = 0;

////
volatile uint8_t	err_McuSafety = 0;
volatile uint8_t	TestProbe = 0;


T_ERR_TEMP mstErrExhaust={false,false,false,0};

T_ERR_TEMP mstErrInlet={false,false,false,0};

T_ERR_TEMP mstErrDhw={false,false,false,0};

T_ERR_TEMP mstErrOp={false,false,false,0};

T_ERR_TEMP mstErrTitle={false,false,false,0};

T_ERR_TEMP mstErrExOverHeat={false,false,false,0};

T_ERR_TEMP mstErrSensorOverHeat={false,false,false,0};

volatile uint8_t 	Cnt_Reset94 = 0;

volatile uint8_t 	Er94Cnt = 0;
volatile uint8_t 	Er94off_en = 0;
volatile unsigned int 	Er94off_Cnt = 0;



//*****************************************************
// Input Variable
//*****************************************************
volatile unsigned int	in_IntervalTime = 0;


typedef struct
{
	bool d1, d2, d3, d4, d5, d6, d7;
}T_SW_INFO;


typedef struct
{
	uint8_t kcalInfo;
	T_SW_INFO sw;
}T_DIP_Info;

T_DIP_Info mstDip={
			false,false,false,false,false,false,false,
			0
			};


volatile uint8_t 	HighAltitudeMode = 0;


T_ERR_TEMP mstHighLimit={false,false,false,0};	// err.20

T_ERR_TEMP mstBurnLimit={false,false,false,0};	// err.43

T_ERR_TEMP mstEqs={false,false,false,0};	// err.42

T_ERR_TEMP mstAps={false,false,false,0};	// err.29


volatile uint8_t	Flow_start = 0;

//*****************************************************
// Output Variable
//*****************************************************
volatile uint8_t 	out_GV_cnt = 0;
volatile uint8_t 	out_GV = 0;
volatile uint8_t 	out_IT = 0;

volatile uint8_t 	out_CP1 = 0;
//volatile uint8_t 	out_CP2 = 0;
//volatile uint8_t 	out_3Way = 0;

volatile uint8_t	CP1_refreeze = 0;
volatile uint8_t	CP1_refreeze_en = 0;
volatile uint8_t	CP1_refreeze1sec_cnt = 0;
volatile unsigned int 	CP1_refreeze_cnt = 0;
/*
volatile uint8_t	CP2_refreeze = 0;
volatile uint8_t	CP2_refreeze_en = 0;
volatile uint8_t	CP2_refreeze1sec_cnt = 0;
volatile unsigned int 	CP2_refreeze_cnt = 0;
*/
// Water Level
volatile uint8_t  out_WaterLevel = 0;
volatile unsigned int  	WLevel_mS_Cnt = 0;

volatile uint8_t  HighLevel = 0;
volatile uint8_t  HighLevel_cnt = 0;
volatile uint8_t  err_HighLevel = 0;	// err_HighLevel '1' is Error
volatile uint8_t  err_HighLevel_cnt = 0;
volatile uint8_t  err_RecoverHighLevel_cnt = 0;
volatile uint8_t  RecoverHighLevel_cnt = 0;

volatile uint8_t  LowLevel = 0;
volatile uint8_t  LowLevel_cnt = 0;
volatile uint8_t  err_LowLevel_cnt = 0;
volatile uint8_t  err_RecoverLowLevel_cnt = 0;
volatile uint8_t  RecoverLowLevel_cnt = 0;

volatile uint8_t  WLevel_cnt = 0;
volatile uint8_t  HighLevel_state = 0;

// Check to Water Level circuit
volatile uint8_t  err_HIGHcircuit = 0;
volatile uint8_t 	chk_HIGHcircuit = 0;
volatile unsigned int  	chk_HIGHcircuit_cnt = 0;
volatile unsigned int  	RecoverHIGHcircuit_cnt = 0;

volatile uint8_t  err_LOWcircuit = 0;
volatile uint8_t 	chk_LOWcircuit = 0;
volatile unsigned int  	chk_LOWcircuit_cnt = 0;
volatile unsigned int  	RecoverLOWcircuit_cnt = 0;

//*****************************************************
// Room is to be or not to be
//*****************************************************
volatile unsigned int	RoomPWoff_cnt = 0;
volatile unsigned int	RoomPWrepeat_cnt = 0;
volatile uint8_t	RoomToBe = 0;

//*****************************************************
// Control Power check
//*****************************************************
volatile unsigned int	PWTime5s_cnt = 0;
volatile uint8_t 	PowerHzOK = 0;
volatile uint8_t 	PowerHz = 0;

volatile uint8_t 	CurrFreqPortState = 0;
volatile uint8_t 	FreqCnt = 0;
volatile unsigned int 	T_Freq1s_Cnt = 0;
volatile uint8_t 	CurrFrequency = 0;
volatile unsigned int 	OutPoint_cnt = 0;
volatile uint8_t 	ZeroPointState = 0;

// Flow
//** Flow Sensor Type
volatile uint8_t  flow_op_stat = 0;


volatile unsigned int 	FlowLiter = 0;
volatile unsigned int 	FlowLiter_x10 = 0;
volatile uint8_t 	FlowOffDelayTmr = 0;
volatile uint8_t 	FlowOnDelayTmr = 0;


volatile unsigned int 	FlowRecirLiter_x10 = 0;

// Power LED
volatile unsigned int	err_PWled_cnt = 0;

//
volatile unsigned int	nomalPosition_cnt = 0;

//*****************************************************
// Gas Sensor check
//*****************************************************
volatile uint8_t 	err_GasDetect = 0;
volatile uint8_t 	GasInputCount = 0;
volatile unsigned int 	GasSenON_cnt = 0;
volatile uint8_t	GasInputTime_en = 0;
volatile unsigned int 	GasInputTime_cnt = 0;
volatile unsigned long	GasLongTime_cnt = 0;
volatile uint8_t	GasDetect = 0;
volatile uint8_t	GasDetectON = 0;

volatile uint8_t	GasSenDetect_en = 0;
volatile uint8_t	Gasoff_en = 0;
volatile unsigned int 	Gasoff_cnt = 0;

volatile uint8_t	GasRetry_en = 0;
volatile unsigned int 	GasRetry_cnt = 0;

volatile uint8_t	err_GasDetect_en = 0;
volatile unsigned int 	err_GasDetect_cnt = 0;
volatile uint8_t	StandbyPower_cnt = 0;

volatile unsigned int	GasSenOFF_cnt = 0;

//*****************************************************
// Error Check
//*****************************************************
volatile uint8_t		err_state = 0;
volatile uint8_t		Cnt_er64 = 0;
volatile uint8_t		Cnt_er65 = 0;
volatile uint8_t		Cnt_er66 = 0;
volatile uint8_t		Cnt_er67 = 0;
volatile uint8_t		Cnt_er68 = 0;
volatile uint8_t		Cnt_er69 = 0;
volatile uint8_t		Cnt_er82 = 0;
volatile uint8_t		Cnt_er80 = 0;
volatile uint8_t		Cnt_er83 = 0;
volatile uint8_t		Cnt_er81 = 0;
volatile uint8_t		Cnt_er20 = 0;
volatile uint8_t		Cnt_er43 = 0;
volatile uint8_t		CumulativeCnt_er43=0;

volatile uint8_t		Cnt_er42 = 0;
volatile uint8_t		CumulativeCnt_er42=0;

volatile uint8_t		Cnt_er29 = 0;
volatile uint8_t		CumulativeCnt_er29=0;

volatile uint8_t		Cnt_er35 = 0;
volatile uint8_t		Cnt_er30 = 0;
volatile uint8_t		Cnt_er31 = 0;
volatile uint8_t		Cnt_er32 = 0;
volatile uint8_t		Cnt_er33 = 0;
volatile uint8_t		Cnt_er28 = 0;
volatile uint8_t		Cnt_er70 = 0;
volatile uint8_t		Cnt_er94 = 0;
volatile uint8_t		Cnt_er16 = 0;

volatile uint8_t		Cnt_er37 = 0;
volatile uint8_t		Cnt_er40 = 0;
volatile uint8_t		Cnt_er61 = 0;
volatile uint8_t		Cnt_er72 = 0;
volatile uint8_t		Cnt_er10 = 0;
volatile uint8_t		Cnt_er11 = 0;
volatile uint8_t		Cnt_er38 = 0;
volatile uint8_t		Cnt_er41 = 0;
volatile uint8_t		Cnt_er45 = 0;
volatile uint8_t		CumulativeCnt_er45=0;


volatile uint8_t		err_DipSW = 0;
volatile uint8_t		err_DipSW_en = 0;
volatile uint8_t		err_DipSW_cnt = 0;
volatile uint8_t		Cnt_er73 = 0;

volatile uint8_t		Cnt_er78 = 0;

volatile uint8_t		err_CirPump = 0;
volatile uint8_t		Cnt_er60 = 0;

volatile unsigned int		ErrorON_buf = 0;
volatile unsigned int		ErrorOFF_buf = 0;
volatile uint8_t		ErrorOFF_buf_LSB = 0;
volatile uint8_t		ErrorOFF_buf_MSB = 0;

volatile uint8_t		Er72Flame_chk = 0;
volatile uint8_t		err_72Flame = 0;
volatile uint8_t		err_Flash = 0;
volatile uint8_t		err_eeprom_cnt = 0;
volatile uint8_t		err_McuSafety_cnt = 0;
volatile bool			Cnt_er76 = 0;
volatile bool			Cnt_er79 = 0;	// 200623:12h lgh


// PT Setting Value
volatile unsigned int 	IgnitionPWM = 0;
volatile unsigned int 	PrePurgePWM = 0;
volatile unsigned int 	PostPurgePWM = 0;
volatile unsigned int 	IgnitionAGM = 0;
volatile unsigned int 	PrePurgeAGM = 0;
volatile unsigned int 	PostPurgeAGM = 0;

volatile unsigned int 	FanMinPWM = 0;
volatile unsigned int 	FanMaxPWM = 0;

volatile unsigned int 	FanCtrlPwm = 0;
volatile uint8_t 	Fan1mSPwm = 0;
volatile uint8_t 	Fan1mSPwm_en = 0;

volatile unsigned int 	err_MaxFanRPM = 0;
volatile uint8_t	MaxFanRPM_check = 0;
volatile unsigned int	MaxFanRPM_check_cnt = 0;
volatile uint8_t	MaxFanRPM_en = 0;
volatile uint8_t	MaxFanRPM_cnt = 0;
volatile uint8_t 	ErrMaxRPM_cnt = 0;  // nnn
volatile uint8_t 	Er41_Reset_en = 0;
volatile unsigned int 	Er41_Reset_cnt = 0;


volatile uint8_t	FlameExt_en = 0;
volatile uint8_t	ItFault_en = 0;

volatile uint8_t	PostPT = 0;
volatile unsigned int 	PostTime_cnt = 0;

volatile uint8_t	RePostPT = 0;
volatile unsigned int 	RePostTime_cnt = 0;

volatile uint8_t	err_10FlameExt = 0;		// 불꽃 소멸.
volatile uint8_t	FlameExt_4time = 0;
volatile uint8_t	FlameExt_cnt = 0;
volatile uint8_t	ItFault_cnt = 0;
volatile uint8_t	err_11ItFault = 0;		// 점화 실패.
volatile uint8_t	Burner_ON = 0;


// Room Set Check
volatile unsigned int	add0_5 = 0;
volatile unsigned int	CHtargetTemp = 0;
volatile unsigned int	ChBurnOn_DiffTemp = 0;

volatile unsigned int	DHWtargetTemp = 0;

///
volatile unsigned long	DHW_CtrlKcal = 0;
volatile unsigned long	needDHW_CtrlKcal = 0;
volatile unsigned int	DHW_DeltaT = 0;
//
volatile unsigned int	InletCurrTemp = 0;
volatile unsigned int 	OPCurrTemp = 0;
volatile unsigned int 	DHWCurrTemp = 0;
volatile unsigned int 	ExhCurrTemp = 0;

// Cc
volatile uint8_t 	CcCurrPercent = 0;

///
volatile unsigned int 	DHW_MaxTemp = 0;

// OP_TempLimit
volatile uint8_t 	OP_TempLimit = 0;
volatile uint8_t 	OP_TempLimit_en = 0;
volatile uint8_t 	OP_TempLimit_cnt = 0;

// DHW_TempLimit
volatile uint8_t 	DHW_TempLimit = 0;
volatile uint8_t 	DHW_TempLimit_en = 0;
volatile unsigned int 	DHW_TempLimit_cnt = 0;
volatile unsigned int 	DHWonDeltaTemp = 0;
volatile unsigned int 	DHWoffDeltaTemp = 0;
volatile unsigned int 	RecyleonDeltaTemp = 0;
volatile unsigned int 	RecyleoffDeltaTemp = 0;

// Inlet_TempLimit
volatile uint8_t 	Inlet_TempLimit = 0;
volatile uint8_t 	Inlet_TempLimit_en = 0;
volatile uint8_t 	Inlet_TempLimit_cnt = 0;

volatile unsigned int 	CH_MinTemp = 0;


// AntiFreeze
volatile uint8_t	AntiFreeze = 0;
volatile uint8_t	AntiFreeze1_en = 0;
volatile unsigned int	AntiFreeze1_cnt = 0;

volatile uint8_t	AntiFreeze2Protect = 0;
volatile uint8_t	AntiFreeze2_en = 0;
volatile unsigned int	AntiFreeze2_cnt = 0;

//volatile unsigned int	Freeze85Check = 0;
//volatile uint8_t	FreezeCurrError = 0;
volatile uint8_t	FreezeError_ok = 0;	// Error Check And Burn OFF.

// PT Set
volatile uint8_t	PT_enable = 0;
volatile uint8_t	DhwProtect = 0;

//volatile uint8_t	PT_Ok = 0;
volatile bool	BurnState = false;	// PT 진입 시.
volatile bool	bBurnControl = false;	// 연소 진행 시.

//volatile uint8_t	PTcurrMode = 0;
//volatile uint8_t	BackupPTcurrMode = 0;
//volatile uint8_t	PTModeOff = 0;
//
///volatile uint8_t	Er78CheckMsb = 0;
//volatile uint8_t	Er78CheckLsb = 0;
//volatile uint8_t	Er78Check = 0;

// Storage Heat
volatile uint8_t	StorageHeat = 0;
volatile uint8_t	RecirMode = 0;
volatile uint8_t	InOut_cirMode = 0;

//volatile uint8_t	PostPreHeatPump_cnt = 0;

// WriteFlash
volatile uint8_t	WriteFlash_a = 0;
volatile uint8_t	WriteFlash_b = 0;
volatile uint8_t	WriteFlash_c = 0;
volatile uint8_t	WriteFlash_d = 0;
volatile uint8_t	WriteFlash_e = 0;
volatile uint8_t	WriteFlash_f = 0;
volatile uint8_t	WriteFlash_g = 0;
volatile uint8_t	WriteFlash_h = 0;
volatile uint8_t	WriteFlash_i = 0;
volatile uint8_t	WriteFlash_j = 0;
volatile uint8_t	WriteFlash_k = 0;
volatile uint8_t	WriteFlash_l = 0;
volatile uint8_t	WriteFlash_m = 0;
volatile uint8_t	WriteFlash_n = 0;
volatile uint8_t	WriteFlash_o = 0;
volatile uint8_t	WriteFlash_p = 0;
volatile uint8_t	WriteFlash_q = 0;
volatile uint8_t	WriteFlash_r = 0;
volatile uint8_t	WriteFlash_s = 0;
volatile uint8_t	WriteFlash_t = 0;
volatile uint8_t	WriteFlash_u = 0;
volatile uint8_t	WriteFlash_v = 0;
volatile uint8_t	WriteFlash_w = 0;
volatile uint8_t	WriteFlash_x = 0;
volatile uint8_t	WriteFlash_y = 0;
volatile uint8_t	WriteFlash_z = 0;
volatile uint8_t	WriteFlash_1 = 0;
volatile uint8_t	WriteFlash_2 = 0;
volatile uint8_t	WriteFlash_3 = 0;
volatile uint8_t	WriteFlash_4 = 0;
volatile uint8_t	WriteFlash_5 = 0;
volatile uint8_t	WriteFlash_6 = 0;
volatile uint8_t	WriteFlash_7 = 0;
volatile uint8_t	WriteFlash_8 = 0;
volatile uint8_t	WriteFlash_9 = 0;
volatile uint8_t	WriteFlash_10 = 0;
volatile uint8_t	WriteFlash_11 = 0;
volatile uint8_t	WriteFlash_12 = 0;
volatile uint8_t	WriteFlash_13 = 0;
volatile uint8_t	WriteFlash_14 = 0;
volatile uint8_t	WriteFlash_15 = 0;
volatile uint8_t	WriteFlash_16 = 0;
volatile uint8_t	WriteFlash_17 = 0;
volatile uint8_t	WriteFlash_18 = 0;
volatile uint8_t	WriteFlash_19 = 0;
volatile uint8_t	WriteFlash_20 = 0;
volatile uint8_t	WriteFlash_21 = 0;
volatile uint8_t	WriteFlash_22 = 0;
volatile uint8_t	WriteFlash_23 = 0;




volatile uint8_t 	All_Write_en = 0;
volatile uint8_t 	All_Read_en = 0;
volatile uint8_t 	EelFormat_en = 0;


// Cascade CH
volatile unsigned int 	Com_PWMValue = 0;


// UART Transmition
volatile uint8_t	Uart0_en = 0;
volatile uint8_t 	Uart0_RxCnt = 0;
volatile uint8_t 	Uart0_Rx_Data[UARTBYTE] = {0};
volatile uint8_t	Uart0_out_frame[UARTBYTE] = {0};

/*
typedef struct
{
	uint8_t	txData[UARTBYTE];
	uint8_t	rxData[UARTBYTE];
}T_UART_DAT;
T_UART_DAT mstUart0={0,};

*/

volatile uint8_t 	rxCrc_ok = 0;
volatile unsigned int	Uart0_SetTime = 0;

volatile bool		CascadeOn=false;
volatile uint8_t 	CascadeNumber = 0;
volatile uint8_t 	CcTotalNumber = 0;


volatile bool 	Uart0Txend = false;
volatile bool	Uart0Rxend = false;

volatile uint8_t	Uart0Tx_en = 0;
volatile uint8_t	UartTx_enable = 0;
volatile unsigned int	Uart0Tx_cnt = 0;

volatile uint8_t 	ReceiveData0 = 0;
volatile unsigned int 	Uart0Tx_crc_value = 0;


// Uart0 Retry
volatile uint8_t	Uart0Rx_en = 0;
volatile uint8_t	Uart0Rx_cnt = 0;


// Cascade system
volatile uint8_t	mucCcBurn01_08 = 0;
volatile uint8_t	mucCcBurn09_16 = 0;
volatile uint8_t	mucCcBurn17_20 = 0;


volatile bool	DeviceTotalchk = false;


volatile bool	AllError = false;



volatile unsigned int 	MIN_KCAL = 0;
volatile unsigned int 	MAX_KCAL = 0;

// DHW Standby time
volatile uint8_t	Burn_Mode = 0;
volatile uint8_t	Burn_Mode_check_en = 0;
//volatile unsigned int 	DHWStandbytime = 0;
volatile unsigned int 	DHWStandbytime_cnt = 0;

// AH_mode
/*
volatile uint8_t	AH_ChON = 0;
volatile uint8_t	AH_mode = 0;
volatile uint8_t	AHtime_en = 0;
volatile unsigned int	AHtime_1scnt = 0;
volatile unsigned int	AHtime_cnt = 0;

volatile unsigned int	TargetAHtime = 0;
volatile unsigned int	AH_ONtime = 0;
 */

//
//volatile uint8_t	ResMode = 0;
volatile uint8_t	CurrBurnSetMode = 0;
volatile uint8_t	BurnSetMode = 0;


// Title 24
volatile uint8_t 	IntExtTitleInput = 0;
volatile unsigned int 	IntExtTitleInput_cnt = 0;

volatile uint8_t 	IntExt_ON = 0;
volatile uint8_t 	Title24_ON = 0;
volatile unsigned int 	TitleCurrTemp = 0;

volatile uint8_t 	BypassMode = 0;

//////////////////////////////////////////////
// Flow2
volatile unsigned int 	munFlowDhwPulsCnt = 0;	// 직수 유량.
volatile unsigned int 	munFlowRecirPulsCnt = 0;// 환수 유량.


volatile uint8_t	AGM_ZeroPoint = 0;
volatile unsigned int 	AGMzeroLevel_cnt = 0;
volatile uint8_t	err_AGMzerofault = 0;
volatile uint8_t	err_AGMoperfault = 0;
volatile uint8_t	err_AGMStuckfault = 0;
volatile uint8_t	err_AGMfault = 0;
volatile unsigned int	err_AGM_cnt = 0;

volatile uint8_t	AGM_PowerPoint_en = 0;
volatile unsigned int 	AGM_PowerPoint_cnt = 0;

volatile uint8_t 	AGM_Initial_try_en = 0;
volatile unsigned int 	AGM_Initial_try_cnt = 0;
volatile uint8_t 	AGM_NonZero_en = 0;
volatile unsigned int 	AGM_NonZero_cnt = 0;
volatile uint8_t 	AGM_Zero_en = 0;
volatile unsigned int 	AGM_Zero_cnt = 0;

volatile uint8_t	AGM_zero = 0;
volatile uint8_t 	TableChange_en = 0;
volatile unsigned int 	TableChange_cnt = 0;

volatile unsigned int 	Add_PWM = 0;

volatile unsigned int 	PostPumpTime = 0;

volatile unsigned int 	PrepurgDelayTime = 0;
volatile unsigned int 	GVDelayTime = 0;

volatile uint8_t 	err_Er39fault = 0;
volatile uint8_t 	Er39fault_en = 0;
volatile unsigned int 	Er39fault_cnt = 0;
volatile uint8_t	Cnt_er39 = 0;

volatile uint8_t	AntiFreezePostpump = 0;

volatile uint8_t	Err16_state = 0;

volatile uint8_t	MCU_rutin = 0;
volatile uint8_t	MCU_rutin_cnt = 0;
//volatile uint8_t	AsCP1test = 0;


// Water Heater Type
volatile uint8_t 	WaterHeatType = 0;
volatile uint8_t 	KoreaType = 0;

//
volatile uint8_t	AGM_AutoReset_cnt = 0;


// T/T Switch
volatile uint8_t	TTswitch = 0;
volatile unsigned int 	Delta_Flow = 0;

volatile unsigned int 	DHMstepValues = 0;


/******************************************************************************
LGH. Insert Variables.
******************************************************************************/

bool mbDhmInit=false;			// DHM Init.
bool mbAgmInit=false;			// AGM Init.
bool mbInitModleChk=false;



bool mbDisconnectedLine=false;	// Panel Disconnected.

bool mbErr46=false;
bool mbErr44=false;

bool mbTiltel24Run=false;


uint32_t mucDhwOnSetTmr=0;		// 환탕 모드 -> 온수 전환시 딜레이 시간.


volatile bool bCumulativeFlowInit=false;
volatile uint32_t mulCumulativeFlow=0;	// 누적 유량.

extern bool boostPt;



void Pump_Flow_Inspection(void);	// 펌프 순환 검사.(24시간에 한번씩 믹싱밸브랑 같이.)


/******************************************************************************
Private global variables and functions
******************************************************************************/

unsigned int crc16_ccitt(const uint8_t *buf, int len);


void  	System_init(void);
void 	Input_output(void);

void	PT_process(void);
void 	Uart0_Com(void);

void 	Uart0_TxFunc(void);
void 	Uart0_RxFunc(void);

void 	flow_calc(void);
void 	rpm_calc(void);
void 	PID_update(void);

void 	AD_chk(void);
void 	AD2degree(uint8_t AD_no);

void 	DHM_driver(void);

void 	ReadFlash(unsigned int type);
void 	WriteFlash(unsigned int type);
void 	AllWrite(void);
void 	AllRead(void);
void	RoomSetChk(void);

void 	Error_state(void);
void 	ErrorON(uint8_t err_ONcode);
void 	ErrorOFF(uint8_t err_OFFcode);

void 	Fan_Control(unsigned int Fan_Speed);
void  	PT_Set(void);
void  	Pump_control(void);
void  	Pump_Std_Ctrl(void);

void	Ini_Machine_Set(void);
void	Machine_Set(void);

void 	IWM_Chk(void);
void  	IWM_control(void);
void 	IWM_driver(void);


void 	AGM_Fault_Check(void);
void 	AGM_Initial(bool ncp);
void 	Add_PWMfunction(unsigned int inputKcal, unsigned int LowKcal, unsigned int HighKcal, unsigned int LowPWM, unsigned int HighPWM);

extern char stl_RL78_registertest(void);
extern char stl_RL78_registertest_psw(void);
extern char stl_RL78_registertest_stack(void);
extern char stl_RL78_registertest_cs(void);
extern char stl_RL78_registertest_es(void);


void EE2memoryData(unsigned int type);
void setFanAGM(uint16_t cal);

/**********************************************************************************************************************/
/**********************************************************************************************************************/


extern uint8_t Init_Mcu_Crc_Chk(void);
extern void Normal_Mcu_Crc_Chk(uint8_t *err, uint8_t fan, uint32_t cnt1sec);

extern char stl_RL78_registertest(void);
extern char stl_RL78_registertest_psw(void);
extern char stl_RL78_registertest_stack(void);
extern char stl_RL78_registertest_cs(void);
extern char stl_RL78_registertest_es(void);

extern void CC_Master_Ctrl(bool on);
extern volatile unsigned int	Presult_crc;




/* End user code. Do not edit comment generated here */
void R_MAIN_UserInit(void);

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
    R_MAIN_UserInit();
    /* Start user code. Do not edit comment generated here */

   	R_WDT_Restart();

#ifndef _debug_
	// mcu test, flash crc check
	err_McuSafety = Init_Mcu_Crc_Chk();
	NCP_tab[PCheckNum] = Presult_crc;

#endif
    R_ADC_Set_OperationOn();
	R_ADC_Start();

	R_INTC0_Start();		// rpm
	R_INTC9_Start();		// rx data

	R_TAU0_Channel0_Start();	// 40us	+ NCP Tx
	R_TAU0_Channel1_Start();	// 1ms
	R_TAU0_Channel2_Start();	// Fan PWM
	R_TAU0_Channel4_Start();	// DHM, IWM, AGM
	R_TAU0_Channel5_Start();	// 100us + Flow
	R_TAU0_Channel6_Start();	// 100ms + PT
	R_TAU0_Channel7_Start();	// Fan PWM

 	/* Start the UART0 Tx/Rx operation */
	R_UART0_Start();		// external PC communication 19200bps

	KoreaType = 0;	// US.
	//KoreaType = 1;	// KOR.
	System_init();

	DHM_GallPoint = DHM_MAXSTEP;

	FlowOnDelayTmr = 100;	// 부팅 후 유량 감지 10 딜레이.


   	while (1U)
    {
		MCU_rutin = 1;
		R_WDT_Restart();
		AGM_Fault_Check();

		Input_output();
		Machine_Set();

		AD_chk();
		RoomSetChk();
		NCP_A_Com();

		rpm_calc();
		flow_calc();
		Error_state();

		if(gstDebugDat.dhmGallPoint > 0)
			DHM_GallPoint = gstDebugDat.dhmGallPoint;
		else
			DHM_driver();

		if(gstDebugDat.iwmGallPoint > 0)
			IWM_GallPoint = gstDebugDat.iwmGallPoint;
		else
			IWM_control();

		PT_Set();
		PT_process();

		if(WaterHeatType == MODEL_STANDARD)
			Pump_Std_Ctrl();
		else
			Pump_control();

		//
		if(BurnState == true || FanCtrl_Value > 0 || DhwProtect == 1 || out_CP1 == 1/* || out_CP2 == 1 */)
		{
			if(IWM_Refreeze360Min_cnt > 43185) IWM_Refreeze360Min_cnt = 43185;
			if(DHM_Refreeze360Min_cnt > 43185) DHM_Refreeze360Min_cnt = 43185;
			if(mstAgm.tmrRefreeze360min > 43185) mstAgm.tmrRefreeze360min = 43185;

			if(CP1_refreeze_cnt > 43185)
				CP1_refreeze_cnt = 43185;
			else ;
		}
		else ;

		if(mstAgm.tmrRefreeze360min >= 43200)	// 43200 / 2 = 21600, 21600 / 60 = 360sec.
			AGM_Initial(true);
		else ;


		Uart0_Com();
		if(MCU_rutin_cnt >= 50)
		{
			if(CascadeNumber == 0)
			{
				CC_Master_Ctrl(CascadeOn);
				if(CcTotalNumber <= 1)
				{
					NCP_tab[MachineFunc1] &= 0xdfff;
					NCP_tab[MachineFunc2] &= ~0x8000;
					DeviceTotalchk = false;
				}
				else
				{
					NCP_tab[MachineFunc2] |= 0x8000;	// C.C Check?;
					DeviceTotalchk = true;
				}
			}
			else
				;//CC_Slave_Ctrl(CascadeNumber);
		}
		else ;

#ifndef _debug_
		//MCU safety Test
		Normal_Mcu_Crc_Chk(&err_McuSafety, Fan_ON, gul1secCnt);
#endif
	}
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    /* End user code. Do not edit comment generated here */
}


/* ########################################################
	Systemp Init
 ########################################################*/
void  System_init(void)
{
	MCU_rutin = 0;
	MIN_KCAL=5000;
	MAX_KCAL=50000;
	DHWonDeltaTemp = 100;
	DHWoffDeltaTemp = 150;

	P_POWER_LED	= 0;	// POWER LED : Active LOW
	P_RoomTx 	= 0;
	P_GasSenON  	= 1;
	FAN_PWM = FANMAXVALUE;
	Input_output();

	NCP_tab[DHWmax] = 0x3c00;	// 10: 0x0013 (3c = 60 degree)

	FanPwmStop = 60000;
	FanCtrlPwm = 0;
	Er72Flame_chk = 1;

	Uart0_SetTime = 2000;	// Cascade Master: 50ms
	Uart0Rxend = false;

	// Model Check.
	while(mbInitModleChk==false)
	{
		NCP_A_Com();
	}

	//nkwP10
	ReadFlash('14');
	///
	ReadFlash('z');
	NCP_A_init();
	if(IniMemoryCheck == MEMORY_ID)
	{
		Input_output();
		AllRead();

		mulCumulativeFlow = (uint32_t)(NCP_tab[Cumulative_Flow_MSB] << 8);
		mulCumulativeFlow |= NCP_tab[Cumulative_Flow_LSB];
	}
	else
	{
		IniMemoryCheck = MEMORY_ID;
		AllWrite();
	}

	Machine_Set();
	AGM_Initial(false);



	IWM_CurrPoint = 0;
	IWM_GallPoint = DHM_MAXSTEP;
	while(IWM_CurrPoint < 2000)
	{
		R_WDT_Restart();
	}
	IWM_GallPoint = 0;

	//DHM RESET
	DHM_CurrPoint = DHM_MAXSTEP;
	DHM_intial_en = 1;
	DHM_GallPoint = 0;	// Full Close

	while(DHM_intial_cnt < 150) {
		R_WDT_Restart();

		if(DHM_intial_cnt > 40 && P_DHM_HALL == 0) {
			DHM_CurrPoint = 0;
			DHM_GallPoint = 0;
			DHM_intial_cnt = 150;
			DHM_intial_OK = 1;
		}
		else DHM_intial_OK = 0;
		Input_output();
	}

	// DHM_IWMdriver
	P_DHM &= 0xe1;
	DHM_on = 0;

	if(DHM_intial_OK == 0)
		err_DHM_ZeroClose = 1;
	else
		err_DHM_ZeroClose = 0;


	NCP_tab[RoomSet] &= ~0x0040;	// 시운전은 저장 하지 않은 값으로.

	// Reset Function
	NCP_tab[MachineFunc1] 	= 0x0000;	// 7: 0x0010
	DHM_GallPoint = DHM_MAXSTEP;

	if(err_AGMfault == 0) {
		AGMzeroLevel_cnt = 0;
		AGM_NonZero_en = 0;
		AGM_NonZero_cnt = 0;
	}

	if(err_DHM_ZeroClose == 0)
		Pump_Flow_Inspection();
	else ;

}


/* ########################################################
	펌프 동작에 따른 유량 체크.
 ########################################################*/
void Pump_Flow_Inspection(void)
{
	uint8_t temp = 0;
	static uint32_t runningTmr=0;

	runningTmr = gul1msCnt;

	while( ((temp & 0x10) == 0) && (WaterHeatType == MODEL_PREMIUM) && ((gul1msCnt-runningTmr) < 30000) )
	{
		WDTE = 0xACU;
		NCP_A_Com();
		Uart0_Com();
		flow_calc();
		temp = Pump_Check_Flow((uint8_t *)&out_CP1, (uint16_t *)&DHM_GallPoint, DHM_CurrPoint, (uint16_t *)&FlowRecirLiter_x10, NCP_tab[ReCPumpMode], gul1msCnt);
		if( (temp & 0x10) > 0)
		{
			if((temp & 0x01) > 0)
				ErrorON(46);
			else ;

			if((temp & 0x02) > 0)
				ErrorON(44);
			else ;
		}
		else ;
	}
}

/* ########################################################
	Standard Model Pump Control
 ########################################################*/
void  Pump_Std_Ctrl(void)
{
	bool pumpOn=false;
	static uint32_t exOutDelay=0;

	if( (MCU_rutin_cnt < 50) || (Error_ok == 1) || (CascadeOn == ON) )
	{
		out_CP1 = 0;

		P_3WAY = OFF;
		NCP_tab[MachineFunc2] &= 0xfdff;
		exOutDelay = gul1secCnt;

		NCP_tab[ReCPumpMode] &= 0xfff7;
	}
	else if(mstStdRc.Ah.on == true)
	{
		// 200602:8h lgh NCP_tab[CHpump] 0x003a 해당 데이터는 Anti 모드에서 On/Off 시간으로 사용 됨.
		pumpOn = Std_Recir_AirHandler((bool)RoomPW, (P_tt24==false)?1:0, (P_FLOW_RECIR==false)?1:0, &mstStdRc.Ah, NCP_tab[CHpump], gul1secCnt, gul1msCnt);
		//pumpOn = Std_Recir_AirHandler((bool)RoomPW, true, (P_FLOW_RECIR==false)?1:0, &mstStdRc.Ah, NCP_tab[CHpump], gul1secCnt, gul1msCnt);//TEST.
		if(mstStdRc.Ah.std == true)
		{
			if(pumpOn == true)
			{
				if((gul1secCnt-exOutDelay) >= 20)	// T/T 출력은 Pump 출력 후 20초 뒤.
				{
					exOutDelay = gul1secCnt - 20;
					NCP_tab[MachineFunc2] |= 0x0200;
					P_3WAY = ON;
				}
				else ;
			}
			else
			{
				NCP_tab[MachineFunc2] &= 0xfdff;
				P_3WAY = OFF;
				exOutDelay = gul1secCnt;
			}
		}
	}
	else
	{
		exOutDelay = gul1secCnt;

		if(mstStdRc.pulsOn == true)
			pumpOn = Std_Recir_Puls((bool)RoomPW,NCP_tab[RecyleTemp], gul1secCnt);
		else if(mstStdRc.demandOn == true)
			pumpOn = Std_Recir_Ondemand((bool)RoomPW, TTswitch, gul1secCnt);
		else if(mstStdRc.tt24On == true)
		{
			if(CascadeNumber == 0)
				pumpOn = Std_Recir_Title24((bool)RoomPW, (P_tt24==false)?1:0, mstTemp.Inlet, gul1secCnt, gul1msCnt);
			else
				pumpOn = 0;

			if(pumpOn == 1)
				NCP_tab[ReCPumpMode] |= 0x0008;
			else
				NCP_tab[ReCPumpMode] &= 0xfff7;
		}
		else
			pumpOn = false;


	}

	if(pumpOn == true)
	{
		out_CP1 = 1;
		munPumpOffDelay = (Burner_ON == 1)? 50: 0;	// * 100ms.
	}
	else
	{
		if(munPumpOffDelay == 0)
			out_CP1 = 0;
		else ;
	}
}


/* ########################################################
	Premium Model Pump Control
 ########################################################*/
void Pump_control(void)
{
	bool reserUse=false, ptOk=false, runTestCP1=false;
	uint8_t i = 0, temp8 = 0;
	static uint32_t asCpTestTmr=0;

	reserUse = ((NCP_tab[ReCPumpMode] & 0x0100) > 0)? true: false; 			// 예약 ON/OFF.
	runTestCP1 = ((NCP_tab[RoomSet] & 0x0080) > 0)? true: false;				// Pump 시 운전.

	if(runTestCP1 == 1)
		PostPumpTime = 20;
	else if(Title24_ON == 0 && IntExt_ON == 1 && InOut_cirMode == 1)
		PostPumpTime = 6000;
	else
		PostPumpTime = 1800;


	// Pump 1
	if(Error_ok==0 && RoomPW == 1 && StorageHeat == 1)
		ptOk = true;
	else
		ptOk = false;


	if(WaterHeatType == MODEL_PREMIUM)
	{
		uint8_t temp = 0;

		if( (mbErr44 == false) && (mbErr46 == false) )
		{
			if(AntiFreeze > 0)
				temp = Recir_Flow_Chk(1, 1, out_CP1, FlowRecirLiter_x10, (uint16_t *)&DHM_GallPoint, DHM_CurrPoint, (NCP_tab[ReCPumpMode] | 0x0100));
			else
				temp = Recir_Flow_Chk(RoomPW, (flow_op_stat==true)?false: (bRoomRunDemand|mbTiltel24Run), out_CP1, FlowRecirLiter_x10,
									(uint16_t *)&DHM_GallPoint, DHM_CurrPoint, NCP_tab[ReCPumpMode]);
		}
		else
			temp = 0;

		if(temp > 0)
		{
			if((temp & 0x01) > 0)
			{
				if(mbErr46 == false)
					ErrorON(46);
				else ;
				mbErr46 = true;
			}
			else ;

			if((temp & 0x02) > 0)
			{
				if(mbErr44 == false)
					ErrorON(44);
				else ;
				mbErr44 = true;
			}
			else ;
		}
		else
		{
			if(flow_op_stat == true)	// 직수량 발생시 강제 해제.
			{
				if(mbErr46 == true)
				{
					ErrorOFF(46);
					mbErr46 = false;
				}
				else ;

				if(mbErr44 == true)
				{
					ErrorOFF(44);
					mbErr44 = false;
				}
				else ;
			}
		}
	}
	else ;

	///
	if(runTestCP1 == 1)
	{	// Test Mode.
		AntiFreeze1_en = 0;
		//AsCP1_en = 1;

		if(err_HighLevel == 0)
		{
			if((gul1msCnt - asCpTestTmr) < 60000)
			{
				out_CP1 = ON;
				CP1_refreeze_en = 0;
			}
			else if((gul1msCnt - asCpTestTmr) < 70000)
				out_CP1 = OFF;
			else
				TMR_RELOAD(asCpTestTmr, gul1msCnt);
		}
		else
			TMR_RELOAD(asCpTestTmr, gul1msCnt);

	}
	else
	{	// Nomal Mode.
		TMR_RELOAD(asCpTestTmr, gul1msCnt);

		if(AntiFreeze == 1)
		{
			AntiFreeze1_en = 1;
			if(AntiFreeze1_cnt < 6000)	// * 100ms.
			{
				if(err_HighLevel == 0)
				{
					out_CP1 = 1;	// 600s
					CP1_refreeze_en = 0;
				}
				else
					out_CP1 = 0;
			}
			else
			{
				out_CP1 = 0;
				if(AntiFreeze1_cnt >= 6600)
					AntiFreeze1_cnt = 0; //60s
				else ;
			}
		}
		else if(AntiFreeze >= 2)
		{
			AntiFreeze1_en = 0;
			if(err_HighLevel == 0)
			{
				out_CP1 = 1;
				AntiFreeze1_en = 0;
				CP1_refreeze_en = 0;
			}
			else
				out_CP1 = 0;
		}
		else
		{	// Nomal Mode.
			AntiFreeze1_en = 0;
			if(RoomPW == 1 && err_HighLevel == 0)
			{
				static uint32_t pumpOndelay=0;

				if(Get_Recir_Flow_Pump() == true)
					out_CP1 = false;
				else if(Trial_Recir_Chk() == true)
				{
					out_CP1 = Recir_Pump_Chk();
				}
				else if( (BypassMode == true) && (Error_ok == 0) )
				{
					out_CP1 = Recir_CrossOver_Pump(bRoomRunDemand, FlowLiter_x10);
				}
				else if( ( (RecirMode == true) || (Title24_ON == true) ) && (flow_op_stat == false) && (Error_ok == 0) )
				{// External Recir or Title24.
					if( (gul1msCnt - pumpOndelay) >= 5000)	// First On is 5sec Delay.
						out_CP1 = Recir_Pump_Chk();
					else
						out_CP1 = false;
				}
				else
				{
					out_CP1 = false;
					TMR_RELOAD(pumpOndelay, gul1msCnt);
				}
			}
			else
			{
				if(err_HighLevel == 1)
					out_CP1 = 0;
				else
					out_CP1 = false;
			}
		}
	}

	// Pump1 refreeze Function
	if(out_CP1 == 0 || CP1_refreeze == 1)
	{
		CP1_refreeze_en = 1;
		if(CP1_refreeze_cnt > 43200 && CP1_refreeze_cnt < 43215)
			CP1_refreeze = 1;
		else if(CP1_refreeze_cnt >= 43215)
		{
			CP1_refreeze = 0;
			CP1_refreeze_cnt = 0;
		}
	}
	else
	{
		CP1_refreeze_en = 0;
		CP1_refreeze = 0;
	}

}

/* ########################################################
	Room Mode Check
 ########################################################*/
void	RoomSetChk(void)
{
	unsigned int TemperValue = 0;

	//**** Room To be or Not To be  ****//
	if(RoomToBe) 	NCP_tab[MachineFunc1] &= 0xfffe;	// to be
	else	NCP_tab[MachineFunc1] |= 0x0001;		// Not To be

	// Anti Frequency time
/*
	AntiProtectTime = (NCP_tab[Boost_AntiFreqTime] & 0x00ff) * 600;
	if(WaterHeatType == MODEL_PREMIUM) {
		if(TitleInput == 1) {
			if(AntiProtect_cnt >= 300) {
				AntiFreq_ok = 0;
				titleAntiburn = 0;
			}
		}
		else {
			if(AntiProtect_cnt >= AntiProtectTime) {
				AntiFreq_ok = 0;
				Antiburn = 0;
			}
		}

		if(flow_op_stat == 0 && (RecirMode == 1 && StorageHeat == 0 && Antiburn == 1) || (TitleInput == 1 && StorageHeat == 0 && titleAntiburn == 1))
			AntiFreq_ok = 1;
		else ;
	}
 */

	// Recirculation Pump Function is used or unused
	TemperValue = NCP_tab[ReCPumpMode] & 0x0103;
	if(WaterHeatType == MODEL_PREMIUM)
	{
		if(TemperValue == 0x0101) {  // External circulation Mode
			BypassMode = 0;
			IntExt_ON = 1;
			Title24_ON = 0;
			RecirMode = 1;
			InOut_cirMode = 1;
		}
		else if(TemperValue == 0x0100) {  // Internal circulation Mode
			BypassMode = 0;
			IntExt_ON = 1;
			Title24_ON = 0;
			RecirMode = 1;
			InOut_cirMode = 0;
		}
		else if(TemperValue == 0x0102) {   // Title 24 Mode
			BypassMode = 0;
			IntExt_ON = 0;
			Title24_ON = 1;
			RecirMode = 0;
			InOut_cirMode = 0;
		}
		else if(TemperValue == 0x0103) {  // Bypass Mode
			BypassMode = 1;
			IntExt_ON = 1;
			Title24_ON = 0;
			RecirMode = 1;
			InOut_cirMode = 0;
		}
		else {				   // OFF Mode
			BypassMode = 0;
			IntExt_ON = 0;
			Title24_ON = 0;
			RecirMode = 0;
			InOut_cirMode = 0;
		}

		if( (InOut_cirMode == true) || (Title24_ON == true) || (Trial_Recir_Chk() == true) )
			Recir_Ex_PumpTmr_Set(NCP_tab[Boost_AntiFreqTime]);
		else ;

	}
	else if(TemperValue == 0x0102)   // 191015:11h lgh Standard Title 24 Mode.
	{
		memset(&mstStdRc, 0, sizeof(mstStdRc));
		mstStdRc.tt24On = true;
		Title24_ON = 1;
	}
	else if((NCP_tab[ReCPumpMode] & 0x0140) == 0x0140)	// Standard Puls Mode.
	{
		memset(&mstStdRc, 0, sizeof(mstStdRc));
		Title24_ON = 0;
		mstStdRc.pulsOn = true;
		mstStdRc.set = true;
	}
	else if((NCP_tab[ReCPumpMode] & 0x0104) == 0x0104)	// Standard On Demand Mode.
	{
		memset(&mstStdRc, 0, sizeof(mstStdRc));
		Title24_ON = 0;
		mstStdRc.demandOn = true;
		mstStdRc.set = true;
	}
	else if((NCP_tab[AhRc_Mode] & 0x000f) != 0)			// Standard Air Handler Mode.
	{
		uint16_t tmp=0;

		memset(&mstStdRc, 0, sizeof(mstStdRc));
		Title24_ON = 0;
		mstStdRc.Ah.on = true;
		tmp = NCP_tab[AhRc_Mode] & 0x000f;
		if(tmp == 0x01)
			mstStdRc.Ah.eco = true;
		else if(tmp == 0x02)
			mstStdRc.Ah.std = true;
		else if(tmp == 0x03)
			mstStdRc.Ah.antiB = true;
		else if(tmp == 0x04)
			mstStdRc.Ah.antiU = true;
		else ;

	}
	else
	{
		memset(&mstStdRc, 0, sizeof(mstStdRc));
		BypassMode = 0;
		IntExt_ON = 0;
		Title24_ON = 0;
		RecirMode = 0;
		InOut_cirMode = 0;
	}

	// CH MIN Temperature Set
	add0_5 = NCP_tab[CHmin] & 0x00ff;
	CH_MinTemp = (NCP_tab[CHmin] >> 8) * 10;
	if(add0_5 > 0) CH_MinTemp += 5;

	// DHW MAX Temperature Set
	add0_5 = NCP_tab[DHWmax] & 0x00ff;
	DHW_MaxTemp = (NCP_tab[DHWmax] >> 8) * 10;
	if(add0_5 > 0) DHW_MaxTemp += 5;

	// CH target temperature Set
	add0_5 = NCP_tab[Chset] & 0x00ff;
	CHtargetTemp = (NCP_tab[Chset] >> 8) * 10;
	if(add0_5 > 0) CHtargetTemp += 5;

	// Return temperature
	add0_5 = NCP_tab[InletTemp] & 0x00ff;
	InletCurrTemp = (NCP_tab[InletTemp] >> 8) * 10;
	if(add0_5 > 0) InletCurrTemp += 5;

	// Title Temperature
	add0_5 = NCP_tab[TitleTemp] & 0x00ff;
	TitleCurrTemp = (NCP_tab[TitleTemp] >> 8) * 10;
	if(add0_5 > 0) TitleCurrTemp += 5;

	// DHW temperature Set
	add0_5 = NCP_tab[Dhwset] & 0x00ff;
	DHWtargetTemp = (NCP_tab[Dhwset] >> 8) * 10;
	if(add0_5 > 0) DHWtargetTemp += 5;

	// DHW current Temperature
	add0_5 = NCP_tab[DhwTemp] & 0x00ff;
	DHWCurrTemp = (NCP_tab[DhwTemp] >> 8) * 10;
	if(add0_5 > 0) DHWCurrTemp += 5;

	// Exhaust Temperature
	add0_5 = NCP_tab[ExhaustTemp] & 0x00ff;
	ExhCurrTemp = (NCP_tab[ExhaustTemp] >> 8) * 10;
	if(add0_5 > 0) ExhCurrTemp += 5;

	// ChBurnOn_Diff Temperature
	add0_5 = NCP_tab[ChBurnOn_DeltaTemp] & 0x00ff;
	ChBurnOn_DiffTemp = (NCP_tab[ChBurnOn_DeltaTemp] >> 8) * 10;
	if(add0_5 > 0) ChBurnOn_DiffTemp += 5;

	// Sensor OP Temperature
	add0_5 = NCP_tab[OPTemp] & 0x00ff;
	OPCurrTemp = (NCP_tab[OPTemp] >> 8) * 10;
	if(add0_5 > 0) OPCurrTemp += 5;

	///
	if(mstErrExhaust.occur == 0) {
		//2 CPVC/PVC
		if(WaterHeatType == MODEL_STANDARD && (NCP_tab[VentType] & 0x0003) > 0) {
			if(ExhCurrTemp >= EXH_CPVC_MAXTEMP) {
				mstErrExOverHeat.run = 1;
			}
			else if(ExhCurrTemp <= EXH_CPVC_RELEASE_TEMP) {
				mstErrExOverHeat.run = 0;
				mstErrExOverHeat.reset = 0;
			}
		}
		else {
			if(ExhCurrTemp >= EXH_PVC_MAXTEMP) {
				mstErrExOverHeat.run = 1;
			}
			else if(ExhCurrTemp <= EXH_PVC_RELEASE_TEMP) {
				mstErrExOverHeat.run = 0;
				mstErrExOverHeat.reset = 0;
			}
		}

		if(mstErrExOverHeat.cnt > 30) {
			mstErrExOverHeat.reset = 1;
		}
	}
	else {
		mstErrExOverHeat.run = 0;
		mstErrExOverHeat.reset = 0;
	}

	//2 Sensor Over Heat Temperature Error Check
	if(mstErrOp.occur == 0)
	{
		uint8_t addTmrSet=(mstTemp.fallOp==true)? 50: 0;

		if(OPCurrTemp >= OVERHEAT_TEMP)
		{
			mstErrSensorOverHeat.run = 1;
		}
		else if(OPCurrTemp <= OVERHEAT_RELEASE_TEMP) {
			mstErrSensorOverHeat.run = 0;
			mstErrSensorOverHeat.cnt = 0;
		}

		if(mstErrSensorOverHeat.cnt >= 200+addTmrSet)
			mstErrSensorOverHeat.occur = 1;
		else
			mstErrSensorOverHeat.occur = 0;
	}
	else {
		mstErrSensorOverHeat.run = 0;
		mstErrSensorOverHeat.occur = 0;
	}

	//2 Anti Freeze Control.
	if( (WaterHeatType == MODEL_PREMIUM) && (flow_op_stat == false) )
		AntiFreeze = Anti_Freeze_Ctrl((mstErrOp.occur == 0)? OPCurrTemp: DHWCurrTemp, flow_op_stat, gul1msCnt);
	else
		AntiFreeze = 0;

	// Anti-Freeze Mode
	if(AntiFreeze == 0)
		NCP_tab[MachineFunc1] &= 0xfbff;
	else if(AntiFreeze >= 1)
		NCP_tab[MachineFunc1] |= 0x0400;
	else ;




}


/* ########################################################
	Machine Burner Set
 ########################################################*/
void Machine_Set(void)
{
	static bool bModelSelErr=false;

	HighAltitudeMode = (uint8_t)NCP_tab[HighAltitude];

	switch (mstDip.kcalInfo) {
	  case 5:	// 150,000BTU (ON/OFF/OFF), if(mstDip.sw.d4 == 1): Vent 3inch
		bModelSelErr = 1;
	   	if(mstDip.sw.d5 == 1) { // LNG
			if(HighAltitudeMode == 4) {	// 10000
				PrePurgePWM = 140;
				PrePurgeAGM = 500;
				IgnitionPWM = 140;
				IgnitionAGM = 500;
			}
			else if(HighAltitudeMode == 2) {	// 8000
				PrePurgePWM = 130;
				PrePurgeAGM = 500;
				IgnitionPWM = 130;
				IgnitionAGM = 500;
			}
			else if(HighAltitudeMode == 1)	{// 5000
				PrePurgePWM = 130;
				PrePurgeAGM = 500;
				IgnitionPWM = 130;
				IgnitionAGM = 500;
			}
			else {	// 0
				PrePurgePWM = 120;
				PrePurgeAGM = 900;
				IgnitionPWM = 120;
				IgnitionAGM = 900;
			}
		}
		else {	// LPG
			if(HighAltitudeMode == 4) {	// 10000
				PrePurgePWM = 180;
				PrePurgeAGM = 1000;
				IgnitionPWM = 180;
				IgnitionAGM = 1000;
			}
			else if(HighAltitudeMode == 2)	{ // 8000
				PrePurgePWM = 170;
				PrePurgeAGM = 1000;
				IgnitionPWM = 170;
				IgnitionAGM = 1000;
			}
			else if(HighAltitudeMode == 1)	{ // 5000
				PrePurgePWM = 160;
				PrePurgeAGM = 1000;
				IgnitionPWM = 160;
				IgnitionAGM = 1000;
			}
			else {	// 0
				PrePurgePWM = 150;
				PrePurgeAGM = 1000;
				IgnitionPWM = 150;
				IgnitionAGM = 1000;
			}
		}
		break;
	   case 7: case 8:  // 8	// 199,000BTU(49,000kcal) (ON/ON/ON), if(mstDip.sw.d4 == 1): Vent 3inch
		bModelSelErr = 0;
	   	if(mstDip.sw.d5 == 1)
		{ // LNG
			if(HighAltitudeMode == 4)
			{	// 10000
				PrePurgePWM = 110;
				PrePurgeAGM = 1100;
				IgnitionPWM = PrePurgePWM;
				IgnitionAGM = PrePurgeAGM;
			}
			else if(HighAltitudeMode == 2)
			{	// 8000
				PrePurgePWM = 110;
				PrePurgeAGM = 1100;
				IgnitionPWM = PrePurgePWM;
				IgnitionAGM = PrePurgeAGM;
			}
			else if(HighAltitudeMode == 1)
			{// 5000
				PrePurgePWM = 110;
				PrePurgeAGM = 1100;
				IgnitionPWM = PrePurgePWM;
				IgnitionAGM = PrePurgeAGM;
			}
			else
			{	// 0
				PrePurgePWM = 80;	// 190926:16h lgh 110 -> 80 변경.
				PrePurgeAGM = 1000;
				IgnitionPWM = PrePurgePWM;
				IgnitionAGM = PrePurgeAGM;
			}
		}
		else {	// LPG
			if(HighAltitudeMode == 4)
			{	// 10000
				PrePurgePWM = 130;
				PrePurgeAGM = 900;
				IgnitionPWM = PrePurgePWM;
				IgnitionAGM = PrePurgeAGM;
			}
			else if(HighAltitudeMode == 2)
			{ // 8000
				PrePurgePWM = 130;
				PrePurgeAGM = 900;
				IgnitionPWM = PrePurgePWM;
				IgnitionAGM = PrePurgeAGM;
			}
			else if(HighAltitudeMode == 1)
			{ // 5000
				PrePurgePWM = 130;
				PrePurgeAGM = 900;
				IgnitionPWM = PrePurgePWM;
				IgnitionAGM = PrePurgeAGM;
			}
			else {	// 0
				PrePurgePWM = 130;
				PrePurgeAGM = 700;
				IgnitionPWM = PrePurgePWM;
				IgnitionAGM = PrePurgeAGM;
			}
		}
		break;
	   default :	// Unuse
	   	bModelSelErr = 1;
		break;
	}

	PostPurgePWM = 120;//200;	// 190816:9h lgh  200->120
	PostPurgeAGM = 0;

	//3 Debug Setting Value.------------
	if(gstDebugDat.prePurgePWM > 0)
		PrePurgePWM = gstDebugDat.prePurgePWM;
	else ;

	if(gstDebugDat.prePurgeAGM > 0)
		PrePurgeAGM = gstDebugDat.prePurgeAGM;
	else ;

	if(gstDebugDat.ignitionPWM > 0)
		IgnitionPWM = gstDebugDat.ignitionPWM;
	else ;

	if(gstDebugDat.ignitionAGM > 0)
		IgnitionAGM = gstDebugDat.ignitionAGM;
	else ;
	//3 -------------


	// Dip Switch Error
	if(MCU_rutin_cnt >= 50)
	{
		static bool bTestDipErr=false;

		bTestDipErr = ((mstDip.sw.d6 == 1) && (mstDip.sw.d7 == 1))? 1: 0;

		if( (bModelSelErr == 1) || (bTestDipErr == 1) )
		{
			err_DipSW_en = 1;
			if(err_DipSW_cnt >= 50) err_DipSW = 1;
		}
		else
		{
			err_DipSW_en = 0;
			err_DipSW = 0;
		}
	}
}


/* ########################################################
	Input & Output
 ########################################################*/
void Input_output(void)
{
	static uint32_t cumulativeTmr=0;

	if(in_IntervalTime >= 10)	// * 1ms.
	{
		uint8_t dipStepCnt=0;
		static uint8_t limitHigh=0, limitCntHightOn=0, limitCntHightOff=0;
		static uint8_t limitBurn=0, limitCntBurnOn=0, limitCntBurnOff=0;
		static uint8_t limitEqs=0, limitCntEqsOn=0, limitCntEqsOff=0;
		static uint8_t limitAps=0, limitCntApsOn=0, limitCntApsOff=0;
		static uint16_t dipOnCnt[8]={0,}, dipOffCnt[8]={0,};

		in_IntervalTime = 0;

		//****************************************************************
		// Input High/Low Variable
		//****************************************************************

		// 1. High Limit
		Input_Limit_Check((P_HL==0)?0:1, (uint8_t *)&limitHigh, (uint8_t *)&limitCntHightOn, (uint8_t *)&limitCntHightOff);

		// 2. Burn Limit
		Input_Limit_Check((P_BurnL==0)?0:1, (uint8_t *)&limitBurn, (uint8_t *)&limitCntBurnOn, (uint8_t *)&limitCntBurnOff);

		// 3. EarthQuake Limit(배기 APS Error)
		Input_Limit_Check((P_EqsL==0)?0:1, (uint8_t *)&limitEqs, (uint8_t *)&limitCntEqsOn, (uint8_t *)&limitCntEqsOff);

		// 4. APS
		Input_Limit_Check((P_ApsL==0)?0:1, (uint8_t *)&limitAps, (uint8_t *)&limitCntApsOn, (uint8_t *)&limitCntApsOff);

		//  Input Error Check:
		//  1. High Limit 2. Burn Limit 3. EarthQuake Limit 4. APS
		//if(in_HighLimit==1 && in_BurnLimit==1 && in_APS==1) {
		if(limitHigh==1 && limitBurn==1 && limitAps==1) {
			mstHighLimit.run = 1;
			mstBurnLimit.run = 0;
			mstAps.run = 0;
		}
		else
			mstHighLimit.run = 0;

		//if(in_HighLimit==0 && in_BurnLimit==1 && in_APS==1) {
		if(limitHigh==0 && limitBurn==1 && limitAps==1) {
			mstHighLimit.run = 0;
			mstBurnLimit.run = 1;
			mstAps.run = 0;
		}
		else
			mstBurnLimit.run = 0;

		//if(in_HighLimit==0 && in_BurnLimit==0 && in_APS==1) {
		if(limitHigh==0 && limitBurn==0 && limitAps==1) {
			mstHighLimit.run = 0;
			mstBurnLimit.run = 0;
			mstAps.run = 1;
		}
		else
			mstAps.run = 0;

		// Er.42 is set when Fan is only ON.
		if(FanCtrl_Value >= 10)
 			mstEqs.run = (limitEqs==1)? 1: 0;
		else if(limitEqs == 0)
			mstEqs.run = 0;
		else ;

		if(mstHighLimit.cnt >= 20) mstHighLimit.occur = 1;
		else mstHighLimit.occur = 0;

		if(mstBurnLimit.cnt >= 20) mstBurnLimit.occur = 1;
		else mstBurnLimit.occur = 0;

		if(mstEqs.cnt >= 100) mstEqs.occur = 1;	// 200525:9h lgh 2sec -> 10sec.
		else mstEqs.occur = 0;

		if(mstAps.cnt >= 100) mstAps.occur = 1;	// 200525:9h lgh 2sec -> 10sec.
		else mstAps.occur = 0;

		// Heat Demand
		if( (CascadeNumber == 0) && (Title24_ON == false) )	// 191015:11h lgh Title24 Chek Insert.
		{
			static uint16_t inCntHdOn=0, inCntHdOff=0;

			if(Input_Port_Check((P_tt24==0)?0:1, 10, (uint16_t *)&inCntHdOn, (uint16_t *)&inCntHdOff) == true)
			{
				TTswitch = 1;
				NCP_tab[MachineFunc1] |= 0x0002;
			}
			else
			{
				TTswitch = 0;
				NCP_tab[MachineFunc1] &= 0xfffd;
			}
		}
		else
			TTswitch = 0;

		// Dip Switch 1
		if(Input_Port_Check((P_DIP1==0)?0:1, 300, (uint16_t *)&dipOnCnt[dipStepCnt], (uint16_t *)&dipOffCnt[dipStepCnt++]) == true)
		{
			mstDip.sw.d1 = 1;
			NCP_tab[MachineMode] |= 0x0100;
		}
		else
		{
			mstDip.sw.d1 = 0;
			NCP_tab[MachineMode] &= 0xfeff;
		}

		// Dip Switch 2
		if(Input_Port_Check((P_DIP2==0)?0:1, 300, (uint16_t *)&dipOnCnt[dipStepCnt], (uint16_t *)&dipOffCnt[dipStepCnt++]) == true)
		{
			mstDip.sw.d2 = 1;
			NCP_tab[MachineMode] |= 0x0200;
		}
		else
		{
			mstDip.sw.d2 = 0;
			NCP_tab[MachineMode] &= 0xfdff;
		}

		// Dip Switch 3
	#ifndef _debug_
		if(Input_Port_Check((P_DIP3==0)?0:1, 300, (uint16_t *)&dipOnCnt[dipStepCnt], (uint16_t *)&dipOffCnt[dipStepCnt]) == true)
	#else
		if(1)
	#endif /* _debug_ */
		{
			mstDip.sw.d3 = 1;
			NCP_tab[MachineMode] |= 0x0400;
		}
		else
		{
			mstDip.sw.d3 = 0;
			NCP_tab[MachineMode] &= 0xfbff;
		}
		dipStepCnt++;

		// Dip Switch 4
		if(Input_Port_Check((P_DIP4==0)?0:1, 300, (uint16_t *)&dipOnCnt[dipStepCnt], (uint16_t *)&dipOffCnt[dipStepCnt]) == true)
		{
			mstDip.sw.d4 = 1;
			NCP_tab[MachineMode] |= 0x0800;
		}
		else
		{
			mstDip.sw.d4 = 0;
			NCP_tab[MachineMode] &= 0xf7ff;
		}
		dipStepCnt++;

		// Dip Switch 5
		if(Input_Port_Check((P_DIP5==0)?0:1, 300, (uint16_t *)&dipOnCnt[dipStepCnt], (uint16_t *)&dipOffCnt[dipStepCnt]) == true)
		{
			NCP_tab[MachineMode] |= 0x1010;
			mstDip.sw.d5 = 1;
		}
		else
		{
			NCP_tab[MachineMode] &= 0xefef;
			mstDip.sw.d5 = 0;
		}
		dipStepCnt++;

		// Dip Switch 6
		if(Input_Port_Check((P_DIP6==0)?0:1, 300, (uint16_t *)&dipOnCnt[dipStepCnt], (uint16_t *)&dipOffCnt[dipStepCnt]) == true)
		{
			mstDip.sw.d6 = 1;
			NCP_tab[MachineMode] |= 0x2002;
		}
		else
		{
			mstDip.sw.d6 = 0;
			NCP_tab[MachineMode] &= 0xdffd;
		}
		dipStepCnt++;

		// Dip Switch 7
		if(Input_Port_Check((P_DIP7==0)?0:1, 300, (uint16_t *)&dipOnCnt[dipStepCnt], (uint16_t *)&dipOffCnt[dipStepCnt]) == true)
		{
			mstDip.sw.d7 = 1;
			NCP_tab[MachineMode] |= 0x4001;
		}
		else
		{
			mstDip.sw.d7 = 0;
			NCP_tab[MachineMode] &= 0xbffe;
		}

		//********************************************
		//  Input Error Check and setting Check:
		//  DIP Switch 1 ~ 3 Machine Check
		//********************************************

		if(mstDip.sw.d1==0 && mstDip.sw.d2==0 && mstDip.sw.d3==1) // 20,000kcal
		{
			mstDip.kcalInfo = 2;
			NCP_tab[BoilerKcal] = 200;		// Unit: 100Kcal
			NCP_tab[MachineMode] &= 0xf8ff;
			NCP_tab[MachineMode] |= 0x0400;		// 001
		}
		else if(mstDip.sw.d1==0 && mstDip.sw.d2==1 && mstDip.sw.d3==0) 	// 25,000kcal
		{
			mstDip.kcalInfo = 3;
			NCP_tab[BoilerKcal] = 250;		// Unit: 100Kcal
			NCP_tab[MachineMode] &= 0xf8ff;
			NCP_tab[MachineMode] |= 0x0200;		// 010
		}
		else if(mstDip.sw.d1==0 && mstDip.sw.d2==1 && mstDip.sw.d3==1) 	// 30,000kcal
		{
			mstDip.kcalInfo = 4;
			NCP_tab[BoilerKcal] = 300;		// Unit: 100Kcal
			NCP_tab[MachineMode] &= 0xf8ff;
			NCP_tab[MachineMode] |= 0x0600;		// 011
		}
		else if(mstDip.sw.d1==1 && mstDip.sw.d2==0 && mstDip.sw.d3==0) 	// 35,000kcal
		{
			mstDip.kcalInfo = 5;
			NCP_tab[BoilerKcal] = 350;		// Unit: 100Kcal
			NCP_tab[MachineMode] &= 0xf8ff;
			NCP_tab[MachineMode] |= 0x0100;		// 100
		}
		else if(mstDip.sw.d1==1 && mstDip.sw.d2==0 && mstDip.sw.d3==1) 	// 40,000kcal
		{
			mstDip.kcalInfo = 6;
			NCP_tab[BoilerKcal] = 400;		// Unit: 100Kcal
			NCP_tab[MachineMode] &= 0xf8ff;
			NCP_tab[MachineMode] |= 0x0500;		// 101
		}
		else if(mstDip.sw.d1==1 && mstDip.sw.d2==1 && mstDip.sw.d3==0) 	// 47,000kcal
		{
			//mstDip.kcalInfo = 7;
			//NCP_tab[BoilerKcal] = 470;		// Unit: 100Kcal
			//NCP_tab[MachineMode] &= 0xf8ff;
			//NCP_tab[MachineMode] |= 0x0300;		// 110
			mstDip.kcalInfo = 8;
			NCP_tab[BoilerKcal] = 500;		// Unit: 100Kcal
			NCP_tab[MachineMode] &= 0xf8ff;
			NCP_tab[MachineMode] |= 0x0700;		// 111
		}
		else if(mstDip.sw.d1==1 && mstDip.sw.d2==1 && mstDip.sw.d3==1) 	// 50,000kcal(199,000Btu)
		{
			mstDip.kcalInfo = 8;
			NCP_tab[BoilerKcal] = 500;		// Unit: 100Kcal
			NCP_tab[MachineMode] &= 0xf8ff;
			NCP_tab[MachineMode] |= 0x0700;		// 111
		}
		else {
			mstDip.kcalInfo = 1;
			NCP_tab[BoilerKcal] = 160;		// Unit: 100Kcal
			NCP_tab[MachineMode] &= 0xf8ff;
			NCP_tab[MachineMode] |= 0x0000;		// 000
		}
	}
	else ;

	// Gas Sensor Check
	if(Fan_ON == 1)
	{
		P_GasSenON = 1;
		StandbyPower_cnt = 0;
	}
	else
	{
		if(StandbyPower_cnt >= 30)
			P_GasSenON = 0;
	}

	if(GasSenDetect_en == 1)
	{
		if(P_GasSenInput == 1 && GasDetectON == 0)
		{
			GasRetry_en = 0;
			GasInputTime_en = 1;
			if(GasInputTime_cnt >= 10)
			{
				GasInputCount++;
				if(GasInputCount >= 3) { 	// three times
					err_GasDetect = 1;
					GasInputCount = 0;
				}

				GasDetect = 1;
				GasDetectON = 1;
			}

			if(GasLongTime_cnt >= 6000) {	// 600s
				err_GasDetect = 1;
				GasInputTime_en = 0;
				GasInputCount = 0;
			}
		}
		else
		{
			GasInputTime_en = 0;
			GasDetect = 0;
			GasRetry_en = 1;
			if(GasRetry_cnt >= 3000) GasDetectON = 0;	// 5min.
		}
	}
	else {
		GasInputTime_en = 0;
		GasDetect = 0;
	}

	if(GasDetect == 0) {
		Gasoff_en = 1;
		if(Gasoff_cnt >= 36000) GasInputCount = 0;
	}
	else {Gasoff_en = 0; Gasoff_cnt = 0;}

	//****************************************************************
	// Output High/Low Variable
	//****************************************************************

	if(out_IT)
	{
		P_IT = 1;
		NCP_tab[MachineFunc1] |= 0x0020;
	}
	else
	{
		P_IT = 0;
		NCP_tab[MachineFunc1] &= 0xffdf;
	}

	if(WaterHeatType == MODEL_PREMIUM)
	{
		if(err_CirPump == 1 || out_CP1 == 1 || CP1_refreeze == 1)
			NCP_tab[MachineFunc1] |= 0x0008;
		else
			NCP_tab[MachineFunc1] &= 0xfff7;
	}
	else
	{
		err_CirPump = 0;
		CP1_refreeze = 0;

		if(out_CP1 == 1) NCP_tab[MachineFunc1] |= 0x0008;
		else	NCP_tab[MachineFunc1] &= 0xfff7;
	}

//	if(out_CP2 == 1 || CP2_refreeze == 1) NCP_tab[MachineFunc2] |= 0x0100;
//	else	NCP_tab[MachineFunc2] &= 0xfeff;

//	if(out_3Way) 	NCP_tab[MachineFunc2] |= 0x0200;
//	else	NCP_tab[MachineFunc2] &= 0xfdff;

	if(NCP_tab[CurrError] > 0 || GasDetectON == 1 || OP_TempLimit==1 || mstErrExOverHeat.reset == 1) {
		err_state = 1;
		if(err_PWled_cnt >= 500) {
			err_PWled_cnt = 0;
			P_POWER_LED = ~P_POWER_LED;
		}
	}
	else {
		err_state = 0;
		P_POWER_LED = 0;
	}

	// Water Level circuit error Detector
	if(WLevel_mS_Cnt >= 100 && WLevel_mS_Cnt <= 800) {
		if(P_HighLevel == 0)  chk_HIGHcircuit = 1; 	// Check to High circuit
		else chk_HIGHcircuit = 0;
	}

	if(RecoverHIGHcircuit_cnt >= 800) {
		err_HIGHcircuit = 0;
		RecoverHIGHcircuit_cnt = 0;
	}

	if(RecoverLOWcircuit_cnt >= 800) {
		err_LOWcircuit = 0;
		RecoverLOWcircuit_cnt = 0;
	}

	// Water Level Detector
	if(WLevel_mS_Cnt >= 940 && WLevel_mS_Cnt <= 960) {
		if(P_HighLevel == 0)  HighLevel = 0;	// HighLevel '0' is normal
		else  HighLevel = 1;			// HighLevel '1' is abnormal
	}

	// Detect High Level
	if(err_HighLevel_cnt >= 4) { // more than 4s
		err_HighLevel = 0;
		err_HighLevel_cnt = 0;
		err_RecoverHighLevel_cnt = 0;
	}

	// Recover High Level
	if(err_RecoverHighLevel_cnt >= 4)
	{
		if(MCU_rutin_cnt >= 50) {
			err_HighLevel = 1;
		}
		err_HighLevel_cnt = 0;
		err_RecoverHighLevel_cnt = 0;
	}

	Temp_Flow_Avg();
	//3 누적 유량 계산 및 저장.............
	if((gul1secCnt-cumulativeTmr) >= 10)
	{// 10sec.
		static bool saveRun=false, fristBit=false;
		static uint8_t saveCnt1=0, saveCnt2=0;
		static float cumulativeFlow=0;

		cumulativeTmr = gul1secCnt;

		if(bCumulativeFlowInit == true)
		{
			bCumulativeFlowInit = false;
			cumulativeFlow = 0;
			mulCumulativeFlow = 0;
			saveRun = true;
		}
		else ;

		if(fristBit == false)
		{
			fristBit = true;
			cumulativeFlow = (float)mulCumulativeFlow;
		}
		else ;

		cumulativeFlow = cumulativeFlow + mstTemp.avgFlow / 6;
		mulCumulativeFlow = (uint32_t)cumulativeFlow;

		if(mstTemp.avgFlow > 1)
		{
			saveCnt1 = 2;
			if(saveCnt2++ >= (6*20))	//  20min.
				saveRun = true;
			else ;
		}
		else if(saveCnt1 > 0)
		{// 20sec.
			saveCnt1 --;
			if(saveCnt1 == 0)
				saveRun = true;
			else ;
		}
		else ;

		if(saveRun == true)
		{
			saveRun = false;

			saveCnt2 = 0;

			NCP_tab[Cumulative_Flow_MSB] = (unsigned int)((mulCumulativeFlow & 0xffff0000) >> 16);
			NCP_tab[Cumulative_Flow_LSB] = (unsigned int)(mulCumulativeFlow & 0x0000ffff);
			WriteFlash_n = 1;
			WriteFlash_o = 1;
		}
		else ;

	}
	else ;
	//3 ..........................
}


/* ########################################################
	PT_Settting
 ########################################################*/
void  PT_Set(void)
{
	unsigned int temp16 = 0;
	unsigned int tempA = 0;
	unsigned long tempB = 0;
	static uint32_t recirOnDelayTmr=0;				// 온수 모드에서 환탕 모드 진입시 딜레이.
	static uint32_t dhwOnDelayTmr=0;				// 환탕 모드에서 온수 모드 진입시 딜레이.

	// Common Vent Set
	if(CcTotalNumber <= 1)
	{
		AllError = false;
		munCcComVentPwm = 0;
		munFollowComVentPWM = 0;
	}
	else
	{
		if(CascadeNumber == 0)
		{

			if(AllError == true)
				munCcComVentPwm = 0;
			else
			{
				if(CCm_Get_MasterNum() == 0)
				{
					if( (Burner_ON == 1) && (PostPT == 0) )
						munCcComVentPwm = FanCtrl_Value;
					else
						munCcComVentPwm = 0;
				}
				else
					munCcComVentPwm = munFollowComVentPWM;
			}

			if(munCcComVentPwm < 10)
				munCcComVentPwm = 0;
			else ;
		}
		else ;
	}

	// MAX_KCAL  MIN_KCAL  DHW_CtrlKcal
	NCP_tab[PWMValue] &= 0xff00;
	NCP_tab[PWMValue] |= Burn_Rate(FanCtrl_Value, out_GV, DHW_CtrlKcal, MAX_KCAL, MIN_KCAL);

	// Vent Type 에 따른 설정 최고 값.
	if(WaterHeatType == MODEL_STANDARD && (NCP_tab[VentType] & 0x0003) > 0)
		NCP_tab[DHWmax] = 0x5400;	// 84c.
	else
		NCP_tab[DHWmax] = 0x3c00;	// 60c.

	if(NCP_tab[Dhwset] > NCP_tab[DHWmax])
	{
		NCP_tab[Dhwset] = NCP_tab[DHWmax];
		WriteFlash_l = 1;
	}
	else ;


	// From DHW -----> To CH Mode
	if(RoomPW == 1 && Burn_Mode == 1 && flow_op_stat == 0)
	{
		uint16_t DHWStandbytime=0;
		DHWStandbytime = (NCP_tab[DHWStandby] & 0x00ff) *  600; // 100ms Unit
		//DHWStandbytime *= 600; // 100ms Unit
		Burn_Mode_check_en = 1;
		// DHW Standby time
		if(DHWStandbytime_cnt >= DHWStandbytime) {
			Burn_Mode = 0;
			Burn_Mode_check_en = 0;
		}
	}
	else
	{
		Burn_Mode = 0;
		Burn_Mode_check_en = 0;
		DHWStandbytime_cnt = 0;
	}

	// DHW Mode
	if(flow_op_stat == 1)
	{
		Burn_Mode = 1;
		Burn_Mode_check_en = 0;
		DHWStandbytime_cnt = 0;
	}
	else ;


	// Room Standby Check
	if((NCP_tab[RoomFunc] & 0x0001) > 0)
	{
		if(RoomPW == 0)
			RoomPW = 1;
		else ;
	}
	else RoomPW = 0;

	if(CascadeNumber == 0)
	{  // Cascade Master
		// Room CH Heat Demand Check
		temp16 = NCP_tab[RoomFunc] & 0x0002;	// 운전 On/OFF.
		if(temp16 > 0 || IntExtTitleInput == 1)
			bRoomRunDemand = 1;
		else
			bRoomRunDemand = 0;
		// Common Ext
		CommonEXT = ((NCP_tab[RoomFunc] & 0x0080) > 0)? true: false;
	}
	else
	{
		;
	}

	// Error Check
	Error_ok = ((NCP_tab[CurrError] == 0) && (GasDetectON == 0))? 0: 1;

	// Enough Kcal
	Op_Limit_Temp((uint8_t *)&OP_TempLimit, (uint8_t *)&OP_TempLimit_en, (uint8_t *)&OP_TempLimit_cnt,
				mstErrOp.occur, OPCurrTemp, DHWtargetTemp, FlowLiter_x10, flow_op_stat, mstTemp.riseOp);

	// DHW Limit Temperature
 	Dhw_Limit_Temp((uint8_t *)&DHW_TempLimit, (uint8_t *)&DHW_TempLimit_en, DHW_TempLimit_cnt, NCP_tab[DHWonDelta], DHWCurrTemp, DHWtargetTemp);

	// Inlet Limit Temperature Function(2015.11.10.), 5F
 	Inlet_Limit_Temp((uint8_t *)&Inlet_TempLimit, (uint8_t *)&Inlet_TempLimit_en, Inlet_TempLimit_cnt,
 					(WaterHeatType == MODEL_STANDARD)? 0: NCP_tab[DeltaInlet], InletCurrTemp, DHWtargetTemp);

	NeedCal_Maintain_Set(OP_TempLimit,DHW_TempLimit,Inlet_TempLimit);


	Trial_Agm_Ctrl((bool *)&mbAgmInit, RoomPW, (uint16_t *)&NCP_tab[Trial_Agm]);
	Trial_Dhm_Ctrl((bool *)&mbDhmInit, RoomPW, (uint16_t *)&NCP_tab[Trial_Dhw]);
	Trial_Recir_Ctrl(RoomPW, flow_op_stat, gul1msCnt, (uint16_t *)&NCP_tab[RoomSet]);



	// AntiFreeze2 Burn
/*
	if(NCP_tab[CurrError] == 0 && GasDetectON == 0)
		FreezeError_ok = 0;
	else
		FreezeError_ok = 1;
*/
	// 200722:10h lgh 믹싱/유량 밸브 오류 시에는 버너 가동 되도록.
	if(GasDetectON > 0)
		FreezeError_ok = 1;
	else if(NCP_tab[CurrError] > 0)
	{
		uint16_t tmp1=0, tmp2, tmp3;

		tmp1 = (NCP_tab[CurrError] & 0xff00) >> 8;
		if(KoreaType == 0)
		{
			tmp2 = 65;
			tmp3 = 66;
		}
		else
		{
			tmp2 = 35;
			tmp3 = 32;
		}

		if( (tmp1 == tmp2) || (tmp1 == tmp3) )
			FreezeError_ok = 0;
		else
			FreezeError_ok = 1;
	}
	else
		FreezeError_ok = 0;

	if(AntiFreeze == 2 && FreezeError_ok == 0 && DhwProtect == 0 && StorageHeat == 0 && mbErr44 == false && mbErr46 == false)
		AntiFreeze2Protect = 1;
	else
		AntiFreeze2Protect = 0;

	// DHW Operation
	//if(RoomPW == 1 && Inlet_TempLimit==0 && DHW_TempLimit==0 && OP_TempLimit==0 && mstErrExOverHeat.reset==0 && flow_op_stat == 1 && FreezeError_ok == 0)
	if( (RoomPW == 1) && (Inlet_TempLimit==0) && (DHW_TempLimit==0) && (OP_TempLimit==0) && (mstErrExOverHeat.reset==0) &&
		(flow_op_stat == 1) && (FreezeError_ok == 0) && (CCm_Get_Err78() == 0) )
	{
		if(DhwProtect == 0)
		{
			if( (FlowLiter_x10 > 50) && ((gul1msCnt-dhwOnDelayTmr) >= 500) )	// Cheterring 500ms. Flow 5liter excess.
				mucDhwOnSetTmr = 500;
			else ;

			if((gul1msCnt-dhwOnDelayTmr) >= mucDhwOnSetTmr)
			{
				if( (mucDhwOnSetTmr > 0) && (DhwProtect == 0) )
					Set_Cal_Fall_Tmr(0);	// * 100ms.
				else ;

				DhwProtect = 1;
				dhwOnDelayTmr = gul1msCnt - mucDhwOnSetTmr;

			}
		}
	}
	else
	{
		/* 온수 -> 환탕 모드 전환시 딜레이 */
		if(DhwProtect == true)
			recirOnDelayTmr = gul1msCnt;	// Recir Mode Run Delay.
		else ;
		DhwProtect = 0;

		/* 환탕 -> 온수 모드 전환시 딜레이 */
		dhwOnDelayTmr = Dhw_Delay_Tmr(StorageHeat, DHWtargetTemp, OPCurrTemp, (uint32_t *)&mucDhwOnSetTmr, gul1msCnt);
		//dhwOnDelayTmr = Dhw_Delay_Tmr(1, DHWtargetTemp, OPCurrTemp, (uint32_t *)&mucDhwOnSetTmr, gul1msCnt);	// 대기 모드에서도 사용.

		boostPt = false;

	}

	if((flow_op_stat == 1 && DhwProtect == 1) || (Burn_Mode == 1))
	{
		NCP_tab[MachineFunc1] |= 0x0100;	// 맞춤 온도 설정.
		NCP_tab[RoomFunc] |= 0x0800;		// 목욕 기능 설정.
	}
	else
	{
		NCP_tab[MachineFunc1] &= 0xfeff;
		NCP_tab[RoomFunc] &= 0xf7ff;
	}

	// Storage Operation
	if(MCU_rutin_cnt < 50)
	{
		;
	}
	else if(WaterHeatType == MODEL_STANDARD)
	{
		StorageHeat = 0;
		//titleAntiburn = 0;
		//AntiFreq_ok = 0;
		NCP_tab[MachineFunc1] &= 0xfdff;	// 축열 운전 해제.
	}
	else if(Trial_Recir_Chk() == true)
	{
		StorageHeat = Recir_External((DHM_CurrPoint == 0)?true: false, OPCurrTemp, (DHWtargetTemp-50), (DHWtargetTemp+30), FlowRecirLiter_x10);
	}
	else
	{
		uint16_t onTemp=0, offTemp=0;

		if(CcTotalNumber > 1)
		{
			onTemp = DHWtargetTemp - 50;
			offTemp = DHWtargetTemp + 30;
		}
		else
		{
			if((NCP_tab[RecyleTemp] & 0x04) > 0)
			{
				onTemp = DHWtargetTemp - 0;
				offTemp = DHWtargetTemp + 70;
			}
			else if((NCP_tab[RecyleTemp] & 0x02) > 0)
			{
				onTemp = DHWtargetTemp - 20;
				offTemp = DHWtargetTemp + 50;
			}
			else
			{
				onTemp = DHWtargetTemp - 50;
				offTemp = DHWtargetTemp + 30;
			}
		}

		if(((NCP_tab[MachineFunc1] & 0x2000) == 0) && (RecirMode == true) && (RoomPW == true) && (flow_op_stat == false) && (Error_ok == false) &&
			(WaterHeatType == MODEL_PREMIUM) )
		{// NCP_tab[MachineFunc1] & 0x2000 : C.C Flow On.
			if(BypassMode == true)
			{// CrossOver.
				StorageHeat = Recir_CrossOver(bRoomRunDemand, OPCurrTemp, onTemp, offTemp, FlowLiter_x10);
			}
			else
			{// External.
				StorageHeat = Recir_External(bRoomRunDemand, OPCurrTemp, onTemp, offTemp, FlowRecirLiter_x10);
			}
		}
		else if(Title24_ON == true)
		{
			bool run=false, pt=false;

			if( (RoomPW == true) && (RoomPW == true) && (flow_op_stat == false) && (Error_ok == false) && (CascadeNumber == 0) )
				run = true;
			else
				run = false;

			pt = Recir_Title24(run, (P_tt24==false)?1:0, TitleCurrTemp, FlowRecirLiter_x10, (bool)flow_op_stat);

			if(pt == true)
			{
				if(OPCurrTemp > offTemp)
					StorageHeat = false;
				else if(OPCurrTemp < onTemp)
					StorageHeat = true;
				else ;
			}
			else
				StorageHeat = false;

		}
		else
		{
			StorageHeat = false;
			Recir_Ex_PumpTmr_Init();	// Pump Cycle Time Init.
		}

		if(StorageHeat == 1)
			NCP_tab[MachineFunc1] |= 0x0200;
		else
			NCP_tab[MachineFunc1] &= 0xfdff;
	}

	if(WaterHeatType == MODEL_STANDARD)
	{
		if(DhwProtect == 1)
		{
			if(mstHighLimit.occur > 0 || mstBurnLimit.occur > 0 || mstAps.occur > 0)
			{
				PT_enable = 0;
				RePostPT = 1;
			}
			else
			{
				if(FreezeError_ok == 1 || OP_TempLimit == 1 || mstErrExOverHeat.reset==1 || (Get_CalLimite_Tmr() > 0) || (mbDisconnectedLine == true) )
					PT_enable = 0;
				else
					PT_enable = 1;
			}
		}
		else
		{
			PT_enable = 0;
		}
	}
	else
	{
		//if(DhwProtect == 1 || AntiFreeze2Protect == 1 || StorageHeat == 1)
		if( (DhwProtect == 1) || (AntiFreeze2Protect == 1) || ( (StorageHeat == 1) && ((gul1msCnt-recirOnDelayTmr) >= 10000) ) )
		{
			recirOnDelayTmr = gul1msCnt - 10000;
			if(mstHighLimit.occur > 0 || mstBurnLimit.occur > 0 || mstAps.occur > 0 || mbErr44 == true || mbErr46 == true)
			{
				PT_enable = 0;
				RePostPT = 1;
			}
			else
			{
				if(FreezeError_ok == 1 || OP_TempLimit == 1 || mstErrExOverHeat.reset==1 || (Get_CalLimite_Tmr() > 0) || (mbDisconnectedLine == true) )
					PT_enable = 0;
				else
					PT_enable = 1;
			}

			if(DhwProtect == 1)
				BurnSetMode = 1;
			else if(StorageHeat == 1)
				BurnSetMode = 2;
			else
				BurnSetMode = 3;

/*
			if((BurnSetMode == CurrBurnSetMode) && (PT_enable == 1))
				PT_enable = 1;
			else
 */
			if((BurnSetMode != CurrBurnSetMode) && (PT_enable == 1) && (WaterHeatType == MODEL_PREMIUM) )
			{
				// 191113:10h lgh  온수 모드 & CrossOver 환탕 모드전환시 버너 유지 하게(PID 만 초기화).
				// 200804:16h lgh	6L 미만은 OFF 되도록.
				if( (BurnSetMode == 3) || (CurrBurnSetMode == 3) || (BypassMode == false) || (FlowLiter_x10 < 60) )
				{
					PT_enable = 0;
					if(FlowLiter_x10 < 60)
						Pt_Burn_OffTmr();
					else ;
				}
				else
					Kcal_Pid_Init();
			}
			else ;

			CurrBurnSetMode = BurnSetMode;

		}
		else
		{
			PT_enable = 0;
		}
	}
}


/* ########################################################
	PT_process
 ########################################################*/
void PT_process(void)
{
	PID_update();
	//if(PT_enable && RePostPT == 0 && FlameExt_4time == 0)
	if(PT_enable && RePostPT == 0 && FlameExt_4time == 0 && mbAgmInit == false)	// 200521:11h lgh AGM 초기화 시 진입 못함.
	{
		uint16_t prePurgeTime=0;

		BurnState = true;
		PostPT = 0;

		if( (PT_step == 0) && (boostPt == true) )
		{
			PT_step = 1;
			PT_time = 20;
		}
		else ;

		switch(PT_step) {
			case 0:
				bBurnControl = false;
				Er39fault_en = 0;

				Er72Flame_chk = 0;

				MaxFanRPM_check = 0;
				MaxFanRPM_en = 0;
				MaxFancut = 0;
				MaxFanRPM_check_cnt = 0;
				MaxFanRPM_cnt = 0;
				Burner_ON = 1;

				mstAgm.gallPoint = PrePurgeAGM;
				FanCtrl_Value = PrePurgePWM + ItFault_Pwm_Adjust(GVDelayTime);

				//PrepurgDelayTime += (PrepurgDelayTime == 0)? 10: 0;

				if(PrepurgDelayTime > 0)
					prePurgeTime = PrepurgDelayTime;
				else
					prePurgeTime = 30;

				if(PT_time >= PrepurgDelayTime)
				{
					PT_step = 1;
					PT_time = 10;
				}
				else ;
				break;
			case 1:
				bBurnControl = false;
				DHW_CtrlKcal = MIN_KCAL;
				mstFnA.tbNum.findOk = 0;
				TableChange_en = 0;
				Er39fault_en = 0;
				PrepurgDelayTime = 0;
				MaxFanRPM_check = 0;
				MaxFanRPM_en = 0;
				MaxFancut = 0;
				MaxFanRPM_check_cnt = 0;
				MaxFanRPM_cnt = 0;
				Er72Flame_chk = 0;
				Burner_ON = 1;

				mstAgm.gallPoint = IgnitionAGM;
				FanCtrl_Value = IgnitionPWM + ItFault_Pwm_Adjust(GVDelayTime);

				if(PT_time < 25)
				{
					out_IT = 1;
				}
				else
				{
					boostPt = false;
					out_GV = 1;
					PT_step = 2;
					bBurnControl = false;
				}
				break;
			case 2:
				Er72Flame_chk = 0;
				if(ItFault_cnt < 3)
					GVDelayTime = 55;
				else if(ItFault_cnt >= 3 && ItFault_cnt < 6)
					GVDelayTime = 65;
				else
					GVDelayTime = 75;

				if(Fire_flag == 0)
				{
					if(PT_time >= GVDelayTime)
					{
						out_IT = 0;
						out_GV = 0;
						PT_step = 3;

						if(PT_time <= GVDelayTime)
						{
							ItFault_en = 1;
							FlameExt_en = 0;
						}
						else if(PT_time < 645)
						{
							ItFault_en = 0;
							FlameExt_en = 1;
						}
						else
							FlameExt_en = 0;
					}
				}
				else
				{
					ItFault_en = 0;
					if(PT_time >= GVDelayTime)
					{
						out_IT = 0;
						/////
						if(PT_time <= (GVDelayTime+10))
						{
							;//Kcal_Pid_Init();
						}
						else {
							//PID_update();
							bBurnControl = true;
						}
					}
					else {
						mstAgm.gallPoint = IgnitionAGM;
						FanCtrl_Value = IgnitionPWM + ItFault_Pwm_Adjust(GVDelayTime);
					}

					if(PT_time >= 6000) {  // 10minutes
						ItFault_cnt = 0;
						FlameExt_cnt = 0;
					}
				}
				break;
			case 3:
				bBurnControl = false;
				MaxFanRPM_check = 0;
				MaxFanRPM_check_cnt = 0;
				MaxFanRPM_en = 0;
				MaxFancut = 0;
				MaxFanRPM_cnt = 0;
				err_72Flame = 0;
				if(err_72Flame == 0) Er72Flame_chk = 0;
				RePostPT = 1;
				RePostTime_cnt = 0;

				mstAgm.gallPoint = PostPurgeAGM;
				FanCtrl_Value = PostPurgePWM;

				if(ItFault_en == 1)
				{
					ItFault_en = 0;
					if(ItFault_cnt < 9) {
						PrepurgDelayTime = 100;
						ItFault_cnt++;
						if(ItFault_cnt == 5)	// 200521:11h lgh 실패 연속 5회 발생시 AGM 초기화.
							mbAgmInit = true;
						else ;
					}
					else {
						err_11ItFault = 1;
						ItFault_cnt = 0;
						FlameExt_cnt = 0;
					}
				}

				if(FlameExt_en == 1)
				{
					FlameExt_en = 0;
					if(FlameExt_cnt < 7)
					{
						PrepurgDelayTime = 50;
						FlameExt_cnt++;
						if(FlameExt_cnt == 4) FlameExt_4time = 1;	// 실화 4회 발생시 AGM 초기화.
					}
					else
					{
						err_10FlameExt = 1;
						FlameExt_cnt = 0;
						ItFault_cnt = 0;
					}
				}

				break;
		}
    }
	else
	{
		mstFnA.tbNum.findOk = 0;
		TableChange_en = 0;

		BurnState = false;
		bBurnControl = false;

		MaxFanRPM_check = 0;
		MaxFanRPM_check_cnt = 0;
		MaxFanRPM_en = 0;
		MaxFancut = 0;
		MaxFanRPM_cnt = 0;
		PT_step = 0;
		PT_time = 0;
		out_IT = 0;
		out_GV = 0;

		if(Error_ok == 0 && RePostPT == 0 && FlameExt_4time == 0)
		{
			FlameExt_en = 0;
			FlameExt_cnt = 0;
			ItFault_en = 0;
			ItFault_cnt = 0;
		}
		else ;

		//if(RePostTime_cnt >= SetAntiProtectTime) {
		{
			RePostPT = 0;
			RePostTime_cnt = 0;
		}

		if(Burner_ON == 1)
		{
			PostPT = 1;
			err_72Flame = 0;
			Er72Flame_chk = 0;

			if(Fire_flag == 1)
				Er39fault_en = 1;
			else
				Er39fault_en = 0;

			if(PostTime_cnt >= POSTFANTIME)
			{
				Burner_ON = 0;
				PostPT = 0;
				PostTime_cnt = 0;
				mstAgm.gallPoint = PostPurgeAGM;
				FanCtrl_Value = 0;
			}
			else
			{
				//mstAgm.gallPoint = PostPurgeAGM;
				// 강제 버너 OFF 시에는 AGM 빠르게 이동 하기 위해 PrePurge 포지션으로 이동.
				mstAgm.gallPoint = (Get_ForcedBurnOff_Tmr() > 0)? IgnitionAGM: PostPurgeAGM;
				FanCtrl_Value = PostPurgePWM;

				// PostPurge 10초 이상 동작시 PrePurge는 1초.
				if( (PostTime_cnt > 100) && (ItFault_cnt == 0) && (FlameExt_cnt == 0) )
					PrepurgDelayTime = 10;
				else ;
			}

/////////////////////////////////

			if(err_MaxFanRPM == 1 && PostTime_cnt >= 100)
			{
				if(FanRpm >= 4300)
					ErrMaxRPM_cnt = 5;
				else
				{
					if(ErrMaxRPM_cnt < 5)
					{
						err_MaxFanRPM = 0;
						ErrorOFF(41);
						Cnt_er41 = 0;
					}
				}
			}
		}
		else
		{
			PrepurgDelayTime = 0;
			// 2015.11.04. Common Vent
			//////////////////////////////////////////////////
			if(CcTotalNumber > 1 && CommonEXT == true && DeviceTotalchk == true && munCcComVentPwm >= 10)
			{
				PostPT = 1;
				FanCtrl_Value = munCcComVentPwm;
				mstAgm.gallPoint = PostPurgeAGM;
				PostTime_cnt = 0;
			}
			else
			{
				if(PostPT == 1 && PostTime_cnt < POSTFANTIME) // more than 1 Minutes --> PostPurgePWM-300(2013.8.1.:LJH) --> 3 Minutes(2016.4.1.:LJH)
					Burner_ON = 1;
				else
				{
					PostPT = 0;
					if(err_GasDetect == 1)
					{
						err_GasDetect_en = 1;
						if(err_GasDetect_cnt < 3000) FanCtrl_Value = PostPurgePWM;
						else FanCtrl_Value = 0;
						mstAgm.gallPoint = PostPurgeAGM;
					}
					else if(GasDetectON == 1 || mstErrSensorOverHeat.occur == 1 || mstHighLimit.occur == 1 || mstBurnLimit.occur == 1 || mstErrExOverHeat.reset == 1) {
						err_GasDetect_en = 0;
						FanCtrl_Value = PostPurgePWM;
						mstAgm.gallPoint = PostPurgeAGM;
					}
					else
					{
						err_GasDetect_en = 0;
						FanCtrl_Value = 0;
						if(err_AGMfault == 0)
						{
							// AGM Standby Postion Set.
							if(nomalPosition_cnt > 30000)
								mstAgm.gallPoint = 500;
							else
								mstAgm.gallPoint = PostPurgeAGM;
						}
					}
				}
			}
		//////////////////////////////////////////////

			if(Fire_flag == 1 && Er39fault_cnt >= POSTFANTIME)
				err_Er39fault = 1;
			else
			{
				Er39fault_en = 0;
				err_Er39fault = 0;
			}

			if(err_Er39fault > 0)
				Er72Flame_chk = 0;
			else
				Er72Flame_chk = 1;
		}

		Kcal_Pid_Init();// 190731:9h lgh

	}


	if(DHW_CtrlKcal >= 60500)
		DHW_CtrlKcal = 60500;
	else ;
	setFanAGM((uint16_t)DHW_CtrlKcal);

	if(DHW_CtrlKcal <= MIN_KCAL)
		DHW_CtrlKcal = MIN_KCAL;
	else if(DHW_CtrlKcal >= MAX_KCAL)
		DHW_CtrlKcal = MAX_KCAL;
	else ;

	//Fan_Control(FanCtrl_Value);	// go to 1ms Interrupt Vecter.

	// Intergration Part
	#if 0 // 190723:9h lgh
	if(DHWtempChk_cnt == 0) {
		DHWtempChk_cnt = DHWTEMPCHK_TIME;
		DHWtemp_state();
	}
	#endif

}



/* ########################################################
	PID
 ########################################################*/
#ifdef _debug_
	uint32_t LPF_AVG_CAL=0;
#endif
void PID_update(void)
{
	if(mstDip.sw.d7 == 1 && mstDip.sw.d6 == 0) {	// Low Fire

		/*** Burn Table TEST part ****************
		mstAgm.gallPoint = AGM_value;
		FanCtrl_Value = FanValue;
		******************************************/
		if(MIN_KCAL == 0)
			DHW_CtrlKcal = 4300;
		else
			DHW_CtrlKcal = MIN_KCAL;
	}
	else if(mstDip.sw.d7 == 0 && mstDip.sw.d6 == 1) {   // High Fire

		/*** Burn Table TEST part ****************
		mstAgm.gallPoint = AGM_value;
		FanCtrl_Value = FanValue;
		******************************************/
		if( (mstTemp.avgFlow >= 5) && (IWM_ForcedOpen == false) )
		{
			if(MAX_KCAL == 0)
				DHW_CtrlKcal = 47500;
			else
				DHW_CtrlKcal = MAX_KCAL;
		}
		else
		{	// 200522:16h lgh Max 연소시 유량에 따라 Min 동작.
			if(MIN_KCAL == 0)
				DHW_CtrlKcal = 4300;
			else
				DHW_CtrlKcal = MIN_KCAL;
		}
	}
	else
	{	// Normal Fire
		// 2. DHW Mode

		if(DhwProtect == 1)
		{
			float ftmp=0;
			int32_t tempKcal=0, subTemp=0, deltaT=0;
			static uint32_t bandKcal=0;

			if(mstTemp.Set > (mstTemp.avgInlet+1))
				ftmp = (mstTemp.Set-mstTemp.avgInlet) * mstTemp.avgFlow * 60;
			else
				ftmp = 1 * mstTemp.avgFlow * 60;

			needDHW_CtrlKcal = (uint32_t)((ftmp>65000)? 65000: ftmp);

			/*PID Control*/
			DHW_CtrlKcal = Kcal_Pid_Func(mstTemp.Set, mstTemp.Dhw, MIN_KCAL, MAX_KCAL, HighCal_Limite(needDHW_CtrlKcal));

			#ifdef _debug_
			{
				static uint32_t lpfSum=0, maintainTmr=0;
				if((gul1secCnt-maintainTmr) > 5)
				{
					maintainTmr = gul1secCnt;
					lpfSum += (DHW_CtrlKcal - (lpfSum >> 4));
					LPF_AVG_CAL = (lpfSum >> 4);
				}
				else ;
			}
			#endif
		}
		else
		{	// Flow OFF

			if(RecirMode == true)
			{
				if(BypassMode == true)
					DHW_CtrlKcal = Recir_Pid_Func(0,DHWtargetTemp, OPCurrTemp, InletCurrTemp, MIN_KCAL, MAX_KCAL, FlowLiter_x10);
				else
					DHW_CtrlKcal = Recir_Pid_Func(1,DHWtargetTemp, OPCurrTemp, InletCurrTemp, MIN_KCAL, MAX_KCAL, FlowRecirLiter_x10);
			}
			else if(Title24_ON == true)
			{
				DHW_CtrlKcal = Recir_Pid_Func(2,390, TitleCurrTemp, InletCurrTemp, MIN_KCAL, MAX_KCAL, FlowRecirLiter_x10);
			}
			else
				;

		}
	}

	// Er.41
	MaxFanRPM_check = 1;
	if(MaxFanRPM_check_cnt > 50)	// * 100ms.
	{
		if( (FanRpm >= MaxFancut) && (mstAgm.currPoint == mstAgm.gallPoint) && (MaxFancut > 0) )
		{
			MaxFanRPM_en = 1;
			if(MaxFanRPM_cnt > 150)	// * 100ms.
				err_MaxFanRPM = 1;
			else ;
		}
		else
		{
			MaxFanRPM_en = 0;
			MaxFanRPM_cnt = 0;
		}
	}
}


/* ########################################################
	DHM Step motor driver
 ########################################################*/
void DHM_driver(void)
{
	volatile uint8_t i = 0;
	volatile unsigned int temp_A = 0, temp_B = 0;
	volatile unsigned long templong_A = 0, templong_B = 0;
	volatile float tempfloat = 0;
	static uint32_t runDelayTmr=0;	// 환탕->온수모드 시 딜레이.

	if(err_DHM_ZeroClose == 0 && err_DHM_ZeroOpen == 0)
		DHM_enable = 0;
	NCP_tab[DHMStep] = DHM_CurrPoint;
	NCP_tab[DhmPercent] = (uint16_t)(DHM_CurrPoint / (DHM_MAXSTEP/100));	// 191111:16h lgh

	// Flow_Down, Flow_Up, DHMstepValues
	if(mstDip.sw.d7 == 0 && mstDip.sw.d6 == 0)
	{
		if(err_CirPump == true)	// 191218:8h lgh Err.60 OPEN.
			DHM_GallPoint = 0;
		else if( (Burn_Mode == 1) && (err_DHM_ZeroClose == 0) && (err_DHM_ZeroOpen == 0) )
		{
			if((gul1secCnt - runDelayTmr) < 10)																// Max 10sec.
			{
				if(PT_time >= GVDelayTime)
					runDelayTmr = gul1secCnt+10;
				else ;
			}
			else if(DHWtargetTemp > 600)													// 60도 초과 설정시 Close 고정.
				DHM_GallPoint = DHM_MAXSTEP;
			else
				Mixing_Pid_Func(true);
		}
		else if(Trial_Recir_Chk() == true)
		{
			DHM_GallPoint = 0;
		}
		else
		{
			if(OP_TempLimit == 1)
				Mixing_Pid_Func(true);
			// DHM Refreeze Test
			else if(err_DHM_ZeroClose == 0 && err_DHM_ZeroOpen == 0)
			{
				if(DHM_Refreeze360Min_cnt < 43200)  // 43200*2=86400sec(24hours)<- 21,600sec(360Min:6hours)
				{
					if(mstDip.sw.d4 == 0)
					{
						if(WaterHeatType == MODEL_STANDARD)
						{
							DHM_GallPoint = Mix_Valve_HotReady();
							Mix_Pid_Init();
						}
						else
						{
							if(Get_Recir_Flow_Dhm() == true)
							{
								DHM_GallPoint = 0;
								Mix_Pid_Init();
							}
							else if(RecirMode == 0)
							{
								Mix_Pid_Init();
								// nkw11
								if(Title24_ON == true)
									DHM_GallPoint = DHM_MAXSTEP;
								else
								{
									//DHM_GallPoint = (out_CP1 == 1)? 0: DHM_MAXSTEP;
									DHM_GallPoint = Mix_Valve_HotReady();
								}
							}
							else
							{
								// Recire -> DHM Delay.
								runDelayTmr = gul1secCnt;
								if(InOut_cirMode == 1)
								{	// External CP
									DHM_GallPoint = DHM_MAXSTEP;	// Close
									Mix_Pid_Init();
								}
								else if(BypassMode == true)	// 191015:17h lgh CrossOver.
								{
									if( (out_CP1 == false) && (FlowLiter_x10 >= 15) )
										Mixing_Pid_Func(true);
									else
									{
										DHM_GallPoint = DHM_MAXSTEP;	// Close
										Mix_Pid_Init();
									}
								}
								else
								{	// Internal CP
									if(CcTotalNumber <= 1)
										DHM_GallPoint = 0;  // Open
									else
										DHM_GallPoint = (out_CP1 == 1)? 0: DHM_MAXSTEP;
									Mix_Pid_Init();
								}
							}
						}
					}
					else
						DHM_GallPoint = DHM_MAXSTEP;
				}
				else if(DHM_Refreeze360Min_cnt >= 43200 && DHM_Refreeze360Min_cnt < 43206)
					DHM_GallPoint = 2000;
				else if(DHM_Refreeze360Min_cnt >= 43206 && DHM_Refreeze360Min_cnt < 43212)
					DHM_GallPoint = 0;
				else if(DHM_Refreeze360Min_cnt >= 43212 && P_DHM_HALL == 0)
				{
					Pump_Flow_Inspection();
					DHM_Refreeze360Min_cnt = 0;
				}
				else ;
			}
		}
	}
	else if(mstDip.sw.d7 == 1 || mstDip.sw.d6 == 1)
	{// Forced Min/Max.
		DHM_GallPoint = 2000;
	}
	else
		DHM_GallPoint = DHM_MAXSTEP;  // All Close in MAX/MIN test

	if(mbDhmInit == true)// 191216:10h lgh
	{
		DHM_GallPoint = 0;
		mbDhmInit = (DHM_GallPoint == DHM_CurrPoint)? false: true;
	}
	else ;

	//DHM_Operating Test
	if(err_DHM_ZeroClose == 0)
	{
		DHMset_en = 0;
		DHMset_cnt = 0;
		if(DHM_GallPoint == 0 && P_DHM_HALL != 0)
		{	// Full Close
			DHM_CloseTest_en = 1;
			if(DHM_CloseTest_cnt >= 120)
			{
				err_DHM_ZeroClose = 1;
				DHM_CloseTest_en = 0;
				DHM_CloseTest_cnt = 0;
				DHM_GallPoint = 0;
			}
		}
		else
		{
			DHM_CloseTest_en = 0;
			DHM_CloseTest_cnt = 0;
		}
	}
	else
	{
		DHM_GallPoint = 0;
		if(P_DHM_HALL == 0)
		{
			DHM_GallPoint = 0;
			err_DHM_ZeroClose = 0;
			DHM_CloseTest_en = 0;
			DHM_CloseTest_cnt = 0;
			DHMset_en = 0;
			DHMset_cnt = 0;
			DHM_enable = 0;
		}
		else
		{
			DHMset_en = 1;
			if(DHMset_cnt < 120)
				DHM_enable = 0;
			else if(DHMset_cnt >= 120 && DHMset_cnt < 300)
				DHM_enable = 1;
			else
				DHMset_cnt = 0;
		}
	}

	if(err_DHM_ZeroOpen == 0)
	{
		if(DHM_GallPoint >= 500 && P_DHM_HALL == 0)
		{
			DHM_OpenTest_en = 1;
			if(DHM_OpenTest_cnt >= 120)
			{
				err_DHM_ZeroOpen = 1;
				DHM_OpenTest_en = 0;
				DHM_OpenTest_cnt = 0;
			}
		}
		else
		{
			DHM_OpenTest_en = 0;
			DHM_OpenTest_cnt = 0;
		}
	}
	else
	{
		DHM_GallPoint = DHM_MAXSTEP;
		DHM_CurrPoint = 0;
		if(P_DHM_HALL != 0)
		{
			DHM_GallPoint = 0;
			err_DHM_ZeroOpen = 0;
			DHM_OpenTest_en = 0;
			DHM_OpenTest_cnt = 0;
			DHMOPset_en = 0;
			DHMOPset_cnt = 0;
			DHM_enable = 0;
		}
		else
		{
			DHMOPset_en = 1;
			if(DHMOPset_cnt < 120)
				DHM_enable = 0;
			else if(DHMOPset_cnt >= 120 && DHMOPset_cnt < 300)
				DHM_enable = 1;
			else
				DHMOPset_cnt = 0;
		}
	}

	if(DHM_GallPoint > DHM_MAXSTEP)
		DHM_GallPoint = DHM_MAXSTEP;
	else ;
}


/* ########################################################
	IWM Step motor driver
 ########################################################*/
void IWM_control(void)
{
	NCP_tab[IwmPercent] = (uint16_t)(IWM_CurrPoint / (IWM_MAXSTEP/100));	// 191111:16h lgh

	IWM_intial_en = 1;
	if(IWM_intial == 0)
	{ // IWM RESET
		IWM_GallPoint = 0;
		IWMdriver_en = 0;
		IWM_gall_en = 0;
		if(P_IWM_HALL == 0)
		{
			IWM_CurrPoint = 0;
			IWM_GallPoint = 0;
			IWM_intial = 1;
			err_IWM_ZeroClose = 0;
			IWM_GallPoint = 0;
		}
		else
		{
			if(IWM_intial_cnt > 120) err_IWM_ZeroClose = 1;
		}
	}
	else
	{
		if( (err_HighLevel == 1) || (CumulativeCnt_er45 >= 5) )	// 200521:8h lgh 수동 해제일 경우 닫힘 유지.
		{
			IWM_GallPoint = IWM_MAXSTEP;
			HighLevel_state = 1;
		}
		else
		{
			IWMuse = (mstDip.sw.d4 == 1)? 0: 1;
/*
			if(CascadeNumber == 0)
				IWMuse = (mstDip.sw.d4 == 1)? 0: 1;
			else
				IWMuse = ((NCP_tab[ReCPumpMode] & 0x0200) > 0)? 1: 0;
*/

			//if((IWMuse == 0) || (mstDip.sw.d7 == 1 && mstDip.sw.d6 == 0) || (mstDip.sw.d7 == 0 && mstDip.sw.d6 == 1))
			if(IWMuse == 0)// 200522:16h lgh min/max 상관 없이 동작 유무 결정.
			{
				IWM_GallPoint = 0;
				IWM_gall_en = 0;
				IWMdriver_en = 0;
			}
			else
			{
				if(WaterHeatType == MODEL_PREMIUM)
				{
					if(err_CirPump == 1)
					{
						IWM_GallPoint = IWM_MAXSTEP;
						HighLevel_state = 1;
					}
					else
					{
						if(HighLevel_state == 1)
						{
							IWM_GallPoint = 0;
							if(P_IWM_HALL == 0) HighLevel_state = 0;
						}
						else
						{
							if( (RoomPW == 0 || AntiFreeze2Protect == 1) && (CcTotalNumber <= 1) )
							{
								if(NCP_tab[Trial_Iwm] > 0)
								{
									IWM_GallPoint = IWM_MAXSTEP;
									if(IWM_CurrPoint == IWM_MAXSTEP)
										NCP_tab[Trial_Iwm] = 0;
									else ;
								}
								else
									IWM_GallPoint = 0;

								Iwm_Pid_Init(IWM_GallPoint);
							}
							else
							{
								IWM_driver();
							}
						}
					}
				}
				else
				{
					HighLevel_state = 0;

					if( (RoomPW == 0 ) && (CcTotalNumber <= 1) )
					{
						if(NCP_tab[Trial_Iwm] > 0)
						{
							IWM_GallPoint = IWM_MAXSTEP;
							if(IWM_CurrPoint == IWM_MAXSTEP)
							{
								NCP_tab[Trial_Iwm] = 0;
								IWM_intial = 0;
							}
							else ;
						}
						else
							IWM_GallPoint = 0;

						Iwm_Pid_Init(IWM_GallPoint);

					}
					else
						IWM_driver();
				}
			}
		}
	}

	IWM_Chk();
}


/* ########################################################
	IWM Control
 ########################################################*/
void IWM_driver(void)
{
	unsigned int IWM_DeltaT=0, IWM_FlowLiter_x10=0, IWM_up=0, IWM_down=0, IWMspeedTime=0;
	unsigned long IWM_CtrlKcal = 0;
	uint16_t originMaxCal=mstFnA.tbl.Info[mstFnA.tbNum.max][1];
	static float outCenterFlow=0;
	static uint32_t forcedCycleTmr=0;


	if(IWMuse == 0)
	{
		IWM_gall_en = 0;
		IWM_GallPoint = 0;
		IWMdriver_en = 0;
	}
	else
	{
		static float adjFlow=0;	// 역산 유량 감소 보정 값.

		if(mstTemp.Set > (mstTemp.avgInlet+1))
			IWM_DeltaT = (uint16_t)(mstTemp.Set - mstTemp.avgInlet) * 10;
		else
			IWM_DeltaT = 10;

		IWM_CtrlKcal = (unsigned long)(mstTemp.avgFlow*10)*(unsigned long)IWM_DeltaT;
		IWM_CtrlKcal = (IWM_CtrlKcal*6)/10;


		IWM_up = originMaxCal+1000;
		IWM_down = originMaxCal-1000;


		if(IWM_CtrlKcal > IWM_up)
		{
			if(IWM_GallPoint <= 1000)
				IWMspeedTime = 50;
			else if(IWM_GallPoint > 1000 && IWM_GallPoint <= 1400)
				IWMspeedTime = 100;
			else
				IWMspeedTime = 250;
		}
		else IWMspeedTime = 100;

		if(mstDip.sw.d7 == 1 && mstDip.sw.d6 == 0)
		{// Forced Min.
			if((gul1msCnt - forcedCycleTmr) >= 3000)	// 3sec cycle.
			{
				forcedCycleTmr = gul1msCnt;
				if(mstTemp.avgFlow > 6)
				{
					if(IWM_GallPoint < 1600)
						IWM_GallPoint += 200;
					else if(IWM_GallPoint < 1800)
						IWM_GallPoint += 20;
					else if(IWM_GallPoint < 2000)
						IWM_GallPoint += 10;
					else
						IWM_GallPoint = 2000;
				}
				else if(mstTemp.avgFlow < 5)
				{
					if(IWM_GallPoint > 10)
						IWM_GallPoint -= 10;
					else
						IWM_GallPoint = 0;
				}
				else ;
			}
			else ;
			IWM_ForcedOpen = true;
		}
		else if(IWM_ForcedOpen == true)
		{
			IWM_GallPoint = 0;
			if(IWM_GallPoint == IWM_CurrPoint)
				IWM_ForcedOpen = false;
			else ;
		}
		else if(mstDip.sw.d7 == 0 && mstDip.sw.d6 == 1)
		{// Forced Max.
			float maxFlow= MAX_KCAL / ((mstTemp.Set-mstTemp.avgInlet) * 60);

			if((gul1msCnt - forcedCycleTmr) >= 4000)	// 4sec cycle.
			{
				forcedCycleTmr = gul1msCnt;

				if(mstTemp.avgFlow > (maxFlow+2))
				{
					if(IWM_GallPoint < 1200)
						IWM_GallPoint += 200;
					else if(IWM_GallPoint < 1400)
						IWM_GallPoint += 20;
					if(IWM_GallPoint < 1800)
						IWM_GallPoint += 10;
					else
						IWM_GallPoint = 1800;
				}
				else if(mstTemp.avgFlow < maxFlow)
				{
					if(IWM_GallPoint > 10)
						IWM_GallPoint -= 10;
					else
						IWM_GallPoint = 0;
				}
				else ;
			}
			else ;
		}
		//3 Single Mode.------------
		//else if((PT_time > 10) && (CcTotalNumber <= 1))
		else if(CcTotalNumber <= 1)
		{
			static bool runing=false, bIwmHold=false;
			static uint32_t maintainTmr=0, tempChkTmr=0;

			if(PT_time > 10)
			{
				outCenterFlow = CAL_CENTER / (IWM_DeltaT * 6) - adjFlow;

				if(IWM_CtrlKcal > CAL_CENTER)
				{
					if( (mstTemp.avgFlow < outCenterFlow+0.5) && (mstTemp.avgFlow > outCenterFlow-0.5) )
					{
						if(mstTemp.avgDhw < mstTemp.Set)
						{
							if((gul1msCnt-maintainTmr) >= 30)
							{
								maintainTmr = gul1secCnt;
								adjFlow ++;
							}
							else ;
						}
						else
							maintainTmr = gul1secCnt;

						IWM_Pid_Fun(outCenterFlow, 1000);
					}
					else
						IWM_GallPoint = IWM_Pid_Fun(outCenterFlow, 1000);
					Set_Iwm_Run(true);
				}
				else if(IWM_CtrlKcal < IWM_down)
				{
					IWM_GallPoint = IWM_Pid_Fun(outCenterFlow, 0);
					Set_Iwm_Run(false);
				}
				else
					Iwm_Pid_Init(IWM_GallPoint);
			}
			else if(flow_op_stat == 0)
			{
				IWM_GallPoint = 0;
				adjFlow = 0;
				Iwm_Pid_Init(IWM_GallPoint);
				Set_Iwm_Run(false);
			}
			else ;
		}
		//3 Cascade Mode.--------------
		else
		{
			IWM_gall_en = 1;
			if(IWM_gall_cnt > 30)
			{
				IWM_gall_cnt = 0;

				if(CcTotalNumber > 1)
				{
					if(CC_Chk_Iwm_Use(CascadeNumber, &mucCcBurn01_08) == true)
					{
						IWMdriver_en = 1;
						IWM_GallPoint = 0;
					}
					else
					{
						IWMdriver_en = 0;
						if(IWM_GallPoint < IWM_MAXSTEP)
							IWM_GallPoint = IWM_MAXSTEP;
						else ;
					}
				}
				else
					IWM_GallPoint = 0;
			}
			adjFlow = 0;
			Iwm_Pid_Init(IWM_GallPoint);
		}
	}
}


/* ########################################################
	IWM Refreeze Init
 ########################################################*/
void IWM_Chk(void)
{
	// IWM Refreeze Test
	if(err_IWM_ZeroClose == 0 && err_IWM_ZeroOpen == 0)
		IWM_enable = 0;
	else ;

	if(err_IWM_ZeroClose == 0 && err_IWM_ZeroOpen == 0 && flow_op_stat == 0)
	{
		if(IWM_Refreeze360Min_cnt >= 43200 && IWM_Refreeze360Min_cnt < 43206)		// 1h:12m:0s.
			IWM_GallPoint = 1500;
		else if(IWM_Refreeze360Min_cnt >= 43206 && IWM_Refreeze360Min_cnt < 43212)	// 1h:12m:1s.
			IWM_GallPoint = 0;
		else if(IWM_Refreeze360Min_cnt >= 43212 && P_IWM_HALL == 0)
			IWM_Refreeze360Min_cnt = 0;
		else ;
	}

	if(err_IWM_ZeroClose == 0)
	{
		IWMset_en = 0;
		IWMset_cnt = 0;
		if(IWM_GallPoint == 0 && P_IWM_HALL != 0)
		{	// Full Open Cehck.
			IWM_CloseTest_en = 1;
			if(IWM_CloseTest_cnt >= 120)
			{
				err_IWM_ZeroClose = 1;
				IWM_CloseTest_en = 0;
				IWM_CloseTest_cnt = 0;
				IWM_GallPoint = 0;
			}
		}
		else
		{
			IWM_CloseTest_en = 0;
			IWM_CloseTest_cnt = 0;
		}
	}
	else
	{
		IWM_GallPoint = 0;
		if(P_IWM_HALL == 0)		// Zero Point Check.
		{
			IWM_GallPoint = 0;
			err_IWM_ZeroClose = 0;
			IWM_CloseTest_en = 0;
			IWM_CloseTest_cnt = 0;
			IWMset_en = 0;
			IWMset_cnt = 0;
			IWM_enable = 0;
			IWM_CurrPoint = 0;
		}
		else
		{
			IWMset_en = 1;
			if(IWMset_cnt < 120)	// 12s.
				IWM_enable = 0;
			else if(IWMset_cnt >= 120 && IWMset_cnt < 300)	// 12~30s.
				IWM_enable = 1;
			else
				IWMset_cnt = 0;
		}
	}

	if(err_IWM_ZeroOpen == 0)
	{
		IWMOPset_en = 0;
		IWMOPset_cnt = 0;
		if(IWM_GallPoint >= 500 && P_IWM_HALL == 0)
		{ // Not Open Check.
			IWM_OpenTest_en = 1;
			if(IWM_OpenTest_cnt >= 120)
			{
				err_IWM_ZeroOpen = 1;
				IWM_OpenTest_en = 0;
				IWM_OpenTest_cnt = 0;
			}
		}
		else
		{
			IWM_OpenTest_en = 0;
			IWM_OpenTest_cnt = 0;
		}
	}
	else
	{
		IWM_GallPoint = IWM_MAXSTEP;
		IWM_CurrPoint = 0;
		if(P_IWM_HALL != 0)
		{
			err_IWM_ZeroOpen = 0;
			IWM_OpenTest_en = 0;
			IWM_OpenTest_cnt = 0;
			IWMOPset_en = 0;
			IWMOPset_cnt = 0;
			IWM_enable = 0;
		}
		else
		{
			IWMOPset_en = 1;
			if(IWMOPset_cnt < 120)
				IWM_enable = 0;
			else if(IWMOPset_cnt >= 120 && IWMOPset_cnt < 300)
				IWM_enable = 1;
			else
				IWMOPset_cnt = 0;
		}
	}
}

/* ########################################################
	AGM Initial
 ########################################################*/
void AGM_Initial(bool ncp)
{
	mstAgm.tmrRefreeze360min = 0;
	AGM_Zero_en = 0;
	AGM_Zero_cnt = 0;
	AGM_PowerPoint_en = 0;
	AGM_PowerPoint_cnt = 0;
	mstAgm.gallPoint = 0;//AGM_MAXSTEP/2;
	mstAgm.currPoint = 2000;
	AGMzeroLevel_cnt = 0;// 200525:9h lgh
	//while(mstAgm.currPoint != AGM_MAXSTEP/2) {
	while(mstAgm.currPoint != mstAgm.gallPoint)
	{
		WDTE = 0xACU;
		if(ncp == true)
			NCP_A_Com();
		else ;
		Uart0_Com();

		if(AGMzeroLevel_cnt > 3333)
			mstAgm.currPoint = 0;
		else ;
	}
	AGM_PowerPoint_en = 1;

	AGM_PowerPoint_cnt = 0;
	while(AGM_PowerPoint_cnt <= 20)	// 2sec.
	{
		;
	}

	mstAgm.gallPoint = 1000;
	mstAgm.currPoint = 0;
	while(mstAgm.currPoint != mstAgm.gallPoint)
	{
		WDTE = 0xACU;
		if(ncp == true)
			NCP_A_Com();
		else ;
		Uart0_Com();
	}

	AGM_PowerPoint_cnt = 0;
	while(AGM_PowerPoint_cnt <= 20)	// 2sec.
	{
		;
	}

	mstAgm.gallPoint = 0;//AGM_MAXSTEP/2;
	mstAgm.currPoint = 2000;
	while(mstAgm.currPoint != mstAgm.gallPoint)
	{
		WDTE = 0xACU;
		if(ncp == true)
			NCP_A_Com();
		else ;
		Uart0_Com();

		if(AGMzeroLevel_cnt > 3333)
			mstAgm.currPoint = 0;
		else ;
	}


	AGM_PowerPoint_cnt = 0;
	while(AGM_PowerPoint_cnt <= 20)
	{
		WDTE = 0xACU;
		if(mstAgm.currPoint == 0 && mstAgm.stepping == 1 && AGMzeroLevel_cnt < 1500)
		{ // 225ms
			AGM_PowerPoint_en = 0;
			AGM_PowerPoint_cnt = 0;
			AGM_Zero_en = 1;
			if(AGM_Zero_cnt >= 80)
			{ // 15s
				err_AGMzerofault = 1;
				AGMzeroLevel_cnt = 1500;
				AGM_PowerPoint_en = 1;
				AGM_PowerPoint_cnt = 30;
			}
		}
		else
		{
			if(mstAgm.currPoint == 0)
			{
				AGM_PowerPoint_en = 1;
				if(mstAgm.stepping == 1 && AGMzeroLevel_cnt >= 1500)
				{
					err_AGMzerofault = 0;
					mstAgm.currPoint = 0;
				}
			}
		}

		if(ncp == true)
			NCP_A_Com();
		else ;
		Uart0_Com();
	}

	if(err_AGMStuckfault == 1 || err_AGMzerofault == 1)
		err_AGMfault = 1;
	else
	{
		err_AGMfault = 0;
		mstAgm.init = true;	// 191114:13h lgh agm.
	}
}


/* ########################################################
	AGM Fault Check
 ########################################################*/
void AGM_Fault_Check(void)
{
	bool initBit=false;
	static uint32_t errDelayTmr=0;

	initBit = ( (mbAgmInit == true) && (mbDhmInit == false) )? true: false;
	// Zero
	if(err_AGMfault == 0 && FlameExt_4time == 0 && initBit == false )
	{
		AGM_Initial_try_en = 0;
		AGM_AutoReset_cnt = 0;
		if(mstAgm.gallPoint == 0 && mstAgm.currPoint == 0 && mstAgm.stepping == 1 && AGMzeroLevel_cnt < 1500) {
			AGM_NonZero_en = 1;
			if(AGM_NonZero_cnt >= 150) { // 15s
				err_AGMzerofault = 1;
				AGMzeroLevel_cnt = 1500;
			}
		}
		else {
			AGM_NonZero_en = 0;
			AGM_NonZero_cnt = 0;
			err_AGMzerofault = 0;
		}
	}
	else
	{
		AGM_NonZero_en = 0;
		AGM_NonZero_cnt = 0;
		AGM_Initial_try_en = 1;
		if(AGM_AutoReset_cnt < 10 && (AGM_Initial_try_cnt >= 200 || FlameExt_4time == 1 || initBit == true))
		{
			AGM_Initial(true);
			mbAgmInit = false;
			AGM_Initial_try_cnt = 0;
			if(FlameExt_4time == 1)
				FlameExt_4time = 0;
			else
				AGM_AutoReset_cnt++;
		}
	}

	if(err_AGMStuckfault == 1 || err_AGMzerofault == 1)
	{
		if(BurnState == true)
		{
			if((gul1msCnt-errDelayTmr) >= 5000)
			{
				errDelayTmr = gul1msCnt-5000;
				err_AGMfault = 1;
			}
			else ;
		}
		else
		 	err_AGMfault = 1;
	}
	else
	{
		err_AGMfault = 0;
		errDelayTmr = gul1msCnt;
	}

	NCP_tab[AgmPercent] = (uint16_t)(mstAgm.currPoint / (AGM_MAXSTEP/100));

}


/* ########################################################
	FAN Pwm Control
 ########################################################*/
void Fan_Control(unsigned int fanCtrl)
{
	uint16_t Fan_Speed=0, fanValue=0;
	static uint16_t FanSpeedPwm=0;

	fanValue = Trial_Fan_Ctrl(RoomPW, (uint16_t *)&NCP_tab[Trial_Fan]);

	if(fanValue > 0)
		Fan_Speed = fanValue;
	else
		Fan_Speed = fanCtrl;

	FanMonitor = Fan_Speed;
	Fan_Speed *= 3;
	if(Fan_Speed > 0)
	{
		if(Fan_Speed > 1200)
			Fan_Speed = 1200;
		else if(Fan_Speed < 180)
			Fan_Speed = 180;
		else ;
	}

	if(err_FanRpm == 1)
	{
		err_FanRpm_CHK_en = 1;
		if(err_FanRpm_CHK_cnt < 30)
			Fan_Speed = 450;
		else if(err_FanRpm_CHK_cnt < 150)
			Fan_Speed = 1;
		else
			err_FanRpm_CHK_cnt = 0;
	}
	else
		err_FanRpm_CHK_en = 0;

	//FanTDR03Chk = FAN_PWM;
	//FanSpeedChk = FANMAXVALUE - Fan_Speed;

	if(Fan_Speed < 1)
	{
		FanPwmStop = 60000;
		Fan_ON = 0;
		Fan1mSPwm_en = 0;
		FanSpeedPwm = 0;
		FanCtrlPwm = 0;
	}
	else
	{
		Fan_ON = 1;
		Fan1mSPwm_en = 1;

		if(err_FanRpm == 0 && FanRpm <= 300)
		{
			FanCtrlPwm = 300;
			FanSpeedPwm = 300;
		}
		else
		{
			//if(Fan1mSPwm >= 10) // * ms.
			if(Fan1mSPwm >= 5) // * ms.
			{
				Fan1mSPwm = 0;
				if(FanSpeedPwm < Fan_Speed)
					FanSpeedPwm++;
				else if(FanSpeedPwm > Fan_Speed)
					FanSpeedPwm--;
				else ;
				FanCtrlPwm = FanSpeedPwm;
			}
			else ;
		}
	}
	NCP_tab[FanPWM] = FanCtrlPwm;//Fan_Speed;
}


/* ########################################################
	Fan Rpm Check.
 ########################################################*/
void rpm_calc(void)
{
	if(FanRpm_time >= 4) // 4ms
	{
		FanRpm_time = 0;
		if(FanRpm_cnt < 100)
		{
			FanRpm_cnt++;
			Rpm_Hz = 10000/RpmCnt_Result;
			RpmHzSum += Rpm_Hz;
			Rpm_Hz = 0;
		}
		else
		{
			RpmHzSum /= 100;
			FanRpm = (unsigned int)(RpmHzSum * 60);
			if(FanRpm > 30000) FanRpm = 0;
			NCP_tab[FanRPM] = FanRpm;
			RpmHzSum = 0;
			Rpm_Hz = 0;
			FanRpm_cnt = 0;
		}
	}

	if(Fan_ON) {  //  mmm
		if(FanRpm <= 100 || FanRpm > 65000) FanRun_NG = 1;
		else if(FanRpm >= 300 && FanRpm <= 65000) FanRun_NG = 0;
	}
	else {
		FanRun_NG = 0;
		FanRpmNG_cnt = 0;
	}

	if(FanRpm <= 100 || FanRpm > 65000) NCP_tab[MachineFunc1] &= 0xffef;
	else if(FanRpm >= 300 && FanRpm <= 65000) NCP_tab[MachineFunc1] |= 0x0010;
}


/* ########################################################
	Flow Sensor Check
 ########################################################*/
void flow_calc(void)
{
	uint8_t flow_ON = 0;
	unsigned int tmp, temp16 = 0, FlowLiterMSB = 0, FlowLiterLSB = 0;
	double FlowFloat_liter = 0;
	static bool beforeCp1=false;
	static uint8_t beforePump=0;
	static uint16_t flowE[3]={0,};
	static uint32_t flowOffTmr=0, bandGapCycle=0;
	static T_BADN_GAP stBandFlow={0,};

	if(munFlowDhwPulsCnt < 5000)
		FlowFloat_liter = (1/((double)munFlowDhwPulsCnt/2 * 0.00004))/7.1;
	else
	{
		FlowFloat_liter = 0;
		FlowDhw_LPF.sum = 0;
	}

	temp16 = (unsigned int)(FlowFloat_liter * 10);
	tmp = Lpf_calc(&FlowDhw_LPF, temp16);
	tmp += (tmp != 0)? 3:0;

	Band_Gap_Error((uint16_t*)&FlowLiter_x10, tmp, 2, &stBandFlow, gul1msCnt);


	FlowLiterMSB = FlowLiter_x10/10;
	FlowLiterMSB = FlowLiterMSB << 8;
	FlowLiterLSB = FlowLiter_x10%10;

	FlowLiter = FlowLiterMSB | FlowLiterLSB;
	NCP_tab[FlowValue] = FlowLiter;	// 24: FlowValue


	if(WaterHeatType == MODEL_PREMIUM)
	{
		if(munFlowRecirPulsCnt < 5000)
			FlowFloat_liter = (1/((float)munFlowRecirPulsCnt/2 * 0.00004))/7.1;
		else
		{
			FlowFloat_liter = 0;
			FlowRecir_LPF.sum = 0;
		}

		temp16 = (unsigned int)(FlowFloat_liter * 10);
		FlowRecirLiter_x10 = Lpf_calc(&FlowRecir_LPF, temp16);
		FlowRecirLiter_x10 += (FlowRecirLiter_x10 != 0)? 3:0;

		FlowLiterMSB = FlowRecirLiter_x10/10;
		FlowLiterMSB = FlowLiterMSB << 8;
		FlowLiterLSB = FlowRecirLiter_x10%10;

		NCP_tab[FlowRecirValue] = FlowLiterMSB | FlowLiterLSB;

		if(BypassMode == true && bRoomRunDemand == true)
		{
			flow_ON = 40;
			if( (flow_op_stat == true) && (FlowLiter_x10 < flow_ON) )
				flow_op_stat = false;
			else ;
		}
		else
			flow_ON = 20;
	}
	else
		flow_ON = 20;

	if(beforePump != out_CP1)
		FlowOnDelayTmr = 50;
	else ;
	beforePump = out_CP1;

/*
	if( (StorageHeat == true) && (out_CP1 == true) && (bRoomRunDemand == true) && FlowLiter_x10 < 12)
		FlowOnDelayTmr = 50;
	else ;
 */



/*
	if(out_CP1 != beforeCp1)	// 191105:15h lgh 펌프에 따른 유량 감지 체크.
	else ;
	beforeCp1 = out_CP1;
 */

	if(FlowLiter_x10 < 12 && FlowOffDelayTmr == 0)	// 191106:15h lgh 15->12.
	{
		if(flow_op_stat == 1)
			FlowOnDelayTmr += 20;
		else ;

		flow_op_stat = 0;

		if( (mstTemp.avgOp > mstTemp.Set+20) && (FlowLiter_x10 < 30) )
			FlowOnDelayTmr += 1;
		else ;
	}
	else if(FlowLiter_x10 >= flow_ON)
	{
		if(FlowOnDelayTmr == 0)
			flow_op_stat = 1;
		else ;

		if(FlowLiter_x10 > 50)
		{
			FlowOffDelayTmr = 5;
			flowOffTmr = gul1msCnt;
		}
		else if( (gul1msCnt - flowOffTmr) > 2000 )
		{
			flowOffTmr = gul1msCnt - 2000;
			FlowOffDelayTmr = 20;
		}
		else
			FlowOffDelayTmr = 10;
	}
	else ;

	if((RoomPW==0) || (flow_op_stat==0))
	{
		Flow_start = 0;
	}

}


/* ########################################################
	ERROR Check
 ########################################################*/
void Error_state(void)
{
	unsigned int TempInt = 0, TempIntA = 0, TempIntB = 0, TempIntC = 0;
	static uint32_t err45maintaintTmr=0, err45ResetTmr=0, err43maintaintTmr=0, err43ResetTmr=0;
	static uint32_t err42maintaintTmr=0, err42ResetTmr=0, err29maintaintTmr=0, err29ResetTmr=0;

	// US & kor
	if(err_DipSW == 1 && Cnt_er73 < 1)
	{
		ErrorON((KoreaType == 0)? 73: 40);
		Cnt_er73 = 1;
	}
	if(err_DipSW == 0)
	{
		ErrorOFF((KoreaType == 0)? 73: 40);
		Cnt_er73 = 0;
	}

	if(err_AGMfault == 1 && Cnt_er67 < 1)
	{
		ErrorON((KoreaType == 0)? 67: 37);
		Cnt_er67 = 1;
	}
	if(err_AGMfault == 0)
	{
		ErrorOFF((KoreaType == 0)? 67: 37);
		Cnt_er67 = 0;
	}

	if(err_DHM_ZeroClose == 1 || err_DHM_ZeroOpen == 1)
	{
		ErrorON((KoreaType == 0)? 66: 32);
		Cnt_er66 = 1;
	}
	if(err_DHM_ZeroClose == 0 && err_DHM_ZeroOpen == 0)
	{
		ErrorOFF((KoreaType == 0)? 66: 32);
		Cnt_er66 = 0;
	}

	if(err_IWM_ZeroClose == 1 || err_IWM_ZeroOpen == 1)
	{
		if(Cnt_er65 == 0)
			ErrorON((KoreaType == 0)? 65: 35);
		else ;
		Cnt_er65 = 1;
	}
	if(err_IWM_ZeroClose == 0 && err_IWM_ZeroOpen == 0)
	{
		ErrorOFF((KoreaType == 0)? 65: 35);
		Cnt_er65 = 0;
	}

	if(mstHighLimit.occur == 1 && Cnt_er20 < 1)
	{
		ErrorON((KoreaType == 0)? 20: 98);
		Cnt_er20 = 1;
	}

	// Er.43 AutoReset
/*
	if(mstBurnLimit.occur == 1 && Cnt_er43 < 1)
	{
		ErrorON((KoreaType == 0)? 43: 18);
		Cnt_er43 = 1;
	}
*/

	if(mstBurnLimit.occur == 1)
	{
		if(Cnt_er43 == 0)
		{
			ErrorON((KoreaType == 0)? 43: 18);
			Cnt_er43 = 1;
			CumulativeCnt_er43 += (CumulativeCnt_er43 < 10)? 1: 0;	// 에러 누적 횟수.
			err43maintaintTmr = gul1secCnt-60;
		}
		else ;

		if((gul1secCnt-err43maintaintTmr) >= 60)	// 1min.	5분 연속 감지 체크.
		{
			err43maintaintTmr = gul1secCnt;
			Cnt_er43 += (Cnt_er43 < 10)? 1: 0;
		}
		else ;
		err43ResetTmr = gul1secCnt;	// 누적 타임 초기화.

	}
	else if( (Cnt_er43 < 5) && (CumulativeCnt_er43 < 5) )
	{
		ErrorOFF((KoreaType == 0)? 43: 18);
		Cnt_er43 = 0;
		if((gul1secCnt-err43ResetTmr) >= 900)	// 15min 유지시 누적 횟수 초기화.
		{
			err43ResetTmr = gul1secCnt - 900;
			CumulativeCnt_er43 = 0;
		}
		else ;
	}else ;

	if(mstEqs.occur == 1)
	{
		if(Cnt_er42 == 0)
		{
			ErrorON((KoreaType == 0)? 42: 21);
			Cnt_er42 = 1;
			CumulativeCnt_er42 += (CumulativeCnt_er42 < 10)? 1: 0;	// 에러 누적 횟수.
			err42maintaintTmr = gul1secCnt-60;
		}
		else ;

		if((gul1secCnt-err42maintaintTmr) >= 60)	// 1min.	5분 연속 감지 체크.
		{
			err42maintaintTmr = gul1secCnt;
			Cnt_er42 += (Cnt_er42 < 10)? 1: 0;
		}
		else ;
		err42ResetTmr = gul1secCnt;	// 누적 타임 초기화.

	}
	else if( (Cnt_er42 < 5) && (CumulativeCnt_er42 < 5) )
	{
		ErrorOFF((KoreaType == 0)? 42: 18);
		Cnt_er42 = 0;
		if((gul1secCnt-err42ResetTmr) >= 900)	// 15min 유지시 누적 횟수 초기화.
		{
			err42ResetTmr = gul1secCnt - 900;
			CumulativeCnt_er42 = 0;
		}
		else ;
	}else ;

/*
	if(mstEqs.occur == 1 && Cnt_er42 < 1)
	{
		if(KoreaType == 0) ErrorON(42);
		else ErrorON(21);
		Cnt_er42 = 1;
	}
*/


	if(mstAps.occur == 1)
	{
		if(Cnt_er29 == 0)
		{
			ErrorON((KoreaType == 0)? 29: 22);
			Cnt_er29 = 1;
			CumulativeCnt_er29 += (CumulativeCnt_er29 < 10)? 1: 0;	// 에러 누적 횟수.
			err29maintaintTmr = gul1secCnt-60;
		}
		else ;

		if((gul1secCnt-err29maintaintTmr) >= 60)	// 1min.	5분 연속 감지 체크.
		{
			err29maintaintTmr = gul1secCnt;
			Cnt_er29 += (Cnt_er29 < 10)? 1: 0;
		}
		else ;
		err29ResetTmr = gul1secCnt;	// 누적 타임 초기화.

	}
	else if( (Cnt_er29 < 5) && (CumulativeCnt_er29 < 5) )
	{
		ErrorOFF((KoreaType == 0)? 29: 18);
		Cnt_er29 = 0;
		if((gul1secCnt-err29ResetTmr) >= 900)	// 15min 유지시 누적 횟수 초기화.
		{
			err29ResetTmr = gul1secCnt - 900;
			CumulativeCnt_er29 = 0;
		}
		else ;
	}else ;

/*
	if(mstAps.occur == 1 && Cnt_er29 < 1)
	{
		ErrorON((KoreaType == 0)? 29: 22);
		Cnt_er29 = 1;
	}
*/

	if(mstErrInlet.occur == 1 && Cnt_er31 < 1)
	{
		ErrorON((KoreaType == 0)? 31: 34);
		Cnt_er31 = 1;
	}
	if(mstErrInlet.occur == 0)
	{
		ErrorOFF((KoreaType == 0)? 31: 34);
		Cnt_er31 = 0;
	}

	if(mstErrExhaust.occur == 1 && Cnt_er35 < 1)
	{
		ErrorON((KoreaType == 0)? 35: 15);
		Cnt_er35 = 1;
	}
	if(mstErrExhaust.occur == 0)
	{
		ErrorOFF((KoreaType == 0)? 35: 15);
		Cnt_er35 = 0;
	}

	if(mstErrDhw.occur == 1 && Cnt_er32 < 1)
	{
		ErrorON((KoreaType == 0)? 32: 14);
		Cnt_er32 = 1;
	}
	if(mstErrDhw.occur == 0)
	{
		ErrorOFF((KoreaType == 0)? 32: 14);
		Cnt_er32 = 0;
	}

	if(mstErrOp.occur && Cnt_er33 < 1)
	{
		ErrorON((KoreaType == 0)? 33: 4);
		Cnt_er33 = 1;
	}
	if(mstErrOp.occur == 0)
	{
		ErrorOFF((KoreaType == 0)? 33: 4);
		Cnt_er33 = 0;
	}

	if(mstErrExOverHeat.reset == 1 && Cnt_Reset94 < 1) {
		Er94off_en = 0;
		Er94off_Cnt = 0;
		Cnt_Reset94 = 1;
		if(Er94Cnt < 2) Er94Cnt++;
		else mstErrExOverHeat.occur = 1;
	}

	if(mstErrExOverHeat.reset == 0) {
		Er94off_en = 1;
		if(Er94off_Cnt >= 6000) Er94Cnt = 0;
		Cnt_Reset94 = 0;
	}

	if(mstErrExOverHeat.occur == 1 && Cnt_er94 < 1)
	{
		ErrorON((KoreaType == 0)? 94: 16);
		Cnt_er94 = 1;
	}

/*
	if(err_McuSafety == 1 && Cnt_er70 < 1)
	{
		if(KoreaType == 0)ErrorON(70);
		else ErrorON(60);
		Cnt_er70 = 1;
	}
	if(err_McuSafety == 0 )	{
		if(KoreaType == 0) ErrorOFF(70);
		else ErrorOFF(60);
		Cnt_er70 = 0;
		err_McuSafety_cnt = 0;
		TestProbe = 0;
	}
 */

	if(mstErrSensorOverHeat.occur == 1 && Cnt_er16 < 1)
	{
		ErrorON((KoreaType == 0)? 16: 99);
		Err16_state = 1;
		Cnt_er16 = 1;
	}
	if(mstErrSensorOverHeat.occur == 0)
	{
		ErrorOFF((KoreaType == 0)? 16: 99);
		Cnt_er16 = 0;
	}

	if(err_Flash == 1 && Cnt_er38 < 1) {
		if(KoreaType == 0) ErrorON(38);
		else ErrorON(67);
		Cnt_er38 = 1;
	}

	if(err_GasDetect == 1 && Cnt_er40 < 1)
	{
		ErrorON((KoreaType == 0)? 40: 97);
		Cnt_er40 = 1;
	}

	if(err_FanRpm == 1 && Cnt_er61 < 1)
	{
		ErrorON((KoreaType == 0)? 61: 11);
		Cnt_er61 = 1;
	}
	if(err_FanRpm == 0)
	{
		ErrorOFF((KoreaType == 0)? 61: 11);
		Cnt_er61= 0;
	}

	if( (err_FanRpm == 1) && (CommonEXT == true) )
	{
		NCP_tab[MachineFunc2] |= 0x4000;	// 200623:11h lgh
		CCm_Set_Err78(CascadeNumber, 0xff);
	}
	else
	{
		NCP_tab[MachineFunc2] &= ~0x4000;
		CCm_Set_Err78(CascadeNumber, 0x00);
	}

	if(err_72Flame == 1 && Cnt_er72 < 1)
	{
		ErrorON((KoreaType == 0)? 72: 2);
		Cnt_er72 = 1;
	}
	if(err_72Flame == 0)
	{
		ErrorOFF((KoreaType == 0)? 72: 2);
		Cnt_er72 = 0;
	}

	if(err_10FlameExt == 1 && Cnt_er10 < 1)
	{
		ErrorON((KoreaType == 0)? 10: 3);
		Cnt_er10 = 1;
	}

	if(err_11ItFault == 1 && Cnt_er11 < 1)
	{
		ErrorON((KoreaType == 0)? 11: 1);// 200318:9h lgh
		Cnt_er11 = 1;
	}

	// Er.41 AutoReset
	if(err_MaxFanRPM >= 1 && Cnt_er41 < 1)
	{
		ErrorON((KoreaType == 0)? 41: 7);
		Cnt_er41 = 1;
		Er41_Reset_en = 1;
		if(ErrMaxRPM_cnt < 250) ErrMaxRPM_cnt++;
	}
	if(Er41_Reset_cnt >= 36000 && ErrMaxRPM_cnt < 5)
	{
		ErrMaxRPM_cnt = 0;
		Er41_Reset_en = 0;
		Er41_Reset_cnt = 0;
	}

	//Cascade Commnication Error
	if( (CC_Get_Communi_ErrCnt(CascadeNumber) == true) && (CascadeOn == true) )
	{
		if(Cnt_er78 == 0)
			ErrorON((KoreaType == 0)? 78: 50);
		else ;
		Cnt_er78 = 1;
	}
	else
	{
		ErrorOFF((KoreaType == 0)? 78: 50);
		Cnt_er78 = 0;
	}

	//if(err_HighLevel == 1 && Cnt_er45 < 1)
	if(err_HighLevel == 1)
	{
		if(Cnt_er45 == 0)
		{
			ErrorON(45);
			Cnt_er45 = 1;
			CumulativeCnt_er45 += (CumulativeCnt_er45 < 10)? 1: 0;	// 에러 누적 횟수.
			err45maintaintTmr = gul1secCnt-60;
		}
		else ;

		if((gul1secCnt-err45maintaintTmr) >= 60)	// 1min.	5분 연속 감지 체크.
		{
			err45maintaintTmr = gul1secCnt;
			Cnt_er45 += (Cnt_er45 < 10)? 1: 0;
		}
		else ;
		err45ResetTmr = gul1secCnt;	// 누적 타임 초기화.

	}
	else if( (Cnt_er45 < 5) && (CumulativeCnt_er45 < 5) )
	{
		ErrorOFF(45);
		Cnt_er45 = 0;
		if((gul1secCnt-err45ResetTmr) >= 900)	// 15min 유지시 누적 횟수 초기화.
		{
			err45ResetTmr = gul1secCnt - 900;
			CumulativeCnt_er45 = 0;
		}
		else ;
	}else ;

	//

	if(err_Er39fault == 1 && Cnt_er39 < 1)
	{
		//ErrorON((KoreaType == 0)? 39: 2);
		if(KoreaType == 0)
		{
			ErrorON((KoreaType == 0)? 39: 0);
			Cnt_er39 = 1;
		}
	}
	else if(err_Er39fault == 0)
	{
		//ErrorOFF((KoreaType == 0)? 39: 2);
		if(KoreaType == 0)
		{
			ErrorOFF((KoreaType == 0)? 39: 0);
			Cnt_er39 = 0;
		}
	}

	if( (WaterHeatType == MODEL_PREMIUM) && (MCU_rutin_cnt > 100) )
	{
		///////////////////////
		//// Er.60
		if(flow_op_stat == 1)
		{
			if(DHWCurrTemp >= 800)
				err_CirPump = 1;
			else
			{
				if(DHWCurrTemp <= DHWtargetTemp && OPCurrTemp <= DHWtargetTemp)
					err_CirPump = 0;
				else ;
			}
		}
		else
			err_CirPump = 0;

		if(err_CirPump == 1 && Cnt_er60 < 1)
		{
			CP1_refreeze_en = 0;
			Cnt_er60 = 1;
			ErrorON(60);
		}
		if(err_CirPump == 0)
		{
			ErrorOFF(60);
			Cnt_er60 = 0;
		}
	}
	else
	{
		//ManualReset = 0;
		ErrorOFF(60);
		Cnt_er60 = 0;
	}

	// 191126:13h lgh
	if(mbDisconnectedLine == true)
	{
		if(Cnt_er76 == false)
			ErrorON((KoreaType == 0)? 76: 8);
		else ;
		Cnt_er76 = true;
	}
	else
	{
		ErrorOFF((KoreaType == 0)? 76: 8);
		Cnt_er76 = false;
	}

	// 200623:10h lgh
	if(CascadeNumber == 0)
	{
		if( (CCm_Get_Err78() > 0) && (CommonEXT == true) && (CascadeOn == true) )
		{
			if(Cnt_er79 == 0)
				ErrorON((KoreaType == 0)? 79: 79);
			else ;
			Cnt_er79 = 1;
			NCP_tab[MachineFunc2] |= 0x4000;
		}
		else
		{
			ErrorOFF((KoreaType == 0)? 79: 79);
			Cnt_er79 = 0;
			NCP_tab[MachineFunc2] &= ~0x4000;
		}
	}
	else ;


	//if(err_HIGHcircuit && Cnt_er83 < 1) 	{ErrorON(83);  	Cnt_er83 = 1;}
	//if(err_HIGHcircuit == 0)		{ErrorOFF(83); 	Cnt_er83 = 0;}
	//if(err_LOWcircuit && Cnt_er81 < 1) 	{ErrorON(81);  	Cnt_er81 = 1;}
	//if(err_LOWcircuit == 0)		{ErrorOFF(81); 	Cnt_er81 = 0;}
	//if(err_PowerHz && Cnt_er37 < 1)	{ErrorON(37); 	Cnt_er37 = 1;}
	//if(err_PowerHz == 0)			{ErrorOFF(37); 	Cnt_er37 = 0;}
	///////////

	if(MCU_rutin_cnt >= 50 && RoomPW == 0) ManualReset = 1;

	if((MCU_rutin_cnt >= 50) && (RoomPW == 1) && ManualReset == 1)
	{
		ManualReset = 0;

		// 200416:16h lgh
		ErrorOFF(45);
		Cnt_er45 = 0;
		CumulativeCnt_er45 = 0;

		AGM_AutoReset_cnt = 0;
		ErrorOFF(44);
		mbErr44 = false;
		ErrorOFF(46);
		mbErr46 = false;
		Clr_Recir_Flow_Kind();

		err_10FlameExt = 0;
		FlameExt_en = 0;
		FlameExt_cnt = 0;
		ErrorOFF((KoreaType == 0)? 10: 3);
		Cnt_er10 = 0;
		FlameExt_4time = 0;

		err_11ItFault = 0;
		ItFault_en = 0;
		ItFault_cnt = 0;
		//ErrorOFF((KoreaType == 0)? 12: 1);
		ErrorOFF((KoreaType == 0)? 11: 1);// 200318:9h lgh
		Cnt_er11 = 0;

		err_GasDetect = 0;
		GasInputCount = 0;
		ErrorOFF((KoreaType == 0)? 40: 97);
		Cnt_er40 = 0;

		mstHighLimit.occur = 0;
		ErrorOFF((KoreaType == 0)? 20: 98);
		Cnt_er20 = 0;

		mstBurnLimit.occur = 0;
		ErrorOFF((KoreaType == 0)? 43: 18);
		Cnt_er43 = 0;
		CumulativeCnt_er43 = 0;// 200416:16h lgh
		mstBurnLimit.run = 0;
		mstBurnLimit.cnt = 0;

		mstEqs.occur = 0;
		ErrorOFF((KoreaType == 0)? 42: 21);
		Cnt_er42 = 0;
		CumulativeCnt_er42 = 0;// 200522:16h lgh

		mstAps.occur = 0;
		ErrorOFF((KoreaType == 0)? 29: 22);
		Cnt_er29 = 0;
		CumulativeCnt_er42 = 0;// 200522:16h lgh

		err_Flash = 0;
		if(KoreaType == 0) ErrorOFF(38);
		else ErrorOFF(67);
		Cnt_er38 = 0;
		err_eeprom_cnt = 0;

		err_MaxFanRPM = 0;
		ErrorOFF((KoreaType == 0)? 41: 7);
		Cnt_er41 = 0;
		ErrMaxRPM_cnt = 0;
		Er41_Reset_en = 0;
		Er41_Reset_cnt = 0;

		Er94off_en = 0;
		Er94off_Cnt = 0;
		Er94Cnt = 0;
		ErrorOFF((KoreaType == 0)? 94: 16);
		Cnt_er94 = 0;
		Cnt_Reset94 = 0;
		mstErrExOverHeat.occur = 0;
	}

	//
	if(NCP_tab[CurrError] == 0)
	{
		if(err_HighLevel == 1) 		Cnt_er45 = 0;	// 1
		if(err_DHM_ZeroClose == 1 || err_DHM_ZeroOpen == 1) Cnt_er66 = 0;	// 2
		if(err_IWM_ZeroClose == 1 || err_IWM_ZeroOpen == 1) Cnt_er65 = 0;	// 3
		if(err_LOWcircuit == 1) 	Cnt_er81 = 0;	// 4
		if(mstHighLimit.occur == 1) 		Cnt_er20 = 0;	// 5
		if(mstBurnLimit.occur == 1) 		Cnt_er43 = 0;	// 6
		if(mstEqs.occur == 1) 		Cnt_er42 = 0;	// 7
		if(mstAps.occur == 1) 		Cnt_er29 = 0;	// 8
		if(mstErrExhaust.occur == 1) 		Cnt_er35 = 0;	// 9
		if(mstErrInlet.occur == 1) 		Cnt_er31 = 0;	//9
		if(mstErrDhw.occur == 1) 		Cnt_er32 = 0;	//10
		if(mstErrOp.occur == 1)			Cnt_er33 = 0;	//11
		//if(err_44Fault == 1) 		Cnt_er44 = 0;	//12
		if(mstErrExOverHeat.occur == 1) 	Cnt_er94 = 0;	//13
		if(err_McuSafety == 1) 		Cnt_er70 = 0;	//14
		if(mstErrSensorOverHeat.occur == 1) 	Cnt_er16 = 0;	//15
		if(err_AGMfault == 1) 		Cnt_er67 = 0;	//17
		if(err_Flash == 1) 		Cnt_er38 = 0;	//19
		if(err_GasDetect == 1) 		Cnt_er40 = 0;	//20
		if(err_FanRpm == 1) 		Cnt_er61 = 0;	//21
		if(err_72Flame == 1) 		Cnt_er72 = 0;	//22
		if(err_10FlameExt == 1) 	Cnt_er10 = 0;	//23
		if(err_11ItFault == 1) 		Cnt_er11 = 0;	//24
		if(err_MaxFanRPM == 1) 		Cnt_er41 = 0;	//25
		if(err_Er39fault == 1)		Cnt_er39 = 0;

		if(CC_Get_Communi_ErrCnt(CascadeNumber) == true)
		{
			CC_Set_Communi_ErrCnt();
			Cnt_er78 = 0;
		}
		else ;
	}
	else ;
}

void ErrorON(uint8_t err_ONcode)
{
	NCP_tab[Before5Error] >>= 8;
	ErrorON_buf = NCP_tab[Before4Error];
	ErrorON_buf <<= 8;
	NCP_tab[Before5Error] |= ErrorON_buf;

	NCP_tab[Before4Error] >>= 8;
	ErrorON_buf = NCP_tab[Before3Error];
	ErrorON_buf <<= 8;
	NCP_tab[Before4Error] |= ErrorON_buf;

	NCP_tab[Before3Error] >>= 8;
	ErrorON_buf = NCP_tab[Before2Error];
	ErrorON_buf <<= 8;
	NCP_tab[Before3Error] |= ErrorON_buf;

	NCP_tab[Before2Error] >>= 8;
	ErrorON_buf = NCP_tab[Before1Error];
	ErrorON_buf <<= 8;
	NCP_tab[Before2Error] |= ErrorON_buf;

	NCP_tab[Before1Error] >>= 8;
	ErrorON_buf = (unsigned int)err_ONcode;
	ErrorON_buf <<= 8;
	NCP_tab[Before1Error] |= ErrorON_buf;

	NCP_tab[CurrError] >>= 8;
	NCP_tab[CurrError] |= ErrorON_buf;
	ErrorON_buf = 0;

	WriteFlash_p = 1;;
	WriteFlash_q = 1;;
	WriteFlash_r = 1;;
	WriteFlash_s = 1;;
	WriteFlash_t = 1;;
	WriteFlash_u = 1;;
}

void ErrorOFF(uint8_t err_OFFcode)
{
	ErrorOFF_buf = NCP_tab[CurrError];
	ErrorOFF_buf_LSB = (uint8_t)ErrorOFF_buf;
	ErrorOFF_buf >>= 8;
	ErrorOFF_buf_MSB = (uint8_t)ErrorOFF_buf;

	if(err_OFFcode == ErrorOFF_buf_LSB) {
		NCP_tab[CurrError] &= 0xff00;
		WriteFlash_p = 1;
	}

	if(err_OFFcode == ErrorOFF_buf_MSB) {
		NCP_tab[CurrError] &= 0x00ff;
		NCP_tab[CurrError] <<= 8;
		WriteFlash_p = 1;
	}
}

/* ########################################################
	UARTO Communication
 ########################################################*/
extern volatile unsigned char	mucReSetRxCnt;

void  Uart0_Com(void)
{
	uint8_t i = 0, j = 0;
	uint8_t tmp=0;
	static uint32_t reTryTmr=0;

	tmp = (uint8_t)(NCP_tab[CascadeSet] >> 8);
	if(tmp > 0)
	{
		CascadeNumber = tmp - 1;
		CascadeOn = true;
	}
	else
	{
		CascadeNumber = 0;
		CascadeOn = false;
	}

	CcCurrPercent = (uint8_t)(NCP_tab[PWMValue] & 0x00ff);

	if(flow_op_stat == 1)
		NCP_tab[MachineFunc1] |= 0x4000;
	else
		NCP_tab[MachineFunc1] &= 0xbfff;

	if(DBG_Get_Mode() == true)
	{
		DBG_Uart_Com();
	}
	else if(CascadeOn == false)
	{
		static uint32_t txRunTmr=0;

		if((gul1msCnt-txRunTmr) >= 500)
		{
			txRunTmr = gul1msCnt;
			P_UartTx = ON;
			Uart0_TxFunc();
			Uart0Txend = false;
			R_UART0_Send(Uart0_out_frame, UARTBYTE);
		}
		else if(Uart0Txend == true)
		{
			Uart0Txend = false;
			P_UartTx = OFF;
			R_UART0_Receive(&ReceiveData0, 1);
		}
		else ;
	}
	else if(CascadeNumber == 0)
	{
		uint32_t txSetTmr = (CascadeOn == true)? 2000: 500;
		static bool startRx=false;
		static uint32_t txRunTmr=0;

		if((gul1msCnt-txRunTmr) >= txSetTmr)
		{
			txRunTmr = gul1msCnt;

			P_UartTx = ON;
			Uart0_TxFunc();
			Uart0Txend = false;
			startRx = false;
			R_UART0_Send(Uart0_out_frame, UARTBYTE);
		}
		else if(Uart0Txend == true)
		{
			Uart0Txend = false;
			P_UartTx = OFF;
			R_UART0_Receive(&ReceiveData0, 1);
			startRx = true;
			txRunTmr = gul1msCnt;
		}
		else if(startRx == true)
		{
			P_UartTx = OFF;
			if(Uart0Rxend == true)
			{
				Uart0_RxFunc();
				rxCrc_ok = 0;
				R_UART0_Receive(&ReceiveData0, 1);
				Uart0Rxend = false;
				//txSetTmr = Uart0_Rx_Data[44] * 50;
				//txRunTmr = gul1msCnt;
			}
			else if(mucReSetRxCnt == 0)
			{
				mucReSetRxCnt = 10;
				R_UART0_Receive(&ReceiveData0, 1);
			}
			else ;
		}
	}
	//3 Slave.-------------
	else
	{
		uint32_t rxSetTmr= 120 * (uint32_t)CascadeNumber;
		static bool startTx=false;
		static uint32_t rxRunTmr=0, rxReTryTmr=0;


		if(startTx == true)
		{
			if((gul1msCnt-rxRunTmr) >= rxSetTmr)
			{
				P_UartTx = ON;
				startTx = false;

				Uart0_TxFunc();
				Uart0Txend = false;
				R_UART0_Send(Uart0_out_frame, UARTBYTE);
			}
			else ;
			rxReTryTmr = gul1secCnt;
		}
		else if(Uart0Txend == true)
		{
			Uart0Txend = false;
			P_UartTx = OFF;
			Uart0Rxend = false;
			R_UART0_Receive(&ReceiveData0, 1);
			rxReTryTmr = gul1secCnt;
		}
		else if(Uart0Rxend == true)
		{
			Uart0Rxend = false;
			Uart0_RxFunc();
			if(rxCrc_ok == 1 && Uart0_Rx_Data[44] == 0)
				startTx = true;
			else
				R_UART0_Receive(&ReceiveData0, 1);
			rxRunTmr = gul1msCnt;
			rxReTryTmr = gul1secCnt;
			rxCrc_ok = 0;
		}
		else if((gul1secCnt-rxReTryTmr) >= 10)
		{
			rxReTryTmr = gul1secCnt;
			if(mucReSetRxCnt == 0)
			{
				mucReSetRxCnt = 10;
				R_UART0_Receive(&ReceiveData0, 1);
			}
			else ;
		}
		else ;
	}
}


void Uart0_TxFunc(void)
{
	uint8_t InternalError = 0;
	unsigned int untmp = 0;

	Uart0_out_frame[0] = 0x0d;
	Uart0_out_frame[1] = (uint8_t)(NCP_tab[MachineMode]>>8);
	Uart0_out_frame[2] = (uint8_t)NCP_tab[MachineMode];
	Uart0_out_frame[3] = (uint8_t)(NCP_tab[BoilerKcal]>>8);
	Uart0_out_frame[4] = (uint8_t)NCP_tab[BoilerKcal];
	Uart0_out_frame[5] = (uint8_t)(NCP_tab[RoomFunc]>>8);
	Uart0_out_frame[6] = (uint8_t)NCP_tab[RoomFunc];
	Uart0_out_frame[7] = (uint8_t)(NCP_tab[MachineFunc1]>>8);
	Uart0_out_frame[8] = (uint8_t)NCP_tab[MachineFunc1];
	Uart0_out_frame[9] = (uint8_t)(NCP_tab[MachineFunc2]>>8);

	if(err_state == 0)
		InternalError = 0;
	else
	{
		if(NCP_tab[CurrError] > 0)
			InternalError = 1;
		else if(OP_TempLimit > 0)
			InternalError = 2;
		else if(mstErrExOverHeat.reset > 0)
			InternalError = 3;
		else if(GasDetectON > 0)
			InternalError = 4;
		else
			InternalError = 5;
	}
	Uart0_out_frame[10] = InternalError;
	Uart0_out_frame[11] = (uint8_t)(NCP_tab[PWMValue]>>8);
	Uart0_out_frame[12] = (uint8_t)NCP_tab[PWMValue];

	Uart0_out_frame[13] = (uint8_t)(FanRpm>>8);
	if(Uart0_out_frame[13] == 0x6e)
		Uart0_out_frame[13] = 0x6f;
	else ;
	Uart0_out_frame[14] = (uint8_t)FanRpm;
	if(Uart0_out_frame[14] == 0x6e)
		Uart0_out_frame[14] = 0x6f;
	else ;

	Uart0_out_frame[15] = (uint8_t)(NCP_tab[DHWmax]>>8);
	Uart0_out_frame[16] = (uint8_t)NCP_tab[DHWmax];

	Uart0_out_frame[17] = (uint8_t)NCP_tab[DHWStandby];

	if(AllError == false)	//bit0
		Uart0_out_frame[18] &= ~0x01;
	else
		Uart0_out_frame[18] |= 0x01;

	Uart0_out_frame[19] = (uint8_t)NCP_tab[DHWPriTime];	// ???

	Uart0_out_frame[20] = (uint8_t)(NCP_tab[RoomSet]>>8);
	Uart0_out_frame[21] = (uint8_t)NCP_tab[RoomSet];
#if 1
	Uart0_out_frame[22] = (uint8_t)(NCP_tab[DhwTemp]>>8);
	Uart0_out_frame[23] = (uint8_t)NCP_tab[DhwTemp];
#else
	{
		uint8_t tmp=0;
		Uart0_out_frame[22] = (uint8_t)mstTemp.avgDhw;
		tmp = (uint8_t)(mstTemp.avgDhw * 10);
		tmp = tmp % 10;
		Uart0_out_frame[23] = tmp;//(uint8_t)((mstTemp.avgDhw * 10) % 10);
	}
#endif
	Uart0_out_frame[24] = (uint8_t)(NCP_tab[PumpOPtime]>>8);	// ???
	Uart0_out_frame[25] = (uint8_t)NCP_tab[PumpOPtime];

	Uart0_out_frame[26] = (uint8_t)(NCP_tab[Dhwset]>>8);		// CHtargetTemp
	Uart0_out_frame[27] = (uint8_t)NCP_tab[Dhwset];

	Uart0_out_frame[28] = (uint8_t)(NCP_tab[ChBurnOn_DeltaTemp]>>8);
	Uart0_out_frame[29] = (uint8_t)NCP_tab[ChBurnOn_DeltaTemp];
#if 1
	Uart0_out_frame[30] = (uint8_t)(NCP_tab[OPTemp]>>8);		// OPCurrTemp
	Uart0_out_frame[31] = (uint8_t)NCP_tab[OPTemp];
#else
	{
		uint8_t tmp=0;
		Uart0_out_frame[30] = (uint8_t)mstTemp.Op;
		tmp = (uint8_t)(mstTemp.Op * 10);
		tmp = tmp % 10;
		Uart0_out_frame[31] = tmp;
	}
#endif
	//Uart0_out_frame[32] = (uint8_t)(NCP_tab[BurnPercent]>>8);	// ???
	//Uart0_out_frame[33] = (uint8_t)NCP_tab[BurnPercent];
	Uart0_out_frame[32] = (uint8_t)(DHW_CtrlKcal>>8);	// ???
	Uart0_out_frame[33] = (uint8_t)DHW_CtrlKcal;

	Uart0_out_frame[34] = (uint8_t)(FanMonitor>>8);
	Uart0_out_frame[35] = (uint8_t)FanMonitor;

	Uart0_out_frame[36] = (uint8_t)(NCP_tab[InletTemp]>>8);
	Uart0_out_frame[37] = (uint8_t)NCP_tab[InletTemp];

	Uart0_out_frame[38] = (uint8_t)(NCP_tab[TitleTemp]>>8);
	Uart0_out_frame[39] = (uint8_t)NCP_tab[TitleTemp];

	Uart0_out_frame[40] = (uint8_t)(NCP_tab[ExhaustTemp]>>8);
	Uart0_out_frame[41] = (uint8_t)NCP_tab[ExhaustTemp];

	Uart0_out_frame[42] = (uint8_t)(FlowRecirLiter_x10>>8);
	Uart0_out_frame[43] = (uint8_t)FlowRecirLiter_x10;

	Uart0_out_frame[44] = CascadeNumber;  // Cascade
	Uart0_out_frame[45] = CcTotalNumber;//(uint8_t)NCP_tab[CcTotal];	// Cascase Total

	Uart0_out_frame[46] = (uint8_t)(NCP_tab[CurrError]>>8);  	// CurrError
	Uart0_out_frame[47] = (uint8_t) NCP_tab[CurrError];

	untmp = (unsigned int)MaxFancut;
	Uart0_out_frame[48] = (uint8_t)(untmp>>8);  	// Cut-off rpm
	Uart0_out_frame[49] = (uint8_t)untmp;

	Uart0_out_frame[50] = (uint8_t)(NCP_tab[Year]>>8);
	Uart0_out_frame[51] = (uint8_t)NCP_tab[Year];
	Uart0_out_frame[52] = (uint8_t)(NCP_tab[MonthDate]>>8);
	Uart0_out_frame[53] = (uint8_t)NCP_tab[MonthDate];
	Uart0_out_frame[54] = (uint8_t)(NCP_tab[HourMin]>>8);
	Uart0_out_frame[55] = (uint8_t)NCP_tab[HourMin];
	Uart0_out_frame[56] = (uint8_t)NCP_tab[Week];
	Uart0_out_frame[57] = (uint8_t)(NCP_tab[SRMode_OPTime]>>8);// ???
	Uart0_out_frame[58] = (uint8_t)NCP_tab[SRMode_OPTime];
	Uart0_out_frame[59] = (uint8_t)(NCP_tab[StorOperTemp]>>8);// ???
	Uart0_out_frame[60] = (uint8_t)NCP_tab[StorOperTemp];
	Uart0_out_frame[61] = (uint8_t)NCP_tab[HighAltitude];
	//
	Uart0_out_frame[62] = mucCcBurn01_08;
	Uart0_out_frame[63] = mucCcBurn09_16;
	Uart0_out_frame[64] = mucCcBurn17_20;

	Uart0_out_frame[65] = (uint8_t)(NCP_tab[OPtimeMSB] >> 8);
	Uart0_out_frame[66] = (uint8_t)NCP_tab[OPtimeMSB];	// Burn Operating Time.

	Uart0_out_frame[67] = (uint8_t)(NCP_tab[OPtimeLSB] >> 8);
	Uart0_out_frame[68] = (uint8_t)NCP_tab[OPtimeLSB];	// Burn Operating Time.

/*
	Uart0_out_frame[67] = (uint8_t)(NCP_tab[SR_MonthDate]>>8);// ???
	Uart0_out_frame[68] = (uint8_t)NCP_tab[SR_MonthDate];
*/

	Uart0_out_frame[69] = (uint8_t)(NCP_tab[CHmin]>>8);
	Uart0_out_frame[70] = (uint8_t)NCP_tab[CHmin];
	Uart0_out_frame[71] = (uint8_t)(mstAgm.currPoint>>8);
	Uart0_out_frame[72] = (uint8_t)mstAgm.currPoint;

	untmp = ((DHM_MAXSTEP-DHM_CurrPoint)*10)/25;
	Uart0_out_frame[73] = (uint8_t)(untmp>>8);
	Uart0_out_frame[74] = (uint8_t)untmp;

	Uart0_out_frame[75] = (uint8_t)(NCP_tab[VentType]>>8);
	Uart0_out_frame[76] = (uint8_t)NCP_tab[VentType];
	Uart0_out_frame[77] = (uint8_t)(NCP_tab[ReCPumpMode]>>8);
	Uart0_out_frame[78] = (uint8_t)NCP_tab[ReCPumpMode];

	if(CascadeNumber == 0)
	{
		Uart0_out_frame[79] = (uint8_t)(munCcComVentPwm>>8);
		Uart0_out_frame[80] = (uint8_t)munCcComVentPwm;
	}
	else if( (Burner_ON == 1) && (PostPT == 0) /* || (PostTime_cnt > 0)*/ )
	{
		Uart0_out_frame[79] = Uart0_out_frame[34];
		Uart0_out_frame[80] = Uart0_out_frame[35];
	}
	else
	{
		Uart0_out_frame[79] = 0;
		Uart0_out_frame[80] = 0;
	}

	/*
	if(Uart0_out_frame[79] == 0x6e)
		Uart0_out_frame[79] = 0x6f;
	if(Uart0_out_frame[80] == 0x6e)
		Uart0_out_frame[80] = 0x6f;
	*/
	Uart0_out_frame[81] = (uint8_t)(NCP_tab[CcRot_FzMode]>>8);
	Uart0_out_frame[82] = (uint8_t)NCP_tab[CcRot_FzMode];

	Uart0_out_frame[83] = (uint8_t)(FlowLiter_x10>>8);
	Uart0_out_frame[84] = (uint8_t)FlowLiter_x10;

	Uart0_out_frame[85] = (uint8_t)(NCP_tab[CcOnTemp]>>8);// ???
	Uart0_out_frame[86] = (uint8_t)NCP_tab[CcOnTemp];

	Uart0_out_frame[87] = (uint8_t)NCP_tab[CascadeSet];
	Uart0_out_frame[88] = (uint8_t)NCP_tab[PumpStopTime];
	//
	Uart0_out_frame[89] = (uint8_t)(MAX_KCAL>>8);
	Uart0_out_frame[90] = (uint8_t)MAX_KCAL;
	Uart0_out_frame[91] = (uint8_t)(MIN_KCAL>>8);
	Uart0_out_frame[92] = (uint8_t)MIN_KCAL;

	untmp = ((IWM_MAXSTEP-IWM_CurrPoint)*10)/25;
	Uart0_out_frame[93] = (uint8_t)(untmp>>8);
	Uart0_out_frame[94] = (uint8_t)untmp;

	Uart0_out_frame[95] = (uint8_t)(NCP_tab[FanMaxSet]>>8);
	Uart0_out_frame[96] = (uint8_t)NCP_tab[FanMaxSet];

	Uart0_out_frame[97] = (uint8_t)(NCP_tab[FanMinSet]>>8);
	Uart0_out_frame[98] = (uint8_t)NCP_tab[FanMinSet];

	// Data check UART0 crc-16
	Uart0Tx_crc_value = crc16_ccitt(&Uart0_out_frame[1], 98);
	*((unsigned int*)&Uart0_out_frame[99]) = Uart0Tx_crc_value;

	// UART0 end
	Uart0_out_frame[101] = 0x2f;
	Uart0_out_frame[102] = 0x6e;

	if(CascadeNumber == 0)
	{
		CCm_Chk_State_Burn_Err(Uart0_out_frame[44], Uart0_out_frame[7], Uart0_out_frame[46], &Uart0_out_frame[93]);
		CCm_Set_Connect(Uart0_out_frame[44]);
		CCm_Get_MasterFlow(Uart0_out_frame[44], &Uart0_out_frame[83]);
	}
	else ;
}

void Uart0_RxFunc(void)
{
	unsigned int temp = 0, temp2 = 0, Uart0Rx_crc_value = 0, Uart0rx_crc = 0;
	uint8_t temp8 = 0;
	unsigned long temp_long = 0;

	if( (Uart0_Rx_Data[101] == 0x2f) && (Uart0_Rx_Data[102] == 0x6e) && (Uart0_Rx_Data[44] <= 20) && (Uart0_Rx_Data[44] != CascadeNumber) )
	{
		Uart0Rx_crc_value = crc16_ccitt(&Uart0_Rx_Data[1], 98);
		Uart0rx_crc = *((unsigned int*)&Uart0_Rx_Data[99]);
		if(Uart0Rx_crc_value == Uart0rx_crc )
		{
			rxCrc_ok = 1;

			CC_Set_Communi_ErrCnt();

			//3 Cascade Master Rx ------------
			//if( (CascadeNumber == 0) && (Uart0_Rx_Data[44] != CascadeNumber) )
			if(CascadeNumber == 0)
			{
				//2016.10.21 in korea
				if(Uart0_Rx_Data[44] == 20)	// 중계기.?.
				{
					temp2 = NCP_tab[RoomFunc];
					if((Uart0_Rx_Data[6] & 0x01) > 0)
						NCP_tab[RoomFunc] |= 0x0001;
					else
						NCP_tab[RoomFunc] &= 0xfffe;
					if(temp2 != NCP_tab[RoomFunc])
						WriteFlash_a = 1;
					else ;

					if(Uart0_Rx_Data[83] > 0)	// 설정 온도.
					{
						temp2 = NCP_tab[Dhwset];
						NCP_tab[Dhwset] = (unsigned int)Uart0_Rx_Data[83];
						NCP_tab[Dhwset] <<= 8;
						NCP_tab[Dhwset] |= (unsigned int)Uart0_Rx_Data[84];
						if(NCP_tab[Dhwset] > NCP_tab[DHWmax])
							NCP_tab[Dhwset] = NCP_tab[DHWmax];
						else ;
						if(temp2 != NCP_tab[Dhwset])
							WriteFlash_l = 1;
						else ;
					}
					else ;
				}

				//4 new----------------
				CCm_Set_Connect(Uart0_Rx_Data[44]);
				CCm_Get_OperatingTime(Uart0_Rx_Data[44], &Uart0_Rx_Data[65]);
				CCm_Chk_State_Burn_Err(Uart0_Rx_Data[44], Uart0_Rx_Data[7], Uart0_Rx_Data[46], &Uart0_Rx_Data[93]);
				CCm_Get_MasterFlow(Uart0_Rx_Data[44], &Uart0_Rx_Data[83]);
				CCm_Set_Err78(Uart0_Rx_Data[44], Uart0_Rx_Data[9]);

				munFollowComVentPWM = CCs_Set_ComVentPwm(Uart0_Rx_Data[44], &Uart0_Rx_Data[79], munFollowComVentPWM);

			}
			//3 Cascade Slave Rx -----------------
			else if(CascadeNumber >= 1 && Uart0_Rx_Data[44] == 0)
			{
				// 200623:10h lgh
				if(CommonEXT == true)
					CCm_Set_Err78(0, Uart0_Rx_Data[9]);
				else
					CCm_Set_Err78(0, 0);

				munCcComVentPwm = CCs_Get_ComVentPwm(&Uart0_Rx_Data[79]);

				CCs_Get_BurnDevice_Dat(&Uart0_Rx_Data[62], &mucCcBurn01_08);

				////
				//bit0, All Error Chk in cascade system
				if((Uart0_Rx_Data[18] & 0x01) > 0) AllError = true;
				else AllError = false;

				NCP_tab[DHWmax] = (unsigned int)Uart0_Rx_Data[15];
				NCP_tab[DHWmax] <<= 8;
				NCP_tab[DHWmax] |= (unsigned int)Uart0_Rx_Data[16];

				NCP_tab[DHWPriTime] &= 0xff00;
				NCP_tab[DHWPriTime] |= (unsigned int)Uart0_Rx_Data[19];

				NCP_tab[RoomSet] = (unsigned int)Uart0_Rx_Data[20];
				NCP_tab[RoomSet] <<= 8;
				NCP_tab[RoomSet] |= (unsigned int)Uart0_Rx_Data[21];

				NCP_tab[PumpOPtime] = (unsigned int)Uart0_Rx_Data[24];
				NCP_tab[PumpOPtime] <<= 8;
				NCP_tab[PumpOPtime] |= (unsigned int)Uart0_Rx_Data[25];

				// DHW target Temperature Set
				NCP_tab[Dhwset] = (unsigned int)Uart0_Rx_Data[26];
				NCP_tab[Dhwset] <<= 8;
				NCP_tab[Dhwset] |= (unsigned int)Uart0_Rx_Data[27];

				NCP_tab[ChBurnOn_DeltaTemp] = (unsigned int)Uart0_Rx_Data[28];
				NCP_tab[ChBurnOn_DeltaTemp] <<= 8;
				NCP_tab[ChBurnOn_DeltaTemp] |= (unsigned int)Uart0_Rx_Data[29];

				NCP_tab[BurnPercent] = (unsigned int)Uart0_Rx_Data[32];
				NCP_tab[BurnPercent] <<= 8;
				NCP_tab[BurnPercent] |= (unsigned int)Uart0_Rx_Data[33];

				/// nkkk
				NCP_tab[CcTotal] &= 0xff00;
				NCP_tab[CcTotal] |= (unsigned int)Uart0_Rx_Data[45];
				CcTotalNumber = Uart0_Rx_Data[45];

				NCP_tab[Year] = (unsigned int)Uart0_Rx_Data[50];
				NCP_tab[Year] <<= 8;
				NCP_tab[Year] |= (unsigned int)Uart0_Rx_Data[51];

				NCP_tab[MonthDate] = (unsigned int)Uart0_Rx_Data[52];
				NCP_tab[MonthDate] <<= 8;
				NCP_tab[MonthDate] |= (unsigned int)Uart0_Rx_Data[53];

				NCP_tab[HourMin] = (unsigned int)Uart0_Rx_Data[54];
				NCP_tab[HourMin] <<= 8;
				NCP_tab[HourMin] |= (unsigned int)Uart0_Rx_Data[55];

				NCP_tab[Week] &= 0xff00;
				NCP_tab[Week] |= (unsigned int)Uart0_Rx_Data[56];

				NCP_tab[SRMode_OPTime] = (unsigned int)Uart0_Rx_Data[57];
				NCP_tab[SRMode_OPTime] <<= 8;
				NCP_tab[SRMode_OPTime] |= (unsigned int)Uart0_Rx_Data[58];

				NCP_tab[StorOperTemp] = (unsigned int)Uart0_Rx_Data[59];
				NCP_tab[StorOperTemp] <<= 8;
				NCP_tab[StorOperTemp] |= (unsigned int)Uart0_Rx_Data[60];

				NCP_tab[HighAltitude] &= 0xff00;
				NCP_tab[HighAltitude] |= (unsigned int)Uart0_Rx_Data[61];
				/*
				NCP_tab[SR_Year] = (unsigned int)Uart0_Rx_Data[65];
				NCP_tab[SR_Year] <<= 8;
				NCP_tab[SR_Year] |= (unsigned int)Uart0_Rx_Data[66];
				NCP_tab[SR_MonthDate] = (unsigned int)Uart0_Rx_Data[67];
				NCP_tab[SR_MonthDate] <<= 8;
				NCP_tab[SR_MonthDate] |= (unsigned int)Uart0_Rx_Data[68];
				*/

				NCP_tab[CHmin] = (unsigned int)Uart0_Rx_Data[69];
				NCP_tab[CHmin] <<= 8;
				NCP_tab[CHmin] |= (unsigned int)Uart0_Rx_Data[70];

				NCP_tab[VentType] = (unsigned int)Uart0_Rx_Data[75];
				NCP_tab[VentType] <<= 8;
				NCP_tab[VentType] |= (unsigned int)Uart0_Rx_Data[76];

				NCP_tab[ReCPumpMode] = (unsigned int)Uart0_Rx_Data[77];
				NCP_tab[ReCPumpMode] <<= 8;
				NCP_tab[ReCPumpMode] |= (unsigned int)Uart0_Rx_Data[78];

/*
				if((NCP_tab[ReCPumpMode] & 0x0008) > 0) TitleInput = 1;
				else TitleInput = 0;

				if((NCP_tab[ReCPumpMode] & 0x0004) > 0) IntExtTitleInput = 1;
				else IntExtTitleInput = 0;
 */

				NCP_tab[CcRot_FzMode] = (unsigned int)Uart0_Rx_Data[81];
				NCP_tab[CcRot_FzMode] <<= 8;
				NCP_tab[CcRot_FzMode] |= (unsigned int)Uart0_Rx_Data[82];

				NCP_tab[CcOnTemp] = (unsigned int)Uart0_Rx_Data[85];
				NCP_tab[CcOnTemp] <<= 8;
				NCP_tab[CcOnTemp] |= (unsigned int)Uart0_Rx_Data[86];

				if((Uart0_Rx_Data[9] & 0x80) > 0)
					DeviceTotalchk = true;
				else
					DeviceTotalchk = false;

				NCP_tab[PumpStopTime] &= 0xff00;
				NCP_tab[PumpStopTime] |= (unsigned int)Uart0_Rx_Data[88];

				NCP_tab[DHWStandby] &= 0xff00;
				NCP_tab[DHWStandby] |= (unsigned int)Uart0_Rx_Data[17];

				temp8 = Uart0_Rx_Data[7] & 0x20;
				if(temp8 > 0) NCP_tab[MachineFunc1] |= 0x2000;
				else NCP_tab[MachineFunc1] &= 0xdfff;

				//temp = NCP_tab[RoomFunc] & 0x0001;
				NCP_tab[RoomFunc] = (unsigned int)Uart0_Rx_Data[5];
				NCP_tab[RoomFunc] <<= 8;
				NCP_tab[RoomFunc] |= (unsigned int)Uart0_Rx_Data[6];

				// follow Storage
				if((NCP_tab[RoomFunc] & 0x0002) > 0 || IntExtTitleInput == 1)
					bRoomRunDemand = 1;
				else
					bRoomRunDemand = 0;

				CommonEXT = ((NCP_tab[RoomFunc] & 0x0080) > 0)? true: false;
			}
		}
	}
	else
	{
		memset(Uart0_Rx_Data, 0, sizeof(Uart0_Rx_Data));
	}
}


/* ########################################################
	Internal EERPOM Contrl
 ########################################################*/
void AllRead(void)
{
	uint8_t 	All_Read_cnt = 0;
	int arrStr[49]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
					'1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17','18','19','20','21','22','23'};
	All_Read_en = 1;
	while(All_Read_cnt < 49)
	{
		All_Read_cnt++;
		R_WDT_Restart();
		ReadFlash(arrStr[All_Read_cnt-1]);
	}
	All_Read_en = 0;
	All_Read_cnt = 0;
}

void AllWrite(void)
{
	WriteFlash_a = 1;	// 1
	WriteFlash_b = 1;	// 2
	WriteFlash_c = 1;	// 3
	WriteFlash_d = 1;	// 4
	WriteFlash_e = 1;	// 5
	WriteFlash_f = 1;	// 6
	WriteFlash_g = 1;	// 7
	WriteFlash_h = 1;	// 8
	WriteFlash_i = 1;	// 9
	WriteFlash_j = 1;	// 10
	WriteFlash_k = 1;	// 11
	WriteFlash_l = 1;	// 12
	WriteFlash_m = 1;	// 13
	WriteFlash_n = 1;	// 14
	WriteFlash_o = 1;	// 15
	WriteFlash_p = 1;	// 16
	WriteFlash_q = 1;	// 17
	WriteFlash_r = 1;	// 18
	WriteFlash_s = 1;	// 19
	WriteFlash_t = 1;	// 20
	WriteFlash_u = 1;	// 21
	WriteFlash_v = 1;	// 22
	WriteFlash_w = 1;	// 23
	WriteFlash_x = 1;	// 24
	WriteFlash_y = 1;	// 25
	WriteFlash_z = 1;	// 26
	WriteFlash_1 = 1;	// 27
	WriteFlash_2 = 1;	// 28
	WriteFlash_3 = 1;	// 29
	WriteFlash_4 = 1;	// 30
	WriteFlash_5 = 1;	// 31
	WriteFlash_6 = 1;	// 32
	WriteFlash_7 = 1;	// 33
	WriteFlash_8 = 1;	// 34
	WriteFlash_9 = 1;	// 35
	WriteFlash_10 = 1;	// 36
	WriteFlash_11 = 1;	// 37
	WriteFlash_12 = 1;	// 38
	WriteFlash_13 = 1;	// 39
	WriteFlash_14 = 1;	// 40
	WriteFlash_15 = 1;	// 41
	WriteFlash_16 = 1;	// 42
	WriteFlash_17 = 1;	// 43
	WriteFlash_18 = 1;	// 44
	WriteFlash_19 = 1;	// 45
	WriteFlash_20 = 1;	// 46
	WriteFlash_21 = 1;	// 47
	WriteFlash_22 = 1;	// 48
	WriteFlash_23 = 1;	// 49
}

void ReadFlash(unsigned int type)
{
	/* Initialize EEL requester */
    	dtyEelReq.address_pu08   = 0;
    	dtyEelReq.identifier_u08 = 0;
    	dtyEelReq.command_enu    = 0;
    	dtyEelReq.status_enu     = 0;

	/*== EEL / uninitialized =*/
    	/***************************************************************************/
    	/* FDL initialization processing                                           */
    	/***************************************************************************/
    	/* Initialize FDL parameters */
	 fdlStatus = FDL_Init( &fdl_descriptor_str );
	if(fdlStatus == FDL_OK )
	{
	        FDL_Open();
	        dtyEelReq.status_enu = EEL_Init();

	        if( dtyEelReq.status_enu == EEL_OK )
	        {
	            	EEL_Open();
			do {
				/* Set parameter of EEL_CMD_STARTUP command   */
				dtyEelReq.command_enu = EEL_CMD_STARTUP;		// EEL_CMD_STARTUP command

				/* Execute EEL_CMD_STARTUP command */
				EEL_Execute( &dtyEelReq );

				/* Wait of command processing end */
		                while( dtyEelReq.status_enu == EEL_BUSY )
		                {
		                     EEL_Handler(); /* Check of command end */
		                }

				/* EEPROM emulation block inconsistency error */
				if(dtyEelReq.status_enu == EEL_ERR_POOL_INCONSISTENT)
				{
					if(err_eeprom_cnt > 20) {
						err_eeprom_cnt = 0;
						err_Flash = 1;
						TestProbe = 2;
						break;
					}
					err_eeprom_cnt++;

					/* Set parameter of EEL_CMD_FORMAT command   */
			                dtyEelReq.command_enu = EEL_CMD_FORMAT;

			                /* Execute EEL_CMD_FORMAT command */
			                EEL_Execute( &dtyEelReq );

			                /* Wait of command processing end */
			                while( dtyEelReq.status_enu == EEL_BUSY )
			                {
			                     EEL_Handler();  /* Check of command end */
			                }
				}
			/* If EEL_CMD_FORMAT command is successful,       */
            		/* re-execute EEL_CMD_STARTUP command.            */
            		} while( ( dtyEelReq.command_enu == EEL_CMD_FORMAT ) &&
                     		 ( dtyEelReq.status_enu  == EEL_OK         )  );
		}

		/* EEL_CMD_WRITE command or EEL_CMD_REFRESH command is successful. */
                /* If command is abnormal end, execute EEL_CMD_SHUTDOWN command.   */
		if( dtyEelReq.status_enu == EEL_OK )
		{
			err_McuSafety_cnt = 0;

			//== -> started -> ( EEPROM Emulation state )
			EE2memoryData(type);

			dtyEelReq.command_enu = EEL_CMD_READ;	// EEL_CMD_READ command
			EEL_Execute( &dtyEelReq );

			while( dtyEelReq.status_enu == EEL_BUSY ) {
				EEL_Handler();
			}

		        if(All_Read_en == 0) {
				dtyEelReq.command_enu = EEL_CMD_SHUTDOWN;
	                	EEL_Execute( &dtyEelReq );
		                while( dtyEelReq.status_enu == EEL_BUSY ) {
		                	EEL_Handler();
		                }
			}
		}
		if(All_Read_en == 0) EEL_Close();
        }
        if(All_Read_en == 0) FDL_Close();
}

void EE2memoryData(unsigned int type) {
	switch(type) {
		case 'a':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[RoomFunc];
			dtyEelReq.identifier_u08 = 1;					// Data ID
			break;
		case 'b':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[DHWmax];
			dtyEelReq.identifier_u08 = 2;					// Data ID
			break;
		case 'c':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[CcTotal];
			dtyEelReq.identifier_u08 = 3;					// Data ID
			break;
		case 'd':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[OpNumberMSB];
			dtyEelReq.identifier_u08 = 4;					// Data ID
			break;
		case 'e':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[OpNumberLSB];
			dtyEelReq.identifier_u08 = 5;					// Data ID
			break;
		case 'f':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[FanMaxSet];
			dtyEelReq.identifier_u08 = 6;					// Data ID
			break;
		case 'g':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[FanMinSet];
			dtyEelReq.identifier_u08 = 7;					// Data ID
			break;
		case 'h':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[DHWPriTime];
			dtyEelReq.identifier_u08 = 8;					// Data ID
			break;
		case 'i':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[CascadeSet];
			dtyEelReq.identifier_u08 = 9;					// Data ID
			break;
		case 'j':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[OPtimeMSB];
			dtyEelReq.identifier_u08 = 10;					// Data ID
			break;
		case 'k':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[Chset];
			dtyEelReq.identifier_u08 = 11;					// Data ID
			break;
		case 'l':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[Dhwset];
			dtyEelReq.identifier_u08 = 12;					// Data ID
			break;
		case 'm':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[CcOnTemp];
			dtyEelReq.identifier_u08 = 13;					// Data ID
			break;
		case 'n':
			//dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[CcRot_FzMode];
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[Cumulative_Flow_LSB];// 200811:9h lgh
			dtyEelReq.identifier_u08 = 14;					// Data ID
			break;
		case 'o':
			//dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[PowerTimeLSB];
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[Cumulative_Flow_LSB];// 200811:9h lgh
			dtyEelReq.identifier_u08 = 15;					// Data ID
			break;
		case 'p':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[CurrError];
			dtyEelReq.identifier_u08 = 16;					// Data ID
			break;
		case 'q':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[Before1Error];
			dtyEelReq.identifier_u08 = 17;					// Data ID
			break;
		case 'r':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[Before2Error];
			dtyEelReq.identifier_u08 = 18;					// Data ID
			break;
		case 's':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[Before3Error];
			dtyEelReq.identifier_u08 = 19;					// Data ID
			break;
		case 't':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[Before4Error];
			dtyEelReq.identifier_u08 = 20;					// Data ID
			break;
		case 'u':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[Before5Error];
			dtyEelReq.identifier_u08 = 21;					// Data ID
			break;
		case 'v':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[OPtimeLSB];
			dtyEelReq.identifier_u08 = 22;					// Data ID
			break;
		case 'w':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[PowerTimeMSB];
			dtyEelReq.identifier_u08 = 23;					// Data ID
			break;
		case 'x':
			//dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[MachineMode];
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[PowerTimeLSB];// 200811:9h lgh
			dtyEelReq.identifier_u08 = 24;					// Data ID
			break;
		case 'y':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[CcTargetTemp];
			dtyEelReq.identifier_u08 = 25;					// Data ID
			break;
		case 'z':
			dtyEelReq.address_pu08   = (uint8_t *)&IniMemoryCheck;
			dtyEelReq.identifier_u08 = 26;					// Data ID
			break;
		case '1':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[Boost_AntiFreqTime];    //NCP_tab[BoostTime];
			dtyEelReq.identifier_u08 = 27;						// Data ID
			break;
		case '2':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[ChBurnOn_DeltaTemp];    //NCP_tab[ChBurnOn_DeltaTemp];
			dtyEelReq.identifier_u08 = 28;						// Data ID
			break;
		case '3':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[RoomSet]; 		//NCP_tab[RoomSet];
			dtyEelReq.identifier_u08 = 29;
			break;
		case '4':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[BurnPercent];
			dtyEelReq.identifier_u08 = 30;
			break;
		case '5':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[PumpOPtime];
			dtyEelReq.identifier_u08 = 31;
			break;
		case '6':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[AGMset];
			dtyEelReq.identifier_u08 = 32;
			break;
		case '7':
			//dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[Re_DHWpump];
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[CHpump];// 200601:10h lgh
			dtyEelReq.identifier_u08 = 33;
			break;
		case '8':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[VentType];
			dtyEelReq.identifier_u08 = 34;
			break;

		case '9':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[DHWonDelta];
			dtyEelReq.identifier_u08 = 35;
			break;
		case '10':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[CHmin];
			dtyEelReq.identifier_u08 = 36;
			break;
		case '11':
			//dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[StorTargetOn];
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[AhRc_Mode];	// 200113:15h lgh
			dtyEelReq.identifier_u08 = 37;
			break;
		case '12':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[RecyleTemp];
			dtyEelReq.identifier_u08 = 38;
			break;
		case '13':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[HighAltitude];
			dtyEelReq.identifier_u08 = 39;
			break;
		case '14':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[ReCPumpMode];
			dtyEelReq.identifier_u08 = 40;
			break;
		case '15':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[DeltaInlet];
			dtyEelReq.identifier_u08 = 41;
			break;
		case '16':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[PumpStopTime];
			dtyEelReq.identifier_u08 = 42;
			break;
		case '17':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[SRMode_OPTime];
			dtyEelReq.identifier_u08 = 43;
			break;
		case '18':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[SR_Year];
			dtyEelReq.identifier_u08 = 44;
			break;
		case '19':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[SR_MonthDate];
			dtyEelReq.identifier_u08 = 45;
			break;
		case '20':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[StorOperTemp];
			dtyEelReq.identifier_u08 = 46;
			break;
		case '21':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[Year];
			dtyEelReq.identifier_u08 = 47;
			break;
		case '22':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[SR_OpTime];
			dtyEelReq.identifier_u08 = 48;
			break;
		case '23':
			dtyEelReq.address_pu08   = (uint8_t *)&NCP_tab[DHWStandby];
			dtyEelReq.identifier_u08 = 49;
			break;
		default :
			break;
	}
}

void WriteFlash(unsigned int type)
{
	/* Initialize EEL requester */
    	dtyEelReq.address_pu08   = 0;
    	dtyEelReq.identifier_u08 = 0;
    	dtyEelReq.command_enu    = 0;
    	dtyEelReq.status_enu     = 0;

	/*== EEL / uninitialized =*/
    	/***************************************************************************/
    	/* FDL initialization processing                                           */
    	/***************************************************************************/
    	/* Initialize FDL parameters */
	fdlStatus = FDL_Init( &fdl_descriptor_str );
	if(fdlStatus == FDL_OK ) {
	        FDL_Open();
	        dtyEelReq.status_enu = EEL_Init();

	        if( dtyEelReq.status_enu == EEL_OK ) {
	            	EEL_Open();
			do {
		                dtyEelReq.command_enu = EEL_CMD_STARTUP;
		               	EEL_Execute( &dtyEelReq );

		                while( dtyEelReq.status_enu == EEL_BUSY ) {
		                     EEL_Handler();
		                }

				if(WriteFlash_z == 1 && EelFormat_en == 1) {
					EelFormat_en = 0;
					dtyEelReq.command_enu = EEL_CMD_FORMAT;
			       		EEL_Execute( &dtyEelReq );

			                while( dtyEelReq.status_enu == EEL_BUSY ) {
			                       EEL_Handler();
			                }
				}

		                /* EEPROM emulation block inconsistency error */
		                if( dtyEelReq.status_enu == EEL_ERR_POOL_INCONSISTENT )
		                {
		                    dtyEelReq.command_enu = EEL_CMD_FORMAT;
		                    EEL_Execute( &dtyEelReq );

		                    while( dtyEelReq.status_enu == EEL_BUSY ) {
		                        EEL_Handler();
		                    }
		                }
		         } while( ( dtyEelReq.command_enu == EEL_CMD_FORMAT ) &&
		                     ( dtyEelReq.status_enu  == EEL_OK         )  );

			if( dtyEelReq.status_enu == EEL_OK ) {
				do {
					EE2memoryData(type);

					dtyEelReq.command_enu    = EEL_CMD_WRITE; 	// EEL_CMD_WRITE command
					EEL_Execute( &dtyEelReq );

				        while( dtyEelReq.status_enu == EEL_BUSY ){
						EEL_Handler();
					}

				         /* Pool full error */
				        if (dtyEelReq.status_enu == EEL_ERR_POOL_FULL) {
				               dtyEelReq.command_enu    = EEL_CMD_REFRESH;
				               EEL_Execute( &dtyEelReq );
				               while( dtyEelReq.status_enu == EEL_BUSY ) {
				                	EEL_Handler();
				               }
				        }
			        } while( ( dtyEelReq.command_enu == EEL_CMD_REFRESH ) &&
		                 	( dtyEelReq.status_enu  == EEL_OK          )  );

				// Maintenance mode
				if( dtyEelReq.status_enu == EEL_OK )
				{
					if(EELclean_cnt < 250)EELclean_cnt++;
					if(EELclean_cnt > 10) EELclean_cnt = 0;
					err_eeprom_cnt = 0;

					if( dtyEelReq.status_enu == EEL_OK ) {
						if(WriteFlash_a == 1)	WriteFlash_a = 0;
						else if(WriteFlash_b == 1) WriteFlash_b = 0;
						else if(WriteFlash_c == 1) WriteFlash_c = 0;
						else if(WriteFlash_d == 1) WriteFlash_d = 0;
						else if(WriteFlash_e == 1) WriteFlash_e = 0;
						else if(WriteFlash_f == 1) WriteFlash_f = 0;
						else if(WriteFlash_g == 1) WriteFlash_g = 0;
						else if(WriteFlash_h == 1) WriteFlash_h = 0;
						else if(WriteFlash_i == 1) WriteFlash_i = 0;
						else if(WriteFlash_j == 1) WriteFlash_j = 0;
						else if(WriteFlash_k == 1) WriteFlash_k = 0;
						else if(WriteFlash_l == 1) WriteFlash_l = 0;
						else if(WriteFlash_m == 1) WriteFlash_m = 0;
						else if(WriteFlash_n == 1) WriteFlash_n = 0;
						else if(WriteFlash_o == 1) WriteFlash_o = 0;
						else if(WriteFlash_p == 1) WriteFlash_p = 0;
						else if(WriteFlash_q == 1) WriteFlash_q = 0;
						else if(WriteFlash_r == 1) WriteFlash_r = 0;
						else if(WriteFlash_s == 1) WriteFlash_s = 0;
						else if(WriteFlash_t == 1) WriteFlash_t = 0;
						else if(WriteFlash_u == 1) WriteFlash_u = 0;
						else if(WriteFlash_v == 1) WriteFlash_v = 0;
						else if(WriteFlash_w == 1) WriteFlash_w = 0;
						else if(WriteFlash_x == 1) WriteFlash_x = 0;
						else if(WriteFlash_y == 1) WriteFlash_y = 0;
						else if(WriteFlash_1 == 1) WriteFlash_1 = 0;
						else if(WriteFlash_2 == 1) WriteFlash_2 = 0;
						else if(WriteFlash_3 == 1) WriteFlash_3 = 0;
						else if(WriteFlash_4 == 1) WriteFlash_4 = 0;
						else if(WriteFlash_5 == 1) WriteFlash_5 = 0;
						else if(WriteFlash_6 == 1) WriteFlash_6 = 0;
						else if(WriteFlash_7 == 1) WriteFlash_7 = 0;
						else if(WriteFlash_8 == 1) WriteFlash_8 = 0;
						else if(WriteFlash_9 == 1) WriteFlash_9 = 0;
						else if(WriteFlash_10 == 1) WriteFlash_10 = 0;
						else if(WriteFlash_11 == 1) WriteFlash_11 = 0;
						else if(WriteFlash_12 == 1) WriteFlash_12 = 0;
						else if(WriteFlash_13 == 1) WriteFlash_13 = 0;
						else if(WriteFlash_14 == 1) WriteFlash_14 = 0;
						else if(WriteFlash_15 == 1) WriteFlash_15 = 0;
						else if(WriteFlash_16 == 1) WriteFlash_16 = 0;
						else if(WriteFlash_17 == 1) WriteFlash_17 = 0;
						else if(WriteFlash_18 == 1) WriteFlash_18 = 0;
						else if(WriteFlash_19 == 1) WriteFlash_19 = 0;
						else if(WriteFlash_20 == 1) WriteFlash_20 = 0;
						else if(WriteFlash_21 == 1) WriteFlash_21 = 0;
						else if(WriteFlash_22 == 1) WriteFlash_22 = 0;
						else if(WriteFlash_23 == 1) WriteFlash_23 = 0;
						else if(WriteFlash_z == 1) {
							WriteFlash_z = 0;
							All_Write_en = 0;
						}
					}
				}
				else	// error massage
				{
					if(err_eeprom_cnt > 100) {
						err_eeprom_cnt = 0;
						err_Flash = 1;
					}
					err_eeprom_cnt++;
				}
			}

			dtyEelReq.command_enu = EEL_CMD_SHUTDOWN;
		        EEL_Execute( &dtyEelReq );
		        while( dtyEelReq.status_enu == EEL_BUSY ) {
				EEL_Handler();
		        }
		}
		EEL_Close();
        }
        FDL_Close();
}


/* ########################################################
	AD CHECK
 ########################################################*/
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int16_t Read_Temp(uint16_t in, stSensorTable rtd)
{
	if(rtd.pst[0].Adc_Data <= in)
		return(rtd.pst[0].Temp); 				// Min
	else if(rtd.pst[rtd.st_Size-1].Adc_Data >= in)
		return(rtd.pst[rtd.st_Size-1].Temp); 	// Max
	else ;

	//Look Up Table Scan
	if(rtd.pst[rtd.st_Index].Adc_Data > in){
		while(!((rtd.pst[rtd.st_Index].Adc_Data >= in) && (rtd.pst[rtd.st_Index+1].Adc_Data < in)))
			rtd.st_Index++;
	}else{
		while(!((rtd.pst[rtd.st_Index].Adc_Data >= in) && (rtd.pst[rtd.st_Index+1].Adc_Data < in)))
			rtd.st_Index--;
	}

	//Mapping
	return(	(int16_t)map((long)in,						//(input)
			(long)(rtd.pst[rtd.st_Index].Adc_Data),		//(in_min)
			(long)(rtd.pst[rtd.st_Index+1].Adc_Data),	//(in_max)
			(long)(rtd.pst[rtd.st_Index].Temp),			//(out_min)
			(long)(rtd.pst[rtd.st_Index+1].Temp)) );	//(out_max)
}

void AD2degree(uint8_t AD_no)
{
	int16_t outTemp=0, tmp=0;
	static int16_t abeforeTemp[5]={0,};
	float fTemp=0;

	ADNo_CurrAvg = temp_average[AD_no];

	// AD OPEN/SHORT error check
	if(AD_no <= 4)
	{
		if(ADNo_CurrAvg >= ADOPEN_HEX || ADNo_CurrAvg <= ADSHORT_HEX)
		{
			if(AD_no == 0) mstErrTitle.run = 1;
			if(AD_no == 1) mstErrExhaust.run = 1;
			if(AD_no == 2) mstErrInlet.run = 1;
			if(AD_no == 3) mstErrDhw.run = 1;
			if(AD_no == 4) mstErrOp.run = 1;

		}
		else if(ADNo_CurrAvg <= ADOPEN_RELEASE_HEX && ADNo_CurrAvg >= ADSHORT_RELEASE_HEX)
		{
			if(AD_no == 0) mstErrTitle.run = 0;
			if(AD_no == 1) mstErrExhaust.run = 0;
			if(AD_no == 2) mstErrInlet.run = 0;
			if(AD_no == 3) mstErrDhw.run = 0;
			if(AD_no == 4) mstErrOp.run = 0;
		}
	}

	// * 100ms.
	if(mstErrExhaust.cnt >= 20) mstErrExhaust.occur = 1;
	else mstErrExhaust.occur = 0;
	if(mstErrInlet.cnt >= 20) mstErrInlet.occur = 1;
	else mstErrInlet.occur = 0;
	if(mstErrDhw.cnt >= 20) mstErrDhw.occur = 1;
	else mstErrDhw.occur = 0;
	if(mstErrOp.cnt >= 20) mstErrOp.occur = 1;
	else mstErrOp.occur = 0;
	if(mstErrTitle.cnt >= 20) mstErrTitle.occur = 1;
	else mstErrTitle.occur = 0;

	if(AD_no == 0)
		outTemp = (short)Read_Temp((unsigned short)ADNo_CurrAvg, stTitleTb);
	else
		outTemp = (short)Read_Temp((unsigned short)ADNo_CurrAvg, stTempTb);


	//if( (outTemp - abeforeTemp[AD_no]) > 50 || (outTemp - abeforeTemp[AD_no]) < -50)
	if( (outTemp - abeforeTemp[AD_no]) > 80 || (outTemp - abeforeTemp[AD_no]) < -80)
	{
		abeforeTemp[AD_no] = outTemp;
	}
	else ;
	ADNo_2CurrDegree = abeforeTemp[AD_no];

	fTemp = (float)outTemp /100.0f;
	if(AD_no == 0)
	{
		mstTemp.Title = fTemp;
		tmp = ((unsigned short)abeforeTemp[AD_no] / 100) << 8;
		NCP_tab[TitleTemp] = tmp + ((unsigned short)(abeforeTemp[AD_no] / 10) % 10);
	}
	else if(AD_no == 1)
	{
		mstTemp.Exhaust = fTemp;
		tmp = ((unsigned short)abeforeTemp[AD_no] / 100) << 8;
		NCP_tab[ExhaustTemp] = tmp + ((unsigned short)(abeforeTemp[AD_no] / 10) % 10);
	}
	else if(AD_no == 2)
	{
		mstTemp.Inlet = fTemp;
		tmp = ((unsigned short)abeforeTemp[AD_no] / 100) << 8;
		NCP_tab[InletTemp] = tmp + ((unsigned short)(abeforeTemp[AD_no] / 10) % 10);
	}
	else if(AD_no == 3)
	{
		mstTemp.Dhw = fTemp;
		tmp = ((unsigned short)abeforeTemp[AD_no] / 100) << 8;
		NCP_tab[DhwTemp] = tmp + ((unsigned short)(abeforeTemp[AD_no] / 10) % 10);
	}
	else if(AD_no == 4)
	{
		mstTemp.Op = fTemp;
		tmp = ((unsigned short)abeforeTemp[AD_no] / 100) << 8;
		NCP_tab[OPTemp] = tmp + ((unsigned short)(abeforeTemp[AD_no] / 10) % 10);
	}
	else ;

	mstTemp.Set = DHWtargetTemp * 0.1f;
}
// AD Average and AD DEGREE


// AD Average and AD DEGREE
void AD_chk(void)
{
	unsigned long temp = 0;

	if(adchk_cnt >= 2) {
		adchk_cnt = 0;
		R_ADC_Start();
	}

	if(ad0chk_ok) {		// Title sensor
		ad0chk_ok = 0;
		AD2degree(0);
	}
	if(ad1chk_ok) {			// Exhaust Temp
		ad1chk_ok = 0;
		AD2degree(1);
	}
	if(ad2chk_ok) {			// Inlet Temp
		ad2chk_ok = 0;
		AD2degree(2);
	}
	if(ad3chk_ok) {			// DHW Temp
		ad3chk_ok = 0;
		AD2degree(3);
	}
	if(ad4chk_ok) {			// OP Temp
		ad4chk_ok = 0;
		AD2degree(4);
	}
	if(ad5chk_ok)			//Flame_Sensor
	{
		ad5chk_ok = 0;

		Flame_ad = temp_average[5];		// about blow 2.5V, fire detect ON(Fire_Sensor_stat = 1U)
		if(Flame_ad <= 500)
		{
			NCP_tab[MachineFunc1] |= 0x0080;
			Fire_flag = 1;
		}
		else ;

		if (Flame_ad >= 650)		// about over 3.2V, fire detect Off(Fire_Sensor_stat = 0U)
		{
			FireOff_delay_en = 1;
			if(FireOff_delay_cnt > 10) { // 1s
				NCP_tab[MachineFunc1] &= ~0x0080;
				Fire_flag = 0;
				FireOff_delay_en = 0;
				FireOff_delay_cnt = 0;
			}
		}

 		if(MCU_rutin_cnt >= 20)
			err_72Flame = ((Er72Flame_chk==1) && (Fire_flag == 1))? 1: 0;
		else ;
	}
}


/* ########################################################
	AGM Range Check
 ########################################################*/
void Agm_Use_Position(T_TBL_TBL *st, uint8_t max)
{
	uint8_t i, cnt=0;

	st->AgmPosition[cnt++] = st->Info[0][3];
	for(i=1; i<max;i++)
	{
		if(st->AgmPosition[cnt-1] != st->Info[i][3])
		{
			st->MaxCal[cnt-1] = st->Info[i][1];			// CAL.

			st->BoundaryIndex[cnt-1] = i;				// Boundary.

			st->AgmPosition[cnt++] = st->Info[i][3];	// AGM.

		}
		else ;
	}
}


/* ########################################################
	Process the Burn Table
 ########################################################*/
void Cal_Tble_Cpy(unsigned int (*tar)[4], unsigned int (*sour)[4], uint8_t max)
{
    int i=0, j=0;
    unsigned int *ptr1, *ptr2;
    for (i = 0; i < max; i++)
    {
        ptr1 = *tar;
        ptr2 = *sour;
        for (j = 0; j < 4; j++)
        {
            *ptr1 = *ptr2;
            ptr1++;
            ptr2++;
        }
        tar++;
        sour++;
    }
}

uint8_t Cal_Table_Num(void)
{
	static uint8_t ctmp=0, cal=0, high=0, dip=0;

	if( (mstDip.kcalInfo == cal) && (HighAltitudeMode == high) && (mstDip.sw.d5 == dip) )
	{
		return ctmp;
	}
	else
	{
		uint8_t i, cnt=0, maxCnt=0;

		cal = mstDip.kcalInfo;
		high = HighAltitudeMode;
		dip = mstDip.sw.d5;
		/* 		Cal	.	Feets(4:10,000/2:8,000/1:5,000) .	LNG/PG(1:LNG/0:LPG)		*/
		if( (mstDip.kcalInfo == 5) && (HighAltitudeMode == 4) && (mstDip.sw.d5 == 1) )
		{
			ctmp = 1;
			maxCnt = 74;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable150LNG10, maxCnt);
		}
		else if( (mstDip.kcalInfo == 5) && (HighAltitudeMode == 4) && (mstDip.sw.d5 == 0) )
		{
			ctmp = 2;
			maxCnt = 56;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable150LPG10, maxCnt);
		}
		else if( (mstDip.kcalInfo == 5) && (HighAltitudeMode == 2) && (mstDip.sw.d5 == 1) )
		{
			ctmp = 3;
			maxCnt = 74;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable150LNG8, maxCnt);
		}
		else if( (mstDip.kcalInfo == 5) && (HighAltitudeMode == 2) && (mstDip.sw.d5 == 0) )
		{
			ctmp = 4;
			maxCnt = 56;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable150LPG8, maxCnt);
		}
		else if( (mstDip.kcalInfo == 5) && (HighAltitudeMode == 1) && (mstDip.sw.d5 == 1) )
		{
			ctmp = 5;
			maxCnt = 74;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable150LNG5, maxCnt);
		}
		else if( (mstDip.kcalInfo == 5) && (HighAltitudeMode == 1) && (mstDip.sw.d5 == 0) )
		{
			ctmp = 6;
			maxCnt = 56;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable150LPG5, maxCnt);
		}
		else if( (mstDip.kcalInfo == 5) && (HighAltitudeMode == 0) && (mstDip.sw.d5 == 1) )
		{
			ctmp = 7;
			maxCnt = 74;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable150LNG0, maxCnt);
		}
		else if( (mstDip.kcalInfo == 5) && (HighAltitudeMode == 0) && (mstDip.sw.d5 == 0) )
		{
			ctmp = 8;
			maxCnt = 56;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable150LPG0, maxCnt);
		}
		else if( (mstDip.kcalInfo == 7) && (HighAltitudeMode == 4) && (mstDip.sw.d5 == 1) )
		{
			ctmp = 9;
			maxCnt = 56;//64;// 200612:16h lgh
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LNG10, maxCnt);
		}
		else if( (mstDip.kcalInfo == 7) && (HighAltitudeMode == 4) && (mstDip.sw.d5 == 0) )
		{
			ctmp = 10;
			maxCnt = 59;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LPG10, maxCnt);
		}
		else if( (mstDip.kcalInfo == 7) && (HighAltitudeMode == 2) && (mstDip.sw.d5 == 1) )
		{
			ctmp = 11;
			maxCnt = 58;//64;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LNG5_8, maxCnt);
		}
		else if( (mstDip.kcalInfo == 7) && (HighAltitudeMode == 2) && (mstDip.sw.d5 == 0) )
		{
			ctmp = 12;
			maxCnt = 62;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LPG5_8, maxCnt);
		}
		else if( (mstDip.kcalInfo == 7) && (HighAltitudeMode == 1) && (mstDip.sw.d5 == 1) )
		{
			ctmp = 13;
			maxCnt = 58;//64;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LNG5_8, maxCnt);
		}
		else if( (mstDip.kcalInfo == 7) && (HighAltitudeMode == 1) && (mstDip.sw.d5 == 0) )
		{
			ctmp = 14;
			maxCnt = 62;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LPG5_8, maxCnt);
		}
		else if( (mstDip.kcalInfo == 7) && (HighAltitudeMode == 0) && (mstDip.sw.d5 == 1) )
		{
			ctmp = 15;
			maxCnt = 64;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LNG0, maxCnt);
		}
		else if( (mstDip.kcalInfo == 7) && (HighAltitudeMode == 0) && (mstDip.sw.d5 == 0) )
		{
			ctmp = 16;
			maxCnt = 64;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LPG0, maxCnt);
		}
		else if( (mstDip.kcalInfo == 8) && (HighAltitudeMode == 4) && (mstDip.sw.d5 == 1) )
		{
			ctmp = 17;
			maxCnt = 56;//64;// 200612:16h lgh
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LNG10, maxCnt);
		}
		else if( (mstDip.kcalInfo == 8) && (HighAltitudeMode == 4) && (mstDip.sw.d5 == 0) )
		{
			ctmp = 18;
			maxCnt = 59;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LPG10, maxCnt);
		}
		else if( (mstDip.kcalInfo == 8) && (HighAltitudeMode == 2) && (mstDip.sw.d5 == 1) )
		{
			ctmp = 19;
			maxCnt = 58;//64;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LNG5_8, maxCnt);
		}
		else if( (mstDip.kcalInfo == 8) && (HighAltitudeMode == 2) && (mstDip.sw.d5 == 0) )
		{
			ctmp = 20;
			maxCnt = 62;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LPG5_8, maxCnt);
		}
		else if( (mstDip.kcalInfo == 8) && (HighAltitudeMode == 1) && (mstDip.sw.d5 == 1) )
		{
			ctmp = 21;
			maxCnt = 58;//64;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LNG5_8, maxCnt);
		}
		else if( (mstDip.kcalInfo == 8) && (HighAltitudeMode == 1) && (mstDip.sw.d5 == 0) )
		{
			ctmp = 22;
			maxCnt = 62;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LPG5_8, maxCnt);
		}
		else if( (mstDip.kcalInfo == 8) && (HighAltitudeMode == 0) && (mstDip.sw.d5 == 1) )
		{
			ctmp = 23;
			maxCnt = 64;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LNG0, maxCnt);
		}
		else if( (mstDip.kcalInfo == 8) && (HighAltitudeMode == 0) && (mstDip.sw.d5 == 0) )
		{
			ctmp = 24;
			maxCnt = 64;
			Cal_Tble_Cpy(mstFnA.tbl.Info, CALtable199LPG0, maxCnt);
		}
		else ;

		Agm_Use_Position(&mstFnA.tbl, maxCnt);

		for(i=0; i<10; i++)
		{
			if(mstFnA.tbl.AgmMap[i] == mstFnA.tbl.AgmPosition[cnt])
			{
				mstFnA.tbl.MaxPwm[i] = (uint16_t)(strCalCacul[i][ctmp].caculA * mstFnA.tbl.MaxCal[cnt] + strCalCacul[i][ctmp].caculB + strCalCacul[i][ctmp].caculC);
				cnt ++;
			}
			else ;
		}
		mstFnA.tbl.MaxPwm[10] = 400;	// AGM '0' Position Max 400.

	}

	return ctmp;
}



void setFanAGM(uint16_t cal)
{
	static uint8_t setNum=0;

	if(cal == 0)
	{
		Add_PWM = 0;
		mstFnA.addCutRpm = 0;
		mstFnA.targetAgm = 0;
		mstFnA.cutRpm = 0;
	}
	else
	{
		bool outDoorSet=false, nonInterpol=false;
		uint8_t maxCnt=0;
		uint16_t temp = 0, temp_A = 0, temp_B = 0;
		static uint8_t adjustMinStep=0, adjustMaxStep=0;
		static uint16_t bkFanMaxSet=0, bkFanMinSet=0;
		static T_BADN_GAP stBandAgm={0,};

		setNum = Cal_Table_Num();

		mstFnA.tbNum.limitMin = maucCalTblLimitNo[setNum][MIN_LIMIT_NO];
		mstFnA.tbNum.min = maucCalTblLimitNo[setNum][MIN_TABLE_NO];

		if((NCP_tab[ReCPumpMode] & 0x1000) > 0) 		// OutDoor.
		{
			if(mstFnA.tbNum.max != (maucCalTblLimitNo[setNum][MAX_TABLE_NO]+2) )
				outDoorSet = true;
			else ;
			mstFnA.tbNum.max = maucCalTblLimitNo[setNum][MAX_TABLE_NO]+2;
		}
		else
		{
			if(mstFnA.tbNum.max != maucCalTblLimitNo[setNum][MAX_TABLE_NO] )
				outDoorSet = true;
			else ;
			mstFnA.tbNum.max = maucCalTblLimitNo[setNum][MAX_TABLE_NO];
		}
		mstFnA.tbNum.limitMax = maucCalTblLimitNo[setNum][MAX_LIMIT_NO];

		if( (bkFanMaxSet != NCP_tab[FanMaxSet]) || (outDoorSet == true) )
		{
			bkFanMaxSet = NCP_tab[FanMaxSet];
			// Add FAN MAX
			temp_B = mstFnA.tbl.Info[mstFnA.tbNum.max][0] - mstFnA.tbl.Info[mstFnA.tbNum.max-1][0];
			if(temp_B == 0)
				temp_B = 1;
			if(NCP_tab[FanMaxSet] >= 256)
			{
				temp = (NCP_tab[FanMaxSet] - 256)/temp_B;
				temp += mstFnA.tbNum.max;
				adjustMaxStep = mstFnA.tbNum.limitMax - mstFnA.tbNum.max;
				if((uint8_t)temp >= mstFnA.tbNum.limitMax)
					mstFnA.tbNum.max = mstFnA.tbNum.limitMax;
				else
					mstFnA.tbNum.max = (uint8_t)temp;

				temp = mstFnA.tbl.Info[mstFnA.tbNum.max][0];
				FanMaxPWM = temp + ((NCP_tab[FanMaxSet] - 256)%temp_B);
			}
			else
			{
				temp = (256 - NCP_tab[FanMaxSet])/temp_B;
				mstFnA.tbNum.max -= (uint8_t)temp;
				temp_A = mstFnA.tbl.Info[mstFnA.tbNum.max][0];
				FanMaxPWM = temp_A - ((256 - NCP_tab[FanMaxSet])%temp_B);
			}
		}
		else ;


		// Max Calorie Set.
		if(mstFnA.tbNum.max > mstFnA.tbNum.limitMax)
			mstFnA.tbNum.max = mstFnA.tbNum.limitMax;
		else ;

		//MAX_KCAL = mstFnA.tbl.Info[mstFnA.tbNum.max][1] + mstFnA.addMaxCal;
		MAX_KCAL = (mstFnA.addMaxCal > 0)?  mstFnA.tbl.Info[mstFnA.tbNum.limitMax][1]: mstFnA.tbl.Info[mstFnA.tbNum.max][1];
		/* Long Vent 일때 Limit Max 값 사용. */
		maxCnt = (mstFnA.addMaxCal > 0)? mstFnA.tbNum.limitMax: mstFnA.tbNum.max;

		if(bkFanMinSet != NCP_tab[FanMinSet])
		{
			bkFanMinSet = NCP_tab[FanMinSet];
			// Add FAN MIN
			temp_B = mstFnA.tbl.Info[mstFnA.tbNum.min+1][0] - mstFnA.tbl.Info[mstFnA.tbNum.min][0];
			if(temp_B == 0) temp_B = 1;
			if(NCP_tab[FanMinSet] >= 256)
			{
				temp = (NCP_tab[FanMinSet] - 256)/temp_B;
				mstFnA.tbNum.min+=(uint8_t)temp;
				adjustMinStep = mstFnA.tbNum.min;
				temp = mstFnA.tbl.Info[mstFnA.tbNum.min][0];
				FanMinPWM = temp + ((NCP_tab[FanMinSet] - 256)%temp_B);
			}
			else
			{
				temp = (256 - NCP_tab[FanMinSet])/temp_B;
				if(temp >= mstFnA.tbNum.limitMin)
				{
					mstFnA.tbNum.min = 0;
					temp_A = mstFnA.tbl.Info[mstFnA.tbNum.min][0];
					FanMinPWM = temp_A - (((temp-mstFnA.tbNum.limitMin)*temp_B)+((256 - NCP_tab[FanMinSet])%temp_B));
				}
				else
				{
					mstFnA.tbNum.min = mstFnA.tbNum.limitMin - (uint8_t)temp;
					temp_A = mstFnA.tbl.Info[mstFnA.tbNum.min][0];
					FanMinPWM = temp_A - ((256 - NCP_tab[FanMinSet])%temp_B);
				}
			}
			if(FanMinPWM < 50)// 200526:17h lgh 최소값 50.
				FanMinPWM = 50;
			else ;
		}
		else ;

		// Min Calorie Set.
		MIN_KCAL = mstFnA.tbl.Info[mstFnA.tbNum.min][1];

		if(cal <= MIN_KCAL)
		{
			mstFnA.tbNum.findOk = 1;
			TableChange_en = 0;
			mstFnA.tbNum.findIndex = mstFnA.tbNum.min+adjustMinStep;
			mstFnA.tbNum.setIndex = mstFnA.tbNum.findIndex;
			Add_PWM = 0;
			mstFnA.addCutRpm = 0;
		}
		else if(cal >= MAX_KCAL)
		{
			mstFnA.tbNum.findOk = 1;
			TableChange_en = 0;
			mstFnA.tbNum.findIndex = mstFnA.tbNum.max+adjustMaxStep;
			mstFnA.tbNum.setIndex = mstFnA.tbNum.findIndex;
			Add_PWM = 0;
			mstFnA.addCutRpm = 0;
		}
		else
		{

			//mstFnA.tbNum.findIndex = Cal_Info(TTEST_CAL, &mstFnA.tbl, (uint16_t)maxCnt);
			//if(mstFnA.hold == false)
				mstFnA.tbNum.findIndex = Cal_Info(cal, &mstFnA.tbl, (uint16_t)maxCnt);

			if(mstFnA.tbNum.findOk == 0)
			{
				mstFnA.tbNum.findOk = 1;
				TableChange_en = 0;
				mstFnA.tbNum.setIndex = mstFnA.tbNum.findIndex;
			}
			else
			{
				if(Chk_Cal_Forced_Tmr() == true)
				{
					mstFnA.tbNum.setIndex = mstFnA.tbNum.findIndex;
				}
				else if( (mstFnA.tbNum.findIndex < mstFnA.tbNum.setIndex && cal < (mstFnA.tbl.Info[mstFnA.tbNum.setIndex][1]-300)) ||
					(mstFnA.tbNum.findIndex > mstFnA.tbNum.setIndex && cal > (mstFnA.tbl.Info[mstFnA.tbNum.setIndex][1]+300)) )
				//if( (mstFnA.tbNum.findIndex < mstFnA.tbNum.setIndex && TTEST_CAL < (mstFnA.tbl.Info[mstFnA.tbNum.setIndex][1]-300)) ||
				//	(mstFnA.tbNum.findIndex > mstFnA.tbNum.setIndex && TTEST_CAL > (mstFnA.tbl.Info[mstFnA.tbNum.setIndex][1]+300)) )
				{
					uint16_t setTmr=10;

					TableChange_en = 1;
					if(TableChange_cnt >= setTmr)	// * 100ms.
					{
						mstFnA.hold = false;
						mstFnA.tbNum.setIndex = mstFnA.tbNum.findIndex;
					}
					else ;
				}
				else
				{
					TableChange_en = 0;
					TableChange_cnt = 0;
				}
			}


/*
			Add_PWMfunction(cal, mstFnA.tbl.Info[mstFnA.tbNum.setIndex][1],	// Input Cal, Set Cal.
							mstFnA.tbl.Info[mstFnA.tbNum.setIndex+1][1], 	// just above Set Cal.
							mstFnA.tbl.Info[mstFnA.tbNum.setIndex][0], 		// Set PWM.
							mstFnA.tbl.Info[mstFnA.tbNum.setIndex+1][0]);	// just above Set PWM.
 */

		}

		if(mstFnA.tbNum.setIndex <= mstFnA.tbNum.min)
			mstFnA.tbNum.setIndex = mstFnA.tbNum.min;
		else if(mstFnA.tbNum.setIndex > maxCnt)
				mstFnA.tbNum.setIndex = maxCnt;
		else ;

		/* interpolation PWM */
		if(mstFnA.tbNum.setIndex == mstFnA.tbNum.findIndex)
		{
			mstFnA.targetPwm = ( (int16_t)map((long)cal, 								// input cal.
							(long)(mstFnA.tbl.Info[mstFnA.tbNum.setIndex][1]),			// Set Cal.
							(long)(mstFnA.tbl.Info[mstFnA.tbNum.setIndex+1][1]),		// just above Set Cal.
							(long)(mstFnA.tbl.Info[mstFnA.tbNum.setIndex][0]),			// Set Pwm.
							(long)(mstFnA.tbl.Info[mstFnA.tbNum.setIndex+1][0])) ); 	// just above Set Pwm.
		}
		else ;

		/* 보간법에 의한 PWM Down 현상 해결 */
		if(mstFnA.hold == true)
		{

			//if( (mstTemp.fallDhw == false) && (mstTemp.riseDhw == false) )
			{
				float subTemp=mstTemp.Set - mstTemp.avgDhw;
				static uint32_t maintainTmr=0;

				if((gul1msCnt-maintainTmr) >= 500)
				{
					maintainTmr = gul1msCnt;

					if(subTemp < -3)
						mstFnA.holdPwm = -7;
					else if(subTemp < -2)
						mstFnA.holdPwm = -3;
					else if(subTemp < -1)
						mstFnA.holdPwm = -1;
					else if(subTemp > 3)
						mstFnA.holdPwm = 7;
					else if(subTemp > 2)
						mstFnA.holdPwm = 3;
					else if(subTemp > 1)
						mstFnA.holdPwm = 1;
					else ;
				}
			}
			//else ;

			mstFnA.targetPwm += mstFnA.holdPwm;
		}
		else
		{
			mstFnA.holdPwm = 0;

			if(mstFnA.tbl.Info[mstFnA.tbNum.setIndex][0] > mstFnA.targetPwm)
				mstFnA.targetPwm = mstFnA.tbl.Info[mstFnA.tbNum.setIndex][0];
			else ;
		}
		if(mstFnA.targetPwm < FanMinPWM)
			mstFnA.targetPwm = FanMinPWM;
		else ;

		/* interpolation Cut RPM.  */
		mstFnA.cutRpm = ( (int16_t)map((long)cal, 									// input cal.
						(long)(mstFnA.tbl.Info[mstFnA.tbNum.setIndex][1]),			// Set Cal.
						(long)(mstFnA.tbl.Info[mstFnA.tbNum.setIndex+1][1]),		// just above Set Cal.
						(long)(mstFnA.tbl.Info[mstFnA.tbNum.setIndex][2]),			// Set Cut Rpm.
						(long)(mstFnA.tbl.Info[mstFnA.tbNum.setIndex+1][2])) ); 	// just above Set Cup Rpm.
#if 0
		/* interpolation AGM */
		//mstFnA.targetAgm = ( (int16_t)map((long)cal, 								// input cal.
		temp = ( (int16_t)map((long)cal, 								// input cal.
						(long)(mstFnA.tbl.Info[mstFnA.tbNum.setIndex][1]),			// Set Cal.
						(long)(mstFnA.tbl.Info[mstFnA.tbNum.setIndex+1][1]),		// just above Set Cal.
						(long)(mstFnA.tbl.Info[mstFnA.tbNum.setIndex][3]),			// Set Agm.
						(long)(mstFnA.tbl.Info[mstFnA.tbNum.setIndex+1][3])) ); 	// just above Set Agm.

		//mstFnA.targetAgm = Band_Gap_Ctrl(temp, mstFnA.targetAgm, 20);
		Band_Gap_Error(&mstFnA.targetAgm, temp, 2, &stBandAgm, gul1msCnt);

#else
		/* Set AGM.  */
		//mstFnA.targetAgm = mstFnA.tbl.Info[mstFnA.tbNum.setIndex+Agm_Boundary_Chk(cal, mstFnA.targetPwm, &mstFnA, gul1msCnt)][3];

		if(0)//(DeltaT_Ca l_Cnt() > 0)
		{
			if( ( (mstFnA.tbNum.max > mstFnA.tbNum.setIndex) && ( mstFnA.tbNum.setIndex != 0 ) ) &&
				( (mstFnA.tbl.Info[mstFnA.tbNum.setIndex][3] != mstFnA.tbl.Info[mstFnA.tbNum.setIndex+1][3]) ||
				(mstFnA.tbl.Info[mstFnA.tbNum.setIndex][3] != mstFnA.tbl.Info[mstFnA.tbNum.setIndex-1][3]) ) )
			{
				;
			}
			mstFnA.targetAgm = mstFnA.tbl.Info[mstFnA.tbNum.setIndex][3];
		}
		else
		{
			uint16_t currAgm=0, capVal=0;
			static uint16_t beforeAgm=0;
			static uint32_t maintainTmr=0, setTime=1000;

			capVal = 430;////mstFnA.tbl.Info[mstFnA.tbNum.setIndex+1][1] - mstFnA.tbl.Info[mstFnA.tbNum.setIndex][1];
			currAgm = mstFnA.tbl.Info[mstFnA.tbNum.setIndex][3];

			if(beforeAgm == 0)
				setTime = 5000;
			else ;

			if(beforeAgm != currAgm)
			{
				char i;

				for(i=0; i < 10; i++)
				{
					if((cal >= mstFnA.tbl.MaxCal[i]) && (cal < mstFnA.tbl.MaxCal[i+1]))
					{
						uint16_t subCal=0;

						subCal = cal - mstFnA.tbl.MaxCal[i];
						if(subCal > (capVal/2))
						{
							beforeAgm = currAgm;
							maintainTmr = gul1msCnt;
						}
						else if((gul1msCnt-maintainTmr)>=setTime)
						{
							maintainTmr = gul1msCnt;
							beforeAgm = currAgm;
						}
						else ;
						break;
					}
					else if(cal < mstFnA.tbl.MaxCal[i])
					{
						uint16_t subCal=0;

						subCal = mstFnA.tbl.MaxCal[i] - cal;
						if(subCal > capVal)
						{
							beforeAgm = currAgm;
							maintainTmr = gul1msCnt;
						}
						else if((gul1msCnt-maintainTmr)>=setTime)
						{
							maintainTmr = gul1msCnt;
							beforeAgm = currAgm;
						}
						else ;
						break;
					}
					else
					{
						beforeAgm = currAgm;
						maintainTmr = gul1msCnt;
						break;

					}
				}

			}
			else
				maintainTmr = gul1msCnt;

			mstFnA.targetAgm = beforeAgm;
		}

#endif /* AGM_interpolation */

		////////////////////////////
	}

	// Add AGM step
	if(bBurnControl == true)
	{
		static uint8_t agmIndex=0;
		uint16_t ctrlFanValue=0, Add_AGMset=0, MaxPwm=0;

		Add_AGMset = NCP_tab[AGMset] & 0x00ff;
		if(mstFnA.targetAgm >= 50)
		{
			if(Add_AGMset > 100)  // Initial
			{
				//temp = NCP_tab[AGMset] & 0xff00;
				NCP_tab[AGMset] = (NCP_tab[AGMset] & 0xff00) + 50;	// 85: 0x0041
				mstAgm.gallPoint = mstFnA.targetAgm;
				WriteFlash_6 = 1;
			}
			else if(Add_AGMset >= 50)
				mstAgm.gallPoint = mstFnA.targetAgm + (Add_AGMset-50);
			else
				mstAgm.gallPoint = mstFnA.targetAgm - (50-Add_AGMset);
		}
		else
			mstAgm.gallPoint = mstFnA.targetAgm;

		if(mstDip.sw.d7 == 0 && mstDip.sw.d6 == 0)
		{
			static uint32_t addCalTmr=0;
			uint32_t outBasicCal=(uint32_t)((mstTemp.Dhw-mstTemp.avgInlet)*mstTemp.avgFlow*60);

			/* Long Vent Check. */
			//if( (DHW_CtrlKcal >= (MAX_KCAL-300)) && (FanCtrl_Value >= (FanMaxPWM-10)) &&
			if( (DHW_CtrlKcal >= (MAX_KCAL-300)) && (outBasicCal < CAL_CENTER) && (FanCtrl_Value >= (FanMaxPWM-10)) && (mstFnA.addMaxCal == 0) &&
				(mstTemp.Op <= (mstTemp.Set+5)) && (mstTemp.Dhw <= (mstTemp.Set-2)) )
			{
				uint32_t chkTime=50;

				chkTime = (Cal_MaxIncrease_Chk() == true)? 10: 120;

				if( (gul1secCnt - addCalTmr) >= chkTime)	// 5sec.
				{
					addCalTmr = gul1secCnt - chkTime;

					if(mstFnA.addMaxCal == 0)
						mstFnA.addMaxCal = 1;
					else ;
				}
				else ;
			}
			else
				addCalTmr = gul1secCnt;
		}
		else	// 강제 Min/Max에서는 Long Vent 사용 안함.
			mstFnA.addMaxCal = 0;

		MaxPwm = (mstFnA.addMaxCal == 0)? FanMaxPWM : mstFnA.tbl.Info[mstFnA.tbNum.limitMax][0];

		if(DHW_CtrlKcal <= MIN_KCAL)
			FanCtrl_Value = FanMinPWM;
		else if(DHW_CtrlKcal >= MAX_KCAL)
			FanCtrl_Value = MaxPwm;
		else
		{
			static T_BADN_GAP stBandFan={0,};

			if(mstAgm.currPoint == mstAgm.gallPoint)
			{
				for(agmIndex=0; agmIndex<11; agmIndex++)
				{
					if(mstFnA.targetAgm == mstFnA.tbl.AgmMap[agmIndex])
						break;
					else ;
				}
			}
			else
			{
				char i;

				for(i=0; i<11; i++)
				{
					if(mstAgm.currPoint >= mstFnA.tbl.AgmPosition[i])
					{
						for(agmIndex=0; agmIndex<11; agmIndex++)
						{
							if(mstFnA.tbl.AgmPosition[i] == mstFnA.tbl.AgmMap[agmIndex])
								break;
							else ;
						}
						break;
					}
					else ;
				}

				MaxFanRPM_cnt = 0;	// 200210:13h lgh Agm 이동시에는 에러 감지 딜레이.
			}

			//FanCtrl_Value = (uint16_t)(strCalCacul[i][setNum].caculA * cal + strCalCacul[i][setNum].caculB + strCalCacul[i][setNum].caculC);
			ctrlFanValue = mstFnA.targetPwm;//(uint16_t)(strCalCacul[agmIndex][setNum].caculA * cal + strCalCacul[agmIndex][setNum].caculB + strCalCacul[agmIndex][setNum].caculC);

			if(Chk_Cal_Forced_Tmr() == false)
			{
				if( (mstAgm.gallPoint == 0) && (mstAgm.gallPoint < mstAgm.currPoint) )
				{	// 포지션 0 일경우 완전이 이동 후에 PWM 적용.
					if(mstFnA.tbl.MaxPwm[agmIndex-1] != 0)
						ctrlFanValue = (mstFnA.tbl.MaxPwm[agmIndex-1]);
					else
						ctrlFanValue = (mstFnA.tbl.MaxPwm[agmIndex-2]);
				}
				else if(ctrlFanValue > (mstFnA.tbl.MaxPwm[agmIndex] + 0))	// AGM Position 별 Max PWM 제한.
					ctrlFanValue = mstFnA.tbl.MaxPwm[agmIndex];
				else ;

				//FanCtrl_Value = Band_Gap_Ctrl(ctrlFanValue, FanCtrl_Value, 2);
				Band_Gap_Error((uint16_t*)&FanCtrl_Value, ctrlFanValue, 2, &stBandFan, gul1msCnt);
			}
			else
				FanCtrl_Value = ctrlFanValue;

		}

		if(FanCtrl_Value < FanMinPWM)
			FanCtrl_Value = FanMinPWM;
		else if(FanCtrl_Value > MaxPwm)
			FanCtrl_Value = FanMaxPWM;
		else ;

		MaxFancut = mstFnA.cutRpm + 1000 + mstFnA.addCutRpm;
	}
	else
	{
		;
	}
}


uint16_t FLOW_OFF(void)
{

	//return (uint16_t)(0.235 * mstTemp.avgInlet + 3.764);
	//0.1492 X 직수 + 6.1429
	//return (uint16_t)(0.1492 * mstTemp.avgInlet + 6.1429);
	if(1)//if(mstTemp.avgOp >= 70)
		return (uint16_t)(0.133 * mstTemp.avgInlet + 7.333);
	else
		return 6;
}

int OFF_TIME(void)
{
	float per=(mstTemp.avgInlet > 10)? 1: mstTemp.avgInlet * 0.1;
	float fFlow=FlowLiter_x10*0.1;

	//return (int)(0.785*(mstTemp.avgFlow*mstTemp.avgFlow) - (9.585 * mstTemp.avgFlow) + 30.6) * per;
	//return (int)(0.5*(mstTemp.avgFlow*mstTemp.avgFlow) - (6.5 * mstTemp.avgFlow) + 22) * per;
	//return (int)(0.5*(fFlow*fFlow) - (6.5 * fFlow) + 22) * per;

	if(fFlow >= 6)
		return 1;
	else if(fFlow <= 2)
		return 10;
	else
		return (int)(0.4286*(fFlow*fFlow) - (5.6286 * fFlow) + 19.4);// * per;


	//OFF 시간 = 0.4286*유량*유량 -5.6286*유량 +19.4
	//		   (6L 이상시 1초, 2L 이하시 10초)

}

uint16_t MIN_TIME(uint32_t cal)
{
	return (uint16_t)(0.4 * (cal*0.001) - 2) * 10;	// 변화 된 열량을 식으로 대입하여 시간으로 환산.
	//return (uint16_t)(0.4 * (cal*0.001) - 3) * 10;	// 변화 된 열량을 식으로 대입하여 시간으로 환산.
	//return (uint16_t)(0.6 * (cal*0.001) - 3) * 10;	// 변화 된 열량을 식으로 대입하여 시간으로 환산.
}

uint32_t OFF_CAL_SET(void)
{
	//return 15000;	// 버너 OFF 기준 열량 값.
	return (uint32_t)(21.579 - 0.2632 * mstTemp.avgInlet) * 1000;
	//열량 = (-0.2632x직수온도 + 21.579) X 1000
	//ex) 직수온도 10도 -> 18947kcal 변화시 OFF

}
/*
	꺼지는 조건은 FLOW_OFF 조건과 OFF_CAL_SET 조건이 and 조건.
*/


/* End user code. Do not edit comment generated here */
