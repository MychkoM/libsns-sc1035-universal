/******************************************************************************
  A driver program of SmartSens SC1035 on HI3518EV100
  SC1035: 1/2.8" 1MP CMOS, 1280x960@30fps, 12-bit parallel DVP, I2C slave 0x30(7bit)/0x60(8bit)
  Register bus: 16-bit sub-address, 8-bit data.
  Reconstructed from the working libsns_sc1035_960p.so (YF1501221) + SC1035 datasheet V1.2
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hi_comm_vi.h"
#include "mpi_vi.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef HI_GPIO_I2C
#include "gpioi2c_ov.h"
#include "gpio_i2c.h"
#else
#include "hi_i2c.h"
#endif

const unsigned int sensor_i2c_addr    = 0x60;        /* 8-bit I2C address (0x30 << 1) of SC1035 */
const unsigned int sensor_addr_byte    = 2;          /* 16-bit register address */
const unsigned int sensor_data_byte    = 1;          /* 8-bit register data */

int sensor_read_register(int addr)
{
#ifdef HI_GPIO_I2C
    int fd = -1;
    int ret;
    int value;

    fd = open("/dev/gpioi2c_ov", 0);
    if(fd<0)
    {
        printf("Open gpioi2c_ov error!\n");
        return -1;
    }

    value = ((sensor_i2c_addr&0xff)<<24) | ((addr&0xffff)<<8);

    ret = ioctl(fd, GPIO_I2C_READ, &value);
    if (ret)
    {
        printf("GPIO-I2C read faild!\n");
        close(fd);
        return -1;
    }

    value &= 0xff;

    close(fd);
    return value;
#else
    int fd = -1;
    int ret;
    I2C_DATA_S i2c_data;

    fd = open("/dev/hi_i2c", 0);
    if(fd<0)
    {
        printf("Open hi_i2c error!\n");
        return -1;
    }

    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sensor_addr_byte;
    i2c_data.data_byte_num = sensor_data_byte;

    ret = ioctl(fd, CMD_I2C_READ, &i2c_data);

    if (ret)
    {
        printf("hi_i2c read faild!\n");
        close(fd);
        return -1;
    }

    close(fd);
#endif

    return i2c_data.data;
}

int sensor_write_register(int addr, int data)
{
#ifdef HI_GPIO_I2C
    int fd = -1;
    int ret;
    int value;

    fd = open("/dev/gpioi2c_ov", 0);
    if(fd<0)
    {
        printf("Open gpioi2c_ov error!\n");
        return -1;
    }

    value = ((sensor_i2c_addr&0xff)<<24) | ((addr&0xffff)<<8) | (data&0xff);

    ret = ioctl(fd, GPIO_I2C_WRITE, &value);
    if (ret)
    {
        printf("GPIO-I2C write faild!\n");
        close(fd);
        return -1;
    }

    close(fd);
#else
    int fd = -1;
    int ret;
    I2C_DATA_S i2c_data;

    fd = open("/dev/hi_i2c", 0);
    if(fd<0)
    {
        printf("Open hi_i2c error!\n");
        return -1;
    }

    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sensor_addr_byte;
    i2c_data.data = data;
    i2c_data.data_byte_num = sensor_data_byte;

    ret = ioctl(fd, CMD_I2C_WRITE, &i2c_data);
    if (ret)
    {
        printf("hi_i2c write faild!\n");
        close(fd);
        return -1;
    }

    close(fd);
#endif
    return 0;
}

