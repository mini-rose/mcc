/* mcc/errmsg.h
   Copyright (c) 2023 bellrise */

#pragma once

#include <stdarg.h>

struct token;
struct file;

void errmsg(char *fmt, ...) __attribute__((noreturn));
void warnmsg(char *fmt, ...);
void infomsg(char *fmt, ...);

void errsrc(struct file *source, struct token *tok, const char *fmt, ...);
void errsrc_v(struct file *source, struct token *tok, const char *fmt,
	      va_list args);
