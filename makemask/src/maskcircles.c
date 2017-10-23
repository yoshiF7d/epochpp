#include <utilities.h>
#include <LinkedList.h>
#include <Data.h>
 
int main(int argc, char *argv[]){
	char *fileout;
	int i,j,invert = 0;
	int rows,columns,r0,c0,r02,c02,r,c,r2,c2;
	double k=1,sigma=0,radius=1,theta=0;
	Data mask;	

	if(argc < 7){
		printf(
			   "usage : %s [outputfile name] [rows] [columns] [k] [theta] [radius] ([invert : 0]) ([sigma])\n"
			   "r0 : row index of the elipse center\n"
			   "c0 : column index of the elipse center\n"
			   "k : distance from origin\n"
			   "theta : angle between the semi-major axis and horizontal line\n"
			   "radius : raius of circles" 
			   "invert : if this value is set to 1, then points inside circles are set to 1 and points outside circles are set to 0. If this value is set to 0, it becomes otherwise\n"
			   "sigma : if specified, circle becomes gaussian and sigma set the scale length of the gaussian\n",
			   argv[0]
		);
		printf("");
		exit(0);	
	}else{
		fileout = String_copy(argv[1]);
		rows = atoi(argv[2]);
		columns = atoi(argv[3]);
		k = atof(argv[4]);
		theta = atof(argv[5]);	
		radius = atof(argv[6]);	
	}
	if(argc > 7){
		invert = atoi(argv[7]);
	}
	if(argc > 8){
		sigma = atof(argv[8]);
	}
	mask = Data_create(rows,columns);
	for(i=0;i<rows;i++){
		for(j=0;j<columns;j++){
			r0 = rows/2 + k*sin(theta);
			c0 = columns/2 + k*cos(theta);
			r02 = rows/2 - k*sin(theta);
			c02 = columns/2 - k*cos(theta);
			r = (i - r0);
			c = (j - c0);
			r2 = (i - r02);
			c2 = (j - c02);
			if(sigma==0){
				if(r*r+c*c < radius*radius || r2*r2+c2*c2 < radius*radius){
					mask->elem[i][j] = !invert;				
				}else{
					mask->elem[i][j] = invert;
				}	
			}else{
				if(invert){
					mask->elem[i][j] = 1 - (exp(-(r*r+c*c)/(sigma*sigma)) + exp(-(r2*r2+c2*c2)/(sigma*sigma)));
				}else{
					mask->elem[i][j] = exp(-(r*r+c*c)/(sigma*sigma)) + exp(-(r2*r2+c2*c2)/(sigma*sigma));
				}		
			}
		}
	}
	Data_output(mask,fileout,p_float);
	Data_delete(mask);
	deallocate(fileout);
	return 0;
}
