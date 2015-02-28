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
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <libnotify/notify.h>

__BEGIN_DECLS
void print_to_file(gpointer, gpointer, gpointer);
int read_config_file(const char*);
static GMimeMessage *parse_message(int);
int parse_mail(const char*);
int notify(GMimeMessage*);

void
do_notify(gpointer, gpointer, gpointer)
__END_DECLS

#endif  /* _M_MAIN_H */
