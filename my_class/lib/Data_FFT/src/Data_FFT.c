#include "Data_FFT.h"
#define Data_FFT_large_prime 

double Data_FFT_cos(int n, int m){
	if(n == 0){
		return 1;
	}
	if(n == m){
		/*2 Pi*/
		return 1;
	}
	if(m%n == 0){
		if(m/n == 2){
			/*Pi*/
			return -1;
		}
		if(m/n == 4){
			/*Pi/2*/
			return 0;
		}
	}
	if(m%(m-n)==0){
		if(m/(m-n)==4){
			/*3 Pi/2*/
			return 0;
		}
	}
	return cos(2*M_PI*n/m);
}

double Data_FFT_sin(int n, int m){
	if(n == 0){
		return 0;
	}
	if(n == m){
		/*2 Pi*/
		return 0;
	}
	if(m%n == 0){
		if(m/n == 2){
			/*Pi*/
			return 0;
		}
		if(m/n == 4){
			/*Pi/2*/
			return 1;
		}
	}
	if(m%(m-n)==0){
		if(m/(m-n)==4){
			/*3 Pi/2*/
			return -1;
		}
	}
	return sin(2*M_PI*n/m);
}

/*temp->elem[0],temp->elem[1] : temporally array*/
/*temp->elem[2],temp->elem[3] : (data->row) th factor of 1 s.*/
void Data_FFT_mod(Data data, Data temp, int n, int sign);
void Data_FFT_mod_slow(Data data, Data temp, int n, int sign){
	int i,j,k,l,m,s,t;
	m = data->row/n;
	Data data2;
	/*hard-code for size 1 ~ 10 DFT*/
	#include "Data_FFT_hardcord_10.h"
	/*hard-coded for size 1,2,4 DFT*/ 
	/*
	switch(m){
	  case 1:
		return;
	  case 2:
		for(i=0;i<n;i++){
			temp->elem[i*2][0] = data->elem[i*2][0] + data->elem[i*2+1][0];
			temp->elem[i*2+1][0] = data->elem[i*2][0] - data->elem[i*2+1][0];
			temp->elem[i*2][1] = data->elem[i*2][1] + data->elem[i*2+1][1];
			temp->elem[i*2+1][1] = data->elem[i*2][1] - data->elem[i*2+1][1];
		}
		for(i=0;i<n;i++){
			data->elem[i*2][0] = temp->elem[i*2][0];
			data->elem[i*2][1] = temp->elem[i*2][1];
			data->elem[i*2+1][0] = temp->elem[i*2+1][0];
			data->elem[i*2+1][1] = temp->elem[i*2+1][1];
		}
		return;
	  case 4:
		for(i=0;i<n;i++){
			temp->elem[i*4][0] = data->elem[i*4][0] + data->elem[i*4+1][0] + data->elem[i*4+2][0] + data->elem[i*4+3][0];
			temp->elem[i*4+1][0] = data->elem[i*4][0] - sign*data->elem[i*4+1][1] - data->elem[i*4+2][0] + sign*data->elem[i*4+3][1];
			temp->elem[i*4+2][0] = data->elem[i*4][0] - data->elem[i*4+1][0] + data->elem[i*4+2][0] - data->elem[i*4+3][0];
			temp->elem[i*4+3][0] = data->elem[i*4][0] + sign*data->elem[i*4+1][1] - data->elem[i*4+2][0] - sign*data->elem[i*4+3][1];
			temp->elem[i*4][1] = data->elem[i*4][1] + data->elem[i*4+1][1] + data->elem[i*4+2][1] + data->elem[i*4+3][1];
			temp->elem[i*4+1][1] = data->elem[i*4][1] + sign*data->elem[i*4+1][0] - data->elem[i*4+2][1] - sign*data->elem[i*4+3][0];
			temp->elem[i*4+2][1] = data->elem[i*4][1] - data->elem[i*4+1][1] + data->elem[i*4+2][1] - data->elem[i*4+3][1];
			temp->elem[i*4+3][1] = data->elem[i*4][1] - sign*data->elem[i*4+1][0] - data->elem[i*4+2][1] + sign*data->elem[i*4+3][0];
		}
		for(i=0;i<n;i++){
			data->elem[i*4][0]   = temp->elem[i*4][0];
			data->elem[i*4+1][0] = temp->elem[i*4+1][0];
			data->elem[i*4+2][0] = temp->elem[i*4+2][0];
			data->elem[i*4+3][0] = temp->elem[i*4+3][0];
			data->elem[i*4][1]   = temp->elem[i*4][1];
			data->elem[i*4+1][1] = temp->elem[i*4+1][1];
			data->elem[i*4+2][1] = temp->elem[i*4+2][1];
			data->elem[i*4+3][1] = temp->elem[i*4+3][1];
		}
		return;
	}
	*/
	if(m==1){return;}
	/*Try factorization*/
	for(s=2;s*s<=m;s++){
		if(m % s == 0){
			t = m/s;
			/*permute*/
			for(i=0;i<n;i++){
				for(j=0;j<s;j++){
					for(k=0;k<t;k++){
						temp->elem[i*m + j*t + k][0] = data->elem[i*m + s*k + j][0];
						temp->elem[i*m + j*t + k][1] = data->elem[i*m + s*k + j][1];
					}
				}
			}
			for(i=0;i<data->row;i++){
				data->elem[i][0] = temp->elem[i][0];
				data->elem[i][1] = temp->elem[i][1];
			}
			Data_FFT_mod(data,temp,n*s,sign);
			for(i=0;i<data->row;i++){
				temp->elem[i][0] = temp->elem[i][1] = 0;
			}
			/*weave in twiddle factors*/
			for(i=0;i<n;i++){
				for(j=0;j<s;j++){
					for(l=0;l<t;l++){
						for(k=0;k<s;k++){
							temp->elem[i*m + j*t + l][0] 
							+= data->elem[i*m + k*t + l][0]*temp->elem[(k*(j*t+l)%m)*n][2] 
							- sign*data->elem[i*m + k*t + l][1]*temp->elem[(k*(j*t+l)%m)*n][3];
							temp->elem[i*m + j*t + l][1] 
							+= sign*data->elem[i*m + k*t + l][0]*temp->elem[(k*(j*t+l)%m)*n][3] 
							+ data->elem[i*m + k*t + l][1]*temp->elem[(k*(j*t+l)%m)*n][2];
						}
					} 
				}
			}
			for(i=0;i<data->row;i++){
				data->elem[i][0] = temp->elem[i][0];
				data->elem[i][1] = temp->elem[i][1];
			}
			return;
		}
	}
	/*Normal DFT*/
	for(i=0;i<data->row;i++){
		temp->elem[i][0] = temp->elem[i][1] = 0;
	}
	for(i=0;i<n;i++){
		for(j=0;j<m;j++){
			for(k=0;k<m;k++){
				temp->elem[i*m+j][0] += data->elem[i*m+k][0]*temp->elem[j*k*n%m][2] 
				- sign*data->elem[i*m+k][1]*temp->elem[j*k*n%m][3];
				temp->elem[i*m+j][1] += sign*data->elem[i*m+k][0]*temp->elem[j*k*n%m][3] 
				+ data->elem[i*m+k][1]*temp->elem[j*k*n%m][2];
			}
		}
	}
	for(i=0;i<data->row;i++){
		data->elem[i][0] = temp->elem[i][0];
		data->elem[i][1] = temp->elem[i][1];
	}
	/*use Data_FFT_prime*/
	/*	
	data2 = Data_create(m,2);
	for(i=0;i<n;i++){
		for(j=0;j<m;j++){
			data2->elem[j][0] = data->elem[i*m+j][0];
			data2->elem[j][1] = data->elem[i*m+j][1];
		}
		Data_FFT_prime(data2);
		for(j=0;j<m;j++){
			data->elem[i*m+j][0] = data2->elem[j][0];
			data->elem[i*m+j][1] = data2->elem[j][1];
		}
	}
	Data_delete(data2);
	*/
	return;
}

