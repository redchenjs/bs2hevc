#ifndef _BS_H
#define	_BS_H

#include "type.h"
#include "common.h"
#include "config.h"

/****************************************************************************
 *						bs function
 ****************************************************************************/
#define WORD_SIZE sizeof(void*)

static inline uint32_t endian_fix32( uint32_t x )
{
    return (x<<24) + ((x<<8)&0xff0000) + ((x>>8)&0xff00) + (x>>24);
}
static inline intptr_t endian_fix( intptr_t x )
{
    return endian_fix32(x);
}

static const uint8_t f264_ue_size_tab[256] =
{
     1, 1, 3, 3, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7,
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
    13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
    13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
    13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
};

static const uint8_t intra4x4_cbp_to_golomb[48]=
{
  3, 29, 30, 17, 31, 18, 37,  8, 32, 38, 19,  9, 20, 10, 11,  2,
 16, 33, 34, 21, 35, 22, 39,  4, 36, 40, 23,  5, 24,  6,  7,  1,
 41, 42, 43, 25, 44, 26, 46, 12, 45, 47, 27, 13, 28, 14, 15,  0
};
static const uint8_t inter_cbp_to_golomb[48]=
{
  0,  2,  3,  7,  4,  8, 17, 13,  5, 18,  9, 14, 10, 15, 16, 11,
  1, 32, 33, 36, 34, 37, 44, 40, 35, 45, 38, 41, 39, 42, 43, 19,
  6, 24, 25, 20, 26, 21, 46, 28, 27, 47, 22, 29, 23, 30, 31, 12
};

static const uint8_t sub_mb_type_p_to_golomb[4]=
{
    3, 1, 2, 0
};

static const uint8_t sub_mb_type_b_to_golomb[13]=
{
    10,  4,  5,  1, 11,  6,  7,  2, 12,  8,  9,  3,  0
};

enum cabac_ctx_block_cat_e
{
    DCT_LUMA_DC   = 0,
    DCT_LUMA_AC   = 1,
    DCT_LUMA_4x4  = 2,
    DCT_CHROMA_DC = 3,
    DCT_CHROMA_AC = 4,
    DCT_LUMA_8x8  = 5,
};

typedef struct
{
    int     last;
    int16_t level[16];
    uint8_t run[16];
} f265_run_level_t;

typedef int (*coeff_level_run_t)( int16_t *dct, f265_run_level_t *runlevel );

void bs_init( bs_t *s, void *p_data, int i_data );
void bs_write( bs_t *s, int i_count, uint32_t i_bits );
void bs_write1( bs_t *s, uint32_t i_bit );
void bs_write32( bs_t *s, uint32_t i_bits );
int bs_pos( bs_t *s );
void bs_flush( bs_t *s );
void bs_rbsp_trailing( bs_t *s );
void bs_align_1( bs_t *s );
void bs_align_0( bs_t *s );
void bs_uvlc( bs_t *s, uint32_t val );
void bs_svlc( bs_t *s, int val );

/* Only works on values under 255. */
void bs_write_ue( bs_t *s, int val );
void bs_write_se( bs_t *s, int val );


#define bs_write_vlc(s,v) bs_write( s, (v).i_size, (v).i_bits )

/****************************************************************************
 *                   ME Cost Cal
 ****************************************************************************/
static inline int bs_size_ue( unsigned int val )
{
    return f264_ue_size_tab[val+1];
}

static inline int bs_size_ue_big( unsigned int val )
{
    if( val < 255 )
        return f264_ue_size_tab[val+1];
    else
        return f264_ue_size_tab[(val+1)>>8] + 16;
}

static inline int bs_size_se( int val )
{
    return bs_size_ue_big( val <= 0 ? -val*2 : val*2-1 );
}

/****************************************************************************
 *                   Bitstream Data Type Definition
 ****************************************************************************/
// nal packet
struct nal_t {
    int		i_ref_idc;  // nal_priority_e 
    int		i_type;     // nal_unit_type_e
    int     i_payload;  // num of byte in NAL
    uint8_t *p_payload; // start pt of NAL
};

/* bitstream output */
struct bs_out {
	int			i_nal;             // num of NAL in bs_out
	nal_t		nal[f264_NAL_MAX]; // NALs in bs_out
	bs_t		bs;                // bs in bs_out
	int         i_bitstream;       // size of bs_out buf
	uint8_t     *p_bitstream;	   // start pt of bs_out	
	int         i_frame_size;
} ;

