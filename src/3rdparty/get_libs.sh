#!/bin/bash
set -xe

ZLIB_NG_VERSION="2.3.3"
LIBPNG_VERSION="1.6.58"

cd $(dirname $0)
rm -rf zlib/src libpng/src

# get zlib-ng
curl -Lo zlib.zip "https://github.com/zlib-ng/zlib-ng/archive/refs/tags/${ZLIB_NG_VERSION}.zip"
unzip zlib.zip -d zlib
rm zlib.zip
mv zlib/zlib-ng-${ZLIB_NG_VERSION} zlib/src

# get libpng
curl -Lo libpng.tar.xz "https://download.sourceforge.net/project/libpng/libpng16/${LIBPNG_VERSION}/libpng-${LIBPNG_VERSION}.tar.xz"
tar xf libpng.tar.xz
rm libpng.tar.xz
mv libpng-${LIBPNG_VERSION} libpng/src

# get and apply apng patch
curl -Lo libpng-apng.patch.gz "https://download.sourceforge.net/project/apng/libpng/libpng16/libpng-${LIBPNG_VERSION}-apng.patch.gz"
gunzip libpng-apng.patch.gz
tr -d '\r' < libpng-apng.patch > tmpfile && mv tmpfile libpng-apng.patch
pushd libpng/src
patch -Np0 -i "../../libpng-apng.patch"
patch -Np1 -i "../fix-cmake-policy.patch"
cp scripts/pnglibconf.h.prebuilt pnglibconf.h
popd
rm libpng-apng.patch
