#include "bs.h"

void bs::bs_proc(){
	char i_buf;
	string s=input_param.file_name;
	FILE *check_fp;
	check_fp=fopen(s.c_str(),"r");
	fscanf(check_fp,"%c",&i_buf);		// "@"
	init();

	while (true) {
		int sum=0;
		int tmp;

		param = input_param;
		init_buf();
		
		fscanf(check_fp,"%c",&i_buf);
		while(i_buf!=0xa){		// end of line
			tmp=i_buf-0x30;
			sum=sum*10+tmp;
			fscanf(check_fp,"%c",&i_buf);
		}
		param.frame_num=sum;
		if(param.frame_num%param.gop_length==0){
			param.type=SLICE_TYPE_I;
		}
		else{
			param.type=SLICE_TYPE_P;
		}
		
		sh_init(&sps, &pps, param.type, param.frame_num, param.qp);

		if (param.type == SLICE_TYPE_I){
			if(param.frame_num==0) 
			{
				vps_init();
				sps_init();
				pps_init(param.qp);
				
				f265_nal_start( NAL_UNIT_VPS, NAL_PRIORITY_HIGHEST, &bs_pt );
				f265_vps_write( &bs_pt.bs, &vps );
				f265_nal_end( &bs_pt );
				
				f265_nal_start( NAL_UNIT_SPS, NAL_PRIORITY_HIGHEST, &bs_pt );
				f265_sps_write( &bs_pt.bs, &sps );
				f265_nal_end( &bs_pt );

				f265_nal_start( NAL_UNIT_PPS, NAL_PRIORITY_HIGHEST, &bs_pt );
				f265_pps_write( &bs_pt.bs, &pps );
				f265_nal_end( &bs_pt );
				
				f265_nal_start( NAL_UNIT_CODED_SLICE_IDR, NAL_PRIORITY_HIGHEST, &bs_pt);
				f265_slice_header_write( &bs_pt.bs, &sh, NAL_PRIORITY_HIGHEST, param.frame_num );
				}
			else{
				f265_nal_start( NAL_UNIT_CODED_SLICE_TRAIL_N, NAL_PRIORITY_HIGHEST, &bs_pt);
				f265_slice_header_write( &bs_pt.bs, &sh, NAL_PRIORITY_HIGHEST, param.frame_num );
			}
		}
		else{
			f265_nal_start( NAL_UNIT_CODED_SLICE_TRAIL_R, NAL_PRIORITY_HIGH, &bs_pt);
			f265_slice_header_write( &bs_pt.bs, &sh, NAL_PRIORITY_HIGH, param.frame_num );
		}					
		f265_check2hevc( &bs_pt.bs, check_fp );
		f265_nal_end( &bs_pt );				
		dump();
		if(param.frame_num==param.frame_total-1){
			break;
		}
	}
	fclose(check_fp);
}

void bs::f265_check2hevc(bs_t *s, FILE *fp){
	char i_buf;
	while((fscanf(fp,"%c",&i_buf))!=EOF){	// read in ascii
		if(i_buf==0x40){

			if(s->i_left != WORD_SIZE*8) {
				if(s->i_left == 8){
					*(s->p) = (s->cur_bits & 0xff0000) >> 16;
					*(s->p+1) = (s->cur_bits & 0x00ff00) >> 8;
					*(s->p+2) = (s->cur_bits & 0x0000ff);
				}	
				else if(s->i_left == 16) {
					*(s->p) = (s->cur_bits & 0xff00) >> 8;
					*(s->p+1) = (s->cur_bits & 0xff);
				}
				else if(s->i_left == 24)
					*(s->p) = (s->cur_bits & 0xff);
			}

			return;
		}
		if(i_buf==0xd||i_buf==0xa){		// \r\n
				continue;
		}
		switch(i_buf){
			case 0x30:	//0
				bs_write1(s,0);
				bs_write1(s,0);
				bs_write1(s,0);
				bs_write1(s,0);
				break;
			case 0x31:	//1
				bs_write1(s,0);
				bs_write1(s,0);
				bs_write1(s,0);
				bs_write1(s,1);
				break;
			case 0x32:	//2
				bs_write1(s,0);
				bs_write1(s,0);
				bs_write1(s,1);
				bs_write1(s,0);
				break;
			case 0x33:	//3
				bs_write1(s,0);
				bs_write1(s,0);
				bs_write1(s,1);
				bs_write1(s,1);
				break;
			case 0x34:	//4
				bs_write1(s,0);
				bs_write1(s,1);
				bs_write1(s,0);
				bs_write1(s,0);
				break;
			case 0x35:	//5
				bs_write1(s,0);
				bs_write1(s,1);
				bs_write1(s,0);
				bs_write1(s,1);
				break;
			case 0x36:	//6
				bs_write1(s,0);
				bs_write1(s,1);
				bs_write1(s,1);
				bs_write1(s,0);
				break;
			case 0x37:	//7
				bs_write1(s,0);
				bs_write1(s,1);
				bs_write1(s,1);
				bs_write1(s,1);
				break;
			case 0x38:	//8
				bs_write1(s,1);
				bs_write1(s,0);
				bs_write1(s,0);
				bs_write1(s,0);
				break;
			case 0x39:	//9
				bs_write1(s,1);
				bs_write1(s,0);
				bs_write1(s,0);
				bs_write1(s,1);
				break;
			case 0x61:	//a
				bs_write1(s,1);
				bs_write1(s,0);
				bs_write1(s,1);
				bs_write1(s,0);
				break;
			case 0x62:	//b
				bs_write1(s,1);
				bs_write1(s,0);
				bs_write1(s,1);
				bs_write1(s,1);
				break;
			case 0x63:	//c
				bs_write1(s,1);
				bs_write1(s,1);
				bs_write1(s,0);
				bs_write1(s,0);
				break;
			case 0x64:	//d
				bs_write1(s,1);
				bs_write1(s,1);
				bs_write1(s,0);
				bs_write1(s,1);
				break;
			case 0x65:	//e
				bs_write1(s,1);
				bs_write1(s,1);
				bs_write1(s,1);
				bs_write1(s,0);
				break;
			case 0x66:	//f
				bs_write1(s,1);
				bs_write1(s,1);
				bs_write1(s,1);
				bs_write1(s,1);
				break;
		}
	}

	if(s->i_left != WORD_SIZE*8) {
		if(s->i_left == 8){
			*(s->p) = (s->cur_bits & 0xff0000) >> 16;
			*(s->p+1) = (s->cur_bits & 0x00ff00) >> 8;
			*(s->p+2) = (s->cur_bits & 0x0000ff);
		}	
		else if(s->i_left == 16) {
			*(s->p) = (s->cur_bits & 0xff00) >> 8;
			*(s->p+1) = (s->cur_bits & 0xff);
		}
		else if(s->i_left == 24)
			*(s->p) = (s->cur_bits & 0xff);
	}
}

void bs::init(){
	start_byte_num=0;
	fp=fopen("bs.hevc","w");
	fclose(fp);
}

void bs::init_buf(){

//	memset(bs_buf, 0, sizeof(bs_buf));                               
	bs_pt.i_nal = 0;
	bs_pt.i_bitstream = BS_BUF_SIZE*param.frame_mb_x_total*param.frame_mb_y_total    ; 
	bs_pt.p_bitstream = (uint8_t*)malloc( bs_pt.i_bitstream );       
	bs_init(&bs_pt.bs, bs_pt.p_bitstream, bs_pt.i_bitstream) ;

	//bs_txt
	int i_bitstream = BS_BUF_SIZE*param.frame_mb_x_total*param.frame_mb_y_total;
	p_bitstream = (uint8_t*)malloc(i_bitstream);
	bs_init(&bs_txt, p_bitstream, i_bitstream) ;
}

