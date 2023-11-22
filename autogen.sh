#!/usr/bin/env bash

ACLOCAL=$(command -v aclocal)
AUTOCONF=$(command -v autoconf)
AUTOHEADER=$(command -v autoheader)
AUTOMAKE=$(command -v automake)
AUTORECONF=$(command -v autoreconf)

MISSING=""

if [ -z "$ACLOCAL" ]; then
    MISSING="$MISSING aclocal"
fi

if [ -z "$AUTOCONF" ]; then
    MISSING="$MISSING autoconf"
fi

if [ -z "$AUTOHEADER" ]; then
    MISSING="$MISSING autoheader"
fi

if [ -z "$AUTOMAKE" ]; then
    MISSING="$MISSING automake"
fi

if [ -z "$AUTORECONF" ]; then
    MISSING="$MISSING autoreconf"
fi

if [ -n "$MISSING" ]; then
    echo "Missing build dependencies: $MISSING"
    exit 1
fi

set -e

echo "Running aclocal..."
$ACLOCAL

echo "Running autoconf..."
$AUTOCONF

echo "Running autoheader..."
$AUTOHEADER

echo "Running automake..."
$AUTOMAKE --add-missing

echo "Now you can run ./configure"
