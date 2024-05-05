#!/bin/bash
set -e

ZLIB_VERSION="1.3.1"
LIBPNG_VERSION="1.6.43"

cd $(dirname $0)
rm -rf zlib/src libpng/src

# get zlib
curl -Lo zlib.zip "https://github.com/madler/zlib/archive/refs/tags/v${ZLIB_VERSION}.zip"
unzip zlib.zip -d zlib
rm zlib.zip
mv zlib/zlib-${ZLIB_VERSION} zlib/src

# get libpng
curl -Lo libpng.tar.xz "https://download.sourceforge.net/project/libpng/libpng16/${LIBPNG_VERSION}/libpng-${LIBPNG_VERSION}.tar.xz"
tar xf libpng.tar.xz
rm libpng.tar.xz
mv libpng-${LIBPNG_VERSION} libpng/src

# get and apply apng patch
curl -Lo libpng-apng.patch.gz "https://download.sourceforge.net/project/apng/libpng/libpng16/libpng-${LIBPNG_VERSION}-apng.patch.gz"
gunzip libpng-apng.patch.gz
pushd libpng/src
patch -Np0 -i "../../libpng-apng.patch"
#patch -Np1 -i "../fix-cmake-policy.patch"
cp scripts/pnglibconf.h.prebuilt pnglibconf.h
popd
rm libpng-apng.patch
