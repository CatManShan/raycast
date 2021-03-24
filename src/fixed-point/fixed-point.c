#include <math.h>

#include "../mem-utils/mem-macros.h"

#include "fixed-point.h"

#define TWO_TO_32 (1L << 32)

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

struct FixedPoint64 fixed_point64_multiply(struct FixedPoint64 a, struct FixedPoint64 b)
{
	int64_t a_integer = (a.as_int >> 32); // Gets only left 32 bits
	int64_t a_fraction = a.as_int & UINT32_MAX; // Gets only right 32 bits
	int64_t b_integer = (b.as_int >> 32); // Gets only left 32 bits
	int64_t b_fraction = b.as_int & UINT32_MAX; // Gets only right 32 bits

	int64_t integer_product = a_integer * b_integer;
	int64_t mixed_product = a_integer * b_fraction + a_fraction * b_integer;
	int64_t fraction_product = a_fraction * b_fraction; // Used only for the carry
	fraction_product += (1L << 31); // Rounds the carry

	int64_t ab_product =
		(integer_product << 32)
		+ mixed_product
		+ (fraction_product >> 32);
	
	return (struct FixedPoint64) { ab_product };
}

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

struct UFixedPoint64 ufixed_point64_multiply(struct UFixedPoint64 a, struct UFixedPoint64 b)
{
	uint64_t a_uinteger = (a.as_uint >> 32); // Gets only left 32 bits
	uint64_t a_fraction = a.as_uint & UINT32_MAX; // Gets only right 32 bits
	uint64_t b_uinteger = (b.as_uint >> 32); // Gets only left 32 bits
	uint64_t b_fraction = b.as_uint & UINT32_MAX; // Gets only right 32 bits

	uint64_t uinteger_product = a_uinteger * b_uinteger;
	uint64_t mixed_product = a_uinteger * b_fraction + a_fraction * b_uinteger;
	uint64_t fraction_product = a_fraction * b_fraction; // Used only for the carry
	fraction_product += (1L << 31); // Rounds the carry

	uint64_t ab_product =
		(uinteger_product << 32)
		+ mixed_product
		+ (fraction_product >> 32);

	return (struct UFixedPoint64) { ab_product };
}
 
// struct UFixedPoint64 ufixed_point64_divide(struct UFixedPoint64 a, struct UFixedPoint64 b)
// {
// }


