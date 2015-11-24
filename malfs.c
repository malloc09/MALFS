#define FUSE_USE_VERSION 26

#include "malfs.h"
#include <stdio.h>
#include <fuse.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

#define MALFS_FILE 0
#define MALFS_DIR 1

static int malfs_getattr(const char* path,struct stat* st){
 return 0;
}
static int malfs_mkdir(const char* path,mode_t mode){
 return 0;
}
static int malfs_rmdir(const char* path){
 return 0;
}
static int malfs_truncate(const char* path,off_t size){
 return 0;
}
static int malfs_open(const char* path,struct fuse_file_info* fi){
	open();
	printf("hello world");
	return 0;
}
static int malfs_read(const char* path,char* buf,size_t size,off_t offset,struct fuse_file_info* fi){
 return 0;
}
static int malfs_write(const char* path,const char* buf,size_t size,off_t offset,struct fuse_file_info* fi){
 return 0;
}
static int malfs_opendir(const char* path,struct fuse_file_info* fi){
 return 0;
}
static int malfs_readdir(const char* path,void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi){
 return 0;
}
static int malfs_create(const char* path,mode_t mode,struct fuse_file_info* fi){
 return 0;
}
static int malfs_utimens(const char* path, const struct timespec tv[2]){
 return 0;
}
static int malfs_unlink(const char* path){
 return 0;
}
static int malfs_release(const char* path,struct fuse_file_info* fi){
	return 0;
}

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
	
};

int main(int argc, char *argv[]){
	return fuse_main(argc, argv, &malfs_oper, NULL);
}
