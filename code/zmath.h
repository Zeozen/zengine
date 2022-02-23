#ifndef ZMATH_H
#define ZMATH_H

#include <math.h> //sin, cos doubles, @TODO: maybe use lookup tables instead?
#include "aliases.h"

/*vvvvvvvvvvvvvvvvvvvv CONSTANTS BEGIN vvvvvvvvvvvvvvvvvvvv*/
#define ZEROVEC {0,0}
#define ZERO_I2 (i2){0, 0}
#define ZERO_R2 (r2){0.f, 0.f}
#define DIR_N	(i2){0 , -1}
#define DIR_S	(i2){0 , 1 }
#define DIR_W	(i2){-1, 0 }
#define DIR_E	(i2){1 , 0 }
#define DIR_NE	(i2){1 , -1}
#define DIR_NW	(i2){-1, -1}
#define DIR_SE	(i2){1 , 1 }
#define DIR_SW	(i2){-1, 1 }
#define DIR_R_N	(r2){0.f , -1.f}
#define DIR_R_S	(r2){0.f , 1.f }
#define DIR_R_W	(r2){-1.f, 0.f }
#define DIR_R_E	(r2){1.f , 0.f }

#define ZPI 3.14159f
#define ZPI_HALF 1.570796f
#define ZPI_THIRDS 1.0471975f
#define COS_PI_THIRDS 0.5f
#define SIN_PI_THIRDS 0.866f
#define COS_PI_SIXTHS 0.866f
#define SIN_PI_SIXTHS 0.5f

#define WORLD_UNIT 16
#define WORLD_HALFUNIT 8
#define WORLD_UNIT_F 16.f
#define WORLD_HALFUNIT_F 8.f




/*^^^^^^^^^^^^^^^^^^^^ CONSTANTSANTS END ^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvv STATICS BEGIN vvvvvvvvvvvvvvvvvvvv*/
u8 NSIN(u8 a);
u8 NCOS(u8 a);
r32 RSIN(u8 a);
r32 RCOS(u8 a);
r32 CURVE(r32 t, r32 k);
r32 SMOOTH(r32 t, r32 k);
r32 PARAMETRIC(r32 t);

r32 RNG();
r32 RNEG();
r32 COINTOSS();
void SEED_ZRNG();
r32 ZRNG();
u32 RUINTG(u32 min, u32 max);
/*^^^^^^^^^^^^^^^^^^^^ STATICS END ^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvv HELPERS BEGIN vvvvvvvvvvvvvvvvvvvv*/

i32 rtoi(r32 x);
i32 rtoi_rnd(r32 x);
i32 utoi(u32 x);
u32 rtou(r32 x);
u32 rtou_rnd(r32 x);
u32 itou(i32 x);
r32 utor(u32 x);
r32 itor(i32 x);


u32	ClampU32(u32 a,  u32 min,  u32 max);
i32	ClampI32(i32 a,  i32 min,  i32 max);
r32 ClampR32(r32 a,  r32 min,  r32 max);

u32	LerpU32(i32 a, i32 b, r32 alpha);
i32	LerpI32(i32 a, i32 b, r32 alpha);
r32 LerpR32(r32 a, r32 b, r32 alpha);
u8 LerpU8(u8 a, u8 b, r32 alpha);

u32	MinU32(u32 a,  u32 b);
i32	MinI32(i32 a,  i32 b);
r32 MinR32(r32 a,  r32 b);

u32 MaxU32(u32 a,  u32 b);
i32 MaxI32(i32 a,  i32 b);
r32 MaxR32(r32 a,  r32 b);

i32	SignI32(i32 a);
r32 SignR32(r32 a);

i32 AbsI32(i32 a);
r32 AbsR32(r32 a);

r32 BiasR32(r32 a);


r32 Wrap(r32 a);
/*^^^^^^^^^^^^^^^^^^^^ HELPERS END ^^^^^^^^^^^^^^^^^^^^*/



