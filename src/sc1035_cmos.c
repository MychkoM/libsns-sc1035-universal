#if !defined(__SC1035_CMOS_H_)
#define __SC1035_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hi_comm_sns.h"
#include "hi_comm_isp.h"
#include "hi_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_af.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define SC1035_ID 130

#define CMOS_SC1035_ISP_WRITE_SENSOR_ENABLE (1)

/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/

extern const unsigned int sensor_i2c_addr;
extern unsigned int sensor_addr_byte;
extern unsigned int sensor_data_byte;

HI_U8 gu8SensorMode = 0;

static HI_U32 gu32FullLinesStd = 1000;
static HI_U32 gu32FullLines = 1000;

/* v1.3.0 universal: режим из ini (см. sc1035_sensor_ctl.c sns_read_mode) */
extern unsigned int g_sns_ispH;
extern unsigned int g_sns_fps;
extern unsigned int sc1035_vts_for_fps(unsigned int fps);

#if CMOS_SC1035_ISP_WRITE_SENSOR_ENABLE
ISP_SNS_REGS_INFO_S g_stSnsRegsInfo = {0};
#endif

/* ----- AWB default: recovered from the reference libsns_sc1035_960p.so ----- */
static AWB_CCM_S g_stAwbCcm =
{
    5700,
    {   0x01db, 0x80bc, 0x801f,
        0x804d, 0x019f, 0x8052,
        0x0014, 0x8128, 0x0213
    },
    3315,
    {   0x01b8, 0x8071, 0x8047,
        0x807d, 0x01c5, 0x8048,
        0x8004, 0x8173, 0x0277
    },
    2580,
    {   0x01de, 0x8085, 0x8059,
        0x8076, 0x01bf, 0x8049,
        0x8025, 0x8244, 0x0369
    }
};

static AWB_AGC_TABLE_S g_stAwbAgcTable =
{
    /* bvalid */
    1,

    /* saturation (vendor lib sdk .so @0x1F40: 8c 8c 87 82 78 6e 5a 3c) */
    {0x8c,0x8c,0x87,0x82,0x78,0x6e,0x5a,0x3c}
};

static ISP_CMOS_AGC_TABLE_S g_stIspAgcTable =
{
    /* bvalid */
    1,

    /* sharpen_alt_d */
    {0x87,0x87,0x82,0x7d,0x78,0x5a,0x32,0x28},

    /* sharpen_alt_ud */
    {0xfa,0xfa,0xf0,0xe6,0xdc,0xa0,0x50,0x3c},

    /* snr_thresh */
    {0x06,0x09,0x0b,0x0e,0x12,0x1a,0x28,0x32},

    /* demosaic_lum_thresh */
    {0x40,0x60,0x80,0x80,0x80,0x80,0x80,0x80},

    /* demosaic_np_offset */
    {0x00,0x07,0x0d,0x15,0x1e,0x28,0x30,0x30},

    /* ge_strength */
    {0x55,0x55,0x55,0x55,0x55,0x55,0x37,0x37}
};

