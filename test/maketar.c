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
#include <unistd.h>
#include <gcutter.h>
#include <sys/stat.h>

#include "progresstar.h"
#include "optparse.h"
#include "fileutil.h"

#define _STR(x) # x
#define STR(x) _STR(x)

const char *tmpdir = "tmp.progresstar.test";
const char *exrdir = "tmp.progresstar.exr";
const char *refdir = "tmp.progresstar.ref";

void cut_setup(void)
{
    char buf[1024];

    mkdir(tmpdir, 0700);
    mkdir(exrdir, 0700);
    mkdir(refdir, 0700);

    snprintf(buf, sizeof(buf)-1, "tar xzf testtar.tar.gz -C %s", refdir);
    cut_assert_equal_int(0, system(buf));

}

void cut_teardown(void)
{
    char buf[10240];
    snprintf(buf, sizeof(buf)-1, "rm -rf %s %s %s", tmpdir, exrdir, refdir);
    //cut_assert_equal_int(0, system(buf));
}

void test_copyfile(void)
{
    const char *destpath = cut_build_path(tmpdir, "testtar.tar.gz", NULL);

    copyfile(destpath, "testtar.tar.gz");
    cut_assert_equal_file_raw("testtar.tar.gz", destpath);
}

void test_maketar1(void)
{
    const char *tarpath = cut_build_fixture_path(tmpdir, "foo.tar.bz2", NULL);
    const char *reftarpath = cut_build_fixture_path("testtar.tar.gz", NULL);
    const char *comparezip = cut_build_fixture_path("compare-zip.py", NULL);
    
    system(cut_take_printf("ls -la %s", cut_build_fixture_path(refdir, NULL)));
    const char * argv[] = {"ptar", "cjf", tarpath,
                           "-C", cut_build_fixture_path(refdir, NULL), "testtar"};
    struct progress_tar tar_state;
    progress_initialize(&tar_state);
    cut_assert_equal_boolean(true, ptar_optparse(&tar_state, sizeof(argv)/sizeof(argv[0]), argv));
    cut_assert_equal_int(0, progress_run(&tar_state));

    cut_assert_equal_int(0, system(cut_take_printf("%s %s %s", comparezip, reftarpath, tarpath)));
}
