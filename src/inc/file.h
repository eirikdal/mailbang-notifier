#ifndef M_FILE_H
#define M_FILE_H

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <glob.h>
#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <libnotify/notify.h>
#include <unistd.h>
#include <gmime/gmime.h>
#include <string.h>

#define NOT_FOUND 0
#define DELETE_ENTITY -1
#define FOUND 1

#endif

int mb_lookup_key (const char*, const char*, char*);
GHashTable *read_notify_history(void);
void read_inbox_folder(const char*);
int inbox_apply(const char*, void (*)(gpointer, gpointer, gpointer));
int write_notification_history(GHashTable *);
