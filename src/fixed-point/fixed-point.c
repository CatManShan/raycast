#include <math.h>

#include "../mem-utils/mem-macros.h"

#include "fixed-point.h"

#define TWO_TO_32 (1LL << 32)

double fixed_point64_to_double(struct FixedPoint64 value)
{
	int64_t fp_as_int = REINTERPRET(value, int64_t);

	return (double) fp_as_int / TWO_TO_32;
}

struct FixedPoint64 fixed_point64_from_double(double value)
{
	int64_t fp_as_int = (int64_t) (value * TWO_TO_32);

	return REINTERPRET(fp_as_int, struct FixedPoint64);
}

double unsigned_fixed_point64_to_double(struct UnsignedFixedPoint64 value)
{
	uint64_t fp_as_uint = REINTERPRET(value, uint64_t);

	return (double) fp_as_uint / TWO_TO_32;
}

struct UnsignedFixedPoint64 unsigned_fixed_point64_from_double(double value)
{
	uint64_t fp_as_uint = (uint64_t) (value * TWO_TO_32);

	return REINTERPRET(fp_as_uint, struct UnsignedFixedPoint64);
}

struct FixedPoint64 fixed_point64_add(struct FixedPoint64 a, struct FixedPoint64 b)
{
	int64_t a_as_int = REINTERPRET(a, int64_t);
	int64_t b_as_int = REINTERPRET(b, int64_t);
	int64_t sum = a_as_int + b_as_int;

	return REINTERPRET(sum, struct FixedPoint64);
}

struct FixedPoint64 fixed_point64_subtract(struct FixedPoint64 a, struct FixedPoint64 b)
{
	int64_t a_as_int = REINTERPRET(a, int64_t);
	int64_t b_as_int = REINTERPRET(b, int64_t);
	int64_t difference = a_as_int - b_as_int;

	return REINTERPRET(difference, struct FixedPoint64);
}

// struct FixedPoint64 fixed_point64_multiply(struct FixedPoint64 a, struct FixedPoint64 b)
// {
// }
// 
// struct FixedPoint64 fixed_point64_divide(struct FixedPoint64 a, struct FixedPoint64 b)
// {
// }

struct UnsignedFixedPoint64 unsigned_fixed_point64_add(struct UnsignedFixedPoint64 a, struct UnsignedFixedPoint64 b)
{
	uint64_t a_as_int = REINTERPRET(a, uint64_t);
	uint64_t b_as_int = REINTERPRET(b, uint64_t);
	uint64_t sum = a_as_int + b_as_int;

	return REINTERPRET(sum, struct UnsignedFixedPoint64);
}

struct UnsignedFixedPoint64 unsigned_fixed_point64_subtract(struct UnsignedFixedPoint64 a, struct UnsignedFixedPoint64 b)
{
	uint64_t a_as_int = REINTERPRET(a, uint64_t);
	uint64_t b_as_int = REINTERPRET(b, uint64_t);
	uint64_t difference = a_as_int - b_as_int;

	return REINTERPRET(difference, struct UnsignedFixedPoint64);
}

// struct UnsignedFixedPoint64 unsigned_fixed_point64_multiply(struct UnsignedFixedPoint64 a, struct UnsignedFixedPoint64 b)
// {
// }
// 
// struct UnsignedFixedPoint64 unsigned_fixed_point64_divide(struct UnsignedFixedPoint64 a, struct UnsignedFixedPoint64 b)
// {
// }


