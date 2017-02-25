/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2009, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	cmm_txbf_cal.c

	Abstract:
	Tx Beamforming calibration and profile related functions

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
	Shiang     2010/07/12
*/

#include "rt_config.h"


/*
	iAtan2 - fixed point atan2. Returns +/-pi. Scaled so pi=0x1000
		Code was taken from MyCordic:
			int MyCordic(int y, int x, int shift, int iter, int *alpha)
			Parameters are hard coded so it's equivalent to MyCordic(y, x, 0, 11, alpha10);
*/
static int iAtan2(int y, int x)
{
	int z = 0, xtemp, i;
	int onepi, halfpi;
	int shift=0;
	int iter=11;
	static int alpha[11] = {0x400, 0x25c, 0x13f, 0x0a2,
				0x051, 0x028, 0x014, 0x00a,
				0x005, 0x002, 0x001};

	onepi = (alpha[0]<<2),
	halfpi = (alpha[0]<<1);

    if (x == 0) {
		if (y == 0)
              z = 0;
        else if (y > 0)
                z = halfpi;
        else
            z = -halfpi;
		}
    else if ((x < 0) && (y == 0)){
            z = -onepi;
		}
    else if ((x > 0) && (y == 0)){
            z = 0;
		}
	else{
		x <<= shift;
		y <<= shift;
		xtemp = x;
		if ((x < 0) && (y > 0)){
            x = y;
            y = -xtemp;
            z = halfpi;
			}
		else if ((x < 0) && (y < 0)){
            x = -y;
            y = xtemp;
            z = -halfpi;
			}
		for (i = 0; i < iter; i++){
			/* printf("%d %d %x\n", x, y, z); */
			if (y == 0)
				break;
			if (y < 0) {
				z -= alpha[i];
				xtemp = x - (y>>i);
				y = y + (x>>i);
				x = xtemp;
			}
			else {
				z += alpha[i];
				xtemp = x + (y>>i);
				y = y - (x>>i);
				x = xtemp;
				}
		}
	}

	if (z == alpha[0]*4)
		z = -(alpha[0]*4);

	return z;
}


/*
	isqrt - fixed point sqrt
		x - unsigned value
*/
static uint32_t isqrt (uint32_t x)
{
	uint32_t base, y;

	if (x &      0xF0000000)
		base = 1<<15;
	else if (x & 0x0F000000)
		base = 1<<13;
	else if (x & 0x00F00000)
		base = 1<<11;
	else if (x & 0x000F0000)
		base = 1<<9;
	else
		base = 1<<7;

    y = 0;
    while (base) {
		y += base;
		if  ((y * y) > x)
			y -= base;
		base >>= 1;
    }
    return y;
}


/*
	icexp - fixed point complex exponential
		phase - 0 to 255 representing 0 to 2pi
		return cos and sin in 1p10 format
*/
static void icexp(short c[2], int phase)
{
	/* cosine table generated with Matlab: round(1024*cos(2*pi*[0:255]/256) */
	static short cosTable[256] = {
		1024, 1024, 1023, 1021, 1019, 1016, 1013, 1009,
		1004, 999, 993, 987, 980, 972, 964, 955,
		946, 936, 926, 915, 903, 891, 878, 865,
		851, 837, 822, 807, 792, 775, 759, 742,
		724, 706, 688, 669, 650, 630, 610, 590,
		569, 548, 526, 505, 483, 460, 438, 415,
		392, 369, 345, 321, 297, 273, 249, 224,
		200, 175, 150, 125, 100, 75, 50, 25,
		0, -25, -50, -75, -100, -125, -150, -175,
		-200, -224, -249, -273, -297, -321, -345, -369,
		-392, -415, -438, -460, -483, -505, -526, -548,
		-569, -590, -610, -630, -650, -669, -688, -706,
		-724, -742, -759, -775, -792, -807, -822, -837,
		-851, -865, -878, -891, -903, -915, -926, -936,
		-946, -955, -964, -972, -980, -987, -993, -999,
		-1004, -1009, -1013, -1016, -1019, -1021, -1023, -1024,
		-1024, -1024, -1023, -1021, -1019, -1016, -1013, -1009,
		-1004, -999, -993, -987, -980, -972, -964, -955,
		-946, -936, -926, -915, -903, -891, -878, -865,
		-851, -837, -822, -807, -792, -775, -759, -742,
		-724, -706, -688, -669, -650, -630, -610, -590,
		-569, -548, -526, -505, -483, -460, -438, -415,
		-392, -369, -345, -321, -297, -273, -249, -224,
		-200, -175, -150, -125, -100, -75, -50, -25,
		0, 25, 50, 75, 100, 125, 150, 175,
		200, 224, 249, 273, 297, 321, 345, 369,
		392, 415, 438, 460, 483, 505, 526, 548,
		569, 590, 610, 630, 650, 669, 688, 706,
		724, 742, 759, 775, 792, 807, 822, 837,
		851, 865, 878, 891, 903, 915, 926, 936,
		946, 955, 964, 972, 980, 987, 993, 999,
		1004, 1009, 1013, 1016, 1019, 1021, 1023, 1024};
	c[0] = cosTable[phase & 0xFF];
	c[1] = cosTable[(phase-64) & 0xFF];
}


/*
	icMult - fixed point complex multiply
		r = a*b
*/
static void icMult(int32_t r[2], int32_t a[2], int32_t b0, int32_t b1)
{
	int32_t t;
	t = a[0]*b0 - a[1]*b1;
	r[1] = a[0]*b1 + a[1]*b0;
	r[0] = t;
}


/*
	------------ DIVIDER AND LNA CALIBRATION --------
*/
typedef	struct {
	LONG	i:8;
	LONG	q:8;
}	COMPLEX_VALUE;		/* Signed 8-bit complex values */

#define CALC_LENGTH		1024				/* Number of samples used to perform phase calculation for LNA or Divider Calibration */
#define CALC_LENGTH_DC	(CALC_LENGTH+512)	/* Number of samples used for DC removal */
#define MAX_CAPTURE_LENGTH		4096			/* Maximum number of samples to capture */
#define DIVCAL_CAPTURE_LENGTH	(CALC_LENGTH+1024)	/* Length of capture for Divider or LNA Calibration */

#define FIXED_M_PI		0x1000						/* Scaling for fixed point PI */
#define DEG(rad)		(radToDeg180(rad-FIXED_M_PI)+180)	/* Convert fixed radians (0x1000=pi) to degrees range [0 360) */
#define DEG180(rad)		radToDeg180(rad)		/* Convert fixed radians (0x1000=pi) to degrees range [-180 180) */

#define BYTE_PHASE_SHIFT		5					/* Shift to convert from byte phase (0x80=pi) to normal phase (0x1000=pi) */
#define CONVERT_TO_BYTE_PHASE(p)	(int)(((p)+(1<<(BYTE_PHASE_SHIFT-1)))>>BYTE_PHASE_SHIFT)	/* Convert from normal phase to byte phase */

#define R65_LNA_LOW		0x4
#define R65_LNA_MID		0x8
#define R65_LNA_HIGH	0xC


/*
	radMod2pi - converts angle in radians to the range [-pi pi)
*/
static LONG radMod2pi(LONG a)
{
	while (a < -FIXED_M_PI)
		a += 2*FIXED_M_PI;
	while (a >= FIXED_M_PI)
		a -= 2*FIXED_M_PI;

	return a;
}


