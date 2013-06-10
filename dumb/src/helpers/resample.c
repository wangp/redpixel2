/*  _______         ____    __         ___    ___
 * \    _  \       \    /  \  /       \   \  /   /     '   '  '
 *  |  | \  \       |  |    ||         |   \/   |       .      .
 *  |  |  |  |      |  |    ||         ||\  /|  |
 *  |  |  |  |      |  |    ||         || \/ |  |       '  '  '
 *  |  |  |  |      |  |    ||         ||    |  |       .      .
 *  |  |_/  /        \  \__//          ||    |  |
 * /_______/edicated  \____/niversal  /__\  /____\usic /|  .  . astardisation
 *                                                    /  \
 *                                                   / .  \
 * resample.c - Resampling helper.                  / / \  \
 *                                                 | <  /   \_
 * By Bob and entheh.                              |  \/ /\   /
 *                                                  \_  /  > /
 * In order to find a good trade-off between          | \ / /
 * speed and accuracy in this code, some tests        |  ' /
 * were carried out regarding the behaviour of         \__/
 * long long ints with gcc. The following code
 * was tested:
 *
 * int a, b, c;
 * c = ((long long)a * b) >> 16;
 *
 * DJGPP GCC Version 3.0.3 generated the following assembly language code for
 * the multiplication and scaling, leaving the 32-bit result in EAX.
 *
 * movl  -8(%ebp), %eax    ; read one int into EAX
 * imull -4(%ebp)          ; multiply by the other; result goes in EDX:EAX
 * shrdl $16, %edx, %eax   ; shift EAX right 16, shifting bits in from EDX
 *
 * Note that a 32*32->64 multiplication is performed, allowing for high
 * accuracy. On the Pentium 2 and above, shrdl takes two cycles (generally),
 * so it is a minor concern when four multiplications are being performed
 * (the cubic resampler). On the Pentium MMX and earlier, it takes four or
 * more cycles, so this method is unsuitable for use in the low-quality
 * resamplers.
 *
 * Since "long long" is a gcc-specific extension, we use LONG_LONG instead,
 * defined in dumb.h. We may investigate later what code MSVC generates, but
 * if it seems too slow then we suggest you use a good compiler.
 */

#include "dumb.h"



/* Compile with -DHEAVYDEBUG if you want to make sure the pick-up function is
 * called when it should be. There will be a considerable performance hit,
 * since at least one condition has to be tested for every sample generated.
 */
#ifdef HEAVYDEBUG
#define HEAVYASSERT(cond) ASSERT(cond)
#else
#define HEAVYASSERT(cond)
#endif


/* Sets the quality of the resampling. A value from 0 to 4 can be used here.
 *
 * When interpolating (decreasing the pitch), the following applies:
 *
 *  0   - aliasing
 *  1,2 - linear resampling
 *  3   - quadratic
 *  4   - cubic
 *
 * When decimating (increasing the pitch), an alternative 'linear average'
 * function will be applied for 2+. This reduces high frequencies that would
 * otherwise 'reflect' off the Nyquist (half the sampling rate) and corrupt
 * the sound.
 *
 * Clearly 0 is the fastest and 4 is the nicest. Higher values will behave
 * the same as 4.
 */
int dumb_resampling_quality = 4;



/* The following macro is used to overcome the fact that most C
 * compilers (including gcc and MSVC) can't correctly multiply signed
 * integers outside the range -32768 to 32767. i86 assembler versions
 * don't need to use this method, since the processor does in fact
 * have instructions to multiply large numbers correctly - which
 * means using assembly language could make a significant difference
 * to the speed.
 *
 * The basic method is as follows. We halve the subposition (how far
 * we are between samples), so it never exceeds 32767. We also halve
 * the delta, which is the amount to be added to the subposition each
 * time. Then we unroll the loop twofold, so that we can add the lost
 * one every other time if necessary (since the halving may have
 * resulted in rounding down).
 *
 * This method doesn't incur any cumulative inaccuracies. There is a
 * very slight loss of quality, which I challenge anyone to notice -
 * but the position will advance at *exactly* the same rate as it
 * would if we didn't use this method. This also means the pitch is
 * exactly the same, which may even make a difference to trained
 * musicians when resampling down a lot :)
 *
 * Each time this macro is invoked, DO_RESAMPLE(inc) must be defined
 * to calculate the samples by the appropriate equation (linear,
 * cubic, etc.). See the individual cases for examples of how this is
 * done.
 */
