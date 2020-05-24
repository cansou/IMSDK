/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/* digital_agc.c
 *
 */

#include "digital_agc.h"

#include <string.h>
#ifdef WEBRTC_AGC_DEBUG_DUMP
#include <stdio.h>
#endif

// To generate the gaintable, copy&paste the following lines to a Matlab window:
// MaxGain = 6; MinGain = 0; CompRatio = 3; Knee = 1;
// zeros = 0:31; lvl = 2.^(1-zeros);
// A = -10*log10(lvl) * (CompRatio - 1) / CompRatio;
// B = MaxGain - MinGain;
// gains = round(2^16*10.^(0.05 * (MinGain + B * (
// log(exp(-Knee*A)+exp(-Knee*B)) - log(1+exp(-Knee*B)) ) /
// log(1/(1+exp(Knee*B))))));
// fprintf(1, '\t%i, %i, %i, %i,\n', gains);
// % Matlab code for plotting the gain and input/output level characteristic
// (copy/paste the following 3 lines):
// in = 10*log10(lvl); out = 20*log10(gains/65536);
// subplot(121); plot(in, out); axis([-30, 0, -5, 20]); grid on; xlabel('Input
// (dB)'); ylabel('Gain (dB)');
// subplot(122); plot(in, in+out); axis([-30, 0, -30, 5]); grid on;
// xlabel('Input (dB)'); ylabel('Output (dB)');
// zoom on;
namespace youmecommon
{
	// Generator table for y=log2(1+e^x) in Q8.
	enum { kGenFuncTableSize = 128 };
	static const uint16_t kGenFuncTable[kGenFuncTableSize] = {
		256, 485, 786, 1126, 1484, 1849, 2217, 2586, 2955, 3324, 3693,
		4063, 4432, 4801, 5171, 5540, 5909, 6279, 6648, 7017, 7387, 7756,
		8125, 8495, 8864, 9233, 9603, 9972, 10341, 10711, 11080, 11449, 11819,
		12188, 12557, 12927, 13296, 13665, 14035, 14404, 14773, 15143, 15512, 15881,
		16251, 16620, 16989, 17359, 17728, 18097, 18466, 18836, 19205, 19574, 19944,
		20313, 20682, 21052, 21421, 21790, 22160, 22529, 22898, 23268, 23637, 24006,
		24376, 24745, 25114, 25484, 25853, 26222, 26592, 26961, 27330, 27700, 28069,
		28438, 28808, 29177, 29546, 29916, 30285, 30654, 31024, 31393, 31762, 32132,
		32501, 32870, 33240, 33609, 33978, 34348, 34717, 35086, 35456, 35825, 36194,
		36564, 36933, 37302, 37672, 38041, 38410, 38780, 39149, 39518, 39888, 40257,
		40626, 40996, 41365, 41734, 42104, 42473, 42842, 43212, 43581, 43950, 44320,
		44689, 45058, 45428, 45797, 46166, 46536, 46905 };

	// -12dB/30B
	//static const int32_t kGainTbl[32] = { 11480,  16475,  22960,  32950,  45916,  65888,  91820,  131416, 
	//                                      165688, 209096, 259008, 306288, 324944, 328528, 329024, 329072,
	//									  329120, 329120, 329072, 329120, 329120, 329072, 329120, 329072,
	//									  329072, 329120, 329120, 329072, 329120, 329072, 329072, 329120 };
	// -6dB/40dB
//	static const int32_t kGainTbl[32] = { 22890, 32862, 45780, 65708, 91544, 131416, 183088, 262832,
//		366144, 471008, 592608, 729760, 941344, 1177728, 1412288, 1589824,
//		1642496, 1651008, 1651776, 1652352, 1652032, 1652352, 1652352, 1652032,
//		1652352, 1652032, 1652032, 1652352, 1652032, 1652032, 1652352, 1652032 };

	// -3dB/50dB
	static const int32_t kGainTbl[32] = { 32814,   45708,   65620,   91416,   131256,  182824,  262496,  365648,
	    524992,  731296,  1046336, 1320256, 1668032, 2092928, 2639744, 3333504,
	    4161408, 5086464, 5607936, 5742848, 5764352, 5766400, 5767680, 5767168,
	    5767680, 5767680, 5767168, 5767680, 5767680, 5767168, 5767680, 5767680 };