/*
	radToDeg180 - converts angle in radians to the deg range [-180 180)
*/
static int radToDeg180(LONG rad)
{
	return (int)(radMod2pi(rad)*180/FIXED_M_PI);
}


/*
	avgPhase - computes the average phase.
		Phase is adjusted so all values are within the range mPhase[0] +/-pi
			mPhase - values to average (radians)
			pLength - number of values to average
		return average
*/
static LONG avgPhase(LONG mPhase[], int pLength)
{
	int i;
	short cval[2];
	LONG sumCos = 0, sumSin=0;
	for (i=0; i<pLength; i++) {
		icexp(cval, CONVERT_TO_BYTE_PHASE(mPhase[i]));
		sumCos += cval[0];
		sumSin += cval[1];
	}

	return iAtan2(sumSin, sumCos);
}


typedef
	COMPLEX_VALUE (*PCAP_IQ_DATA)[3];	/* CAP_IQ_DATA - Buffer to hold I/Q data for three RX chains */


/*
	RemoveDC - calculate mean and subtract. Return peak values
		peak - used to return the peak value of the three RX chains
		iqData - pointer to array of I/Q data for the three RX chains. DC is removed from the samples
		dataLength - number of samples in iqData
*/
static void RemoveDC(
	IN int peak[3],
	IN COMPLEX_VALUE (*iqData)[3],
	IN int dataLength)
{
	int i, j;
	int dcI[3] = {0, 0, 0};
	int dcQ[3] = {0, 0, 0};

	/* Calculate DC offset for each RX chain */
	for (i=0; i<dataLength; i++) {
		for (j=0; j<3; j++) {
			dcI[j] += iqData[i][j].i;
			dcQ[j] += iqData[i][j].q;
		}
	}

	for (j=0; j<3; j++) {
		dcI[j] /= dataLength;
		dcQ[j] /= dataLength;
	}

	/* Subtract DC and find peak */
	peak[0] = peak[1] = peak[2] = 0;

	for (i=0; i<dataLength; i++) {
		for (j=0; j<3; j++) {
			int sati = iqData[i][j].i - dcI[j];
			int satq = iqData[i][j].q - dcQ[j];

			/* Saturate */
			if (sati > 127)
				sati = 127;
			else if (sati < -128)
				sati = -128;
			iqData[i][j].i = sati;

			if (satq > 127)
				satq = 127;
			else if (satq < -128)
				satq = -128;
			iqData[i][j].q = satq;

			/* Record peak */
			if (peak[j] < iqData[i][j].i)
				peak[j] = iqData[i][j].i;
			if (peak[j] < iqData[i][j].q)
				peak[j] = iqData[i][j].q;
		}
	}
}


/*
	CalcRFCalPhase - process RF calibration to calculate phase of the three channels
		Parameters:
			phase - returns the phase of each channel. Fixed point value scaled so 0x1000 = PI
			avgI, avgQ - returns the avg I/Q of each channel. Implied scale factor of 256
			peak - returns the peak value of each channel after DC removal
			iqData - the input I/Q data for three channels. DC is removed.
			relPhase - If true it returns phase relative to Ant1. Otherwise it returns the
						phase relative to the reference signal.
			actTx - index of an active TX chain, used to detect start of signal
*/
static void CalcRFCalPhase(
	OUT LONG phase[3],
	OUT int avgI[3],
	OUT int avgQ[3],
	OUT int peak[3],
	IN COMPLEX_VALUE (*iqData)[3],
	IN BOOLEAN relPhase,
	IN int actTx)
{
	int i, j;
	LONG sumI[3], sumQ[3];
	static CHAR refSignal[64] = {	/* round(sin(-[0:63]*6*pi/64)*127) - three cycles per 64 samples */
		0, -37, -71, -98, -117, -126, -125, -112,
		-90, -60, -25, 12, 49, 81, 106, 122,
		127, 122, 106, 81, 49, 12, -25, -60,
		-90, -112, -125, -126, -117, -98, -71, -37,
		0, 37, 71, 98, 117, 126, 125, 112,
		90, 60, 25, -12, -49, -81, -106, -122,
		-127, -122, -106, -81, -49, -12, 25, 60,
		90, 112, 125, 126, 117, 98, 71, 37};


	/* Skip the first 200 samples to avoid the transient at the beginning */
	iqData += 200;

	/* Remove DC offset to help with low signal levels */
	RemoveDC(peak, iqData, CALC_LENGTH_DC);

	/* Search active channel to find sample with abs>12 */
	for (i=0; i<(CALC_LENGTH_DC-CALC_LENGTH); i++, iqData++) {
		if ((iqData[0][actTx].i*iqData[0][actTx].i + iqData[0][actTx].q*iqData[0][actTx].q) >= 144)
			break;
	}

	/* Move in 16 samples */
	iqData += 16;

	/* Sum the I and Q then calculate the angle of the sum */
	sumI[0] = sumI[1] = sumI[2] = 0;
	sumQ[0] = sumQ[1] = sumQ[2] = 0;

	for (i=0; i<CALC_LENGTH; i++) {
		/* Either calculate the phase relative to Ant1 or phase relative to reference */
		if (relPhase) {
			sumQ[0] += -iqData[i][0].i*iqData[i][1].q + iqData[i][0].q*iqData[i][1].i;
			sumI[0] +=  iqData[i][0].i*iqData[i][1].i + iqData[i][0].q*iqData[i][1].q;
			sumQ[2] += -iqData[i][2].i*iqData[i][1].q + iqData[i][2].q*iqData[i][1].i;
			sumI[2] +=  iqData[i][2].i*iqData[i][1].i + iqData[i][2].q*iqData[i][1].q;
		}
		else {
			int cval= refSignal[(i+16) % 64];
			int sval= refSignal[i % 64];
			for (j=0; j<3; j++) {
				sumQ[j] += -iqData[i][j].i*sval + iqData[i][j].q*cval;
				sumI[j] +=  iqData[i][j].i*cval + iqData[i][j].q*sval;
			}
		}
	}


	for (i=0; i<3; i++) {
		if (relPhase && i==1) {
			phase[i] = 0;
			avgI[i] = avgQ[i] = 0;
		}
		else {
		phase[i] = iAtan2(sumQ[i]>>6, sumI[i]>>6);
			/* Multiplication by refSignal added a scale factor of 128. Shift left by 1 for 256 scale factor */
			avgI[i] = (sumI[i]<<1)/CALC_LENGTH;
			avgQ[i] = (sumQ[i]<<1)/CALC_LENGTH;
		}
	}
}


