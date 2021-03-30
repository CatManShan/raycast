#ifndef fixed_h
#define fixed_h

#include <stdint.h>

/*** 32-bit ***/

struct Fixed32 {
	int32_t as_int;
};

struct UFixed32 {
	uint32_t as_uint;
};

double fixed32_to_double(struct Fixed32 value);
struct Fixed32 fixed32_from_double(double value);
double ufixed32_to_double(struct UFixed32 value);
struct UFixed32 ufixed32_from_double(double value);

struct Fixed32 fixed32_add(struct Fixed32 a, struct Fixed32 b);
struct Fixed32 fixed32_subtract(struct Fixed32 a, struct Fixed32 b);
struct Fixed32 fixed32_multiply(struct Fixed32 a, struct Fixed32 b);
struct Fixed32 fixed32_divide(struct Fixed32 a, struct Fixed32 b);
struct UFixed32 ufixed32_add(struct UFixed32 a, struct UFixed32 b);
struct UFixed32 ufixed32_subtract(struct UFixed32 a, struct UFixed32 b);
struct UFixed32 ufixed32_multiply(struct UFixed32 a, struct UFixed32 b);
struct UFixed32 ufixed32_divide(struct UFixed32 a, struct UFixed32 b);

/*** 64-bit ***/

struct Fixed64 {
	int64_t as_int;
};

struct UFixed64 {
	uint64_t as_uint;
};

double fixed64_to_double(struct Fixed64 value);
struct Fixed64 fixed64_from_double(double value);
double ufixed64_to_double(struct UFixed64 value);
struct UFixed64 ufixed64_from_double(double value);

struct Fixed64 fixed64_add(struct Fixed64 a, struct Fixed64 b);
struct Fixed64 fixed64_subtract(struct Fixed64 a, struct Fixed64 b);
struct Fixed64 fixed64_multiply(struct Fixed64 a, struct Fixed64 b);
struct Fixed64 fixed64_divide(struct Fixed64 a, struct Fixed64 b);
struct UFixed64 ufixed64_add(struct UFixed64 a, struct UFixed64 b);
struct UFixed64 ufixed64_subtract(struct UFixed64 a, struct UFixed64 b);
struct UFixed64 ufixed64_multiply(struct UFixed64 a, struct UFixed64 b);
struct UFixed64 ufixed64_divide(struct UFixed64 a, struct UFixed64 b);

#endif // fixed_h

