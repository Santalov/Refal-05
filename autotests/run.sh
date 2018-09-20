#!/bin/bash

run_test_aux() {
  echo Passing $1...
  REF=$1
  CPP=${REF%%.ref}.cpp
  EXE=${REF%%.ref}

  ../src/refal05c $REF 2>__error.txt
  if [ $? -gt 0 ]; then
    echo COMPILER ON $REF FAILS, SEE __error.txt
    exit
  fi
  rm __error.txt
  if [ ! -e $CPP ]; then
    echo COMPILATION FAILED
    exit
  fi

  g++ -I../lib -DDUMP_FILE=\"dump.txt\" -DDONT_PRINT_STATISTICS \
    -o$EXE $CPP ../lib/refalrts.cpp
  if [ $? -gt 0 ]; then
    echo COMPILATION FAILED
    exit
  fi

  ./$EXE
  if [ $? -gt 0 ]; then
    echo TEST FAILED, SEE dump.txt
    exit
  fi

  rm $CPP $EXE
  [ -e dump.txt ] && rm dump.txt

  echo
}

run_test_aux.BAD-SYNTAX() {
  echo Passing $1...
  REF=$1
  CPP=${REF%%.ref}.cpp
  EXE=${REF%%.ref}

  ../src/refal05c $REF 2>__error.txt
  if [ $? -gt 0 ]; then
    echo COMPILER ON $REF FAILS, SEE __error.txt
    exit
  fi
  rm __error.txt
  if [ -e $CPP ]; then
    echo COMPILATION SUCCESSED, BUT EXPECTED SYNTAX ERROR
    rm $CPP
    exit
  fi

  echo "Ok! Compiler didn't crash on invalid syntax"
  echo
}

run_test() {
  REF=$1
  SUFFIX=`echo ${REF%%.ref} | sed 's/[^.]*\(\.[^.]*\)*/\1/'`
  run_test_aux$SUFFIX $1
}

if [ -z "$1" ]; then
  for s in *.ref; do
    run_test $s
  done
else
  for s in $*; do
    run_test $s
  done
fi
