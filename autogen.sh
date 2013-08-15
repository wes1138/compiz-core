#!/bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

ORIGDIR=`pwd`
cd $srcdir

autoreconf -v --install || exit 1
glib-gettextize --copy --force || exit 1
intltoolize --copy --force --automake || exit 1

cd $ORIGDIR || exit $?

./gen-cmd-metadata.sh $NUM_COMMANDS

$srcdir/configure --enable-maintainer-mode "$@"
