#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */
#include <math.h>       /*sqrt etc*/
#include <time.h>
#include "zmath.h"


//LOOKUP TABLE TRIGONOMETRY
u8 NSIN(u8 a)
{
	static u8 sine_lut[256] = 
	{
		128,	131,	134,	137,	140,	143,	146,	149,
		152,	155,	158,	162,	165,	167,	170,	173,
		176,	179,	182,	185,	188,	190,	193,	196,
		198,	201,	203,	206,	208,	211,	213,	215,
		218,	220,	222,	224,	226,	228,	230,	232,
		234,	235,	237,	238,	240,	241,	243,	244,
		245,	246,	248,	249,	250,	250,	251,	252,
		253,	253,	254,	254,	254,	255,	255,	255,
		255,	255,	255,	255,	254,	254,	254,	253,
		253,	252,	251,	250,	250,	249,	248,	246,
		245,	244,	243,	241,	240,	238,	237,	235,
		234,	232,	230,	228,	226,	224,	222,	220,
		218,	215,	213,	211,	208,	206,	203,	201,
		198,	196,	193,	190,	188,	185,	182,	179,
		176,	173,	170,	167,	165,	162,	158,	155,
		152,	149,	146,	143,	140,	137,	134,	131,
		128,	124,	121,	118,	115,	112,	109,	106,
		103,	100,	97,		93,		90,		88,		85,		82,
		79,		76,		73,		70,		67,		65,		62,		59,
		57,		54,		52,		49,		47,		44,		42,		40,
		37,		35,		33,		31,		29,		27,		25,		23,
		21,		20,		18,		17,		15,		14,		12,		11,
		10,		9,		7,		6,		5,		5,		4,		3,
		2,		2,		1,		1,		1,		0,		0,		0,
		0,		0,		0,		0,		1,		1,		1,		2,
		2,		3,		4,		5,		5,		6,		7,		9,
		10,		11,		12,		14,		15,		17,		18,		20,
		21,		23,		25,		27,		29,		31,		33,		35,
		37,		40,		42,		44,		47,		49,		52,		54,
		57,		59,		62,		65,		67,		70,		73,		76,
		79,		82,		85,		88,		90,		93,		97,		100,
		103,	106,	109,	112,	115,	118,	121,	124
	};
	return sine_lut[a];
}

u8 NCOS(u8 a)
{
	return NSIN(a + 64);
}

r32 RSIN(u8 a)
{
	//return (r32)((NSIN(a) / 255.f) * 2.f - 1.f);

	//alternative: no divide
	return (r32)((((NSIN(a)+1)*0.00390625f) * 2.f) - 1.f);
}

r32 RCOS(u8 a)
{
	//return (r32)((NCOS(a) / 255.f) * 2.f - 1.f);

	//alternative: no divide
	return (r32)((((NCOS(a)+1)*0.00390625f) * 2.f) - 1.f);
}


/*
** k < 1 and k > 0 gives slow start and fast end (slower start closer to 0) 
** k >> 1 gives linear
** k < -1 and k > -2.5 gives fast start and slow end (steeper closer to -1)
** k between -1 and 0 inclusive is undefined
** invert with 1 - CURVE
*/ 
r32 CURVE(r32 t, r32 k)
{
	r32 _t = ClampR32(t, 0.f, 1.f);
	if ((k >= -1) && (k <= 0))
		return t;
	else
		return ((k * _t) / (k - _t + 1));
}

/*
** t = 0 	--> 0
** t = 0.5	--> 1
** t = 1	--> 0
** parametric smoothing inbetween
*/ 
r32 PARAMETRIC(r32 t)
{
	t = ClampR32(t, 0.f, 1.f);
	return (-4.f * t * t + 4 * t);
}


//RNG
#define ROTL(d,lrot) ((d<<(lrot)) | (d>>(8*sizeof(d)-(lrot))))

static i32 ZRNG_SEED = 1;
void SEED_ZRNG()
{
	ZRNG_SEED = time(NULL);
} 
r32 ZRNG()
{
	ZRNG_SEED *= 16807;	
	return (r32)ZRNG_SEED * 4.6566129e-010f;
	/*
	This turns into a handy 0-1 random number if you take off the sign bit:
	(r32)(ZRNG_SEED & 0x7FFFFFFF) * 4.6566129e-010f;
	*/
}