static ISP_CMOS_NOISE_TABLE_S g_stIspNoiseTable =
{
    /* bvalid */
    1,

    /* noise_profile_weight_lut */
    {
     0x19,0x20,0x24,0x27,0x29,0x2b,0x2d,0x2e,0x2f,0x31,0x32,0x33,0x34,0x34,0x35,0x36,0x37,\
    0x37,0x38,0x38,0x39,0x3a,0x3a,0x3b,0x3b,0x3b,0x3c,0x3c,0x3d,0x3d,0x3d,0x3e,0x3e,0x3e,\
    0x3f,0x3f,0x3f,0x40,0x40,0x40,0x41,0x41,0x41,0x41,0x42,0x42,0x42,0x42,0x43,0x43,0x43,\
    0x43,0x44,0x44,0x44,0x44,0x44,0x45,0x45,0x45,0x45,0x45,0x46,0x46,0x46,0x46,0x46,0x46,\
    0x47,0x47,0x47,0x47,0x47,0x47,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x49,0x49,0x49,0x49,\
    0x49,0x49,0x49,0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x4b,0x4b,0x4b,0x4b,0x4b,\
    0x4b,0x4b,0x4b,0x4c,0x4c,0x4c,0x4c,0x4c,0x4c,0x4c,0x4c,0x4c,0x4c,0x4d,0x4d,0x4d,0x4d,\
    0x4d,0x4d,0x4d,0x4d,0x4d,0x4d,0x4e,0x4e,0x4e
    },

    /* demosaic_weight_lut */
    {
        0x19,0x20,0x24,0x27,0x29,0x2b,0x2d,0x2e,0x2f,0x31,0x32,0x33,0x34,0x34,0x35,0x36,0x37,\
    0x37,0x38,0x38,0x39,0x3a,0x3a,0x3b,0x3b,0x3b,0x3c,0x3c,0x3d,0x3d,0x3d,0x3e,0x3e,0x3e,\
    0x3f,0x3f,0x3f,0x40,0x40,0x40,0x41,0x41,0x41,0x41,0x42,0x42,0x42,0x42,0x43,0x43,0x43,\
    0x43,0x44,0x44,0x44,0x44,0x44,0x45,0x45,0x45,0x45,0x45,0x46,0x46,0x46,0x46,0x46,0x46,\
    0x47,0x47,0x47,0x47,0x47,0x47,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0x49,0x49,0x49,0x49,\
    0x49,0x49,0x49,0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x4a,0x4b,0x4b,0x4b,0x4b,0x4b,\
    0x4b,0x4b,0x4b,0x4c,0x4c,0x4c,0x4c,0x4c,0x4c,0x4c,0x4c,0x4c,0x4c,0x4d,0x4d,0x4d,0x4d,\
    0x4d,0x4d,0x4d,0x4d,0x4d,0x4d,0x4e,0x4e,0x4e
    }
};

static ISP_CMOS_DEMOSAIC_S g_stIspDemosaic =
{
    /* bvalid */
    1,

    /*vh_slope*/
    0xdc,

    /*aa_slope*/
    0xc8,

    /*va_slope*/
    0xb9,

    /*uu_slope*/
    0xd2,

    /*sat_slope*/
    0x5d,

    /*ac_slope*/
    0xa0,

    /*vh_thresh*/
    0x00,

    /*aa_thresh*/
    0x00,

    /*va_thresh*/
    0x08,

    /*uu_thresh*/
    0x00,

    /*sat_thresh*/
    0x1b3,

    /*ac_thresh*/
    0x00
};

static ISP_CMOS_GAMMA_S g_stIspGamma =
{
    /* bvalid */
    1,

    /* au16Gamma[GAMMA_NODE_NUMBER] == same curve as in vendor libsns_sc1035_960p.so (verified byte-for-byte, 2026-09-17; equals AR0130 curve) */
    {
    0,120,220,310,390,470,540,610,670,730,786,842,894,944,994,1050,
    1096,1138,1178,1218,1254,1280,1314,1346,1378,1408,1438,1467,1493,1519,1543,1568,
    1592,1615,1638,1661,1683,1705,1726,1748,1769,1789,1810,1830,1849,1869,1888,1907,
    1926,1945,1963,1981,1999,2017,2034,2052,2069,2086,2102,2119,2136,2152,2168,2184,
    2200,2216,2231,2247,2262,2277,2292,2307,2322,2337,2351,2366,2380,2394,2408,2422,
    2436,2450,2464,2477,2491,2504,2518,2531,2544,2557,2570,2583,2596,2609,2621,2634,
    2646,2659,2671,2683,2696,2708,2720,2732,2744,2756,2767,2779,2791,2802,2814,2825,
    2837,2848,2859,2871,2882,2893,2904,2915,2926,2937,2948,2959,2969,2980,2991,3001,
    3012,3023,3033,3043,3054,3064,3074,3085,3095,3105,3115,3125,3135,3145,3155,3165,
    3175,3185,3194,3204,3214,3224,3233,3243,3252,3262,3271,3281,3290,3300,3309,3318,
    3327,3337,3346,3355,3364,3373,3382,3391,3400,3409,3418,3427,3436,3445,3454,3463,
    3471,3480,3489,3498,3506,3515,3523,3532,3540,3549,3557,3566,3574,3583,3591,3600,
    3608,3616,3624,3633,3641,3649,3657,3665,3674,3682,3690,3698,3706,3714,3722,3730,
    3738,3746,3754,3762,3769,3777,3785,3793,3801,3808,3816,3824,3832,3839,3847,3855,
    3862,3870,3877,3885,3892,3900,3907,3915,3922,3930,3937,3945,3952,3959,3967,3974,
    3981,3989,3996,4003,4010,4018,4025,4032,4039,4046,4054,4061,4068,4075,4082,4089,
    4095,
    }
};