void Data_FFT(Data data){
	Data temp;
	int i;
	
	if(data->column!=2){
		fprintf(stdout,"Data_FFT : data column length must be 2\n");
		return;
	}
	
	temp = Data_create(data->row,4);
	for(i=0;i<data->row;i++){
		temp->elem[i][2] = Data_FFT_cos(i,data->row);
		temp->elem[i][3] = Data_FFT_sin(i,data->row);
	}
	Data_FFT_mod(data,temp,1,-1);
	Data_delete(temp);
}

void Data_FFT_slow(Data data){
	Data temp;
	int i;
	
	if(data->column!=2){
		fprintf(stdout,"Data_FFT : data column length must be 2\n");
		return;
	}
	
	temp = Data_create(data->row,4);
	for(i=0;i<data->row;i++){
		temp->elem[i][2] = Data_FFT_cos(i,data->row);
		temp->elem[i][3] = Data_FFT_sin(i,data->row);
	}
	Data_FFT_mod_slow(data,temp,1,-1);
	Data_delete(temp);
}

void Data_IFFT(Data data){
	Data temp;
	int i;
	
	if(data->column!=2){
		fprintf(stdout,"Data_FFT : data column length must be 2\n");
		return;
	}
	
	temp = Data_create(data->row,4);
	for(i=0;i<data->row;i++){
		temp->elem[i][2] = Data_FFT_cos(i,data->row);
		temp->elem[i][3] = Data_FFT_sin(i,data->row);
	}
	Data_FFT_mod(data,temp,1,1);
	for(i=0;i<data->row;i++){
		data->elem[i][0] /= data->row;
		data->elem[i][1] /= data->row;
	}
	Data_delete(temp);
}

