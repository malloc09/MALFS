#define FUSE_USE_VERSION 26

#include "malfs.h"
#include <stdio.h>
#include <fuse.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

struct malfs_file* root;

static int malfs_getattr(const char* path,struct stat* st){
 	int res=0;
	//for test
	if(strcmp(path,"/")==0)
		*st=root->stat;
	return 0;
}
//make a file structure and assign initial value
static malfs_filep create_malfs(mode_t mode,int type){
	malfs_filep res=(malfs_filep)calloc(1,sizeof(malfs_file));
		if(type==MAL_DIR){
			res->type=MAL_DIR;
			res->stat.st_mode=S_IFDIR | mode;
			res->stat.st_nlink=0;
		}
		else{
			res->type=MAL_FILE;
			res->stat.st_mode=S_IFREG | mode;
			res->stat.st_nlink=0;
		}
	return res;
}
static int malfs_mkdir(const char* path,mode_t mode){
 	int res=0;
	//if there is no root
	if(strcmp(path,"/")==0 && root==NULL){
		root=create_malfs(mode,MAL_DIR);
	}
	return res;
}
static int malfs_rmdir(const char* path){
 return 0;
}
static int malfs_truncate(const char* path,off_t size){
 return 0;
}
static int malfs_open(const char* path,struct fuse_file_info* fi){
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
 	filler(buf,".",NULL,0);
	filler(buf,"..",NULL,0);
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
static void* init(struct fuse_conn_info *conn){
}
int main(int argc, char *argv[]){

	malfs_mkdir("/",0755);//make a root dir
	return fuse_main(argc, argv, &malfs_oper, NULL);
}






