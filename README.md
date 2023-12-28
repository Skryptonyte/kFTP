# kFTP

A very WIP project that aims to run a function FTP server in the kernel.

## Features

* USER, PASS, TYPE, PASV, RETR, QUIT commands recognized
* Passive data transfer of any file in the designated FTP directory using PASV and RETR supported!
* Mount namespacing to fully isolate FTP directories from path traversal escapes and unpermitted accesses using the helper userspace binary run_server.

## TODO

* More essential commands like LIST, PORT etc.
* Multithreaded or event based client loop support, whichever comes first.
* Proper clean up of resources.
* Fixes for millions of potential UAFs and race conditions.

## Compile and run

```
make
sudo ./run_server <your-ftp-directory>
```

Server runs at port 2121.

Note: You can also insmod kftp.ko directly but you lose the benefit of having an isolated mount namespace.