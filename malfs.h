#ifndef __MAL__FS__
#define __MAL__FS__

#include <fuse.h>
#define MAL_FILE 1
#define MAL_DIR 0

typedef struct malfs_file{
	char* name;
	char type;
	struct stat stat;
	struct malfs_file* parent;
	struct malfs_file* next;
	struct malfs_file* children_head;
	unsigned int num_of_children;

	char* data;
}malfs_file;
typedef struct malfs_file* malfs_filep;

static int malfs_getattr(const char* path,struct stat* st);
static int malfs_mkdir(const char* path,mode_t mode);
static int malfs_rmdir(const char* path);
static int malfs_truncate(const char* path,off_t size);
static int malfs_open(const char* path,struct fuse_file_info* fi);
static int malfs_read(const char* path,char* buf,size_t size,off_t offset,struct fuse_file_info* fi);
static int malfs_write(const char* path,const char* buf,size_t size,off_t offset,struct fuse_file_info* fi);
static int malfs_opendir(const char* path,struct fuse_file_info* fi);
static int malfs_readdir(const char* path,void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi);
static int malfs_create(const char* path,mode_t mode,struct fuse_file_info* fi);
static int malfs_utimens(const char* path, const struct timespec tv[2]);
static int malfs_unlink(const char* path);
static int malfs_release(const char* path,struct fuse_file_info* fi);
static int malfs_chmod(const char* path,mode_t mode);
static int malfs_chown(const char* path,uid_t uid,gid_t gid);
static int malfs_rename(const char* from, const char* to);
static void* malfs_init(struct fuse_conn_info* conn);

#endif
