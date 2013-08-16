/*
 * Nestopia UE
 * 
 * Copyright (C) 2013 R. Danbrook
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <stdio.h>
#include <stdlib.h>

#include "config.h"

settings *conf;
GKeyFile *keyfile;
GKeyFileFlags flags;
	
void read_config_file() {
	
	keyfile = g_key_file_new();
	
	flags = G_KEY_FILE_KEEP_COMMENTS;
	
	if (!g_key_file_load_from_file(keyfile, "nestopia.conf", flags, NULL)) {
		printf("Could not read config file.\n");
		//set defaults;
	}
	
	// Set aside memory for the settings
	conf = g_slice_new(settings);
	
	// Video
	conf->video_renderer = g_key_file_get_integer(keyfile, "video", "renderer", NULL);
	conf->video_filter = g_key_file_get_integer(keyfile, "video", "filter", NULL);
	conf->video_scale_factor = g_key_file_get_integer(keyfile, "video", "scale_factor", NULL);
	conf->video_ntsc_mode = g_key_file_get_integer(keyfile, "video", "ntsc_mode", NULL);
	conf->video_xbr_corner_rounding = g_key_file_get_integer(keyfile, "video", "xbr_corner_rounding", NULL);
	
	conf->video_xbr_pixel_blending = g_key_file_get_boolean(keyfile, "video", "xbr_pixel_blending", NULL);
	conf->video_tv_aspect = g_key_file_get_boolean(keyfile, "video", "tv_aspect", NULL);
	conf->video_mask_overscan = g_key_file_get_boolean(keyfile, "video", "mask_overscan", NULL);
	conf->video_fullscreen = g_key_file_get_boolean(keyfile, "video", "fullscreen", NULL);
	conf->video_stretch_fullscreen = g_key_file_get_boolean(keyfile, "video", "stretch_fullscreen", NULL);
	conf->video_unlimited_sprites = g_key_file_get_boolean(keyfile, "video", "unlimited_sprites", NULL);
	
	// Audio
	conf->audio_api = g_key_file_get_integer(keyfile, "audio", "api", NULL);
	conf->audio_sample_rate = g_key_file_get_integer(keyfile, "audio", "sample_rate", NULL);
	conf->audio_volume = g_key_file_get_integer(keyfile, "audio", "volume", NULL);
	conf->audio_surround_multiplier = g_key_file_get_integer(keyfile, "audio", "surround_multiplier", NULL);
	
	conf->audio_surround = g_key_file_get_boolean(keyfile, "audio", "surround", NULL);
	conf->audio_stereo = g_key_file_get_boolean(keyfile, "audio", "stereo", NULL);
	conf->audio_stereo_exciter = g_key_file_get_boolean(keyfile, "audio", "stereo_exciter", NULL);
	
	// Misc
	conf->misc_video_region = g_key_file_get_integer(keyfile, "misc", "video_region", NULL);
	conf->misc_default_system = g_key_file_get_integer(keyfile, "misc", "default_system", NULL);
	conf->misc_soft_patching = g_key_file_get_boolean(keyfile, "misc", "soft_patching", NULL);
	conf->misc_disable_gui = g_key_file_get_boolean(keyfile, "misc", "disable_gui", NULL);
	
	g_key_file_free(keyfile);
}

void write_config_file() {
	printf("Tried to write config file\n");
}