r32 RNG() 
{
	// set to nonzero seed
	static uint64_t xState = 0;
	static uint64_t yState = 0;
	if (xState == 0)
	{
		xState = time(NULL);
		yState = time(NULL)*1524109428429579u;
	}
	uint64_t xp = xState;
	xState = 15241094284759029579u * yState;
	yState = yState - xp;  yState = ROTL(yState,27);
	return (r32)xp/(r32)UINT64_MAX;
}


// r32 RNG() //returns from 0.f to 1.f
// {
// 	//static u32 seed = 14123;
// 	static u32 seed = 0;
// 	if (seed)
// 	{
// 		u32 a = 16807;
// 		u32 m = 2147483647;
// 		seed = (a*seed) % m;
// 		return (r32)seed / (r32)m;
// 	}
// 	else
// 	{
// 		seed = time(NULL);
// 		u32 a = 16807;
// 		u32 m = 2147483647;
// 		seed = (a*seed) % m;
// 		return (r32)seed / (r32)m;
// 	}
// }

r32 RNEG() //returns from -1.f to 1.f
{
	return (RNG() - 0.5f) * 2.f;
}

//returns -1.f or 1.f
r32 COINTOSS()
{
	r32 r = RNG();
	b8 c = (r > 0.5f);
	return ((1.f * c) + (-1.f * !c));
}

u32 RUINTG(u32 min, u32 max)
{
	return ClampU32(LerpU32(min, max+1, RNG()), min, max);
}






//VECTOR
i2 make_i2( i32 x,  i32 y)
{
	i2 v;
	v.x = x;
	v.y = y;
	return v;
};

r2 make_r2( r32 x,  r32 y)
{
	r2 w;
	w.x = x;
	w.y = y;
	return w;
}

r2_t make_motion(r2 acc, r2 vel, r2 pos)
{
	r2_t motion = {acc, vel, pos};
	return motion;
}

i2 r2_to_i2( r2 w) {
	return make_i2(rtoi(w.x), rtoi(w.y));
}

r2 i2_to_r2( i2 v) {
	return make_r2(itor(v.x), itor(v.y));
}


// ARITHMETIC

i2 add_i2( i2 a,  i2 b)
{
	return make_i2(a.x + b.x, a.y + b.y);
}

r2 add_r2( r2 a,  r2 b)
{
	return make_r2(a.x + b.x, a.y + b.y);
}


i2 i2_add_n( i2 a,  i32 n)
{
	return make_i2(a.x + n, a.y + n);
}


r2 r2_add_n( r2 a,  r32 n)
{
	return make_r2(a.x + n, a.y + n);
}

i2 sub_i2( i2 a,  i2 b)
{
	return make_i2(a.x - b.x, a.y - b.y);
}

i2 i2_sub_n( i2 a,  i32 n)
{
	return make_i2(a.x - n, a.y - n);
}

r2 sub_r2( r2 a,  r2 b)
{
	return make_r2(a.x - b.x, a.y - b.y);
}

r2 r2_sub_n( r2 a,  r32 n)
{
	return make_r2(a.x - n, a.y - n);
}

i2 mul_i2( i2 a,  i2 b)
{
	return make_i2(a.x * b.x, a.y * b.y);
}

r2 mul_r2( r2 a,  r2 b)
{
	return make_r2(a.x*b.x, a.y*b.y);
}

i2 i2_mul_n( i2 a,  i32 n)
{
	return make_i2(a.x * n, a.y * n);
}

i2 i2_mul_r2( i2 a,  r2 b)
{
	return make_i2((i32)(a.x*b.x), (i32)(a.y*b.y));
}

r2 r2_mul_n( r2 a,  i32 n)
{
	return make_r2(a.x*n, a.y*n);
}

r2 r2_mul_x( r2 a,  r32 x)
{
	return make_r2(a.x*x, a.y*x);
}

i2 div_i2( i2 a,  i2 b)
{
	return make_i2(a.x / b.x, a.y / b.y);
}

i2 i2_div_n( i2 a,  i32 n)
{
	return make_i2(a.x / n, a.y / n);
}

r2 div_r2( r2 a,  r2 b)
{
	return make_r2(a.x / b.x, a.y / b.y);
}