int sensor_write_register_bit(int addr, int data, int mask)
{
#ifdef HI_GPIO_I2C
    int fd = -1;
    int ret;
    int value;

    fd = open("/dev/gpioi2c_ov", 0);
    if(fd<0)
    {
        printf("Open gpioi2c_ov error!\n");
        return -1;
    }

    value = ((sensor_i2c_addr&0xff)<<24) | ((addr&0xffff)<<8);

    ret = ioctl(fd, GPIO_I2C_READ, &value);
    if (ret)
    {
        printf("GPIO-I2C read faild!\n");
        close(fd);
        return -1;
    }

    value &= 0xff;
    value &= ~mask;
    value |= data & mask;

    value = ((sensor_i2c_addr&0xff)<<24) | ((addr&0xffff)<<8) | (value&0xff);

    ret = ioctl(fd, GPIO_I2C_WRITE, &value);
    if (ret)
    {
        printf("GPIO-I2C write faild!\n");
        close(fd);
        return -1;
    }

    close(fd);
#else
    int fd = -1;
    int ret;
    int value;
    I2C_DATA_S i2c_data;

    fd = open("/dev/hi_i2c", 0);
    if(fd<0)
    {
        printf("Open hi_i2c error!\n");
        return -1;
    }

    i2c_data.dev_addr = sensor_i2c_addr;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = sensor_addr_byte;
    i2c_data.data_byte_num = sensor_data_byte;

    ret = ioctl(fd, CMD_I2C_READ, &i2c_data);
    if (ret)
    {
        printf("hi_i2c read faild!\n");
        close(fd);
        return -1;
    }

    value = i2c_data.data;
    value &= ~mask;
    value |= data & mask;

    i2c_data.data = value;

    ret = ioctl(fd, CMD_I2C_WRITE, &i2c_data);
    if (ret)
    {
        printf("hi_i2c write faild!\n");
        close(fd);
        return -1;
    }

    close(fd);
#endif
    return 0;
}

static void delay_ms(int ms)
{
    usleep(ms*1000);
}

void sensor_prog(int* rom)
{
    int i = 0;
    while (1)
    {
        int u32Lookup = rom[i++];
        int addr = (u32Lookup >> 16) & 0xFFFF;
        int data = u32Lookup & 0xFFFF;
        if (addr == 0xFFFE)
        {
            delay_ms(data);
        }
        else if (addr == 0xFFFF)
        {
            return;
        }
        else
        {
            sensor_write_register(addr, data);
        }
    }
}

/* SC1035 1280x960@30fps register sequence (verified live on the reference board;
   matches the YF1501221 libsns_sc1035_960p.so init + SC1035 datasheet V1.2) */
/* ===== v1.4.0 STANDARD MODE SOURCE =====
   Режим либы берёт из САМОГО MPP, а не из конфигов приложений:
   приоритет = API sc1035_set_mode() > VI-интроспекция (HI_MPI_VI_GetChnAttr —
   официальный MPI; любой app — majestic/сэмпл/наш flow — обязан сконфигурировать
   VI_CHN_ATTR, либа читает ЕГО конфиг) > env SC1035_ISP_H/SC1035_FPS > default.
   majestic ini НЕ читается вовсе (закрытая логика может меняться — либе всё равно).
   ABI-слоты set_image_mode/set_resolution в MPP V1.0.9 мертвы (0 вызовов в libisp,
   вендор держит NULL) — потому стандартный путь = VI-запрос + рантайм cmos_fps_set. */
unsigned int g_sns_ispH = 960;  /* геометрия VI-канала приложения */
unsigned int g_sns_fps  = 30;   /* стартовый fps; рантайм уточнит cmos_fps_set */

#define SNS_LINES_PER_SEC 30000u  /* PLL 54МГц / HTS 1800 */
#define SNS_VTS_MIN       1000u  /* readout+blank (доказано) => авто-макс 30fps */
#define SNS_VTS_MAX       6000u  /* => авто-мин 5fps */

unsigned int sc1035_vts_for_fps(unsigned int fps)
{
    unsigned int vts;
    if (fps < 5)  fps = 5;
    if (fps > 30) fps = 30;
    vts = (SNS_LINES_PER_SEC + fps / 2) / fps;  /* round(30000/fps) */
    if (vts < SNS_VTS_MIN) vts = SNS_VTS_MIN;
    if (vts > SNS_VTS_MAX) vts = SNS_VTS_MAX;
    return vts;
}