/*vvvvvvvvvvvvvvvvvvvv VECTOR BEGIN vvvvvvvvvvvvvvvvvvvv*/
typedef struct Signed_Vector_2
{
	i32 x;
	i32 y;
}i2;

typedef struct Real_Vector_2
{
	r32 x;
	r32 y;
}r2;

typedef struct Pixel_Aligned_Line
{
	i2 beg;
	i2 end;
} i2_line;

typedef struct Real_Line
{
	r2 beg;
	r2 end;
} r2_line;

typedef struct Pixel_Aligned_Box
{
	i2 pos; //top left corner
	i2 siz;
} i2_box;

typedef struct Real_Box
{
	r2 pos; //top left corner
	r2 siz;
} r2_box;

typedef struct Real_2x2_Matrix
{
	r2 row_0;
	r2 row_1;
}m2;

typedef struct Transform
{
	r2 acc;
	r2 vel;
	r2 pos;
} r2_t;

i2 make_i2( i32 x,  i32 y);
r2 make_r2( r32 x,  r32 y);
r2_t make_motion(r2 acc, r2 vel, r2 pos);

i2 r2_to_i2( r2 w);
r2 i2_to_r2( i2 v);


i2 add_i2( i2 a,  i2 b);
r2 add_r2( r2 a,  r2 b);
i2 i2_add_n( i2 a,  i32 n);
r2 r2_add_n( r2 a,  r32 n);


i2 sub_i2( i2 a,  i2 b);
i2 i2_sub_n( i2 a,  i32 n);
r2 sub_r2( r2 a,  r2 b);
r2 r2_sub_n( r2 a,  r32 n);


i2 mul_i2( i2 a,  i2 b);
r2 mul_r2( r2 a,  r2 b);
i2 i2_mul_n( i2 a,  i32 n);
i2 i2_mul_r2( i2 a,  r2 b);
r2 r2_mul_n( r2 a,  i32 n);
r2 r2_mul_x( r2 a,  r32 x);

i2 div_i2( i2 a,  i2 b);
i2 i2_div_n( i2 a,  i32 n);
r2 div_r2( r2 a,  r2 b);
r2 r2_div_n( r2 a,  i32 n);
r2 r2_div_x( r2 a,  r32 x);

i2 abs_i2( i2 v);
r2 abs_r2( r2 w);

i2 clamp_i2( i2 v,  i32 min,  i32 max);
r2 clamp_r2( r2 w,  r32 min,  r32 max);
i2 i2_clamp_i2(i2 v, i2 min, i2 max);

r2 r2_flatten( r2 a);

r2 norm_i2( i2 a);
r2 norm_r2( r2 a);

i32 len2_i2( i2 a);
i32 len_i2( i2 a);

r32 len2_r2( r2 a);
r32 len_r2( r2 a);

r32 dot_i2( i2 a,  i2 b);
r32 r2_dot( r2 a,  r2 b);
r32 r2_dot_full( r2 a,  r2 b);

r32 cross_r2( r2 a,  r2 b);
i32 cross_i2( i2 a,  i2 b);

i2 i2_rot_90_ccw( i2 a);
r2 r2_rot_90_ccw( r2 a);
r2 r2_rot_90_cw( r2 a);

r2 r2_rot_60_ccw(r2 a);

r2 r2_rot_t(r2 a, r32 t);

r2 r32_to_rot(r32 a);

i2 i2_sign( i2 a);
r2 r2_sign( r2 a);

i2 lerp_i2( i2 a,  i2 b,  r32 alpha);
r2 lerp_r2( r2 a,  r2 b,  r32 alpha);

b8 i2_isempty( i2 a);
b8 i2_equals( i2 a,  i2 b);
b8 r2_equals( r2 a,  r2 b);

b8 r2_lines_intersect( r2 line_a_beg,  r2 line_a_end,  r2 line_b_beg,  r2 line_b_end, r2* intersecting_point, r2* normal);
b8 r2_line_intersect_box( r2 line_beg,  r2 line_end,  r2_box box, r2* hit_point, r2* hit_normal);

r32 new_velocity_from_added_energy( r32 old_vel,  r32 added_energy);



