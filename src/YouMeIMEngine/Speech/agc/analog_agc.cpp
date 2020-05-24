/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/* analog_agc.c
 *
 * Using a feedback system, determines an appropriate analog volume level
 * given an input signal and current volume level. Targets a conservative
 * signal level and is intended for use with a digital AGC to apply
 * additional gain.
 *
 */

#include "analog_agc.h"

#include <stdlib.h>
#ifdef WEBRTC_AGC_DEBUG_DUMP
#include <stdio.h>
#endif
namespace youmecommon
{

	/* The slope of in Q13*/
	static const int16_t kSlope1[8] = { 21793, 12517, 7189, 4129,
		2372, 1362, 472, 78 };

	/* The offset in Q14 */
	static const int16_t kOffset1[8] = { 25395, 23911, 22206, 20737,
		19612, 18805, 17951, 17367 };

	/* The slope of in Q13*/
	static const int16_t kSlope2[8] = { 2063, 1731, 1452, 1218, 1021, 857, 597, 337 };

	/* The offset in Q14 */
	static const int16_t kOffset2[8] = { 18432, 18379, 18290, 18177,
		18052, 17920, 17670, 17286 };

	static const int16_t kMuteGuardTimeMs = 8000;
	static const int16_t kInitCheck = 42;
	static const size_t kNumSubframes = 10;

	/* Default settings if config is not used */
#define AGC_DEFAULT_TARGET_LEVEL 3
#define AGC_DEFAULT_COMP_GAIN 9
	/* This is the target level for the analog part in ENV scale. To convert to RMS
	 * scale you
	 * have to add OFFSET_ENV_TO_RMS.
	 */
#define ANALOG_TARGET_LEVEL 11
#define ANALOG_TARGET_LEVEL_2 5  // ANALOG_TARGET_LEVEL / 2
	/* Offset between RMS scale (analog part) and ENV scale (digital part). This
	 * value actually
	 * varies with the FIXED_ANALOG_TARGET_LEVEL, hence we should in the future
	 * replace it with
	 * a table.
	 */
#define OFFSET_ENV_TO_RMS 9
	/* The reference input level at which the digital part gives an output of
	 * targetLevelDbfs
	 * (desired level) if we have no compression gain. This level should be set high
	 * enough not
	 * to compress the peaks due to the dynamics.
	 */
#define DIGITAL_REF_AT_0_COMP_GAIN 4
	/* Speed of reference level decrease.
	 */
#define DIFF_REF_TO_ANALOG 5

#ifdef MIC_LEVEL_FEEDBACK
#define NUM_BLOCKS_IN_SAT_BEFORE_CHANGE_TARGET 7
#endif
	/* Size of analog gain table */
#define GAIN_TBL_LEN 32
	/* Matlab code:
	 * fprintf(1, '\t%i, %i, %i, %i,\n', round(10.^(linspace(0,10,32)/20) * 2^12));
	 */
	/* Q12 */
	static const uint16_t kGainTableAnalog[GAIN_TBL_LEN] = {
		4096, 4251, 4412, 4579, 4752, 4932, 5118, 5312, 5513, 5722, 5938,
		6163, 6396, 6638, 6889, 7150, 7420, 7701, 7992, 8295, 8609, 8934,
		9273, 9623, 9987, 10365, 10758, 11165, 11587, 12025, 12480, 12953 };

	/* Gain/Suppression tables for virtual Mic (in Q10) */
	static const uint16_t kGainTableVirtualMic[128] = {
		1052, 1081, 1110, 1141, 1172, 1204, 1237, 1271, 1305, 1341, 1378,
		1416, 1454, 1494, 1535, 1577, 1620, 1664, 1710, 1757, 1805, 1854,
		1905, 1957, 2010, 2065, 2122, 2180, 2239, 2301, 2364, 2428, 2495,
		2563, 2633, 2705, 2779, 2855, 2933, 3013, 3096, 3180, 3267, 3357,
		3449, 3543, 3640, 3739, 3842, 3947, 4055, 4166, 4280, 4397, 4517,
		4640, 4767, 4898, 5032, 5169, 5311, 5456, 5605, 5758, 5916, 6078,
		6244, 6415, 6590, 6770, 6956, 7146, 7341, 7542, 7748, 7960, 8178,
		8402, 8631, 8867, 9110, 9359, 9615, 9878, 10148, 10426, 10711, 11004,
		11305, 11614, 11932, 12258, 12593, 12938, 13292, 13655, 14029, 14412, 14807,
		15212, 15628, 16055, 16494, 16945, 17409, 17885, 18374, 18877, 19393, 19923,
		20468, 21028, 21603, 22194, 22801, 23425, 24065, 24724, 25400, 26095, 26808,
		27541, 28295, 29069, 29864, 30681, 31520, 32382 };
	static const uint16_t kSuppressionTableVirtualMic[128] = {
		1024, 1006, 988, 970, 952, 935, 918, 902, 886, 870, 854, 839, 824, 809, 794,
		780, 766, 752, 739, 726, 713, 700, 687, 675, 663, 651, 639, 628, 616, 605,
		594, 584, 573, 563, 553, 543, 533, 524, 514, 505, 496, 487, 478, 470, 461,
		453, 445, 437, 429, 421, 414, 406, 399, 392, 385, 378, 371, 364, 358, 351,
		345, 339, 333, 327, 321, 315, 309, 304, 298, 293, 288, 283, 278, 273, 268,
		263, 258, 254, 249, 244, 240, 236, 232, 227, 223, 219, 215, 211, 208, 204,
		200, 197, 193, 190, 186, 183, 180, 176, 173, 170, 167, 164, 161, 158, 155,
		153, 150, 147, 145, 142, 139, 137, 134, 132, 130, 127, 125, 123, 121, 118,
		116, 114, 112, 110, 108, 106, 104, 102 };

