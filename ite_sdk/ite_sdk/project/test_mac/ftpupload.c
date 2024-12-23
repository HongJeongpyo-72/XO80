/***************************************************************************
*                                  _   _ ____  _
*  Project                     ___| | | |  _ \| |
*                             / __| | | | |_) | |
*                            | (__| |_| |  _ <| |___
*                             \___|\___/|_| \_\_____|
*
* Copyright (C) 1998 - 2019, Daniel Stenberg, <daniel@haxx.se>, et al.
*
* This software is licensed as described in the file COPYING, which
* you should have received as part of this distribution. The terms
* are also available at https://curl.haxx.se/docs/copyright.html.
*
* You may opt to use, copy, modify, merge, publish, distribute and/or sell
* copies of the Software, and permit persons to whom the Software is
* furnished to do so, under the terms of the COPYING file.
*
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express or implied.
*
***************************************************************************/
#include <stdio.h>
#include <string.h>

#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include "ite/ith.h"

/* <DESC>
* Performs an FTP upload and renames the file just after a successful
* transfer.
* </DESC>
*/

#define FTP_REAL_FILE

#define LOCAL_FILE      "A:/test_020m.zip"
#define UPLOAD_FILE_AS  "test_020m.zip"
#define REMOTE_URL      "ftp://192.168.1.100/test_020m.zip"
//#define RENAME_FILE_TO  "ite_020m.zip"
#define FTP_ACCOUNT  "irene:irene"

/* NOTE: if you want this example to work on Windows with libcurl as a
DLL, you MUST also provide a read callback with CURLOPT_READFUNCTION.
Failing to do so will give you a crash since a DLL may not use the
variable's memory when passed in to it from an app like this. */
static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    curl_off_t nread;
    static uint32_t cnt = 0;

    /* in real-world cases, this would probably get this data differently
    as this fread() stuff is exactly what the library already would do
    by default internally */
    #if defined(FTP_REAL_FILE)
    size_t retcode = fread(ptr, size, nmemb, stream);
    #else
    size_t retcode = size * nmemb;
    #endif

    nread = (curl_off_t)retcode;

#if 1
    if (!(cnt++ % 100)) {
        putchar('#');
        fflush(stdout);
    }
#else
    printf("*** We read %" CURL_FORMAT_CURL_OFF_T
        " bytes from file\n", nread);
#endif

    return retcode;
}

static void _timerStart(ITHTimer timer)
{
    ithTimerReset(timer);
    ithTimerCtrlEnable(timer, ITH_TIMER_UPCOUNT);
    ithTimerIsrEnable(timer);
    ithTimerSetLoad(timer, 0);
    ithTimerSetCounter(timer, 0);
    ithTimerEnable(timer);
}

static uint32_t _timerStop(ITHTimer timer)
{
    ithTimerDisable(timer);
    return ithTimerGetTime(timer);
}

int DoFtpUpload(void)
{
    CURL *curl;
    CURLcode res;
    FILE *hd_src;
    struct stat file_info;
    curl_off_t fsize;
    uint32_t time;

    struct curl_slist *headerlist = NULL;
    //static const char buf_1[] = "RNFR " UPLOAD_FILE_AS;
    //static const char buf_2[] = "RNTO " RENAME_FILE_TO;

    /* get the file size of the local file */
    if (stat(LOCAL_FILE, &file_info)) {
        printf("Couldn't open '%s': %s\n", LOCAL_FILE, strerror(errno));
        return 1;
    }
    fsize = (curl_off_t)file_info.st_size;

    printf("Local file size: %" CURL_FORMAT_CURL_OFF_T " bytes.\n", fsize);

    /* get a FILE * of the same file */
    hd_src = fopen(LOCAL_FILE, "rb");

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {
        /* build a list of commands to pass to libcurl */
        //headerlist = curl_slist_append(headerlist, buf_1);
        //headerlist = curl_slist_append(headerlist, buf_2);

        /* we want to use our own read function */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

        /* enable uploading */
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        /* specify target */
        curl_easy_setopt(curl, CURLOPT_URL, REMOTE_URL);
        curl_easy_setopt(curl, CURLOPT_USERPWD, FTP_ACCOUNT); // Irene Lin
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // Irene Lin

        /* pass in that last of FTP commands to run after the transfer */
        curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

        /* now specify which file to upload */
        curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

        /* Set the size of the file to upload (optional).  If you give a *_LARGE
        option you MUST make sure that the type of the passed-in argument is a
        curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
        make sure that to pass in a type 'long' argument. */
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
            (curl_off_t)fsize);

        _timerStart(ITH_TIMER3); // Irene Lin

        /* Now run off and do what you've been told! */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));

        time = _timerStop(ITH_TIMER3); // Irene Lin
        printf("\r\n file size: %" CURL_FORMAT_CURL_OFF_T ", total time: %d ms\n", fsize, (time / 1000));
        printf(" ==> %f Mbps \n\n", ((double)((fsize * 8) / (1024 * 1024)) / time) * 1000000);

        /* clean up the FTP commands list */
        curl_slist_free_all(headerlist);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    fclose(hd_src); /* close the local file */

    curl_global_cleanup();
    return 0;
}
