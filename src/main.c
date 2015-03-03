#include "inc/main.h"

const char *argp_program_version =
    "mbangnotify 1.0";
const char *argp_program_bug_address =
    "<https://github.com/eirikdal/mbangnotify/issues>";

/* Program documentation. */
static char doc[] =
    "mbangnotify - mailscanner integration with libnotify and isync";

/* A description of the arguments we accept. */
static char args_doc[] = "CONFIG";

/* The options we understand. */
static struct argp_option options[] = {
    {"icon",  'i', 0,      0,  "Use custom icon" },
    { 0 }
};

struct arguments
{
    char *args[1];
    char *icon;
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key)
        {
        case 'i':
            arguments->icon = arg;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num > 1) {
                argp_usage (state);
            }
            arguments->args[state->arg_num] = arg;
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1)
                argp_usage (state);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
        }
    return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, const char* argv[]) {
    struct arguments arguments;

    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    argp_parse (&argp, argc, argv, 0, 0, &arguments);

    struct mbangopt opts = { .icon = arguments.icon };

    char inbox_path[100];
    if (mb_lookup_key(arguments.args[0], "Inbox", inbox_path) == EXIT_SUCCESS) {
        inbox_apply(inbox_path, do_notify, opts);
    }
    return EXIT_SUCCESS;
}