#ifdef MT76x2
static VOID CalcDividerPhase(
	IN  struct rtmp_adapter *pAd,
	OUT UCHAR *mPhase0
	)
{
	int i;
	UCHAR  channel = pAd->CommonCfg.Channel;
	uint32_t phaseCaliStatus, phaseCaliResult, rfValue[2], value32;
	UCHAR  divPhCalPath[2]={RX0TX0, RX1TX1};
	UCHAR  timeOutCount, AGCtimeOutCount;
	LONG   mCalPhase0[2];
	LONG   avgIData, avgQData;
	UCHAR  peakI[2];
	UCHAR   VGAGainIdx[2] = {4,4};
	BOOLEAN SwAgc1stflg = TRUE;
	BOOLEAN gBandFlg = FALSE;
	uint32_t  tmp_Value;

	// band check
	if (channel <= 14) gBandFlg = TRUE;

	/* DPD and TSSI HW off */
	value32 = mt7612u_read32(pAd,TXBE_R8);
	value32 &= ~0x08000;
	mt7612u_write32(pAd,TXBE_R8, value32); // DPD off

	value32 = mt7612u_read32(pAd,CORE_R34);
	value32 &= ~0x60;
	value32 |= 0x40;
	mt7612u_write32(pAd,CORE_R34, value32); // TSSI off

	/* Do Calibration */
	/* Divider closeloop settng */
	// RXA IQ CalSetting
	if (gBandFlg)
	{
		mt_rf_write(pAd, RF_Path0, RFDIGI_TRX17,   0x00010170); // set txg gain table = to manual mode
		mt_rf_write(pAd, RF_Path1, RFDIGI_TRX17,   0x00010170); // set txg gain table = to manual mode
		mt_rf_write(pAd, RF_Path0, RG_WF0_RXG_TOP, 0x00492016); // TRSWITCH
		mt_rf_write(pAd, RF_Path1, RG_WF0_RXG_TOP, 0x00492016); // TRSWITCH
		mt_rf_write(pAd, RF_Path0, RFDIGI_TOP1,    0x0004012C); // tx block mode. 0x0 should behind 0x10
		mt_rf_write(pAd, RF_Path1, RFDIGI_TOP1,    0x0004012C); // tx block mode. 0x0 should behind 0x10
	}
	else
	{
		mt_rf_write(pAd, RF_Path0, RFDIGI_TRX17, 0x000101D0); // set txg gain table = to manual mode
		mt_rf_write(pAd, RF_Path1, RFDIGI_TRX17, 0x000101D0); // set txg gain table = to manual mode
	}

	// DCOC for RXA IQ Cal
	mt7612u_write32(pAd,CORE_R1,   0x00000000); // BW=20MHz ADC=40MHz
	mt7612u_write32(pAd,CORE_R33,  0x00021E00);

	// Send single tone
	mt7612u_write32(pAd,DACCLK_EN_DLY_CFG, 0x80008000); // DAC Clock on
	mt7612u_write32(pAd,TXBE_R6,           0x00000000); // Test format contol : Tx single tone setting

	/* Divider phase calibration process */
	for (i = 0; i < 2; i++) // ANT0, ANT1
	{
		//AGCtimeOutCount = (SwAgc1stflg == TRUE) ? 0 : 19;
		AGCtimeOutCount = 0;
		while (AGCtimeOutCount < 20) // SW AGC update to make sure I peak value can prevent peak value from satuation or too low
		{
			mt7612u_write32(pAd,CORE_R4, 0x00000001);	//core soft reset enable
			mt7612u_write32(pAd,CORE_R4, 0x00000000);	//core soft reset disable
			mt7612u_write32(pAd,TXBE_R1, 0x00001010); 	//no attenuation, full DAC swing

			switch (i)
			{
				case 0:
					// Set LNA to M
					mt_rf_write(pAd, RF_Path0, RFDIGI_TRX4, ((1<<19)|(2<<16)|(1<<15)|((0 + VGAGainIdx[0])<<8)|(1<<7)|(0 + VGAGainIdx[0])));

					// Internal loopback
					mt7612u_write32(pAd, TXBE_R4, 0x00000008); // a default setting, 2T

					mt_rf_write(pAd, RF_Path0, RFDIGI_TOP4,    0x30D71047); 	// tx block mode

					if (gBandFlg)
						mt_rf_write(pAd, RF_Path0, RFDIGI_TOP0,    0x80056F53); // manul mode for external loopback(chip mode=5)
					else
						mt_rf_write(pAd, RF_Path0, RFDIGI_TOP0,    0x80056757); // manul mode for external loopback(chip mode=5)

					mt_rf_write(pAd, RF_Path0, RFDIGI_ABB_TO_AFE5,0x00C211F1); 	// set ABB config switch

					mt7612u_write32(pAd,       RF_BSI_CKDIV,   0x00000008);		// Adjust SPI clock
					mt_rf_write(pAd, RF_Path0, RFDIGI_TRX0,    0x0500010F);	    // start rxiq dcoc
					mt7612u_write32(pAd,       RF_BSI_CKDIV,   0x00000002);		// Adjust SPI clock

					DBGPRINT(RT_DEBUG_TRACE,("Loop0\n"));
					break;
				case 1:
					// Set LNA to M
					mt_rf_write(pAd, RF_Path1, RFDIGI_TRX4, ((1<<19)|(2<<16)|(1<<15)|((0 + VGAGainIdx[1])<<8)|(1<<7)|(0 + VGAGainIdx[1])));

					mt7612u_write32(pAd, TXBE_R4, 0x00000008); // a default setting, 2T

					mt_rf_write(pAd, RF_Path1, RFDIGI_TOP4,    0x30D71047); 	// tx block mode

					if (gBandFlg)
						mt_rf_write(pAd, RF_Path1, RFDIGI_TOP0,    0x80056F53); // manul mode for external loopback(chip mode=5)
					else
						mt_rf_write(pAd, RF_Path1, RFDIGI_TOP0,    0x80056757); // manul mode for external loopback(chip mode=5)

					mt_rf_write(pAd, RF_Path1, RFDIGI_ABB_TO_AFE5,0x00C211F1); 	// set ABB config switch

					mt7612u_write32(pAd,       RF_BSI_CKDIV,   0x00000008); 	// Adjust SPI clock
					mt_rf_write(pAd, RF_Path1, RFDIGI_TRX0,    0x0500010F);		// start rxiq dcoc
					mt7612u_write32(pAd,       RF_BSI_CKDIV,   0x00000002); 	// Adjust SPI clock
					DBGPRINT(RT_DEBUG_TRACE,("Loop1\n"));
					break;
				default:
					break;
			}


			// Set Tx/Rx index
			mt7612u_write32(pAd,CAL_R2,  divPhCalPath[i]); 	// Tx0
			mt7612u_write32(pAd,TXBE_R6, 0xC0002101); 		//Test format contol : Tx single tone setting
			mt7612u_write32(pAd,CAL_R5,  0x0000140F); 		//set accumulation length
			//mt7612u_write32(pAd,CAL_R5,  0x000040C);

			//RtmpOsMsDelay(1); // waiting 1ms

			// Enable Divider phase calibration
			mt7612u_write32(pAd,CAL_R1, 0x00000086);
			phaseCaliStatus = mt7612u_read32(pAd,CAL_R1);
			timeOutCount = 0;
			while (phaseCaliStatus & 0x80)
			{
				if (timeOutCount == 10)
				{
					DBGPRINT(RT_DEBUG_TRACE,("phaseCaliStatus = %x\n", phaseCaliStatus));
					DBGPRINT(RT_DEBUG_TRACE,("LNA HW calibration can't finish process\n"));
					break;
				}

				timeOutCount++;

				RtmpOsMsDelay(1); // waiting 1ms

				phaseCaliStatus = mt7612u_read32(pAd,CAL_R1);
			}

			// 0x2C2C
			// Bit 23:16	Correlator Phase
			// Bit 15:8 	 Correlator Q value
			// Bit 7:0		  Correlator I value
			phaseCaliResult = mt7612u_read32(pAd,CAL_R11);
			mPhase0[i] = (INT)((INT)((phaseCaliResult << 8) & 0xFF000000) >> 24);
			avgQData = (INT)((INT)((phaseCaliResult << 16) & 0xFF000000) >> 24);
			avgIData = (INT)((INT)((phaseCaliResult << 24) & 0xFF000000) >> 24);
			mCalPhase0[i] = iAtan2(avgQData, avgIData);

			peakI[i] = (UCHAR)(phaseCaliResult >> 24);

			DBGPRINT(RT_DEBUG_TRACE,(
				    "CAL_R11=0x%x\n"
					"Peak I value=0x%x\n"
					"I value=0x%lx, Q value=0x%lx\n",
					phaseCaliResult,
					peakI[i],
					avgIData, avgQData));

			mt7612u_write32(pAd,CAL_R1, 0x00000006); // Disable Calibration
			// SW AGC calculation
			//if (SwAgc1stflg == TRUE && VGAGainIdx[i] < 128)
			if (VGAGainIdx[i] < 128)
			{
				// Calculate AGC ad check if AGC update is enough or not
				//if (peakI[i] >= 120 && VGAGainIdx[i] > 0)
				if (peakI[i] >= 0x50 && VGAGainIdx[i] > 0)
					VGAGainIdx[i]--;
				//else if (peakI[i] >= 60)
				else if (peakI[i] >= 0x28)
				{
					AGCtimeOutCount = 20;
					//SwAgc1stflg = FALSE;
					break;
				}
				//else if (peakI[i] <= 16)
				//	VGAGainIdx[i] += 5;
				//else if (peakI[i] <= 20)
				//	VGAGainIdx[i] += 4;
				//else if (peakI[i] <= 25)
				//	VGAGainIdx[i] += 3;
				//else if (peakI[i] <= 28)
				//	VGAGainIdx[i] += 2;
				else
					VGAGainIdx[i]++;
			}

			DBGPRINT(RT_DEBUG_TRACE,("SW AGC = %d\n", VGAGainIdx[i]));
			AGCtimeOutCount++;
		}

		// RF0 Chip mode release
		mt_rf_write(pAd, RF_Path0, RFDIGI_TOP0,    0x00056757);    // Turn off the RFDIGI logic clock
		mt_rf_write(pAd, RF_Path0, RFDIGI_TOP0,    0x00056754);    // Release manual control of top control
		RtmpOsMsDelay(1); // waiting 1ms
		mt_rf_write(pAd, RF_Path0, RFDIGI_TOP0,    0x80056754);    // Enable RFDIGI logic clock

		// RF1 Chip mode release
		mt_rf_write(pAd, RF_Path1, RFDIGI_TOP0,    0x00056757);    // Turn off the RFDIGI logic clock
		mt_rf_write(pAd, RF_Path1, RFDIGI_TOP0,    0x00056754);    // Release manual control of top control
		RtmpOsMsDelay(1); // waiting 1ms
		mt_rf_write(pAd, RF_Path1, RFDIGI_TOP0,    0x80056754);    // Release manual control of top control
	}

	DBGPRINT(RT_DEBUG_TRACE, (
			"HW Phase vs Driver Phase (deg)\n"
			"Peak I:0x%x	  ANT0/0 :%d		%d\n"
			"Peak I:0x%x	  ANT1/1 :%d		%d\n"
			"divPhase[0] : %d\n",
			peakI[0], (360*mPhase0[0])>> 8, DEG180(mCalPhase0[0]),
			peakI[1], (360*mPhase0[1])>> 8, DEG180(mCalPhase0[1]),
			(360*(mPhase0[0]-mPhase0[1]))>> 8));

}
#endif // MT76x2