void bs::dump(){
//dump hevc bin file
	fp=fopen("bs.hevc","ab");	
	int i_total=0;
	for( int i = 0; i < bs_pt.i_nal; i++ ) {
		int i_size;
		uint8_t *i_buf;
		i_size = bs_pt.nal[i].i_payload*2 + 4; // actual bs num is i_payload + 5(annex), for 00->add 03 consideration
		i_buf = (uint8_t*)malloc( sizeof(uint8_t)*i_size );
		f265_nal_encode( i_buf, &i_size, 1, &bs_pt.nal[i] );

		if (fwrite(i_buf, i_size, 1, fp) > 0)
			i_total+=i_size;
        else
			i_total+=-1;		
		free(i_buf);
	}
	fclose(fp);
}

/****************************************************************************
 *						   bs member func
 ****************************************************************************/
void bs::vps_init() {
	vps.video_parameter_set_id							= 0 ;  
    vps.vps_temporal_id_nesting_flag					= true ;
    vps.vps_reserved_three_2bits						= 3 ;            
    vps.vps_reserved_zero_2bits							= 0 ;            
    vps.vps_reserved_zero_6bits							= 0 ;            
    vps.vps_max_sub_layers_minus1						= 0 ;       	 
    vps.vps_reserved_ffff_16bits						= 0xffff ;       
    vps.vps_reserved_zero_12bits						= 0 ;            
    vps.vps_sub_layer_ordering_info_present_flag		= true ;
	if(param.gop_length==1)									
		vps.vps_max_dec_pic_buffering[0]				= 0 ;        
	else												
		vps.vps_max_dec_pic_buffering[0]				= 1 ;
    vps.vps_num_reorder_pics[0]							= 0 ;
    vps.vps_max_latency_increase[0]						= 0 ;
    vps.vps_num_hrd_parameters							= 0 ;
    vps.vps_max_nuh_reserved_zero_layer_id				= 0 ;
    vps.op_layer_id_included_flag[0][0]					= 0 ;			
    vps.vps_num_hrd_parameters							= 0 ;
    vps.vps_extension_flag								= 0 ;			
    													
	//ptl,profile tier									
	vps.ptl.general_level_idc							= 0 ;
	vps.ptl.sub_layer_profile_present_flag				= 0 ;
	vps.ptl.sub_layer_level_present_flag				= 0 ;
	vps.ptl.sub_layer_level_idc							= 0 ;
	vps.ptl.profile_space                       		= 0 ;
	vps.ptl.tier_flag                           		= false ;
    vps.ptl.profile_idc                         		= 0 ;
    vps.ptl.profile_compatibility_flag          		= true  ;        
    vps.ptl.reserved_zero_16bits                		= 0 ;           
	vps.ptl.general_progressive_source_flag				= 0 ;     
	vps.ptl.general_interlaced_source_flag				= 0 ;    
	vps.ptl.general_non_packed_constraint_flag			= 0 ;    
	vps.ptl.general_frame_only_constraint_flag			= 0 ;    
														
	//bit rate info													 					
	vps.bit_rate_info_present_flag[0]					= false ;
	vps.pic_rate_info_present_flag[0]					= false ;
	vps.avg_bit_rate[0]									= 0 ;		
	vps.max_bit_rate[0]									= 0 ;
	vps.constant_pic_rate_idc[0]						= 0 ;		
	vps.avg_pic_rate[0]									= 0 ;
	vps.vps_max_op_sets_minus1							= 0 ;
	vps.vps_timing_info_present_flag					= 0      ;
	vps.vps_num_units_in_tick							= 1001   ;
	vps.vps_time_scale									= 60000	 ;
	vps.vps_poc_proportional_to_timing_flag				= 0	;
	vps.vps_num_ticks_poc_diff_one_minus1				= 0	;
}		
												