	/* Table for target energy levels. Values in Q(-7)
	 * Matlab code
	 * targetLevelTable = fprintf('%d,\t%d,\t%d,\t%d,\n',
	 * round((32767*10.^(-(0:63)'/20)).^2*16/2^7) */

	static const int32_t kTargetLevelTable[64] = {
		134209536, 106606424, 84680493, 67264106, 53429779, 42440782, 33711911,
		26778323, 21270778, 16895980, 13420954, 10660642, 8468049, 6726411,
		5342978, 4244078, 3371191, 2677832, 2127078, 1689598, 1342095,
		1066064, 846805, 672641, 534298, 424408, 337119, 267783,
		212708, 168960, 134210, 106606, 84680, 67264, 53430,
		42441, 33712, 26778, 21271, 16896, 13421, 10661,
		8468, 6726, 5343, 4244, 3371, 2678, 2127,
		1690, 1342, 1066, 847, 673, 534, 424,
		337, 268, 213, 169, 134, 107, 85,
		67 };

	int WebRtcAgc_Process(void* agcInst,
		const int16_t* in_near,
		size_t samples,
		int16_t* out) {
		LegacyAgc* stt;

		stt = (LegacyAgc*)agcInst;

		//
		if (stt == NULL) {
			return -1;
		}
		//

		if (stt->fs == 8000) {
			if (samples != 80) {
				return -1;
			}
		}
		else if (stt->fs == 16000 || stt->fs == 32000 || stt->fs == 48000) {
			if (samples != 160) {
				return -1;
			}
		}
		else {
			return -1;
		}

		if (WebRtcAgc_ProcessDigital(&stt->digitalAgc, in_near, out,
			stt->fs) == -1) {
			return -1;
		}
		return 0;
	}

	void* WebRtcAgc_Create_YM() {
		LegacyAgc* stt = (LegacyAgc *)malloc(sizeof(LegacyAgc));

		stt->initFlag = 0;
		stt->lastError = 0;

		return stt;
	}

	void WebRtcAgc_Free(void* state) {
		LegacyAgc* stt;

		stt = (LegacyAgc*)state;
#ifdef WEBRTC_AGC_DEBUG_DUMP
		fclose(stt->fpt);
		fclose(stt->agcLog);
		fclose(stt->digitalAgc.logFile);
#endif
		free(stt);
	}

	/* minLevel     - Minimum volume level
	 * maxLevel     - Maximum volume level
	 */
	int WebRtcAgc_Init(void* agcInst,
		int32_t minLevel,
		int32_t maxLevel,
		int16_t agcMode,
		uint32_t fs) {
		LegacyAgc* stt;

		/* typecast state pointer */
		stt = (LegacyAgc*)agcInst;

		if (WebRtcAgc_InitDigital(&stt->digitalAgc, agcMode) != 0) {
			stt->lastError = AGC_UNINITIALIZED_ERROR;
			return -1;
		}

		stt->fs = fs;

		// TODO(bjornv): Investigate if we really need to scale up a small range now
		// when we have
		// a guard against zero-increments. For now, we do not support scale up (scale
		// = 0).
//		stt->scale = 0;
//		maxLevel <<= stt->scale;
//		minLevel <<= stt->scale;

		/* Make minLevel and maxLevel static in AdaptiveDigital */
//		minLevel = 0;
//		maxLevel = 255;
		stt->scale = 0;

		stt->initFlag = kInitCheck;

		return 0;
	}
}