typedef struct
{
	int video_parameter_set_id                            ;
    int vps_temporal_id_nesting_flag					  ;
    int vps_reserved_three_2bits						  ;
    int vps_reserved_zero_2bits							  ;
    int vps_reserved_zero_6bits							  ;
    int vps_max_sub_layers_minus1						  ;
    int vps_reserved_ffff_16bits						  ;
    int vps_reserved_zero_12bits						  ;
    int vps_sub_layer_ordering_info_present_flag		  ;
    uint32_t vps_max_dec_pic_buffering[2]				  ;
    uint32_t vps_num_reorder_pics[2]					  ;
    int vps_max_latency_increase[2]						  ;
    int vps_num_hrd_parameters							  ;
    int vps_max_nuh_reserved_zero_layer_id				  ;
    int op_layer_id_included_flag[2][2]					  ;
    int vps_extension_flag								  ;

	struct
	{
		int general_level_idc							  ;
		int sub_layer_profile_present_flag   			  ;
		int sub_layer_level_present_flag     			  ;
		int sub_layer_level_idc              			  ;
		int profile_space                    			  ;
		int tier_flag                        			  ;
		int profile_idc                      			  ;
		int profile_compatibility_flag       			  ;
		int reserved_zero_16bits						  ;
		int general_progressive_source_flag               ;     
		int general_interlaced_source_flag                ;    
		int general_non_packed_constraint_flag            ;    
		int general_frame_only_constraint_flag            ;    
	} ptl;

	int bit_rate_info_present_flag[2]					  ;
	int pic_rate_info_present_flag[2]    				  ;
	int avg_bit_rate[2]                  				  ;
	int max_bit_rate[2]                  				  ;
	int constant_pic_rate_idc[2]         				  ;
	int avg_pic_rate[2]									  ;
	int vps_max_op_sets_minus1                            ;
	int vps_timing_info_present_flag                      ;
	int vps_num_units_in_tick                             ;
	int vps_time_scale									  ;
	int vps_poc_proportional_to_timing_flag				  ;
	int vps_num_ticks_poc_diff_one_minus1				  ;
}vps_t;

