#ifndef xplayer_fs_H
#define simon_savegame_H

#define MEDIADIR "sdmc:/switch/media/"

struct stat s;
int err;
const char* savepath;
char linea[100];// 
char nameVideo[15][50];
int contFilesInMedia;

void leeDirMedia();

#endif

