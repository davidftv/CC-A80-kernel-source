/*
 * sound\soc\sunxi\daudio\sunxi_snddaudio.c
 * (C) Copyright 2010-2016
 * Reuuimlla Technology Co., Ltd. <www.reuuimllatech.com>
 * huangxin <huangxin@Reuuimllatech.com>
 *
 * some simple description for this code
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */

#include <linux/module.h>
#include <linux/clk.h>
#include <linux/mutex.h>

#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <sound/soc-dapm.h>
#include <linux/io.h>
#include <mach/sys_config.h>

#include "sunxi-daudio0.h"
#include "sunxi-daudiodma0.h"

static bool daudio_pcm_select 	= 0;

static int daudio_used 			= 0;
static int daudio_master 		= 0;
static int audio_format 		= 0;
static int signal_inversion 	= 0;

/*
*	daudio_pcm_select == 0:-->	pcm
*	daudio_pcm_select == 1:-->	i2s
*/
static int sunxi_daudio_set_audio_mode(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	daudio_pcm_select = ucontrol->value.integer.value[0];

	if (daudio_pcm_select) {
		audio_format 			= 1;
		signal_inversion 		= 1;
		daudio_master 			= 4;
	} else {
		audio_format 			= 4;
		signal_inversion 		= 3;
		daudio_master 			= 1;
	}

	return 0;
}

static int sunxi_daudio_get_audio_mode(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = daudio_pcm_select;
	return 0;
}

/* I2s Or Pcm Audio Mode Select */
static const struct snd_kcontrol_new sunxi_daudio_controls[] = {
	SOC_SINGLE_BOOL_EXT("I2s Or Pcm Audio Mode Select format", 0,
			sunxi_daudio_get_audio_mode, sunxi_daudio_set_audio_mode),
};

static int sunxi_snddaudio_hw_params(struct snd_pcm_substream *substream,
					struct snd_pcm_hw_params *params)
{
	int ret  = 0;
	u32 freq = 22579200;

	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	unsigned long sample_rate = params_rate(params);

	switch (sample_rate) {
		case 8000:
		case 16000:
		case 32000:
		case 64000:
		case 128000:
		case 12000:
		case 24000:
		case 48000:
		case 96000:
		case 192000:
			freq = 24576000;
			break;
	}

	/*set system clock source freq and set the mode as daudio or pcm*/
	ret = snd_soc_dai_set_sysclk(cpu_dai, 0 , freq, daudio_pcm_select);
	if (ret < 0) {
		return ret;
	}

	/*set system clock source freq and set the mode as daudio or pcm*/
	ret = snd_soc_dai_set_sysclk(codec_dai, 0 , freq, daudio_pcm_select);
	if (ret < 0) {
		return ret;
	}
#ifdef CONFIG_SND_SOC_AC100_CODEC
	/*
	* ac100: master. AP: slave
	*/
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S |
			SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBM_CFM);
	if (ret < 0) {
		printk("%s, line:%d\n", __func__, __LINE__);
		return ret;
	}

	ret = snd_soc_dai_set_fmt(cpu_dai, (audio_format | (signal_inversion<<8) | SND_SOC_DAIFMT_CBM_CFM));
	if (ret < 0) {
		return ret;
	}
#else
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S |
			SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0) {
		printk("%s, line:%d\n", __func__, __LINE__);
		return ret;
	}
	/*
	* codec clk & FRM master. AP as slave
	*/
	ret = snd_soc_dai_set_fmt(cpu_dai, (audio_format | (signal_inversion<<8) | (daudio_master<<12)));
	if (ret < 0) {
		return ret;
	}
#endif
	ret = snd_soc_dai_set_clkdiv(cpu_dai, 0, sample_rate);
	if (ret < 0) {
		return ret;
	}
	ret = snd_soc_dai_set_clkdiv(codec_dai, 0, sample_rate);
	if (ret < 0) {
		printk("%s, line:%d\n", __func__, __LINE__);
		return ret;
	}
	/*
	*	audio_format == SND_SOC_DAIFMT_DSP_A
	*	signal_inversion<<8 == SND_SOC_DAIFMT_IB_NF
	*	daudio_master<<12 == SND_SOC_DAIFMT_CBM_CFM
	*/
	DAUDIO_DBG("%s,line:%d,audio_format:%d,SND_SOC_DAIFMT_DSP_A:%d\n",\
			__func__, __LINE__, audio_format, SND_SOC_DAIFMT_DSP_A);
	DAUDIO_DBG("%s,line:%d,signal_inversion:%d,signal_inversion<<8:%d,SND_SOC_DAIFMT_IB_NF:%d\n",\
			__func__, __LINE__, signal_inversion, signal_inversion<<8, SND_SOC_DAIFMT_IB_NF);
	DAUDIO_DBG("%s,line:%d,daudio_master:%d,daudio_master<<12:%d,SND_SOC_DAIFMT_CBM_CFM:%d\n",\
			__func__, __LINE__, daudio_master, daudio_master<<12, SND_SOC_DAIFMT_CBM_CFM);

	return 0;
}

