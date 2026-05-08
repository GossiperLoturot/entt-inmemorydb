#!/bin/bash

CODEGEN_DIR=codegen

if [ -d $CODEGEN_DIR ]; then
  rm -rf $CODEGEN_DIR
  mkdir $CODEGEN_DIR
else
  mkdir $CODEGEN_DIR
fi

protoc \
 --proto_path=protos \
 --grpc_out=${CODEGEN_DIR} --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./protos/*.proto

protoc \
 --proto_path=protos \
 --cpp_out=${CODEGEN_DIR} protos/*.proto

BUILD_DIR=./build

if [ -d $BUILD_DIR ]; then
  rm -rf $BUILD_DIR
  mkdir $BUILD_DIR
else
  mkdir $BUILD_DIR
fi

cd build
cmake ..
make
