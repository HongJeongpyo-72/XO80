#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include "tuya_hal_fs.h"

#define LOGIPTF(...) \
do \
{ \
    printf("FS [%s:%d] %s ",__FILE__,__LINE__,(const char*)__func__, \
             ##__VA_ARGS__); \
}while(0)

int tuya_hal_fs_mkdir(const char* path)
{
    char mkdir_path[128];
    char tmp[32];
	char *s, *ss;

    if(path == NULL)
        return -1;

	s = strchr(path, '/');
    if(s == NULL)
        return -1;

    strncpy(tmp, path, s - path);
    strcpy(mkdir_path, tmp);
    s++;

    while(1)
    {
        ss = strchr(s, '/');
        if(ss == NULL)
        {
            ss = strchr(s, '.');
            if(ss == NULL)
            {
                if (access(path, F_OK))
                {
                    mkdir(path, S_IRWXU);
                }
            }
            else
            {
                FILE* fp = fopen(path, "w");
                if(fp != NULL)
                    fclose(fp);
            }

			break;
        }
        else
        {
            strncpy(tmp, s, ss - s);
            sprintf(mkdir_path, "%s/%s", mkdir_path, tmp);

            if (access(mkdir_path, F_OK))
            {
                mkdir(mkdir_path, S_IRWXU);
            }   

            s = ss + 1;
        }     
    }

    return 0;
}

int tuya_hal_fs_remove(const char* path)
{	
    return remove(path);
}

int tuya_hal_fs_mode(const char* path, uint32_t* mode)
{
    return chmod(path, mode);
}

int tuya_hal_fs_is_exist(const char *path, bool *is_exist)
{
    if (!access(path, F_OK))
    {
        *is_exist = true;
        return 0;
    }
    else
    {
        *is_exist = false;
        return 0;
    }
}

int tuya_hal_dir_open(const char* path, TUYA_DIR* dir)
{
	dir = opendir(path);
	if(dir == NULL)
    	return -1;
	else
		return 0;
}

int tuya_hal_dir_close(TUYA_DIR dir)
{
    return closedir(dir);
}

int tuya_hal_dir_read(TUYA_DIR dir, TUYA_FILEINFO* info)
{
	info = readdir(dir);
  	if(info == NULL)
    	return -1;
	else
		return 0;
}

int tuya_hal_dir_name(TUYA_FILEINFO info, const char** name)
{
    struct dirent* pdirEntry = (struct dirent*)info;
    if(pdirEntry != NULL && *name != NULL)
    {
        strncpy(*name, pdirEntry->d_name, sizeof(*name));
        return 0;
    } 
    return -1;
}

int tuya_hal_dir_is_directory(TUYA_FILEINFO info, bool* is_dir)
{
    struct dirent* pdirEntry = (struct dirent*)info;
    *is_dir = false;
    if(pdirEntry != NULL)
    {
        if(pdirEntry->d_type == DT_DIR)
        {
            *is_dir = true;
            return 0;
        }
    }
    return -1;
}

int tuya_hal_dir_is_regular(TUYA_FILEINFO info, bool* is_regular)
{
    struct dirent* pdirEntry = (struct dirent*)info;
    *is_regular = false;
    if(pdirEntry != NULL)
    {
        if(pdirEntry->d_type == DT_REG)
        {
            *is_regular = true;
            return 0;
        }
    }
    return -1;
}

int tuya_hal_fs_rename(char *path_old,char *path_new)
{
	return rename(path_old, path_new);
}

TUYA_FILE tuya_hal_fopen(const char* path, const char* mode)
{
	return fopen(path, mode);
}

int tuya_hal_fclose(TUYA_FILE file)
{
	return fclose(file);
}

int tuya_hal_fread(void* buf, int bytes, TUYA_FILE file)
{
	return fread(buf, 1, bytes, file);
}

int tuya_hal_fwrite(void* buf, int bytes, TUYA_FILE file)
{
    return fwrite(buf, 1, bytes, file);
}

int tuya_hal_fsync(TUYA_FILE file)
{
	return fsync(file);
}

char* tuya_hal_fgets(char* buf, int len, TUYA_FILE file)
{
	return fgets(buf, len, file);
}

int tuya_hal_feof(TUYA_FILE file)
{
int ret;
ret=feof(file);

ithPritf("\n tuya_hal_feof = %d \n",ret);

    return ret;
}

int tuya_hal_fseek(TUYA_FILE file, long offs, int whence)
{
    return fseek(file, offs, whence);
}

int64_t tuya_hal_ftell(TUYA_FILE file)
{
    return ftell(file);
}

