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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "progresstar.h"
#include "optparse.h"


void print_usage(void);

static void debug_print_options(const struct progress_tar *tar_state);


static void progress_start_count_files(struct progress_tar *tar);
static void progress_count_files(struct progress_tar *tar,
                                 const char *filename,
                                 uint64_t file_num,
                                 off_t file_size);
static void progress_end_count_files(struct progress_tar *tar,
                                     uint64_t file_num,
                                     off_t file_size);

static void progress_start_creating(struct progress_tar *tar);
static void progress_creating(struct progress_tar *tar,
                              const char *filename,
                              uint64_t file_num,
                              off_t file_size);
static void progress_end_creating(struct progress_tar *tar);

static void progress_list(struct progress_tar *tar,
                          const char *filename,
                          off_t readsize, off_t filesize);



int main(int argc, char *argv[])
{
    struct progress_tar tar_state;

    progress_initialize(&tar_state);
    ptar_optparse(&tar_state, argc, (const char **)argv);

    tar_state.progress_start_count_files = progress_start_count_files;
    tar_state.progress_count_files = progress_count_files;
    tar_state.progress_end_count_files = progress_end_count_files;

    tar_state.progress_start_creating = progress_start_creating;
    tar_state.progress_creating = progress_creating;
    tar_state.progress_end_creating = progress_end_creating;

    tar_state.progress_list = progress_list;

    debug_print_options(&tar_state);
    progress_run(&tar_state);
    
    return 0;
}

void print_usage(void)
{
    fprintf (stderr, "Usage: ptar [cxtjzZJafC] ... path ...\n");
}

static void progress_start_count_files(struct progress_tar *tar)
{
    (void)tar;
    fprintf(stderr, "\n");
}

static void progress_count_files(struct progress_tar *tar,
                                 const char *filename,
                                 uint64_t file_num,
                                 off_t file_size)
{
    (void)tar;
    fprintf(stderr, "counting %s  %llu %lld\n", filename, file_num, file_size);
}

static void progress_end_count_files(struct progress_tar *tar,
                                     uint64_t file_num,
                                     off_t file_size)
{
    (void)tar;
    fprintf(stderr, "Sum of filesize: %lld\n", file_num);
    fprintf(stderr, "Number of files: %llu\n", file_size);
}


static void progress_start_creating(struct progress_tar *tar)
{
    (void)tar;
    fprintf(stderr, "\n");
}

static void progress_creating(struct progress_tar *tar,
                              const char *filename,
                              uint64_t file_num,
                              off_t file_size)
{
    (void)tar;
    const int weight = 1000;
    fprintf(stderr, "adding %s  %llu/%llu %lld/%lld  (%.1lf%%)\n", filename,
            file_num, tar->number_of_files_all,
            file_size, tar->sum_of_filesize,
            ((double)file_size + file_num*weight)/(tar->sum_of_filesize + tar->number_of_files_all*weight)*100);
}

static void progress_end_creating(struct progress_tar *tar)
{
    (void)tar;
    fprintf(stderr, "\n");
}

static void progress_list(struct progress_tar *tar,
                          const char *filename,
                          off_t readsize, off_t filesize)
{
    (void)tar;
    fprintf(stdout, "%s (%.1lf%%)\n",
            filename,
            ((double)readsize)/filesize*100);

}


static void debug_print_options(const struct progress_tar *tar_state)
{
    const char *mode = "Unknown";
    size_t i;
    switch(tar_state->mode) {
    case NOT_SET_MODE:
        mode = "NOT_SET_MODE";
        break;
    case CREATE:
        mode = "CREATE";
        break;
    case EXTRACT:
        mode = "EXTRACT";
        break;
    case LIST:
        mode = "LIST";
        break;
    /* case COMPARE: */
    /*     mode = "COMPARE"; */
    /*     break; */
    }
    
    fprintf(stderr, "Mode        : %s\n", mode);
    fprintf(stderr, "Filter      : %d\n", tar_state->filter);
    fprintf(stderr, "Archive path: %s\n", tar_state->archive_path);
    fprintf(stderr, "# of files  : %lu\n", tar_state->number_of_files);
    for (i = 0; i < tar_state->number_of_files; ++i) {
        fprintf(stderr, "Files[%2lu]: %s\n", i, tar_state->files_path[i]);
    }
}

