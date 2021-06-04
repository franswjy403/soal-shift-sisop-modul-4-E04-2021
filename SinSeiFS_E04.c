#define FUSE_USE_VERSION 28
#include<fuse.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<dirent.h>
#include<errno.h>
#include<sys/time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdbool.h>

char dirpath[50] = "/home/frans/Downloads";
char ext[100000] = "\0";
int id = 0;

static  int  xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    res = lstat(path, stbuf);

    if (res == -1) return -errno;
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    dp = opendir(path);

    if (dp == NULL) return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        if(filler(buf, de->d_name, &st, 0)) break;
    }
    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    int fd;
    int res;
    (void) fi;

    fd = open(path, O_RDONLY);

    if (fd == -1) return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1) res = -errno;

    close(fd);

    return res;
}

void check_ext(char* file)
{
	char temp[1000];
    strcpy(temp, file);
    char dot = '.';
    char *tempExt = strrchr(temp, dot);
	memset(ext, 0, sizeof(ext));
	strcpy(ext, tempExt);
}

char key[90] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";
int ges = 10;
void encr(char* str)
{
	int i, j;
	if(!strcmp(str, ".") || !strcmp(str, "..")) return;
	for(i = 0; i < strlen(str); i++)
	{
		for(j = 0; j < 87; j++)
		{
			if(str[i] == key[j]){
				str[i] = key[(j + ges) % 87];
				break;
			}
		}
	}
}

void substring(char s[], char sub[], int p, int l) {
   int c;
   for (c=0; c<l; c++){
       sub[c] = s[p+c];
   }
   sub[c] = '\0';
}

void listFilesRecursivelyEnc1(char *basePath)
{
	char fpath[1000];
	if(strcmp(basePath,"/") == 0)
	{
		basePath=dirpath;	
		sprintf(fpath,"%s",basePath);
	}
	else sprintf(fpath, "%s%s",dirpath,basePath);

    char path[1024];
    struct dirent *dp;
    DIR *dir = opendir(fpath);

    if(!dir) return;

    while((dp = readdir(dir)) != NULL)
    {
        if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
        	char name[1024], newname[1024], foldername[1024];
        	strcpy(name, dp->d_name);
            check_ext(name);
			char namafile[100000];
			memset(namafile, 0, strlen(namafile));
			strncpy(namafile, name, id);
			
        	encr(namafile);
        	sprintf(newname, "/%s.%s", namafile, ext);
        	memset(foldername, '\0', sizeof(foldername));
			sprintf(foldername, "%s%s", fpath, newname);
			
			pid_t cid;
			cid = fork();
			if(cid == 0)
			{
				char *argv[] = {"cp", fpath, foldername, NULL};
				execv("/bin/cp", argv);
				exit(1);
			}
			else wait(NULL);
			
            printf("%s\n", foldername);
            
            strcpy(path, fpath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            listFilesRecursivelyEnc1(path);
        }
    }
    closedir(dir);
}

void encrypt1(char *name)
{
	listFilesRecursivelyEnc1(name);
}

static int xmp_mkdir(char *path, mode_t mode)
{
	printf("MKDIR\n");
    
    char cek_substr[10];
    substring(path, cek_substr, 0, 5);
	printf("%s -- %s", path, cek_substr);
	if(strcpy(cek_substr, "AtoZ_") == 0) //folder encrypt1
	{
		encrypt1(path);
	}
	// else if(strcpy(cek_substr, "encv2_") == 0) //folder encrypt2
	// {
	// 	encrypt2(path);
	// }
	
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res;

	res = mkdir(fpath, mode);
	if (res == -1) return -errno;
	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	// .access		= xmp_access,
	// .readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	// .mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	// .symlink	= xmp_symlink,
	// .unlink		= xmp_unlink,
	// .rmdir		= xmp_rmdir,
	// .rename		= xmp_rename,
	// .link		= xmp_link,
	// .chmod		= xmp_chmod,
	// .chown		= xmp_chown,
	// .truncate	= xmp_truncate,
	// .utimens	= xmp_utimens,
	// .open		= xmp_open,
	.read		= xmp_read,
	// .write		= xmp_write,
	// .statfs		= xmp_statfs,
	// .create     = xmp_create,
	// .release	= xmp_release,
	// .fsync		= xmp_fsync,
};

int main(){
    
}