r2 r2_div_n( r2 a,  i32 n)
{
	return make_r2(a.x / n, a.y / n);
}

r2 r2_div_x( r2 a,  r32 x)
{
	return make_r2(a.x / x, a.y / x);
}

i2 abs_i2( i2 v)
{
	return make_i2(abs(v.x), abs(v.y));
}

r2 abs_r2( r2 w)
{
	r32 a = (w.x < 0) ? w.x * -1.f : w.x;
	r32 b = (w.y < 0) ? w.y * -1.f : w.y;
	return make_r2(a, b);
}

i2 clamp_i2( i2 v,  i32 min,  i32 max)
{
	return make_i2(ClampI32(v.x, min, max), ClampI32(v.y, min, max));
}

r2 clamp_r2( r2 w,  r32 min,  r32 max)
{
	return make_r2(ClampR32(w.x, min, max), ClampR32(w.y, min, max));
}

i2 i2_clamp_i2(i2 v, i2 min, i2 max)
{
	return make_i2(ClampI32(v.x, min.x, max.x), ClampI32(v.y, min.y, max.y));
}

r2 r2_flatten( r2 a)
{
	return make_r2((r32)trunc(a.x), (r32)trunc(a.y));
}

r2 norm_i2( i2 a)
{
	if (a.x == 0 && a.x == 0)
		return ZERO_R2;
	else
		return norm_r2(i2_to_r2(a));
}

r2 norm_r2( r2 a)
{
	if (a.x == 0.f && a.y == 0.f)
		return ZERO_R2;
	else
	{
		r32 len = len_r2(a);
		if (len != 0.f)
			return r2_div_x(a, len_r2(a));
		else
			return ZERO_R2;
	}
}

i32 len2_i2( i2 a)
{
	return a.x * a.x + a.y * a.y;
}

i32 len_i2( i2 a)
{
	return (i32)(sqrtl(len2_i2(a)));
}

r32 len2_r2( r2 a)
{
	return a.x * a.x + a.y * a.y;
}

r32 len_r2( r2 a)
{
	return sqrtf(AbsR32(len2_r2(a)));
}

r32 dot_i2( i2 a,  i2 b)
{
	r32 result = 0;
	r2 a_n = norm_i2(a);
	r2 b_n = norm_i2(b);
	result = a_n.x * b_n.x + a_n.y * b_n.y;
	return result;
}

r32 r2_dot( r2 a,  r2 b)
{
	r32 result = 0;
	r2 a_n = norm_r2(a);
	r2 b_n = norm_r2(b);
	result = a_n.x * b_n.x + a_n.y * b_n.y;
	return result;
}

r32 r2_dot_full( r2 a,  r2 b)
{
	return a.x * b.x + a.y * b.y;
}

r32 cross_r2( r2 a,  r2 b)
{
	return a.x*b.y - a.y*b.x;
}

i32 cross_i2( i2 a,  i2 b)
{
	return a.x*b.y - a.y*b.x;
}

i2 i2_rot_90_ccw( i2 a)
{
	return make_i2(a.y, -a.x);
}

r2 r2_rot_90_ccw( r2 a)
{
	return make_r2(a.y, -a.x);
}

r2 r2_rot_90_cw( r2 a)
{
	return make_r2(-a.y, a.x);
}

r2 r2_rot_60_ccw(r2 a)
{
	return make_r2(a.x * COS_PI_THIRDS - a.y * SIN_PI_THIRDS, a.x * SIN_PI_THIRDS + a.y * COS_PI_THIRDS);
}

r2 r2_rot_t(r2 a, r32 t)
{
	return make_r2(a.x * cosf(t) - a.y * sinf(t), a.x * sinf(t) + a.y * cosf(t));
}

r2 r32_to_rot(r32 a)
{
	u8 bitdegree = (u8)(ClampR32(a, -1.f, 1.f) * 0xff);
	return make_r2(RCOS(bitdegree), RSIN(bitdegree));
}

i2 i2_sign( i2 a)
{
	return make_i2(SignI32(a.x), SignI32(a.y));
}

r2 r2_sign( r2 a)
{
	return make_r2(SignR32(a.x), SignR32(a.y));
}

i2 lerp_i2( i2 a,  i2 b,  r32 alpha)
{
	return make_i2(LerpI32(a.x, b.x, alpha), LerpI32(a.y, b.y, alpha));
}

