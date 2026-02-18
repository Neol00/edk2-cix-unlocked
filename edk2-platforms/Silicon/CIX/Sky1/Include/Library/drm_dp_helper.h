/*
 * Copyright © 2008 Keith Packard
  Copyright 2024 Cix Technology Group Co., Ltd. All Rights Reserved
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#ifndef _DRM_DP_HELPER_H_
#define _DRM_DP_HELPER_H_

BOOLEAN
drm_dp_channel_eq_ok (
  const UINT8  link_status[DP_LINK_STATUS_SIZE],
  INT32        lane_count
  );

BOOLEAN
drm_dp_clock_recovery_ok (
  const UINT8  link_status[DP_LINK_STATUS_SIZE],
  INT32        lane_count
  );

UINT8
drm_dp_get_adjust_request_voltage (
  const UINT8  link_status[DP_LINK_STATUS_SIZE],
  INT32        lane
  );

UINT8
drm_dp_get_adjust_request_pre_emphasis (
  const UINT8  link_status[DP_LINK_STATUS_SIZE],
  INT32        lane
  );

UINT8
drm_dp_get_adjust_tx_ffe_preset (
  const UINT8  link_status[DP_LINK_STATUS_SIZE],
  INT32        lane
  );

INT32
drm_dp_read_clock_recovery_delay (
  // struct drm_dp_aux   *aux,
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE],
  // enum drm_dp_phy     dp_phy,
  INT32        uhbr
  );

INT32
drm_dp_read_channel_eq_delay (
  // struct drm_dp_aux  *aux,
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE],
  // enum drm_dp_phy    dp_phy,
  BOOLEAN      uhbr
  );

void
drm_dp_link_train_clock_recovery_delay (
  // const struct drm_dp_aux  *aux,
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  );

void
drm_dp_lttpr_link_train_clock_recovery_delay (
  void
  );

void
drm_dp_link_train_channel_eq_delay (
  // const struct drm_dp_aux  *aux,
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  );

void
drm_dp_lttpr_link_train_channel_eq_delay (
  // const struct drm_dp_aux  *aux,
  const UINT8  caps[DP_LTTPR_PHY_CAP_SIZE]
  );

INTN
drm_dp_128b132b_read_aux_rd_INTNerval (
  // struct drm_dp_aux  *aux
  );

BOOLEAN
drm_dp_128b132b_lane_channel_eq_done (
  const UINT8  link_status[DP_LINK_STATUS_SIZE],
  INT32        lane_count
  );

BOOLEAN
drm_dp_128b132b_lane_symbol_locked (
  const UINT8  link_status[DP_LINK_STATUS_SIZE],
  INT32        lane_count
  );

BOOLEAN
drm_dp_128b132b_eq_INTNerlane_align_done (
  const UINT8  link_status[DP_LINK_STATUS_SIZE]
  );

BOOLEAN
drm_dp_128b132b_cds_INTNerlane_align_done (
  const UINT8  link_status[DP_LINK_STATUS_SIZE]
  );

BOOLEAN
drm_dp_128b132b_link_training_failed (
  const UINT8  link_status[DP_LINK_STATUS_SIZE]
  );

UINT8
drm_dp_link_rate_to_bw_code (
  INT32  link_rate
  );

STATIC INT32
drm_dp_bw_code_to_link_rate (
  UINT8  link_bw
  )
{
  switch (link_bw) {
    case DP_LINK_BW_10:
      return 1000000;
    case DP_LINK_BW_13_5:
      return 1350000;
    case DP_LINK_BW_20:
      return 2000000;
    default:
      /* Spec says link_rate = link_bw * 0.27Gbps */
      return link_bw * 27000;
  }
}

// const char *
// drm_dp_phy_name (
//  enum drm_dp_phy  dp_phy
//  );

/**
 * struct drm_dp_vsc_sdp - drm DP VSC SDP
 *
 * This structure represents a DP VSC SDP of drm
 * It is based on DP 1.4 spec [Table 2-116: VSC SDP Header Bytes] and
 * [Table 2-117: VSC SDP Payload for DB16 through DB18]
 *
 * @sdp_type: secondary-data packet type
 * @revision: revision number
 * @length: number of valid data bytes
 * @pixelformat: pixel encoding format
 * @colorimetry: colorimetry format
 * @bpc: bit per color
 * @dynamic_range: dynamic range information
 * @content_type: CTA-861-G defines content types and expected processing by a sink device
 */
