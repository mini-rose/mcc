/* mcc/mcc.h
   Copyright (c) 2023 bellrise */

#pragma once

#define MCC_MAJOR 0
#define MCC_MINOR 12

#define MCC_LD  "/usr/bin/ld"
#define MCC_CC  "/usr/bin/cc"
#define MCC_LDD "/lib/ld-linux-x86-64.so.2"

#define MCC_HELP_DIR "help"

#define MCC_CMDSIZ 8192

struct strlist;

enum mcc_link_mode
{
	MCC_LINK_EXEC,
	MCC_LINK_SHARED
};

int mcc_run();
int mcc_compile(const char *input, const char *output);
int mcc_compile_c(const char *intput, const char *output);
int mcc_link(enum mcc_link_mode mode, struct strlist *inputs,
	     const char *output);
