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

static
void print_to_file(gpointer key, gpointer value, gpointer user_data) {
    FILE *fp = (FILE *) user_data;

    if (value != (int*) (-1)) {
        fprintf(fp, "%s\n", (char *)key);
    }
}

void read_inbox_folder(const char* dirarg) {
    DIR* dir;
    GHashTable *histTable = read_notify_history();
    printf("%s\n", dirarg);
    if ((dir = opendir (dirarg)) != NULL) {
        struct dirent *ent;
        while ((ent = readdir (dir)) != NULL) {
            if( strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 ) {
                if (g_hash_table_lookup(histTable, ent->d_name) == NULL) {
                    g_hash_table_replace(histTable, ent->d_name, (gpointer) 1);
                    // start notification process
                    char fpath[255];
                    sprintf(fpath, "%s%s%s", dirarg, "/", ent->d_name);
                    parse_mail(fpath);
                } else {
                    // set flag
                    int val = 1;
                    g_hash_table_replace(histTable, ent->d_name, (void *) &val);
                }
            }
        }
        closedir (dir);
    } else {
        perror ("");
    }

    FILE *fp;

    fp = fopen("hist.dat", "wb+");

    if (fp == NULL) {
        printf("Couldn't open file for writing.\n");
        exit(0);
    }

    g_hash_table_foreach(histTable, (GHFunc)print_to_file, fp);
    fclose(fp);

    g_hash_table_destroy(histTable);
}

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        printf( "usage: %s <mailbox>", argv[0] );
        return 0;
    }

    char inbox_path[100];
    if (mb_lookup_key(argv[1], "Inbox", inbox_path) == EXIT_SUCCESS) {
        inbox_new_foreach(inbox_path, read_inbox_folder);
    }
    return EXIT_SUCCESS;
}