	static const int16_t kAvgDecayTime = 250;  // frames; < 3000

	// allpass filter coefficients.
	static const uint16_t kResampleAllpass1[3] = { 3284, 24441, 49528 };
	static const uint16_t kResampleAllpass2[3] = { 12199, 37471, 60255 };

	// C + the 32 most significant bits of A * B
#define WEBRTC_SPL_SCALEDIFF32(A, B, C) \
	(C + (B >> 16) * A + (((uint32_t)(0x0000FFFF & B) * A) >> 16))
	// Multiply a 32-bit value with a 16-bit value and accumulate to another input:
#define MUL_ACCUM_1(a, b, c) WEBRTC_SPL_SCALEDIFF32(a, b, c)
#define MUL_ACCUM_2(a, b, c) WEBRTC_SPL_SCALEDIFF32(a, b, c)

	static int16_t WebRtcSpl_SatW32ToW16(int32_t value32) {
		int16_t out16 = (int16_t)value32;

		if (value32 > 32767)
			out16 = 32767;
		else if (value32 < -32768)
			out16 = -32768;

		return out16;
	}

	static  int16_t WebRtcSpl_NormW32(int32_t a) {
		int16_t zeros;

		if (a == 0) {
			return 0;
		}
		else if (a < 0) {
			a = ~a;
		}

		if (!(0xFFFF8000 & a)) {
			zeros = 16;
		}
		else {
			zeros = 0;
		}
		if (!(0xFF800000 & (a << zeros))) zeros += 8;
		if (!(0xF8000000 & (a << zeros))) zeros += 4;
		if (!(0xE0000000 & (a << zeros))) zeros += 2;
		if (!(0xC0000000 & (a << zeros))) zeros += 1;

		return zeros;
	}

	static int16_t WebRtcSpl_NormU32(uint32_t a) {
		int16_t zeros;

		if (a == 0) return 0;

		if (!(0xFFFF0000 & a)) {
			zeros = 16;
		}
		else {
			zeros = 0;
		}
		if (!(0xFF000000 & (a << zeros))) zeros += 8;
		if (!(0xF0000000 & (a << zeros))) zeros += 4;
		if (!(0xC0000000 & (a << zeros))) zeros += 2;
		if (!(0x80000000 & (a << zeros))) zeros += 1;

		return zeros;
	}

	static int16_t WebRtcSpl_AddSatW16(int16_t a, int16_t b)
	{
		int16_t out;
		int32_t tmp = (int32_t)a + (int32_t)b;
		if (tmp > 32767) {
			tmp = 32767;
		}
		else if (tmp < -32768) {
			tmp = -32768;
		}
		out = (int16_t)tmp;
		return out;
	}

	static int16_t WebRtcSpl_DivW32W16ResW16(int32_t num, int16_t den)
	{
		// Guard against division with 0
		if (den != 0)
		{
			return (int16_t)(num / den);
		}
		else
		{
			return (int16_t)0x7FFF;
		}
	}

	static int32_t WebRtcSpl_DivW32W16(int32_t num, int16_t den)
	{
		// Guard against division with 0
		if (den != 0)
		{
			return (int32_t)(num / den);
		}
		else
		{
			return (int32_t)0x7FFFFFFF;
		}
	}

