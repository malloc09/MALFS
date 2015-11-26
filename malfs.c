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


//find the file or directory which has given path
static malfs_filep find_malfs(const char* path){
	malfs_filep temp=root;
	malfs_filep child,res;
	char* tpath;
	char* name;
	int i,isfound;
	tpath=(char*)malloc(sizeof(char)*(strlen(path)+1));
	strcpy(tpath,path);
	if(strcmp(tpath,"/")==0)
		return root;
	else{
		name=strtok(tpath,"/");
		while(temp && name){
			isfound=0;
			child=temp->children_head;
			for(i=0;i<temp->num_of_children;i++){
				if(strcmp(child->name,name)==0){
					isfound=1;
					temp=child;
					break;
				}
				child=child->next;
			}
			if(!isfound)
				temp=temp->next;
			else
			name=strtok(NULL,"/");
		}
		free(tpath);
		return temp;
	}

}

//make a file structure and assign initial value
static malfs_filep create_malfs(char* name,mode_t mode,int type){
	malfs_filep res=(malfs_filep)calloc(1,sizeof(malfs_file));
	struct fuse_context* conn=fuse_get_context();
	if(type==MAL_DIR){
		res->name=(char*)malloc(strlen(name)+1);
		strcpy(res->name,name);
		res->type=MAL_DIR;
		res->stat.st_mode=S_IFDIR | mode;

	}
	else{
			res->name=(char*)malloc(strlen(name)+1);
		strcpy(res->name,name);
		res->type=MAL_FILE;
		res->stat.st_mode=S_IFREG | mode;
	}
	res->stat.st_uid=conn->uid;
	res->stat.st_gid=conn->gid;
	res->stat.st_nlink=0;
	time(&res->stat.st_atime);
	time(&res->stat.st_mtime);
	res->next=NULL;
	res->children_head=NULL;
	res->num_of_children=0;
	res->data=NULL;
	return res;
}

static int malfs_getattr(const char* path,struct stat* st){
	int res=0;
	malfs_filep temp;

	temp=find_malfs(path);
	if(temp!=NULL){
		*st=temp->stat;
		return 0;
	}
	else
		return -ENOENT;

}
//insert children to parent directory
static void insert_children(malfs_filep parent,malfs_filep child){

	child->next=parent->children_head;
	parent->children_head=child;
	child->parent=parent;
	parent->num_of_children++;
	parent->stat.st_nlink++;

}
//find name from path 1st argument is total path 2nd argument is pointer for saving file/dir name
static char* find_parent(char* path,char** fname){
		char* ppath,*name;
		int len,itr;
		ppath=(char*)malloc((strlen(path)+1));
		strcpy(ppath,path);
		len=strlen(ppath);
		while(ppath[len]!='/'){
			len--;
		}
		name=(char*)malloc(strlen(ppath)-len);
		while(itr<strlen(ppath)-len){
			name[itr]=ppath[len+itr+1];
			itr++;
		}
		*fname=name;
		ppath[len]='\0';
		return ppath;
}
static int malfs_mkdir(const char* path,mode_t mode){
	int res=0;
	malfs_filep newdir;
	malfs_filep parent;
	char* name,*ppath;
	struct fuse_context* conn=fuse_get_context();
	//make root only if there is no root
	if(strcmp(path,"/")==0 && root==NULL){
		root=create_malfs("root",mode,MAL_DIR);
		return 0;
	}
	else{
		//already exist
		if(find_malfs(path))
			return -EEXIST;

		find_parent(path,&name);		
		//make a dir
		parent=find_malfs(ppath);
		newdir=create_malfs(name,mode,MAL_DIR);
		insert_children(parent,newdir);
		free(name);
		free(ppath);
		return 0;
	}
}
static int malfs_rmdir(const char* path){
	printf("rmdir\n");
	malfs_filep target;
	malfs_filep before,curr;
	int i;
	//if root dir 
	if((strcmp(path,"/"))==0)
		return -EPERM;

	target=find_malfs(path);

	//target not exist
	if(!target)
		return -ENOENT;		

	//target is not dir
	if(target->type!=MAL_DIR)
		return -ENOTDIR;

	//if it has children can't delete
	if(target->num_of_children>0)
		return -ENOTEMPTY;

	curr=target->parent->children_head;

	//if head is target
	if(curr==target){
		target->parent->children_head=target->next;
		free(target);
		target->parent->num_of_children--;
		return 0;
	}
	else{
		before=curr;
		curr=curr->next;
		for(i=1;i<target->parent->num_of_children;i++){
			if(target==curr){
				before->next=curr->next;
				free(target);
				target->parent->num_of_children--;
				return 0;
			}
			before=curr;
			curr=curr->next;
		}
	}


}
static int malfs_truncate(const char* path,off_t size){
	printf("truncate\n");
	return 0;
}
static int malfs_open(const char* path,struct fuse_file_info* fi){
	printf("open\n");
	return 0;
}
static int malfs_read(const char* path,char* buf,size_t size,off_t offset,struct fuse_file_info* fi){
	printf("read\n");
	return 0;
}
static int malfs_write(const char* path,const char* buf,size_t size,off_t offset,struct fuse_file_info* fi){
	printf("write\n");
	return 0;
}
static int malfs_opendir(const char* path,struct fuse_file_info* fi){
	if(find_malfs(path))
		return 0;
	else 
		return -ENOENT;
}
static int malfs_readdir(const char* path,void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi){
	printf("readdir\n");
	int i;
	malfs_filep target=find_malfs(path);
	malfs_filep child;
	if(target==NULL)
		return -ENOENT;
	else if(target->type==MAL_DIR){

		filler(buf,".",&target->stat,0);
		if(strcmp(path,"/")==0){
			filler(buf,"..",NULL,0);	
		}
		else{
			filler(buf,"..",&target->parent->stat,0);}
		child=target->children_head;
		for(i=0;i<target->num_of_children;i++){			filler(buf,child->name,&child->stat,0);
			child=child->next;

		}
		return 0;
	}
	else
		return -ENOTDIR;
}
static int malfs_create(const char* path,mode_t mode,struct fuse_file_info* fi){
	printf("create\n");
	return 0;
}
static int malfs_utimens(const char* path, const struct timespec tv[2]){
	printf("utimens\n");
	return 0;
}
static int malfs_unlink(const char* path){
	printf("unlink\n");return 0;
}
static int malfs_release(const char* path,struct fuse_file_info* fi){
	printf("release\n");return 0;
}

void* malfs_init(struct fuse_conn_info* fi){
	malfs_mkdir("/test1",0666);

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
	.init=malfs_init,

};
int main(int argc, char *argv[]){

	malfs_mkdir("/",0777);//make a root dir
	return fuse_main(argc, argv, &malfs_oper, NULL);
}






