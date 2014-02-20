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

#ifndef _PROGRESSTAR_H_
#define _PROGRESSTAR_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <archive.h>
#include <archive_entry.h>

#define UNKNOWN_FORMAT     0
#define AUTOSUGGEST_FORMAT 1
#define AUTOSUGGEST_FILTER 1

struct progress_tar {
/* public */
    bool show_help;
    int format;
    int filter;
    enum mode {NOT_SET_MODE, CREATE, LIST, EXTRACT} mode;
    const char *archive_path;
    size_t number_of_files;
    const char *chdir;
    const char **files_path;

    void *userdata;

    void (*progress_start_count_files)(struct progress_tar *tar);
    void (*progress_count_files)(struct progress_tar *tar,
                                 const char *filename,
                                 uint64_t file_num,
                                 off_t file_size);
    void (*progress_end_count_files)(struct progress_tar *tar,
                                     uint64_t file_num,
                                     off_t file_size);

    void (*progress_start_creating)(struct progress_tar *tar);
    void (*progress_creating)(struct progress_tar *tar,
                              const char *filename,
                              uint64_t file_num,
                              off_t file_size);
    void (*progress_end_creating)(struct progress_tar *tar);
                                       
    void (*progress_list)(struct progress_tar *tar,
                          const char *filename,
                          off_t readsize, off_t filesize);

    void (*progress_start_extract)(struct progress_tar *tar, size_t filesize);
    void (*progress_extract)(struct progress_tar *tar,
                             const char *filename,
                             off_t readsize, off_t filesize);

    /* private */
    off_t sum_of_filesize;
    uint64_t number_of_files_all;

    off_t processed_filesize;
    uint64_t processed_filenum;
    
    struct archive *archive;
    
};

void progress_initialize(struct progress_tar *pt);
int progress_run(struct progress_tar *pt);

#endif /* _PROGRESSTAR_H_ */

