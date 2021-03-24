#include <stdlib.h>
#include <string.h>

#include "../mem-utils/mem-macros.h"

#ifdef MEM_DEBUG
#include "../mem-utils/mem-debug.h"
#endif

#include "option-map.h"

static struct OptionMapError om_last_error = { OM_NO_ERROR, NULL };

static struct OptionMapOption *option_map_get_option_by_alias(struct OptionMap *option_map, const char *option_name);
static struct OptionMapOption *option_map_get_option_by_name(struct OptionMap *option_map, const char *option_name);

static char *str_dup(const char *str);
#ifdef UNUSED
static char *str_n_dup(const char *str, size_t length);
#endif // UNUSED

static size_t str_arr_len(char **str_arr);
static char **str_arr_dup(char **str_arr);
void str_arr_destroy(char **str_arr);

struct OptionMap *option_map_create(struct OptionMapOption *option_arr, size_t option_count)
{
	struct OptionMap *option_map = ALLOC_FLEX_STRUCT(option_map, options, option_count);

	option_map->option_count = option_count;

	for (size_t index = 0; index < option_count; ++index) {
		struct OptionMapOption *option = &option_map->options[index];

		option->aliases = str_arr_dup(option_arr[index].aliases);
		option->takes_value = option_arr[index].takes_value;
		option->given = false;
		option->value = NULL;
	}

	return option_map;
}

void option_map_destroy(struct OptionMap *option_map)
{
	for (size_t index = 0; index < option_map->option_count; ++index) {
		struct OptionMapOption *option = &option_map->options[index];
		str_arr_destroy(option->aliases);

		free(option->value);
	}

	free(option_map);
}

struct OptionMapError option_map_set_options(struct OptionMap *option_map, int options_list_length, char **options_list)
{
	for (int option_num = 0; option_num < options_list_length; ++option_num) {
		char *option_alias = options_list[option_num];

		struct OptionMapOption *option = option_map_get_option_by_alias(option_map, option_alias);

		if (option == NULL) {
			om_last_error = (struct OptionMapError) { OM_UNKNOWN_OPTION, option_alias };
			return om_last_error;
		} else {
			option->given = true;

			if (option->takes_value) {
				bool value_given = (option_num < options_list_length - 1);
				if (value_given) {
					char *next_option = options_list[option_num + 1];
					option->value = str_dup(next_option);
					++option_num;
				} else {
					option->value = NULL;

					om_last_error = (struct OptionMapError) { OM_MISSING_ARG, option_alias };
					return om_last_error;
				}
			}
		}
	}

	return (struct OptionMapError) { OM_NO_ERROR, NULL };
}

bool option_map_is_option_given(struct OptionMap *option_map, const char *option_name)
{
	struct OptionMapOption *option = option_map_get_option_by_name(option_map, option_name);

	if (option == NULL) {
		return false;
	}

	return option->given;
}

char *option_map_get_option_value(struct OptionMap *option_map, const char *option_name)
{
	struct OptionMapOption *option = option_map_get_option_by_name(option_map, option_name);

	if (option == NULL) {
		return NULL;
	}

	return option->value;
}

static struct OptionMapOption *option_map_get_option_by_name(struct OptionMap *option_map, const char *option_name)
{
	for (size_t index = 0; index < option_map->option_count; ++index) {
		struct OptionMapOption *option = &option_map->options[index];
		if (strcmp(option->aliases[0], option_name) == 0) {
			return option;
		}
	}

	return NULL;
}

static struct OptionMapOption *option_map_get_option_by_alias(struct OptionMap *option_map, const char *option_alias)
{
	for (size_t index = 0; index < option_map->option_count; ++index) {
		struct OptionMapOption *option = &option_map->options[index];

		for (char **p_name = &option->aliases[0]; *p_name != NULL; ++p_name) {
			if (strcmp(*p_name, option_alias) == 0) {
				return option;
			}
		}
	}

	return NULL;
}

static char *str_dup(const char *str)
{
	if (str == NULL) {
		return NULL;
	}

	char *str_copy = ALLOC_STR_LENGTH(strlen(str));

	if (str_copy == NULL) {
		return NULL;
	}

	strcpy(str_copy, str);

	return str_copy;
}

#ifdef UNUSED
static char *str_n_dup(const char *str, size_t length)
{
	if (str == NULL) {
		return NULL;
	}

	size_t str_length = strlen(str);
	size_t size = (length < str_length) ? length + 1 : str_length + 1;
	
	char *str_copy = ALLOC_STR_SIZE(size);

	if (str_copy == NULL) {
		return NULL;
	}

	strncpy(str_copy, str, length);

	return str_copy;
}
#endif // UNUSED

static size_t str_arr_len(char **str_arr)
{
	if (str_arr == NULL) {
		return 0;
	}

	size_t length = 0;
	while (str_arr[length] != NULL) {
		++length;
	}

	return length;
}

static char **str_arr_dup(char **str_arr)
{
	size_t size = str_arr_len(str_arr) + 1;

	char **str_arr_copy = ALLOC_ARR(str_arr_copy, size);
	for (size_t index = 0; index < size; index++) {
		str_arr_copy[index] = str_dup(str_arr[index]);
	}

	return str_arr_copy;
}

void str_arr_destroy(char **str_arr)
{
	size_t size = str_arr_len(str_arr) + 1;

	for (size_t index = 0; index < size; index++) {
		free(str_arr[index]);
	}

	free(str_arr);
}

struct OptionMapError option_map_get_last_error()
{
	return om_last_error;
}

void option_map_print_error_message(FILE *stream, const char *prefix, struct OptionMapError error)
{
	static const char *ERROR_MESSAGES[OM_ERROR_COUNT] = {
		[OM_NO_ERROR]       = "",
		[OM_MISSING_ARG]    = "option '%s' requires an argument",
		[OM_UNKNOWN_OPTION] = "unknown option '%s'"
	};

	const char *message_format = ERROR_MESSAGES[error.error_code];

	fprintf(stream, "%s", prefix);
	fprintf(stream, message_format, error.related_option_alias);
	fprintf(stream, "\n");
}

void option_map_print_last_error_message(FILE *stream, const char *prefix)
{
	option_map_print_error_message(stream, prefix, om_last_error);
}

void option_map_reset_error()
{
	om_last_error.error_code = OM_NO_ERROR;
	om_last_error.related_option_alias = NULL;
}