#define MAKE_RESAMPLER()							\
{													\
	if (dt & 1) {									\
		long todo2;									\
													\
		dt >>= 1;									\
													\
		if (src_subpos & 1) {						\
			src_subpos >>= 1;						\
			DO_RESAMPLE(1);							\
			todo--;									\
		} else										\
			src_subpos >>= 1;						\
													\
		todo2 = todo >> 1;							\
													\
		while (todo2) {								\
			DO_RESAMPLE(0);							\
			DO_RESAMPLE(1);							\
			todo2--;								\
		}											\
													\
		if (todo & 1) {								\
			DO_RESAMPLE(0);							\
			src_subpos = (src_subpos << 1) | 1;		\
		} else										\
			src_subpos <<= 1;						\
													\
		todo = 0;									\
		dt = (dt << 1) | 1;							\
	} else {										\
		long subposbit = src_subpos & 1;			\
		dt >>= 1;									\
		src_subpos >>= 1;							\
													\
		if (todo & 1) {								\
			DO_RESAMPLE(0);							\
		}											\
													\
		todo >>= 1;									\
													\
		while (todo) {								\
			DO_RESAMPLE(0);							\
			DO_RESAMPLE(0);							\
			todo--;									\
		}											\
													\
		src_subpos = (src_subpos << 1) | subposbit; \
		dt <<= 1;									\
	}												\
}



sample_t dumb_resample_get_value(
	sample_t *src, long *_src_pos, int *_src_subpos,
	long src_start, long src_end,
	float volume, int *_dir,
	DUMB_RESAMPLE_PICKUP pickup, void *pickup_data
)
{
	long src_pos = *_src_pos;
	int src_subpos = *_src_subpos;
	int dir = _dir ? *_dir : 1;

	sample_t value = 0;

	if (dir == 0)
		return 0;

	ASSERT(dir == 1 || dir == -1);

	if (dir < 0 ? (src_pos < src_start) : (src_pos >= src_end)) {

		/* If there's no pick-up function, we stop. */
		if (!pickup) {
			dir = 0;
			goto end;
		}

		/* Process the pick-up. It may need invoking more than once. */
		do {
			dir = (*pickup)(src, &src_pos, &src_subpos, &src_start, &src_end, dir, pickup_data);

			if (dir == 0)
				goto end;

			ASSERT(dir == 1 || dir == -1);
		} while (dir < 0 ? (src_pos < src_start) : (src_pos >= src_end));
	}

	HEAVYASSERT(dir < 0 ? (src_pos >= src_start) : (src_pos < src_end));

	if (dumb_resampling_quality == 0) {
		/* Aliasing (coarse) */
		int volume_fact = (int)(volume * 16384.0);
		value = (src[src_pos] * volume_fact) >> 14;
	} else if (dumb_resampling_quality <= 2) {
		/* Linear interpolation */
		int volume_fact = (int)(volume * 16384.0);
		int subpos = src_subpos >> 1;
		value = ((src[src_pos] + ((((src[src_pos + 1] - src[src_pos]) >> 1) * subpos) >> 14)) * volume_fact) >> 14;
	} else if (dumb_resampling_quality == 3) {
		/* Quadratic interpolation */
		int volume_fact = (int)(volume * 16384.0);
		int a, b;
		sample_t *x;
		int subpos = src_subpos >> 1;
		x = &src[src_pos];
		a = ((x[0] + x[2]) >> 1) - x[1];
		b = ((x[2] - x[0]) >> 1) - (a << 1);
		value = (((((((a * subpos) >> 15) + b) * subpos) >> 15) + x[0]) * volume_fact) >> 14;
	} else {
		/* Cubic interpolation */
		int volume_fact = (int)(volume * 16384.0);
		int a, b, c;
		sample_t *x;
		int subpos = src_subpos >> 1;
		x = &src[src_pos];
		a = (((x[1] - x[2]) << 1) + (x[1] - x[2]) + (x[3] - x[0])) >> 1;
		b = (x[2] << 1) + x[0] - ((5 * x[1] + x[3]) >> 1);
		c = (x[2] - x[0]) >> 1;
		value = (((int)(((LONG_LONG)((int)(((LONG_LONG)((int)(((LONG_LONG)a * subpos) >> 15) + b) * subpos) >> 15) + c) * subpos) >> 15) + x[1]) * volume_fact) >> 14;
	}

	end:

	*_src_pos = src_pos;
	*_src_subpos = src_subpos;
	if (_dir) *_dir = dir;

	return value;
}



