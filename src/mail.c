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

void
get_message(const char *file, GMimeMessage **out) {
    int fd;

    if ((fd = open (file, O_RDONLY, 0)) == -1) {
        fprintf (stderr, "Cannot open message `%s': %s\n", file, g_strerror (errno));
    }

    /* parse the message */
    *out = parse_message (fd);
}