static int    g_sns_mode_src_set = 0;   /* режим задан API — VI/env не переопределяют */
static const char *g_sns_mode_src = "default";

/* v1.3.1+: экспорт для приложения (опционально, до sensor_init):
   явная установка wins над VI-интроспекцией. */
int sc1035_set_mode(unsigned int ispH, unsigned int fps)
{
    if (ispH) g_sns_ispH = (ispH <= 720) ? 720 : 960;
    if (fps)  g_sns_fps  = fps;
    g_sns_mode_src_set = 1;
    g_sns_mode_src = "api";
    return 0;
}

static void sns_read_mode(void)
{
    VI_CHN_ATTR_S stViChnAttr;
    const char *e;

    /* 1) API sc1035_set_mode() уже зван — ничего не трогаем */
    if (g_sns_mode_src_set)
        goto fps_hint;

    /* 2) СТАНДАРТ MPP: геометрия из VI-конфига приложения (GetChnAttr).
       Символ резолвится из процесса-хоста (libmpi загружен у любого MPP-app). */
    if (HI_MPI_VI_GetChnAttr(0, &stViChnAttr) == HI_SUCCESS
        && stViChnAttr.stCapRect.u32Height > 0)
    {
        g_sns_ispH = (stViChnAttr.stCapRect.u32Height <= 720) ? 720 : 960;
        g_sns_mode_src_set = 1;
        g_sns_mode_src = "vi(mpp)";
    }
    else
    {
        /* 3) env — если VI ещё не сконфигурирован (ранний init) */
        e = getenv("SC1035_ISP_H");
        if (e && *e) { g_sns_ispH = (unsigned int)atoi(e); g_sns_mode_src_set = 1; }
        if (g_sns_mode_src_set) g_sns_mode_src = "env";
    }

fps_hint:
    /* стартовый fps: env-подсказка (рантайм перекроет cmos_fps_set приложения) */
    e = getenv("SC1035_FPS");
    if (e && *e) g_sns_fps = (unsigned int)atoi(e);

    printf("[camlib v1.4.0] universal: src=%s Isp_H=%u fps=%u -> %s window, VTS=%u => %u.%02u fps\n",
           g_sns_mode_src, g_sns_ispH, g_sns_fps,
           (g_sns_ispH <= 720) ? "720-crop" : "960-full",
           sc1035_vts_for_fps(g_sns_fps),
           30000u / sc1035_vts_for_fps(g_sns_fps),
           (30000u % sc1035_vts_for_fps(g_sns_fps)) * 100u / sc1035_vts_for_fps(g_sns_fps));
}

