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
#include "progresstar.h"
#include <cutter.h>

void test_fifo(void)
{
    struct fifo_queue q;
    fifo_queue_initialize(&q);
    
    cut_assert_equal_size(0, fifo_queue_length(&q));
    cut_assert_equal_boolean(true, fifo_queue_shift(&q, 'a'));
    cut_assert_equal_size(1, fifo_queue_length(&q));
    cut_assert_equal_boolean(true, fifo_queue_shift(&q, 'b'));
    cut_assert_equal_size(2, fifo_queue_length(&q));
    cut_assert_equal_char('a', fifo_queue_unshift(&q));
    cut_assert_equal_size(1, fifo_queue_length(&q));
    cut_assert_equal_char('b', fifo_queue_unshift(&q));
    cut_assert_equal_size(0, fifo_queue_length(&q));
}

void test_optparse_create(void)
{
    const char* argv[] = {"ptar", "caf", "foo.tar.gz", "foo", "bar"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(true, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));

    cut_assert_equal_int(CREATE, tar_state.mode);
    cut_assert_equal_int(AUTOSUGGEST_FORMAT, tar_state.format);
    cut_assert_equal_int(AUTOSUGGEST_FILTER, tar_state.filter);
    cut_assert_equal_string("foo.tar.gz", tar_state.archive_path);
    cut_assert_equal_string_or_null(NULL, tar_state.chdir);

    cut_assert_equal_int(2, tar_state.number_of_files);
    cut_assert_equal_string("foo", tar_state.files_path[0]);
    cut_assert_equal_string("bar", tar_state.files_path[1]);
}

void test_optparse_create2(void)
{
    const char* argv[] = {"ptar", "-c", "-J", "-f", "foo.tar.xz", "foo"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(true, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));

    cut_assert_equal_int(CREATE, tar_state.mode);
    cut_assert_equal_int(AUTOSUGGEST_FORMAT, tar_state.format);
    cut_assert_equal_int(ARCHIVE_FILTER_XZ, tar_state.filter);
    cut_assert_equal_string("foo.tar.xz", tar_state.archive_path);

    cut_assert_equal_int(1, tar_state.number_of_files);
    cut_assert_equal_string("foo", tar_state.files_path[0]);
}

void test_optparse_create3(void)
{
    const char* argv[] = {"ptar", "c", "foo"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(true, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));

    cut_assert_equal_int(CREATE, tar_state.mode);
    cut_assert_equal_int(AUTOSUGGEST_FORMAT, tar_state.format);
    cut_assert_equal_int(AUTOSUGGEST_FILTER, tar_state.filter);
    cut_assert_equal_string_or_null(NULL, tar_state.archive_path);
    cut_assert_equal_string_or_null(NULL, tar_state.chdir);

    cut_assert_equal_int(1, tar_state.number_of_files);
    cut_assert_equal_string("foo", tar_state.files_path[0]);
}

void test_optparse_create4(void)
{
    const char* argv[] = {"ptar", "c", "-C", "bar", "foo"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(true, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));

    cut_assert_equal_int(CREATE, tar_state.mode);
    cut_assert_equal_int(AUTOSUGGEST_FORMAT, tar_state.format);
    cut_assert_equal_int(AUTOSUGGEST_FILTER, tar_state.filter);
    cut_assert_equal_string_or_null(NULL, tar_state.archive_path);
    cut_assert_equal_string_or_null("bar", tar_state.chdir);

    cut_assert_equal_int(1, tar_state.number_of_files);
    cut_assert_equal_string("foo", tar_state.files_path[0]);
}

void test_optparse_create5(void)
{
    const char* argv[] = {"ptar", "cfC", "bar", "foo", "aa", "bb"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(true, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));

    cut_assert_equal_int(CREATE, tar_state.mode);
    cut_assert_equal_int(AUTOSUGGEST_FORMAT, tar_state.format);
    cut_assert_equal_int(AUTOSUGGEST_FILTER, tar_state.filter);
    cut_assert_equal_string_or_null("bar", tar_state.archive_path);
    cut_assert_equal_string_or_null("foo", tar_state.chdir);

    cut_assert_equal_int(2, tar_state.number_of_files);
    cut_assert_equal_string("aa", tar_state.files_path[0]);
    cut_assert_equal_string("bb", tar_state.files_path[1]);
}

