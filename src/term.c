/* term.c
   Copyright (c) 2023 bellrise */

#include <mcc/settings.h>
#include <mcc/term.h>

const char *color_end()
{
	if (settings_global()->use_colors)
		return "\033[0m";
	return "";
}

const char *color_err()
{
	if (settings_global()->use_colors)
		return "\033[1;91m";
	return "";
}

const char *color_warn()
{
	if (settings_global()->use_colors)
		return "\033[35m";
	return "";
}

const char *color_grey()
{
	if (settings_global()->use_colors)
		return "\033[90m";
	return "";
}

const char *color_bold_white()
{
	if (settings_global()->use_colors)
		return "\033[1;39m";
	return "";
}