struct drm_dp_vsc_sdp {
  UINT8                    sdp_type;
  UINT8                    revision;
  UINT8                    length;
  enum dp_pixelformat      pixelformat;
  enum dp_colorimetry      colorimetry;
  INTN                     bpc;
  enum dp_dynamic_range    dynamic_range;
  enum dp_content_type     content_type;
};

INTN
drm_dp_psr_setup_time (
  const UINT8  psr_cap[EDP_PSR_RECEIVER_CAP_SIZE]
  );

STATIC inline INT32
drm_dp_max_link_rate (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  )
{
  return drm_dp_bw_code_to_link_rate (dpcd[DP_MAX_LINK_RATE]);
}

STATIC inline UINT8
drm_dp_max_lane_count (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  )
{
  return dpcd[DP_MAX_LANE_COUNT] & DP_MAX_LANE_COUNT_MASK;
}

STATIC inline BOOLEAN
drm_dp_enhanced_frame_cap (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  )
{
  return dpcd[DP_DPCD_REV] >= 0x11 &&
         (dpcd[DP_MAX_LANE_COUNT] & DP_ENHANCED_FRAME_CAP);
}

STATIC inline BOOLEAN
drm_dp_fast_training_cap (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  )
{
  return dpcd[DP_DPCD_REV] >= 0x11 &&
         (dpcd[DP_MAX_DOWNSPREAD] & DP_NO_AUX_HANDSHAKE_LINK_TRAINING);
}

STATIC inline BOOLEAN
drm_dp_tps3_supported (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  )
{
  return dpcd[DP_DPCD_REV] >= 0x12 &&
         dpcd[DP_MAX_LANE_COUNT] & DP_TPS3_SUPPORTED;
}

STATIC inline BOOLEAN
drm_dp_max_downspread (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  )
{
  return dpcd[DP_DPCD_REV] >= 0x11 ||
         dpcd[DP_MAX_DOWNSPREAD] & DP_MAX_DOWNSPREAD_0_5;
}

STATIC inline BOOLEAN
drm_dp_tps4_supported (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  )
{
  return dpcd[DP_DPCD_REV] >= 0x14 &&
         dpcd[DP_MAX_DOWNSPREAD] & DP_TPS4_SUPPORTED;
}

STATIC inline UINT8
drm_dp_training_pattern_mask (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  )
{
  return (dpcd[DP_DPCD_REV] >= 0x14) ? DP_TRAINING_PATTERN_MASK_1_4 :
         DP_TRAINING_PATTERN_MASK;
}

STATIC inline BOOLEAN
drm_dp_is_branch (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  )
{
  return dpcd[DP_DOWNSTREAMPORT_PRESENT] & DP_DWN_STRM_PORT_PRESENT;
}

/* DP/eDP DSC support */
UINT8
drm_dp_dsc_sink_max_slice_count (
  const UINT8  dsc_dpcd[DP_DSC_RECEIVER_CAP_SIZE],
  BOOLEAN      is_edp
  );

UINT8
drm_dp_dsc_sink_line_buf_depth (
  const UINT8  dsc_dpcd[DP_DSC_RECEIVER_CAP_SIZE]
  );

INTN
drm_dp_dsc_sink_supported_input_bpcs (
  const UINT8  dsc_dpc[DP_DSC_RECEIVER_CAP_SIZE],
  UINT8        dsc_bpc[3]
  );

STATIC inline BOOLEAN
drm_dp_sink_supports_dsc (
  const UINT8  dsc_dpcd[DP_DSC_RECEIVER_CAP_SIZE]
  )
{
  return dsc_dpcd[DP_DSC_SUPPORT - DP_DSC_SUPPORT] &
         DP_DSC_DECOMPRESSION_IS_SUPPORTED;
}

STATIC inline UINT16
drm_edp_dsc_sink_output_bpp (
  const UINT8  dsc_dpcd[DP_DSC_RECEIVER_CAP_SIZE]
  )
{
  return dsc_dpcd[DP_DSC_MAX_BITS_PER_PIXEL_LOW - DP_DSC_SUPPORT] |
         ((dsc_dpcd[DP_DSC_MAX_BITS_PER_PIXEL_HI - DP_DSC_SUPPORT] &
           DP_DSC_MAX_BITS_PER_PIXEL_HI_MASK) << 8);
}

