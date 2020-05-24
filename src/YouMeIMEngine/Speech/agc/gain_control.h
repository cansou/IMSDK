/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_PROCESSING_AGC_LEGACY_GAIN_CONTROL_H_
#define WEBRTC_MODULES_AUDIO_PROCESSING_AGC_LEGACY_GAIN_CONTROL_H_
namespace youmecommon
{
	// Errors
#define AGC_UNSPECIFIED_ERROR 18000
#define AGC_UNSUPPORTED_FUNCTION_ERROR 18001
#define AGC_UNINITIALIZED_ERROR 18002
#define AGC_NULL_POINTER_ERROR 18003
#define AGC_BAD_PARAMETER_ERROR 18004

	// Warnings
#define AGC_BAD_PARAMETER_WARNING 18050

	enum {
		kAgcModeUnchanged,
		kAgcModeAdaptiveAnalog,
		kAgcModeAdaptiveDigital,
		kAgcModeFixedDigital
	};

	enum { kAgcFalse = 0, kAgcTrue };

	typedef struct {
		int16_t targetLevelDbfs;    // default 3 (-3 dBOv)
		int16_t compressionGaindB;  // default 9 dB
		uint8_t limiterEnable;      // default kAgcTrue (on)
	} WebRtcAgcConfig;

#if defined(__cplusplus)
	extern "C" {
#endif

		/*
		 * This function processes a 10 ms frame and adjusts (normalizes) the gain both
		 * analog and digitally. The gain adjustments are done only during active
		 * periods of speech. The length of the speech vectors must be given in samples
		 * (80 when FS=8000, and 160 when FS=16000, FS=32000 or FS=48000). The echo
		 * parameter can be used to ensure the AGC will not adjust upward in the
		 * presence of echo.
		 *
		 * This function should be called after processing the near-end microphone
		 * signal, in any case after any echo cancellation.
		 *
		 * Input:
		 *      - agcInst           : AGC instance
		 *      - inNear            : Near-end input speech vector for each band
		 *      - samples           : Number of samples in input/output vector
		 *
		 * Output:
		 *      - out               : Gain-adjusted near-end speech vector
		 *                          : May be the same vector as the input.
		 *
		 * Return value:
		 *                          :  0 - Normal operation.
		 *                          : -1 - Error
		 */
		int WebRtcAgc_Process(void* agcInst,
			const int16_t* inNear,
			size_t samples,
			int16_t* out);

		/*
		 * This function creates and returns an AGC instance, which will contain the
		 * state information for one (duplex) channel.
		 */
		void* WebRtcAgc_Create_YM();

		/*
		 * This function frees the AGC instance created at the beginning.
		 *
		 * Input:
		 *      - agcInst           : AGC instance.
		 */
		void WebRtcAgc_Free(void* agcInst);

		/*
		 * This function initializes an AGC instance.
		 *
		 * Input:
		 *      - agcInst           : AGC instance.
		 *      - minLevel          : Minimum possible mic level
		 *      - maxLevel          : Maximum possible mic level
		 *      - agcMode           : 0 - Unchanged
		 *                          : 1 - Adaptive Analog Automatic Gain Control -3dBOv
		 *                          : 2 - Adaptive Digital Automatic Gain Control -3dBOv
		 *                          : 3 - Fixed Digital Gain 0dB
		 *      - fs                : Sampling frequency
		 *
		 * Return value             :  0 - Ok
		 *                            -1 - Error
		 */
		int WebRtcAgc_Init(void* agcInst,
			int32_t minLevel,
			int32_t maxLevel,
			int16_t agcMode,
			uint32_t fs);

#if defined(__cplusplus)
	}
#endif
}
#endif  // WEBRTC_MODULES_AUDIO_PROCESSING_AGC_LEGACY_GAIN_CONTROL_H_
