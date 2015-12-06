#define FUSE_USE_VERSION 26


#include <fuse.h>
#include <fcntl.h>
#include "malfs.h"

static struct fuse_operations malfs_oper = {
	.getattr=malfs_getattr,
	.mkdir=malfs_mkdir,
	.rmdir=malfs_rmdir,
	.truncate=malfs_truncate,
	.open=malfs_open,
	.read=malfs_read,
	.write=malfs_write,
	.opendir=malfs_opendir,
	.readdir=malfs_readdir,
	.create=malfs_create,
	.utimens=malfs_utimens,
	.unlink=malfs_unlink,
	.release=malfs_release,
	.chmod=malfs_chmod,
	.chown=malfs_chown,
	.init=malfs_init,

};

int main(int argc, char *argv[]){

	malfs_mkdir("/",0777);//make a root dir
	return fuse_main(argc, argv, &malfs_oper, NULL);
}