STATIC inline UINT32
drm_dp_dsc_sink_max_slice_width (
  const UINT8  dsc_dpcd[DP_DSC_RECEIVER_CAP_SIZE]
  )
{
  /* Max Slicewidth = Number of Pixels * 320 */
  return dsc_dpcd[DP_DSC_MAX_SLICE_WIDTH - DP_DSC_SUPPORT] *
         DP_DSC_SLICE_WIDTH_MULTIPLIER;
}

/* Forward Error Correction Support on DP 1.4 */
STATIC inline BOOLEAN
drm_dp_sink_supports_fec (
  const UINT8  fec_capable
  )
{
  return fec_capable & DP_FEC_CAPABLE;
}

STATIC inline BOOLEAN
drm_dp_channel_coding_supported (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  )
{
  return dpcd[DP_MAIN_LINK_CHANNEL_CODING] & DP_CAP_ANSI_8B10B;
}

STATIC inline BOOLEAN
drm_dp_alternate_scrambler_reset_cap (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  )
{
  return dpcd[DP_EDP_CONFIGURATION_CAP] &
         DP_ALTERNATE_SCRAMBLER_RESET_CAP;
}

/* Ignore MSA timing for Adaptive Sync support on DP 1.4 */
STATIC inline BOOLEAN
drm_dp_sink_can_do_video_without_timing_msa (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE]
  )
{
  return dpcd[DP_DOWN_STREAM_PORT_COUNT] &
         DP_MSA_TIMING_PAR_IGNORED;
}

/**
 * drm_edp_backlight_supported() - Check an eDP DPCD for VESA backlight support
 * @edp_dpcd: The DPCD to check
 *
 * Note that currently this function will return %false for panels which support various DPCD
 * backlight features but which require the brightness be set through PWM, and don't support setting
 * the brightness level via the DPCD.
 *
 * Returns: %True if @edp_dpcd indicates that VESA backlight controls are supported, %false
 * otherwise
 */
STATIC inline BOOLEAN
drm_edp_backlight_supported (
  const UINT8  edp_dpcd[EDP_DISPLAY_CTL_CAP_SIZE]
  )
{
  return !!(edp_dpcd[1] & DP_EDP_TCON_BACKLIGHT_ADJUSTMENT_CAP);
}

/*
 * DisplayPort AUX channel
 */

/**
 * struct drm_dp_aux_msg - DisplayPort AUX channel transaction
 * @address: address of the (first) register to access
 * @request: contains the type of transaction (see DP_AUX_* macros)
 * @reply: upon completion, contains the reply type of the transaction
 * @buffer: poINTNer to a transmission or reception buffer
 * @size: size of @buffer
 */
struct drm_dp_aux_msg {
  INTN      address;
  UINT8     request;
  UINT8     reply;
  void      *buffer;
  UINT32    size;
};

/**
 * struct drm_dp_aux - DisplayPort AUX channel
 *
 * An AUX channel can also be used to transport I2C messages to a sink. A
 * typical application of that is to access an EDID that's present in the sink
 * device. The @transfer() function can also be used to execute such
 * transactions. The drm_dp_aux_register() function registers an I2C adapter
 * that can be passed to drm_probe_ddc(). Upon removal, drivers should call
 * drm_dp_aux_unregister() to remove the I2C adapter. The I2C adapter uses long
 * transfers by default; if a partial response is received, the adapter will
 * drop down to the size given by the partial response for this transaction
 * only.
 */
struct drm_dp_aux {
  const char    *name;

  /**
   * @is_remote: Is this AUX CH actually using sideband messaging.
   */
  BOOLEAN       is_remote;
};

UINT32
drm_dp_dpcd_probe (
  struct drm_dp_aux  *aux,
  UINT32             offset
  );

UINT32
drm_dp_dpcd_read (
  struct drm_dp_aux  *aux,
  UINT32             offset,
  void               *buffer,
  UINT32             size
  );

UINT32
drm_dp_dpcd_write (
  struct drm_dp_aux  *aux,
  UINT32             offset,
  void               *buffer,
  INTN               size
  );

