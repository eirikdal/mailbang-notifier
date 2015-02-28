#include "inc/mail.h"

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

int
inbox_new_foreach (const char* glb_inbox_path, void (*func)(char*)) {
    char glb_inbox_new_path[256];
    glob_t result;

    snprintf(glb_inbox_new_path, sizeof glb_inbox_new_path, "%s%s", glb_inbox_path, "/new");

    if (glob (glb_inbox_new_path, GLOB_TILDE, NULL, &result) == 0) {
        char    **uglb_path;
        for (uglb_path=result.gl_pathv; *uglb_path != NULL; ++uglb_path) {

            (*func)(*uglb_path);
        }
        globfree(&result);
    }

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