r2  PixToPos(i2 pix);
i2  PosToPix(r2 pos);



char* PRINT_I2(const i2 a);
char* PRINT_R2(const r2 a);
/*^^^^^^^^^^^^^^^^^^^^ VECTOR END ^^^^^^^^^^^^^^^^^^^^*/


/*vvvvvvvvvvvvvvvvvvvv BIT BEGIN vvvvvvvvvvvvvvvvvvvv*/
//setting and manipulating individual bits:
// use bitwise OR to set bits (turn on)						: byte |= bit_5; (sets bit 5 to 1, regardless if it's already a 0 or 1)
// use bitwise AND to query bit								: byte & bit_6; (true if set, false if not)
// use bitwise AND + bitwise NOT to clear bit (turn off)	: byte &= ~bit2; (sets bit 2 to 0, regardless what was already there)
// use bitwise XOR to flip bit state, toggle on or off		: byte ^= bit1;	 (sets bit 1 to the opposite of whatever it was)

	//bitmasks defining

//bit extraction single, X = X-1, so 0th bit(first bit) corresponds to an X of 1
#define BIT(X)	(1 << ((X)))
#define B1(X)	((u32)1<<(X))
#define B2(X)	((u32)3<<(X*2))
#define B4(X)	((u32)15<<(X*4))
#define B8(X)	((u32)UINT8_MAX<<(X*8))
#define B8_64(X) ((u64)0xff<<(X*8))
#define B16(X)	((u32)UINT16_MAX<<(X*16))


	//extraction, useful for turning into normal numbers, or otherwise writing subsection of bits on a "standard" form
	//u2 from u8
	u8 GET2IN8( u8 target,  int pos);

	//u4 from u8
	u8 GET4IN8( u8 target,  int pos);

	//u8 from u16
	u8 GET8IN16( u16 target,  int pos);

	//u16 from u32
	u16 GET16IN32( u32 target,  int pos);

	//u8 from u32
	u8 GET8IN32( u32 target,  int pos);

	u8 GET8IN64(u64 target, int pos);
	//insertion
	// the idea is to extract a byte from a larger set, modify the bits you need using bitwise operators, and then insert the byte back into the right position
	// and the same thing just with nibbles

	void SET8IN16( u8 source, u16* destination,  int pos);

	void SET8IN32( u8 source, u32* destination,  int pos);

	void SET8IN64( u8 source, u64* destination,  int pos);

	void SET4IN8( u8 source, u8* destination,  int pos);

	void SET2IN8( u8 source, u8* destination, i32 pos);

	//compose bytes using these methods
	u8 MAKE4FROM2( u8 left,  u8 right);

	u8 MAKE8FROM4( u8 left,  u8 right);

	u16 MAKE16FROM8( u8 left,  u8 right);

	u32 MAKE32FROM16( u16 left,  u16 right);

	//manipulating, modifies supplied data in some way
	void SWAP4IN8(u8* target);
	void SWAP16(u16* a, u16* b);

	//queries
	b8 QUERY8( u8 target,  int pos);
	b8 QUERY32( u32 target,  int pos);
	b8 QUERY64( u64 target,  int pos);

	char* PRNT8 ( u8 source);
	char* PRNT16( u16 source);
	char* PRNT32( u32 source);

	void SERIALIZEi32(i32 src, u8* dst, i32 index);
	void DESERIALIZEi32(u8* src, i32 index, i32* dst);

	void SERIALIZEu32(u32 src, u8* dst, u32 index);
	void DESERIALIZEu32(u8* src, u32 index, u32* dst);

	void SERIALIZEu64(u64 src, u8* dst, i32 index);
	void DESERIALIZEu64(u8* src, i32 index, u64* dst);

	void SERIALIZEi2(i2 src, u8* dst, i32 index);
	void DESERIALIZEi2(u8* src, i32 index, i2* dst);

	
/*^^^^^^^^^^^^^^^^^^^^ BIT END ^^^^^^^^^^^^^^^^^^^^*/



#endif //ZMATH_H
