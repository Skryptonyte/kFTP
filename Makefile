obj-m += kftp.o
kftp-objs+= kftp_mod.o ftp_serv.o ftp_utils.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean