void print_to_file(gpointer, gpointer, gpointer);
int lsdir(const char*);
int rdconf(const char*);
int whist(const char*);
int scanmail(const char*);
int readmail(const char*);
int notify(GMimeMessage*);
GHashTable *rdhist(void);