void bs::sps_init(){									
	sps.video_parameter_set_id							= 0 ;  
	sps.sps_max_sub_layers_minus1						= 0 ;        
	sps.sps_temporal_id_nesting_flag					= true  ;	
														
	//ptl,profile tier									
	sps.ptl.general_level_idc							= 0 ;		
	sps.ptl.sub_layer_profile_present_flag				= 0 ;		
	sps.ptl.sub_layer_level_present_flag				= 0 ;		
	sps.ptl.sub_layer_level_idc							= 0 ;		
	sps.ptl.profile_space                       		= 0 ;		
	sps.ptl.tier_flag                           		= false ;
    sps.ptl.profile_idc                         		= 0 ;
    sps.ptl.profile_compatibility_flag          		= true  ;  	
    sps.ptl.reserved_zero_44bits                		= 0 ;    	
	sps.ptl.general_progressive_source_flag				= 0 ;
	sps.ptl.general_interlaced_source_flag    			= 0 ;
	sps.ptl.general_non_packed_constraint_flag			= 0 ;
	sps.ptl.general_frame_only_constraint_flag			= 0 ;
	
	sps.sps_reserved_zero_bit                           = 0 ;		
	sps.seq_parameter_set_id                            = 0 ;
	sps.chroma_format_idc                               = 1 ;
	sps.separate_colour_plane_flag                      = 0 ;		
	sps.pic_width_in_luma_samples                       = param.frame_width  ;   
	sps.pic_height_in_luma_samples                      = param.frame_height ;   
	
	sps.pic_cropping_flag                               = true  ;
	sps.crop.pic_crop_left_offset                       = 0 ;
	sps.crop.pic_crop_right_offset                      = 0 ;	
	sps.crop.pic_crop_top_offset                        = 0 ;
	sps.crop.pic_crop_bottom_offset                     = 0 ;	
	
	sps.bit_depth_luma_minus8                           = BIT_DEPTH-8 ;		
	sps.bit_depth_chroma_minus8                         = BIT_DEPTH-8 ;		
	sps.pcm_enabled_flag                                = 0 ;		
	sps.pcm_bit_depth_luma_minus1                       = 0 ;		
	sps.pcm_bit_depth_chroma_minus1                     = 0 ;		
	sps.log2_max_pic_order_cnt_lsb_minus4               = 4 ;		
	sps.sps_sub_layer_ordering_info_present_flag        = 1 ;		
	if(param.gop_length==1)
		sps.max_dec_pic_buffering[0]                    = 0 ;
	else
		sps.max_dec_pic_buffering[0]                    = 1 ;	
	sps.num_reorder_pics[0]                             = 0 ;
	sps.max_latency_increase[0]                         = 0 ;
	sps.restricted_ref_pic_lists_flag                   = 0;	
	sps.lists_modification_present_flag                 = 0;	
	sps.log2_min_coding_block_size_minus3               = 0 ;	
	sps.log2_diff_max_min_coding_block_size             = 3 ;	
	sps.log2_min_transform_block_size_minus2            = 0 ;	
	sps.log2_diff_max_min_transform_block_size          = 3 ;	
	sps.log2_min_pcm_coding_block_size_minus3           = 0 ;	
	sps.log2_diff_max_min_pcm_coding_block_size         = 0 ;	
	sps.max_transform_hierarchy_depth_inter             = 2	;	
	sps.max_transform_hierarchy_depth_intra             = 2	;	
	sps.scaling_list_enabled_flag                       = false ;
	sps.sps_scaling_list_data_present_flag              = 0 ;		
	sps.asymmetric_motion_partitions_enabled_flag       = true  ;
#ifdef  SAO_OPEN
	sps.sample_adaptive_offset_enabled_flag             = true  ;
#else
	sps.sample_adaptive_offset_enabled_flag             = false ;
#endif
	sps.pcm_enabled_flag                                = false ;
	sps.pcm_loop_filter_disable_flag                    = 0 ;		
	sps.pcm_sample_bit_depth_luma_minus1                = 7 ;		
	sps.pcm_sample_bit_depth_chroma_minus1              = 7 ;		
	sps.log2_min_pcm_luma_coding_block_size_minus3      = 0 ;		
	sps.log2_diff_max_min_pcm_luma_coding_block_size    = 2 ;		
	sps.pcm_loop_filter_disable_flag                    = 0 ;		
	sps.temporal_id_nesting_flag                        = 0 ;		
	sps.num_short_term_ref_pic_sets                     = 1 ;
	sps.inter_ref_pic_set_prediction_flag				= false;	
																	
	if(param.gop_length==1)
		sps.num_negative_pics							= 0;
	else
		sps.num_negative_pics							= 1 ;
	sps.num_positive_pics								= 0 ;
	sps.delta_poc_s0_minus1								= -1;   	
	sps.used_by_curr_pic_s0_flag						= 1 ;
	sps.delta_poc_s1_minus1								= -1;   	
	sps.used_by_curr_pic_s1_flag						= 1 ;
	
	
	sps.long_term_ref_pics_present_flag                 = false ;
	sps.num_long_term_ref_pic_sps                       = 0 ;		
	sps.lt_ref_pic_poc_lsb_sps                          = 0 ;		
	sps.used_by_curr_pic_lt_sps_flag                    = 0 ;		
	sps.sps_temporal_mvp_enable_flag                    = true  ;
	sps.sps_strong_intra_smoothing_enable_flag          = false ;
	sps.vui_parameters_present_flag                     = false ;
	sps.sps_extension_flag                              = 0 ;		
	

	//vui
	sps.vui.aspect_ratio_info_present_flag              = 0 ;		
	sps.vui.aspect_ratio_idc                            = 0 ;		
	sps.vui.sar_width                                   = 0 ;		
	sps.vui.sar_height                                  = 0 ;		
	sps.vui.overscan_info_present_flag                  = 0 ;		
	sps.vui.overscan_appropriate_flag                   = 0 ;		
	sps.vui.video_signal_type_present_flag              = 0 ;		
	sps.vui.video_format                                = 0 ;		
	sps.vui.video_full_range_flag                       = 0 ;		
	sps.vui.colour_description_present_flag             = 0 ;		
	sps.vui.colour_primaries                            = 0 ;		
	sps.vui.transfer_characteristics                    = 0 ;		
	sps.vui.matrix_coefficients                         = 0 ;		
	sps.vui.chroma_loc_info_present_flag                = 0 ;		
	sps.vui.chroma_sample_loc_type_top_field            = 0 ;		
	sps.vui.chroma_sample_loc_type_bottom_field         = 0 ;		
	sps.vui.neutral_chroma_indication_flag              = 0 ;		
	sps.vui.field_seq_flag                              = 0 ;		
	sps.vui.default_display_window_flag                 = 0 ;		
	sps.vui.pic_struct_present_flag                     = 0 ;		
	sps.vui.hrd_parameters_present_flag                 = 0 ;		
	sps.vui.timing_info_present_flag                    = 0 ;		
	sps.vui.num_units_in_tick                           = 0 ;		
	sps.vui.time_scale                                  = 0 ;		
	sps.vui.nal_hrd_parameters_present_flag             = 0 ;		
	sps.vui.vcl_hrd_parameters_present_flag             = 0 ;		
	sps.vui.sub_pic_cpb_params_present_flag             = 0 ;		
	sps.vui.tick_divisor_minus2                         = 0 ;		
	sps.vui.du_cpb_removal_delay_length_minus1          = 0 ;		
	sps.vui.bit_rate_scale                              = 0 ;		
	sps.vui.cpb_size_scale                              = 0 ;		
	sps.vui.du_cpb_size_scale                           = 0 ;		
    sps.vui.initial_cpb_removal_delay_length_minus1     = 0 ;		
	sps.vui.cpb_removal_delay_length_minus1             = 0 ;		
	sps.vui.dpb_output_delay_length_minus1              = 0 ;		
	sps.vui.fixed_pic_rate_flag                         = 0 ;		
	sps.vui.pic_duration_in_tc_minus1                   = 0 ;		
	sps.vui.low_delay_hrd_flag                          = 0 ;		
	sps.vui.cpb_cnt_minus1                              = 0 ;		
	sps.vui.bit_size_value_minus1                       = 0 ;		
	sps.vui.cpb_size_value_minus1                       = 0 ;		
	sps.vui.du_cpb_size_value_minus1                    = 0 ;		
	sps.vui.cbr_flag                                    = 0 ;		
	sps.vui.poc_proportional_to_timing_flag             = 0 ;		
	sps.vui.num_ticks_poc_diff_one_minus1               = 0 ;		
	sps.vui.bitstream_restriction_flag                  = 0 ;		
	sps.vui.tiles_fixed_structure_flag                  = 0 ;		
	sps.vui.motion_vectors_over_pic_boundaries_flag     = 0 ;		
	sps.vui.restricted_ref_pic_lists_flag               = 0 ;		
	sps.vui.min_spatial_segmentation_idc                = 0 ;		
	sps.vui.max_bytes_per_pic_denom                     = 0 ;		
	sps.vui.max_bits_per_mincu_denom                    = 0 ;		
	sps.vui.log2_max_mv_length_horizontal               = 0 ;		
	sps.vui.log2_max_mv_length_vertical                 = 0 ;		

}

void bs::pps_init(int i_qp){
	pps.pic_parameter_set_id        					= 0 ;									        
	pps.seq_parameter_set_id				            = 0 ;
	pps.dependent_slice_enabled_flag		            = false ;
	pps.sign_data_hiding_flag                           = 0 ;          
	pps.cabac_init_present_flag                         = true  ;
	if(param.gop_length==1)
	{
		pps.num_ref_idx_l0_default_active_minus1        = 3 ;  		
		pps.num_ref_idx_l1_default_active_minus1        = 3;		
	}
	else
	{
		pps.num_ref_idx_l0_default_active_minus1        = 0 ;  		
		pps.num_ref_idx_l1_default_active_minus1        = 0;		
	}
	pps.pic_init_qp_minus26                             = 0 ;
	pps.constrained_intra_pred_flag                     = false ;
	pps.transform_skip_enabled_flag                     = true  ;
	pps.cu_qp_delta_enabled_flag                        = true ;
	pps.diff_cu_qp_delta_depth                          = 0 ;			
	pps.cb_qp_offset                                    = 0 ;
    pps.cr_qp_offset                                    = 0 ;
#ifdef SAO_OPEN
	if( param.qp<23)
		pps.slicelevel_chroma_qp_flag = true;
	else if(param.qp>22 && param.qp<30) //25-27:true/false  
		pps.slicelevel_chroma_qp_flag = false;
	else if(param.qp>29)//30-33:true else true/false 
		pps.slicelevel_chroma_qp_flag = true;
	else 
		pps.slicelevel_chroma_qp_flag = false;
#else 
	if(param.qp>=0 && param.qp<=10)
		pps.slicelevel_chroma_qp_flag = false;
	else if(param.qp>=11 && param.qp<=18)
		pps.slicelevel_chroma_qp_flag = true;
	else if(param.qp>=19 && param.qp<=25)
		pps.slicelevel_chroma_qp_flag = false;
	else if(param.qp==26)
		pps.slicelevel_chroma_qp_flag = true;
	else if(param.qp>=27 && param.qp<=33)
		pps.slicelevel_chroma_qp_flag = false;
	else if(param.qp>=34 && param.qp<=41)
		pps.slicelevel_chroma_qp_flag = true;
	else if(param.qp>=42 && param.qp<=51)
		pps.slicelevel_chroma_qp_flag = false;
	else 
		pps.slicelevel_chroma_qp_flag = false;
#endif 

	pps.weighted_pred_flag                              = false ;
	pps.weighted_bipred_flag                            = false ;
	pps.output_flag_present_flag                        = false ;
	pps.transquant_bypass_enable_flag                   = false ;
	pps.tiles_enabled_flag                              = false ;
	pps.entropy_coding_sync_enabled_flag                = false ;
	pps.entropy_slice_enabled_flag                      = 0 ;			
	pps.num_tile_columns_minus1                         = 0 ;			
	pps.num_tile_rows_minus1                            = 0 ;			
	pps.uniform_spacing_flag                            = 0 ;			
	pps.column_width_minus1                             = 0 ;			
	pps.row_height_minus1                               = 0 ;			
	pps.loop_filter_across_tiles_enabled_flag           = 0 ;			
	pps.loop_filter_across_slices_enabled_flag          = false  ;      
	pps.deblocking_filter_control_present_flag          = true   ;       
	pps.deblocking_filter_override_enabled_flag         = 0 ;		
#ifdef  DB_OPEN
	pps.pic_disable_deblocking_filter_flag              = 0 ;		
#else 
	pps.pic_disable_deblocking_filter_flag              = 1 ;		
#endif 
	pps.pps_beta_offset_div2                            = 0 ;			
	pps.pps_tc_offset_div2                              = 0 ;			
	pps.pps_scaling_list_data_present_flag              = false ;
	pps.scaling_list_pred_mode_flag						= 0 ;			
	pps.scaling_list_pred_matrix_id_delta				= 0 ;			
	pps.scaling_list_dc_coef_minus8						= 0 ;			
	pps.scaling_list_delta_coef							= 0 ;			
	pps.lists_modification_present_flag                 = false ;
	pps.log2_parallel_merge_level_minus2                = 0 ;
	pps.num_extra_slice_header_bits                     = 0 ;
	pps.slice_header_extension_present_flag             = false ;
	pps.pps_extension_flag                              = 0 ;			
}