void Data_DFT(Data data){
	Data temp;
	int i,j;
	
	if(data->column!=2){
		fprintf(stdout,"Data_FFT : data column length must be 2\n");
		return;
	}
	
	temp = Data_create(data->row,4);
	for(i=0;i<data->row;i++){
		temp->elem[i][2] = Data_FFT_cos(i,data->row);
		temp->elem[i][3] = Data_FFT_sin(i,data->row);
	}
	for(i=0;i<data->row;i++){
		temp->elem[i][0] = temp->elem[i][1] = 0;
	}
	
	for(i=0;i<data->row;i++){
		for(j=0;j<data->row;j++){
			temp->elem[i][0] += data->elem[j][0]*temp->elem[i*j%data->row][2] 
			+ data->elem[j][1]*temp->elem[i*j%data->row][3];
			temp->elem[i][1] += -data->elem[j][0]*temp->elem[i*j%data->row][3] 
			+ data->elem[j][1]*temp->elem[i*j%data->row][2];
		}
	}
	for(i=0;i<data->row;i++){
		data->elem[i][0] = temp->elem[i][0];
		data->elem[i][1] = temp->elem[i][1];
	}
	Data_delete(temp);
	return;
}

void Data_IDFT(Data data){
	Data temp;
	int i,j;
	
	if(data->column!=2){
		fprintf(stdout,"Data_FFT : data column length must be 2\n");
		return;
	}
	
	temp = Data_create(data->row,4);
	for(i=0;i<data->row;i++){
		temp->elem[i][2] = cos(2*M_PI*i/data->row);
		temp->elem[i][3] = sin(2*M_PI*i/data->row);
	}
	for(i=0;i<data->row;i++){
		temp->elem[i][0] = temp->elem[i][1] = 0;
	}
	
	for(i=0;i<data->row;i++){
		for(j=0;j<data->row;j++){
			temp->elem[j][0] += data->elem[j][0]*temp->elem[i*j%data->row][2] 
			- data->elem[i][1]*temp->elem[i*j%data->row][3];
			temp->elem[i][1] += data->elem[j][0]*temp->elem[i*j%data->row][3] 
			+ data->elem[j][1]*temp->elem[i*j%data->row][2];
		}
	}
	for(i=0;i<data->row;i++){
		data->elem[i][0] = temp->elem[i][0]/data->row;
		data->elem[i][1] = temp->elem[i][1]/data->row;
	}
	Data_delete(temp);
	return;
}

