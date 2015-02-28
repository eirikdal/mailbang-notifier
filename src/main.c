#include "inc/file.h"
#include "inc/mail.h"
#include "inc/main.h"

int notify(GMimeMessage *message) {
    char notiftext[255];
    sprintf(notiftext, "<u>%s</u>", g_mime_message_get_subject(message));

    NotifyNotification *n;
    notify_init("mbangnotify");

    n = notify_notification_new (g_mime_message_get_sender(message), notiftext, "/usr/share/icons/Numix/64x64/actions/mail_new.svg");

    notify_notification_set_timeout(n, 10000);
    if (!notify_notification_show (n, NULL)) {
        g_error("Failed to send notification.\n");
        return 1;
    }
    g_object_unref(G_OBJECT(n));

    return EXIT_SUCCESS;
}

void
do_notify(gpointer key, gpointer value, gpointer user_data) {
    if (value != (int *) FOUND) {
        GMimeMessage *mail;
        get_message(key, &mail);
        if (notify(mail) == EXIT_SUCCESS) {
            value = (void *) FOUND;
        }
        g_object_unref(mail);
    }
}

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        printf( "usage: %s <mailbox>", argv[0] );
        return 0;
    }

    char inbox_path[100];
    if (mb_lookup_key(argv[1], "Inbox", inbox_path) == EXIT_SUCCESS) {
        inbox_apply(inbox_path, do_notify);
    }
    return EXIT_SUCCESS;
}