	// decimator
	void WebRtcSpl_DownsampleBy2(const int16_t* in, size_t len,
		int16_t* out, int32_t* filtState) {
		int32_t tmp1, tmp2, diff, in32, out32;
		size_t i;

		register int32_t state0 = filtState[0];
		register int32_t state1 = filtState[1];
		register int32_t state2 = filtState[2];
		register int32_t state3 = filtState[3];
		register int32_t state4 = filtState[4];
		register int32_t state5 = filtState[5];
		register int32_t state6 = filtState[6];
		register int32_t state7 = filtState[7];

		for (i = (len >> 1); i > 0; i--) {
			// lower allpass filter
			in32 = (int32_t)(*in++) << 10;
			diff = in32 - state1;
			tmp1 = MUL_ACCUM_1(kResampleAllpass2[0], diff, state0);
			state0 = in32;
			diff = tmp1 - state2;
			tmp2 = MUL_ACCUM_2(kResampleAllpass2[1], diff, state1);
			state1 = tmp1;
			diff = tmp2 - state3;
			state3 = MUL_ACCUM_2(kResampleAllpass2[2], diff, state2);
			state2 = tmp2;

			// upper allpass filter
			in32 = (int32_t)(*in++) << 10;
			diff = in32 - state5;
			tmp1 = MUL_ACCUM_1(kResampleAllpass1[0], diff, state4);
			state4 = in32;
			diff = tmp1 - state6;
			tmp2 = MUL_ACCUM_1(kResampleAllpass1[1], diff, state5);
			state5 = tmp1;
			diff = tmp2 - state7;
			state7 = MUL_ACCUM_2(kResampleAllpass1[2], diff, state6);
			state6 = tmp2;

			// add two allpass outputs, divide by two and round
			out32 = (state3 + state7 + 1024) >> 11;

			// limit amplitude to prevent wrap-around, and write to output array
			*out++ = WebRtcSpl_SatW32ToW16(out32);
		}

		filtState[0] = state0;
		filtState[1] = state1;
		filtState[2] = state2;
		filtState[3] = state3;
		filtState[4] = state4;
		filtState[5] = state5;
		filtState[6] = state6;
		filtState[7] = state7;
	}

	int32_t WebRtcSpl_SqrtLocal(int32_t in)
	{

		int16_t x_half, t16;
		int32_t A, B, x2;

		/* The following block performs:
		y=in/2
		x=y-2^30
		x_half=x/2^31
		t = 1 + (x_half) - 0.5*((x_half)^2) + 0.5*((x_half)^3) - 0.625*((x_half)^4)
		+ 0.875*((x_half)^5)
		*/

		B = in / 2;

		B = B - ((int32_t)0x40000000); // B = in/2 - 1/2
		x_half = (int16_t)(B >> 16);  // x_half = x/2 = (in-1)/2
		B = B + ((int32_t)0x40000000); // B = 1 + x/2
		B = B + ((int32_t)0x40000000); // Add 0.5 twice (since 1.0 does not exist in Q31)

		x2 = ((int32_t)x_half) * ((int32_t)x_half) * 2; // A = (x/2)^2
		A = -x2; // A = -(x/2)^2
		B = B + (A >> 1); // B = 1 + x/2 - 0.5*(x/2)^2

		A >>= 16;
		A = A * A * 2; // A = (x/2)^4
		t16 = (int16_t)(A >> 16);
		B += -20480 * t16 * 2;  // B = B - 0.625*A
		// After this, B = 1 + x/2 - 0.5*(x/2)^2 - 0.625*(x/2)^4

		A = x_half * t16 * 2;  // A = (x/2)^5
		t16 = (int16_t)(A >> 16);
		B += 28672 * t16 * 2;  // B = B + 0.875*A
		// After this, B = 1 + x/2 - 0.5*(x/2)^2 - 0.625*(x/2)^4 + 0.875*(x/2)^5

		t16 = (int16_t)(x2 >> 16);
		A = x_half * t16 * 2;  // A = x/2^3

		B = B + (A >> 1); // B = B + 0.5*A
		// After this, B = 1 + x/2 - 0.5*(x/2)^2 + 0.5*(x/2)^3 - 0.625*(x/2)^4 + 0.875*(x/2)^5

		B = B + ((int32_t)32768); // Round off bit

		return B;
	}