void Data_FFT2D_mod_row(Data re, Data im, Data temp, int n, int sign){
	int i,j,k,l,m,s,t,u,v;
	Data pdata,ptemp,pomega;
	int len,pad,p,g,ig,flag;
	m = re->column/n;
	/*hard-coded for size 1,2,4 DFT*/
	switch(m){
	  case 1:
		return;
	  case 2:
		for(i=0;i<re->row;i++){
			for(j=0;j<n;j++){
				temp->elem[j*2][0] = re->elem[i][j*2] + re->elem[i][j*2+1];
				temp->elem[j*2+1][0] = re->elem[i][j*2] - re->elem[i][j*2+1];
				temp->elem[j*2][1] = im->elem[i][j*2] + im->elem[i][j*2+1];
				temp->elem[j*2+1][1] = im->elem[i][j*2] - im->elem[i][j*2+1];
			}
			for(j=0;j<n;j++){
				re->elem[i][j*2] = temp->elem[j*2][0];
				re->elem[i][j*2+1] = temp->elem[j*2+1][0];
				im->elem[i][j*2] = temp->elem[j*2][1];
				im->elem[i][j*2+1] = temp->elem[j*2+1][1];
			}
		}
		return;
	  case 4:
		for(i=0;i<re->row;i++){
			for(j=0;j<n;j++){
				temp->elem[j*4][0] = re->elem[i][j*4] + re->elem[i][j*4+1] + re->elem[i][j*4+2] + re->elem[i][j*4+3];
				temp->elem[j*4+1][0] = re->elem[i][j*4] - sign*(im->elem[i][j*4+1]) - re->elem[i][j*4+2] + sign*(im->elem[i][j*4+3]);
				temp->elem[j*4+2][0] = re->elem[i][j*4] - re->elem[i][j*4+1] + re->elem[i][j*4+2] - re->elem[i][j*4+3];
				temp->elem[j*4+3][0] = re->elem[i][j*4] + sign*(im->elem[i][j*4+1]) - re->elem[i][j*4+2] - sign*(im->elem[i][j*4+3]);
				temp->elem[j*4][1] = im->elem[i][j*4] + im->elem[i][j*4+1] + im->elem[i][j*4+2] + im->elem[i][j*4+3];
				temp->elem[j*4+1][1] = im->elem[i][j*4] + sign*(re->elem[i][j*4+1]) - im->elem[i][j*4+2] - sign*(re->elem[i][j*4+3]);
				temp->elem[j*4+2][1] = im->elem[i][j*4] - im->elem[i][j*4+1] + im->elem[i][j*4+2] - im->elem[i][j*4+3];
				temp->elem[j*4+3][1] = im->elem[i][j*4] - sign*(re->elem[i][j*4+1]) - im->elem[i][j*4+2] + sign*(re->elem[i][j*4+3]);
			}
			for(j=0;j<n;j++){
				re->elem[i][j*4] = temp->elem[j*4][0];
				re->elem[i][j*4+1] = temp->elem[j*4][0];
				re->elem[i][j*4+2] = temp->elem[j*4+2][0];
				re->elem[i][j*4+3] = temp->elem[j*4+3][0];
				im->elem[i][j*4] = temp->elem[j*4][1];
				im->elem[i][j*4+1] = temp->elem[j*4][1];
				im->elem[i][j*4+2] = temp->elem[j*4+2][1];
				im->elem[i][j*4+3] = temp->elem[j*4+3][1];
			}
		}
		return;
	}
	/*Try factorization*/
	for(s=2;s*s<=m;s++){
		if(m % s == 0){
			t = m/s;
			/*permute*/
			for(u=0;u<re->row;u++){
				for(i=0;i<n;i++){
					for(j=0;j<s;j++){
						for(k=0;k<t;k++){
							temp->elem[i*m + j*t + k][0] = re->elem[u][i*m + s*k + j];
							temp->elem[i*m + j*t + k][1] = im->elem[u][i*m + s*k + j];
						}
					}
				}
				for(i=0;i<re->column;i++){
					re->elem[u][i] = temp->elem[i][0];
					im->elem[u][i] = temp->elem[i][1];
				}
			}
			Data_FFT2D_mod_row(re,im,temp,n*s,sign);
			
			for(u=0;u<re->row;u++){
				for(i=0;i<re->column;i++){
					temp->elem[i][0] = temp->elem[i][1] = 0;
				}
				/*weave in twiddle factors*/
				for(i=0;i<n;i++){
					for(j=0;j<s;j++){
						for(l=0;l<t;l++){
							for(k=0;k<s;k++){
								temp->elem[i*m + j*t + l][0] 
								+= re->elem[u][i*m + k*t + l]*temp->elem[(k*(j*t+l)%m)*n][2] 
								- sign*im->elem[u][i*m + k*t + l]*temp->elem[(k*(j*t+l)%m)*n][3];
								temp->elem[i*m + j*t + l][1] 
								+= sign*re->elem[u][i*m + k*t + l]*temp->elem[(k*(j*t+l)%m)*n][3] 
								+ im->elem[u][i*m + k*t + l]*temp->elem[(k*(j*t+l)%m)*n][2];
							}
						} 
					}
				}
				for(i=0;i<re->column;i++){
					re->elem[u][i] = temp->elem[i][0];
					im->elem[u][i] = temp->elem[i][1];
				}
			}
			return;
		}
	}
	/*Prime DFT*/
	p = m;
	for(len=p-1,i=0,flag=0;len>1;len>>=1,i++){
		if(len & 1){
			flag = 1;
		}
	}
	len = flag ? (1 << (i+2)) : p-1;
	pad = len - (p-1);
	g = primePrimitiveRoot(p);
	ig = powerMod(g,p-2,p);
	
	/*make temp array for Data_FFT_mod*/
	ptemp = Data_create(len,4);
	for(i=0;i<len;i++){
		ptemp->elem[i][2] = Data_FFT_cos(i,len);
		ptemp->elem[i][3] = Data_FFT_sin(i,len);
	}
	/*make sequence of root of 1s*/
	pomega = Data_create(len,2);
	for(i=0;i<len;i++){
		pomega->elem[i][0] = Data_FFT_cos(powerMod(ig,i%(p-1),p),p);
		pomega->elem[i][1] = sign*Data_FFT_sin(powerMod(ig,i%(p-1),p),p);
	}
	Data_FFT_mod(pomega,ptemp,1,-1);
	pdata = Data_create(len,2);	
	for(u=0;u<re->row;u++){
		/*make main data with padding*/
		for(k=0;k<n;k++){
			pdata->elem[0][0] = re->elem[u][k*m+1];
			pdata->elem[0][1] = im->elem[u][k*m+1];
			for(i=1;i<len;i++){
				if(i <= pad){
					pdata->elem[i][0] = 0;
					pdata->elem[i][1] = 0;
				}else{
					pdata->elem[i][0] = re->elem[u][k*m+powerMod(g,(i-pad)%(p-1),p)];
					pdata->elem[i][1] = im->elem[u][k*m+powerMod(g,(i-pad)%(p-1),p)];
				}
			}
			/*calculate convolution by FFT*/
			Data_FFT_mod(pdata,ptemp,1,-1);
			/*pointwise multiplication of 2 FFTs*/
			for(i=0;i<len;i++){
				ptemp->elem[i][0] = pdata->elem[i][0]*pomega->elem[i][0] - pdata->elem[i][1]*pomega->elem[i][1];
				ptemp->elem[i][1] = pdata->elem[i][0]*pomega->elem[i][1] + pdata->elem[i][1]*pomega->elem[i][0];
			}
			for(i=0;i<len;i++){
				pdata->elem[i][0] = ptemp->elem[i][0];
				pdata->elem[i][1] = ptemp->elem[i][1];
			}
			/*IFFT*/
			Data_FFT_mod(pdata,ptemp,1,1);
			/*add result of IFFT and 1st data*/
			for(i=0;i<len;i++){
				pdata->elem[i][0] = pdata->elem[i][0]/len + re->elem[u][k*m];
				pdata->elem[i][1] = pdata->elem[i][1]/len + im->elem[u][k*m];
			}
			/*below 2 procedures must be done in this order*/
			/*calculate DC term*/
			ptemp->elem[0][0] = 0;
			ptemp->elem[0][1] = 0;
			for(i=0;i<p;i++){
				ptemp->elem[0][0] += re->elem[u][k*m+i];
				ptemp->elem[0][1] += im->elem[u][k*m+i];
			}
			re->elem[u][k*m] = ptemp->elem[0][0];
			im->elem[u][k*m] = ptemp->elem[0][1];
			/*extract non-DC term*/
			for(i=0;i<p-1;i++){
				re->elem[u][k*m+powerMod(ig,i,p)] = pdata->elem[i][0];
				im->elem[u][k*m+powerMod(ig,i,p)] = pdata->elem[i][1];
			}
		}
	}
	Data_delete(pdata);
	Data_delete(pomega);
	Data_delete(ptemp);
	return;
}


