#include <assert.h>
#include <dirent.h>
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "ite/itp.h"
#include "redblack/redblack.h"
#include "ctrlboard.h"

const char VideoSubdirectory[6]= {"Video"};
const char JpegSubdirectory[4]  = {"Jpg"};

VideoProcessHandle handle = {0};
static VideoFileLoadCallback VideoLoadCallback;
static bool DeleteTaskFlag = false;

static int VFCompare(const void *pa, const void *pb, const void *config)
{
    struct dirent   *dira = (struct dirent *) pa;
    struct dirent   *dirb = (struct dirent *) pb;
    char            *ta, *tb;
    assert( pa);
    assert( pb);

    if ((dira->d_type == DT_DIR) && (dirb->d_type != DT_DIR))
        return -1;

    if ((dira->d_type != DT_DIR) && (dirb->d_type == DT_DIR))
        return 1;

    // comare by time
    ta  = strchr(dira->d_name, '_');
    tb  = strchr(dirb->d_name, '_');

    return strcmp(ta, tb);
}


static void *SearchFileTask(void *arg)
{
    void    *rbTree;
    struct dirent   *val;
    DIR     *pdir   = NULL;

    rbTree = rbinit(VFCompare, NULL);
    if (rbTree == NULL)
    {
        printf("insufficient memory");
        goto end;
    }


    if(StorageGetCurrType() == STORAGE_SD)
    {      
    	handle.NOSDcard  = 0;
		handle.FileCount = 0;
		handle.PageCount = 0;

        // Open the directory
        pdir = opendir(handle.SubDir);
        if (pdir)
        {
            struct dirent *pent;

            // Make this our current directory
            chdir(handle.SubDir);

            // List the contents of the directory
            while ((pent = readdir(pdir)) != NULL)
            {
                struct dirent   *dir, *ret;

                if ((strcmp(pent->d_name, ".") == 0) || (strcmp(pent->d_name, "..") == 0))
                    continue;


                dir = malloc(sizeof(struct dirent));
                if (dir == NULL)
                {
                    printf("out of memory: %d\n", sizeof(struct dirent));
                    goto end;
                }

                memcpy(dir, pent, sizeof(struct dirent));

                ret = (struct dirent *) rbsearch((void *)dir, rbTree);
                if (ret == NULL)
                {
                    printf("out of memory: %d\n", sizeof(struct dirent));
                    free(dir);
                    goto end;
                }
                
                handle.FileCount++;
				handle.PageCount = (handle.FileCount / 8);
            }    

            int ArrayCounter = 0;
            for (val = (struct dirent *) rblookup(RB_LULAST, NULL, rbTree);
                  val != NULL;
                  val = (struct dirent *)rblookup(RB_LUPREV, val, rbTree))
            {   
               

                if(val->d_type == DT_REG && ArrayCounter < FILE_NUM_MAX)
                {
                    //printf("name = %s \n ",val->d_name);
                    char tmp_array[PATH_MAX] = {0};
                    char *token_p = NULL;
				    strcpy(tmp_array, val->d_name);

                    strcpy(handle.VideoFilePathArray[ArrayCounter], handle.SubDir);
                    strcat(handle.VideoFilePathArray[ArrayCounter], "/");
                    strcat(handle.VideoFilePathArray[ArrayCounter], tmp_array);
                    
					token_p = strtok(tmp_array, ".");
					strcpy(handle.VideoFileNameArray[ArrayCounter], token_p);

                    strcpy(handle.ThumbnailPathArray[ArrayCounter], handle.SubJpgDir);
                    strcat(handle.ThumbnailPathArray[ArrayCounter], "/");
                    strcat(handle.ThumbnailPathArray[ArrayCounter], handle.VideoFileNameArray[ArrayCounter]);
					strcat(handle.ThumbnailPathArray[ArrayCounter], ".jpg");

					//printf("n=%s, p=%s, t = %s\n", handle.VideoFileNameArray[ArrayCounter], handle.VideoFilePathArray[ArrayCounter], handle.ThumbnailPathArray[ArrayCounter]);
					ArrayCounter++;
                    
                }

            }
            //printf("File count = %d\n",handle.FileCount);

        }
        else
        {
            printf("opendir(%s) failed\n", handle.SubDir);
            goto end;
        }
    } 
    else
    {
		 handle.NOSDcard  = 1;
		 handle.FileCount = 0;
		 handle.PageCount = 0;    
         printf("not SD card type\n");
    }

end:
    if(pdir)
        closedir(pdir);
    
    if (rbTree)
    {
        RBLIST *rblist;

        if ((rblist = rbopenlist(rbTree)) != NULL)
        {
            void *val;
            while ((val = (void *) rbreadlist(rblist)))
                free(val);

            rbcloselist(rblist);
        }
        rbdestroy(rbTree);
        rbTree = NULL;
    }
	
	// return result
	VideoLoadCallback(&handle);

    //printf("task finish \n");

}


