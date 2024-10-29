#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define MAX_FILES 10
#define MAX_CHAR_COUNT 2000
#define MAX_FILE_NAME 20

struct File {
    char name[MAX_FILE_NAME];
    char content[MAX_CHAR_COUNT];
};

struct FileSystem {
    struct File files[MAX_FILES];
};

extern void init_file_system();
extern int create_file(char* name);
extern void set_content_for(int index, char* content);
extern char* get_file_name_at(int index);
extern int get_index_by_name(char* name);
extern struct File* get_file_at(int index);
extern void delete_file(char* name);

#endif //FILESYSTEM_H