void Data_FFT2D(Data re, Data im){
	int i,j;
	Data rtemp, ctemp,re2,im2;
	if(re->row != im->row || re->column != im->column){
		fprintf(stdout,"Data_FFT2D : re matrix and im matrix must have same dimension. re : (%d,%d) im : (%d,%d)\n",
				re->row,re->column,im->row,im->column);
		return;
	}
	rtemp = Data_create(re->row,4);
	ctemp = Data_create(re->column,4);
	
	for(i=0;i<re->row;i++){
		rtemp->elem[i][2] = Data_FFT_cos(i,re->row);
		rtemp->elem[i][3] = Data_FFT_sin(i,re->row);
	}
	for(i=0;i<re->column;i++){
		ctemp->elem[i][2] = Data_FFT_cos(i,re->column);
		ctemp->elem[i][3] = Data_FFT_sin(i,re->column);
	}
	Data_FFT2D_mod_row(re,im,ctemp,1,-1);
	
	re2=Data_transpose(re);
	im2=Data_transpose(im);
	Data_FFT2D_mod_row(re2,im2,rtemp,1,-1);
	for(i=0;i<re->row;i++){
		for(j=0;j<re->column;j++){
			re->elem[i][j] = re2->elem[j][i];
			im->elem[i][j] = im2->elem[j][i];
		}
	}
	Data_delete(re2);
	Data_delete(im2);
	Data_delete(rtemp);
	Data_delete(ctemp);
}

