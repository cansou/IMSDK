/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_PROCESSING_AGC_LEGACY_ANALOG_AGC_H_
#define WEBRTC_MODULES_AUDIO_PROCESSING_AGC_LEGACY_ANALOG_AGC_H_

//#define MIC_LEVEL_FEEDBACK
#ifdef WEBRTC_AGC_DEBUG_DUMP
#include <stdio.h>
#endif

#include "digital_agc.h"
#include "AGCPlatform.h"
#include "gain_control.h"

/* Analog Automatic Gain Control variables:
 * Constant declarations (inner limits inside which no changes are done)
 * In the beginning the range is narrower to widen as soon as the measure
 * 'Rxx160_LP' is inside it. Currently the starting limits are -22.2+/-1dBm0
 * and the final limits -22.2+/-2.5dBm0. These levels makes the speech signal
 * go towards -25.4dBm0 (-31.4dBov). Tuned with wbfile-31.4dBov.pcm
 * The limits are created by running the AGC with a file having the desired
 * signal level and thereafter plotting Rxx160_LP in the dBm0-domain defined
 * by out=10*log10(in/260537279.7); Set the target level to the average level
 * of our measure Rxx160_LP. Remember that the levels are in blocks of 16 in
 * Q(-7). (Example matlab code: round(db2pow(-21.2)*16/2^7) )
 */
namespace youmecommon
{
#define RXX_BUFFER_LEN 10

	static const int16_t kMsecSpeechInner = 520;
	static const int16_t kMsecSpeechOuter = 340;

	static const int16_t kNormalVadThreshold = 400;

	static const int16_t kAlphaShortTerm = 6;  // 1 >> 6 = 0.0156
	static const int16_t kAlphaLongTerm = 10;  // 1 >> 10 = 0.000977

	typedef struct {
		// Configurable parameters/variables
		uint32_t fs;                // Sampling frequency
		int16_t  scale;  // Scale factor for internal volume levels
		// Structs for VAD and digital_agc
		DigitalAgc digitalAgc;

		int16_t    initFlag;
		int16_t    lastError;
	} LegacyAgc;
}
#endif  // WEBRTC_MODULES_AUDIO_PROCESSING_AGC_LEGACY_ANALOG_AGC_H_
