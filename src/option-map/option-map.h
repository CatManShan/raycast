#ifndef option_map_h
#define option_map_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

enum OptionMapErrorCode {
	OM_NO_ERROR = 0,
	OM_MISSING_ARG,
	OM_UNKNOWN_OPTION,
	OM_ERROR_COUNT
};

struct OptionMapOption {
	char **aliases;
	bool takes_value; // TODO: refactor -> takes_arg (??)

	bool given;
	char *value;
};

struct OptionMapError {
	enum OptionMapErrorCode error_code;
	const char *related_option_alias;
};

struct OptionMap {
	size_t option_count;
	struct OptionMapOption options[];
};

struct OptionMap *option_map_create(struct OptionMapOption *option_arr, size_t option_count);
void option_map_destroy(struct OptionMap *option_map);

struct OptionMapError option_map_set_options(struct OptionMap *option_map, int options_list_length,
		char **options_list);

bool option_map_is_option_given(struct OptionMap *option_map, const char *option_name);
char *option_map_get_option_value(struct OptionMap *option_map, const char *option_name);

struct OptionMapError option_map_get_last_error();
void option_map_print_error_message(FILE *stream, const char *prefix, struct OptionMapError error);
void option_map_print_last_error_message(FILE *stream, const char *prefix);
void option_map_reset_error();

#endif // option_map_h

