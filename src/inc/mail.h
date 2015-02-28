#ifndef _M_MAIN_H
#define _M_MAIN_H

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <glob.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>
#include <gmime/gmime.h>

__BEGIN_DECLS
static GMimeMessage *parse_message(int);
int inbox_new_foreach(const char*, void (*)(char*));
int parse_mail(const char*);
__END_DECLS

#endif  /* _M_MAIN_H */
