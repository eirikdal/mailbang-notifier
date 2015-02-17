void print_to_file(gpointer, gpointer, gpointer);
int read_inbox_folder(const char*);
int read_config_file(const char*);
static GMimeMessage *parse_message(int);
int unglob_inbox_path(const char*);
int parse_mail(const char*);
int notify(GMimeMessage*);
GHashTable *read_notify_history(void);
