/* mcc/errmsg.h
   Copyright (c) 2023 bellrise */

#pragma once

struct token;
struct file;

void errmsg(char *fmt, ...) __attribute__((noreturn));
void warnmsg(char *fmt, ...);
void infomsg(char *fmt, ...);

enum error_code
{
	E_UNTERMSTR
};

const char *errstr(enum error_code e);

void errsrc(struct file *source, struct token *tok, enum error_code);
