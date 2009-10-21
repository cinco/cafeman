#ifndef LOCKPIX_H
#define LOCKPIX_H

unsigned char *grabber_gif;
unsigned char *locker_gif;
unsigned char *popup_gif;
unsigned char *startpopup_gif;
unsigned char *endpopup_gif;

int init_img(char *libname, const char *version);

#endif