r2 lerp_r2( r2 a,  r2 b,  r32 alpha)
{
	return make_r2(LerpR32(a.x, b.x, alpha), LerpR32(a.y, b.y, alpha));
}

b8 i2_isempty( i2 a)
{
	if (a.x == 0 && a.x == 0)
		return 1;
	else
		return 0;
}

b8 i2_equals( i2 a,  i2 b)
{
	if (a.x == b.x && a.y == b.y)
		return 1;
	else
		return 0;
}

b8 r2_equals( r2 a,  r2 b)
{
	if (a.x == b.x && a.y == b.y)
		return 1;
	else
		return 0;
}

b8 r2_lines_intersect( r2 line_a_beg,  r2 line_a_end,  r2 line_b_beg,  r2 line_b_end, r2* intersecting_point, r2* normal)
{
	r2 r = sub_r2(line_a_end, line_a_beg);
	r2 s = sub_r2(line_b_end, line_b_beg);
	r2 c_a = sub_r2(line_b_beg, line_a_beg);
	r32 cross_rs = cross_r2(r, s);
	if (cross_rs == 0.f)
	{
		*intersecting_point = ZERO_R2;
		return 0; //no length or parallell
	}
	r32 t = cross_r2(c_a, s) / cross_rs;
	r32 u = cross_r2(c_a, r) / cross_rs;
	if (t >= 0.f && t <= 1.f && u >= 0.f && u <= 1.f)
	{
		*intersecting_point = add_r2(line_a_beg, r2_mul_x(r, t));
		r2 normalized_target = norm_r2(s);
		r2 rot_target = r2_rot_90_ccw(normalized_target);
		r2 dot_check = norm_r2(sub_r2(line_a_beg, *intersecting_point));
		if (r2_dot(rot_target, dot_check) < 0.f)
			*normal = r2_mul_n(rot_target, -1);
		else
			*normal = rot_target;
		return 1;
	}
	else
	{
		*intersecting_point = ZERO_R2;
		return 0; //no intersection
	}

}

b8 r2_line_intersect_box( r2 line_beg,  r2 line_end,  r2_box box, r2* hit_point, r2* hit_normal)
{
	r2 corners[4];
	corners[0] = box.pos;
	corners[1] = make_r2(box.pos.x + box.siz.x, box.pos.y);
	corners[2] = make_r2(box.pos.x + box.siz.x, box.pos.y + box.siz.y);
	corners[3] = make_r2(box.pos.x,				box.pos.y + box.siz.y);
	r2 candidate_point;
	r2 candidate_point_final = line_end;
	r2 candidate_normal;
	r2 candidate_normal_final = ZERO_R2;
	b8 result = 0;
	r2 l_0;
	r2 l_1;

	for (int i = 0; i < 4; i++)
	{
		l_0 = corners[i];
		l_1 = corners[(i + 1) % 4];
		if (r2_lines_intersect(line_beg, line_end, l_0, l_1, &candidate_point, &candidate_normal))
		{
			if (len2_r2(sub_r2(candidate_point, line_beg)) < len2_r2(sub_r2(candidate_point_final, line_beg)))
			{
				candidate_point_final = candidate_point;
				candidate_normal_final = r2_rot_90_ccw(norm_r2(sub_r2(l_1, l_0)));
				//candidate_normal_final = candidate_normal;
			}
			result = 1;
		}
	}
	if (result == 1)
	{
		*hit_point = candidate_point_final;
		*hit_normal = candidate_normal_final;
		return result;
	}
	else
	{
		return result;
	}
}



r32 new_velocity_from_added_energy( r32 old_vel,  r32 added_energy)
{
	r32 old_energy = 0.5f * old_vel * old_vel;
	r32 new_energy = old_energy + added_energy ;
	r32 new_velocity_magnitude = sqrtf(AbsR32(new_energy) * 2 ) * SignR32(new_energy);
	return new_velocity_magnitude;
}


r2 PixToPos( i2 pix)
{
	return i2_to_r2(pix);
}

i2 PosToPix( r2 pos)
{
	return r2_to_i2(pos);
}



