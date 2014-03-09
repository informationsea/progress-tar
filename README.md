Progress Tar
============

Implemented Options
-------------------

### Mode

* `c`: create
* `t`: list
* `x`: extract

### Options

* `C`: change directory
* `z`: gzip filter
* `Z`: compress filter
* `j`: bzip2 filter
* `J`: xz filter
* `a`: auto suggest a file format and a filter from file name

Install
-------

1. Install `libarchive`, `automake`, `autoconf`, compiler and make
2. Run `./autogen.sh`
3. Run `./configure`
4. Run `make`
5. Run `make install` as root user

This software is being developed on OS X Mavericks and MacPorts. This
software may work with other UNIX based systems, but not tested.

License
-------

Copyright (C) 2014 Yasunobu OKAMURA

GPL version 3 or later