static void *DeleteFileTask(void *arg)
{

	struct statvfs info;
	void	*rbTree;
	struct dirent	*val;
	struct dirent	*valmin;
	DIR 	*pdir	= NULL;	
	StorageType storageType = STORAGE_NONE;
	char v_path[PATH_MAX] = {0};
	char j_path[PATH_MAX] = {0};

	memset(v_path, 0, sizeof(v_path));
	memset(j_path, 0, sizeof(j_path));

	while(DeleteTaskFlag)
	{
    	// check capacity
	    storageType = StorageGetCurrType();
		if(storageType == STORAGE_SD)
		{

			rbTree = rbinit(VFCompare, NULL);
			if (rbTree == NULL)
			{
				printf("insufficient memory \n");
			}		

			strncpy(v_path, StorageGetDrivePath(storageType), sizeof(v_path));
			strncpy(j_path, StorageGetDrivePath(storageType), sizeof(j_path));
			
			strcat(v_path, VideoSubdirectory);
			strcat(j_path, JpegSubdirectory);


		    if (statvfs(StorageGetDrivePath(storageType), &info) == 0)
		    {
		        uint64_t avail = (uint64_t)info.f_bfree * info.f_bsize;

				if(avail < (uint64_t)MIN_RESERVED_SIZE)
				{

				     printf("free space of sd disk is %llu bytes\n", avail);

					 // Open the directory
					 pdir = opendir(v_path);
					 if (pdir)
					 {
							struct dirent *pent;

							// Make this our current directory
							chdir(v_path);

							// List the contents of the directory
							while ((pent = readdir(pdir)) != NULL)
							{
								struct dirent	 *dir, *ret;

								if ((strcmp(pent->d_name, ".") == 0) || (strcmp(pent->d_name, "..") == 0))
								 continue;

								dir = malloc(sizeof(struct dirent));
								if (dir == NULL)
								{
									 printf("out of memory: %d\n", sizeof(struct dirent));
									 //goto delete_end;
								}

								memcpy(dir, pent, sizeof(struct dirent));

								ret = (struct dirent *) rbsearch((void *)dir, rbTree);
								if (ret == NULL)
								{
									 printf("out of memory: %d\n", sizeof(struct dirent));
									 free(dir);
									 //goto delete_end;
								}
							}
							
	
							for (val = (struct dirent *) rblookup(RB_LUFIRST, NULL, rbTree);
								  val != NULL;
								  val = (struct dirent *)rblookup(RB_LULESS, val, rbTree))
							{  
								  valmin = val;
							}
								  
							if(valmin != NULL)
							{
								char buf[PATH_MAX] = {0};
								char jbuf[PATH_MAX] = {0};
								void * fp = NULL;
								char *token_p = NULL;

								//try to remove avi
								strcpy(buf, v_path);
								strcat(buf, "/");
								strcat(buf, valmin->d_name);
								fp = fopen(buf, "rb") ;
								if(fp != NULL)
								{
									  fclose(fp);
									  remove(buf);
									  printf("remove file = %s !!!\n", buf);
									  //try to remove jpg
									  strncpy(buf, valmin->d_name, sizeof(buf));
									  token_p = strtok(buf, ".");
									  strcpy(jbuf, j_path);
									  strcat(jbuf, "/");
									  strcat(jbuf, token_p);
									  strcat(jbuf, ".jpg");
									  fp = fopen(jbuf, "rb");
									  if(fp !=NULL)
									  {
										  fclose(fp);
										  printf("remove jpg = %s !!!\n", jbuf);
										  remove(jbuf);   
									  }
								}							
							}
						
							closedir(pdir); 

					 	}

					 }
		    }
			
			if (rbTree)
			{
				RBLIST *rblist;
			
				if ((rblist = rbopenlist(rbTree)) != NULL)
				{
					void *val;
					while ((val = (void *) rbreadlist(rblist)))
						free(val);
			
					rbcloselist(rblist);
				}
				rbdestroy(rbTree);
				rbTree = NULL;
			}  

		}	
		
		sleep(10);
	}
	
	printf("DeleteFileTask finish\n");
}


void VideoFileInit(void)
{

    StorageType storageType = STORAGE_NONE;
    pthread_t       task;
    pthread_attr_t  attr;	
    char temp[PATH_MAX] = {0};
	int i =0;
	
    storageType = StorageGetCurrType();
    if(storageType == STORAGE_SD)
    {
    	
		memset(temp, 0, sizeof(temp));
        strncpy(temp, StorageGetDrivePath(storageType), sizeof(temp));
        strcat(temp, VideoSubdirectory);		
 
		if(mkdir(temp, S_IRWXU) != 0)
		{
			//printf("mkdir fail\n");
		}

		memset(temp, 0, sizeof(temp));
		strncpy(temp, StorageGetDrivePath(storageType), sizeof(temp));
		strcat(temp, JpegSubdirectory); 
		
		if(mkdir(temp, S_IRWXU) != 0)
		{
			//printf("mkdir fail\n");
		}	

    }
	else
	{
		printf("not SD card type\n");
	}

}


int VideoFileLoad(VideoFileLoadCallback func)
{

    pthread_t       task;
    pthread_attr_t  attr;
    StorageType storageType = STORAGE_NONE;
	
    if(func == NULL)
		return -1;

	VideoLoadCallback    = func;
	
    storageType = StorageGetCurrType();
    if(storageType == STORAGE_SD)
    {
         strncpy(handle.SubDir, StorageGetDrivePath(storageType), sizeof(handle.SubDir));
         strcat(handle.SubDir, VideoSubdirectory);

         strncpy(handle.SubJpgDir, StorageGetDrivePath(storageType), sizeof(handle.SubDir));
         strcat(handle.SubJpgDir, JpegSubdirectory);		 
         //printf("create task %s\n",SubDir);
         pthread_attr_init(&attr);
         pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
         pthread_create( &task,  &attr,  SearchFileTask, NULL);
         
    }
	else
	{
		handle.NOSDcard  = 1;
		handle.FileCount = 0;
		handle.PageCount = 0;
		VideoLoadCallback(&handle);
		printf("not SD card type\n");
		return -1;
	}

	return 0;

}

void VideoFileMemSpaceCheckStart()
{

    pthread_t       task;
    pthread_attr_t  attr;

	DeleteTaskFlag = true;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create( &task,	&attr,	DeleteFileTask, NULL);	

}

void VideoFileMemSpaceCheckStop()
{
	DeleteTaskFlag = false;
}