static HI_U32 cmos_get_isp_default(ISP_CMOS_DEFAULT_S *pstDef)
{
    if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }

    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));

    pstDef->stComm.u8Rggb           = 0x3;      //3: BGGR (нормализация цветов eb16179)
    pstDef->stComm.u8BalanceFe      = 0x1;

    pstDef->stDenoise.u8SinterThresh= 0x15;
    pstDef->stDenoise.u8NoiseProfile= 0x0;      //0: use default profile table
    pstDef->stDenoise.u16Nr0        = 0x0;
    pstDef->stDenoise.u16Nr1        = 0x0;

    pstDef->stDrc.u8DrcBlack        = 0x0;
    pstDef->stDrc.u8DrcVs           = 0x04;
    pstDef->stDrc.u8DrcVi           = 0x08;
    pstDef->stDrc.u8DrcSm           = 0xa0;
    pstDef->stDrc.u16DrcWl          = 0x4ff;

    memcpy(&pstDef->stAgcTbl, &g_stIspAgcTable, sizeof(ISP_CMOS_AGC_TABLE_S));
    memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTable, sizeof(ISP_CMOS_NOISE_TABLE_S));
    memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
    memcpy(&pstDef->stGamma, &g_stIspGamma, sizeof(ISP_CMOS_GAMMA_S)); // 2026-09-17: гамма из вендорской .so (257 pts, verified)

    return 0;
}

static HI_U32 cmos_get_isp_black_level(ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel)
{
    if (HI_NULL == pstBlackLevel)
    {
        printf("null pointer when get isp black level value!\n");
        return -1;
    }

    pstBlackLevel->bUpdate = HI_FALSE;
    pstBlackLevel->au16BlackLevel[0] = 194;
    pstBlackLevel->au16BlackLevel[1] = 194;
    pstBlackLevel->au16BlackLevel[2] = 194;
    pstBlackLevel->au16BlackLevel[3] = 193;

    return 0;
}

static HI_VOID cmos_set_pixel_detect(HI_BOOL bEnable)
{
    if (HI_FALSE == bEnable)
    {
        printf("pixel detect enter!\n");
        /* use default 960P30 timing */
        sensor_write_register(0x320e, 0x03);
        sensor_write_register(0x320f, 0xe8);
    }
    else
    {
        printf("pixel detect not used\n");
    }

    return;
}

static HI_VOID cmos_set_wdr_mode(HI_U8 u8Mode)
{
    switch(u8Mode)
    {
        //sensor mode 0
        case 0:
            gu8SensorMode = 0;
        break;
        //sensor mode 1
        case 1:
            gu8SensorMode = 1;
        break;

        default:
            printf("NOT support this mode!\n");
            return;
        break;
    }

    return;
}

static HI_S32 cmos_get_ae_default(AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (HI_NULL == pstAeSnsDft)
    {
        printf("null pointer when get ae default value!\n");
        return -1;
    }

    /* v1.3.0 UNIVERSAL: VTS из текущего режима (ini fps; дефолт 30 => 1000).
       Линий/сек = 30000 всегда (PLL 54МГц/HTS 1800) => LinesPer500ms = 15000
       независимо от fps; fps AE выводит из 15000*2/FullLinesStd. */
    {
        HI_U32 vts = sc1035_vts_for_fps(g_sns_fps);
        gu32FullLinesStd = vts;
        pstAeSnsDft->u32LinesPer500ms = 15000;
        pstAeSnsDft->u32FullLinesStd = vts;
        pstAeSnsDft->u32FlickerFreq = 0;

        pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
        pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;
        pstAeSnsDft->u32MaxIntTime = vts - 2;
        pstAeSnsDft->u32MinIntTime = 2;
    }
    pstAeSnsDft->u32MaxIntTimeTarget = 65535;
    pstAeSnsDft->u32MinIntTimeTarget = 2;

    pstAeSnsDft->au8HistThresh[0] = 0x0d;
    pstAeSnsDft->au8HistThresh[1] = 0x28;
    pstAeSnsDft->au8HistThresh[2] = 0x60;
    pstAeSnsDft->au8HistThresh[3] = 0x80;

    pstAeSnsDft->u8AeCompensation = 0x40;

    /* AGC: 0.0588 approx = 1/17 linear step, 3.76x ~ 100x */
    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.0588f;
    pstAeSnsDft->u32MaxAgain = 1703;
    pstAeSnsDft->u32MinAgain = 64;
    pstAeSnsDft->u32MaxAgainTarget = 1703;
    pstAeSnsDft->u32MinAgainTarget = 64;

    /* Dgain not used (sensor max 4x dig gain only at top again) */
    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 1;
    pstAeSnsDft->u32MaxDgain = 1;
    pstAeSnsDft->u32MinDgain = 1;
    pstAeSnsDft->u32MaxDgainTarget = 1;
    pstAeSnsDft->u32MinDgainTarget = 1;

    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MaxISPDgainTarget = 4 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;

    return 0;
}

