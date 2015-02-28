#include "inc/file.h"
#include "inc/mail.h"

static void
write_to_notify_history(gpointer key, gpointer value, gpointer user_data) {
    FILE *fp = (FILE *) user_data;

    if (value != (int*) (DELETE_ENTITY)) {
        fprintf(fp, "%s\n", (char *)key);
    }
}

static void
compare_with_history(const char *fpath, GHashTable *histTable) {
    if (g_hash_table_lookup(histTable, fpath) == NULL) {
        g_hash_table_replace(histTable, (void *) fpath, (void *) NOT_FOUND);
    } else {
        g_hash_table_replace(histTable, (void *) fpath, (void *) FOUND);
    }
}

static void
inbox_foreach(const char* inbox_folder, void (*func)(gpointer, gpointer, gpointer)) {
    DIR* dir;
    GHashTable *histTable = read_notify_history();

    if ((dir = opendir (inbox_folder)) != NULL) {
        struct dirent *ent;
        while ((ent = readdir (dir)) != NULL) {
            if( strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 ) {
                char fpath[255];
                sprintf(fpath, "%s%s%s", inbox_folder, "/", ent->d_name);
                compare_with_history(fpath, histTable);
            }
        }
        closedir (dir);
    } else {
        perror ("");
    }

    g_hash_table_foreach(histTable, (GHFunc)func, NULL);
    write_notification_history(histTable);
    g_hash_table_destroy(histTable);
}

int
inbox_apply (const char* glb_inbox_path, void (*func)(gpointer, gpointer, gpointer)) {
    char glb_inbox_new_path[256];
    glob_t result;

    snprintf(glb_inbox_new_path, sizeof glb_inbox_new_path, "%s%s", glb_inbox_path, "/new");

    if (glob (glb_inbox_new_path, GLOB_TILDE, NULL, &result) == 0) {
        char    **uglb_path;
        for (uglb_path=result.gl_pathv; *uglb_path != NULL; ++uglb_path) {
            inbox_foreach(*uglb_path, func);
        }
        globfree(&result);
    }

    return EXIT_SUCCESS;
}

int
write_notification_history(GHashTable *histTable) {
    FILE *fp;

    fp = fopen("hist.dat", "wb+");

    if (fp == NULL) {
        printf("Couldn't open file for writing.\n");
        exit(0);
    }

    g_hash_table_foreach(histTable, (GHFunc)write_to_notify_history, fp);
    fclose(fp);

    return EXIT_SUCCESS;
}

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

GHashTable
*read_notify_history() {
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