typedef struct
{
    int video_parameter_set_id                            ;
	int sps_max_sub_layers_minus1						  ;
	int sps_temporal_id_nesting_flag					  ;

	struct
	{
		int general_level_idc							  ;				
		int sub_layer_profile_present_flag  			  ;
		int sub_layer_level_present_flag    			  ;
		int sub_layer_level_idc             			  ;
		int profile_space                   			  ;
		int tier_flag                       			  ;
		int profile_idc                     			  ;
		int profile_compatibility_flag      			  ;
		int reserved_zero_44bits            			  ;
		int general_progressive_source_flag               ;
		int general_interlaced_source_flag    			  ;
		int general_non_packed_constraint_flag			  ;
		int general_frame_only_constraint_flag			  ;
	} ptl;

	int sps_reserved_zero_bit							  ;
	int seq_parameter_set_id							  ;
	int chroma_format_idc								  ;
	int separate_colour_plane_flag						  ;
	int pic_width_in_luma_samples						  ;
	int pic_height_in_luma_samples						  ;
	int pic_cropping_flag								  ;

	struct 
	{
		int pic_crop_left_offset						  ;
		int pic_crop_right_offset						  ;
		int pic_crop_top_offset							  ;
		int pic_crop_bottom_offset						  ;
	} crop;

	int bit_depth_luma_minus8							  ;
	int bit_depth_chroma_minus8							  ;
	int pcm_enabled_flag								  ;
	int pcm_bit_depth_luma_minus1						  ;
	int pcm_bit_depth_chroma_minus1						  ;
	int log2_max_pic_order_cnt_lsb_minus4				  ;
	int sps_sub_layer_ordering_info_present_flag		  ;
	int max_dec_pic_buffering[2]						  ;
	int num_reorder_pics[2]								  ;
	int max_latency_increase[2]							  ;
	int restricted_ref_pic_lists_flag					  ;
	int lists_modification_present_flag					  ;
	int log2_min_coding_block_size_minus3				  ;
	int log2_diff_max_min_coding_block_size				  ;
	int log2_min_transform_block_size_minus2			  ;
	int log2_diff_max_min_transform_block_size			  ;
	int log2_min_pcm_coding_block_size_minus3			  ;
	int log2_diff_max_min_pcm_coding_block_size			  ;
	int max_transform_hierarchy_depth_inter				  ;
	int max_transform_hierarchy_depth_intra				  ;
	int scaling_list_enabled_flag						  ;
	int sps_scaling_list_data_present_flag				  ;
	int asymmetric_motion_partitions_enabled_flag		  ;
	int sample_adaptive_offset_enabled_flag				  ;

	int pcm_sample_bit_depth_luma_minus1				  ;
	int pcm_sample_bit_depth_chroma_minus1				  ;
	int log2_min_pcm_luma_coding_block_size_minus3		  ;
	int log2_diff_max_min_pcm_luma_coding_block_size	  ;
	int pcm_loop_filter_disable_flag					  ;
	int temporal_id_nesting_flag						  ;
	int num_short_term_ref_pic_sets						  ;
	int inter_ref_pic_set_prediction_flag				  ;	
														  
	int num_negative_pics								  ;
	int num_positive_pics								  ;
	int delta_poc_s0_minus1								  ;
	int used_by_curr_pic_s0_flag						  ;
	int delta_poc_s1_minus1								  ;
	int used_by_curr_pic_s1_flag						  ;

	int long_term_ref_pics_present_flag					  ;
	int num_long_term_ref_pic_sps						  ;
	int lt_ref_pic_poc_lsb_sps							  ;
	int used_by_curr_pic_lt_sps_flag					  ;
	int sps_temporal_mvp_enable_flag					  ;
	int sps_strong_intra_smoothing_enable_flag			  ;
	int vui_parameters_present_flag						  ;
	int sps_extension_flag								  ;

	struct
	{
		int aspect_ratio_info_present_flag                ;
		int aspect_ratio_idc							  ;
		int sar_width									  ;
		int sar_height									  ;
		int overscan_info_present_flag					  ;
		int overscan_appropriate_flag					  ;
		int video_signal_type_present_flag				  ;
		int video_format								  ;
		int video_full_range_flag						  ;
		int colour_description_present_flag				  ;
		int colour_primaries							  ;
		int transfer_characteristics					  ;
		int matrix_coefficients							  ;
		int chroma_loc_info_present_flag				  ;
		int chroma_sample_loc_type_top_field			  ;
		int chroma_sample_loc_type_bottom_field			  ;
		int neutral_chroma_indication_flag				  ;
		int field_seq_flag								  ;
		int default_display_window_flag					  ;
		int pic_struct_present_flag						  ;
		int hrd_parameters_present_flag					  ;
		int timing_info_present_flag					  ;
		int num_units_in_tick							  ;
		int time_scale									  ;
		int nal_hrd_parameters_present_flag				  ;
		int vcl_hrd_parameters_present_flag				  ;
		int sub_pic_cpb_params_present_flag				  ;
		int tick_divisor_minus2							  ;
		int du_cpb_removal_delay_length_minus1			  ;
		int bit_rate_scale								  ;
		int cpb_size_scale								  ;
		int du_cpb_size_scale							  ;
		int initial_cpb_removal_delay_length_minus1		  ;
		int cpb_removal_delay_length_minus1				  ;
		int dpb_output_delay_length_minus1				  ;
		int fixed_pic_rate_flag							  ;
		int pic_duration_in_tc_minus1					  ;
		int low_delay_hrd_flag							  ;
		int cpb_cnt_minus1								  ;
		int bit_size_value_minus1						  ;
		int cpb_size_value_minus1						  ;
		int du_cpb_size_value_minus1					  ;
		int cbr_flag									  ;
		int poc_proportional_to_timing_flag				  ;
		int num_ticks_poc_diff_one_minus1				  ;
		int bitstream_restriction_flag					  ;
		int tiles_fixed_structure_flag					  ;
		int motion_vectors_over_pic_boundaries_flag		  ;
		int restricted_ref_pic_lists_flag				  ;
		int min_spatial_segmentation_idc				  ;
		int max_bytes_per_pic_denom						  ;
		int max_bits_per_mincu_denom					  ;
		int log2_max_mv_length_horizontal				  ;
		int log2_max_mv_length_vertical					  ;
	} vui;
} sps_t;

typedef struct
{
	int pic_parameter_set_id                         ;       
	int seq_parameter_set_id						 ;
	int dependent_slice_enabled_flag				 ;
	int sign_data_hiding_flag						 ;
	int cabac_init_present_flag						 ;
	int num_ref_idx_l0_default_active_minus1		 ;
	int num_ref_idx_l1_default_active_minus1		 ;
	int pic_init_qp_minus26							 ;
	int constrained_intra_pred_flag					 ;
	int transform_skip_enabled_flag					 ;
	int cu_qp_delta_enabled_flag					 ;
	int diff_cu_qp_delta_depth						 ;
	int cb_qp_offset								 ;
	int cr_qp_offset								 ;
	int slicelevel_chroma_qp_flag					 ;
	int weighted_pred_flag							 ;
	int weighted_bipred_flag						 ;
	int output_flag_present_flag					 ;
	int transquant_bypass_enable_flag				 ;
	int tiles_enabled_flag							 ;
	int entropy_coding_sync_enabled_flag			 ;
	int entropy_slice_enabled_flag					 ;
	int num_tile_columns_minus1						 ;
	int num_tile_rows_minus1						 ;
	int uniform_spacing_flag						 ;
	int column_width_minus1							 ;
	int row_height_minus1							 ;
	int loop_filter_across_tiles_enabled_flag		 ;
	int loop_filter_across_slices_enabled_flag		 ;
	int deblocking_filter_control_present_flag		 ;
	int deblocking_filter_override_enabled_flag		 ;
	int pic_disable_deblocking_filter_flag			 ;
	int pps_beta_offset_div2						 ;
	int pps_tc_offset_div2							 ;
	int pps_scaling_list_data_present_flag			 ;
	int scaling_list_pred_mode_flag					 ;
	int scaling_list_pred_matrix_id_delta			 ;
	int scaling_list_dc_coef_minus8					 ;
	int	scaling_list_delta_coef						 ;
	int lists_modification_present_flag				 ;
	int log2_parallel_merge_level_minus2			 ;
	int num_extra_slice_header_bits					 ;
	int slice_header_extension_present_flag			 ;
	int pps_extension_flag							 ;
} pps_t;

