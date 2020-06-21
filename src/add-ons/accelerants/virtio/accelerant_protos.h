/*
 * Copyright 2020 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Jacob Secunda, secundaja@gmail.com
 */
#ifndef ACCELERANT_PROTOS_H
#define ACCELERANT_PROTOS_H


#include <Accelerant.h>
#include "video_overlay.h"


#ifdef __cplusplus
extern "C" {
#endif

// general
status_t virtio_init_accelerant(int fd);
ssize_t virtio_accelerant_clone_info_size(void);
void virtio_get_accelerant_clone_info(void *data);
status_t virtio_clone_accelerant(void *data);
void virtio_uninit_accelerant(void);
status_t virtio_get_accelerant_device_info(accelerant_device_info *adi);
sem_id virtio_accelerant_retrace_semaphore(void);

// modes & constraints
uint32 virtio_accelerant_mode_count(void);
status_t virtio_get_mode_list(display_mode *dm);
status_t virtio_propose_display_mode(display_mode *target,
	const display_mode *low, const display_mode *high);
status_t virtio_set_display_mode(display_mode *modeToSet);
status_t virtio_get_display_mode(display_mode *currentMode);
status_t virtio_get_edid_info(void *info, size_t size, uint32 *_version);
status_t virtio_get_frame_buffer_config(frame_buffer_config *config);
status_t virtio_get_pixel_clock_limits(display_mode *dm, uint32 *low,
	uint32 *high);
status_t virtio_move_display(uint16 hDisplayStart, uint16 vDisplayStart);
status_t virtio_get_timing_constraints(display_timing_constraints *dtc);
void virtio_set_indexed_colors(uint count, uint8 first, uint8 *colorData,
	uint32 flags);

// DPMS
uint32 virtio_dpms_capabilities(void);
uint32 virtio_dpms_mode(void);
status_t virtio_set_dpms_mode(uint32 dpmsFlags);

// cursor
status_t virtio_set_cursor_shape(uint16 width, uint16 height, uint16 hotX,
	uint16 hotY, const uint8 *andMask, const uint8 *xorMask);
status_t virtio_set_cursor_bitmap(uint16 width, uint16 height, uint16 hotX,
	uint16 hotY, color_space colorSpace, uint16 bytesPerRow,
	const uint8* bitmapData);
void virtio_move_cursor(uint16 x, uint16 y);
void virtio_show_cursor(bool is_visible);

// accelerant engine
uint32 virtio_accelerant_engine_count(void);
status_t virtio_acquire_engine(uint32 capabilities, uint32 maxWait,
	sync_token *st, engine_token **et);
status_t virtio_release_engine(engine_token *et, sync_token *st);
void virtio_wait_engine_idle(void);
status_t virtio_get_sync_token(engine_token *et, sync_token *st);
status_t virtio_sync_to_token(sync_token *st);

// 2D acceleration
void virtio_screen_to_screen_blit(engine_token *et, blit_params *list,
	uint32 count);
void virtio_fill_rectangle(engine_token *et, uint32 color, fill_rect_params *list,
	uint32 count);
void virtio_invert_rectangle(engine_token *et, fill_rect_params *list,
	uint32 count);
void virtio_fill_span(engine_token *et, uint32 color, uint16 *list, uint32 count);

// overlay
uint32 virtio_overlay_count(const display_mode *dm);
const uint32 *virtio_overlay_supported_spaces(const display_mode *dm);
uint32 virtio_overlay_supported_features(uint32 a_color_space);
const overlay_buffer *virtio_allocate_overlay_buffer(color_space cs, uint16 width,
	uint16 height);
status_t virtio_release_overlay_buffer(const overlay_buffer *ob);
status_t virtio_get_overlay_constraints(const display_mode *dm,
	const overlay_buffer *ob, overlay_constraints *oc);
overlay_token virtio_allocate_overlay(void);
status_t virtio_release_overlay(overlay_token ot);
status_t virtio_configure_overlay(overlay_token ot, const overlay_buffer *ob,
	const overlay_window *ow, const overlay_view *ov);

#ifdef __cplusplus
}
#endif

#endif	/* ACCELERANT_PROTOS_H */
