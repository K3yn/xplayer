#ifndef xplayer_helper_H
#define xplayer_helper_H

u32 kDown;
u32 kHeld;
u32 kUp;


bool mode_main_menu;
bool mode_display_media_dir;
bool mode_play_video;
bool mode_display_debug;
int contMenuMain;
int selector;
int selectorVideo;
char charSelector[10];
char charSelectorVideo[10];

void loadIniVars();
void resetVars();

#endif