void bs::sh_init(sps_t *sps, pps_t *pps, int i_type, int i_frame, int i_qp){
	sh.sps = sps;
	sh.pps = pps;

	sh.first_slice_in_pic_flag							= 1 ;
	sh.no_output_of_prior_pics_flag	                    = 0 ;
	sh.pic_parameter_set_id                             = 0 ;			
	sh.dependent_slice_flag                             = 0 ;			
	sh.slice_address                                    = 0 ;			
	sh.dependent_slice_flag                             = 0 ;			
	sh.slice_reserved_undetermined_flag[0]              = 0 ;			
	sh.slice_type                                       = param.type ;	
	sh.pic_output_flag                                  = 1 ;			
	sh.pic_order_cnt_lsb                                = 0 ;			
	sh.short_term_ref_pic_set_sps_flag                  = 0 ;			
	sh.short_term_ref_pic_set_sps_flag                  = 0 ;			
	sh.short_term_ref_pic_set_idx                       = 0 ;			
	sh.num_long_term_sps                                = 0 ;			
	sh.num_long_term_pics                               = 0 ;			
	sh.lt_idx_sps[0]                                    = 0 ;			
	sh.poc_lsb_lt                                       = 0 ;			
	sh.used_by_curr_pic_lt_flag                         = 0 ;			
	sh.delta_poc_msb_present_flag                       = 0 ;			
	sh.delta_poc_msb_cycle_lt[0]                        = 0 ;			
	sh.slice_sao_luma_flag                              = true ;
	sh.slice_sao_chroma_flag                            = true ;
	sh.enable_temporal_mvp_flag                         = 0 ;			
	sh.num_ref_idx_active_override_flag                 = 0 ;			
	sh.num_ref_idx_l0_active_minus1                     = 0 ;			
	sh.num_ref_idx_l1_active_minus1                     = 0 ;			
	sh.ref_pic_list_modification_flag_l0                = 0 ;			
	sh.list_entry_l0                                    = 0 ;			
	sh.ref_pic_list_modification_flag_l1                = 0 ;			
	sh.list_entry_l1                                    = 0 ;			
	sh.mvd_l1_zero_flag                                 = 0 ;			
	sh.cabac_init_flag                                  = 0 ;			
	sh.collocated_from_l0_flag                          = 1 ;			
	sh.collocated_ref_idx                               = 0 ;			
	sh.five_minus_max_num_merge_cand                    = 0 ;			
	sh.slice_qp_delta                                   = 6 ;
	sh.slice_qp_delta_cb                                = 0 ;			
	sh.slice_qp_delta_cr                                = 0 ;			
	sh.deblocking_filter_override_flag                  = 0 ;			
	sh.slice_disable_deblocking_filter_flag             = 1 ;			
	sh.beta_offset_div2                                 = 0 ;			
	sh.tc_offset_div2                                   = 0 ;			
	sh.slice_loop_filter_across_slices_enabled_flag     = false ;      
	sh.slice_header_extension_length                    = 0 ;			
}

void bs::f265_nal_start( int i_type, int i_ref_idc, bs_out *out )
{
    nal_t *nal = &out->nal[out->i_nal];//add

    nal->i_ref_idc = i_ref_idc; //
    nal->i_type    = i_type;    //5bit

    nal->i_payload= 0;
    nal->p_payload= &out->p_bitstream[bs_pos( &out->bs ) / 8];
}

void bs::f265_nal_end( bs_out *out )
{
    nal_t *nal = &out->nal[out->i_nal];
    nal->i_payload = &out->p_bitstream[bs_pos( &out->bs ) / 8] - nal->p_payload;
    out->i_nal++;
}

void bs::f265_vps_write( bs_t *s, vps_t *vps) {

	bs_write ( s, 4, vps->video_parameter_set_id );
	bs_write ( s, 2, vps->vps_reserved_three_2bits );
	bs_write ( s, 6, vps->vps_reserved_zero_6bits );
	bs_write ( s, 3, vps->vps_max_sub_layers_minus1 );
	bs_write1( s, vps->vps_temporal_id_nesting_flag );
	bs_write ( s, 16, vps->vps_reserved_ffff_16bits );

	//code ptl
	bs_write ( s, 2, vps->ptl.profile_space );
	bs_write1( s, vps->ptl.tier_flag );
	bs_write ( s, 5, vps->ptl.profile_idc );
	bs_write1( s, vps->ptl.profile_compatibility_flag );
	for(int i=0; i<31; i++)
		bs_write1( s, !(vps->ptl.profile_compatibility_flag) );
	bs_write1( s, vps->ptl.general_progressive_source_flag);      //0
	bs_write1( s, vps->ptl.general_interlaced_source_flag );	  //0
	bs_write1( s, vps->ptl.general_non_packed_constraint_flag );  //0
	bs_write1( s, vps->ptl.general_frame_only_constraint_flag );  //0
	bs_write( s, 44, 0 );    //XXX_reserved_zero_44bits
	bs_write ( s, 8, vps->ptl.general_level_idc );
	for(int i=0; i<vps->vps_max_sub_layers_minus1; i++)
	{
		bs_write1( s, vps->ptl.sub_layer_profile_present_flag );
		bs_write1( s, vps->ptl.sub_layer_level_present_flag );
	}
	if(vps->vps_max_sub_layers_minus1>0)
	{
		for(int i=vps->vps_max_sub_layers_minus1; i<8; i++)
			bs_write( s, 2, 0 );
	}
	for(int i=0; vps->vps_max_sub_layers_minus1; i++)
	{
		//codeProfileTier
	}

	/*//bit rate and pic rate info
	for(int i=0; i<=vps->vps_max_sub_layers_minus1; i++)
	{
		bs_write1( s, uint32_t(vps->bit_rate_info_present_flag[0]) );
		bs_write1( s, uint32_t(vps->pic_rate_info_present_flag[0]) );
		if(vps->bit_rate_info_present_flag[0]) {
			bs_write( s, 16, uint32_t(vps->avg_bit_rate) );
			bs_write( s, 16, uint32_t(vps->max_bit_rate) );
		}
		if(vps->pic_rate_info_present_flag[0]) {
			bs_write( s, 2,  uint32_t(vps->constant_pic_rate_idc) );
			bs_write( s, 16, uint32_t(vps->avg_pic_rate) );
		}
	}
	*/

	bs_write1( s, vps->vps_sub_layer_ordering_info_present_flag );
	for(int i=0; i<=vps->vps_max_sub_layers_minus1; i++)
	{
		bs_uvlc( s, vps->vps_max_dec_pic_buffering[0] );
		bs_uvlc( s, vps->vps_num_reorder_pics[0] );
		bs_uvlc( s, vps->vps_max_latency_increase[0] );
	}
	//bs_uvlc( s, vps->vps_num_hrd_parameters );
	bs_write( s, 6, vps->vps_max_nuh_reserved_zero_layer_id );
	bs_uvlc( s, vps->vps_max_op_sets_minus1 );         //0
	for(int op_idx=1; op_idx<vps->vps_max_op_sets_minus1; op_idx++)
	{
		for(int i=0; i<=vps->vps_max_nuh_reserved_zero_layer_id; i++ )
		{
			bs_write1( s, vps->op_layer_id_included_flag[op_idx][i] );
		}
	}

	bs_write1( s, vps->vps_timing_info_present_flag );    //false
	if(vps->vps_timing_info_present_flag)
	{
		bs_write( s, 32, vps->vps_num_units_in_tick );    //1001
		bs_write( s, 32, vps->vps_time_scale );           //60000
		bs_write1( s, vps->vps_poc_proportional_to_timing_flag );   //false
		if(vps->vps_poc_proportional_to_timing_flag)
			bs_uvlc( s, vps->vps_num_ticks_poc_diff_one_minus1 );   //0
		bs_uvlc( s, vps->vps_num_hrd_parameters );
		for(int i=0; i<vps->vps_num_hrd_parameters; i++)
		{
			//codeHrdParameters
		}
	}

	bs_write1( s, vps->vps_extension_flag );

	//rbsp trailing bits
	bs_write ( s, 1, 1 );
	bs_align_0( s );
}


