#include <stdio.h>
#include <string.h>
#include <switch.h>
#include <dirent.h>
#include <sys/stat.h> 
#include "xplayer_fs.h"

void leeDirMedia(){
		DIR *dip; 
        struct dirent *dit;
        int i = 0; 


        if ((dip = opendir("/media/")) == NULL) 
        { 
            perror("opendir");
			strcpy(nameVideo[0],"error desde leeDirMedia();");
            //return 0; 
        } 	
        while ((dit = readdir(dip)) != NULL) 
        { 
            strcpy(nameVideo[i],dit->d_name);
			i++;
			contFilesInMedia++;
			if(i>11)break;
		}

		if (closedir(dip) == -1) 
		{ 
				perror("closedir"); 
				//return 0; 
		} 
		// cerramos el fichero

}
