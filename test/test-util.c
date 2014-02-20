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

#include "fileutil.h"
#include <cutter.h>
#include <archive.h>

void test_basename(void)
{
    const char *path = "/home/foo/bar.c";
    cut_assert_equal_string("bar.c", filepath_basename(path));

    const char *path2 = "test.h";
    cut_assert_equal_string("test.h", filepath_basename(path2));

    const char *path3 = "/home/foo/";
    cut_assert_equal_string("", filepath_basename(path3));

}

void test_suffix(void)
{
    const char *path = "/home/foo/bar.c";
    cut_assert_equal_string("c", filepath_suffix(path));

    const char *path2 = "test.h";
    cut_assert_equal_string("h", filepath_suffix(path2));

    cut_assert_equal_string_or_null(NULL, filepath_suffix("test"));
    cut_assert_equal_string_or_null(NULL, filepath_suffix(""));
}


void test_suffix2(void)
{
    const char *path = "/home/foo/bar.c";
    cut_assert_equal_string("c", filepath_suffix2(path));

    const char *path2 = "test.h";
    cut_assert_equal_string("h", filepath_suffix2(path2));

    cut_assert_equal_string_or_null(NULL, filepath_suffix2("test"));
    cut_assert_equal_string_or_null(NULL, filepath_suffix2(""));

    cut_assert_equal_string_or_null("tar.bz2", filepath_suffix2("test.tar.bz2"));
    cut_assert_equal_string_or_null("tar.xz", filepath_suffix2("test.tar.xz"));
}

void test_suggest_format(void)
{
    cut_assert_equal_int(ARCHIVE_FORMAT_7ZIP, filepath_suggest_format("test.7z"));
    cut_assert_equal_int(ARCHIVE_FORMAT_ZIP, filepath_suggest_format("foo.zip"));
    cut_assert_equal_int(ARCHIVE_FORMAT_CPIO_POSIX, filepath_suggest_format("test.cpio.bz2"));
    cut_assert_equal_int(ARCHIVE_FORMAT_CPIO_POSIX, filepath_suggest_format("te.st.cpio"));
    cut_assert_equal_int(ARCHIVE_FORMAT_SHAR_BASE, filepath_suggest_format("test.shar.gz"));
    cut_assert_equal_int(ARCHIVE_FORMAT_TAR_PAX_RESTRICTED, filepath_suggest_format("te.st.tar"));
    cut_assert_equal_int(ARCHIVE_FORMAT_TAR_PAX_RESTRICTED, filepath_suggest_format("test.tar.gz"));
    cut_assert_equal_int(ARCHIVE_FORMAT_TAR_PAX_RESTRICTED, filepath_suggest_format("test.tgz"));
    cut_assert_equal_int(ARCHIVE_FORMAT_TAR_PAX_RESTRICTED, filepath_suggest_format("test.tbz"));
    cut_assert_equal_int(ARCHIVE_FORMAT_TAR_PAX_RESTRICTED, filepath_suggest_format("test.taz"));
    cut_assert_equal_int(ARCHIVE_FORMAT_ISO9660, filepath_suggest_format("test.iso"));
    cut_assert_equal_int(ARCHIVE_FORMAT_ISO9660, filepath_suggest_format("test.iso.gz"));
    cut_assert_equal_int(ARCHIVE_FORMAT_ISO9660, filepath_suggest_format("test.tar.iso"));
    cut_assert_equal_int(ARCHIVE_FORMAT_AR_BSD, filepath_suggest_format("test.a"));
    cut_assert_equal_int(ARCHIVE_FORMAT_LHA, filepath_suggest_format("test.lzh"));
    cut_assert_equal_int(ARCHIVE_FORMAT_LHA, filepath_suggest_format("test.lha"));
    cut_assert_equal_int(ARCHIVE_FORMAT_XAR, filepath_suggest_format("test.xar"));
    cut_assert_equal_int(ARCHIVE_FORMAT_CAB, filepath_suggest_format("test.cab"));
    cut_assert_equal_int(ARCHIVE_FORMAT_RAR, filepath_suggest_format("test.rar"));

    cut_assert_equal_int(ARCHIVE_FORMAT_TAR_PAX_RESTRICTED, filepath_suggest_format("foo.bar"));
}

void test_suggest_filter(void)
{
    cut_assert_equal_int(ARCHIVE_FILTER_NONE, filepath_suggest_filter("test.7z"));
    cut_assert_equal_int(ARCHIVE_FILTER_NONE, filepath_suggest_filter("test.zip"));
    cut_assert_equal_int(ARCHIVE_FILTER_NONE, filepath_suggest_filter("test.cab"));
    cut_assert_equal_int(ARCHIVE_FILTER_NONE, filepath_suggest_filter("test.rar"));
    cut_assert_equal_int(ARCHIVE_FILTER_GZIP, filepath_suggest_filter("test.tar.gz"));
    cut_assert_equal_int(ARCHIVE_FILTER_BZIP2, filepath_suggest_filter("test.cpio.bz2"));
    cut_assert_equal_int(ARCHIVE_FILTER_XZ, filepath_suggest_filter("test.tar.xz"));
    cut_assert_equal_int(ARCHIVE_FILTER_LZMA, filepath_suggest_filter("test.tar.lzma"));
    cut_assert_equal_int(ARCHIVE_FILTER_COMPRESS, filepath_suggest_filter("test.tar.Z"));
    cut_assert_equal_int(ARCHIVE_FILTER_LZIP, filepath_suggest_filter("test.tar.lz"));
    cut_assert_equal_int(ARCHIVE_FILTER_LRZIP, filepath_suggest_filter("test.tar.lrz"));
    cut_assert_equal_int(ARCHIVE_FILTER_GRZIP, filepath_suggest_filter("test.tar.grz"));
    cut_assert_equal_int(ARCHIVE_FILTER_LZOP, filepath_suggest_filter("test.tar.lzo"));
    cut_assert_equal_int(ARCHIVE_FILTER_LZOP, filepath_suggest_filter("test.tar.lzop"));
    cut_assert_equal_int(ARCHIVE_FILTER_COMPRESS, filepath_suggest_filter("test.taz"));
    cut_assert_equal_int(ARCHIVE_FILTER_GZIP, filepath_suggest_filter("test.tgz"));
    cut_assert_equal_int(ARCHIVE_FILTER_BZIP2, filepath_suggest_filter("test.tbz"));
    
}
