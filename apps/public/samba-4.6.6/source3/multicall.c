#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <stdarg.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

extern int smbd_main(int argc, char **argv);
extern int nmbd_main(int argc, char **argv);
extern int smbpasswd_main(int argc, char **argv);
extern int testparm_main(int argc, char **argv);

static struct {
	const char *name;
	int (*func_main)(int argc, char **argv);
} multicall[] = {
	{ "smbd", smbd_main },
	{ "nmbd", nmbd_main },
	{ "smbpasswd", smbpasswd_main },
	{ "testparm", testparm_main },
};

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

int main(int argc, char **argv)
{
	unsigned int i, len;
	extern char *__progname;
	char path[PATH_MAX], *self;

	len = readlink("/proc/self/exe", path, sizeof(path));
	path[len] = 0;
	for (i=len-2; i>0 && path[i] != '/'; --i);
	self = path + i + 1;

	if (0 == strcmp(__progname, self)) {
		if (argc == 1) goto usage;
		argv++;
		--argc;
		__progname = argv[0];
	}

	for (i = 0; i < ARRAY_SIZE(multicall); i++) {
		if (0 == strcmp(__progname, multicall[i].name)) {
			return multicall[i].func_main(argc, argv);
		}
	}

usage:
	fprintf(stderr, "Available commands:\n", self);
	for (i = 0; i < ARRAY_SIZE(multicall); i++) {
		if (i != 0)
			fprintf(stderr, ", ");
		fprintf(stderr, "%s", multicall[i].name);
	}
	fprintf(stderr, "\n");

	return 1;
}