	int32_t WebRtcSpl_Sqrt(int32_t value)
	{
		int16_t x_norm, nshift, t16, sh;
		int32_t A;

		int16_t k_sqrt_2 = 23170; // 1/sqrt2 (==5a82)

		A = value;

		// The convention in this function is to calculate sqrt(abs(A)). Negate the
		// input if it is negative.
		if (A < 0) {
			if (A == WEBRTC_SPL_WORD32_MIN) {
				// This number cannot be held in an int32_t after negating.
				// Map it to the maximum positive value.
				A = WEBRTC_SPL_WORD32_MAX;
			}
			else {
				A = -A;
			}
		}
		else if (A == 0) {
			return 0;  // sqrt(0) = 0
		}

		sh = WebRtcSpl_NormW32(A); // # shifts to normalize A
		A = WEBRTC_SPL_LSHIFT_W32(A, sh); // Normalize A
		if (A < (WEBRTC_SPL_WORD32_MAX - 32767))
		{
			A = A + ((int32_t)32768); // Round off bit
		}
		else
		{
			A = WEBRTC_SPL_WORD32_MAX;
		}

		x_norm = (int16_t)(A >> 16);  // x_norm = AH

		nshift = (sh / 2);
		//RTC_DCHECK_GE(nshift, 0);

		A = (int32_t)WEBRTC_SPL_LSHIFT_W32((int32_t)x_norm, 16);
		A = WEBRTC_SPL_ABS_W32(A); // A = abs(x_norm<<16)
		A = WebRtcSpl_SqrtLocal(A); // A = sqrt(A)

		if (2 * nshift == sh) {
			// Even shift value case

			t16 = (int16_t)(A >> 16);  // t16 = AH

			A = k_sqrt_2 * t16 * 2;  // A = 1/sqrt(2)*t16
			A = A + ((int32_t)32768); // Round off
			A = A & ((int32_t)0x7fff0000); // Round off

			A >>= 15;  // A = A>>16

		}
		else
		{
			A >>= 16;  // A = A>>16
		}

		A = A & ((int32_t)0x0000ffff);
		A >>= nshift;  // De-normalize the result.

		return A;
	}

	int32_t WebRtcAgc_InitDigital(DigitalAgc* stt, int16_t agcMode) {
		// start out with 0 dB gain
		stt->capacitorSlow = 134217728;  // (int32_t)(0.125f * 32768.0f * 32768.0f);

		stt->capacitorFast = 0;
		stt->gain = 65536;
		stt->gatePrevious = 0;
		stt->agcMode = agcMode;
#ifdef WEBRTC_AGC_DEBUG_DUMP
		stt->frameCounter = 0;
#endif

		// initialize VADs
		WebRtcAgc_InitVad(&stt->vadNearend);

		// Very important, set the digital gain table
		for (int i = 0; i < 32; i++)
		{
			stt->gainTable[i] = kGainTbl[i];
		}
		return 0;
	}

