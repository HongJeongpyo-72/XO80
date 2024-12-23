#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include "bootloader.h"
#include "config.h"

#include "../aXO80/HT_Define.H"


static char pkgFilePath[PATH_MAX];
static ITCFileStream fileStream;
static char usbPath[4] = {0};
static char privatePath[4] = {0};

#define MAX_ADDRESSBOOK 24

static uint32_t DcpsGetSize( uint8_t *Src, uint32_t SrcLen )
{
	uint32_t DcpsSize=0;
	uint32_t i=0;
	uint32_t out_len=0;
	uint32_t in_len=0;

	while(i<SrcLen)
	{
		out_len = ((Src[i+3]) | (Src[i+2]<<8) | (Src[i+1]<<16) | (Src[i]<<24));
        i=i+4;
        in_len = ((Src[i+3]) | (Src[i+2]<<8) | (Src[i+1]<<16) | (Src[i]<<24));
        i=i+4;

        if (out_len == 0)
			break;

        if( in_len < out_len)	DcpsSize += out_len;
        else					DcpsSize += in_len;

		i += in_len;
	}
	return	DcpsSize;
}

static int CountAddressbook(void)
{
    struct stat sb;
    char filepath[PATH_MAX];
    int i=0;

    for(i= 1; i <= MAX_ADDRESSBOOK; i++) {
        sprintf(filepath, "%saddressbook%d.ucl", privatePath, i);
        if(stat(filepath, &sb) != 0) {
            break;
        }
    }

    return i-1;
}

static void GetUsbDriveName(void)
{
	int i = 0;
	ITPDriveStatus* driveStatusTable;
    ITPDriveStatus* driveStatus = NULL;

	// try to find the package drive
    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_GET_TABLE, &driveStatusTable);

	for (i = 0; i < ITP_MAX_DRIVE; i++) {
		driveStatus = &driveStatusTable[i];
		if (driveStatus->avail &&
			driveStatus->removable &&
			driveStatus->disk >= ITP_DISK_MSC00 &&
			driveStatus->disk <= ITP_DISK_MSC17) {
			strcpy(usbPath, driveStatus->name); //Get USB Mount Drive Name
			break;
		}

	}

	for (i = 0; i < ITP_MAX_DRIVE; i++) {
		driveStatus = &driveStatusTable[i];
		if (driveStatus->avail && driveStatus->disk == ITP_DISK_NOR) {
			strcpy(privatePath, driveStatus->name); //Get Private Folder Mount Drive Name
			break;
		}
	}
}

static int CopyUclFileMore(char *ucl_filename)
{
    int ret = 0;
    FILE *f = NULL;
    uint8_t* filebuf = NULL;
    uint8_t* xmlbuf = NULL;
    char xml_filename[PATH_MAX];
    char buf[PATH_MAX];
    char filepath[PATH_MAX];
    struct stat sb;
    int readsize, compressedSize, xmlsize;
    int index = 0;

    sprintf(filepath, "%s%s", privatePath, ucl_filename);
    f = fopen(filepath, "rb");
    if (!f)
    {
        printf("file open %s fail\n", filepath);
        goto error;
    }

    if (fstat(fileno(f), &sb) == -1)
    {
        printf("get file size fail\n");
        goto error;
    }

    filebuf = malloc(sb.st_size);
    if (!filebuf)
        goto error;

    readsize = fread(filebuf, 1, sb.st_size, f);
    assert(readsize == sb.st_size);

    fclose(f);
    f = NULL;

    compressedSize = sb.st_size - 18;
    xmlsize = DcpsGetSize(filebuf + 18, compressedSize);

    xmlbuf = malloc(xmlsize + xmlsize / 8 + 256);
    if (!xmlbuf)
        goto error;

#ifdef CFG_DCPS_ENABLE
    // hardware decompress
    ioctl(ITP_DEVICE_DECOMPRESS, ITP_IOCTL_INIT, NULL);
    ret = write(ITP_DEVICE_DECOMPRESS, filebuf + 18, compressedSize);
    assert(ret == compressedSize);
    ret = read(ITP_DEVICE_DECOMPRESS, xmlbuf, xmlsize);
    assert(ret == xmlsize);
    ioctl(ITP_DEVICE_DECOMPRESS, ITP_IOCTL_EXIT, NULL);

#else
    // software decompress
    ret = SoftwareDecompress(xmlbuf, filebuf);
    assert(ret == 0);

#endif // CFG_DCPS_ENABLE

    free(filebuf);
    filebuf = NULL;

    sscanf(ucl_filename, "addressbook%d.ucl", &index);
    sprintf(xml_filename, "addressbook%d.xml", index);

    strcpy(buf, usbPath);
    strcat(buf, xml_filename);

    f = fopen(buf, "wb");
    if (!f)
    {
        printf("file open %s fail\n", buf);
        goto error;
    }

    ret = fwrite(xmlbuf, 1, xmlsize, f);
    assert(ret == xmlsize);

    free(xmlbuf);

    fclose(f);
    f = NULL;

    printf("save to %s\n", buf);

    return 0;
error:
    if (xmlbuf)
        free(xmlbuf);

    if (filebuf)
        free(filebuf);

    if (f)
        fclose(f);

    return -1;
}

