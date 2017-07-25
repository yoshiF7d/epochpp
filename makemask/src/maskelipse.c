#include <utilities.h>
#include <LinkedList.h>
#include <Data.h>
 
int main(int argc, char *argv[]){
	char *fileout;
	int i,j,invert = 0;
	int rows,columns,r0,c0,r,c;
	double a,b,r2,c2,sigma=0,theta;
	Data mask;	

	if(argc < 9){
		printf(
			   "usage : %s [outputfile name] [rows] [columns] [r0] [c0] [a] [b] [theta] ([invert : 0]) ([sigma])\n"
			   "r0 : row index of the elipse center\n"
			   "c0 : column index of the elipse center\n"
			   "a : length of the semi-major axis\n"
			   "b : length of the semi-minor axis\n"
			   "theta : angle between the semi-major axis and horizontal line\n"
			   "invert : if this value is set to 1, then points inside the elipse is set to 1 and points outside the elipse is set to 0. If this value is set to 0, it becomes otherwise\n"
			   "sigma : if specified, mask becomes elipse-like gaussian and sigma set the scale length of the gaussian\n",
			   argv[0]
		);
		printf("");
		exit(0);	
	}else{
		fileout = String_copy(argv[1]);
		rows = atoi(argv[2]);
		columns = atoi(argv[3]);
		r0 = atoi(argv[4]);
		c0 = atoi(argv[5]);
		a = atof(argv[6]);
		b = atof(argv[7]);
		theta = atof(argv[8]);	
	}
	if(argc > 9){
		invert = atoi(argv[9]);
	}
	if(argc > 10){
		sigma = atof(argv[10]);
	}
	mask = Data_create(rows,columns);
	for(i=0;i<rows;i++){
		for(j=0;j<columns;j++){
			r = (i - r0);
			c = (j - c0);
			c2 = c*cos(theta) + r*sin(theta);
			r2 = -c*sin(theta) + r*cos(theta);
			if(sigma==0){
				if((c2/a)*(c2/a) + (r2/b)*(r2/b) < 1){
					mask->elem[i][j] = !invert;
				}else{
					mask->elem[i][j] = invert;
				}
			}else{	
				if(invert){
					mask->elem[i][j] = 1 - exp(-((c2/a)*(c2/a) + (r2/b)*(r2/b))/(sigma*sigma));

				}else{
					mask->elem[i][j] = exp(-((c2/a)*(c2/a) + (r2/b)*(r2/b))/(sigma*sigma));
				}
			}
		}
	}
	Data_output(mask,fileout,p_float);
	Data_delete(mask);
	deallocate(fileout);
	return 0;
}
