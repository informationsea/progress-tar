/*
 *   Progress tar
 *   Copyright (C) 2014 Yasunobu OKAMURA
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "progresstar.h"
#include "fileutil.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <zlib.h>

#define MAXIMUM_PATH 1024*100

#define ACALL(func) if ((func) != ARCHIVE_OK) goto onerror

static int progress_create(struct progress_tar *pt);
static int progress_extract(struct progress_tar *pt);
static int progress_list(struct progress_tar *pt);

#define BUFFER_SIZE 1024*5

struct fileinfo {
    struct progress_tar *pt;
    int fd;
    char buffer[BUFFER_SIZE];
    off_t size;
    off_t readsize;
};

static ssize_t progress_read_callback(struct archive *ar, void *client_data, const void **buffer);
static off_t progress_skip_callback(struct archive *ar, void *client_data, off_t request);
static int progress_open_callback(struct archive *ar, void *client_data);
static int progress_close_callback(struct archive *ar, void *client_data);


void progress_initialize(struct progress_tar *pt)
{
    memset(pt, 0, sizeof(struct progress_tar));
    pt->mode = NOT_SET_MODE;
    pt->filter = ARCHIVE_FORMAT_TAR_PAX_RESTRICTED;
    pt->format = ARCHIVE_FILTER_NONE;
    pt->archive_path = NULL;
    pt->number_of_files = 0;
    pt->files_path = NULL;
}

int progress_run(struct progress_tar *pt)
{
    char original[10240];
    if (pt->chdir) {
        getcwd(original, sizeof(original));
        fprintf(stderr, "Change to %s\n", pt->chdir);
        if (chdir(pt->chdir)) {
            perror("Cannot change directory");
            return 1;
        }
    }

    int retvalue = 2;
    
    switch(pt->mode) {
    case CREATE:
        retvalue = progress_create(pt);
        break;
    case EXTRACT:
        retvalue = progress_extract(pt);
        break;
    case LIST:
        retvalue = progress_list(pt);
        break;
    default:
        break;
    }

    chdir(original);
    
    return retvalue;
}

static int progress_create_countfiles(const char* path, struct progress_tar *pt)
{
    struct archive *disk;
    struct archive_entry *entry;
    int r;

    disk = archive_read_disk_new();
    archive_read_disk_set_standard_lookup(disk);
    archive_read_disk_set_symlink_hybrid(disk);

    r = archive_read_disk_open(disk, path);
    if (r != ARCHIVE_OK) {
        fprintf(stderr, "failed to open. %s\n", archive_error_string(disk));
        return 1;
    }

    if (pt->progress_start_count_files)
        pt->progress_start_count_files(pt);


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

        pt->number_of_files_all += 1;
        if (archive_entry_filetype(entry) == AE_IFREG)
            pt->sum_of_filesize += archive_entry_size(entry);

        if (pt->progress_count_files)
            pt->progress_count_files(pt, archive_entry_pathname(entry),
                                     pt->number_of_files_all, pt->sum_of_filesize);
        
        archive_entry_free(entry);
    }

    if (pt->progress_end_count_files)
        pt->progress_end_count_files(pt,
                                     pt->number_of_files_all, pt->sum_of_filesize);
        
    
    return 0;
}

static int progress_create_addfiles(const char* path, struct progress_tar *pt)
{

    struct archive *disk;
    struct archive_entry *entry;
    int r;

    disk = archive_read_disk_new();
    archive_read_disk_set_standard_lookup(disk);
    archive_read_disk_set_symlink_hybrid(disk);

    r = archive_read_disk_open(disk, path);
    if (r != ARCHIVE_OK) {
        fprintf(stderr, "failed to open. %s\n", archive_error_string(disk));
        return 1;
    }

    if (pt->progress_start_creating)
        pt->progress_start_creating(pt);

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
        r = archive_write_header(pt->archive, entry);

        pt->processed_filenum += 1;

        if (r < ARCHIVE_OK) {
            fprintf(stderr, "failed to write. %s\n", archive_error_string(pt->archive));
        }
        if (r == ARCHIVE_FATAL)
            return 1;

        if (archive_entry_filetype(entry) == AE_IFREG) {
            if (r > ARCHIVE_FAILED) {
                int fd = open(archive_entry_sourcepath(entry), O_RDONLY);
                char buf[1024*5];
                ssize_t len, total_len = 0;
                while((len = read(fd, buf, sizeof(buf))) > 0) {
                    archive_write_data(pt->archive, buf, len);
                    total_len += len;

                    if (pt->progress_creating)
                        pt->progress_creating(pt, archive_entry_pathname(entry),
                                              pt->processed_filenum, pt->processed_filesize + total_len);

                }
                close(fd);
            }

            pt->processed_filesize += archive_entry_size(entry);
        }

        if (pt->progress_creating)
            pt->progress_creating(pt, archive_entry_pathname(entry),
                                  pt->processed_filenum, pt->processed_filesize);

        archive_entry_free(entry);
    }

    if (pt->progress_end_creating)
        pt->progress_end_creating(pt);

    archive_read_close(disk);
    archive_read_free(disk);
    
    return 0;
}

static int progress_create(struct progress_tar *pt)
{
    size_t i;
    assert(pt->number_of_files != 0);

    /* count files */

    for (i = 0; i < pt->number_of_files; ++i) {
        int code = progress_create_countfiles(pt->files_path[i], pt);
        if (code) {
            fprintf(stderr, "Cannot read some files\n");
        }
    }

    /* archiving */
    pt->archive = archive_write_new();

    if (pt->format == AUTOSUGGEST_FORMAT)
        pt->format = filepath_suggest_format(pt->archive_path);
    if (pt->filter == AUTOSUGGEST_FILTER)
        pt->filter = filepath_suggest_filter(pt->archive_path);

    ACALL(archive_write_set_format(pt->archive, pt->format));
    ACALL(archive_write_add_filter(pt->archive, pt->filter));
    
    //switch

    ACALL(archive_write_open_filename(pt->archive, pt->archive_path));

    for (i = 0; i < pt->number_of_files; ++i) {
        int code = progress_create_addfiles(pt->files_path[i], pt);
        if (code) {
            goto onerror;
        }
    }