void bs::f265_sps_write( bs_t *s ,sps_t *sps ){

	bs_write ( s, 4, sps->video_parameter_set_id );
	bs_write ( s, 3, sps->sps_max_sub_layers_minus1 );
	bs_write1( s, sps->sps_temporal_id_nesting_flag );

	//code ptl
	bs_write ( s, 2, sps->ptl.profile_space);
	bs_write1( s, sps->ptl.tier_flag );
	bs_write ( s, 5, sps->ptl.profile_idc );
	bs_write1( s, sps->ptl.profile_compatibility_flag );
	for(int i=0; i<31; i++)
		bs_write1( s, !(sps->ptl.profile_compatibility_flag) );
	bs_write1( s, sps->ptl.general_progressive_source_flag    );   //0
	bs_write1( s, sps->ptl.general_interlaced_source_flag     );   //0
	bs_write1( s, sps->ptl.general_non_packed_constraint_flag );   //0
	bs_write1( s, sps->ptl.general_frame_only_constraint_flag );   //0

	bs_write ( s, 44, sps->ptl.reserved_zero_44bits );
	bs_write ( s, 8, sps->ptl.general_level_idc );
	for(int i=0; i<sps->sps_max_sub_layers_minus1; i++)
	{
		bs_write1( s, sps->ptl.sub_layer_profile_present_flag );
		bs_write1( s, sps->ptl.sub_layer_level_present_flag );
		if(sps->sps_max_sub_layers_minus1>0)
		{
			for(int i=sps->sps_max_sub_layers_minus1; i<8; i++)
				bs_write( s, 2, 0 );
		}
		for(int i=0; i<sps->sps_max_sub_layers_minus1; i++)
		{
			//codeProfileTier
		}
	}

	bs_uvlc( s, sps->seq_parameter_set_id );
	bs_uvlc( s, sps->chroma_format_idc );
	if(sps->chroma_format_idc==3)
		bs_write1( s, sps->separate_colour_plane_flag );
	bs_uvlc( s, sps->pic_width_in_luma_samples );
	bs_uvlc( s, sps->pic_height_in_luma_samples );

	//crop
	bs_write1( s, sps->pic_cropping_flag );
	if(sps->pic_cropping_flag)
	{
		bs_uvlc( s, sps->crop.pic_crop_left_offset );
		bs_uvlc( s, sps->crop.pic_crop_right_offset );
		bs_uvlc( s, sps->crop.pic_crop_top_offset );
		bs_uvlc( s, sps->crop.pic_crop_bottom_offset );
	}

	bs_uvlc( s, sps->bit_depth_luma_minus8 );
	bs_uvlc( s, sps->bit_depth_chroma_minus8 );
	bs_uvlc( s, sps->log2_max_pic_order_cnt_lsb_minus4 );
	bs_write1( s, sps->sps_sub_layer_ordering_info_present_flag );
	for(int i=0; i<=sps->sps_max_sub_layers_minus1; i++)
	{
		bs_uvlc( s, sps->max_dec_pic_buffering[0] );
		bs_uvlc( s, sps->num_reorder_pics[0] );
		bs_uvlc( s, sps->max_latency_increase[0] );
	}
	bs_uvlc( s, sps->log2_min_coding_block_size_minus3 );
	bs_uvlc( s, sps->log2_diff_max_min_coding_block_size );
	bs_uvlc( s, sps->log2_min_transform_block_size_minus2 );
	bs_uvlc( s, sps->log2_diff_max_min_transform_block_size );
	bs_uvlc( s, sps->max_transform_hierarchy_depth_inter );
	bs_uvlc( s, sps->max_transform_hierarchy_depth_intra );
	bs_write1( s, sps->scaling_list_enabled_flag );
	if(sps->scaling_list_enabled_flag)
	{
		bs_write1( s, sps->sps_scaling_list_data_present_flag );
		if(sps->sps_scaling_list_data_present_flag)
		{
			//coding scaling list
		}
	}
	bs_write1( s, sps->asymmetric_motion_partitions_enabled_flag );
	bs_write1( s, sps->sample_adaptive_offset_enabled_flag );
	bs_write1( s, sps->pcm_enabled_flag );
	if(sps->pcm_enabled_flag)
	{
		bs_write( s, 4, sps->pcm_bit_depth_luma_minus1 );
		bs_write( s, 4, sps->pcm_bit_depth_chroma_minus1 );
		bs_uvlc ( s, sps->log2_min_pcm_luma_coding_block_size_minus3 );
		bs_uvlc ( s, sps->log2_diff_max_min_pcm_luma_coding_block_size );
		bs_write1( s, sps->pcm_loop_filter_disable_flag);
	}
	//short term ref pic set
	bs_uvlc( s, sps->num_short_term_ref_pic_sets );
	for(int i=0; i<sps->num_short_term_ref_pic_sets; i++)
	{
		if(i>0)
			bs_write1( s, sps->inter_ref_pic_set_prediction_flag );
		if(sps->inter_ref_pic_set_prediction_flag)
		{
			int delta_rps = 0;
			bs_write( s, 1, (delta_rps>=0 ? 0 : 1) );
			bs_uvlc	( s, abs(delta_rps)-1 );
			//
		}
		else
		{
			bs_uvlc( s, sps->num_negative_pics );
			bs_uvlc( s, sps->num_positive_pics );
			int prev = 0;
			for(int i=0; i<sps->num_negative_pics; i++)
			{
				bs_uvlc( s, prev-(sps->delta_poc_s0_minus1)-1 );
				prev = sps->delta_poc_s0_minus1;
				bs_write1( s, sps->used_by_curr_pic_s0_flag );
			}
			prev = 0;
			for(int i=sps->num_negative_pics; i<(sps->num_negative_pics+sps->num_positive_pics); i++)
			{
				bs_uvlc( s, sps->delta_poc_s1_minus1-prev-1 );
				prev = sps->delta_poc_s1_minus1;
				bs_write1( s, sps->used_by_curr_pic_s1_flag );
			}
		}
	}

	bs_write1( s, sps->long_term_ref_pics_present_flag ? 1 :0 );
	if(sps->long_term_ref_pics_present_flag)
	{
		bs_uvlc( s, sps->num_long_term_ref_pic_sps );
		for(int i=0; i<sps->num_long_term_ref_pic_sps; i++)
		{
			//bs_write( s, sps->lt_ref_pic_poc_lsb_sps );
			//	
		}
	}
	bs_write1( s, sps->sps_temporal_mvp_enable_flag );
	bs_write1( s, sps->sps_strong_intra_smoothing_enable_flag );
	bs_write1( s, sps->vui_parameters_present_flag );

	//vui
	if(sps->vui_parameters_present_flag)
	{
		bs_write1( s, sps->vui.aspect_ratio_info_present_flag );
		if(sps->vui.aspect_ratio_info_present_flag)
		{
			bs_write( s, 8, sps->vui.aspect_ratio_idc );
			if(sps->vui.aspect_ratio_idc==255)
			{
				bs_write( s, 16, sps->vui.sar_width );
				bs_write( s, 16, sps->vui.sar_height );
			}
		}

		bs_write1( s, sps->vui.overscan_info_present_flag );
		if(sps->vui.overscan_info_present_flag)
			bs_write1( s, sps->vui.overscan_appropriate_flag );

		bs_write1( s, sps->vui.video_signal_type_present_flag );
		if(sps->vui.video_signal_type_present_flag)
		{
			bs_write ( s, 3, sps->vui.video_format );
			bs_write1( s, sps->vui.video_full_range_flag );
			bs_write1( s, sps->vui.colour_description_present_flag );
			if(sps->vui.colour_description_present_flag)
			{
				bs_write( s, 8, sps->vui.colour_primaries );
				bs_write( s, 8, sps->vui.transfer_characteristics );
				bs_write( s, 8, sps->vui.matrix_coefficients );
			}
		}

		bs_write1( s, sps->vui.chroma_loc_info_present_flag );
		if(sps->vui.chroma_loc_info_present_flag)
		{
			bs_uvlc( s, sps->vui.chroma_sample_loc_type_top_field );
			bs_uvlc( s, sps->vui.chroma_sample_loc_type_bottom_field );
		}

		bs_write1( s, sps->vui.neutral_chroma_indication_flag );
		bs_write1( s, sps->vui.field_seq_flag );
		bs_write1( s, sps->vui.default_display_window_flag );
		bs_write1( s, sps->vui.pic_struct_present_flag );

		bs_write1( s, sps->vui.hrd_parameters_present_flag );
		if(sps->vui.hrd_parameters_present_flag)
		{
			bs_write1( s, sps->vui.timing_info_present_flag );
			if(sps->vui.timing_info_present_flag)
			{
				bs_write( s, 32, sps->vui.num_units_in_tick );
				bs_write( s, 32, sps->vui.time_scale );
			}
			bs_write1( s, sps->vui.nal_hrd_parameters_present_flag );
			bs_write1( s, sps->vui.vcl_hrd_parameters_present_flag );
			if(sps->vui.nal_hrd_parameters_present_flag || sps->vui.vcl_hrd_parameters_present_flag)
			{
				bs_write1( s, sps->vui.sub_pic_cpb_params_present_flag );
				if(sps->vui.sub_pic_cpb_params_present_flag)
				{
					bs_write( s, 8, sps->vui.tick_divisor_minus2 );
					bs_write( s, 5, sps->vui.du_cpb_removal_delay_length_minus1 );				
				}
				bs_write( s, 4, sps->vui.bit_rate_scale );
				bs_write( s, 4, sps->vui.cpb_size_scale );
				if(sps->vui.cpb_size_scale)
					bs_write( s, 4, sps->vui.du_cpb_size_scale );
				bs_write( s, 5, sps->vui.initial_cpb_removal_delay_length_minus1 );
				bs_write( s, 5, sps->vui.cpb_removal_delay_length_minus1 );
				bs_write( s, 5, sps->vui.dpb_output_delay_length_minus1 );
			}

			int nal_or_vcl;
			for(int i=0; i<sps->sps_max_sub_layers_minus1+1; i++)
			{
				bs_write1( s, sps->vui.fixed_pic_rate_flag );
				if(sps->vui.fixed_pic_rate_flag)
					bs_uvlc( s, sps->vui.pic_duration_in_tc_minus1 );
				bs_write1( s, sps->vui.low_delay_hrd_flag );
				bs_uvlc( s, sps->vui.cpb_cnt_minus1 );
				for(nal_or_vcl=0; nal_or_vcl<2; nal_or_vcl++)
				{
					if(((nal_or_vcl==0) && (sps->vui.nal_hrd_parameters_present_flag)) || ((nal_or_vcl==1) && (sps->vui.vcl_hrd_parameters_present_flag)))
					{
						for(i=0; i<sps->vui.cpb_cnt_minus1; i++)
						{
							bs_uvlc( s, sps->vui.bit_size_value_minus1 );
							bs_uvlc( s, sps->vui.cpb_size_value_minus1 );
							if(sps->vui.sub_pic_cpb_params_present_flag)
								bs_uvlc( s, sps->vui.du_cpb_size_value_minus1 );
							bs_write1( s, sps->vui.cbr_flag );
						}
					}
				}
			}
		}
	}

	bs_write1( s, sps->sps_extension_flag );
	
	//rbsp_trailing_bits
	bs_write( s, 1, 1 );
	bs_align_0( s );

}