#ifdef DBG
#ifdef LINUX
/* #define TIMESTAMP_CAL_CAPTURE0 */
/* #define TIMESTAMP_CAL_CAPTURE1 */
#endif /* LINUX */
#endif /* DBG */



/*
	ITxBFSaveData - save MAC data
		Returns pointer to allocated buffer containing saved data
*/
static uint32_t *ITxBFSaveData(struct rtmp_adapter *pAd)
{
	uint32_t *saveData, *sdPtr, macAddr, maxAddr;

	/* Save 48KB MAC data. */
	saveData = kmalloc(0xC000, GFP_ATOMIC);
	if (saveData == NULL) {
		DBGPRINT(RT_DEBUG_ERROR, ("%s():Alloc memory failed\n", __FUNCTION__));
		return NULL;
	}

	maxAddr = 0x10000;

	for (sdPtr=saveData, macAddr=0x4000; macAddr<maxAddr; macAddr += 4, sdPtr++) {
		sdPtr = mt7612u_read32(pAd, macAddr);
	}
	return saveData;
}


/*
	ITxBFSaveData - restore MAC data
		saveData - buffer containing data to restore
*/
static void ITxBFRestoreData(struct rtmp_adapter *pAd, uint32_t *saveData)
{
	uint32_t *sdPtr, macAddr, maxAddr;

	maxAddr = 0x10000;

	for (sdPtr=saveData, macAddr=0x4000; macAddr<maxAddr; macAddr += 4, sdPtr++)
	{
		mt7612u_write32(pAd, macAddr, *sdPtr);
	}
}


/*
	mapChannelKHz - map channel number to KHz
*/
static LONG mapChannelKHz(int ch)
{
	long x;
	MAP_CHANNEL_ID_TO_KHZ(ch, x);
	return x;
}


/*
	InterpParam - Interpolate calibration parameters
		ch - channel to interpolate for
		chBeg, chEnd - begining and ending channel
		yBeg, yEnd - the hex phase values corresponding to chBeg and chEnd
*/
#ifdef MT76x2
static UCHAR InterpParam_check(int ch, int chBeg, int chEnd, UCHAR yBeg, UCHAR yEnd)
{
	long x, xBeg, xEnd, yDelta;
	UCHAR output;

	x = mapChannelKHz(ch);
	xBeg = mapChannelKHz(chBeg);
	xEnd = mapChannelKHz(chEnd);
	yDelta = yEnd - yBeg;

	/*
		Handle the phase wraparound. We always assume the delta phase is in
		the range [-180, 180] degrees = [0x80, 0x7f] in hex
	*/
	if (yDelta >= 128)
		yDelta -= 256;
	else if (yDelta <= -128)
		yDelta += 256;

	output = yBeg + yDelta*(x-xBeg)/(xEnd-xBeg);

	DBGPRINT(RT_DEBUG_TRACE,
		  ("%s : \n"
		   "x    = mapChannelKHz(%d) = %ld\n"
		   "xBeg = mapChannelKHz(%d) = %ld\n"
		   "xEnd = mapChannelKHz(%d) = %ld\n"
		   "yDelta = %ld\n"
		   "output = %d\n",
		   __FUNCTION__,
		   ch, x,
		   chBeg, xBeg,
		   chEnd, xEnd,
		   yDelta,
		   output));

	return output;
}
#endif


