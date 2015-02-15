#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <glob.h>
#include <libnotify/notify.h>
#include <glib.h>
#include <unistd.h>
#include "inc/config.h"
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

    /* create a stream to read from the file descriptor */
    stream = g_mime_stream_fs_new (fd);

    /* create a new parser object to parse the stream */
    parser = g_mime_parser_new_with_stream (stream);

    /* unref the stream (parser owns a ref, so this object does not actually get free'd until we destroy the parser) */
    g_object_unref (stream);

    /* parse the message from the stream */
    message = g_mime_parser_construct_message (parser);

    /* free the parser (and the stream) */
    g_object_unref (parser);

    return message;
}

int notify() {
    NotifyNotification *n;
    notify_init("mbangnotify");
    n = notify_notification_new ("Ny mail!",NULL, NULL);
    notify_notification_set_timeout(n, 3000); //3 seconds
    if (!notify_notification_show (n, NULL)) {
        g_error("Failed to send notification.\n");
        return 1;
    }
    g_object_unref(G_OBJECT(n));
    return EXIT_SUCCESS;
}

int readmail(const char *file) {
    FILE *ifp;
    char *mode = "r+";
    char key[100];
    char val[100];
    GMimeMessage *message;
    int fd;

    printf("%s", file);
    ifp = fopen(file, mode);

    if (ifp == NULL) {
        fprintf(stderr, "Can't open config file %s!\n", file);
        return 0;
    }

    notify();

    if ((fd = open (file, "r", 0)) == -1) {
        fprintf (stderr, "Cannot open message `%s': %s\n", file, g_strerror (errno));
        return 0;
    }

    /* init the gmime library */
    g_mime_init (0);

    /* parse the message */
    message = parse_message (fd);

    while (fscanf(ifp, "%s %[^\n]", key, val) != EOF) {
        message = parse_message (fd);
    }

    fclose(ifp);
    /* free the mesage */
    g_object_unref (message);

    return EXIT_SUCCESS;
}

int lsdir(const char* dirarg) {
    DIR* dir;
    char fpath[255];

    struct dirent *ent;
    if ((dir = opendir (dirarg)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if( strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 ) {
                sprintf(fpath, "%s%s%s", dirarg, "/", ent->d_name);
                readmail(fpath);
            }
        }
        closedir (dir);
    } else {
        perror ("");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
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
            printf("hell yeah! %s\n", p);
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

