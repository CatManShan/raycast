#include <math.h>

#include "fixed.h"

#define TWO_TO_32 (1L << 32)
#define TWO_TO_16 (1 << 16)
#define TWO_TO_8 (1 << 8)

/*** 32-bit ***/

double fixed32_to_double(struct Fixed32 value)
{
	return (double) value.as_int / TWO_TO_16;
}

struct Fixed32 fixed32_from_double(double value)
{
	return (struct Fixed32) { .as_int = (int32_t) (value * TWO_TO_16) };
}

double ufixed32_to_double(struct UFixed32 value)
{
	return (double) value.as_uint / TWO_TO_16;
}

struct UFixed32 ufixed32_from_double(double value)
{
	return (struct UFixed32) { .as_uint = (uint32_t) (value * TWO_TO_16) };
}

struct Fixed32 fixed32_add(struct Fixed32 a, struct Fixed32 b)
{
	return (struct Fixed32) { .as_int = a.as_int + b.as_int };
}

struct Fixed32 fixed32_subtract(struct Fixed32 a, struct Fixed32 b)
{
	return (struct Fixed32) { .as_int = a.as_int - b.as_int };
}

struct Fixed32 fixed32_multiply(struct Fixed32 a, struct Fixed32 b)
{
	int64_t product = (int64_t) a.as_int * (int64_t) b.as_int;

	product += (1 << 15); // round
	
	return (struct Fixed32) { .as_int = (int32_t) (product >> 16) };
}

struct Fixed32 fixed32_divide(struct Fixed32 a, struct Fixed32 b)
{
	int64_t a_shifted = ((int64_t) a.as_int) << 17;
	int64_t quotient = a_shifted / b.as_int;
	quotient += 1; // round
	quotient >>= 1;

	return (struct Fixed32) { .as_int = quotient };
}

struct UFixed32 ufixed32_add(struct UFixed32 a, struct UFixed32 b)
{
	return (struct UFixed32) { .as_uint = a.as_uint + b.as_uint };
}

struct UFixed32 ufixed32_subtract(struct UFixed32 a, struct UFixed32 b)
{
	return (struct UFixed32) { .as_uint = a.as_uint - b.as_uint };
}

struct UFixed32 ufixed32_multiply(struct UFixed32 a, struct UFixed32 b)
{
	uint64_t product = (uint64_t) a.as_uint * (uint64_t) b.as_uint;

	product += (1 << 15); // round
	
	return (struct UFixed32) { .as_uint = (uint32_t) (product >> 16) };
}
 
struct UFixed32 ufixed32_divide(struct UFixed32 a, struct UFixed32 b)
{

     uint64_t a_shifted = ((uint64_t) a.as_uint) << 17;
     uint64_t quotient = a_shifted / b.as_uint;
     quotient += 1; // round
     quotient >>= 1;

     return (struct UFixed32) { .as_uint = quotient };
}

/*** 64-bit ***/

double fixed64_to_double(struct Fixed64 value)
{
	return (double) value.as_int / TWO_TO_32;
}

struct Fixed64 fixed64_from_double(double value)
{
	return (struct Fixed64) { .as_int = (int64_t) (value * TWO_TO_32) };
}

double ufixed64_to_double(struct UFixed64 value)
{
	return (double) value.as_uint / TWO_TO_32;
}

struct UFixed64 ufixed64_from_double(double value)
{
	return (struct UFixed64) { .as_uint = (uint64_t) (value * TWO_TO_32) };
}

struct Fixed64 fixed64_add(struct Fixed64 a, struct Fixed64 b)
{
	return (struct Fixed64) { .as_int = a.as_int + b.as_int };
}

struct Fixed64 fixed64_subtract(struct Fixed64 a, struct Fixed64 b)
{
	return (struct Fixed64) { .as_int = a.as_int - b.as_int };
}

struct Fixed64 fixed64_multiply(struct Fixed64 a, struct Fixed64 b)
{
	int64_t a_int = (a.as_int >> 32); // Gets only left 32 bits
	int64_t a_frac = a.as_int & UINT32_MAX; // Gets only right 32 bits
	int64_t b_int = (b.as_int >> 32); // Gets only left 32 bits
	int64_t b_frac = b.as_int & UINT32_MAX; // Gets only right 32 bits

	int64_t int_product = a_int * b_int;
	int64_t mixed_product = a_int * b_frac + a_frac * b_int;
	int64_t frac_product = a_frac * b_frac; // Used only for the carry
	frac_product += (1L << 31); // Rounds the carry

	int64_t ab_product =
		(int_product << 32)
		+ mixed_product
		+ (frac_product >> 32);
	
	return (struct Fixed64) { .as_int = ab_product };
}

struct Fixed64 fixed64_divide(struct Fixed64 a, struct Fixed64 b)
{
	double a_double = fixed64_to_double(a);
	double b_double = fixed64_to_double(b);

	return fixed64_from_double(a_double / b_double);
}

struct UFixed64 ufixed64_add(struct UFixed64 a, struct UFixed64 b)
{
	return (struct UFixed64) { .as_uint = a.as_uint + b.as_uint };
}

struct UFixed64 ufixed64_subtract(struct UFixed64 a, struct UFixed64 b)
{
	return (struct UFixed64) { .as_uint = a.as_uint - b.as_uint };
}

struct UFixed64 ufixed64_multiply(struct UFixed64 a, struct UFixed64 b)
{
	uint64_t a_uint = (a.as_uint >> 32); // Gets only left 32 bits
	uint64_t a_frac = a.as_uint & UINT32_MAX; // Gets only right 32 bits
	uint64_t b_uint = (b.as_uint >> 32); // Gets only left 32 bits
	uint64_t b_frac = b.as_uint & UINT32_MAX; // Gets only right 32 bits

	uint64_t uint_product = a_uint * b_uint;
	uint64_t mixed_product = a_uint * b_frac + a_frac * b_uint;
	uint64_t frac_product = a_frac * b_frac; // Used only for the carry
	frac_product += (1L << 31); // Rounds the carry

	uint64_t ab_product =
		(uint_product << 32)
		+ mixed_product
		+ (frac_product >> 32);

	return (struct UFixed64) { .as_uint = ab_product };
}
 
struct UFixed64 ufixed64_divide(struct UFixed64 a, struct UFixed64 b)
{
	double a_double = ufixed64_to_double(a);
	double b_double = ufixed64_to_double(b);

	return ufixed64_from_double(a_double / b_double);
}


