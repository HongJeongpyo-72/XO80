#include <sys/ioctl.h>
#include <assert.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include "ite/itp.h"
#include "sports_equipment.h"
#include "scene.h"
#include "ite/ite_esp32_sdio_at.h"

static char *pkgbuffer = NULL;
static int pkgSizeCount;

static bool httpOK = false, fwExist = false;
static sem_t sem_callback_done;
extern ITUProgressBar* updateDownloadProgressBar;

struct esp32_cmd at_cmd = { 0 };
struct esp32_data data;
uint8_t buf[128] = { 0 };

static char *atc[] = {
	"AT+CWMODE=1\r\n",
	"AT+CWJAP=\"SSID\",\"PASSWD\"\r\n",
	"AT+CIPSTART=\"TCP\",\"192.168.191.102\",80\r\n",//2
	"AT+CIPSEND=30\r\n",
	"GET /ITEPKG03.PKG HTTP/1.1\r\n\r\n",//4
	"AT+CIPCLOSE\r\n",
	"AT+CIPSEND=31\r\n",//6
	"HEAD /ITEPKG03.PKG HTTP/1.1\r\n\r\n"
};

static void UpgradeSdioRecv(uint8_t *buf, uint32_t len)
{
	static uint32_t file_size;
	//static uint32_t cnt;

	if (!file_size)
	{
		uint8_t *len_start = NULL;
		//grep file size
		if ((len_start = strstr(buf, "Content-Length: ")) != NULL)
		{
			uint8_t *len_end, c_len[10] = "";
			len_start += 16;
			len_end = strstr(len_start, "\r\n");
			memcpy(c_len, len_start, len_end - len_start);
			file_size = atoi(c_len);
			if (file_size > 0)
			{
				pkgbuffer = malloc(file_size);
				if (pkgbuffer == NULL)
					printf("\npkgbuffer malloc failed!");
				else
					printf("\npkgbuffer malloc success! size: %d\n", file_size);
			}
			pkgSizeCount = 0;
			//cnt = 0;
		}
		else {
			printf("can't find \"Content-Length\"");
			while (1);
		}
	}
	else
	{
		memcpy(pkgbuffer + pkgSizeCount, buf, len);
		pkgSizeCount += len;
		/*if ((cnt++ % 200) == 0)
			printf("download size: %d\n", pkgSize);*/
		ituProgressBarSetValue(updateDownloadProgressBar, (int)(pkgSizeCount * 100 / file_size));
		if (file_size == pkgSizeCount)
		{
			printf("download finish! \n");
			esp32_at_cmd_ipd_end();

			file_size = 0;
			//sem_post(&sem_callback_done);

			esp32_sdio_at_unlock();
			SceneQuit(QUIT_UPGRADE_ESP);
		}
	}
}

void UpgradeSdioCheckFW(uint8_t *buf, int readLen)
{
	uint8_t *p;
	if (readLen > 0)
	{
		if (!fwExist)
		{
			if ((p = strstr(buf, "filename=")) != NULL)
			{
				if (!strncmp(p + 10, "ITEPKG03.PKG", 12))
					fwExist = true;
			}
		}
		esp32_at_cmd_ipd_end();
		sem_post(&sem_callback_done);
	}
}

void UpgradeGetPKG(void)
{
	int rc;
	char buf[128] = { 0 };
	sem_init(&sem_callback_done, 0, 0);
	esp32_sdio_at_lock();

	sprintf(buf, "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", theConfig.hfs, theConfig.port);

	strcpy(at_cmd.cmd_buf, buf);
	at_cmd.flags = ESP32_WAIT_ACK;  // wait "OK" or "ERROR"
	at_cmd.timeout = 5000;
	rc = esp32_at_cmd(&at_cmd);
	if (rc || at_cmd.status) {
		printf("TCP start fail! \n");
		esp32_sdio_at_unlock();
		return;
	}
	strcpy(at_cmd.cmd_buf, atc[3]);  // next HTTP cmd's length
	at_cmd.flags = ESP32_WAIT_FOR_SEND;  // wait '>'
	esp32_at_cmd(&at_cmd);

	strcpy(at_cmd.cmd_buf, atc[4]);
	at_cmd.flags = ESP32_WAIT_ACK;  // wait "SEND OK"
	at_cmd.ipd_cb = UpgradeSdioRecv;
	rc = esp32_at_cmd(&at_cmd);
	if(rc)
		esp32_sdio_at_unlock();
}

bool UpgradeCheckFW(void)
{
	int rc;
	char buf[128] = { 0 };
	sprintf(buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", theConfig.ssid, theConfig.password);

	sem_init(&sem_callback_done, 0, 0);
	esp32_sdio_at_lock();

	strcpy(at_cmd.cmd_buf, atc[0]);
	at_cmd.flags = ESP32_WAIT_ACK;
	esp32_at_cmd(&at_cmd);

	strcpy(at_cmd.cmd_buf, buf);
	at_cmd.flags = ESP32_WAIT_ACK;
	at_cmd.timeout = 10000;
	esp32_at_cmd(&at_cmd);
	if (at_cmd.status) {
		printf("join AP fail! \n");
		goto end;
	}
	at_cmd.timeout = 0; // use default timeout

	memset(buf, 0, 128);
	sprintf(buf, "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", theConfig.hfs, theConfig.port);

	strcpy(at_cmd.cmd_buf, buf);
	at_cmd.flags = ESP32_WAIT_ACK;  // wait "OK" or "ERROR"
	at_cmd.timeout = 5000;
	rc = esp32_at_cmd(&at_cmd);
	if (rc || at_cmd.status) {
		printf("TCP start fail! \n");
		goto end;
	}
	strcpy(at_cmd.cmd_buf, atc[6]);  // next HTTP cmd's length
	at_cmd.flags = ESP32_WAIT_FOR_SEND;  // wait '>'
	esp32_at_cmd(&at_cmd);

	strcpy(at_cmd.cmd_buf, atc[7]);
	at_cmd.flags = ESP32_WAIT_ACK;  // wait "SEND OK"
	at_cmd.ipd_cb = UpgradeSdioCheckFW;
	esp32_at_cmd(&at_cmd);

	sem_wait(&sem_callback_done);
	sem_destroy(&sem_callback_done);

	at_cmd.ipd_cb = NULL;

	strcpy(at_cmd.cmd_buf, atc[5]);
	at_cmd.flags = ESP32_WAIT_ACK;  // wait OK
	esp32_at_cmd(&at_cmd);
end:
	esp32_sdio_at_unlock();
	return fwExist;
}

void UpgradeEsp32Restart(void)
{
	esp32_sdio_at_lock();

	at_cmd.ipd_cb = NULL;

	strcpy(at_cmd.cmd_buf, "AT+RST\r\n");
	at_cmd.flags = ESP32_WAIT_ACK;  // wait "OK" or "ERROR"
	esp32_at_cmd(&at_cmd);

	esp32_sdio_at_unlock();
}

void UpgradeGetPkgPointer(char **pkgBuffer, int *pkgSize)
{
	*pkgBuffer = pkgbuffer;
	*pkgSize = pkgSizeCount;
}