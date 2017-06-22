#include "Data_loadPNG.h"

Data Data_loadPNG(char *file){
	Data data;
	unsigned char header[8];
	int i,j,k;
	int width,height;
	double value,alpha,valmax;
	
	//int channels;
	png_byte color_type,bit_depth;
	png_byte *row;
	unsigned char temp;
	unsigned long temp2;
	
	png_structp png_ptr;
	png_infop info_ptr;
	
	int number_of_passes;
	png_bytep * row_pointers;
	png_size_t byteincr,rowbytes;
	
	FILE *fp = NULL;
	
	fp = fopen(file,"rb");
	if(!fp){
		fprintf(stderr,"Data_readPNG : File %s cannot be opened for reading\n",file);
		return NULL;
	}
	fread(header,1,8,fp);
	if(png_sig_cmp(header,0,8)){
		fprintf(stderr,"Data_readPNG : File %s is not recognized as a PNG file\n",file);
		fclose(fp);
		return NULL;
	}
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if(!png_ptr){
		fprintf(stderr,"Data_readPNG : png_create_read_struct failed\n");
		fclose(fp);
		return NULL;
	}
	
	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr){
		fprintf(stderr,"Data_readPNG : png_create_info_struct failed\n");
		fclose(fp);
		free(png_ptr);
		return NULL;
	}
	
	if(setjmp(png_jmpbuf(png_ptr))){
		fprintf(stderr,"Data_readPNG : Error during init_io\n");
		fclose(fp);
		free(png_ptr);
		free(info_ptr);
		return NULL;
	}
	
	png_init_io(png_ptr,fp);
	png_set_sig_bytes(png_ptr,8); /*8 bytes are read previously. so let libpng know that.*/
	png_read_info(png_ptr,info_ptr);
	
	width = png_get_image_width(png_ptr,info_ptr);
	height = png_get_image_height(png_ptr,info_ptr);
	color_type = png_get_color_type(png_ptr,info_ptr);
	bit_depth = png_get_bit_depth(png_ptr,info_ptr);
	
	valmax = (1<<bit_depth) - 1; /*8 bit -> 255 16 bit -> 65535*/
	//channels = png_get_channels(png_ptr,info_ptr);
	
	/*convert image to grayscale*/
	
	if(color_type == PNG_COLOR_TYPE_PALETTE){
		png_set_palette_to_rgb(png_ptr);
		png_set_rgb_to_gray_fixed(png_ptr,1,-1,-1);
	}else if(color_type == PNG_COLOR_TYPE_RGB 
	   || color_type == PNG_COLOR_TYPE_RGB_ALPHA){
		png_set_rgb_to_gray_fixed(png_ptr,1,-1,-1);
	}
	/*add alpha channel to treat all data equally*/
	png_set_add_alpha(png_ptr,0xffff,PNG_FILLER_AFTER);
	
	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr,info_ptr);
	
	
	width = png_get_image_width(png_ptr,info_ptr);
	height = png_get_image_height(png_ptr,info_ptr);
	bit_depth = png_get_bit_depth(png_ptr,info_ptr);
	
	//fprintf(stderr,"width : %d\n",width);
	//fprintf(stderr,"height : %d\n",height);
	//fprintf(stderr,"bit depth : %d\n",bit_depth);
	
	color_type = png_get_color_type(png_ptr,info_ptr);
	/*
	switch(color_type){
	  case PNG_COLOR_TYPE_PALETTE:
		fprintf(stderr,"palette\n"); break;
	  case PNG_COLOR_TYPE_RGB:
		fprintf(stderr,"rgb\n"); break;
	  case PNG_COLOR_TYPE_RGB_ALPHA:
		fprintf(stderr,"rgb alpha\n"); break;
	  case PNG_COLOR_TYPE_GRAY:
		fprintf(stderr,"gray\n"); break;
	  case PNG_COLOR_TYPE_GRAY_ALPHA:
		fprintf(stderr,"gray alpha\n"); break;
	  default:
		fprintf(stderr,"unknown color type\n"); break;
	}
	*/
	if(setjmp(png_jmpbuf(png_ptr))){
		fprintf(stderr,"Data_readPNG : Error during read_image\n");
		fclose(fp);
		free(png_ptr);
		free(info_ptr);
		return NULL;
	}
	
	row_pointers = (png_bytep*) allocate(sizeof(png_bytep)*height);
	rowbytes = png_get_rowbytes(png_ptr,info_ptr);
	for(i=0;i<height;i++){
		row_pointers[i] = (png_byte*) allocate(rowbytes);
	}
	png_read_image(png_ptr, row_pointers);
	fclose(fp);
	
	data = Data_create(height,width);
	byteincr = rowbytes/width/2;
	//fprintf(stderr,"byteincr : %zu\n",byteincr);
	
	for(i=0;i<height;i++){
		row = row_pointers[i];
		//printbit(row,512);
		for(j=0;j<width;j++){
			for(k=0,temp2 = 0;k<byteincr;k++,temp2 <<= 8){
				temp = row[k];
				temp2 |= temp;
			}
			//printbit(row,16); printf("\t%lu\n",temp2);
			value = (double)temp2;
			row += byteincr;
			
			for(k=0,temp2 = 0;k<byteincr;k++,temp2 <<= 8){
				temp = row[k];
				temp2 |= temp;
			}
			//printbit(row,16); printf("\t%lu\n",temp2);
			alpha = (double)temp2;
			row += byteincr;
			Data_set(data,height-1-i,j,value * (alpha/valmax));
		}
	}
	
	png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
	for(i=0;i<height;i++){
		deallocate(row_pointers[i]);
	}
	deallocate(row_pointers);
	
	return data;
}
