#include "Data_loadTIFF.h"

Data Data_loadTIFF(char *file){
	Data data = NULL;
	unsigned int i,j,w,h,*raster;
	unsigned char r,g,b,a;
	double red,green,blue,alpha;
	double gray;
	size_t npixels;
	
	TIFF *tif = TIFFOpen(file,"r");
	
	if(tif){
		TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&w);
		TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&h);
		data = Data_create(h,w);
		npixels = w*h;
		
		raster = (unsigned int*) _TIFFmalloc(npixels*sizeof(unsigned int));
		if(raster != NULL){
			if(TIFFReadRGBAImage(tif,w,h,raster,0)){
				for(i=0;i<h;i++){
					for(j=0;j<w;j++){
						r = TIFFGetR(raster[i*w+j]);
						g = TIFFGetG(raster[i*w+j]);
						b = TIFFGetB(raster[i*w+j]);
						a = TIFFGetA(raster[i*w+j]);
						
						red = (double)(r - 255)/255;
						green = (double)(g - 255)/255;
						blue = (double)(b - 255)/255;
						alpha = (double)a/255;
						/*https://en.wikipedia.org/wiki/Grayscale*/
						gray = (0.2126*red + 0.7152*green + 0.0722*blue)*alpha;
						Data_set(data,h-1-i,j,gray);
					}
				}
			}else{
				TIFFError("Data_loadTIFF : ","Error during TIFFReadRGBAImage\n");
			}
		}
	}else{
		TIFFError("Data_loadTIFF : ","File %s cannot be opened for reading\n",file);
	}
	return data;
}
