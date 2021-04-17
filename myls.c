#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>
#include <termio.h>

DIR* dir;
struct dirent* ptr;

int get_param(int argc, char *argvp[]);
void Handle_R(char* path);
void Handle_i(char* path);
void Handle_l(char* path);
void convert_mode_to_letter(int mode);
void plainFunc();

int main(int argc, char** argv){
	char path[PATH_MAX+1];
	char path_ext[PATH_MAX+1];
	memset(path, 0, sizeof(path));

	int flag = 0;

	if(argc == 1){
		Handle_R("./");
	}
	else{
		//printf("In func\n");
		int param = get_param(argc, argv);

		// Getting parameter
		//printf("After getting param\n");
		strcpy(path, *argv);

		while(--argc != 0){
			argv++;
			if((*argv)[0] != '-'){
				flag = 1;
				strncpy(path,*argv, PATH_MAX);
			}
		}

		if(flag==0){
			strcpy(path,"./");
		}

		if((path[0] != '.' && path[0] != '/' && path[0] != '~' && path[0] != '-')
		|| (path[0] == '.' && path[1] == '.' && path[2] != '/')
        || (path[0] == '.' && path[1] != '/')){
			strcpy(path_ext, "./");
			strncat(path_ext, path, PATH_MAX - strlen("./"));
		}
		else{
			strncpy(path_ext, path, PATH_MAX);
		}

		if(param==1){
			Handle_i(path_ext);
		}
		else if(param==2){
			Handle_l(path_ext);
		}
		else if(param==3){
			Handle_R(path_ext);
		}
	}
}

int get_param(int argc,char *argv[])
{
	// param == 1 -> i
	// param == 2 -> l
	// param == 3 -> R
	int param = 0;
	int i;

	while(--argc != 0)
    {
		++argv;
		if((*argv)[0] == '-')
		{
			for(i = 1; (*argv)[i] != '\0'; i++)
			{
				if((*argv)[i] == 'i')
				{
					param = 1;
					continue;
				}
      	else if((*argv)[i] == 'l')
				{
					param = 2;
					continue;
				}
				else if((*argv)[i] == 'R'){
					param = 3;
					continue;
				}
				else
                {
					exit(1);
				}
			}
		}
	}
	return param;
}

void Handle_l(char *path){
	//printf("In handle l\n");
	struct stat theStat;

	dir = opendir(path);
	
	readdir(dir);
	readdir(dir);

	while((ptr=readdir(dir))!=NULL){
		stat(ptr->d_name, &theStat);

		struct stat* stat_buf = &theStat;

		if(strcmp(ptr->d_name, ".")==0){
			continue;
		}

		//getting mode
		convert_mode_to_letter(stat_buf->st_mode);

		printf("%2d ", (int)stat_buf->st_nlink);
		printf("%4s ", getpwuid(stat_buf->st_uid)->pw_name);
		printf("%4s", getgrgid(stat_buf->st_gid)->gr_name);
		printf(" %6d", (int)stat_buf->st_size);

		char buf_time[40];
		strcpy(buf_time, ctime(&stat_buf->st_mtime));
		buf_time[strlen(buf_time)-1] = '\0';
		printf(" %s", 4+buf_time);

		printf(" %s\n", ptr->d_name);
	}

	closedir(dir);
}

void Handle_i(char *path){
	dir = opendir(path);

	readdir(dir);
	readdir(dir);

	while((ptr = readdir(dir))!=NULL){
		printf("\t%ld ", ptr->d_ino);
		printf("%s\n", ptr->d_name);
	}

	closedir(dir);
}

void Handle_R(char *path){
	dir = opendir(path);

	readdir(dir);
	readdir(dir);

	while((ptr=readdir(dir))!=NULL){
		if(strcmp(ptr->d_name, ".")==0)
			continue;

		if(ptr->d_type & DT_DIR){
			printf("%s: \n", ptr->d_name);
			//Recursive to get to another path (directory)
			Handle_R(ptr->d_name);
		}
		else{
			printf("%s ", ptr->d_name);
		}
	}
}

void convert_mode_to_letter(int mode){

	if(S_ISLNK(mode)){
		printf("l");
	}
	else if(S_ISREG(mode)){
		printf("-");
	}
	else if(S_ISCHR(mode)){
		printf("c");
	}
	else if(S_ISBLK(mode)){
		printf("b");
	}
	else if(S_ISFIFO(mode)){
		printf("f");
	}
	else if(S_ISSOCK(mode)){
		printf("s");
	}
	if(S_IRUSR & mode){
		printf("r");
	}
	else{
		printf("-");
	}
	if(S_IWUSR & mode){
		printf("w");
	}
	else{
		printf("-");
	}

	//
	if(S_ISUID & mode){
		if(S_IXOTH & mode){
			printf("s");
		}
        else{
			printf("S");
		}
	}
	else if(S_IXUSR & mode){
		printf("x");
	}
	else{
		printf("-");
	}

	//
	if(S_IRGRP & mode){
		printf("r");
	}
	else{
		printf("-");
	}

	//
	if(S_IWGRP & mode){
		printf("w");
	}
	else
    {
		printf("-");
	}

	//
	if(S_ISGID & mode){
		if(S_IXOTH & mode){
			printf("s");
		}
        else{
			printf("S");
		}
	}
	else if(S_IXGRP & mode){
		printf("x");
	}
	else{
		printf("-");
	}

	//
	if(S_IROTH & mode){
		printf("r");
	}
	else{
		printf("-");
	}

	//
	if(S_IWOTH & mode){
		printf("w");
	}
	else
    {
		printf("-");
	}

	//
	if(S_ISVTX & mode){
		printf("T");
	}
	else if(S_IXOTH & mode){
		printf("x");
	}
	else{
		printf("-");
	}
}