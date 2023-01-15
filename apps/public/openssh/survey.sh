#!/bin/sh
#
# Copyright (c) 2004, 2005 Darren Tucker
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

host="mips-unknown-linux-gnu"
AWK="gawk"
CC="mips-unknown-linux-uclibc-gcc"
CPP="/opt/toolchains/crosstools-mips-gcc-4.6-linux-3.4-uclibc-0.9.32-binutils-2.21/usr/bin/mips-unknown-linux-uclibc-cpp"
CFLAGS="-g -O2 -Wall -Wpointer-arith -Wuninitialized -Wsign-compare -Wformat-security -Wno-pointer-sign -Wno-unused-result -fno-strict-aliasing -D_FORTIFY_SOURCE=2 -fno-builtin-memset "
CPPFLAGS="-I/home/tplink/VG5612_ALGAR/linux_bcm_UIv1/build/../apps/public/openssl/include -I/home/tplink/VG5612_ALGAR/linux_bcm_UIv1/build/../apps/public/zlib-1.2.6 "
LDFLAGS="-L/home/tplink/VG5612_ALGAR/linux_bcm_UIv1/build/../apps/public/openssl/lib -L/home/tplink/VG5612_ALGAR/linux_bcm_UIv1/build/../apps/public/zlib-1.2.6 "
LIBS="-lcrypto -ldl -lutil -lz  -lcrypt"

# Note format:
# identifier: [data] CRCR

echo "openssh-survey-version: 1"
echo
echo "openssh-version: `./ssh -V 2>&1`"
echo
configinv=`$AWK '/^  \\\$.*configure/' config.log | sed 's/^  \\\$ //g'`
echo "configure-invocation: $configinv"
echo
echo "host: $host"
echo
echo "uname: `uname`"
echo
echo "uname-r: `uname -r`"
echo
echo "uname-m: `uname -m`"
echo
echo "uname-p: `uname -p`"
echo
echo "oslevel: `oslevel 2>/dev/null`"
echo
echo "oslevel-r: `oslevel -r 2>/dev/null`"
echo
echo "cc: $CC"
echo
echo "cflags: $CFLAGS"
echo
echo "cppflags: $CPPFLAGS"
echo
echo "ldflags: $LDFLAGS"
echo
echo "libs: $LIBS"
echo
echo "ccver-v: `$CC -v 2>&1 | sed '/^[ \t]*$/d'`"
echo
echo "ccver-V: `$CC -V 2>&1 | sed '/^[ \t]*$/d'`"
echo
echo "cppdefines:"
${CPP} -dM - </dev/null
echo
echo "config.h:"
egrep '#define|#undef' config.h
echo
