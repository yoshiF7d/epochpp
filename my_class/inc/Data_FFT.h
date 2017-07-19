#ifndef DATA_FFT_H
#define DATA_FFT_H
#include <Data.h>

void Data_FFT(Data data);
void Data_IFFT(Data data);
void Data_DFT(Data data);
void Data_IDFT(Data data);
void Data_FFT2D(Data re, Data im);
void Data_IFFT2D(Data re, Data im);
void Data_FFT_prime(Data data);
void Data_IFFT_prime(Data data);
#endif