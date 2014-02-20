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

#include "optparse.h"

#include <string.h>

void fifo_queue_initialize(struct fifo_queue *queue)
{
    memset(queue, 0, sizeof(struct fifo_queue));
}

bool fifo_queue_shift(struct fifo_queue *queue, fifo_queue_type data)
{
    if (queue->next_input_pos >= FIFO_MAX_QUEUE && queue->next_output_pos > 0) {
        memmove(queue->queue, queue->queue + queue->next_output_pos, queue->next_input_pos - queue->next_output_pos);
        queue->next_output_pos = 0;
        queue->next_input_pos -= queue->next_output_pos;
    }

    if (queue->next_input_pos >= FIFO_MAX_QUEUE)
        return false;

    queue->queue[queue->next_input_pos++] = data;
    return true;
}

ssize_t fifo_queue_length(struct fifo_queue *queue)
{
    return queue->next_input_pos - queue->next_output_pos;
}

fifo_queue_type fifo_queue_unshift(struct fifo_queue *queue)
{
    if (fifo_queue_length(queue) > 0)
        return queue->queue[queue->next_output_pos++];
    return 0;
}

static bool ptar_parse_one(struct progress_tar *pt, const char *arg, struct fifo_queue *pending_options)
{
    if (arg[0] == '-') { // long options

    } else {
        for (; *arg; arg++) {
            switch (*arg) {
            case 'c':
                pt->mode = CREATE;
                break;
            case 'x':
                pt->mode = EXTRACT;
                break;
            case 't':
                pt->mode = LIST;
                break;
            case 'z':
                pt->filter = ARCHIVE_FILTER_GZIP;
                break;
            case 'Z':
                pt->filter = ARCHIVE_FILTER_COMPRESS;
                break;
            case 'j':
                pt->filter = ARCHIVE_FILTER_BZIP2;
                break;
            case 'J':
                pt->filter = ARCHIVE_FILTER_XZ;
                break;
            case 'a':
                pt->filter = AUTOSUGGEST_FILTER;
                pt->format = AUTOSUGGEST_FORMAT;
                break;
            case 'h':
                pt->show_help = true;
                break;
            case 'f':
            case 'C':
                fifo_queue_shift(pending_options, *arg);
                break;
            default: // unknown option
                return false;
            }
        }
    }
    return true;
}

bool ptar_optparse(struct progress_tar *pt, int argc, const char **argv)
{
    int i;
    struct fifo_queue pending_options;
    fifo_queue_initialize(&pending_options);

    if (argc == 1) {
        return false;
    }
    
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-' || i == 1) {
            if (argv[i][0] == '-') {
                if (!ptar_parse_one(pt, argv[i]+1, &pending_options)) return false;
            } else {
                if (!ptar_parse_one(pt, argv[i], &pending_options)) return false;
            }
            // options
        } else {
            if (fifo_queue_length(&pending_options) > 0) {
                switch (fifo_queue_unshift(&pending_options)) {
                case 'f':
                    pt->archive_path = argv[i];
                    break;
                case 'C':
                    pt->chdir = argv[i];
                    break;
                }
            } else {
                pt->files_path = &argv[i];
                pt->number_of_files = argc - i;
                break;
            }
        }
    }

    if (pt->show_help)
        return true;

    if (pt->mode == NOT_SET_MODE) {
        fprintf(stderr, "Error: No mode options\n");
        return false;
    }

    if (pt->mode == CREATE && pt->number_of_files == 0) {
        fprintf(stderr, "Error: No files\n");
        return false;
    }

    
    return true;
}
