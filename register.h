#ifndef REGISTER_H
#define REGISTER_H

const char *register_name(int r);

int register_alloc();

void register_free(int r);

void register_freeall();
#endif
