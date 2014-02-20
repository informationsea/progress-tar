#include <stdio.h>
#include <archive.h>
#include <archive_entry.h>

int main(int argc, char *argv[])
{
    struct archive *disk;
    struct archive_entry *entry;
    int r;
    
    if (argc != 2) {
        fprintf(stderr, "%s PATH\n", argv[0]);
        return 1;
    }

    disk = archive_read_disk_new();
    archive_read_disk_set_standard_lookup(disk);
    archive_read_disk_set_symlink_hybrid(disk);

    r = archive_read_disk_open(disk, argv[1]);
    if (r != ARCHIVE_OK) {
        fprintf(stderr, "failed to open. %s\n", archive_error_string(disk));
        return 1;
    }

    for (;;) {
        entry = archive_entry_new();
        r = archive_read_next_header2(disk, entry);
        if (r == ARCHIVE_EOF) {
            break;
        }
        
        if (r != ARCHIVE_OK) {
            fprintf(stderr, "failed to open. %s\n", archive_error_string(disk));
            break;
        }
        archive_read_disk_descend(disk);

        printf("%s %lld\n", archive_entry_pathname(entry), archive_entry_size(entry));
        archive_entry_free(entry);
    }
    
    
    return 0;
}