long dumb_resample(
	sample_t *src, long *_src_pos, int *_src_subpos,
	long src_start, long src_end,
	sample_t *dst, long dst_size,
	float volume, float delta, int *_dir,
	DUMB_RESAMPLE_PICKUP pickup, void *pickup_data
)
{
	int dt = (int)(delta * 65536.0 + 0.5);
	long s = 0; /* Current position in the destination buffer */

	long src_pos = *_src_pos;
	int src_subpos = *_src_subpos;
	int dir = _dir ? *_dir : 1;

	int linear_average;

	if (dir == 0)
		return 0;

	ASSERT(dir == 1 || dir == -1);

	linear_average = dst && dumb_resampling_quality >= 2 && dt > 65536;

	if (dir < 0) dt = -dt;

	if (linear_average)
		volume /= delta;

	while (s < dst_size) {

		long todo;

		/* Process pick-ups first, just in case. */

		if (linear_average) {

			/* For linear average, the pick-up point could split a sum into
			 * two parts. We handle this by putting the pick-up code inside
			 * the summing loop. Note that this code is only executed when we
			 * know that a pick-up is necessary somewhere during this sum
			 * (although it is always executed once for the first sample).
			 * We use a separate loop further down when we know we won't have
			 * to do a pick-up, so the condition does not need testing inside
			 * the loop.
			 */

			float sum;
			long i;
			int advance;
			int x[3];

			advance = src_subpos + dt;

			/* Make these negative. Then they stay within the necessary
			 * range for integer multiplication, -32768 to 32767 ;)
			 */
			x[0] = ~(src_subpos >> 1); /* = -1 - (src_subpos >> 1) */
			x[2] = x[0] ^ 0x7FFF; /* = -32768 + (src_subpos >> 1) */

			sum = (float)(-((src[src_pos] * (x+1)[dir]) >> 15));

			i = src_pos + (advance >> 16);
			src_pos += dir;
			src_subpos = (dir >> 1) & 65535; /* changes 1,-1 to 0,65535 */

			advance &= 65535;

			/* i is the index of the first sample NOT to sum fully,
			 * regardless of the direction of resampling.
			 */

			while (dir < 0 ? (i < src_start) : (i >= src_end)) {
				if (dir < 0) {
					while (src_pos >= src_start)
						sum += src[src_pos--];
				} else {
					while (src_pos < src_end)
						sum += src[src_pos++];
				}

				i -= src_pos;
				/* i is now the number of samples left to sum fully, except
				 * it's negative if we're going backwards.
				 */

				if (!pickup) {
					dir = 0;
					goto endsum;
				}

				dir = (*pickup)(src, &src_pos, &src_subpos, &src_start, &src_end, dir, pickup_data);

				if (dir == 0)
					goto endsum;

				ASSERT(dir == 1 || dir == -1);

				if ((dir ^ dt) < 0) {
					dt = -dt;
					advance ^= 65535;
					i = -i;
				}

				i += src_pos;
				/* There, i is back to normal. */
			}

			for (; src_pos != i; src_pos += dir)
				sum += src[src_pos];

			src_subpos = advance;

			x[2] = src_subpos >> 1;
			x[0] = x[2] ^ 0x7FFF; /* = 32767 - (src_subpos >> 1) */

			sum += (src[src_pos] * (x+1)[dir]) >> 15;

			endsum:

			sum *= volume;
			dst[s] += (int)sum;

			s++;

			if (dir == 0)
				break;

		} else if (dir < 0 ? (src_pos < src_start) : (src_pos >= src_end)) {

			/* If there's no pick-up function, we stop. */
			if (!pickup) {
				dir = 0;
				break;
			}

			/* Process the pick-up. It may need invoking more than once. */
			do {
				dir = (*pickup)(src, &src_pos, &src_subpos, &src_start, &src_end, dir, pickup_data);

				if (dir == 0)
					goto end;

				ASSERT(dir == 1 || dir == -1);
			} while (dir < 0 ? (src_pos < src_start) : (src_pos >= src_end));

			/* Update sign of dt to match that of dir. */
			if ((dir ^ dt) < 0)
				dt = -dt;
		}

		/* Work out how many contiguous samples we can now render. */
		if (dir < 0)
			todo = (long)((((LONG_LONG)(src_pos - src_start) << 16) + src_subpos) / -dt);
		else
			todo = (long)((((LONG_LONG)(src_end - src_pos) << 16) - src_subpos - 1) / dt);

		/* The above equations work out how many complete dt-sized
		 * intervals there are between the current position and the loop
		 * point (provided there is a little fractional extra). The linear
		 * average function needs complete intervals - but the other
		 * resamplers only read a sample from the beginning of each interval,
		 * so they can process one extra sample in their main loops (so we
		 * increment todo in a moment).
		 *
		 * The linear average function makes up the extra sample using the
		 * specialised pick-up code above.
		 *
		 * Note that our above pick-up process should have absolutely ensured
		 * that the result of this function will be nonnegative.
		 */

		ASSERT(todo >= 0);

		if (!linear_average)
			todo++;

		/* Of course we don't want to overrun the output buffer! */
		if (todo > dst_size - s)
			todo = dst_size - s;

		if (!dst) {

			LONG_LONG t = src_subpos + (LONG_LONG)dt * todo;
			src_pos += (long)(t >> 16);
			src_subpos = (int)t & 0xFFFFl;

			s += todo;

		} else if (linear_average) {

			float sum;
			long i;
			int advance;
			int x[3];

			while (todo) {

				advance = src_subpos + dt;

				/* Make these negative. Then they stay within the necessary
				 * range for integer multiplication, -32768 to 32767 ;)
				 */
				x[0] = ~(src_subpos >> 1); /* = -1 - (src_subpos >> 1) */
				x[2] = x[0] ^ 0x7FFF; /* = -32768 + (src_subpos >> 1) */

				sum = (float)(-((src[src_pos] * (x+1)[dir]) >> 15));

				i = src_pos + (advance >> 16);
				src_pos += dir;
				src_subpos = (dir >> 1) & 65535; /* changes 1,-1 to 0,65535 */

				advance &= 65535;

				/* i is the index of the first sample NOT to sum fully,
				 * regardless of the direction of resampling.
				 */

				HEAVYASSERT(dir < 0 ? (i >= src_start) : (i < src_end));

				for (; src_pos != i; src_pos += dir)
					sum += src[src_pos];

				src_subpos = advance;

				x[2] = src_subpos >> 1;
				x[0] = x[2] ^ 0x7FFF; /* = 32767 - (src_subpos >> 1) */

				sum += (src[src_pos] * (x+1)[dir]) >> 15;

				sum *= volume;
				dst[s] += (int)sum;

				s++;
				todo--;
			}

		} else if (dumb_resampling_quality == 0 || (dumb_resampling_quality == 1 && delta >= 1.0)) {

			/* Aliasing (coarse) */
			int volume_fact = (int)(volume * 16384.0);

			do {
				HEAVYASSERT(dir < 0 ? (src_pos >= src_start) : (src_pos < src_end));
				dst[s] += ((src[src_pos] * volume_fact) >> 14);
				src_subpos += dt;
				src_pos += src_subpos >> 16;
				src_subpos &= 0xFFFFl;
				s++;
			} while (--todo);

		} else if (dumb_resampling_quality <= 2) {

			/* Linear interpolation */
			int volume_fact = (int)(volume * 16384.0);

			#define DO_RESAMPLE(inc)		 \
			{								 \
				HEAVYASSERT(dir < 0 ? (src_pos >= src_start) : (src_pos < src_end)); \
											 \
				dst[s] += (((src[src_pos] + ((((src[src_pos + 1] - src[src_pos]) >> 1) * src_subpos) >> 14)) * volume_fact) >> 14); \
											 \
				src_subpos += dt + inc;		 \
				src_pos += src_subpos >> 15; \
				src_subpos &= 0x7FFFl;		 \
				s++;						 \
			}

			MAKE_RESAMPLER();

			#undef DO_RESAMPLE

		} else if (dumb_resampling_quality == 3) {

			/* Quadratic interpolation */

			int volume_fact = (int)(volume * 16384.0);
			int a = 0, b = 0;
			sample_t *x = NULL;
			int last_src_pos = -1;

			/* AIM: no integer multiplicands must transcend the range -32768 to 32767.
			 * This limitation is imposed by most compilers, including gcc and MSVC.
			 *
			 * a = 0.5 * (s0 + s2) - s1
			 * b = -1.5 * s0 + 2 * s1 - 0.5 * s2
			 * c = s0
			 *
			 * s = (a * t + b) * t + c
			 *
			 * In fixed-point:
			 *
			 * a = ((s0 + s2) >> 1) - s1
			 * b = ((-3 * s0 - s2) >> 1) + (s1 << 1)
			 *
			 * s = (((((a * t) >> 16) + b) * t) >> 16) + s0
			 *
			 * With t halved (since t can reach 65535):
			 *
			 * s = (((((a * t) >> 15) + b) * t) >> 15) + s0
			 *
			 * a currently reaches 65536
			 * b currently reaches 131072
			 *
			 * So we must use aon2
			 *
			 * s = (((((aon2 * t) >> 14) + b) * t) >> 15) + s0
			 *
			 * ((aon2 * t) >> 14) + b is 5 times too big
			 * so we must divide by 8
			 *
			 * s = (((((aon2 * t) >> 17) + bon8) * t) >> 12) + s0
			 *
			 * aon2 = ((s0 + s2) >> 2) - (s1 >> 1)
			 * bon8 = ((-3 * s0 - s2) >> 4) + (s1 >> 2)
			 * or:
			 * bon8 = ((s2 - s0) >> 4) - (aon2 >> 1)
			 */

			/* Unh4x0r3d version:
			#define DO_RESAMPLE(inc)						\
			{												\
				HEAVYASSERT(dir < 0 ? (src_pos >= src_start) : (src_pos < src_end)); \
															\
				if (src_pos != last_src_pos) {				\
					last_src_pos = src_pos;					\
					x = &src[src_pos];						\
					a = ((x[0] + x[2]) >> 2) - (x[1] >> 1); \
					b = ((x[2] - x[0]) >> 4) - (a >> 1);	\
				}											\
															\
				dst[s] += ((((((((a * src_subpos) >> 17) + b) * src_subpos) >> 12) + x[0]) * volume_fact) >> 14); \
															\
				src_subpos += dt + inc;						\
				src_pos += src_subpos >> 15;				\
				src_subpos &= 0x7FFFl;						\
				s++;										\
			}
			*/

			/* H4x0r3d version: */
			#define DO_RESAMPLE(inc)						\
			{												\
				HEAVYASSERT(dir < 0 ? (src_pos >= src_start) : (src_pos < src_end)); \
															\
				if (src_pos != last_src_pos) {				\
					last_src_pos = src_pos;					\
					x = &src[src_pos];						\
					a = ((x[0] + x[2]) >> 1) - x[1];		\
					b = ((x[2] - x[0]) >> 1) - (a << 1);	\
				}											\
															\
				dst[s] += ((((((((a * src_subpos) >> 15) + b) * src_subpos) >> 15) + x[0]) * volume_fact) >> 14); \
															\
				src_subpos += dt + inc;						\
				src_pos += src_subpos >> 15;				\
				src_subpos &= 0x7FFFl;						\
				s++;										\
			}

			MAKE_RESAMPLER();

			#undef DO_RESAMPLE

		} else {

			/* Cubic interpolation */

			int volume_fact = (int)(volume * 16384.0);
			int a = 0, b = 0, c = 0;
			sample_t *x = NULL;
			int last_src_pos = -1;

			/* AIM: never multiply integers outside the range -32768 to 32767.
			 *
			 * a = 1.5f * (x[1] - x[2]) + (x[3] - x[0]) * 0.5f;
			 * b = 2.0f * x[2] + x[0] - 2.5f * x[1] - x[3] * 0.5f;
			 * c = (x[2] - x[0]) * 0.5f;
			 *
			 * s = ((a * t + b) * t + c) * t + x[1];
			 *
			 * Fixed-point version:
			 *
			 * a = (((x[1] - x[2]) << 1) + (x[1] - x[2]) + (x[3] - x[0])) >> 1;
			 * b = (x[2] << 1) + x[0] - ((5 * x[1] + x[3]) >> 1);
			 * c = (x[2] - x[0]) >> 1;
			 *
			 * s = ((((((((a * t) >> 15) + b) * t) >> 15) + c) * t) >> 15) + x[1];
			 *   (with t already halved, maximum 32767)
			 *
			 * a is in (((1+1)*2)+(1+1)+(1+1))/2 = 8 times the required range
			 * b is in (1*2)+1+((5*1+1)/2) = 6 times
			 * c is in the required range
			 *
			 * We must use aon8
			 *
			 * s = ((((((((aon8 * t) >> 12) + b) * t) >> 15) + c) * t) >> 15) + x[1];
			 *
			 * But ((aon8 * t) >> 12) is in 2^(15+15-12) = 2^18 = 8 times
			 * b is in 6 times
			 * so we divide both ((aon8 * t) >> 12) and b by 16
			 *
			 * s = ((((((((aon8 * t) >> 16) + bon16) * t) >> 11) + c) * t) >> 15) + x[1];
			 *
			 * ((... + bon16) * t) >> 11 is 16 times too big
			 * c is in the correct range
			 * we must divide both by 32
			 *
			 * s = ((((((((aon8 * t) >> 16) + bon16) * t) >> 16) + con32) * t) >> 10) + x[1];
			 *
			 * aon8  = (((x[1] - x[2]) << 1) + (x[1] - x[2]) + (x[3] - x[0])) >> 4;
			 * bon16 = ((x[2] << 2) + (x[0] << 1) - (5 * x[1] + x[3])) >> 5;
			 * con32 = (x[2] - x[0]) >> 6;
			 *
			 * A lot of accuracy is lost here. It is quite likely that some
			 * of the above would cancel anyway, so the scaling down wouldn't
			 * have to be so severe. However, I'm not in the mood to work it
			 * out now :P
			 *
			 * It may also be worth investigating whether doing this stuff
			 * in floats would be faster.
			 */

			/* Unh4x0r3d version:
			#define DO_RESAMPLE(inc)		   \
			{								   \
				HEAVYASSERT(dir < 0 ? (src_pos >= src_start) : (src_pos < src_end)); \
											   \
				if (src_pos != last_src_pos) { \
					last_src_pos = src_pos;	   \
					x = &src[src_pos];		   \
					a = (((x[1] - x[2]) << 1) + (x[1] - x[2]) + (x[3] - x[0])) >> 4; \
					b = ((x[2] << 2) + (x[0] << 1) - (5 * x[1] + x[3])) >> 5; \
					c = (x[2] - x[0]) >> 6;	   \
				}							   \
											   \
				dst[s] += ((((((((((a * src_subpos) >> 16) + b) * src_subpos) >> 16) + c) * src_subpos) >> 10) + x[1]) * volume_fact) >> 14; \
											   \
				src_subpos += dt + inc;		   \
				src_pos += src_subpos >> 15;   \
				src_subpos &= 0x7FFFl;		   \
				s++;						   \
			}
			*/

			/* H4x0r3d version: */
			#define DO_RESAMPLE(inc)		   \
			{								   \
				HEAVYASSERT(dir < 0 ? (src_pos >= src_start) : (src_pos < src_end)); \
											   \
				if (src_pos != last_src_pos) { \
					last_src_pos = src_pos;	   \
					x = &src[src_pos];		   \
					a = (((x[1] - x[2]) << 1) + (x[1] - x[2]) + (x[3] - x[0])) >> 1; \
					b = (x[2] << 1) + x[0] - ((5 * x[1] + x[3]) >> 1); \
					c = (x[2] - x[0]) >> 1;	   \
				}							   \
											   \
				dst[s] += (((int)(((LONG_LONG)((int)(((LONG_LONG)((int)(((LONG_LONG)a * src_subpos) >> 15) + b) * src_subpos) >> 15) + c) * src_subpos) >> 15) + x[1]) * volume_fact) >> 14; \
											   \
				src_subpos += dt + inc;		   \
				src_pos += src_subpos >> 15;   \
				src_subpos &= 0x7FFFl;		   \
				s++;						   \
			}

			MAKE_RESAMPLER();

			#undef DO_RESAMPLE

		}

	}

	end:

	ASSERT(s <= dst_size);

	*_src_pos = src_pos;
	*_src_subpos = src_subpos;
	if (_dir) *_dir = dir;

	return s;
}