/*
	InterpParam - Interpolate calibration parameters
		ch - channel to interpolate for
		chBeg, chEnd - begining and ending channel
		yBeg, yEnd - the hex phase values corresponding to chBeg and chEnd
*/
static UCHAR InterpParam(int ch, int chBeg, int chEnd, UCHAR yBeg, UCHAR yEnd)
{
	long x, xBeg, xEnd, yDelta;

	x = mapChannelKHz(ch);
	xBeg = mapChannelKHz(chBeg);
	xEnd = mapChannelKHz(chEnd);
	yDelta = yEnd - yBeg;

	/*
		Handle the phase wraparound. We always assume the delta phase is in
		the range [-180, 180] degrees = [0x80, 0x7f] in hex
	*/
	if (yDelta >= 128)
		yDelta -= 256;
	else if (yDelta <= -128)
		yDelta += 256;

	return yBeg + yDelta*(x-xBeg)/(xEnd-xBeg);
}


/*
	ITxBFDivParams - interpolate Divider calibration parameter based on channel and EEPROM
		divValues - returns the Divider Calibration values for this channel
		channel - the channel to interpolate for
		divParams - the Divider Calibration parameters from EEPROM
*/

#ifdef MT76x2
static void mt76x2_ITxBFDivParams(UCHAR divValues[2], int channel, ITXBF_DIV_PARAMS *divParams)
{
	if (channel <= 14)
		divValues[0] = InterpParam(channel, 1, 14, divParams->E1gBeg, divParams->E1gEnd);
	else if (channel <= 60)
		divValues[0] = InterpParam(channel, 36, 64, divParams->E1aLowBeg, divParams->E1aLowEnd);
	//else if (channel <= 60)
	//	divValues[0] = InterpParam(channel, 44, 60, divParams->aLowBeg, divParams->aLowEnd);
	else if (channel <= 120)
		divValues[0] = InterpParam(channel, 100, 120, divParams->E1aMidBeg, divParams->E1aMidMid);
	else if (channel <= 140)
		divValues[0] = InterpParam(channel, 120, 140, divParams->E1aMidMid, divParams->E1aMidEnd);
	else
		divValues[0] = InterpParam(channel, 149, 173, divParams->E1aHighBeg, divParams->E1aHighEnd);
}
#endif

/*
	ITxBFLnaParams - interpolate LNA compensation parameter based on channel and EEPROM.
		lnaValues - returns the quantized LNA compensation values for M-L, H-L and H-M
		channel - the channel to interpolate for
		lnaParams - the LNA Calibration parameters from EEPROM
*/

#ifdef MT76x2
static void mt76x2_ITxBFLnaParams(UCHAR lnaValues[3], int channel, ITXBF_LNA_PARAMS *lnaParams)
{
	int i;

	if (channel <= 14) {
		lnaValues[0] = InterpParam(channel, 1, 14, lnaParams->E1gBeg[0], lnaParams->E1gEnd[0]);
		lnaValues[1] = InterpParam(channel, 1, 14, lnaParams->E1gBeg[1], lnaParams->E1gEnd[1]);
		lnaValues[2] = InterpParam(channel, 1, 14, lnaParams->E1gBeg[2], lnaParams->E1gEnd[2]);
	}
	else if (channel <= 64) {
		lnaValues[0] = InterpParam_check(channel, 36, 64, lnaParams->E1aLowBeg[0], lnaParams->E1aLowEnd[0]);
		lnaValues[1] = InterpParam_check(channel, 36, 64, lnaParams->E1aLowBeg[1], lnaParams->E1aLowEnd[1]);
		lnaValues[2] = InterpParam_check(channel, 36, 64, lnaParams->E1aLowBeg[2], lnaParams->E1aLowEnd[2]);
	}
	else if (channel <= 120) {
		lnaValues[0] = InterpParam(channel, 100, 120, lnaParams->E1aMidBeg[0], lnaParams->E1aMidMid[0]);
		lnaValues[1] = InterpParam(channel, 100, 120, lnaParams->E1aMidBeg[1], lnaParams->E1aMidMid[1]);
		lnaValues[2] = InterpParam(channel, 100, 120, lnaParams->E1aMidBeg[2], lnaParams->E1aMidMid[2]);
	}
		else if (channel <= 140) {
		lnaValues[0] = InterpParam(channel, 120, 140, lnaParams->E1aMidMid[0], lnaParams->E1aMidEnd[0]);
		lnaValues[1] = InterpParam(channel, 120, 140, lnaParams->E1aMidMid[1], lnaParams->E1aMidEnd[1]);
		lnaValues[2] = InterpParam(channel, 120, 140, lnaParams->E1aMidMid[2], lnaParams->E1aMidEnd[2]);
	}
	else {
		lnaValues[0] = InterpParam(channel, 149, 173, lnaParams->E1aHighBeg[0], lnaParams->E1aHighEnd[0]);
		lnaValues[1] = InterpParam(channel, 149, 173, lnaParams->E1aHighBeg[1], lnaParams->E1aHighEnd[1]);
		lnaValues[2] = InterpParam(channel, 149, 173, lnaParams->E1aHighBeg[2], lnaParams->E1aHighEnd[2]);
	}
}
#endif


/*
	ITxBFPhaseParams - interpolate Phase compensation parameters based on channel and EEPROM
		phaseValues - returns the Phase compensation values for this channel
		channel - the channel to interpolate for
		phaseParams - the Phase Calibration parameters from EEPROM
*/

#ifdef MT76x2
static void mt76x2_ITxBFPhaseParams(UCHAR phaseValues[2], int channel, ITXBF_PHASE_PARAMS *phaseParams)
{
	if (channel <= 14) {
		phaseValues[0] = InterpParam(channel, 1, 14, phaseParams->E1gBeg, phaseParams->E1gEnd);
	}
	else if (channel <= 64) {
		phaseValues[0] = InterpParam(channel, 36, 64, phaseParams->E1aLowBeg, phaseParams->E1aLowEnd);
	}
	else if (channel <= 120) {
		phaseValues[0] = InterpParam(channel, 100, 120, phaseParams->E1aMidBeg, phaseParams->E1aMidMid);
	}
	else if (channel <= 140) {
		phaseValues[0] = InterpParam(channel, 120, 140, phaseParams->E1aMidMid, phaseParams->E1aMidEnd);
	}
	else {
		phaseValues[0] = InterpParam(channel, 149, 173, phaseParams->E1aHighBeg, phaseParams->E1aHighEnd);
	}
}
#endif


#define ITXBF_EEPROM_WORDS		19	/* 38 bytes of ITxBF parameters */