void Data_IFFT2D(Data re, Data im){
	int i,j;
	Data rtemp, ctemp,re2,im2;
	if(re->row != im->row || re->column != im->column){
		fprintf(stdout,"Data_FFT2D : re matrix and im matrix must have same dimension. re : (%d,%d) im : (%d,%d)\n",
				re->row,re->column,im->row,im->column);
		return;
	}
	rtemp = Data_create(re->row,4);
	ctemp = Data_create(re->column,4);
	
	for(i=0;i<re->row;i++){
		rtemp->elem[i][2] = Data_FFT_cos(i,re->row);
		rtemp->elem[i][3] = Data_FFT_sin(i,re->row);
	}
	for(i=0;i<re->column;i++){
		ctemp->elem[i][2] = Data_FFT_cos(i,re->column);
		ctemp->elem[i][3] = Data_FFT_sin(i,re->column);
	}
	
	Data_FFT2D_mod_row(re,im,rtemp,1,1);
	for(i=0;i<re->row;i++){
		for(j=0;j<re->row;j++){
			re->elem[i][j] /= re->column;
			im->elem[i][j] /= re->column;
		}
	}
	re2=Data_transpose(re);
	im2=Data_transpose(im);
	Data_FFT2D_mod_row(re2,im2,ctemp,1,1);
	for(i=0;i<re2->row;i++){
		for(j=0;j<re2->row;j++){
			re2->elem[i][j] /= re2->column;
			im2->elem[i][j] /= re2->column;
		}
	}
	for(i=0;i<re->row;i++){
		for(j=0;j<re->column;j++){
			re->elem[i][j] = re2->elem[j][i];
			im->elem[i][j] = im2->elem[j][i];
		}
	}
	Data_delete(re2);
	Data_delete(im2);
	Data_delete(rtemp);
	Data_delete(ctemp);
}

typedef struct st_intlist{
	int value;
	struct st_intlist *next;
}IntList_Sub;
typedef struct st_intlist *IntList;

IntList IntList_create(int value){
	IntList list = allocate(sizeof(IntList_Sub));
	list->value = value;
	list->next = NULL;
	return list;
}

void IntList_delete(IntList list){
	deallocate(list);
}

void IntList_deleteRoot(IntList list){
	IntList_delete(list->next);
	IntList_delete(list);
}

IntList IntList_append(IntList list, int value){
	if(!list){
		list = IntList_create(value);
	}else{
		list->next = IntList_create(value);
	}
	return list;
}

int IntList_memberQ(IntList list, int value){
	for(;list;list=list->next){
		if(value == list->value){return 1;}
	}
	return 0;
}

int primePrimitiveRoot(int p){
	int i,t;
	IntList list=NULL,s;
	for(i=2,t=p-1;i*i<=t;i++){
		if(t % i == 0){
			list = IntList_append(list,i);
			do{
				t/=i;
			}while(t%i==0);
		}
	}
	for(s=list;s;s=s->next){
		s->value = (p-1) / s->value;
	}
	for(i=2;i<=p-1;i++){
		for(s=list;s;s=s->next){
			if(powerMod(i,s->value,p)==1){
				goto loopend;
			}
		}
		break;
	  loopend: ;
	}
	return i;
}

int powerMod(int a, int b, int m){
	int i;
	for(i=1;b;b>>=1){
		if(b & 1){
			i = (i*a)%m;
		}
		a = (a*a)%m;
	}
	return i;
}