static HI_S32 cmos_get_sensor_max_resolution(ISP_CMOS_SENSOR_MAX_RESOLUTION *pstSensorMaxResolution)
{
    if (HI_NULL == pstSensorMaxResolution)
    {
        printf("null pointer when get sensor max resolution \n");
        return -1;
    }

    memset(pstSensorMaxResolution, 0, sizeof(ISP_CMOS_SENSOR_MAX_RESOLUTION));

    pstSensorMaxResolution->u32MaxWidth  = 1280;
    pstSensorMaxResolution->u32MaxHeight = 960;

    return 0;
}

/* the function of sensor set fps */
static HI_VOID cmos_fps_set(HI_U8 u8Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    /* v1.3.0 UNIVERSAL: VTS-закон при единственном PLL 0x46 (54МГц), HTS 1800:
       fps = 30000/VTS. Авто-границы VTS [1000..6000] => запрошенный fps клампится
       в [30..5] (макс = readout+blank сенсора, мин = sane-пол), AE-поля под факт.
       Примеры: 30->VTS1000, 25->1200, 20->1500, 18->1667, 15->2000, 10->3000, 5->6000. */
    HI_U32 vts = sc1035_vts_for_fps(u8Fps);

    gu32FullLinesStd = vts;
    pstAeSnsDft->u32FullLinesStd  = vts;
    pstAeSnsDft->u32MaxIntTime    = vts - 2;
    pstAeSnsDft->u32LinesPer500ms = 15000;  /* 30000 линий/с -> 15000 за 500мс (всегда) */

    sensor_write_register(0x3011, 0x46);   /* 54 МГц — единственный PLL */
    sensor_write_register(0x320e, (vts >> 8) & 0xff);
    sensor_write_register(0x320f, vts & 0xff);

    return;
}

static HI_VOID cmos_slow_framerate_set(HI_U16 u16FullLines,
    AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    if (u16FullLines > 6000) u16FullLines = 6000;  /* v1.3.0: авто-пол 5fps (VTS-закон) */
    sensor_write_register(0x320e, u16FullLines >> 8);
    sensor_write_register(0x320f, u16FullLines & 0xff);

    pstAeSnsDft->u32MaxIntTime = u16FullLines - 2;

    return;
}

#if CMOS_SC1035_ISP_WRITE_SENSOR_ENABLE
static HI_VOID cmos_init_regs_info(HI_VOID)
{
    HI_S32 i;
    static HI_BOOL bInit = HI_FALSE;

    if (HI_FALSE == bInit)
    {
        g_stSnsRegsInfo.enSnsType = ISP_SNS_I2C_TYPE;
        g_stSnsRegsInfo.u32RegNum = 4;
        g_stSnsRegsInfo.bDelayCfgIspDgain = HI_FALSE;
        for (i=0; i<4; i++)
        {
            g_stSnsRegsInfo.astI2cData[i].u8DevAddr = sensor_i2c_addr;
            g_stSnsRegsInfo.astI2cData[i].u32AddrByteNum = sensor_addr_byte;
            g_stSnsRegsInfo.astI2cData[i].u32DataByteNum = sensor_data_byte;
        }
        g_stSnsRegsInfo.astI2cData[0].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[0].u32RegAddr = 0x3e01; /* exposure[15:8] */
        g_stSnsRegsInfo.astI2cData[1].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[1].u32RegAddr = 0x3e02; /* exposure[7:0] */
        g_stSnsRegsInfo.astI2cData[2].bDelayCfg = HI_FALSE;
        g_stSnsRegsInfo.astI2cData[2].u32RegAddr = 0x3e09; /* SNR gain */
        g_stSnsRegsInfo.astI2cData[3].bDelayCfg = HI_TRUE;
        g_stSnsRegsInfo.astI2cData[3].u32RegAddr = 0x3e0f; /* dig gain ctrl */

        bInit = HI_TRUE;
    }
    return;
}
#endif

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(HI_U32 u32IntTime)
{
#if CMOS_SC1035_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();
    /* exposure is 20-bit; lower 4 bits of 0x3e02 are fraction of a line, do not use.
       We get a 20-bit line value with 4 fractional bits: exp[15:8]->0x3e01, exp[7:4]<<4 -> 0x3e02 */
    g_stSnsRegsInfo.astI2cData[0].u32Data = (u32IntTime >> 4) & 0xff;
    g_stSnsRegsInfo.astI2cData[1].u32Data = (u32IntTime << 4) & 0xff;
#ifdef HI_MPI_ISP_SnsRegsCfg
    HI_MPI_ISP_SnsRegsCfg(&g_stSnsRegsInfo);
#endif
#else
    sensor_write_register(0x3e01, (u32IntTime >> 4) & 0xff);
    sensor_write_register(0x3e02, (u32IntTime << 4) & 0xff);
#endif
    return;
}

