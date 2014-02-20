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

#ifndef _OPTPARSE_H_
#define _OPTPARSE_H_

#include <stdbool.h>

#include "progresstar.h"


#define FIFO_MAX_QUEUE 100
typedef char fifo_queue_type;

struct fifo_queue {
    fifo_queue_type queue[FIFO_MAX_QUEUE];
    ssize_t next_input_pos;
    ssize_t next_output_pos;
};

void fifo_queue_initialize(struct fifo_queue *queue);
bool fifo_queue_shift(struct fifo_queue *queue, fifo_queue_type data);
ssize_t fifo_queue_length(struct fifo_queue *queue);
fifo_queue_type fifo_queue_unshift(struct fifo_queue *queue);

bool ptar_optparse(struct progress_tar *pt, int argc, const char **argv);

#endif /* _OPTPARSE_H_ */










