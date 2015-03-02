#include "inc/notify.h"

int
notify (GMimeMessage *message, struct mbangopt *opts) {
    char notiftext[255];
    sprintf(notiftext, "<u>%s</u>", g_mime_message_get_subject(message));

    NotifyNotification *n;
    notify_init("mbangnotify");

    n = notify_notification_new (g_mime_message_get_sender(message), notiftext, opts->icon);

    notify_notification_set_timeout(n, 10000);
    if (!notify_notification_show (n, NULL)) {
        g_error("Failed to send notification.\n");
        return 1;
    }
    g_object_unref(G_OBJECT(n));

    return EXIT_SUCCESS;
}

void
do_notify (gpointer key, gpointer value, gpointer user_data) {
    struct mbangopt *opt = (struct mbangopt *) user_data;
    if (value == (int *) NOT_FOUND) {
        GMimeMessage *mail;
        get_message(key, &mail);
        if (notify(mail, opt) == EXIT_SUCCESS) {
            value = (void *) FOUND;
        }
        g_object_unref(mail);
    }
}
