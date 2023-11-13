README
======

This package includes the following files.

|-- Coordinator.c [Coordinates fork of Checker.c, creates shared a pipe and shared memory, waits for execution, reads from shared memory.]
|-- Checker.c [Checks if parameters are divisible, write that result to shared memory, exit successfully.]
|-- Makefile [Make file which includes a make for Coordinator.c and Checker.c and a clean]
|-- README.txt [This file]

To compile:
    make all
