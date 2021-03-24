#ifndef fixed_point_h
#define fixed_point_h

#include <stdint.h>

struct FixedPoint64 {
	int32_t integer;
	uint32_t fraction;
};

struct UnsignedFixedPoint64 {
	uint32_t integer;
	uint32_t fraction;
};

double fixed_point64_to_double(struct FixedPoint64 value);
struct FixedPoint64 fixed_point64_from_double(double value);
double unsigned_fixed_point64_to_double(struct UnsignedFixedPoint64 value);
struct UnsignedFixedPoint64 unsigned_fixed_point64_from_double(double value);

struct FixedPoint64 fixed_point64_add(struct FixedPoint64 a, struct FixedPoint64 b);
struct FixedPoint64 fixed_point64_subtract(struct FixedPoint64 a, struct FixedPoint64 b);
struct FixedPoint64 fixed_point64_multiply(struct FixedPoint64 a, struct FixedPoint64 b);
struct FixedPoint64 fixed_point64_divide(struct FixedPoint64 a, struct FixedPoint64 b);

struct UnsignedFixedPoint64 unsigned_fixed_point64_add(struct UnsignedFixedPoint64 a, struct UnsignedFixedPoint64 b);
struct UnsignedFixedPoint64 unsigned_fixed_point64_subtract(struct UnsignedFixedPoint64 a, struct UnsignedFixedPoint64 b);
struct UnsignedFixedPoint64 unsigned_fixed_point64_multiply(struct UnsignedFixedPoint64 a, struct UnsignedFixedPoint64 b);
struct UnsignedFixedPoint64 unsigned_fixed_point64_divide(struct UnsignedFixedPoint64 a, struct UnsignedFixedPoint64 b);

#endif // fixed_point_h

