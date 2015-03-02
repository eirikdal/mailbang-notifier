#ifndef M_NOTIFY_H
#define M_NOTIFY_H

struct mbangopt
{
    char *icon;
};

#include "file.h"
#include <libnotify/notify.h>

int notify (GMimeMessage*, struct mbangopt*);
void do_notify(gpointer, gpointer, gpointer);

#endif