void Data_FFT_prime_mod(Data data,int sign){
	int i,j,p,len,pad,g,ig,flag;
	Data data2,omega,temp;
	p = data->row;
	for(len=p-1,i=0,flag=0;len>1;len>>=1,i++){
		if(len & 1){
			flag = 1;
		}
	}
	len = flag ? (1 << (i+2)) : p-1;
	pad = len - (p-1);
	g = primePrimitiveRoot(p);
	ig = powerMod(g,p-2,p);
	
	/*make sequence of root of 1s*/
	omega = Data_create(len,2);
	for(i=0;i<len;i++){
		omega->elem[i][0] = Data_FFT_cos(powerMod(ig,i%(p-1),p),p);
		omega->elem[i][1] = sign*Data_FFT_sin(powerMod(ig,i%(p-1),p),p);
	}
	/*make temp array for Data_FFT_mod*/
	temp = Data_create(len,4);
	for(i=0;i<len;i++){
		temp->elem[i][2] = Data_FFT_cos(i,len);
		temp->elem[i][3] = Data_FFT_sin(i,len);
	}
	
	/*make main data with padding*/
	data2 = Data_create(len,2);
	data2->elem[0][0] = data->elem[1][0];
	data2->elem[0][1] = data->elem[1][1];
	for(i=1;i<len;i++){
		if(i <= pad){
			data2->elem[i][0] = 0;
			data2->elem[i][1] = 0;
		}else{
			data2->elem[i][0] = data->elem[powerMod(g,(i-pad)%(p-1),p)][0];
			data2->elem[i][1] = data->elem[powerMod(g,(i-pad)%(p-1),p)][1];
		}
	}
	
	/*calculate convolution by FFT*/
	Data_FFT_mod(data2,temp,1,-1);
	Data_FFT_mod(omega,temp,1,-1);
	/*pointwise multiplication of 2 FFTs*/
	for(i=0;i<len;i++){
		temp->elem[i][0] = data2->elem[i][0]*omega->elem[i][0] - data2->elem[i][1]*omega->elem[i][1];
		temp->elem[i][1] = data2->elem[i][0]*omega->elem[i][1] + data2->elem[i][1]*omega->elem[i][0];
	}
	for(i=0;i<len;i++){
		data2->elem[i][0] = temp->elem[i][0];
		data2->elem[i][1] = temp->elem[i][1];
	}
	/*IFFT*/
	Data_FFT_mod(data2,temp,1,1);
	
	/*add result of IFFT and 1st data*/
	for(i=0;i<len;i++){
		data2->elem[i][0] = data2->elem[i][0]/len + data->elem[0][0];
		data2->elem[i][1] = data2->elem[i][1]/len + data->elem[0][1];
	}
	/*below 2 procedures must be done in this order*/
	/*calculate DC term*/
	temp->elem[0][0] = 0;
	temp->elem[0][1] = 0;
	for(i=0;i<p;i++){
		temp->elem[0][0] += data->elem[i][0];
		temp->elem[0][1] += data->elem[i][1];
	}
	data->elem[0][0] = temp->elem[0][0];
	data->elem[0][1] = temp->elem[0][1];
	/*extract non-DC term*/
	for(i=0;i<p-1;i++){
		data->elem[powerMod(ig,i,p)][0] = data2->elem[i][0];
		data->elem[powerMod(ig,i,p)][1] = data2->elem[i][1];
	}
	if(sign>0){
		for(i=0;i<p;i++){
			data->elem[i][0] /= p;
			data->elem[i][1] /= p;
		}
	}
	
	Data_delete(data2);
	Data_delete(omega);
	Data_delete(temp);
}

void Data_FFT_prime(Data data){
	Data_FFT_prime_mod(data,-1);
};
void Data_IFFT_prime(Data data){
	Data_FFT_prime_mod(data,1);
};