static HI_VOID cmos_gains_update(HI_U32 u32Again, HI_U32 u32Dgain)
{
    HI_U8 u8SnrGain, u8Dgc;

    /* SC1035 SNR gain register 0x3e09: bit[3:0] fine gain, bit[6:4] sa1 gain
       digital gain 0x3e0f bit[1:0] (r_dig_gain_sel1=1 default -> manual dig gain bits) */
    (void)u32Dgain;

    if (u32Again <= 63)
    {
        u8SnrGain = 0x00;
        u8Dgc     = 0x00;
    }
    else if (u32Again <= 119)
    {
        u8SnrGain = ((17*u32Again - 1088) >> 6) & 0xf;
        u8Dgc     = 0x00;
    }
    else if (u32Again <= 225)
    {
        u8SnrGain = (HI_U8)((((HI_U32)(((HI_U64)17*(HI_U32)u32Again * 0x8d3dcb09UL) >> 38)) - 17) & 0xf) | 0x80;
        u8Dgc     = 0x00;
    }
    else if (u32Again <= 452)
    {
        u8SnrGain = (HI_U8)((((HI_U32)(((HI_U64)17*(HI_U32)u32Again * 0x487ede05UL) >> 37)) - 17) & 0xf) | 0x90;
        u8Dgc     = 0x00;
    }
    else if (u32Again <= 905)
    {
        u8SnrGain = (HI_U8)((((HI_U32)(((HI_U64)17*(HI_U32)u32Again * 0x21579805UL) >> 36)) - 17) & 0xf) | 0xb0;
        u8Dgc     = 0x00;
    }
    else if (u32Again <= 1811)
    {
        u8SnrGain = (HI_U8)((((HI_U32)(((HI_U64)17*(HI_U32)u32Again * 0x90abcc03UL) >> 40)) - 17) & 0xf) | 0xf0;
        u8Dgc     = 0x00;
    }
    else
    {
        u8SnrGain = (HI_U8)((((HI_U32)(((HI_U64)17*(HI_U32)u32Again * 0x242af301UL) >> 38)) - 17) & 0xf) | 0xf0;
        u8Dgc     = (u32Again > 3619) ? 0x03 : 0x01;
    }

#if CMOS_SC1035_ISP_WRITE_SENSOR_ENABLE
    cmos_init_regs_info();
    g_stSnsRegsInfo.astI2cData[2].u32Data = u8SnrGain;
    g_stSnsRegsInfo.astI2cData[3].u32Data = u8Dgc | 0x14;
    HI_MPI_ISP_SnsRegsCfg(&g_stSnsRegsInfo);
#else
    sensor_write_register(0x3e09, u8SnrGain);
    sensor_write_register(0x3e0f, u8Dgc | 0x14);
#endif

    return;
}