void bs::f265_pps_write( bs_t *s,pps_t *pps )
{
	bs_uvlc  ( s, pps->pic_parameter_set_id );
	bs_uvlc  ( s, pps->seq_parameter_set_id );
	bs_write1( s, pps->dependent_slice_enabled_flag );
	bs_write1( s, pps->output_flag_present_flag );
	bs_write ( s, 3, pps->num_extra_slice_header_bits );
	bs_write1( s, pps->sign_data_hiding_flag );
	bs_write1( s, pps->cabac_init_present_flag );
	bs_uvlc  ( s, pps->num_ref_idx_l0_default_active_minus1 );
	bs_uvlc  ( s, pps->num_ref_idx_l1_default_active_minus1 );
	bs_svlc  ( s, pps->pic_init_qp_minus26 );
	bs_write1( s, pps->constrained_intra_pred_flag ? 1 : 0 );
	bs_write1( s, pps->transform_skip_enabled_flag ? 1 : 0 );
	bs_write1( s, pps->cu_qp_delta_enabled_flag ? 1 : 0 );
	if(pps->cu_qp_delta_enabled_flag)
		bs_uvlc( s, pps->diff_cu_qp_delta_depth );
	bs_svlc ( s, pps->cb_qp_offset );
	bs_svlc ( s, pps->cr_qp_offset );
	bs_write1( s, pps->slicelevel_chroma_qp_flag ? 1 : 0 );
	bs_write1( s, pps->weighted_pred_flag ? 1 : 0 );
	bs_write1( s, pps->weighted_bipred_flag ? 1 : 0 );
	bs_write1( s, pps->transquant_bypass_enable_flag ? 1 : 0 );

	bs_write1( s, pps->tiles_enabled_flag ? 1 : 0 );
	bs_write1( s, pps->entropy_coding_sync_enabled_flag ? 1 : 0 );
	if(pps->tiles_enabled_flag)
	{
		bs_uvlc( s, pps->num_tile_columns_minus1 );
		bs_uvlc( s, pps->num_tile_rows_minus1 );
		bs_write1( s, pps->uniform_spacing_flag );
		if(pps->uniform_spacing_flag==0)
		{
			for(int i=0; i<pps->num_tile_columns_minus1; i++)
				bs_uvlc( s, pps->column_width_minus1 );
			for(int i=0; i<pps->num_tile_rows_minus1; i++)
				bs_uvlc( s, pps->row_height_minus1 );
		}
		if((pps->num_tile_columns_minus1!=0) ||(pps->num_tile_rows_minus1!=0))
			bs_write1( s, pps->loop_filter_across_tiles_enabled_flag ? 1 : 0 );
	}

	bs_write1( s, pps->loop_filter_across_slices_enabled_flag ? 1 : 0 );
	bs_write1( s, pps->deblocking_filter_control_present_flag ? 1 : 0 );
	if(pps->deblocking_filter_control_present_flag)
	{
		bs_write1( s, pps->deblocking_filter_override_enabled_flag ? 1 : 0 );
		bs_write1( s, pps->pic_disable_deblocking_filter_flag );
		if(!pps->pic_disable_deblocking_filter_flag)
		{
			bs_svlc( s, pps->pps_beta_offset_div2 );
			bs_svlc( s, pps->pps_tc_offset_div2 );
		}	
	}

	//scaling list
	bs_write1( s, pps->pps_scaling_list_data_present_flag ? 1 : 0 );           
	if(pps->pps_scaling_list_data_present_flag)
	{
		for(int size_id=0; size_id<4; size_id++)
		{
			for(int list_id=0; list_id<4; list_id++)
			{
				bs_write1( s, pps->scaling_list_pred_mode_flag );            //
			}
			if(!pps->scaling_list_pred_mode_flag)
				bs_uvlc( s, pps->scaling_list_pred_matrix_id_delta );        //
			else 
			{
				//xCodeScalingList
			}
		}
	}

	bs_write1( s, pps->lists_modification_present_flag );
	bs_uvlc	 ( s, pps->log2_parallel_merge_level_minus2 );
	bs_write1( s, pps->slice_header_extension_present_flag );
	bs_write1( s, pps->pps_extension_flag );

	//rbsp trailing bits
	bs_write( s, 1, 1 );
	bs_align_0( s );
}