void sensor_init()
{
    sns_read_mode();  /* v1.3.0: режим до записи регистров */
    /* EXACT mark4h primary sensor_init() (HI3518-SC1035 repo, vi_bayerdump_SC1035/SC1035_sensor_ctl.c),
       720p HTS=0x708=1800, AEC/AGC AUTO (0x3e03=0x00) -> bright image. PROVEN capture. */
    sensor_write_register(0x3000, 0x1); /* soft reset */
    sensor_write_register(0x3003, 0x1);
    usleep(20000);
    sensor_write_register(0x3400, 0x53);
    sensor_write_register(0x3416, 0xC0);
    sensor_write_register(0x3d08, 0x00);
    sensor_write_register(0x5000, 0x09);
    /* 2026-09-17 fix: AEC/AGC MANUAL (0x03) — как в вендорской libsns_sc1035_960p.so.
       AUTO (0x00) даёт ЧЁРНЫЙ кадр (Y=33): внутренний AEC сенсора крутит exp/gain в
       регистрах, но к кадру они НЕ применяются. В manual экспозицией/gain управляет
       внешний ISP-колбэк (cmos_inttime_update/cmos_gains_update) — картинка нормальная. */
    sensor_write_register(0x3e03, 0x03); /* AEC/AGC manual (управляет ISP AE) */
    sensor_write_register(0x3928, 0x00);
    sensor_write_register(0x3612, 0x00);
    sensor_write_register(0x3620, 0x44);
    sensor_write_register(0x3622, 0x2e);
    sensor_write_register(0x3630, 0x58);
    sensor_write_register(0x3631, 0x80);
    sensor_write_register(0x3632, 0x41);
    sensor_write_register(0x3633, 0x7c);
    sensor_write_register(0x3635, 0x04);
    sensor_write_register(0x3500, 0x10);
    sensor_write_register(0x3780, 0x0b);
    sensor_write_register(0x3300, 0x33);
    sensor_write_register(0x3301, 0x61);
    sensor_write_register(0x3302, 0x30);
    sensor_write_register(0x3303, 0x56);
    sensor_write_register(0x3304, 0xA0);
    sensor_write_register(0x3305, 0x72);
    sensor_write_register(0x331e, 0x56);
    sensor_write_register(0x321e, 0x00);
    sensor_write_register(0x321f, 0x0a);
    sensor_write_register(0x3216, 0x0a);
    sensor_write_register(0x3115, 0x0a);
    sensor_write_register(0x3332, 0x38);
    sensor_write_register(0x5054, 0x82);
    sensor_write_register(0x3907, 0x01);
    sensor_write_register(0x3908, 0xc0);
    sensor_write_register(0x3e0f, 0x14);
    sensor_write_register(0x3223, 0x22); /* REQUIRED */
    sensor_write_register(0x3010, 0x07);
    sensor_write_register(0x3011, 0x46);
    sensor_write_register(0x3004, 0x04);
    sensor_write_register(0x320c, 0x07); /* HTS 0x708 = 1800 */
    sensor_write_register(0x320d, 0x08);
    sensor_write_register(0x320e, 0x03); /* VTS 0x3e8 */
    sensor_write_register(0x320f, 0xe8);
    /* .168 port: 960p — ПОЛНАЯ последовательность HISI-референса libsns_sc1035_960p.so
       (hi3516cv100 SDK). Идёт ПОСЛЕ mark4h-базы выше, поэтому окончательное состояние = точный
       HISI-ref (включая AE manual 0x3e03=0x03 — экспозицией управляет ISP-колбэк). */
    /* 2026-09-18 fps: PLL вернули 07/46 (стабильно 12.5). 21/86+3004=1F из SDK-примера
       на этой прошивке НЕ работает (сенсор уходит в 1fps/8fps) - пример для другого MCLK. */
    sensor_write_register(0x3010, 0x07);
    sensor_write_register(0x3011, 0x46);
    sensor_write_register(0x3004, 0x04);
    sensor_write_register(0x3202, 0x00);
    sensor_write_register(0x3203, 0x08);  /* Y_START 0x0008 */
    sensor_write_register(0x3206, 0x03);
    sensor_write_register(0x3207, 0xcf);  /* Y_END 0x03cf */
    sensor_write_register(0x3208, 0x05);
    sensor_write_register(0x3209, 0x00);  /* XOUT 0x0500 = 1280 */
    sensor_write_register(0x320a, 0x03);
    sensor_write_register(0x320b, 0xc0);  /* YOUT 0x03c0 = 960 */
    sensor_write_register(0x3400, 0x53);
    sensor_write_register(0x3416, 0xc0);
    sensor_write_register(0x3780, 0x0b);
    sensor_write_register(0x3d08, 0x00);
    sensor_write_register(0x3223, 0x22);  /* REQUIRED */
    sensor_write_register(0x5000, 0x00);
    sensor_write_register(0x3e03, 0x03);  /* AEC/AGC manual (управляет ISP AE) */
    sensor_write_register(0x3e0e, 0x50);
    sensor_write_register(0x3e0f, 0x14);
    sensor_write_register(0x3500, 0x10);
    sensor_write_register(0x3928, 0x00);
    sensor_write_register(0x3620, 0x44);
    sensor_write_register(0x3622, 0x26);
    sensor_write_register(0x3630, 0x58);
    sensor_write_register(0x3635, 0x04);
    sensor_write_register(0x3633, 0x7c);
    sensor_write_register(0x3612, 0x00);
    sensor_write_register(0x3632, 0x41);
    sensor_write_register(0x3631, 0x80);
    sensor_write_register(0x3300, 0x33);
    sensor_write_register(0x3302, 0x30);
    sensor_write_register(0x3303, 0x56);
    sensor_write_register(0x3304, 0xa0);
    sensor_write_register(0x3305, 0x72);
    sensor_write_register(0x331e, 0x56);
    sensor_write_register(0x321f, 0x0a);
    sensor_write_register(0x3216, 0x0a);
    sensor_write_register(0x3115, 0x0a);
    sensor_write_register(0x3332, 0x38);
    sensor_write_register(0x3210, 0x00);
    sensor_write_register(0x3211, 0x62);  /* HOFFS 0x0062 */
    sensor_write_register(0x3212, 0x00);
    /* v1.3.0 UNIVERSAL: окно из ini [isp_image] Isp_H.
       720-кроп (v1.2.4-проверенный): VOFFS 0x80 — ЧЁТНЫЙ (нечётный сдвигал readout
       на строку, ломая фазу Bayer RG/GB => маджента на белом), YOUT 0x02d0;
       960-полное: VOFFS 0x08, YOUT 0x03c0 (записан выше). */
    if (g_sns_ispH <= 720) {
        sensor_write_register(0x3213, 0x80);  /* VOFFS 0x0080 (чётный) */
        sensor_write_register(0x320a, 0x02);  /* YOUT 0x02d0 = 720 */
        sensor_write_register(0x320b, 0xd0);
    } else {
        sensor_write_register(0x3213, 0x08);  /* VOFFS 0x0008 (полное окно 960) */
    }
    sensor_write_register(0x5054, 0x82);
    sensor_write_register(0x320c, 0x07);  /* HTS 0x0708 = 1800 */
    sensor_write_register(0x320d, 0x08);
    /* v1.3.0 UNIVERSAL: VTS из ini-fps (VTS-закон); cmos_fps_set скорректирует
       при смене fps в рантайме. Авто-границы [1000..6000] => fps [30..5]. */
    {
        unsigned int vts = sc1035_vts_for_fps(g_sns_fps);
        sensor_write_register(0x320e, (vts >> 8) & 0xff);
        sensor_write_register(0x320f, vts & 0xff);
    }
    sensor_write_register(0x3907, 0x02);
    sensor_write_register(0x3908, 0x00);
    sensor_write_register(0x3600, 0x54);
    sensor_write_register(0x3600, 0x7c);
    /* 2026-09-17/18: вендорский финальный блок (из i2c-лога libsns_sc1035_960p_vendor.so, дважды:
       3610=3b/3600=54 затем 3610=03/3600=7c). Без него сенсор в дефолтных таймингах
       (sa1-окно 0x3330/0x3320-23, выборка 0x3315 bit6+bit2, 0x3301) - неверный цвет при gain (пурпур). */
    sensor_write_register(0x3330, 0x0d);
    sensor_write_register(0x3320, 0x06);
    sensor_write_register(0x3321, 0x18);
    sensor_write_register(0x3322, 0x01);
    sensor_write_register(0x3323, 0xf0);
    sensor_write_register(0x3315, 0x44);
    sensor_write_register(0x3301, 0x83);
    sensor_write_register(0x3610, 0x03);
    sensor_write_register(0x3600, 0x7c);
}
