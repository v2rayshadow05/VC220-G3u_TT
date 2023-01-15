/* Shared library add-on to iptables to add TCP support. */
#include <stdbool.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <netinet/in.h>
#include <xtables.h>

struct xt_gre
{
	__u16 proNum;
};

static void gre_help(void)
{
	printf("%s", "gre match options:\n[!] --next-pro protocol match when gre next protocol == comp\n");
}

static const struct option gre_opts[] = {
	{.name = "next-pro", .has_arg = true,  .val = '1'},
	XT_GETOPT_TABLEEND,
};

struct gre_next_protocols {
	const char *name;
	unsigned int num;
};

static const struct gre_next_protocols gre_next_protocols[] = 
{ 
	{ "PPP", 0x880b },
    { "NONE", 0 },
};

static unsigned int
parse_gre_next_protocol(const char *protocolName)
{
	unsigned int ret = 0;
	char *buffer;
	int i = 0;

	buffer = strdup(protocolName);

	for (i = 0; i < ARRAY_SIZE(gre_next_protocols); ++i)
		if (strcasecmp(gre_next_protocols[i].name, protocolName) == 0) {
			ret = gre_next_protocols[i].num;
			break;
		}
	if (i == ARRAY_SIZE(gre_next_protocols))
		xtables_error(PARAMETER_PROBLEM, "Unknown GRE next protocol name `%s'", protocolName);

	free(buffer);
	return ret;
}

static void gre_init(struct xt_entry_match *m)
{
	struct xt_gre *greinfo = (struct xt_gre *)m->data;
	greinfo->proNum = 0xFFFF;
}

static int
gre_parse(int c, char **argv, int invert, unsigned int *flags,
          const void *entry, struct xt_entry_match **match)
{
	struct xt_gre *greinfo = (struct xt_gre *)(*match)->data;

	switch (c) {
	case '1':
		/* check validation */	
		greinfo->proNum = (__u16)parse_gre_next_protocol(optarg);
		break;
	}

	return 1;
}

static void
gre_print(const void *ip, const struct xt_entry_match *match, int numeric)
{
	const struct xt_gre *greinfo = (struct xt_gre *)match->data;
	const char *pProName = NULL;
	int i = 0;

	for (i = 0; i < ARRAY_SIZE(gre_next_protocols); ++i)
		if (gre_next_protocols[i].num == greinfo->proNum) {
			pProName = gre_next_protocols[i].name;
			break;
		}
	printf("gre next protocol number: %04x, name: %s", greinfo->proNum, pProName);
}

static void gre_save(const void *ip, const struct xt_entry_match *match)
{
	const struct xt_gre *greinfo = (struct xt_gre *)match->data;

	printf("[ %s ]: gre next protocol number: %04x", __FUNCTION__, greinfo->proNum);
}

static struct xtables_match gre_match = {
	.family		= NFPROTO_UNSPEC,
	.name		= "gre",
	.version	= XTABLES_VERSION,
	.size		= XT_ALIGN(sizeof(struct xt_gre)),
	.userspacesize	= XT_ALIGN(sizeof(struct xt_gre)),
	.help		= gre_help,
	.init		= gre_init,
	.parse		= gre_parse,
	.print		= gre_print,
	.save		= gre_save,
	.extra_opts	= gre_opts,
};

void
_init(void)
{
	xtables_register_match(&gre_match);
}