/*
 * Card initialization
 */
static int sunxi_daudio_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_card *card = rtd->card;
	int ret;

	/* Add virtual switch */
	ret = snd_soc_add_codec_controls(codec, sunxi_daudio_controls,
					ARRAY_SIZE(sunxi_daudio_controls));
	if (ret) {
		dev_warn(card->dev,
				"Failed to register audio mode control, "
				"will continue without it.\n");
	}
	return 0;
}

static struct snd_soc_ops sunxi_snddaudio_ops = {
	.hw_params 		= sunxi_snddaudio_hw_params,
};

static struct snd_soc_dai_link sunxi_snddaudio_dai_link = {
	.name 			= "s_i2s1",
#ifdef CONFIG_ARCH_SUN9I
	.cpu_dai_name 	= "s_i2s1",
	.stream_name 	= "SUNXI-I2S0",
#else
	.cpu_dai_name 	= "tdm0",
	.stream_name 	= "SUNXI-TDM0",
#endif
#ifdef CONFIG_SND_SOC_AC100_CODEC
	.codec_dai_name = "sndvir_audio",
	.codec_name 	= "vir_audio-codec.0-001a",
#else
	.codec_dai_name = "snddaudio",
	.codec_name 	= "sunxi-daudio-codec.0",
#endif
	.init 			= sunxi_daudio_init,
	.platform_name 	= "sunxi-daudio-pcm-audio.0",
	.ops 			= &sunxi_snddaudio_ops,
};

static struct snd_soc_card snd_soc_sunxi_snddaudio = {
	.name 		= "snddaudio",
	.owner 		= THIS_MODULE,
	.dai_link 	= &sunxi_snddaudio_dai_link,
	.num_links 	= 1,
};

static struct platform_device *sunxi_snddaudio_device;

static int __init sunxi_snddaudio_init(void)
{
	int ret = 0;
	script_item_u val;
	script_item_value_type_e  type;
printk("%s, line:%d\n", __func__, __LINE__);
	type = script_get_item(TDM_NAME, "daudio_used", &val);
	if (SCIRPT_ITEM_VALUE_TYPE_INT != type) {
        printk("[I2S0] type err!\n");
    }
	daudio_used = val.val;
	printk("%s, line:%d, daudio_used:%d\n", __func__, __LINE__, daudio_used);
	type = script_get_item(TDM_NAME, "daudio_select", &val);
	if (SCIRPT_ITEM_VALUE_TYPE_INT != type) {
        printk("[I2S0] daudio_select type err!\n");
    }
	daudio_pcm_select = val.val;

	type = script_get_item(TDM_NAME, "daudio_master", &val);
	if (SCIRPT_ITEM_VALUE_TYPE_INT != type) {
        printk("[I2S0] daudio_master type err!\n");
    }
	daudio_master = val.val;
	
	type = script_get_item(TDM_NAME, "audio_format", &val);
	if (SCIRPT_ITEM_VALUE_TYPE_INT != type) {
        printk("[I2S0] audio_format type err!\n");
    }
	audio_format = val.val;

	type = script_get_item(TDM_NAME, "signal_inversion", &val);
	if (SCIRPT_ITEM_VALUE_TYPE_INT != type) {
        printk("[I2S0] signal_inversion type err!\n");
    }
	signal_inversion = val.val;

    if (daudio_used) {
		sunxi_snddaudio_device = platform_device_alloc("soc-audio", 4);
		if(!sunxi_snddaudio_device)
			return -ENOMEM;
		platform_set_drvdata(sunxi_snddaudio_device, &snd_soc_sunxi_snddaudio);
		ret = platform_device_add(sunxi_snddaudio_device);
		if (ret) {
			platform_device_put(sunxi_snddaudio_device);
		}
	} else {
		printk("[I2S0]sunxi_snddaudio cannot find any using configuration for controllers, return directly!\n");
        return 0;
	}
	return ret;
}

static void __exit sunxi_snddaudio_exit(void)
{
	if (daudio_used) {
		daudio_used = 0;
		platform_device_unregister(sunxi_snddaudio_device);
	}
}

module_init(sunxi_snddaudio_init);
module_exit(sunxi_snddaudio_exit);

MODULE_AUTHOR("huangxin");
MODULE_DESCRIPTION("SUNXI_snddaudio ALSA SoC audio driver");
MODULE_LICENSE("GPL");
