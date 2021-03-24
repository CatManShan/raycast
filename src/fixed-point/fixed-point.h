#ifndef fixed_point_h
#define fixed_point_h

#include <stdint.h>

struct FixedPoint64 {
	int32_t integer;
	uint32_t fraction;
};

struct UFixedPoint64 {
	uint32_t integer;
	uint32_t fraction;
};

double fixed_point64_to_double(struct FixedPoint64 value);
struct FixedPoint64 fixed_point64_from_double(double value);
double ufixed_point64_to_double(struct UFixedPoint64 value);
struct UFixedPoint64 ufixed_point64_from_double(double value);

struct FixedPoint64 fixed_point64_add(struct FixedPoint64 a, struct FixedPoint64 b);
struct FixedPoint64 fixed_point64_subtract(struct FixedPoint64 a, struct FixedPoint64 b);
struct FixedPoint64 fixed_point64_multiply(struct FixedPoint64 a, struct FixedPoint64 b);
struct FixedPoint64 fixed_point64_divide(struct FixedPoint64 a, struct FixedPoint64 b);

struct UFixedPoint64 ufixed_point64_add(struct UFixedPoint64 a, struct UFixedPoint64 b);
struct UFixedPoint64 ufixed_point64_subtract(struct UFixedPoint64 a, struct UFixedPoint64 b);
struct UFixedPoint64 ufixed_point64_multiply(struct UFixedPoint64 a, struct UFixedPoint64 b);
struct UFixedPoint64 ufixed_point64_divide(struct UFixedPoint64 a, struct UFixedPoint64 b);

#endif // fixed_point_h

