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

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <archive.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>

const char *filepath_suffix(const char *path)
{
    const char *basename = filepath_basename(path);
    const char *suffix = strrchr(basename, '.');
    if (suffix)
        return suffix+1;
    return NULL;
}

const char *filepath_suffix2(const char *path)
{
    const char *basename = filepath_basename(path);
    const char *suffix = strrchr(basename, '.');
    const char *p;

    if (suffix == NULL)
        return NULL;

    for (p = suffix-1; p >= basename; p--) {
        if (*p == '.')
            return p+1;
    }

    return suffix+1;
}

const char *filepath_basename(const char *path)
{
    const char *slash = strrchr(path, '/');
    if (slash)
        return slash+1;
    return path;
}

int filepath_suggest_filter(const char *path)
{
    const char *suffix = filepath_suffix(path);

    if (strcmp(suffix, "gz") == 0 || strcmp(suffix, "tgz") == 0) {
        return ARCHIVE_FILTER_GZIP;
    } else if (strcmp(suffix, "bz2") == 0 || strcmp(suffix, "tbz") == 0) {
        return ARCHIVE_FILTER_BZIP2;
    } else if (strcmp(suffix, "lzma") == 0) {
        return ARCHIVE_FILTER_LZMA;
    } else if (strcmp(suffix, "xz") == 0) {
        return ARCHIVE_FILTER_XZ;
    } else if (strcmp(suffix, "Z") == 0 || strcmp(suffix, "taz") == 0) {
        return ARCHIVE_FILTER_COMPRESS;
    } else if (strcmp(suffix, "lz") == 0) {
        return ARCHIVE_FILTER_LZIP;
    } else if (strcmp(suffix, "lzo") == 0 || strcmp(suffix, "lzop") == 0) {
        return ARCHIVE_FILTER_LZOP;
    } else if (strcmp(suffix, "lrz") == 0) {
        return ARCHIVE_FILTER_LRZIP;
    } else if (strcmp(suffix, "grz") == 0) {
        return ARCHIVE_FILTER_GRZIP;
    } else {
        return ARCHIVE_FILTER_NONE;
    }
}

static int _filepath_suggest_format_helper(const char *suffix)
{
    if (strcmp(suffix, "7z") == 0)
        return ARCHIVE_FORMAT_7ZIP;
    if (strcmp(suffix, "cpio") == 0)
        return ARCHIVE_FORMAT_CPIO_POSIX;
    if (strcmp(suffix, "shar") == 0)
        return ARCHIVE_FORMAT_SHAR_BASE;
    if (strcmp(suffix, "tar") == 0 || strcmp(suffix, "tgz") == 0 || strcmp(suffix, "tbz") == 0 || strcmp(suffix, "taz") == 0)
        return ARCHIVE_FORMAT_TAR_PAX_RESTRICTED;
    if (strcmp(suffix, "iso") == 0)
        return ARCHIVE_FORMAT_ISO9660;
    if (strcmp(suffix, "zip") == 0)
        return ARCHIVE_FORMAT_ZIP;
    if (strcmp(suffix, "a") == 0)
        return ARCHIVE_FORMAT_AR_BSD;
    if (strcmp(suffix, "lha") == 0 || strcmp(suffix, "lzh") == 0)
        return ARCHIVE_FORMAT_LHA;
    if (strcmp(suffix, "xar") == 0)
        return ARCHIVE_FORMAT_XAR;
    if (strcmp(suffix, "cab") == 0)
        return ARCHIVE_FORMAT_CAB;
    if (strcmp(suffix, "rar") == 0)
        return ARCHIVE_FORMAT_RAR;
    return 0;
}

int filepath_suggest_format(const char *path)
{
    const char *suffix = filepath_suffix(path);

    int retvalue = _filepath_suggest_format_helper(suffix);
    if (retvalue)
        return retvalue;

    const char *suffix2_tmp = filepath_suffix2(path);
    const char *has_double_suffix = strchr(suffix2_tmp, '.');
    retvalue = ARCHIVE_FORMAT_TAR_PAX_RESTRICTED;

    if (has_double_suffix) {
        char *suffix2 = strndup(suffix2_tmp, has_double_suffix - suffix2_tmp);
        retvalue = _filepath_suggest_format_helper(suffix2);
        free(suffix2);
    }

    return retvalue;
}

int copyfile(const char *destpath, const char *srcpath)
{
    int src_fd = open(srcpath, O_RDONLY);
    if (src_fd < 0)
        return src_fd;
    
    int dest_fd = open(destpath, O_WRONLY|O_CREAT|O_TRUNC, 0600);
    if (dest_fd < 0)
        return src_fd;

    char buffer[1024*100];
    ssize_t ssize;

    while ((ssize = read(src_fd, buffer, sizeof(buffer))) > 0) {
        ssize_t wrote_size = write(dest_fd, buffer, ssize);
        if (wrote_size < 0) {
            perror("Failed to copy file");
            return wrote_size;
        }
    }

    if (ssize < 0) {
        perror("Failed to copy file");
        return ssize;
    }
    
    return 0;
}