/*Notice: copy addressbook only for outdoor*/
int CopyUclFile(void)
{
        int ret = 0;
        FILE *f = NULL;
        uint8_t* filebuf = NULL;
        uint8_t* xmlbuf = NULL;
        int k = 0;
        struct stat sb;
        int readsize, compressedSize, xmlsize, addressbook_count = 0;
        char ucl_filename[32];
		char filepath[32];

		// Find Usb and Private Drive Mount Drive
		GetUsbDriveName();

		sprintf(filepath, "%saddressbook.ucl", privatePath);
        f = fopen(filepath, "rb");
        if (!f)
        {
            printf("file open addressbook.ucl fail\n");
            goto error;
        }

        if (fstat(fileno(f), &sb) == -1)
        {
            printf("get file size fail\n");
            goto error;
        }

        filebuf = malloc(sb.st_size);
        if (!filebuf)
            goto error;

        readsize = fread(filebuf, 1, sb.st_size, f);
        assert(readsize == sb.st_size);

        fclose(f);
        f = NULL;

        compressedSize = sb.st_size - 18;
        xmlsize = DcpsGetSize(filebuf + 18, compressedSize);

        xmlbuf = malloc(xmlsize + xmlsize / 8 + 256);
        if (!xmlbuf)
            goto error;

#ifdef CFG_DCPS_ENABLE
        // hardware decompress
        ioctl(ITP_DEVICE_DECOMPRESS, ITP_IOCTL_INIT, NULL);
        ret = write(ITP_DEVICE_DECOMPRESS, filebuf + 18, compressedSize);
        assert(ret == compressedSize);
        ret = read(ITP_DEVICE_DECOMPRESS, xmlbuf, xmlsize);
        assert(ret == xmlsize);
        ioctl(ITP_DEVICE_DECOMPRESS, ITP_IOCTL_EXIT, NULL);

#else
        // software decompress
        ret = SoftwareDecompress(xmlbuf, filebuf);
        assert(ret == 0);

#endif // CFG_DCPS_ENABLE

        free(filebuf);
        filebuf = NULL;

		sprintf(filepath, "%saddressbook.xml", usbPath);
        f = fopen(filepath, "wb");
        if (!f)
        {
            printf("file open addressbook.xml fail\n");
            goto error;
        }

        ret = fwrite(xmlbuf, 1, xmlsize, f);
        assert(ret == xmlsize);

        free(xmlbuf);

        fclose(f);
        f = NULL;

        printf("save to %s\n", filepath);

        addressbook_count = CountAddressbook();
        for(k = 1;k <= addressbook_count; k++) {
            sprintf(ucl_filename, "addressbook%d.ucl", k);
            CopyUclFileMore(ucl_filename);
        }

        return 0;
    error:
        if (xmlbuf)
            free(xmlbuf);

        if (filebuf)
            free(filebuf);

        if (f)
            fclose(f);

        return -1;

}

