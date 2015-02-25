#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <glob.h>
#include <libnotify/notify.h>
#include <glib.h>
#include <unistd.h>
#include <gmime/gmime.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#ifndef MAIN_H
#include "inc/main.h"
#define MAIN_H
#endif

static GMimeMessage *
parse_message (int fd)
{
    GMimeMessage *message;
    GMimeParser *parser;
    GMimeStream *stream;

    g_mime_init (0);

    stream = g_mime_stream_fs_new (fd);
    parser = g_mime_parser_new_with_stream (stream);
    g_object_unref (stream);
    message = g_mime_parser_construct_message (parser);
    g_object_unref (parser);
    return message;
}

int notify(GMimeMessage *message) {
    char notiftext[255];
    sprintf(notiftext, "<u>%s</u>", g_mime_message_get_subject(message));

    NotifyNotification *n;
    notify_init("mbangnotify");

    n = notify_notification_new (g_mime_message_get_sender(message), notiftext, "/usr/share/icons/Numix/64x64/actions/mail_new.svg");

    notify_notification_set_timeout(n, 10000); //3 seconds
    if (!notify_notification_show (n, NULL)) {
        g_error("Failed to send notification.\n");
        return 1;
    }
    g_object_unref(G_OBJECT(n));

    return EXIT_SUCCESS;
}

int parse_mail(const char *file) {
    GMimeMessage *message;
    int fd;

    if ((fd = open (file, O_RDONLY, 0)) == -1) {
        fprintf (stderr, "Cannot open message `%s': %s\n", file, g_strerror (errno));
        return 0;
    }

    /* parse the message */
    message = parse_message (fd);
    notify(message);
    g_object_unref (message);

    return EXIT_SUCCESS;
}

GHashTable *read_notify_history() {
    FILE *ifp;
    char mailf[100];
    GHashTable* hash = g_hash_table_new(g_str_hash, g_str_equal);


    ifp = fopen("hist.dat", "ab+");

    if (ifp == NULL) {
        fprintf(stderr, "Can't open history!\n");
        exit(EXIT_FAILURE);
    }

    while (fscanf(ifp, "%s", mailf) != EOF) {
        char* hkey = malloc(sizeof(char) * 100);
        sprintf(hkey, "%s", mailf);
        g_hash_table_insert(hash, hkey, -1);
    }

    fclose(ifp);
    return hash;
}

int read_inbox_folder(const char* dirarg) {
    DIR* dir;
    char fpath[255];
    GHashTable *histTable = read_notify_history();

    struct dirent *ent;
    if ((dir = opendir (dirarg)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if( strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 ) {
                if (g_hash_table_lookup(histTable, ent->d_name) == NULL) {
                    g_hash_table_replace(histTable, ent->d_name, 1);
                    // start notification process
                    sprintf(fpath, "%s%s%s", dirarg, "/", ent->d_name);
                    parse_mail(fpath);
                } else {
                    // set flag
                    g_hash_table_replace(histTable, ent->d_name, 1);
                }
            }
        }
        closedir (dir);
    } else {
        perror ("");
        return EXIT_FAILURE;
    }

    FILE *fp;

    fp = fopen("hist.dat", "wb+");

    if (fp == NULL) {
        printf("I couldn't open results.dat for writing.\n");
        exit(0);
    }

    g_hash_table_foreach(histTable, (GHFunc)print_to_file, fp); // create new buffer with all keys having value 1 in hashtable..
    fclose(fp);

    g_hash_table_destroy(histTable);
    return EXIT_SUCCESS;
}

void print_to_file(gpointer key, gpointer value, gpointer user_data) {
    FILE *fp = (FILE *) user_data;

    if (value != -1) {
        fprintf(fp, "%s\n", (char *)key);
    }
}

int unglob_inbox_path(const char* glb_inbox_path) {
    char glb_inbox_new_path[256];
    glob_t result;
    char    **uglb_path;

    snprintf(glb_inbox_new_path, sizeof glb_inbox_new_path, "%s%s", glb_inbox_path, "/new");

    if (glob (glb_inbox_new_path, GLOB_TILDE, NULL, &result) == 0) {
        for (uglb_path=result.gl_pathv; *uglb_path != NULL; ++uglb_path) {
            read_inbox_folder(*uglb_path);
        }
        globfree(&result);
    }

    return EXIT_SUCCESS;
}

int read_config_file(const char* config_file) {
    FILE *ifp;
    char *mode = "r+";
    char key[100];
    char val[100];

    ifp = fopen(config_file, mode);

    if (ifp == NULL) {
        fprintf(stderr, "Can't open config file %s!\n", config_file);
        exit(EXIT_FAILURE);
    }

    while (fscanf(ifp, "%s %[^\n]", key, val) != EOF) {
        if (strcmp("Inbox", key) == 0) {
            unglob_inbox_path(val);
        }
    }

    fclose(ifp);
    return EXIT_SUCCESS;
}

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        printf( "usage: %s <mailbox>", argv[0] );
        return 0;
    }

    read_config_file(argv[1]);

    return EXIT_SUCCESS;
}