static HI_S32 cmos_get_awb_default(AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    if (HI_NULL == pstAwbSnsDft)
    {
        printf("null pointer when get awb default value!\n");
        return -1;
    }

    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));

    pstAwbSnsDft->u16WbRefTemp = 5700;

    pstAwbSnsDft->au16GainOffset[0] = 0x18b;
    pstAwbSnsDft->au16GainOffset[1] = 0x100;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;
    pstAwbSnsDft->au16GainOffset[3] = 0x155;

    pstAwbSnsDft->as32WbPara[0] = 97;
    pstAwbSnsDft->as32WbPara[1] = 34;
    pstAwbSnsDft->as32WbPara[2] = -125;
    pstAwbSnsDft->as32WbPara[3] = 199044;
    pstAwbSnsDft->as32WbPara[4] = 128;
    pstAwbSnsDft->as32WbPara[5] = -156331;

    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm, sizeof(AWB_CCM_S));
    memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S)); /* vendor: 8c..3c */

    return 0;
}

HI_VOID sensor_global_init()
{
    gu8SensorMode = 0;
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/
HI_S32 cmos_init_sensor_exp_function(ISP_SENSOR_EXP_FUNC_S *pstSensorExpFunc)
{
    memset(pstSensorExpFunc, 0, sizeof(ISP_SENSOR_EXP_FUNC_S));

    pstSensorExpFunc->pfn_cmos_sensor_init = sensor_init;
    pstSensorExpFunc->pfn_cmos_sensor_global_init = sensor_global_init;
    pstSensorExpFunc->pfn_cmos_get_isp_default = cmos_get_isp_default;
    pstSensorExpFunc->pfn_cmos_get_isp_black_level = cmos_get_isp_black_level;
    pstSensorExpFunc->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    pstSensorExpFunc->pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;
    pstSensorExpFunc->pfn_cmos_get_sensor_max_resolution = cmos_get_sensor_max_resolution;
    pstSensorExpFunc->pfn_cmos_set_resolution = HI_NULL;
    pstSensorExpFunc->pfn_cmos_set_image_mode = HI_NULL;

    return 0;
}

HI_S32 cmos_init_ae_exp_function(AE_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    memset(pstExpFuncs, 0, sizeof(AE_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_ae_default    = cmos_get_ae_default;
    pstExpFuncs->pfn_cmos_fps_set           = cmos_fps_set;
    pstExpFuncs->pfn_cmos_slow_framerate_set= cmos_slow_framerate_set;
    pstExpFuncs->pfn_cmos_inttime_update    = cmos_inttime_update;
    pstExpFuncs->pfn_cmos_gains_update      = cmos_gains_update;

    return 0;
}

HI_S32 cmos_init_awb_exp_function(AWB_SENSOR_EXP_FUNC_S *pstExpFuncs)
{
    memset(pstExpFuncs, 0, sizeof(AWB_SENSOR_EXP_FUNC_S));

    pstExpFuncs->pfn_cmos_get_awb_default = cmos_get_awb_default;

    return 0;
}

int sensor_register_callback(void)
{
    HI_S32 s32Ret;
    ALG_LIB_S stLib;
    ISP_SENSOR_REGISTER_S stIspRegister;
    AE_SENSOR_REGISTER_S  stAeRegister;
    AWB_SENSOR_REGISTER_S stAwbRegister;

    cmos_init_sensor_exp_function(&stIspRegister.stSnsExp);
    s32Ret = HI_MPI_ISP_SensorRegCallBack(SC1035_ID, &stIspRegister);
    if (s32Ret)
    {
        printf("sensor register callback function failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    cmos_init_ae_exp_function(&stAeRegister.stSnsExp);
    s32Ret = HI_MPI_AE_SensorRegCallBack(&stLib, SC1035_ID, &stAeRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    cmos_init_awb_exp_function(&stAwbRegister.stSnsExp);
    s32Ret = HI_MPI_AWB_SensorRegCallBack(&stLib, SC1035_ID, &stAwbRegister);
    if (s32Ret)
    {
        printf("sensor register callback function to ae lib failed!\n");
        return s32Ret;
    }

    return 0;
}

int sensor_unregister_callback(void)
{
    HI_S32 s32Ret;
    ALG_LIB_S stLib;

    s32Ret = HI_MPI_ISP_SensorUnRegCallBack(SC1035_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
    s32Ret = HI_MPI_AE_SensorUnRegCallBack(&stLib, SC1035_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to ae lib failed!\n");
        return s32Ret;
    }

    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
    s32Ret = HI_MPI_AWB_SensorUnRegCallBack(&stLib, SC1035_ID);
    if (s32Ret)
    {
        printf("sensor unregister callback function to awb lib failed!\n");
        return s32Ret;
    }

    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __SC1035_CMOS_H_ */