void test_optparse_create6(void)
{
    const char* argv[] = {"ptar", "cZf", "foo.tar.Z", "foo"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(true, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));

    cut_assert_equal_int(CREATE, tar_state.mode);
    cut_assert_equal_int(AUTOSUGGEST_FORMAT, tar_state.format);
    cut_assert_equal_int(ARCHIVE_FILTER_COMPRESS, tar_state.filter);
    cut_assert_equal_string("foo.tar.Z", tar_state.archive_path);

    cut_assert_equal_int(1, tar_state.number_of_files);
    cut_assert_equal_string("foo", tar_state.files_path[0]);
}

void test_optparse_create7(void)
{
    const char* argv[] = {"ptar", "cjf", "foo.tar.bz2", "foo"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(true, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));

    cut_assert_equal_int(CREATE, tar_state.mode);
    cut_assert_equal_int(AUTOSUGGEST_FORMAT, tar_state.format);
    cut_assert_equal_int(ARCHIVE_FILTER_BZIP2, tar_state.filter);
    cut_assert_equal_string("foo.tar.bz2", tar_state.archive_path);

    cut_assert_equal_int(1, tar_state.number_of_files);
    cut_assert_equal_string("foo", tar_state.files_path[0]);
}

void test_optparse_create8(void)
{
    const char* argv[] = {"ptar", "czf", "foo.tar.gz", "foo"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(true, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));

    cut_assert_equal_int(CREATE, tar_state.mode);
    cut_assert_equal_int(AUTOSUGGEST_FORMAT, tar_state.format);
    cut_assert_equal_int(ARCHIVE_FILTER_GZIP, tar_state.filter);
    cut_assert_equal_string("foo.tar.gz", tar_state.archive_path);

    cut_assert_equal_int(1, tar_state.number_of_files);
    cut_assert_equal_string("foo", tar_state.files_path[0]);
}

void test_optparse_create_fail1(void)
{
    const char* argv[] = {"ptar", "cfC", "bar", "foo"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(false, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));
}

void test_optparse_list(void)
{
    const char* argv[] = {"ptar", "tf", "foo.tar.Z", "foo"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(true, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));

    cut_assert_equal_int(LIST, tar_state.mode);
    cut_assert_equal_int(AUTOSUGGEST_FORMAT, tar_state.format);
    cut_assert_equal_int(AUTOSUGGEST_FILTER, tar_state.filter);
    cut_assert_equal_string("foo.tar.Z", tar_state.archive_path);

    cut_assert_equal_int(1, tar_state.number_of_files);
    cut_assert_equal_string("foo", tar_state.files_path[0]);
}

void test_optparse_extract(void)
{
    const char* argv[] = {"ptar", "xf", "foo.tar.Z", "foo"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(true, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));

    cut_assert_equal_int(EXTRACT, tar_state.mode);
    cut_assert_equal_int(AUTOSUGGEST_FORMAT, tar_state.format);
    cut_assert_equal_int(AUTOSUGGEST_FILTER, tar_state.filter);
    cut_assert_equal_string("foo.tar.Z", tar_state.archive_path);

    cut_assert_equal_int(1, tar_state.number_of_files);
    cut_assert_equal_string("foo", tar_state.files_path[0]);
}

void test_optparse_extract2(void)
{
    const char* argv[] = {"ptar", "xf", "foo.tar.Z"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(true, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));

    cut_assert_equal_int(EXTRACT, tar_state.mode);
    cut_assert_equal_int(AUTOSUGGEST_FORMAT, tar_state.format);
    cut_assert_equal_int(AUTOSUGGEST_FILTER, tar_state.filter);
    cut_assert_equal_string("foo.tar.Z", tar_state.archive_path);

    cut_assert_equal_int(0, tar_state.number_of_files);
}

void test_optparse_nomode_fail(void)
{
    const char* argv[] = {"ptar", "fC", "bar", "foo"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(false, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));
}


void test_optparse_help(void)
{
    const char* argv[] = {"ptar", "-h"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);

    cut_assert_equal_boolean(true, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));
    cut_assert_equal_boolean(true, tar_state.show_help);
}























