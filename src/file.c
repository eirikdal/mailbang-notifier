#include "inc/file.h"

int
mb_lookup_key (const char *config_file, const char *key, char *out) {
    FILE *ifp;
    char *mode = "r+";
    char __val[100];
    char __key[100];

    ifp = fopen(config_file, mode);

    if (ifp == NULL) {
        fprintf(stderr, "Can't open config file %s!\n", config_file);
        exit(EXIT_FAILURE);
    }

    while (fscanf(ifp, "%99s %99[^\n]", __key, __val) != EOF) {
        if (strcmp(__key, key) == 0) {
            memcpy(out, __val, strlen(__val));
        }
    }

    fclose(ifp);
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

    while (fscanf(ifp, "%99s", mailf) != EOF) {
        char* hkey = malloc(sizeof(char) * 100);
        sprintf(hkey, "%s", mailf);
        g_hash_table_insert(hash, hkey, (gpointer) ((int*)(-1)));
    }

    fclose(ifp);
    return hash;
}