void bs::f265_slice_header_write( bs_t *s, slice_header_t *sh, int i_nal_ref_idc, int frame_num )
{
	bs_write1( s, sh->first_slice_in_pic_flag );
	if(param.frame_num==0)
		bs_write1( s, sh->no_output_of_prior_pics_flag );
	bs_uvlc( s, sh->pic_parameter_set_id );

	int address = 0;
	int slice_address = 0;
	if(sh->pps->dependent_slice_enabled_flag && (address!=0))
		bs_write1( s, sh->dependent_slice_flag );
	if(address>0)
		bs_write( s, address, 5 );

	if(!sh->pps->dependent_slice_enabled_flag)
	{
		for(int i=0; i<sh->pps->num_extra_slice_header_bits; i++)
			bs_write1( s, 0 );                //slice_reserved_undetermined_flag
		bs_uvlc( s, sh->slice_type );         //slice_type
		if(sh->pps->output_flag_present_flag)
			bs_write1( s, sh->pic_output_flag ? 1 : 0 );

		bool idr_pic_flag = false;
		if(param.frame_num==0)
			idr_pic_flag = true;
		if(!idr_pic_flag)
		{
			bs_write( s, (sh->sps->log2_max_pic_order_cnt_lsb_minus4+4), ((param.frame_num+0)%(1<<(sh->sps->log2_max_pic_order_cnt_lsb_minus4+4))) );           //pic_order_cnt_lsb
			int getrpc = 0;		
			if(getrpc<0)
			{
				bs_write1( s, sh->short_term_ref_pic_set_sps_flag );
				//codeShortTermRefPicSet
			}
			else
			{
				bs_write1( s, !sh->short_term_ref_pic_set_sps_flag );
			}
			if(sh->sps->long_term_ref_pics_present_flag)
			{
				//long term pic
			}
			if(sh->sps->sps_temporal_mvp_enable_flag)
				bs_write1( s, sh->enable_temporal_mvp_flag );
		}

		
		if(sh->sps->sample_adaptive_offset_enabled_flag)
		{
			bs_write1( s, sh->slice_sao_luma_flag );
			bs_write1( s, sh->slice_sao_chroma_flag );
		}

		if(!(sh->slice_type==SLICE_TYPE_I))
		{
			bs_write1( s, sh->num_ref_idx_active_override_flag );
			if(sh->num_ref_idx_active_override_flag)
			{
				bs_uvlc( s, sh->num_ref_idx_l0_active_minus1 );
				if(sh->slice_type==SLICE_TYPE_B)
					bs_uvlc( s, sh->num_ref_idx_l1_active_minus1 );
			}
		}

		if(sh->pps->lists_modification_present_flag && (0>1))
		{
			if(!(sh->slice_type==SLICE_TYPE_I))
			{
				bs_write1( s, sh->ref_pic_list_modification_flag_l0 ? 1 : 0 );
				if(sh->ref_pic_list_modification_flag_l0)    //false
				{
					//
				}			
			}

			if(sh->slice_type==SLICE_TYPE_B)
			{
				bs_write1( s, sh->ref_pic_list_modification_flag_l1 );
				if(sh->ref_pic_list_modification_flag_l1)
				{
					//
				}
			}
		}
		if(sh->slice_type==SLICE_TYPE_B)
			bs_write1( s, sh->mvd_l1_zero_flag );

		if(!(sh->slice_type==SLICE_TYPE_I))
		{
			if(sh->pps->cabac_init_present_flag)
			{
				int enc_cabac_table_idx = param.type;
				sh->cabac_init_flag = (param.type!=enc_cabac_table_idx && enc_cabac_table_idx!=SLICE_TYPE_I) ? true : false;
				bs_write1( s, sh->cabac_init_flag ? 1 : 0 );
			}
		}

		bool tmvp_flag_enable = true;
		int num_ref_idx0 = 0;
		int num_ref_idx1 = 0;
		if(sh->enable_temporal_mvp_flag)
		{
			if(sh->slice_type==SLICE_TYPE_B)
				bs_write1( s, sh->collocated_from_l0_flag );
			if(sh->slice_type!=SLICE_TYPE_I && ( (sh->collocated_from_l0_flag==1 && num_ref_idx0>1) || (sh->collocated_from_l0_flag==0 && num_ref_idx1>1)))
				bs_uvlc( s, sh->collocated_ref_idx );
		}
		if((sh->pps->weighted_pred_flag && sh->slice_type==SLICE_TYPE_P) || (sh->pps->weighted_bipred_flag && sh->slice_type==SLICE_TYPE_B))
		{
			//xCodePredHeight
		}
		if(!(sh->slice_type==SLICE_TYPE_I))
			bs_uvlc( s, sh->five_minus_max_num_merge_cand );
		
		//qp
		int i_code = param.qp - (sh->pps->pic_init_qp_minus26 + 26);
		bs_svlc( s, i_code );
		if(sh->pps->slicelevel_chroma_qp_flag)
		{
			//cr,cb qp_delta
			bs_svlc( s, sh->slice_qp_delta_cb );
			bs_svlc( s, sh->slice_qp_delta_cr );
		}

		if(sh->pps->deblocking_filter_control_present_flag)
		{
			if(sh->pps->deblocking_filter_override_enabled_flag)
				bs_write1( s, sh->deblocking_filter_override_flag );
			if(sh->deblocking_filter_override_flag)
			{
				bs_write1( s, sh->slice_disable_deblocking_filter_flag );
				if(!sh->slice_disable_deblocking_filter_flag)
				{
					bs_svlc( s, sh->beta_offset_div2 );
					bs_svlc( s, sh->tc_offset_div2 );
				}
			}
		}

		bool sao_enable = (!sh->sps->sample_adaptive_offset_enabled_flag) ? false : (sh->slice_sao_luma_flag || sh->slice_sao_chroma_flag);
		bool dbf_enable = (!sh->slice_disable_deblocking_filter_flag);
		if(sh->pps->loop_filter_across_slices_enabled_flag && (sao_enable || dbf_enable))
			bs_write1( s, sh->slice_loop_filter_across_slices_enabled_flag );
		
	}
	if(sh->pps->slice_header_extension_present_flag)
		bs_uvlc( s, sh->slice_header_extension_length );
	bs_align_1( s );
}