//    fclose(pt->file);
    ACALL(archive_write_free(pt->archive));
    
    return 0;

  onerror:
    fprintf(stderr, "%s\n", archive_error_string(pt->archive));
    
    return 1;

}

static int progress_extract(struct progress_tar *pt)
{
    struct archive *disk;
    struct archive_entry *entry;

    disk = archive_write_disk_new();
    entry = archive_entry_new();

    //ACALL(archive_write_disk_open(disk, "."));
    ACALL(archive_write_disk_set_standard_lookup(disk));
    
    pt->archive = archive_read_new();
    archive_read_support_filter_all(pt->archive);
    archive_read_support_format_all(pt->archive);

    struct fileinfo fileinfo;
    bzero(&fileinfo, sizeof(fileinfo));
    fileinfo.pt = pt;

    ACALL(archive_read_open2(pt->archive, &fileinfo,
                            progress_open_callback, progress_read_callback,
                            progress_skip_callback, progress_close_callback));

    while (archive_read_next_header2(pt->archive, entry) == ARCHIVE_OK) {
        archive_write_header(disk, entry);


        if (archive_entry_filetype(entry) == AE_IFREG) {
            char buffer[1024*10];
            ssize_t readlen;
        
            while ((readlen = archive_read_data(pt->archive, buffer, sizeof(buffer))) > 0) {
                ssize_t wrotelen = archive_write_data(disk, buffer, readlen);
                if (wrotelen < 0)
                    goto onerror;
            }

            if (readlen < 0) goto onerror;
        }

        if (pt->progress_list)
            pt->progress_list(pt, archive_entry_pathname(entry),
                              fileinfo.readsize, fileinfo.size);
    }
    
    archive_entry_free(entry);
    ACALL(archive_write_free(disk));
    ACALL(archive_read_free(pt->archive));
    
    return 0;

  onerror:
    fprintf(stderr, "%s\n", archive_error_string(pt->archive));
    
    return 1;
}

static int progress_list(struct progress_tar *pt)
{
    pt->archive = archive_read_new();
    archive_read_support_filter_all(pt->archive);
    archive_read_support_format_all(pt->archive);

    struct fileinfo fileinfo;
    bzero(&fileinfo, sizeof(fileinfo));
    fileinfo.pt = pt;

    ACALL(archive_read_open2(pt->archive, &fileinfo,
                            progress_open_callback, progress_read_callback,
                            progress_skip_callback, progress_close_callback));

    struct archive_entry *entry = archive_entry_new();
    while (archive_read_next_header2(pt->archive, entry) == ARCHIVE_OK) {
        if (pt->progress_list)
            pt->progress_list(pt, archive_entry_pathname(entry),
                              fileinfo.readsize, fileinfo.size);
        else
            fprintf(stdout, "%s (%lld/%lld) %.1lf%%\n",
                    archive_entry_pathname(entry),
                    fileinfo.readsize, fileinfo.size,
                    ((double)fileinfo.readsize)/fileinfo.size*100);
    }

    archive_entry_free(entry);
    ACALL(archive_read_free(pt->archive));
    
    return 0;

  onerror:
    fprintf(stderr, "%s\n", archive_error_string(pt->archive));
    
    return 1;
}

static ssize_t progress_read_callback(struct archive *ar, void *client_data, const void **buffer)
{
    struct fileinfo *fi = (struct fileinfo *)client_data;
    *buffer = fi->buffer;
    ssize_t readbytes = read(fi->fd, fi->buffer, sizeof(fi->buffer));

    if (readbytes < 0) {
        archive_set_error(ar, errno, "%s", strerror(errno));
        return ARCHIVE_FATAL;
    }

    fi->readsize += readbytes;

    return readbytes;
}

static off_t progress_skip_callback(struct archive *ar, void *client_data, off_t request)
{
    struct fileinfo *fi = (struct fileinfo *)client_data;

    if (request == 0)
        return 0;
    
    off_t current = lseek(fi->fd, 0, SEEK_CUR);
    off_t newpos = lseek(fi->fd, request, SEEK_CUR);

    if (newpos < 0) {
        archive_set_error(ar, errno, "%s", strerror(errno));
        return 0;
    }

    fi->readsize = newpos;

    return newpos - current;
}

static int progress_open_callback(struct archive *ar, void *client_data)
{
    struct fileinfo *fi = (struct fileinfo *)client_data;
    if (fi->pt->archive_path)
        fi->fd = open(fi->pt->archive_path, O_RDONLY);
    else
        fi->fd = STDIN_FILENO;

    if (fi->fd < 0) {
        archive_set_error(ar, errno, "%s", strerror(errno));
        return ARCHIVE_FATAL;
    }

    struct stat st;
    int ret = fstat(fi->fd, &st);
    if (ret == 0) {
        fi->size = st.st_size;
    }

    return ARCHIVE_OK;
}

static int progress_close_callback(struct archive *ar, void *client_data)
{
    struct fileinfo *fi = (struct fileinfo *)client_data;
    int ret = close(fi->fd);
    if (ret == 0)
        return ARCHIVE_OK;
    archive_set_error(ar, errno, "%s", strerror(errno));
    return ARCHIVE_FATAL;
}