char* PRINT_I2(const i2 a)
{
	char* msg;
	int n = sizeof(i32) * 16 + 3 + 1;
	msg = malloc(n);
	int cx;
	cx = snprintf(msg, n, "(%d,%d)", a.x, a.y);
	if (cx >= 0 && cx < n)
	{
		return msg;
	}
	else
	{
		msg = "errorstring\0";
		return msg;
	}
}

char* PRINT_R2(const r2 a)
{
	char* msg;
	int n = sizeof(i32) * 16 + 3 + 1;
	msg = malloc(n);
	int cx;
	cx = snprintf(msg, n, "(%f,%f)", a.x, a.y);
	if (cx >= 0 && cx < n)
	{
		return msg;
	}
	else
	{
		msg = "errorstring\0";
		return msg;
	}
}

/*~~~~~~~~~~~~~~~~~~~~ VECTOR END ~~~~~~~~~~~~~~~~~~~~*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv VECTOR 3D BEGIN vvvvvvvvvvvvvvvvvvvvvvvvvv*/

vec MakeVec(r32 x, r32 y, r32 z)
{
	vec new_vec;
	new_vec.x = x;
	new_vec.y = y;
	new_vec.z = z;
	return new_vec;
}

mesh* CreateMesh(tri* tris, i32 num)
{
	mesh* new_mesh 	= malloc(sizeof(mesh));
	new_mesh->tris	= tris;
	new_mesh->num 	= num;

	return new_mesh;
}


void MultiplyMatrixVector(vec* v_in, vec* v_out, mat4x4* m)
{
	v_out->x = v_in->x * m->m[0][0] + v_in->y * m->m[1][0] + v_in->z * m->m[2][0] + m->m[3][0];
	v_out->y = v_in->x * m->m[0][1] + v_in->y * m->m[1][1] + v_in->z * m->m[2][1] + m->m[3][1];
	v_out->z = v_in->x * m->m[0][2] + v_in->y * m->m[1][2] + v_in->z * m->m[2][2] + m->m[3][2];
	r32 w 	 = v_in->x * m->m[0][3] + v_in->y * m->m[1][3] + v_in->z * m->m[2][3] + m->m[3][3];

	if (w != 0.f)
	{
		v_out->x = v_out->x / w;
		v_out->y = v_out->y / w;
		v_out->z = v_out->z / w;
	}
}




/*^^^^^^^^^^^^^^^^^^^^^^^^^^ VECTOR 3D END ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*~~~~~~~~~~~~~~~~~~~~ HELPERS BEGIN ~~~~~~~~~~~~~~~~~~~~*/



i32 rtoi(r32 x)
{
	return (i32)(x);
}

i32 rtoi_rnd(r32 x)
{
	return (i32)(x + 0.5f);
}

u32 rtou(r32 x)
{
	return (u32)(x);
}

u32 rtou_rnd(r32 x)
{
	return (u32)(x + 0.5f);
}

i32 utoi(u32 x)
{
	return (i32)x;
}

r32 utor(u32 x)
{
	return (r32)x;
}

u32 itou(i32 x)
{
	return (u32)abs(x);
}

r32 itor(i32 x)
{
	return (r32)x;
}

u32 ClampU32( u32 a,  u32 min,  u32 max)
{
	if (a <= min)
		return min;
	else if (a >= max)
		return max;
	else
		return a;
}

i32 ClampI32( i32 a,  i32 min,  i32 max)
{
	if (a <= min)
		return min;
	else if (a >= max)
		return max;
	else
		return a;
}

r32 ClampR32( r32 a,  r32 min,  r32 max)
{
	if (a < min)
		return min;
	else if (a > max)
		return max;
	else
		return a;
}

u32 LerpU32(i32 a, i32 b, r32 alpha)
{
	alpha = ClampR32(alpha, 0.f, 1.f);
	return rtou(a*(1.f - alpha) + b * alpha);
}

i32 LerpI32(i32 a, i32 b, r32 alpha)
{
	alpha = ClampR32(alpha, 0.f, 1.f);
	return rtoi(a*(1.f - alpha) + b * alpha);
}

r32 LerpR32(r32 a, r32 b, r32 alpha)
{
	alpha = ClampR32(alpha, 0.f, 1.f);
	return (a*(1.f-alpha) + b*alpha);
}

