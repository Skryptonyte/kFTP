obj-m += kftp.o
kftp-objs+= kftp_mod.o ftp_serv.o ftp_utils.o ftp_list.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc run_server.c -o run_server
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
