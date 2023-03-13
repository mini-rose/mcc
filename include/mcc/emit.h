/* emit.h - emit targets for mcc
   Copyright (c) 2023 bellrise */

#pragma once

#include <mcc/parser.h>

struct emit_target
{
	const char *id;
	const char *file_suffix;
	char *(*fn)(struct module *mod);
};

const char **emit_target_list(int *len);

/*
 * Return the emitter definition for the given target.
 *
 * Currently supported targets:
 *  - "llvm"
 */
const struct emit_target *emit_for_target(const char *tgt);