typedef struct
{
	sps_t *sps;
	pps_t *pps;

    int first_slice_in_pic_flag                      ;
	int no_output_of_prior_pics_flag				 ;
	int pic_parameter_set_id						 ;
	int slice_address								 ;
	int dependent_slice_flag						 ;
	int slice_reserved_undetermined_flag[2]			 ;
	int slice_type									 ;
	int pic_output_flag								 ;
	int pic_order_cnt_lsb							 ;
	int short_term_ref_pic_set_sps_flag				 ;
	int short_term_ref_pic_set_idx					 ;
	int num_long_term_sps							 ;
	int num_long_term_pics							 ;
	int lt_idx_sps[2]								 ;
	int poc_lsb_lt									 ;
	int used_by_curr_pic_lt_flag					 ;
	int delta_poc_msb_present_flag					 ;
	int delta_poc_msb_cycle_lt[2]					 ;
	int slice_sao_luma_flag							 ;
	int slice_sao_chroma_flag						 ;
	int enable_temporal_mvp_flag					 ;
	int num_ref_idx_active_override_flag			 ;
	int num_ref_idx_l0_active_minus1				 ;
	int num_ref_idx_l1_active_minus1				 ;
	int ref_pic_list_modification_flag_l0			 ;
	int list_entry_l0								 ;
	int ref_pic_list_modification_flag_l1			 ;
	int list_entry_l1								 ;
	int mvd_l1_zero_flag							 ;
	int cabac_init_flag								 ;
	int collocated_from_l0_flag						 ;
	int collocated_ref_idx							 ;
	int five_minus_max_num_merge_cand				 ;
	int slice_qp_delta								 ;
	int slice_qp_delta_cb							 ;
	int slice_qp_delta_cr							 ;
	int deblocking_filter_override_flag				 ;
	int slice_disable_deblocking_filter_flag		 ;
	int beta_offset_div2							 ;
	int tc_offset_div2								 ;
	int slice_loop_filter_across_slices_enabled_flag ;
	int slice_header_extension_length				 ;

} slice_header_t;


/****************************************************************************
 *						          bs module
 ****************************************************************************/
struct bs{
    /*****************************************
	*              encoder if                *
	*****************************************/
	bool		start_h;
	bool		start_e;
    param_t		input_param;	
	ec_t		input_ec;

	/*****************************************
	*              private var               *
	*****************************************/
	// input var
	ec_t    ec;
	param_t param;

	// bs buf var
	bs_out  bs_pt;

	//bs for bs_check
	bs_t bs_txt;

	// header info
	vps_t			vps;
	sps_t			sps;
	pps_t			pps;
	slice_header_t	sh;
	int				i_idr_pic_id;
	int				i_last_idr;
	int				i_poc;

	int				start_byte_num;

	// dump
	FILE *fp;
	uint8_t * p_bitstream;

	/*****************************************
	*              private func              *
	*****************************************/
	void init();
	void init_buf();
	void dump();

	void f265_check2hevc(bs_t *s, FILE *fp);
	void f265_nal_start( int i_type, int i_ref_idc, bs_out *out );
	void f265_nal_end( bs_out *out );
	void f265_vps_write( bs_t *s, vps_t *vps );
	void f265_sps_write( bs_t *s ,sps_t *sps );
	void f265_pps_write( bs_t *s,pps_t *pps );
	void f265_slice_header_write( bs_t *s, slice_header_t *sh, int i_nal_ref_idc, int frame_num );
	void f265_slice_data_write( bs_t *s, bs_t *ec);
	void f265_nal_encode( void *p_data, int *pi_data, int b_annexeb, nal_t *nal );
	void f265_nal_encodeN03( void *p_data, int *pi_data, int b_annexeb, nal_t *nal );
	void vps_init();
	void sps_init();
	void pps_init(int i_qp);
	void sh_init(sps_t *sps, pps_t *pps, int i_type, int i_frame, int i_qp);

	// module proc
	void bs_proc();
};

#endif	/* _BS_H */