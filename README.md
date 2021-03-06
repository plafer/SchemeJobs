## Overview
SchemeJobs is a toy Scheme interpreter written in C.

## Building and running

    $ make && sudo make install
    $ schemejobs [-i init_file_path]

## Running tests

    $ make testsuite

## Highlights / Shortcomings
+ Only runs on POSIX-compliant operating systems (e.g. Linux, the BSDs, etc.)
+ Init file written in Scheme that defines standard Scheme procedures
+ Symbols cannot contain numbers (e.g. `fn1` is an invalid symbol)

## Upcoming Features
+ Variable arguments (varargs)
+ Meaningful error messages
+ Tail call optimization
+ Mark and sweep GC
+ Macro system

## C coding conventions
+ Functions return 0 on success and an errno value on error. The documentation
should clearly explained what errno value is to be expected and exactly what it
means in the context of the function.
      + If a function is guaranteed to succeed, it is ok to make it void.
      + Out parameters should be used to return actual data.
+ Don't typedef structs.
