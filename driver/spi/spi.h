#ifndef __SPI_H__
#define __SPI_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

typedef struct spiDev_t{

}spiDev_t;

void startCondition(spiDev_t * dev);
void stopCondition(spiDev_t * dev);
void sendByte(spiDev_t * dev, unsigned char * send, unsigned char * res);

#endif