/**
 * drm_dp_dpcd_readb() - read a single byte from the DPCD
 * @aux: DisplayPort AUX channel
 * @offset: address of the register to read
 * @valuep: location where the value of the register will be stored
 *
 * Returns the number of bytes transferred (1) on success, or a negative
 * error code on failure.
 */
STATIC inline UINT32
drm_dp_dpcd_readb (
  struct drm_dp_aux  *aux,
  UINT32             offset,
  UINT8              *valuep
  )
{
  return drm_dp_dpcd_read (aux, offset, valuep, 1);
}

/**
 * drm_dp_dpcd_writeb() - write a single byte to the DPCD
 * @aux: DisplayPort AUX channel
 * @offset: address of the register to write
 * @value: value to write to the register
 *
 * Returns the number of bytes transferred (1) on success, or a negative
 * error code on failure.
 */
STATIC inline UINT32
drm_dp_dpcd_writeb (
  struct drm_dp_aux  *aux,
  UINT32             offset,
  UINT8              value
  )
{
  return drm_dp_dpcd_write (aux, offset, &value, 1);
}

INTN
drm_dp_read_dpcd_caps (
  struct drm_dp_aux  *aux,
  UINT8              dpcd[DP_RECEIVER_CAP_SIZE]
  );

INTN
drm_dp_dpcd_read_link_status (
  struct drm_dp_aux  *aux,
  UINT8              status[DP_LINK_STATUS_SIZE]
  );

INTN
drm_dp_dpcd_read_phy_link_status (
  struct drm_dp_aux  *aux,
  enum drm_dp_phy    dp_phy,
  UINT8              link_status[DP_LINK_STATUS_SIZE]
  );

BOOLEAN
drm_dp_send_real_edid_checksum (
  struct drm_dp_aux  *aux,
  UINT8              real_edid_checksum
  );

INTN
drm_dp_read_downstream_info (
  struct drm_dp_aux  *aux,
  const UINT8        dpcd[DP_RECEIVER_CAP_SIZE],
  UINT8              downstream_ports[DP_MAX_DOWNSTREAM_PORTS]
  );

BOOLEAN
drm_dp_downstream_is_type (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE],
  const UINT8  port_cap[4],
  UINT8        type
  );

BOOLEAN
drm_dp_downstream_is_tmds (
  const UINT8        dpcd[DP_RECEIVER_CAP_SIZE],
  const UINT8        port_cap[4],
  const struct Edid  *edid
  );

INTN
drm_dp_downstream_max_dotclock (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE],
  const UINT8  port_cap[4]
  );

INTN
drm_dp_downstream_max_tmds_clock (
  const UINT8        dpcd[DP_RECEIVER_CAP_SIZE],
  const UINT8        port_cap[4],
  const struct Edid  *edid
  );

INTN
drm_dp_downstream_min_tmds_clock (
  const UINT8        dpcd[DP_RECEIVER_CAP_SIZE],
  const UINT8        port_cap[4],
  const struct Edid  *edid
  );

INTN
drm_dp_downstream_max_bpc (
  const UINT8        dpcd[DP_RECEIVER_CAP_SIZE],
  const UINT8        port_cap[4],
  const struct Edid  *edid
  );

BOOLEAN
drm_dp_downstream_420_passthrough (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE],
  const UINT8  port_cap[4]
  );

BOOLEAN
drm_dp_downstream_444_to_420_conversion (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE],
  const UINT8  port_cap[4]
  );

struct drm_display_mode *
drm_dp_downstream_mode (
  // struct drm_device  *dev,
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE],
  const UINT8  port_cap[4]
  );

INTN
drm_dp_downstream_id (
  struct drm_dp_aux  *aux,
  char               id[6]
  );

enum drm_mode_subconnector
drm_dp_subconnector_type (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE],
  const UINT8  port_cap[4]
  );

void
drm_dp_set_subconnector_property (
  // struct drm_connector       *connector,
  // enum drm_connector_status  status,
  const UINT8  *dpcd,
  const UINT8  port_cap[4]
  );

struct drm_dp_desc;
BOOLEAN
drm_dp_read_sink_count_cap (
  // struct drm_connector      *connector,
  const UINT8               dpcd[DP_RECEIVER_CAP_SIZE],
  const struct drm_dp_desc  *desc
  );

