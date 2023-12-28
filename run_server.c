#define _GNU_SOURCE
#include <sched.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

void sig_handler(int signum){
	if (syscall(SYS_delete_module,"kftp",0) != -1)
	{
		printf("kftp module unloaded.\n");
	}
	exit(0);
}
int main(int argc, char** argv){
	char* new_root;
	char old_root_path[256];
	int module_fd = open("./kftp.ko",0);
	
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	
	if (argc < 2){
		printf("Usage: ./run_server <ftp-file-directory>\n");
		exit(-1);
	}
	
	new_root = argv[1];
	unshare(CLONE_NEWNS);
	
	if (module_fd == -1){
		printf("Failed to open kernel module\n");
		exit(-1);
	}
	printf("Kernel module kftp.ko is present.\n");
	
	// Make current root mount private
	mount(NULL,"/",NULL,MS_REC|MS_PRIVATE,NULL);
	// Mount target directory into itself as bind mount.
	mount(new_root,new_root,NULL,MS_BIND,NULL);
	
	sprintf(old_root_path, "%s/%s",new_root,"old_root");
	mkdir(old_root_path,0755);
	
	if (syscall(SYS_pivot_root, new_root, old_root_path) == -1){
		printf("Pivot root failed. Can't set up mount namespace\n");
		exit(-1);
	}
	
	
	chdir("/");
	
	// Discard our old root
	if (umount2("/old_root", MNT_DETACH) == -1){
		printf("Old root unmount failed!\n");
		exit(-1);
	}
	if (rmdir("/old_root") == -1){
		printf("Old root directory delete failed!\n");
		exit(-1);
	}
	
	
	printf("Mount namespace set up successfully.\n");
	printf("Loading kftp module.\n");
	
	if (syscall(SYS_finit_module,module_fd,"",0) == -1){
		printf("Failed to load kernel module\n");
		exit(-1);
	}
}
