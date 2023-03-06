/* help.c
   Copyright (c) 2023 bellrise */

#include <dirent.h>
#include <mcc/errmsg.h>
#include <mcc/mcc.h>
#include <mcc/settings.h>
#include <mcc/strlist.h>
#include <stdio.h>
#include <string.h>

void help_short()
{
	puts("usage: mcc [-h] [-o output] source...");
}

void help()
{
	help_short();
	puts("Compile, build & link mocha source code.\n");
	printf(
	    "  -c                    only compile, do not link\n"
	    "  --color[=never]       use colors in the output\n"
	    "  -h, --help[=topic]    show a help page and exit\n"
	    "  --helpdir <dir>       directory containing help pages\n"
	    "  -o, --output <path>   output file\n"
	    "  -s, --shared          link into a shared object\n"
	    "  -V, --verbose         be more verbose\n"
	    "  -v, --version         output the compiler version and exit\n");
}

static struct strlist find_topics()
{
	struct strlist topics = {0};
	struct dirent *dirent;
	DIR *dirp;

	dirp = opendir(settings_global()->helpdir ? settings_global()->helpdir
						  : MCC_HELP_DIR);

	if (!dirp)
		return topics;

	while ((dirent = readdir(dirp))) {
		if (dirent->d_type != DT_REG)
			continue;
		strlist_append(&topics, dirent->d_name);
	}

	closedir(dirp);
	return topics;
}

static void help_list_topics()
{
	struct strlist topics = find_topics();

	if (!topics.len) {
		puts("Found no help pages, try `--helpdir <dir>`");
		return;
	}

	printf("Found %d help page(s), show one of them using `--help=page`\n",
	       topics.len);
	for (int i = 0; i < topics.len; i++) {
		printf("  %s\n", topics.strs[i]);
	}

	strlist_destroy(&topics);
}

static void dump_help_file(const char *name)
{
	char path[PATH_MAX];
	FILE *f;

	snprintf(path, PATH_MAX, "%s/%s",
		 settings_global()->helpdir ? settings_global()->helpdir
					    : MCC_HELP_DIR,
		 name);

	f = fopen(path, "r");
	if (!f)
		errmsg("failed to open help file at %s", path);

	/* Re-use the path buffer to print the file. */
	while (fgets(path, PATH_MAX, f))
		fputs(path, stdout);

	fclose(f);
}

void help_topic(const char *topic)
{
	struct strlist topics;

	if (!strcmp(topic, "help") || !strcmp(topic, "list")) {
		help_list_topics();
		return;
	}

	topics = find_topics();
	for (int i = 0; i < topics.len; i++) {
		if (strcmp(topic, topics.strs[i]))
			continue;

		dump_help_file(topic);
		break;
	}

	strlist_destroy(&topics);
}