INTN
drm_dp_read_sink_count (
  struct drm_dp_aux  *aux
  );

INTN
drm_dp_read_lttpr_common_caps (
  struct drm_dp_aux  *aux,
  const UINT8        dpcd[DP_RECEIVER_CAP_SIZE],
  UINT8              caps[DP_LTTPR_COMMON_CAP_SIZE]
  );

INTN
drm_dp_read_lttpr_phy_caps (
  struct drm_dp_aux  *aux,
  const UINT8        dpcd[DP_RECEIVER_CAP_SIZE],
  enum drm_dp_phy    dp_phy,
  UINT8              caps[DP_LTTPR_PHY_CAP_SIZE]
  );

INTN
drm_dp_lttpr_count (
  const UINT8  cap[DP_LTTPR_COMMON_CAP_SIZE]
  );

INTN
drm_dp_lttpr_max_link_rate (
  const UINT8  caps[DP_LTTPR_COMMON_CAP_SIZE]
  );

INTN
drm_dp_lttpr_max_lane_count (
  const UINT8  caps[DP_LTTPR_COMMON_CAP_SIZE]
  );

BOOLEAN
drm_dp_lttpr_voltage_swing_level_3_supported (
  const UINT8  caps[DP_LTTPR_PHY_CAP_SIZE]
  );

BOOLEAN
drm_dp_lttpr_pre_emphasis_level_3_supported (
  const UINT8  caps[DP_LTTPR_PHY_CAP_SIZE]
  );

void
drm_dp_remote_aux_init (
  struct drm_dp_aux  *aux
  );

void
drm_dp_aux_init (
  struct drm_dp_aux  *aux
  );

INTN
drm_dp_aux_register (
  struct drm_dp_aux  *aux
  );

void
drm_dp_aux_unregister (
  struct drm_dp_aux  *aux
  );

struct drm_dp_dpcd_ident {
  UINT8    oui[3];
  UINT8    device_id[6];
  UINT8    hw_rev;
  UINT8    sw_major_rev;
  UINT8    sw_minor_rev;
} __attribute__ ((packed));

/**
 * struct drm_dp_desc - DP branch/sink device descriptor
 * @ident: DP device identification from DPCD 0x400 (sink) or 0x500 (branch).
 * @quirks: Quirks; use drm_dp_has_quirk() to query for the quirks.
 */
struct drm_dp_desc {
  struct drm_dp_dpcd_ident    ident;
  UINT32                      quirks;
};

INTN
drm_dp_read_desc (
  struct drm_dp_aux   *aux,
  struct drm_dp_desc  *desc,
  BOOLEAN             is_branch
  );

/**
 * struct drm_edp_backlight_info - Probed eDP backlight info struct
 * @pwmgen_bit_count: The pwmgen bit count
 * @pwm_freq_pre_divider: The PWM frequency pre-divider value being used for this backlight, if any
 * @max: The maximum backlight level that may be set
 * @lsb_reg_used: Do we also write values to the DP_EDP_BACKLIGHT_BRIGHTNESS_LSB register?
 * @aux_enable: Does the panel support the AUX enable cap?
 * @aux_set: Does the panel support setting the brightness through AUX?
 *
 * This structure contains various data about an eDP backlight, which can be populated by using
 * drm_edp_backlight_init().
 */
struct drm_edp_backlight_info {
  UINT8      pwmgen_bit_count;
  UINT8      pwm_freq_pre_divider;
  UINT16     max;

  BOOLEAN    lsb_reg_used : 1;
  BOOLEAN    aux_enable   : 1;
  BOOLEAN    aux_set      : 1;
};

INTN
drm_edp_backlight_init (
  struct drm_dp_aux              *aux,
  struct drm_edp_backlight_info  *bl,
  UINT16                         driver_pwm_freq_hz,
  const UINT8                    edp_dpcd[EDP_DISPLAY_CTL_CAP_SIZE],
  UINT16                         *current_level,
  UINT8                          *current_mode
  );

INTN
drm_edp_backlight_set_level (
  struct drm_dp_aux                    *aux,
  const struct drm_edp_backlight_info  *bl,
  UINT16                               level
  );

INTN
drm_edp_backlight_enable (
  struct drm_dp_aux                    *aux,
  const struct drm_edp_backlight_info  *bl,
  UINT16                               level
  );