void Data_FFT_mod(Data data, Data temp, int n, int sign){
	int i,j,k,l,m,s,t;
	m = data->row/n;
	/*use in Rader's algorithm*/
	Data pdata,ptemp,pomega;
	int len,pad,p,g,ig,flag;
	/*hard-coded for size 1~10 DFT*/
	#include "Data_FFT_hardcord_10.h"
	/*Try factorization*/
	for(s=2;s*s<=m;s++){
		if(m % s == 0){
			t = m/s;
			/*permute*/
			for(i=0;i<n;i++){
				for(j=0;j<s;j++){
					for(k=0;k<t;k++){
						temp->elem[i*m + j*t + k][0] = data->elem[i*m + s*k + j][0];
						temp->elem[i*m + j*t + k][1] = data->elem[i*m + s*k + j][1];
					}
				}
			}
			for(i=0;i<data->row;i++){
				data->elem[i][0] = temp->elem[i][0];
				data->elem[i][1] = temp->elem[i][1];
			}
			Data_FFT_mod(data,temp,n*s,sign);
			for(i=0;i<data->row;i++){
				temp->elem[i][0] = temp->elem[i][1] = 0;
			}
			/*weave in twiddle factors*/
			for(i=0;i<n;i++){
				for(j=0;j<s;j++){
					for(l=0;l<t;l++){
						for(k=0;k<s;k++){
							temp->elem[i*m + j*t + l][0] 
							+= data->elem[i*m + k*t + l][0]*temp->elem[(k*(j*t+l)%m)*n][2] 
							- sign*data->elem[i*m + k*t + l][1]*temp->elem[(k*(j*t+l)%m)*n][3];
							temp->elem[i*m + j*t + l][1] 
							+= sign*data->elem[i*m + k*t + l][0]*temp->elem[(k*(j*t+l)%m)*n][3] 
							+ data->elem[i*m + k*t + l][1]*temp->elem[(k*(j*t+l)%m)*n][2];
						}
					} 
				}
			}
			for(i=0;i<data->row;i++){
				data->elem[i][0] = temp->elem[i][0];
				data->elem[i][1] = temp->elem[i][1];
			}
			return;
		}
	}
	/*Normal DFT*/
	/*
	for(i=0;i<data->row;i++){
		temp->elem[i][0] = temp->elem[i][1] = 0;
	}
	for(i=0;i<n;i++){
		for(j=0;j<m;j++){
			for(k=0;k<m;k++){
				temp->elem[i*m+j][0] += data->elem[i*m+k][0]*temp->elem[j*k*n%m][2] 
				- sign*data->elem[i*m+k][1]*temp->elem[j*k*n%m][3];
				temp->elem[i*m+j][1] += sign*data->elem[i*m+k][0]*temp->elem[j*k*n%m][3] 
				+ data->elem[i*m+k][1]*temp->elem[j*k*n%m][2];
			}
		}
	}
	for(i=0;i<data->row;i++){
		data->elem[i][0] = temp->elem[i][0];
		data->elem[i][1] = temp->elem[i][1];
	}
	*/
	/*use Data_FFT_prime*/
	p = m;
	for(len=p-1,i=0,flag=0;len>1;len>>=1,i++){
		if(len & 1){
			flag = 1;
		}
	}
	len = flag ? (1 << (i+2)) : p-1;
	pad = len - (p-1);
	g = primePrimitiveRoot(p);
	ig = powerMod(g,p-2,p);
	
	/*make temp array for Data_FFT_mod*/
	ptemp = Data_create(len,4);
	for(i=0;i<len;i++){
		ptemp->elem[i][2] = Data_FFT_cos(i,len);
		ptemp->elem[i][3] = Data_FFT_sin(i,len);
	}
	/*make sequence of root of 1s*/
	pomega = Data_create(len,2);
	for(i=0;i<len;i++){
		pomega->elem[i][0] = Data_FFT_cos(powerMod(ig,i%(p-1),p),p);
		pomega->elem[i][1] = sign*Data_FFT_sin(powerMod(ig,i%(p-1),p),p);
	}
	Data_FFT_mod(pomega,ptemp,1,-1);
	pdata = Data_create(len,2);	
	for(k=0;k<n;k++){
		/*make main data with padding*/
		pdata->elem[0][0] = data->elem[k*m+1][0];
		pdata->elem[0][1] = data->elem[k*m+1][1];
		for(i=1;i<len;i++){
			if(i <= pad){
				pdata->elem[i][0] = 0;
				pdata->elem[i][1] = 0;
			}else{
				pdata->elem[i][0] = data->elem[k*m+powerMod(g,(i-pad)%(p-1),p)][0];
				pdata->elem[i][1] = data->elem[k*m+powerMod(g,(i-pad)%(p-1),p)][1];
			}
		}
		/*calculate convolution by FFT*/
		Data_FFT_mod(pdata,ptemp,1,-1);
		/*pointwise multiplication of 2 FFTs*/
		for(i=0;i<len;i++){
			ptemp->elem[i][0] = pdata->elem[i][0]*pomega->elem[i][0] - pdata->elem[i][1]*pomega->elem[i][1];
			ptemp->elem[i][1] = pdata->elem[i][0]*pomega->elem[i][1] + pdata->elem[i][1]*pomega->elem[i][0];
		}
		for(i=0;i<len;i++){
			pdata->elem[i][0] = ptemp->elem[i][0];
			pdata->elem[i][1] = ptemp->elem[i][1];
		}
		/*IFFT*/
		Data_FFT_mod(pdata,ptemp,1,1);
		/*add result of IFFT and 1st data*/
		for(i=0;i<len;i++){
			pdata->elem[i][0] = pdata->elem[i][0]/len + data->elem[k*m][0];
			pdata->elem[i][1] = pdata->elem[i][1]/len + data->elem[k*m][1];
		}
		/*below 2 procedures must be done in this order*/
		/*calculate DC term*/
		ptemp->elem[0][0] = 0;
		ptemp->elem[0][1] = 0;
		for(i=0;i<p;i++){
			ptemp->elem[0][0] += data->elem[k*m+i][0];
			ptemp->elem[0][1] += data->elem[k*m+i][1];
		}
		data->elem[k*m][0] = ptemp->elem[0][0];
		data->elem[k*m][1] = ptemp->elem[0][1];
		/*extract non-DC term*/
		for(i=0;i<p-1;i++){
			data->elem[k*m+powerMod(ig,i,p)][0] = pdata->elem[i][0];
			data->elem[k*m+powerMod(ig,i,p)][1] = pdata->elem[i][1];
		}
	}
	Data_delete(pdata);
	Data_delete(pomega);
	Data_delete(ptemp);
}