void bs::f265_slice_data_write( bs_t *s, bs_t *ec) {

	if( true ) {
		if(s->i_left == 32)
			s->cur_bits = 0;
		if (ec->p-ec->p_start != 0) {
			for (int i=0; i<(ec->p-ec->p_start); i++)
				bs_write(s, 8, *(ec->p_start+i));
		}

		if(s->i_left != WORD_SIZE*8) {
			if(s->i_left == 8){
				*(s->p) = s->cur_bits >> 16;
				*(s->p+1) = (s->cur_bits & 0x00ff00) >> 8;
				*(s->p+2) = (s->cur_bits & 0x0000ff);
			}	
			else if(s->i_left == 16) {
				*(s->p) = s->cur_bits >> 8;
				*(s->p+1) = s->cur_bits & 0x0ff;
			}
			else if(s->i_left == 24) 
				*(s->p) = s->cur_bits;
		}	
	}
	else {
		if (ec->p-ec->p_start != 0) {
			for (int i=0; i<(ec->p-ec->p_start); i++)
				bs_write(s, 8, *(ec->p_start+i));
		}
	
		if (ec->i_left != WORD_SIZE*8)
			bs_write(s, WORD_SIZE*8-ec->i_left, ec->cur_bits);
		}
}

void bs::f265_nal_encode( void *p_data, int *pi_data, int b_annexeb, nal_t *nal)
{
	uint8_t *dst = (uint8_t*)p_data;//
	uint8_t *src = nal->p_payload;
	uint8_t *end = &nal->p_payload[nal->i_payload];
	int i_count = 0;

	/* FIXME this code doesn't check overflow */

	if( b_annexeb )
	{
		/* long nal start code (we always use long ones)*/
		if(nal->i_type==NAL_UNIT_CODED_SLICE_IDR)
		{
			*dst++ = 0x00;
			*dst++ = 0x00;
			*dst++ = 0x01;
		}
		else
		{
			*dst++ = 0x00;
			*dst++ = 0x00;
			*dst++ = 0x00;
			*dst++ = 0x01;
		}

	}

	//nal_unit_header
	*dst++ = (0x00 << 7) | (nal->i_type << 1);
	*dst++ = 1;

	while( src < end )
	{
		if( i_count == 2 && *src <= 0x03 )
		{
			*dst++ = 0x03;
			i_count = 0;
		}
		if( *src == 0 )
			i_count++;
		else
			i_count = 0;
		*dst++ = *src++;
	}
	*pi_data = dst - (uint8_t*)p_data;
}


void bs::f265_nal_encodeN03( void *p_data, int *pi_data, int b_annexeb, nal_t *nal )
{
	uint8_t *dst = (uint8_t*)p_data;//
	uint8_t *src = nal->p_payload;
	uint8_t *end = &nal->p_payload[nal->i_payload];
	int i_count = 0;

	/* FIXME this code doesn't check overflow */

	if( b_annexeb )
	{
		/* long nal start code (we always use long ones)*/
		*dst++ = 0x00;
		*dst++ = 0x00;
		*dst++ = 0x00;
		*dst++ = 0x01;
	}

	/* nal header */
	*dst++ = ( 0x00 << 7 ) | ( nal->i_ref_idc << 5 ) | nal->i_type;

	while( src < end )
	{
		if( i_count == 2 && *src <= 0x03 )
		{
		//	*dst++ = 0x03; // only for test, should add it for working
			i_count = 0;
		}
		if( *src == 0 )
			i_count++;
		else
			i_count = 0;
		*dst++ = *src++;
	}
	*pi_data = dst - (uint8_t*)p_data;
}

/****************************************************************************
 *						bs non-member func
 ****************************************************************************/
void bs_init( bs_t *s, void *p_data, int i_data )
{
    int offset = ((intptr_t)p_data & (WORD_SIZE-1));
    s->p       = s->p_start = (uint8_t*)p_data - offset;
    s->p_end   = (uint8_t*)p_data + i_data;
    s->i_left  = offset ? 8*offset : (WORD_SIZE*8);
    s->cur_bits = endian_fix( *(intptr_t*)s->p );
}

void bs_write( bs_t *s, int i_count, uint32_t i_bits )
{
    if( i_count < s->i_left )
    {
        s->cur_bits = (s->cur_bits << i_count) | i_bits;
        s->i_left -= i_count;
    }
    else
    {
		uint32_t i_bits_cache = 0;        //added for CABAC coding
		if(i_count == s->i_left)
			i_bits_cache = 0;
		else
			i_bits_cache = ( i_bits << (32-(i_count-s->i_left)) ) >> (32-(i_count-s->i_left));
        i_count -= s->i_left;
        s->cur_bits = (s->cur_bits << s->i_left) | (i_bits >> i_count);
        *(uint32_t*)s->p = endian_fix( s->cur_bits );
        s->p += 4;
		if( true )
	        s->cur_bits = i_bits_cache;
		else 
			s->cur_bits = i_bits;
        s->i_left = 32 - i_count;
    }
}

void bs_write1( bs_t *s, uint32_t i_bit )
{
    s->cur_bits <<= 1;
    s->cur_bits |= i_bit;
    s->i_left--;
    if( s->i_left == WORD_SIZE*8-32 )
    {
        *(uint32_t*)s->p = endian_fix32( s->cur_bits );
        s->p += 4;
        s->i_left = WORD_SIZE*8;
    }
}

void bs_write32( bs_t *s, uint32_t i_bits )
{
    bs_write( s, 16, i_bits >> 16 );
    bs_write( s, 16, i_bits );
}

void bs_write_ue( bs_t *s, int val )
{
    bs_write( s, f264_ue_size_tab[val+1], val+1 );
}

void bs_write_se( bs_t *s, int val )
{
    int size = 0;
    int tmp = val = val <= 0 ? -val*2+1 : val*2;
    if( tmp >= 0x100 )
    {
        size = 16;
        tmp >>= 8;
    }
    size += f264_ue_size_tab[tmp];
    bs_write( s, size, val );
}

int bs_pos( bs_t *s )
{
    return( 8 * (s->p - s->p_start) + (WORD_SIZE*8) - s->i_left );
}

void bs_flush( bs_t *s )
{
    *(intptr_t*)s->p = endian_fix( s->cur_bits << s->i_left );
	s->p += WORD_SIZE - s->i_left / 8;
    s->i_left = WORD_SIZE*8;
}

void bs_rbsp_trailing( bs_t *s )
{
	bs_write1( s, 1 );
    bs_flush( s );
}

void bs_align_1( bs_t *s )
{
    if( s->i_left&7 )
    {
        s->cur_bits <<= s->i_left&7;
        s->cur_bits |= (1 << (s->i_left&7)) - 1;
        s->i_left &= ~7;
    }
    bs_flush( s );
}

void bs_align_0( bs_t *s )
{
	if( s->i_left&7 )
	{
		s->cur_bits <<= s->i_left&7;
		s->i_left &= ~7;
	}
	bs_flush( s );
}

void bs_uvlc( bs_t *s, uint32_t val )
{
	uint32_t length = 1;
	uint32_t temp = ++val;
	
	while( 1 != temp )
	{
	  temp >>= 1;
	  length += 2;
	}

	bs_write( s, length >> 1, 0);
	bs_write( s, (length+1) >> 1, val);
}

void bs_svlc( bs_t *s, int val )
{
	uint32_t u_val;
	u_val = (val<=0) ? (-val<<1) : ((val<<1)-1);
	bs_uvlc(s, u_val);

}