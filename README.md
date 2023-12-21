# kFTP

A very WIP project that aims to run a function FTP server in the kernel.

## What works

* USER, PASS, TYPE, PASV, RETR, QUIT commands recognized
* Passive data transfer using PASV and RETR supported!

## TODO

* Implement actual filesystem access
* Active connection support
* Multithreaded client support
* Proper clean up of resources
* Fixes for millions of potential UAFs and race conditions

## Compile

```
make
```