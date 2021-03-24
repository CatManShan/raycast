#include <math.h>

#include "../mem-utils/mem-macros.h"

#include "fixed-point.h"

#define TWO_TO_32 (1LL << 32)

double fixed_point64_to_double(struct FixedPoint64 value)
{
	return (double) value.as_int / TWO_TO_32;
}

struct FixedPoint64 fixed_point64_from_double(double value)
{
	return (struct FixedPoint64) { .as_int = (int64_t) (value * TWO_TO_32) };
}

double ufixed_point64_to_double(struct UFixedPoint64 value)
{
	return (double) value.as_uint / TWO_TO_32;
}

struct UFixedPoint64 ufixed_point64_from_double(double value)
{
	return (struct UFixedPoint64) { .as_uint = (uint64_t) (value * TWO_TO_32) };
}

struct FixedPoint64 fixed_point64_add(struct FixedPoint64 a, struct FixedPoint64 b)
{
	return (struct FixedPoint64) { .as_int = a.as_int + b.as_int };
}

struct FixedPoint64 fixed_point64_subtract(struct FixedPoint64 a, struct FixedPoint64 b)
{
	return (struct FixedPoint64) { .as_int = a.as_int - b.as_int };
}

// struct FixedPoint64 fixed_point64_multiply(struct FixedPoint64 a, struct FixedPoint64 b)
// {
// }

// struct FixedPoint64 fixed_point64_divide(struct FixedPoint64 a, struct FixedPoint64 b)
// {
	// return (struct FixedPoint64) { .as_int = a.as_int / b.as_int * TWO_TO_32 };
// }

struct UFixedPoint64 ufixed_point64_add(struct UFixedPoint64 a, struct UFixedPoint64 b)
{
	return (struct UFixedPoint64) { .as_uint = a.as_uint + b.as_uint };
}

struct UFixedPoint64 ufixed_point64_subtract(struct UFixedPoint64 a, struct UFixedPoint64 b)
{
	return (struct UFixedPoint64) { .as_uint = a.as_uint - b.as_uint };
}

// struct UFixedPoint64 ufixed_point64_multiply(struct UFixedPoint64 a, struct UFixedPoint64 b)
// {
// }
// 
// struct UFixedPoint64 ufixed_point64_divide(struct UFixedPoint64 a, struct UFixedPoint64 b)
// {
// }