	int32_t WebRtcAgc_ProcessDigital(DigitalAgc* stt,
		const int16_t* inNear,
		int16_t* out,
		uint32_t FS) {
		// array for gains (one value per ms, incl start & end)
		int32_t gains[11] = { 0 };
		int32_t out_tmp, tmp32;
		int32_t env[10] = { 0 };
		int32_t max_nrg;
		int32_t cur_level;
		int32_t gain32, delta;
		int16_t logratio;
		int16_t lower_thr, upper_thr;
		int16_t zeros = 0, zeros_fast, frac = 0;
		int16_t decay;
		int16_t gate, gain_adj;
		int16_t k;
		int16_t n, L;
		int16_t L2;  // samples/subframe

		// determine number of samples per ms
		if (FS == 8000) {
			L = 8;
			L2 = 3;
		}
		else if (FS == 16000 || FS == 32000 || FS == 48000) {
			L = 16;
			L2 = 4;
		}
		else {
			return -1;
		}

		// VAD for near end
		logratio = WebRtcAgc_ProcessVad(&stt->vadNearend, out, L * 10);

		// Determine decay factor depending on VAD
		//  upper_thr = 1.0f;
		//  lower_thr = 0.25f;
		upper_thr = 1024;  // Q10
		lower_thr = 0;     // Q10
		if (logratio > upper_thr) {
			// decay = -2^17 / DecayTime;  ->  -65
			decay = -65;
		}
		else if (logratio < lower_thr) {
			decay = 0;
		}
		else {
			// decay = (int16_t)(((lower_thr - logratio)
			//       * (2^27/(DecayTime*(upper_thr-lower_thr)))) >> 10);
			// SUBSTITUTED: 2^27/(DecayTime*(upper_thr-lower_thr))  ->  65
			tmp32 = (lower_thr - logratio) * 65;
			decay = (int16_t)(tmp32 >> 10);
		}

		// adjust decay factor for long silence (detected as low standard deviation)
		// This is only done in the adaptive modes
		if (stt->vadNearend.stdLongTerm < 4000) {
			decay = 0;
		}
		else if (stt->vadNearend.stdLongTerm < 8096) {
			// decay = (int16_t)(((stt->vadNearend.stdLongTerm - 4000) * decay) >>
			// 12);
			tmp32 = (stt->vadNearend.stdLongTerm - 4000) * decay;
			decay = (int16_t)(tmp32 >> 12);
		}

		decay = 0;

#ifdef WEBRTC_AGC_DEBUG_DUMP
		stt->frameCounter++;
		fprintf(stt->logFile, "%5.2f\t%d\t%d\t%d\t", (float)(stt->frameCounter) / 100,
			logratio, decay, stt->vadNearend.stdLongTerm);
#endif
		// Find max amplitude per sub frame
		// iterate over sub frames
		for (k = 0; k < 10; k++) {
			// iterate over samples
			max_nrg = 0;
			for (n = 0; n < L; n++) {
				int32_t nrg = out[k * L + n] * out[k * L + n];
				if (nrg > max_nrg) {
					max_nrg = nrg;
				}
			}
			env[k] = max_nrg;
		}

		// Calculate gain per sub frame
		gains[0] = stt->gain;
		for (k = 0; k < 10; k++) {
			// Fast envelope follower
			//  decay time = -131000 / -1000 = 131 (ms)
			stt->capacitorFast =
				AGC_SCALEDIFF32(-1000, stt->capacitorFast, stt->capacitorFast);
			if (env[k] > stt->capacitorFast) {
				stt->capacitorFast = env[k];
			}
			// Slow envelope follower
			if (env[k] > stt->capacitorSlow) {
				// increase capacitorSlow
				stt->capacitorSlow = AGC_SCALEDIFF32(500, (env[k] - stt->capacitorSlow),
					stt->capacitorSlow);
			}
			else {
				// decrease capacitorSlow
				stt->capacitorSlow =
					AGC_SCALEDIFF32(decay, stt->capacitorSlow, stt->capacitorSlow);
			}

			// use maximum of both capacitors as current level
			if (stt->capacitorFast > stt->capacitorSlow) {
				cur_level = stt->capacitorFast;
			}
			else {
				cur_level = stt->capacitorSlow;
			}
			// Translate signal level into gain, using a piecewise linear approximation
			// find number of leading zeros
			zeros = WebRtcSpl_NormU32((uint32_t)cur_level);
			if (cur_level == 0) {
				zeros = 31;
			}
			tmp32 = (cur_level << zeros) & 0x7FFFFFFF;
			frac = (int16_t)(tmp32 >> 19);  // Q12.
			tmp32 = (stt->gainTable[zeros - 1] - stt->gainTable[zeros]) * frac;
			gains[k + 1] = stt->gainTable[zeros] + (tmp32 >> 12);
#ifdef WEBRTC_AGC_DEBUG_DUMP
			if (k == 0) {
				fprintf(stt->logFile, "%d\t%d\t%d\t%d\t%d\n", env[0], cur_level,
					stt->capacitorFast, stt->capacitorSlow, zeros);
			}
#endif
		}

		// Gate processing (lower gain during absence of speech)
		zeros = (zeros << 9) - (frac >> 3);
		// find number of leading zeros
		zeros_fast = WebRtcSpl_NormU32((uint32_t)stt->capacitorFast);
		if (stt->capacitorFast == 0) {
			zeros_fast = 31;
		}
		tmp32 = (stt->capacitorFast << zeros_fast) & 0x7FFFFFFF;
		zeros_fast <<= 9;
		zeros_fast -= (int16_t)(tmp32 >> 22);

		gate = 1000 + zeros_fast - zeros - stt->vadNearend.stdShortTerm;

		if (gate < 0) {
			stt->gatePrevious = 0;
		}
		else {
			tmp32 = stt->gatePrevious * 7;
			gate = (int16_t)((gate + tmp32) >> 3);
			stt->gatePrevious = gate;
		}
		// gate < 0     -> no gate
		// gate > 2500  -> max gate
		if (gate > 0) {
			if (gate < 2500) {
				gain_adj = (2500 - gate) >> 5;
			}
			else {
				gain_adj = 0;
			}
			for (k = 0; k < 10; k++) {
				if ((gains[k + 1] - stt->gainTable[0]) > 8388608) {
					// To prevent wraparound
					tmp32 = (gains[k + 1] - stt->gainTable[0]) >> 8;
					tmp32 *= 178 + gain_adj;
				}
				else {
					tmp32 = (gains[k + 1] - stt->gainTable[0]) * (178 + gain_adj);
					tmp32 >>= 8;
				}
				gains[k + 1] = stt->gainTable[0] + tmp32;
			}
		}

		// Limit gain to avoid overload distortion
		for (k = 0; k < 10; k++) {
			// To prevent wrap around
			zeros = 10;
			if (gains[k + 1] > 47453132) {
				zeros = 16 - WebRtcSpl_NormW32(gains[k + 1]);
			}
			gain32 = (gains[k + 1] >> zeros) + 1;
			gain32 *= gain32;
			// check for overflow
			while (AGC_MUL32((env[k] >> 12) + 1, gain32) >
				WEBRTC_SPL_SHIFT_W32((int32_t)32767, 2 * (1 - zeros + 10))) {
				// multiply by 253/256 ==> -0.1 dB
				if (gains[k + 1] > 8388607) {
					// Prevent wrap around
					gains[k + 1] = (gains[k + 1] / 256) * 253;
				}
				else {
					gains[k + 1] = (gains[k + 1] * 253) / 256;
				}
				gain32 = (gains[k + 1] >> zeros) + 1;
				gain32 *= gain32;
			}
		}
		// gain reductions should be done 1 ms earlier than gain increases
		for (k = 1; k < 10; k++) {
			if (gains[k] > gains[k + 1]) {
				gains[k] = gains[k + 1];
			}
		}
		// save start gain for next frame
		stt->gain = gains[10];

		// Apply gain
		// handle first sub frame separately
		delta = (gains[1] - gains[0]) << (4 - L2);
		gain32 = gains[0] << 4;
		// iterate over samples
		for (n = 0; n < L; n++) {
			tmp32 = out[n] * ((gain32 + 127) >> 7);
			out_tmp = tmp32 >> 16;
			if (out_tmp > 4095) {
				out[n] = (int16_t)32767;
			}
			else if (out_tmp < -4096) {
				out[n] = (int16_t)-32768;
			}
			else {
				tmp32 = out[n] * (gain32 >> 4);
				out[n] = (int16_t)(tmp32 >> 16);
			}
			gain32 += delta;
		}
		// iterate over subframes
		for (k = 1; k < 10; k++) {
			delta = (gains[k + 1] - gains[k]) << (4 - L2);
			gain32 = gains[k] << 4;
			// iterate over samples
			for (n = 0; n < L; n++) {
				tmp32 = out[k * L + n] * (gain32 >> 4);
				out[k * L + n] = (int16_t)(tmp32 >> 16);
				gain32 += delta;
			}
		}

		return 0;
	}