u8 LerpU8(u8 a, u8 b, r32 alpha)
{
	alpha = ClampR32(alpha, 0.f, 1.f);
	if (a > b)
	{
		if (((0xff - a) + b) < (a - b))
		{
			//lerp upwards through overflow
			return (u8)( (b * alpha) - ((0xff - a) * (1.f - alpha)) );
		}
		else
		{
			return (u8)(a * (1.f - alpha)) + (b * alpha);
		}
	}
	else if (a < b)
	{
		if (((0xff - b) + a) < (b - a))
		{
			// lerp downwards through overflow
			return (u8)( (b * alpha) - ((0xff) * (1.f - alpha) + (a * (1.f - alpha))) );
		}
		else
		{
			return (u8)(a * (1.f - alpha)) + (b * alpha);
		}
	}
	else
	{
		return a;
	}
}

u32 MinU32( u32 a,  u32 b)
{
	if (a <= b)
		return a;
	else
		return b;
}

i32 MinI32( i32 a,  i32 b)
{
	if (a <= b)
		return a;
	else
		return b;
}

r32 MinR32( r32 a,  r32 b)
{
	if (a <= b)
		return a;
	else
		return b;
}

u32 MaxU32( u32 a,  u32 b)
{
	if (a >= b)
		return a;
	else
		return b;
}

i32 MaxI32( i32 a,  i32 b)
{
	if (a >= b)
		return a;
	else
		return b;
}

r32 MaxR32( r32 a,  r32 b)
{
	b8 a_max = (a >= b);
	return a * a_max + b * !a_max;
}



i32 SignI32( i32 a)
{
	if (a > 0)
		return 1;
	else if (a < 0)
		return -1;
	else
		return 0;
}

r32 SignR32( r32 a)
{
	if (a > 0)
		return 1.f;
	else if (a < 0)
		return -1.f;
	else
		return 0.f;
}


i32 AbsI32( i32 a)
{
	return a * ((a > 0) - (a < 0));
}

r32 AbsR32( r32 a)
{
	return a * ((a > 0) - (a < 0));
}


r32 Wrap(r32 a)
{
	r32 remainder = fmodf(a, 2.f);
	
	r32 integral;
	r32 frac = AbsR32(modff(remainder, &integral));
	if (a < -1.f)
		return 1.f - frac;
	else if (a > 1.f)
		return -1.f + frac;
	else
		return a;
	//return remainder + -1.f;
}


/*
transform real value from range [-1.f, 1.f] to [0.f, 1.f]
*/
r32 BiasR32(r32 a)
{
	return ((a + 1.f) * 0.5f);
}
/*~~~~~~~~~~~~~~~~~~~~ HELPERS END ~~~~~~~~~~~~~~~~~~~~*/



/*vvvvvvvvvvvvvvvvvvvvvvvvvv	BIT IMPLEMENTATION BEGIN	vvvvvvvvvvvvvvvvvvvvvvvvvv*/
u8 GET2IN8( u8 target,  int pos)
{
	return (u8)((target & B2(pos)) >> pos * 2);
}

u8 GET4IN8( u8 target,  int pos)
{
	return (u8)((target & B4(pos)) >> pos * 4);
}

u8 GET8IN16( u16 target,  int pos)
{
	return (u8)((target & B8(pos)) >> pos * 8);
}

u16 GET16IN32( u32 target,  int pos)
{
	return (u16)((target & B16(pos)) >> pos * 16);
}


u8 GET8IN32( u32 target,  int pos)
{
	return (u8)((target & B8(pos)) >> pos * 8);
}

u8 GET8IN64(u64 target, int pos)
{
	return (u8)((target & B8_64(pos)) >> pos * 8);
}

void SET8IN16( u8 source, u16* destination,  int pos)
{
	*destination &= ~B8(pos); //clear byte before insertion
	*destination |= (u16)(source << pos * 8);
}

void SET8IN32( u8 source, u32* destination,  int pos)
{
	*destination &= ~B8(pos); //clear byte before insertion
	*destination |= (u32)(source << pos * 8);
}

void SET8IN64( u8 source, u64* destination,  int pos)
{
	*destination &= ~B8_64(pos); //clear byte before insertion
	*destination |= (u64)((u64)source << (pos * 8));
}

void SET4IN8( u8 source, u8* destination,  int pos)
{
	*destination &= ~B4(pos); //clear byte before insertion
	*destination |= (u8)(source << pos * 4);
}