#if (LCD_MODULE_SEL == ADS_LCD)
	#define CFG_UPGRADE_FILENAME_LIST_ORG "XO80_17301-43843.PKG"
#elif (LCD_MODULE_SEL == ITS_LCD)
	#define CFG_UPGRADE_FILENAME_LIST_ORG "XO80_17301-43843_ITS.PKG"
#elif (LCD_MODULE_SEL == EMB_LCD)
	#define CFG_UPGRADE_FILENAME_LIST_ORG "XO80_17301-43843_EMB.PKG"
#else
	"Error!!!  No LCD module selected"
#endif


ITCStream* OpenUpgradePackage(void)
{
    ITPDriveStatus* driveStatusTable;
    ITPDriveStatus* driveStatus = NULL;
    int i;

    // try to find the package drive
    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_GET_TABLE, &driveStatusTable);

#if 0
    for (i = ITP_MAX_DRIVE - 1; i >= 0; i--)
    {
        driveStatus = &driveStatusTable[i];
        if (driveStatus->avail && driveStatus->removable)
        {
            char buf[PATH_MAX], *ptr;

            LOG_INFO "H1 drive[%d]:disk=%d\n", i, driveStatus->disk LOG_END

            // get file path from list
            strcpy(buf, CFG_UPGRADE_FILENAME_LIST);
            ptr = strtok(buf, " ");
            do
            {
                strcpy(pkgFilePath, driveStatus->name);
                strcat(pkgFilePath, ptr);

                if (itcFileStreamOpen(&fileStream, pkgFilePath, false) == 0)
                {
                    LOG_INFO "H1 Found package file %s\n", pkgFilePath LOG_END
                    return &fileStream.stream;
                }
                else
                {
                    LOG_DBG "H1 try to fopen(%s) fail:0x%X\n", pkgFilePath, errno LOG_END
                }
            }
            while ((ptr = strtok(NULL, " ")) != NULL);
        }
    }
#endif
    for (i = ITP_MAX_DRIVE - 1; i >= 0; i--)
    {
        driveStatus = &driveStatusTable[i];
        if (driveStatus->avail && driveStatus->removable)
        {
            char buf[PATH_MAX], *ptr;

            LOG_INFO "H2 drive[%d]:disk=%d\n", i, driveStatus->disk LOG_END

            // get file path from list
            strcpy(buf, CFG_UPGRADE_FILENAME_LIST_ORG);
            ptr = strtok(buf, " ");
            do
            {
                strcpy(pkgFilePath, driveStatus->name);
                strcat(pkgFilePath, ptr);

                if (itcFileStreamOpen(&fileStream, pkgFilePath, false) == 0)
                {
                    LOG_INFO "H2 Found package file %s\n", pkgFilePath LOG_END
                    return &fileStream.stream;
                }
                else
                {
                    LOG_DBG "H2 try to fopen(%s) fail:0x%X\n", pkgFilePath, errno LOG_END
                }
            }
            while ((ptr = strtok(NULL, " ")) != NULL);
        }
    }    
    LOG_DBG "HJP cannot find package file.\n" LOG_END
    return NULL;
}

#define CFG_UPGRADE_FOLDER "XO_UP.TXT"	//XO_FACUP

void ShowRemoveUSB(void);


bool DeleteUpgradeCheck(void)
{
    int i;
	uint8_t  tfilePath[256] = {0};
	uint8_t  ttfilePath[256] = {0};
	FILE* file = NULL;
ITPDriveStatus* driveStatusTable;
ITPDriveStatus* driveStatus = NULL;
	
    // try to find the package drive
    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_GET_TABLE, &driveStatusTable);

    for (i = ITP_MAX_DRIVE - 1; i >= 0; i--)
    {
        driveStatus = &driveStatusTable[i];
        if (driveStatus->avail && driveStatus->removable)
        {
            char buf[PATH_MAX], *ptr;

          //  LOG_DBG "drive[%d]:disk=%d\n", i, driveStatus->disk LOG_END

            // get file path from list
            
            sprintf(tfilePath, "%s%s", driveStatus->name, CFG_UPGRADE_FOLDER);
  
		 	file = fopen(tfilePath, "r");
		    if(!file)
		    {
		        printf("Can not File : %s \n\n", tfilePath);
		        return 0;
		    }
		    fclose(file);		
		    return 1;
        }
    }

	return 0;
}



