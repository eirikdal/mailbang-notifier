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
    NotifyNotification *n;
    notify_init("mbangnotify");
    n = notify_notification_new (g_mime_message_get_subject(message),NULL, NULL);
    notify_notification_set_timeout(n, 3000); //3 seconds
    if (!notify_notification_show (n, NULL)) {
        g_error("Failed to send notification.\n");
        return 1;
    }
    g_object_unref(G_OBJECT(n));
    return EXIT_SUCCESS;
}

int readmail(const char *file) {
    char *mode = "r+";
    char key[100];
    char val[100];
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

GHashTable *rdhist() {
    FILE *ifp;
    char *mode = "r+";
    char mailf[100];
    char val[100];
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

int lsdir(const char* dirarg) {
    DIR* dir;
    char fpath[255];
    GHashTable *histTable = rdhist();

    struct dirent *ent;
    if ((dir = opendir (dirarg)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if( strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 ) {
                if (g_hash_table_lookup(histTable, ent->d_name) == NULL) {
                    g_hash_table_replace(histTable, ent->d_name, 1);
                    // start notification process
                    sprintf(fpath, "%s%s%s", dirarg, "/", ent->d_name);
                    readmail(fpath);
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
    int i;

    fp = fopen("hist.dat", "ab+");

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
        fprintf(fp, "%s\n", key);
    }
}

int scanmail(const char* inboxDir) {
    char* newDir = malloc(strlen(inboxDir)+5);
    char buf[256];
    int csource;
    glob_t result;
    char    **p;

    snprintf(buf, sizeof buf, "%s%s", inboxDir, "/new");
    csource = glob (buf, GLOB_TILDE, NULL, &result);

    if (csource == 0) {
        for (p=result.gl_pathv; *p != NULL; ++p) {
            lsdir(*p);
        }
        globfree(&result);
    }

    lsdir(buf);

    return EXIT_SUCCESS;
}

int rdconf(const char* configFile) {
    FILE *ifp;
    char *mode = "r+";
    char key[100];
    char val[100];

    ifp = fopen(configFile, mode);

    if (ifp == NULL) {
        fprintf(stderr, "Can't open config file %s!\n", configFile);
        exit(EXIT_FAILURE);
    }

    while (fscanf(ifp, "%s %[^\n]", key, val) != EOF) {
        if (strcmp("Inbox", key) == 0) {
            scanmail(val);
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

    rdconf(argv[1]);

    return EXIT_SUCCESS;
}