void SET2IN8( u8 source, u8* destination, i32 pos)
{
	*destination &= ~B2(pos); //clear byte before insertion
	*destination |= (u8)(source << pos * 4);
}

u8 MAKE4FROM2( u8 left,  u8 right)
{
	return (u8)(right | (left << 2));
}

u8 MAKE8FROM4( u8 left,  u8 right)
{
	return (u8)(right | (left << 4));
}

u16 MAKE16FROM8( u8 left,  u8 right)
{
	return (u16)(right | (left << 8));
}

u32 MAKE32FROM16( u16 left,  u16 right)
{
	return (u32)(right | (left << 16));
}

void SWAP4IN8(u8* target)
{
	//*target = ((*target & B4(1)) >> 4) | ((*target & B4(0)) << 4);
	*target = ((*target & B4(1)) >> 4) | (*target << 4);
}

void SWAP16(u16* a, u16* b)
{
	u16 c = *a;
	*a = *b;
	*b = c;
}

b8 QUERY8( u8 target,  int pos)
{
	return (b8)(target & B1(pos));
}

b8 QUERY32( u32 target,  int pos)
{
	return (b8)(target & B1(pos));
}

b8 QUERY64( u64 target,  int pos)
{
	return (b8)(target & (u64)B1(pos));
}

//REMEMBER TO free(msg) after printing!!!
char* PRNT8( u8 source)
{
	char* msg;
	msg = malloc(sizeof(char) * 9);

	for (int i = 0; i < 8; i++)
	{
			if (source & B1(i))
				msg[7 - i] = '1';
			else
				msg[7 - i] = '0';
	}
	msg[8] = '\0';

	return msg;
}

char* PRNT16( u16 source)
{
	char* msg;
	msg = malloc(sizeof(char) * 17);
	for (int i = 0; i < 16; i++)
	{
		if (source & B1(i))
			msg[15 - i] = '1';
		else
			msg[15 - i] = '0';
	}
	msg[16] = '\0';
	return msg;
}

char* PRNT32( u32 source)
{
	char* msg;
	msg = malloc(sizeof(char) * 33);
	for (int i = 0; i < 32; i++)
	{
		if (source & B1(i))
			msg[31 - i] = '1';
		else
			msg[31 - i] = '0';
	}
	msg[32] = '\0';
	return msg;
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^	BIT IMPLEMENTATION END	^^^^^^^^^^^^^^^^^^^^^^^^^^*/
void SERIALIZEi32(i32 src, u8* dst, i32 index)
{
	for (i32 i = 0; i < 4; i++)
	{
		dst[index + i] 	 = (src >> (24 - i*8)) & 0xFF;
	}
}

void DESERIALIZEi32(u8* src, i32 index, i32* dst)
{
	for (i32 i = 0; i < 4; i++)
	{
		*dst |= (src[index+i] << (24 - i * 8));
	}
}

void SERIALIZEu32(u32 src, u8* dst, u32 index)
{
	for (u32 i = 0; i < 4; i++)
	{
		dst[index + i] 	 = (src >> (24 - i*8)) & 0xFF;
	}
}

void DESERIALIZEu32(u8* src, u32 index, u32* dst)
{
	for (u32 i = 0; i < 4; i++)
	{
		*dst |= (src[index+i] << (24 - i * 8));
	}
}


void SERIALIZEu64(u64 src, u8* dst, i32 index)
{
	for (i32 i = 0; i < 8; i++)
	{
		dst[index + i] 	 = (src >> ((8*7) - (i*8))) & 0xFF;
	}
}

void DESERIALIZEu64(u8* src, i32 index, u64* dst)
{
	for (i32 i = 0; i < 8; i++)
	{
		*dst |= (src[index+i] << ((8*7) - i * 8));
	}
}

void SERIALIZEi2(i2 src, u8* dst, i32 index)
{
	SERIALIZEi32(src.x, dst, index);
	SERIALIZEi32(src.y, dst, index+sizeof(i32));
}

void DESERIALIZEi2(u8* src, i32 index, i2* dst)
{
	DESERIALIZEi32(src, index, &dst->x);
	DESERIALIZEi32(src, index+sizeof(i32), &dst->y);
}