/*
	ITxBFGetEEPROM - Read ITxBF calibration parameters from EEPROM
		phaseParams - pointer to BBP Phase calibration parameters. If NULL then parameters are not returned
		lnaParams - pointer to BBP LNA calibration parameters. If NULL then parameters are not returned
		divParams - divider calibration parameters. If NULL then parameters are not returned
*/
void ITxBFGetEEPROM(
	IN struct rtmp_adapter *pAd,
	IN ITXBF_PHASE_PARAMS *phaseParams,
	IN ITXBF_LNA_PARAMS *lnaParams,
	IN ITXBF_DIV_PARAMS *divParams)
{
	USHORT	EE_Value[14], andValue;
	int		i;

	/* Get Phase parameters */
	if (phaseParams != NULL) {
		/* Read and check for initialized values */
		andValue = 0xFFFF;


#ifdef MT76x2
		if (IS_MT76x2(pAd))
		{
			RT28xx_EEPROM_READ16(pAd, EEPROM1_ITXBF_CAL, EE_Value[0]);

			phaseParams->E1gBeg = (EE_Value[0] & 0x00FF);
			phaseParams->E1gEnd = (EE_Value[0] & 0xFF00)>>8;

			for (i=0; i<4; i++) {
				RT28xx_EEPROM_READ16(pAd, EEPROM1_ITXBF_CAL + 2*i + 4, EE_Value[i]);
				andValue &= EE_Value[i];
			}

			if (andValue == 0xFFFF) {
				memset(phaseParams, 0, sizeof(*phaseParams));
			} else {
				phaseParams->E1aHighBeg = (EE_Value[0] & 0x00FF);
				phaseParams->E1aHighEnd = (EE_Value[0] & 0xFF00)>>8;
				phaseParams->E1aLowBeg  = (EE_Value[1] & 0x00FF);
				phaseParams->E1aLowEnd  = (EE_Value[1] & 0xFF00)>>8;
				phaseParams->E1aMidBeg  = (EE_Value[2] & 0x00FF);
				phaseParams->E1aMidMid  = (EE_Value[2] & 0xFF00)>>8;
				phaseParams->E1aMidEnd  = (EE_Value[3] & 0x00FF);
			}
		}
#endif
	}

	/* Get Divider Phase parameters */
	if (divParams != NULL) {
		/* Read and check for initialized values */
		andValue = 0xFFFF;

#ifdef MT76x2
		if (IS_MT76x2(pAd))
		{
			RT28xx_EEPROM_READ16(pAd, EEPROM1_ITXBF_CAL + 2, EE_Value[0]);
			divParams->E1gBeg = (EE_Value[0] & 0x00FF);
			divParams->E1gEnd = (EE_Value[0] & 0xFF00)>>8;

			for (i=0; i<4; i++) {
				RT28xx_EEPROM_READ16(pAd, EEPROM1_ITXBF_CAL + 12 + 2*i, EE_Value[i]);
				andValue &= EE_Value[i];
			}

			if (andValue == 0xFFFF) {
				memset(divParams, 0, sizeof(*divParams));
			}
			else {
				divParams->E1aHighBeg = (EE_Value[0] & 0x00FF);
				divParams->E1aHighEnd = (EE_Value[0] & 0xFF00)>>8;
				divParams->E1aLowBeg  = (EE_Value[1] & 0x00FF);
				divParams->E1aLowEnd  = (EE_Value[1] & 0xFF00)>>8;
				divParams->E1aMidBeg  = (EE_Value[2] & 0x00FF);
				divParams->E1aMidMid  = (EE_Value[2] & 0xFF00)>>8;
				divParams->E1aMidEnd  = (EE_Value[3] & 0x00FF);
			}
		}/* IS_MT76x2(pAd) */
#endif
	}

	/* Get LNA Parameters */
	if (lnaParams != NULL) {
		/* Read and check for initialized values */
		andValue = 0xFFFF;


#ifdef MT76x2
		if (IS_MT76x2(pAd))
		{
			for (i=0; i<14; i++) {
				RT28xx_EEPROM_READ16(pAd, EEPROM1_ITXBF_CAL + 20 + 2*i, EE_Value[i]);
				andValue &= EE_Value[i];
			}

			if (andValue == 0xFFFF) {
				memset(lnaParams, 0, sizeof(*lnaParams));
			}
			else {
				lnaParams->E1gBeg[0] = (EE_Value[0] & 0x00FF);
				lnaParams->E1gBeg[1] = (EE_Value[0] & 0xFF00)>>8;
				lnaParams->E1gBeg[2] = (EE_Value[1] & 0x00FF);
				lnaParams->E1gEnd[0] = (EE_Value[1] & 0xFF00)>>8;
				lnaParams->E1gEnd[1] = (EE_Value[2] & 0x00FF);
				lnaParams->E1gEnd[2] = (EE_Value[2] & 0xFF00)>>8;

				lnaParams->E1aHighBeg[0] = (EE_Value[3] & 0x00FF);
				lnaParams->E1aHighBeg[1] = (EE_Value[3] & 0xFF00)>>8;
				lnaParams->E1aHighBeg[2] = (EE_Value[4] & 0x00FF);
				lnaParams->E1aHighEnd[0] = (EE_Value[4] & 0xFF00)>>8;
				lnaParams->E1aHighEnd[1] = (EE_Value[5] & 0x00FF);
				lnaParams->E1aHighEnd[2] = (EE_Value[5] & 0xFF00)>>8;

				lnaParams->E1aLowBeg[0] = (EE_Value[6] & 0x00FF);
				lnaParams->E1aLowBeg[1] = (EE_Value[6] & 0xFF00)>>8;
				lnaParams->E1aLowBeg[2] = (EE_Value[7] & 0x00FF);
				lnaParams->E1aLowEnd[0] = (EE_Value[7] & 0xFF00)>>8;
				lnaParams->E1aLowEnd[1] = (EE_Value[8] & 0x00FF);
				lnaParams->E1aLowEnd[2] = (EE_Value[8] & 0xFF00)>>8;

				lnaParams->E1aMidBeg[0] = (EE_Value[9] & 0x00FF);
				lnaParams->E1aMidBeg[1] = (EE_Value[9] & 0xFF00)>>8;
				lnaParams->E1aMidBeg[2] = (EE_Value[10] & 0x00FF);
				lnaParams->E1aMidMid[0] = (EE_Value[10] & 0xFF00)>>8;
				lnaParams->E1aMidMid[1] = (EE_Value[11] & 0x00FF);
				lnaParams->E1aMidMid[2] = (EE_Value[11] & 0xFF00)>>8;
				lnaParams->E1aMidEnd[0] = (EE_Value[12] & 0x00FF);
				lnaParams->E1aMidEnd[1] = (EE_Value[12] & 0xFF00)>>8;
				lnaParams->E1aMidEnd[2] = (EE_Value[13] & 0x00FF);
			}/* andValue == 0xFFFF */
		}/* IS_MT76x2(pAd) */
#endif
	}/* lnaParams != NULL */
}