bool DeleteUpgradePackage(void)
{
	if (DeleteUpgradeCheck()==0)
	{
	    if (remove(pkgFilePath))
			LOG_ERR "DeleteUpgradePackage : Delete %s fail: %d\n", pkgFilePath, errno LOG_END
		return 0;
    }
    else
    {
    	return 1;
    }
}

void ShowUpgradeFail(void)
{
    int textColor = ITH_RGB565(255, 0, 0);
    char c = 219;

    ioctl(ITP_DEVICE_LCDCONSOLE, ITP_IOCTL_SET_FGCOLOR, (void*)textColor);

    puts("");
    printf("%c%c%c%c%c%c%c %c%c%c%c%c%c  %c%c%c%c%c%c   %c%c%c%c%c%c  %c%c%c%c%c%c  %c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    printf("%c%c      %c%c   %c%c %c%c   %c%c %c%c    %c%c %c%c   %c%c %c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    printf("%c%c%c%c%c   %c%c%c%c%c%c  %c%c%c%c%c%c  %c%c    %c%c %c%c%c%c%c%c  %c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    printf("%c%c      %c%c   %c%c %c%c   %c%c %c%c    %c%c %c%c   %c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    printf("%c%c%c%c%c%c%c %c%c   %c%c %c%c   %c%c  %c%c%c%c%c%c  %c%c   %c%c %c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    puts("");
    puts("UPGRADE FAIL! PLEASE UPGRADE AGAIN!");
    puts("ShowUpgradeFail UPGRADE FAIL! PLEASE UPGRADE AGAIN!");
    puts("UPGRADE FAIL! PLEASE UPGRADE AGAIN!");
}

void ShowLastUpgradeFail(void)
{
    int textColor = ITH_RGB565(255, 0, 0);
    char c = 219;

    ioctl(ITP_DEVICE_LCDCONSOLE, ITP_IOCTL_SET_FGCOLOR, (void*)textColor);

    puts("");
    printf("%c%c%c%c%c%c%c %c%c%c%c%c%c  %c%c%c%c%c%c   %c%c%c%c%c%c  %c%c%c%c%c%c  %c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    printf("%c%c      %c%c   %c%c %c%c   %c%c %c%c    %c%c %c%c   %c%c %c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    printf("%c%c%c%c%c   %c%c%c%c%c%c  %c%c%c%c%c%c  %c%c    %c%c %c%c%c%c%c%c  %c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    printf("%c%c      %c%c   %c%c %c%c   %c%c %c%c    %c%c %c%c   %c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    printf("%c%c%c%c%c%c%c %c%c   %c%c %c%c   %c%c  %c%c%c%c%c%c  %c%c   %c%c %c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    puts("");
    puts("LAST UPGRADE FAIL! PLEASE UPGRADE AGAIN!");
    puts("ShowLastUpgradeFail LAST UPGRADE FAIL! PLEASE UPGRADE AGAIN!");
    puts("LAST UPGRADE FAIL! PLEASE UPGRADE AGAIN!");

    textColor = ITH_RGB565(255, 255, 255);
    ioctl(ITP_DEVICE_LCDCONSOLE, ITP_IOCTL_SET_FGCOLOR, (void*)textColor);
}

void ShowRemoveUSB(void)
{
    int textColor = ITH_RGB565(0, 255, 0);
    char c = 219,i;

    ioctl(ITP_DEVICE_LCDCONSOLE, ITP_IOCTL_SET_FGCOLOR, (void*)textColor);

	for (i=0;i<100;i++)
    	puts("");

    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c);
    puts("");


	textColor = ITH_RGB565(255, 255, 255);
	ioctl(ITP_DEVICE_LCDCONSOLE, ITP_IOCTL_SET_FGCOLOR, (void*)textColor);
}


