#!/usr/bin/env python

#  Progress tar
#  Copyright (C) 2014 Yasunobu OKAMURA
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

import argparse
import zipfile
import tarfile
import hashlib
import sys

def _main():
    parser = argparse.ArgumentParser(description="Compare zip or tar file contents")
    parser.add_argument('file', nargs=2)
    options = parser.parse_args()
    
    contents = [dict(), dict()]

    for i, path in enumerate(options.file):
        if path.endswith('.zip'): # zip mode
            pass
        else: # tar mode
            t = tarfile.open(path)
            for f in t:
                contents[i][f.name] = {
                    'name': f.name,
                    'size': f.size,
#                    'mtime': f.mtime,
                    'mode': f.mode,
                    'type': f.type,
                    'linkname': f.linkname,
#                    'uid': f.uid,
#                    'gid': f.gid,
#                    'uname': f.uname,
#                    'gname': f.gname
                }

                if f.isfile():
                    fo = t.extractfile(f)
                    h = hashlib.new('sha1')
                    h.update(fo.read())
                    contents[i][f.name]['content'] = h.hexdigest()

                #print contents[i][f.name]

    filelist = [set(x.keys()) for x in contents]
    commonfiles = filelist[0] & filelist[1]
    difffiles = [filelist[0] - filelist[1], filelist[1] - filelist[0]];

    status = 0
    
    for one in commonfiles:
        if contents[0][one] != contents[1][one]:
            print 'DIFF', contents[0][one], contents[1][one]
            status = 1

    for one in difffiles[0]:
        print 'DIFF', contents[0][one], None
        status = 1
        
    for one in difffiles[1]:
        print 'DIFF', None, contents[1][one]
        status = 1

    exit(status)
    
if __name__ == '__main__':
    _main()