	void WebRtcAgc_InitVad(AgcVad* state) {
		int16_t k;

		state->HPstate = 0;   // state of high pass filter
		state->logRatio = 0;  // log( P(active) / P(inactive) )
		// average input level (Q10)
		state->meanLongTerm = 15 << 10;

		// variance of input level (Q8)
		state->varianceLongTerm = 500 << 8;

		state->stdLongTerm = 0;  // standard deviation of input level in dB
		// short-term average input level (Q10)
		state->meanShortTerm = 15 << 10;

		// short-term variance of input level (Q8)
		state->varianceShortTerm = 500 << 8;

		state->stdShortTerm =
			0;               // short-term standard deviation of input level in dB
		state->counter = 3;  // counts updates
		for (k = 0; k < 8; k++) {
			// downsampling filter
			state->downState[k] = 0;
		}
	}

	int16_t WebRtcAgc_ProcessVad(AgcVad* state,      // (i) VAD state
		const int16_t* in,  // (i) Speech signal
		size_t nrSamples)   // (i) number of samples
	{
		int32_t out, nrg, tmp32, tmp32b;
		uint16_t tmpU16;
		int16_t k, subfr, tmp16;
		int16_t buf1[8];
		int16_t buf2[4];
		int16_t HPstate;
		int16_t zeros, dB;

		// process in 10 sub frames of 1 ms (to save on memory)
		nrg = 0;
		HPstate = state->HPstate;
		for (subfr = 0; subfr < 10; subfr++) {
			// downsample to 4 kHz
			if (nrSamples == 160) {
				for (k = 0; k < 8; k++) {
					tmp32 = (int32_t)in[2 * k] + (int32_t)in[2 * k + 1];
					tmp32 >>= 1;
					buf1[k] = (int16_t)tmp32;
				}
				in += 16;

				WebRtcSpl_DownsampleBy2(buf1, 8, buf2, state->downState);
			}
			else {
				WebRtcSpl_DownsampleBy2(in, 8, buf2, state->downState);
				in += 8;
			}

			// high pass filter and compute energy
			for (k = 0; k < 4; k++) {
				out = buf2[k] + HPstate;
				tmp32 = 600 * out;
				HPstate = (int16_t)((tmp32 >> 10) - buf2[k]);
				nrg += (out * out) >> 6;
			}
		}
		state->HPstate = HPstate;

		// find number of leading zeros
		if (!(0xFFFF0000 & nrg)) {
			zeros = 16;
		}
		else {
			zeros = 0;
		}
		if (!(0xFF000000 & (nrg << zeros))) {
			zeros += 8;
		}
		if (!(0xF0000000 & (nrg << zeros))) {
			zeros += 4;
		}
		if (!(0xC0000000 & (nrg << zeros))) {
			zeros += 2;
		}
		if (!(0x80000000 & (nrg << zeros))) {
			zeros += 1;
		}

		// energy level (range {-32..30}) (Q10)
		dB = (15 - zeros) << 11;

		// Update statistics

		if (state->counter < kAvgDecayTime) {
			// decay time = AvgDecTime * 10 ms
			state->counter++;
		}

		// update short-term estimate of mean energy level (Q10)
		tmp32 = state->meanShortTerm * 15 + dB;
		state->meanShortTerm = (int16_t)(tmp32 >> 4);

		// update short-term estimate of variance in energy level (Q8)
		tmp32 = (dB * dB) >> 12;
		tmp32 += state->varianceShortTerm * 15;
		state->varianceShortTerm = tmp32 / 16;

		// update short-term estimate of standard deviation in energy level (Q10)
		tmp32 = state->meanShortTerm * state->meanShortTerm;
		tmp32 = (state->varianceShortTerm << 12) - tmp32;
		state->stdShortTerm = (int16_t)WebRtcSpl_Sqrt(tmp32);

		// update long-term estimate of mean energy level (Q10)
		tmp32 = state->meanLongTerm * state->counter + dB;
		state->meanLongTerm =
			WebRtcSpl_DivW32W16ResW16(tmp32, WebRtcSpl_AddSatW16(state->counter, 1));

		// update long-term estimate of variance in energy level (Q8)
		tmp32 = (dB * dB) >> 12;
		tmp32 += state->varianceLongTerm * state->counter;
		state->varianceLongTerm =
			WebRtcSpl_DivW32W16(tmp32, WebRtcSpl_AddSatW16(state->counter, 1));

		// update long-term estimate of standard deviation in energy level (Q10)
		tmp32 = state->meanLongTerm * state->meanLongTerm;
		tmp32 = (state->varianceLongTerm << 12) - tmp32;
		state->stdLongTerm = (int16_t)WebRtcSpl_Sqrt(tmp32);

		// update voice activity measure (Q10)
		tmp16 = 3 << 12;
		// TODO(bjornv): (dB - state->meanLongTerm) can overflow, e.g., in
		// ApmTest.Process unit test. Previously the macro WEBRTC_SPL_MUL_16_16()
		// was used, which did an intermediate cast to (int16_t), hence losing
		// significant bits. This cause logRatio to max out positive, rather than
		// negative. This is a bug, but has very little significance.
		tmp32 = tmp16 * (int16_t)(dB - state->meanLongTerm);
		tmp32 = WebRtcSpl_DivW32W16(tmp32, state->stdLongTerm);
		tmpU16 = (13 << 12);
		tmp32b = WEBRTC_SPL_MUL_16_U16(state->logRatio, tmpU16);
		tmp32 += tmp32b >> 10;

		state->logRatio = (int16_t)(tmp32 >> 6);

		// limit
		if (state->logRatio > 2048) {
			state->logRatio = 2048;
		}
		if (state->logRatio < -2048) {
			state->logRatio = -2048;
		}

		return state->logRatio;  // Q10
	}
}
