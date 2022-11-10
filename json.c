#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>

char *jsonExtract(const char *tag, const char *data, bool subtable)
{
	regex_t regex;
	regmatch_t captures[2];
	char reg[128];
	int match;
	int i;
	char *result;
	int capture_len;

	if (subtable)
	{
		//TODO: this regex does not work with sub table
		snprintf(reg, sizeof(reg), "\"%s\" *: *{\\([^}]*\\)}", tag);
	}
	else
	{
		snprintf(reg, sizeof(reg), "\"%s\" *: *\"\\([^\"]*\\)\"", tag);
	}
	regcomp(&regex, reg, 0);

	result = NULL;

	match = regexec(&regex, data, 2, captures, 0);
	if (!match)
	{
		capture_len = captures[1].rm_eo - captures[1].rm_so;
		result = (char *)malloc(capture_len + 1);
		strncpy(result, data + captures[1].rm_so, capture_len);
		result[capture_len] = '\0';
	}
	regfree(&regex);

	return result;
}

