#ifndef __PWMLEDDRIVER_H__
#define __PWMLEDDRIVER_H__


#define R(rgb) ((rgb >> 16) & ((1<<8)-1))
#define G(rgb) ((rgb >> 8) & ((1<<8)-1))
#define B(rgb) ((rgb >> 0) & ((1<<8)-1))

#define RED	0x0FF0000
#define ORANGE 0x0FF8C00
#define YELLOW 0x0FFFF00
#define GREEN 0x0008000
#define BLUE 0x00000FF
#define PURPLE 0x0800080

extern int PWMLEDDriverInit(void);
extern int PWMLEDDriverColor(int, int, int);

#endif
