#include <limits.h>
#include <mcc/alloc.h>
#include <mcc/utils/utils.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

bool isfile(const char *path)
{
	struct stat _stat;

	if (stat(path, &_stat) != 0)
		return false;

	return (S_ISREG(_stat.st_mode)) ? true : false;
}

bool isdir(const char *path)
{
	struct stat _stat;

	if (stat(path, &_stat) != 0)
		return false;

	return (S_ISDIR(_stat.st_mode)) ? true : false;
}

char *abspath(const char *path)
{
	char abs[PATH_MAX];

	getcwd(abs, PATH_MAX);
	strcat(abs, "/");
	strcat(abs, path);

	return slab_strdup(abs);
}
