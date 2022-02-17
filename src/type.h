#ifndef _TYPE_H
#define	_TYPE_H

#include <string>
#include "config.h"
using std::string;

typedef signed char			int8_t;
typedef unsigned char		uint8_t;
typedef short				int16_t;
typedef unsigned short		uint16_t;
typedef int					int32_t;
typedef unsigned			uint32_t;
typedef long long			int64_t;
typedef unsigned long long	uint64_t;

#if BIT_DEPTH>8
typedef   unsigned short   PIXEL;
#else
typedef   unsigned char   PIXEL;
#endif

/********************************************************************************
*                            Normal Type Def.
*********************************************************************************/
/* bitstream*/
struct bs_t {
    uint8_t *p_start;   //buffer start address
    uint8_t *p;         //current address
    uint8_t *p_end;     //buffer end address
    intptr_t cur_bits;  //current stored bits (32-bit length buf)
    int     i_left;     //available bits in cur_bits
    int     i_bits_encoded; /* RD only */
};

/********************************************************************************
*                            SystemC Type Def.
*********************************************************************************/
using namespace std;
/*************************************
                 ec_t
*************************************/
struct ec_t {
//	string name;
	int x;
	int y;

	bs_t	bs_buf_pt;
	uint8_t bs_buf[BS_BUF_SIZE];

	ec_t& operator= (const ec_t& arg){
//		name=arg.name;
		x = arg.x;
		y = arg.y;
		memcpy(bs_buf, arg.bs_buf, sizeof(bs_buf));
		bs_buf_pt.p_start = bs_buf;
		bs_buf_pt.p_end   = bs_buf+BS_BUF_SIZE;
		bs_buf_pt.p       = bs_buf+(arg.bs_buf_pt.p-arg.bs_buf_pt.p_start);
		bs_buf_pt.cur_bits = arg.bs_buf_pt.cur_bits;
		bs_buf_pt.i_left   = arg.bs_buf_pt.i_left;
		bs_buf_pt.i_bits_encoded = arg.bs_buf_pt.i_bits_encoded;
		return *this;
	}

};

/*************************************
                 param_t
*************************************/
struct param_t {	
	string name;
	string file_name;
	int type;
	int frame_num;
	int qp;
	int sr_w;
	int sr_h;
	int gop_length;
	int frame_width;
	int frame_height;
	int frame_total;
	int frame_mb_x_total;
	int frame_mb_y_total;
	param_t& operator= (const param_t& arg){
		name=arg.name;
		file_name  = arg.file_name;
		type       = arg.type;
		frame_num  = arg.frame_num;
		qp         = arg.qp;
		sr_w       = arg.sr_w;
		sr_h       = arg.sr_h;
		gop_length      = arg.gop_length;
		frame_width     = arg.frame_width;
		frame_height    = arg.frame_height;
		frame_total     = arg.frame_total;
		frame_mb_x_total= arg.frame_mb_x_total;
		frame_mb_y_total= arg.frame_mb_y_total;
		return *this;
	}
};

#endif	/* _TYPE_H */