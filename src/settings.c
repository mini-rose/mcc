/* settings.c
   Copyright (c) 2023 bellrise */

#include <mcc/alloc.h>
#include <mcc/mcc.h>
#include <mcc/settings.h>
#include <string.h>

void settings_defaults()
{
	struct settings *s = settings_global();

	memset(s, 0, sizeof(*s));
	s->use_colors = true;
	s->ldd = slab_strdup(MCC_LDD);
}

struct settings *settings_global()
{
	static struct settings global_settings = {0};
	return &global_settings;
}