/*
	ITxBFSetEEPROM - Save ITxBF calibration parameters in EEPROM
		phaseParams - pointer to BBP calibration parameters. If NULL then parameters are not written
		lnaParams - pointer to BBP LNA calibration parameters. If NULL then parameters are not written
		divParams - divider calibration parameters. If NULL then parameters are not written
*/
void ITxBFSetEEPROM(
	IN struct rtmp_adapter *pAd,
	IN ITXBF_PHASE_PARAMS *phaseParams,
	IN ITXBF_LNA_PARAMS *lnaParams,
	IN ITXBF_DIV_PARAMS *divParams)
{
	USHORT	EE_Value[14], eeTmp;
	int		i, eeAddr;

	/* Set EEPROM parameters */

	/* Phase parameters */
	if (phaseParams != NULL) {

#ifdef MT76x2
		if (IS_MT76x2(pAd))
		{
			EE_Value[0] = phaseParams->E1gBeg | (phaseParams->E1gBeg<<8);
			RT28xx_EEPROM_READ16(pAd, EEPROM1_ITXBF_CAL, eeTmp);

			if (eeTmp != EE_Value[0])
			{
				RT28xx_EEPROM_WRITE16(pAd, EEPROM1_ITXBF_CAL, EE_Value[0]);
			}

			EE_Value[0] = phaseParams->E1aHighBeg | (phaseParams->E1aHighEnd<<8);
			EE_Value[1] = phaseParams->E1aLowBeg  | (phaseParams->E1aLowEnd <<8);
			EE_Value[2] = phaseParams->E1aMidBeg  | (phaseParams->E1aMidMid <<8);
			EE_Value[3] = phaseParams->E1aMidEnd  | 0xFF00;

			for (i=0; i<4; i++)
			{
				eeAddr = EEPROM1_ITXBF_CAL + 2*i + 4;

				RT28xx_EEPROM_READ16(pAd, eeAddr, eeTmp);

				if (eeTmp != EE_Value[i])
				{
					RT28xx_EEPROM_WRITE16(pAd, eeAddr, EE_Value[i]);
				}

				DBGPRINT(RT_DEBUG_INFO,
				   ("ITxBFGetEEPROM check ::: \n"
				   	"EEPROM origina data =0x%x\n"
				   	"Input data = 0x%x\n"
				   	"Saved low byte data in EEPROM=0x%x\n"
				   	"Saved high byte data in EEPROM=0x%x\n",
				   	eeTmp,
				   	EE_Value[i],
				   	pAd->EEPROMImage[eeAddr],
				   	pAd->EEPROMImage[eeAddr + 1]));
			}
		}/* IS_MT76x2(pAd) */
#endif
	}/* phaseParams != NULL */

	/* Divider Phase parameters */
	if (divParams != NULL) {

#ifdef MT76x2
		if (IS_MT76x2(pAd))
		{
			EE_Value[0] = divParams->E1gBeg | (divParams->E1gBeg<<8);
			RT28xx_EEPROM_READ16(pAd, EEPROM1_ITXBF_CAL + 2, eeTmp);

			if (eeTmp != EE_Value[0])
			{
				RT28xx_EEPROM_WRITE16(pAd, EEPROM1_ITXBF_CAL + 2, EE_Value[0]);
			}

			EE_Value[0] = divParams->E1aHighBeg | (divParams->E1aHighEnd<<8);
			EE_Value[1] = divParams->E1aLowBeg  | (divParams->E1aLowEnd <<8);
			EE_Value[2] = divParams->E1aMidBeg  | (divParams->E1aMidMid <<8);
			EE_Value[3] = divParams->E1aMidEnd  | 0xFF00;

			for (i=0; i<4; i++)
			{
				eeAddr = EEPROM1_ITXBF_CAL + 12 + 2*i;
				RT28xx_EEPROM_READ16(pAd, eeAddr, eeTmp);

				if (eeTmp != EE_Value[i])
				{
					RT28xx_EEPROM_WRITE16(pAd, eeAddr, EE_Value[i]);
				}
			}
		}/* IS_MT76x2(pAd) */
#endif
	}/* divParams != NULL*/

	/* LNA Phase parameters */
	if (lnaParams != NULL) {

#ifdef MT76x2
		if (IS_MT76x2(pAd))
		{
			EE_Value[0]  = lnaParams->E1gBeg[0]     | (lnaParams->E1gBeg[1]<<8);
			EE_Value[1]  = lnaParams->E1gBeg[2]     | (lnaParams->E1gEnd[0]<<8);
			EE_Value[2]  = lnaParams->E1gEnd[1]     | (lnaParams->E1gEnd[2]<<8);

			EE_Value[3]  = lnaParams->E1aHighBeg[0] | (lnaParams->E1aHighBeg[1]<<8);
			EE_Value[4]  = lnaParams->E1aHighBeg[2] | (lnaParams->E1aHighEnd[0]<<8);
			EE_Value[5]  = lnaParams->E1aHighEnd[1] | (lnaParams->E1aHighEnd[2]<<8);
			EE_Value[6]  = lnaParams->E1aLowBeg[0]  | (lnaParams->E1aLowBeg[1]<<8);
			EE_Value[7]  = lnaParams->E1aLowBeg[2]  | (lnaParams->E1aLowEnd[0]<<8);
			EE_Value[8]  = lnaParams->E1aLowEnd[1]  | (lnaParams->E1aLowEnd[2]<<8);
			EE_Value[9]  = lnaParams->E1aMidBeg[0]  | (lnaParams->E1aMidBeg[1]<<8);
			EE_Value[10] = lnaParams->E1aMidBeg[2]  | (lnaParams->E1aMidMid[0]<<8);
			EE_Value[11] = lnaParams->E1aMidMid[1]  | (lnaParams->E1aMidMid[2]<<8);
			EE_Value[12] = lnaParams->E1aMidEnd[0]  | (lnaParams->E1aMidEnd[1]<<8);
			EE_Value[13] = lnaParams->E1aMidEnd[2]  | 0xAA00;

			for (i=0; i<14; i++)
			{
				eeAddr = EEPROM1_ITXBF_CAL + 20 + 2*i;

				RT28xx_EEPROM_READ16(pAd, eeAddr, eeTmp);

				if (eeTmp != EE_Value[i])
				{
					RT28xx_EEPROM_WRITE16(pAd, eeAddr, EE_Value[i]);
				}

				DBGPRINT(RT_DEBUG_INFO,
				   ("ITxBFGetEEPROM check ::: \n"
				   	"EEPROM origina data =0x%x\n"
				   	"Input data = 0x%x\n"
				   	"Saved low byte data in EEPROM=0x%x\n"
				   	"Saved high byte data in EEPROM=0x%x\n",
				   	eeTmp,
				   	pAd->EEPROMImage[eeAddr],
				   	pAd->EEPROMImage[eeAddr + 1],
				   	EE_Value[i]));
			}
		}/* IS_MT76x2(pAd) */
#endif
	}/* lnaParams != NULL */
}


/*
	ITxBFLoadLNAComp - load the LNA compensation registers
*/

/*
	ITxBFDividerCalibration - perform divider calibration
		calFunction - the function to perform
			0=>Display cal param,
			1=>Update EEPROM
			2=>Update BBP
			3=>Just return the quantized divider phase in divPhase
			10=> Display params and dump capture data
		calMethod - the calibration method to use. 0=>use default method for the band
		divPhase - if not NULL, returns the quantized divider phase (0, +/-90, 180 for 2.4G, 0,180 for 5G)
	returns TRUE if no errors
*/
#define ITXBF_MAX_WAIT_CYCLE	10000


