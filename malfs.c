#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include "malfs.h"
struct malfs_file* root;


//find the file or directory which has given path
static malfs_filep find_malfs(const char* path){
	malfs_filep temp=root;
	malfs_filep child,res;
	char* tpath;
	char* name;
	unsigned int i;
	int isfound;
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
		printf("ii%s\n",tpath);
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

}
//find name from path 1st argument is total path 2nd argument is pointer for saving file/dir name
static char* find_parent(const char* path,char** fname){
		char* ppath,*name;
		int len;
		unsigned int itr=0;
		ppath=(char*)malloc((strlen(path)+1));
		strcpy(ppath,path);
		len=strlen(ppath);
		while(ppath[len]!='/'){
			len--;
		}
		name=(char*)malloc(strlen(ppath)-len+1);
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

		ppath=find_parent(path,&name);		
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
	unsigned int i;
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
	if(!find_malfs(path))
		return -ENOENT;
	
	return 0;
}
static int malfs_read(const char* path,char* buf,size_t size,off_t offset,struct fuse_file_info* fi){
	printf("%d read\n",size);
	malfs_filep temp = find_malfs(path);
	if(temp){
		if(temp->data==NULL)
			return 0;
		if(temp->type != MAL_FILE)
			return -EINVAL;
		strncpy(buf, temp->data + offset, size);
		return size;
	}
	else
		return -ENOENT;
}
static int malfs_write(const char* path,const char* buf,size_t size,off_t offset,struct fuse_file_info* fi){
	printf("write\n");
	malfs_filep temp = find_malfs(path);
	if(temp){
		if(temp->type != MAL_FILE)	
			return -EINVAL;
		if(temp->stat.st_size < (size + offset)){
			int new_size = size + offset;
			char *contents = (char*)realloc(temp->data, new_size);
			temp->data = contents;
			temp->stat.st_size = size + offset;
		}
		strncpy(temp->data + offset, buf, size);
		return size;
	}
	else
		return -ENOENT;
}
static int malfs_opendir(const char* path,struct fuse_file_info* fi){
	if(find_malfs(path))
		return 0;
	else 
		return -ENOENT;
}
static int malfs_readdir(const char* path,void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi){
	printf("readdir\n");
	unsigned int i;
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
	char *name, *ppath;
	malfs_filep parent;
	malfs_filep newfile;

	if(find_malfs(path))	return -EEXIST;
	
	ppath = find_parent(path, &name);
	parent = find_malfs(ppath);
	newfile = create_malfs(name, mode, MAL_FILE);
	insert_children(parent, newfile);

	free(name);
	free(ppath);
	return 0;
}
static int malfs_utimens(const char* path, const struct timespec tv[2]){
	printf("utimens\n");
	malfs_filep target=find_malfs(path);
	if(target){
		target->stat.st_atime=tv[0].tv_sec;
		target->stat.st_mtime=tv[1].tv_sec;
		return 0;
	}
	else return -ENOENT;
}
static int malfs_unlink(const char* path){
	printf("unlink\n");
	malfs_filep target;
	malfs_filep before, curr;
	unsigned int i;

	target = find_malfs(path);
	//target does not exist
	if(!target)
		return -ENOENT;
	//target is a directory.
	if(target->type!=MAL_FILE)
		return -EISDIR;
	
	curr = target->parent->children_head;
	if(curr==target){
		target->parent->children_head = target->next;
		free(target);
		target->parent->num_of_children--;
		return 0;
	}
	else{
		before = curr;
		curr = curr->next;
		for(i=1; i<(target->parent->num_of_children); i++){
			if(target==curr){
				before->next = curr->next;
				free(target);
				target->parent->num_of_children--;
				return 0;
			}
			before = curr;
			curr = curr->next;
		}
	}
}
static int malfs_release(const char* path,struct fuse_file_info* fi){
	printf("release\n");return 0;
}
static int malfs_chmod(const char* path,mode_t mode){
	malfs_filep target=find_malfs(path);
	if(target){
		target->stat.st_mode=mode;
		return 0;
	}
	else
		return -ENOENT;

}
static int malfs_chown(const char* path,uid_t uid,gid_t gid){
	malfs_filep target=find_malfs(path);
	if(target){
		target->stat.st_uid=uid;
		target->stat.st_gid=gid;
		return 0;
	}
	else
		return -ENOENT;
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
	.chmod=malfs_chmod,
	.chown=malfs_chown,
	.init=malfs_init,

};

int main(int argc, char *argv[]){

	malfs_mkdir("/",0777);//make a root dir
	return fuse_main(argc, argv, &malfs_oper, NULL);
}






