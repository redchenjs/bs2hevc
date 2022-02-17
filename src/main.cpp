#include "type.h"
#include "bs.h"

void get_input_param(param_t* param,int argc,char *argv[]){
	param->name         =  "param"      ;
	param->file_name    = BS_FILE      ;
	param->type         = SLICE_TYPE_I  ;
	param->frame_num    =  0            ;
	param->qp           =  INIT_QP      ;
	param->gop_length   =  GOP_LENGTH   ;
	param->frame_width  =  FRAMEWIDTH   ;
	param->frame_height =  FRAMEHEIGHT  ;
	param->frame_total  =  FRAME_TOTAL  ;
	param->frame_mb_x_total = MB_X_TOTAL;
	param->frame_mb_y_total = MB_Y_TOTAL;

	param->sr_h = (SEARCH_RANGE / 2 - 4);
	param->sr_w = (SEARCH_RANGE / 2 - 4);

	bool help = 0;
	for( int i=1; i<argc; i++ ) {
		if ( !strcmp(argv[i], "-i") ) {			// check input YUV file exist
			param->file_name = argv[++i];
			if (!fopen(param->file_name.c_str(),"rb")){
				fprintf( stderr, "ERROR: File not find \n");
				exit(0);
			}
		}
		else if ( !strcmp(argv[i], "-w") ) {	// check frame width is valid
			param->frame_width = atoi(argv[++i]);
			if (!isdigit(*argv[i])){
				fprintf( stderr, "ERROR: -w [%s] Not a valid number \n", argv[i]);
				exit(0);
			}
			else {				
				param->frame_mb_x_total = ( (param->frame_width  +(f_LCU_SIZE-1)) / f_LCU_SIZE );				
			}			
		}
		else if ( !strcmp(argv[i], "-h") ) {	// check frame height is valid
			param->frame_height = atoi(argv[++i]);
			if (!isdigit(*argv[i])){
				fprintf( stderr, "ERROR: -h [%s] Not a valid number \n", argv[i]);
				exit(0);
			}
			else{				
				param->frame_mb_y_total = ( (param->frame_height +(f_LCU_SIZE-1)) / f_LCU_SIZE );
			}			
		}
		else if ( !strcmp(argv[i], "-g") ) {
			param->gop_length = atoi(argv[++i]);
			if (!isdigit(*argv[i])){
				fprintf( stderr, "ERROR: -g [%s] Not a valid number \n", argv[i]);
				exit(0);
			}				
		}
		else if ( !strcmp(argv[i], "-f") ) {
			param->frame_total = atoi(argv[++i]);
			if (!isdigit(*argv[i])){
				fprintf( stderr, "ERROR: -f [%s] Not a valid number \n", argv[i]);
				exit(0);
			}
		}
		else if ( !strcmp(argv[i], "-q") ) {
			param->qp = atoi(argv[++i]);
			if (!isdigit(*argv[i])){
				fprintf( stderr, "ERROR: -q [%s] Not a valid number \n", argv[i]);
				exit(0);
			}
		}
		else if(!strcmp(argv[i], "-help") ){
			help = 1;
		}
		else {
			fprintf( stderr, "Unknown option [%s], type [-h] for more information \n", argv[i] );
			exit(0);
		}
	}

	if ( help ) {
		fprintf( stderr, "Usage:\n\t%s [-i|-help] inYuvFile [options]\n", argv[0] );
		fprintf( stderr, "\nOptions:\n" );
		fprintf( stderr, "    -help       :Printf usage               \n");
		fprintf( stderr, "    -i          :Input file                 \n");
		fprintf( stderr, "    -w          :Width of Video             \n");
		fprintf( stderr, "    -h          :Height of Video            \n");
		fprintf( stderr, "    -g          :GOP Length                 \n");
		fprintf( stderr, "    -f          :Encode frames              \n");
		fprintf( stderr, "    -q          :Encode qp                  \n");
		fprintf( stderr, "\nExamples:\n");
		fprintf( stderr, "vs2010.exe -i bs_416x240_g5_f10.dat -w 416 -h 240 -g 5 -f 10 -q 22\n");
		exit(0);
	}

	if (!fopen(param->file_name.c_str(),"rb"))
		{fprintf( stderr, "ERROR: File not find                 \n"); exit(0);}
	if (param->frame_height<0 || param->frame_height>10000)
		{fprintf( stderr, "ERROR: -h [%4d] is not a valid number \n", param->frame_height); exit(0);}
	if (param->frame_width<0 || param->frame_width>10000)
		{fprintf( stderr, "ERROR: -w [%4d] is not a valid number \n", param->frame_width); exit(0);}
	if (param->gop_length<0)
		{fprintf( stderr, "ERROR: -g [%2d] is not a valid number \n", param->gop_length); exit(0);}
	if (param->frame_total<0)
		{fprintf( stderr, "ERROR: -f [%2d] is not a valid number \n", param->frame_total); exit(0);}
	if (param->qp<0 || param->qp>64)
		{fprintf( stderr, "ERROR: -q [%2d] is not a valid number \n", param->qp); exit(0);}

	fprintf( stderr, "    -i          :Input file                 [%s]\n" , (param->file_name).c_str());
	fprintf( stderr, "    -w          :Width of Video             [%4d], [%2d] CUs\n", param->frame_width,param->frame_mb_x_total);
	fprintf( stderr, "    -h          :Height of Video            [%4d], [%2d] CUs\n", param->frame_height,param->frame_mb_y_total);
	fprintf( stderr, "    -g          :GOP Length                 [%4d]\n", param->gop_length         );
	fprintf( stderr, "    -f          :Encode frames              [%4d]\n", param->frame_total        );
	fprintf( stderr, "    -q          :Encode qp                  [%4d]\n", param->qp                 );
}


int main(int argc,char *argv[]){
	param_t      header_param ;
	param_t				param ;

	bs			*u_bs=new bs;

	get_input_param(&param,argc,argv);
	u_bs->input_param=param;
	u_bs->bs_proc();

    return 0;
}