#ifdef MT76x2
INT mt76x2_ITxBFDividerCalibration(
	IN struct rtmp_adapter *pAd,
	IN int calFunction,
	IN int calMethod,
	OUT UCHAR *divPhase)
{
	int i, j;
	ITXBF_DIV_PARAMS divParams;
	UCHAR	calRefValue[2];
	UCHAR channel = pAd->CommonCfg.Channel;
	UCHAR newRefValue[2];
	LONG refValue[2];
	UCHAR phase[2];
	int avgI[3], avgQ[3];
	int peak[3][3];
	LONG d01, d21;
	int result = TRUE;
	uint32_t value32;

	UCHAR divPhaseValue[2];
	PCAP_IQ_DATA capIqData = NULL;
	ITXBF_PHASE_PARAMS phaseParams;
	UCHAR phaseValues[2];

	uint32_t *saveData = NULL;
	uint32_t saveSysCtrl, savePbfCfg, saveMacSysCtrl, saveDmaCtrl;
	int allocSize = (calFunction==11? MAX_CAPTURE_LENGTH: DIVCAL_CAPTURE_LENGTH)*sizeof(COMPLEX_VALUE)*3;
	BOOLEAN displayParams = (calFunction==0 || calFunction==10);

	uint32_t phaseCaliStatus, phaseCaliResult;
	UCHAR  timeOutCount;
	LONG   mCalPhase0[4], mCalPhase1[4];
	LONG   avgIData, avgQData;
	UCHAR  peakI[3];


#ifdef TIMESTAMP_CAL_CAPTURE1
	struct timeval tval0, tval1;
#endif
#ifdef TIMESTAMP_CAL_CAPTURE1
	do_gettimeofday(&tval0);
#endif

	/* Handle optional divPhase parameter */
	if (divPhase == NULL)
		divPhase = divPhaseValue;

#ifdef TIMESTAMP_CAL_CAPTURE1
	do_gettimeofday(&tval1);
#endif

	/* Do Calibration */
	// Optional A-Band or G-Band. RF Cal with each TX active */
	CalcDividerPhase(pAd, phase);

	/* Calculate difference */
	divPhase[0] = phase[0] - phase[1];

	/* Compute the quantized delta phase */
	/* Quantize to 180 deg (0x80) with rounding */
	DBGPRINT(RT_DEBUG_WARN, (
				"HW Phase vs Driver Phase (deg)\n"
				"ANT0/0 :%d\n"
				"ANT1/1 :%d\n"
				"divPhase[0] : %d\n",
				(360*phase[0])>> 8,
				(360*phase[1])>> 8,
				(360*divPhase[0])>> 8));

	divPhase[0] = (divPhase[0] + 0x40) & 0x80;

	DBGPRINT(RT_DEBUG_WARN, (
				"After divderPase[0] : %d\n",(360*divPhase[0])>> 8));

	/* Either display parameters, update EEPROM, update BBP registers or dump capture data */
	switch (calFunction) {
	case 0:
		break;

	case 1:
		/*
			Save new reference values in EEPROM. The new reference is just the delta phase
			values with the old ref value added back in
		*/

		ITxBFGetEEPROM(pAd, 0, 0, &divParams);

		/* Only allow calibration on specific channels */
		if (channel == 1)
			divParams.E1gBeg = divPhase[0];
		else if (channel == 14)
			divParams.E1gEnd = divPhase[0];
		else if (channel == 36)
			divParams.E1aLowBeg = divPhase[0];
		else if (channel == 64)
			divParams.E1aLowEnd = divPhase[0];
		else if (channel == 100)
			divParams.E1aMidBeg = divPhase[0];
		else if (channel == 120)
			divParams.E1aMidMid = divPhase[0];
		else if (channel == 140)
			divParams.E1aMidEnd = divPhase[0];
		else if (channel == 149)
			divParams.E1aHighBeg = divPhase[0];
		else if (channel == 173)
			divParams.E1aHighEnd = divPhase[0];
		else {
			DBGPRINT(RT_DEBUG_ERROR, ("Invalid channel: %d\nMust calibrate channel 1, 14, 36, 64, 100, 120, 140, 149 or 173", channel) );
			result = FALSE;
			goto exitDivCal;
		}

		ITxBFSetEEPROM(pAd, 0, 0, &divParams);
		break;

	case 2:
		/*
			Update BBP Registers. Quantize DeltaPhase to 90 or 180 depending on band. Then
			update original phase calibration values from EEPROM and set R176 for Ant 0 and Ant2
		*/
		ITxBFGetEEPROM(pAd, &phaseParams, 0, 0);
		mt76x2_ITxBFPhaseParams(phaseValues, channel, &phaseParams);

		/* Ant0 */
		mt7612u_write32(pAd, CAL_R0, 0);
		mt7612u_write32(pAd, TXBE_R13, phaseValues[0] + divPhase[0]);  // for method1
		//mt7612u_write32(pAd, TXBE_R13, 0xC4);

		DBGPRINT(RT_DEBUG_TRACE, (
			   " ============================================================ \n"
		       " Divider phase  = 0x%x\n"
		       " Residual phase = 0x%x\n"
		       " Tx phase compensated value = 0x%x\n"
		       " ============================================================ \n",
		       divPhase[0], phaseValues[0], phaseValues[0] + divPhase[0]));

		/* Enable TX Phase Compensation */
		value32 = mt7612u_read32(pAd, TXBE_R12);
		mt7612u_write32(pAd, TXBE_R12, value32 | 0x08);
		break;

	case 3:
		/*
			Update BBP Registers. Quantize DeltaPhase to 90 or 180 depending on band. Then
			update original phase calibration values from EEPROM and set R176 for Ant 0 and Ant2
		*/
		ITxBFGetEEPROM(pAd, &phaseParams, 0, 0);
		mt76x2_ITxBFPhaseParams(phaseValues, channel, &phaseParams);

		/* Ant0 */
		mt7612u_write32(pAd, CAL_R0, 0);
		mt7612u_write32(pAd, TXBE_R13, phaseValues[0] + divPhase[0]);  // for method1

		DBGPRINT(RT_DEBUG_TRACE, (
				" ============================================================ \n"
		   		" Divider phase  = 0x%x\n"
		       	" Residual phase = 0x%x\n"
		       	" Tx phase compensated value = 0x%x\n"
		       	" ============================================================ \n",
		       	divPhase[0], phaseValues[0], phaseValues[0] + divPhase[0]));

		break;
	case 10:
		break;

	default:
		result = FALSE;
		break;
	}

exitDivCal:
#ifdef TIMESTAMP_CAL_CAPTURE1
	do_gettimeofday(&tval1);

	DBGPRINT(RT_DEBUG_ERROR, ("%s t1=%ld\n", __FUNCTION__,
			tval1.tv_usec - tval0.tv_usec));
#endif

	return result;
}

#endif

#define MAX_LNA_CAPS	10		/* Maximum number of LNA captures per calibration */



#ifdef MT76x2

INT    avgPhase32[3];
UCHAR  MidVGA[2];

#endif // MT76x2

#ifdef MT76x2
static SC_TABLE_ENTRY impSubCarrierTable[3] = { {224, 255, 1, 31}, {198, 254, 2, 58}, {134, 254, 2, 122} };
static SC_TABLE_ENTRY expSubCarrierTable[3] = { {224, 255, 1, 31}, {198, 254, 2, 58}, {134, 254, 2, 122} };

#endif /* MT76x2 */

#ifdef DBG
#ifdef LINUX
/* #define TIMESTAMP_CALC_CALIBRATION */
#endif /* LINUX */
#endif /* DBG */

#define P_RESOLUTION	256		/* Resolution of phase calculation: 2pi/256 */

static UCHAR maxCarrTab[3] = {PROFILE_MAX_CARRIERS_20,
		                      PROFILE_MAX_CARRIERS_40,
		                 	  PROFILE_MAX_CARRIERS_80};

int32_t ei0[PROFILE_MAX_CARRIERS_40][2];
int32_t ei1[PROFILE_MAX_CARRIERS_40][2];
int32_t ei2[PROFILE_MAX_CARRIERS_40][2];

