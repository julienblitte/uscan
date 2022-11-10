#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>

char *xmlExtract(const char *tag, const char *data, bool namespace)
{
	regex_t regex;
	regmatch_t captures[3];
	char reg[128];
	int match;
	int i;
	char *result;
	int capture_len;

	if (namespace)
	{
		snprintf(reg, sizeof(reg), "[<:]%s( [a-zA-Z0-9: ]+)?>([^<>]+)", tag);
	}
	else
	{
		snprintf(reg, sizeof(reg), "<( *%s *)> *([^<>]+) *< */%s *>", tag);
	}
	regcomp(&regex, reg, 0);

	result = NULL;

	match = regexec(&regex, data, 3, captures, 0);
	if (!match)
	{
		capture_len = captures[2].rm_eo - captures[2].rm_so;
		result = (char *)malloc(capture_len + 1);
		strncpy(result, data + captures[2].rm_so, capture_len);
		result[capture_len] = '\0';
	}
	regfree(&regex);

	return result;
}

