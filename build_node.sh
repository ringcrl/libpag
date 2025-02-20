#!/bin/bash -e

function make_dir() {
  rm -rf $1
  mkdir -p $1
}

if [[ $(uname) == 'Darwin' ]]; then
  BUILD_DIR="mac_out"
  PLATFORM="mac"
elif [[ $(uname) == 'Linux' ]]; then
  BUILD_DIR="linux_out"
  PLATFORM="linux"
fi

make_dir $BUILD_DIR

cd $BUILD_DIR

cmake -DPAG_USE_SWIFTSHADER=ON -DPAG_BUILD_SHARED=ON -DCMAKE_BUILD_TYPE=Release ../

RETURN_CODE=$?
if [ "$RETURN_CODE" -eq "0" ]
then
  echo "cmake success"
else
  echo "cmake failed"
  exit 1
fi

cmake --build . --target pag -- -j 12
RETURN_CODE=$?
if [ "$RETURN_CODE" -eq "0" ]
then
  echo "compile success"
else
  echo "compile failed"
  exit 1
fi

cd ..

make_dir node/vendor/pag/include
cp -a include/* node/vendor/pag/include

make_dir node/vendor/pag/$PLATFORM/x64
cp -a $BUILD_DIR/libpag.so node/vendor/pag/$PLATFORM/x64

make_dir node/vendor/swiftshader
cp -a vendor/swiftshader/* node/vendor/swiftshader

rm -rf $BUILD_DIR
