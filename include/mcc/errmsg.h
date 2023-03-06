/* mcc/errmsg.h
   Copyright (c) 2023 bellrise */

#pragma once

void errmsg(char *fmt, ...) __attribute__((noreturn));
void warnmsg(char *fmt, ...);
void infomsg(char *fmt, ...);
