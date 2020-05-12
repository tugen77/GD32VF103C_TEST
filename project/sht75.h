#ifndef __SHT75_H
#define __SHT75_H

typedef union
{
    unsigned int i;
    float f;
}sht_value;

enum {TEMP,HUMI};

void sht75Init(void);
int sht75Val(unsigned int *h, int *t);

#endif