INTN
drm_edp_backlight_disable (
  struct drm_dp_aux                    *aux,
  const struct drm_edp_backlight_info  *bl
  );

/**
 * struct drm_dp_phy_test_params - DP Phy Compliance parameters
 * @link_rate: Requested Link rate from DPCD 0x219
 * @num_lanes: Number of lanes requested by sing through DPCD 0x220
 * @phy_pattern: DP Phy test pattern from DPCD 0x248
 * @hbr2_reset: DP HBR2_COMPLIANCE_SCRAMBLER_RESET from DCPD 0x24A and 0x24B
 * @custom80: DP Test_80BIT_CUSTOM_PATTERN from DPCDs 0x250 through 0x259
 * @enhanced_frame_cap: flag for enhanced frame capability.
 */
struct drm_dp_phy_test_params {
  INTN       link_rate;
  UINT8      num_lanes;
  UINT8      phy_pattern;
  UINT8      hbr2_reset[2];
  UINT8      custom80[10];
  BOOLEAN    enhanced_frame_cap;
};

INTN
drm_dp_get_phy_test_pattern (
  struct drm_dp_aux              *aux,
  struct drm_dp_phy_test_params  *data
  );

INTN
drm_dp_set_phy_test_pattern (
  struct drm_dp_aux              *aux,
  struct drm_dp_phy_test_params  *data,
  UINT8                          dp_rev
  );

INTN
drm_dp_get_pcon_max_frl_bw (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE],
  const UINT8  port_cap[4]
  );

INTN
drm_dp_pcon_frl_prepare (
  struct drm_dp_aux  *aux,
  BOOLEAN            enable_frl_ready_hpd
  );

BOOLEAN
drm_dp_pcon_is_frl_ready (
  struct drm_dp_aux  *aux
  );

INTN
drm_dp_pcon_frl_configure_1 (
  struct drm_dp_aux  *aux,
  INTN               max_frl_gbps,
  UINT8              frl_mode
  );

INTN
drm_dp_pcon_frl_configure_2 (
  struct drm_dp_aux  *aux,
  INTN               max_frl_mask,
  UINT8              frl_type
  );

INTN
drm_dp_pcon_reset_frl_config (
  struct drm_dp_aux  *aux
  );

INTN
drm_dp_pcon_frl_enable (
  struct drm_dp_aux  *aux
  );

BOOLEAN
drm_dp_pcon_hdmi_link_active (
  struct drm_dp_aux  *aux
  );

INTN
drm_dp_pcon_hdmi_link_mode (
  struct drm_dp_aux  *aux,
  UINT8              *frl_trained_mask
  );

BOOLEAN
drm_dp_pcon_enc_is_dsc_1_2 (
  const UINT8  pcon_dsc_dpcd[DP_PCON_DSC_ENCODER_CAP_SIZE]
  );

INTN
drm_dp_pcon_dsc_max_slices (
  const UINT8  pcon_dsc_dpcd[DP_PCON_DSC_ENCODER_CAP_SIZE]
  );

INTN
drm_dp_pcon_dsc_max_slice_width (
  const UINT8  pcon_dsc_dpcd[DP_PCON_DSC_ENCODER_CAP_SIZE]
  );

INTN
drm_dp_pcon_dsc_bpp_incr (
  const UINT8  pcon_dsc_dpcd[DP_PCON_DSC_ENCODER_CAP_SIZE]
  );

INTN
drm_dp_pcon_pps_default (
  struct drm_dp_aux  *aux
  );

INTN
drm_dp_pcon_pps_override_buf (
  struct drm_dp_aux  *aux,
  UINT8              pps_buf[128]
  );

INTN
drm_dp_pcon_pps_override_param (
  struct drm_dp_aux  *aux,
  UINT8              pps_param[6]
  );

BOOLEAN
drm_dp_downstream_rgb_to_ycbcr_conversion (
  const UINT8  dpcd[DP_RECEIVER_CAP_SIZE],
  const UINT8  port_cap[4],
  UINT8        color_spc
  );

INTN
drm_dp_pcon_convert_rgb_to_ycbcr (
  struct drm_dp_aux  *aux,
  UINT8              color_spc
  );

#endif /* _DRM_DP_HELPER_H_ */
