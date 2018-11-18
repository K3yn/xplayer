#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>

#include "SDL_Helper.h"
#include "xplayer_helper.h"
#include "xplayer_touch.h"
#include "xplayer_fonts.h"
#include "xplayer_fs.h"
#define COLOR
#define SDL_AUDIO_BUFFER_SIZE 1024
#define MAX_AUDIO_FRAME_SIZE 192000

#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000
#define VIDEO_PICTURE_QUEUE_SIZE 1
AVFormatContext *pFormatContext;
AVPacket *pPacket;
AVFrame *pFrame;
AVCodecContext *pCodecContext;
AVCodecContext *paudiocodecContext;
AVCodec *paudiocodec;
AVOption *opts;
AVFormatContext *pFormatCtx;
AVCodecContext  *aCodecCtx;
SDL_Event       event;
SDL_AudioSpec   wanted_spec, spec;
int *nullptr;
char s1[100], s2[100];
char videofile[50];
char charSonido[200];
char charError[200];
int x;
int i;
int j;
int i;
uint8_t audio_buff[(192000 * 3) / 2];
uint8_t *paudio_buff = audio_buff;
int audio_buff_size = 0;
int videoStream;
int audioStream;
bool error;
bool salir;
bool bquit;
//static uint8_t *internalbuffer = NULL;


typedef struct PacketQueue {
  AVPacketList *first_pkt, *last_pkt;
  int nb_packets;
  int size;
  SDL_mutex *mutex;
  SDL_cond *cond;
} PacketQueue;
PacketQueue audioq;



void **buffer;
size_t *buffersize;
//static SwrContext *pSwrContext = NULL;
// print out the steps and errors

//static void logging(const char *fmt, ...);
static int decode_packet(AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame);


/*
 * Audio decoding.
 */
void strClear(char p[],u8 len){u8 i=0; 
	if(len){while(i<len){p[i]=0;i++;}}
}
void displayPlayer(){
	SDL_RenderClear(renderer);//CLEAR
	if(mode_main_menu)
	{
		SDL_RenderCopy(renderer, background[0].texture, NULL, NULL);//Background
		SDL_DrawText(renderer, Roboto, 1260, 690, BLACK, charSelector);//
		SDL_DrawText(renderer, Roboto_50, 400, 260, BLACK, "SDMC:/media/");//
		SDL_DrawText(renderer, Roboto_50, 400, 360, BLACK, "OPCION 2");//
		SDL_DrawText(renderer, Roboto_50, 400, 460, BLACK, "EXIT");//
		SDL_DrawText(renderer, Roboto, 80, 600, RED, "* THIS IS AN ALPHA, IT DOES NOT HAVE SOUND, ONLY PROOF MP4 OR MKV OR IT CRASH");//
		switch(selector){
			case 1:
				SDL_DrawText(renderer, Roboto_50, 400, 260, GREEN, "SDMC:/media/");//;
				break;
			case 2:
				SDL_DrawText(renderer, Roboto_50, 400, 360, GREEN, "OPCION 2");//;
				break;	
			case 3:
				SDL_DrawText(renderer, Roboto_50, 400, 460, GREEN, "EXIT");;
				break;
			default:
				break;
		}
	}
	else if(mode_display_media_dir)
	{
		SDL_RenderCopy(renderer, background[0].texture, NULL, NULL);//Background
		SDL_DrawText(renderer, Roboto, 1260, 690, BLACK, charSelectorVideo);//
		SDL_DrawText(renderer, Roboto_50, 540, 20, BLACK, "SDMC:/MEDIA/");//
		for(x=0;x<contFilesInMedia;x++){
			SDL_DrawText(renderer, Roboto_large, 70, 120+x*45, BLACK, nameVideo[x]);//
		}
		SDL_DrawText(renderer, Roboto_large, 70, 120+selectorVideo*45, GREEN, nameVideo[selectorVideo]);//
		
		
	}
	else if(mode_play_video)
	{
		//SDL_RenderCopy(renderer, texture, NULL, NULL);
		//SDL_RenderCopy(renderer, background[0].texture, NULL, NULL);//Background
		//SDL_DrawText(renderer, Roboto_80, 530, 140, BLACK, s1);//
	}
	else if(mode_display_debug)
	{
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderCopy(renderer, background[1].texture, NULL, NULL);//Background
		SDL_DrawText(renderer, Roboto_80, 530, 100, BLACK, "DEBUG");//
		SDL_DrawText(renderer, Roboto_large, 50, 250, BLACK, charError);//
		SDL_DrawText(renderer, Roboto_large, 50, 300, GREEN, nameVideo[selectorVideo]);//
		SDL_DrawText(renderer, Roboto_large, 50, 350, GREEN, s1);//
	}
	SDL_DrawText(renderer, Roboto_80, 30, 0, RED, "X");//
	SDL_DrawText(renderer, Roboto_80, 85, 0, BLACK, "PLAYER");//
	SDL_RenderPresent(renderer);//REFRESH		
}
void manageControls(){
	if(mode_main_menu)
	{
		if (kDown & KEY_DOWN)
		{
			selector++;
			selector %= 4;
		}
		else if (kDown & KEY_UP)
		{
			if (selector > 0)
			{
				selector--;
			}
			else
			{
				selector = 3;
			}
		}
		else if ((kUp & KEY_A) && (selector == 1))
		{
			mode_main_menu = false;
			mode_display_media_dir=true;
		}
		else if ((kUp & KEY_A) && (selector == 2))
		{
			//mode_main_menu = false;
			//mode_play_video= true;
		}	
		else if ((kUp & KEY_A) && (selector == 3))
		{
			salir = true;
		}		
		//TACTIL
		
		u8 newContMenuMain;
		if ((kDown & KEY_TOUCH && inBox(Stylus, 500, 210, 700, 280)))// - start game
		{
			newContMenuMain=0;
			
			if (contMenuMain == newContMenuMain)
			{
				mode_main_menu=false;
				//selectorModo=true;
			}
			else
			{
				contMenuMain = newContMenuMain;
				//playSound('N');
			}
		} 
		else if ((kDown & KEY_TOUCH && inBox(Stylus, 500, 410, 700, 480)))// - EXIT
		{	newContMenuMain=1;
			
			if (contMenuMain == newContMenuMain)
			{
				//salir=true;
				//selectorModo=true;
				//playSound('A');
			}
			else
			{
				contMenuMain = newContMenuMain;
				//playSound('N');
			}
		} 		
	}
	else if(mode_display_media_dir)
	{
		if (kDown & KEY_DOWN)
		{
			selectorVideo++;
			selectorVideo %= contFilesInMedia;
		}
		else if (kDown & KEY_UP)
		{
			if (selectorVideo > 0)
			{
				selectorVideo--;
			}
			else
			{
				selectorVideo = contFilesInMedia-1;
			}
		}
		else if (kUp & KEY_A)
		{
			mode_display_media_dir=false;
			mode_play_video=true;
		}
	
		else if (kUp & KEY_B)
		{
			mode_main_menu = true;
			mode_display_media_dir=false;
		}		
				
		
	}
	else if(mode_play_video)
	{

		strClear(s1,strlen(s1));
		strcpy(s1,"/media/");
		strcpy(s2,nameVideo[selectorVideo]);

		// calculate the length of string s1
		// and store it in i
		for(i = 0; s1[i] != '\0'; ++i);

		for(j = 0; s2[j] != '\0'; ++j, ++i)
		{
			s1[i] = s2[j];
		}
			//logging("initializing all the containers, codecs and protocols.");
			// AVFormatContext holds the header information from the format (Container)
			// contiene la información del encabezado del formato
			// Allocating memory for this component
			// Asignación de memoria para este componente
		AVFormatContext *pFormatContext = avformat_alloc_context();
		if (!pFormatContext) {
			//logging("ERROR could not allocate memory for Format Context");
			error=true;
			strcpy(charError,"ERROR could not allocate memory for Format Context");
		}
		//logging("opening the input file (%s) and loading format (container) header", "sdmc:/media/oceans.mp4");
		// Open the file and read its header. The codecs are not opened.
		// The function arguments are:
		// AVFormatContext (the component we allocated memory for),
		// url (filename),
		// AVInputFormat (if you pass NULL it'll do the auto detect)
		// and AVDictionary (which are options to the demuxer)

		// Abre el archivo y lee su encabezado. Los codecs no se abren.
		// Los argumentos de la función son:
		// AVFormatContext (el componente al que asignamos memoria),
		// url (nombre de archivo),
		// AVInputFormat (si pasa NULL, hará la detección automática)
		// y AVDictionary (que son opciones para el demuxer)
		if(!error)
		{
			
			if (avformat_open_input(&pFormatContext, s1, NULL, NULL) != 0)
			{
			//logging("ERROR could not open the file");
			error=true;
			strcpy(charError,"ERROR could not open the file");
			}
		}


		// now we have access to some information about our file
		// since we read its header we can say what format (container) it's
		// and some other information related to the format itself.
		// ahora tenemos acceso a alguna información sobre nuestro archivo
		// desde que leemos su encabezado podemos decir qué formato (contenedor) es
		// y alguna otra información relacionada con el formato en sí.	  
		//logging("format %s, duration %lld us, bit_rate %lld", pFormatContext->iformat->name, pFormatContext->duration, pFormatContext->bit_rate);

		//logging("finding stream info from format");
		// read Packets from the Format to get stream information
		// this function populates pFormatContext->streams
		// (of size equals to pFormatContext->nb_streams)
		// the arguments are:
		// the AVFormatContext
		// and options contains options for codec corresponding to i-th stream.
		// On return each dictionary will be filled with options that were not found.
		// lee Paquetes del Formato para obtener información de transmisión
		// esta función llena pFormatContext-> streams
		// (de tamaño igual a pFormatContext-> nb_streams)
		// los argumentos son:
		// el AVFormatContext
		// y opciones contiene opciones para el códec correspondiente a i-th stream.
		// A la vuelta, cada diccionario se llenará con opciones que no se encontraron.
	  	if(!error)
		{
			if (avformat_find_stream_info(pFormatContext,  NULL) < 0) {
				// logging("ERROR could not get the stream info");
				strcpy(charError,"ERROR could not get the stream info");
				error=true;
			}
		}
		// the component that knows how to enCOde and DECode the stream
		// it's the codec (audio or video)
		
		// el componente que sabe cómo codificar y deCodificar el flujo
		// es el codec (audio o video) 
		if(!error)
		{
			AVCodec *pCodec = NULL;
			// this component describes the properties of a codec used by the stream i
			// este componente describe las propiedades de un códec utilizado por el flujo i

			AVCodecParameters *pCodecParameters =  NULL;
			int video_stream_index = -1;
			int audio_stream_index = -1;
			// loop though all the streams and print its main information
			// recorre todas las secuencias e imprime su información principal
			for (int i = 0; i < pFormatContext->nb_streams; i++)
			{
				AVCodecParameters *pLocalCodecParameters =  NULL;
				pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
				//logging("AVStream->time_base before open coded %d/%d", pFormatContext->streams[i]->time_base.num, pFormatContext->streams[i]->time_base.den);
				//logging("AVStream->r_frame_rate before open coded %d/%d", pFormatContext->streams[i]->r_frame_rate.num, pFormatContext->streams[i]->r_frame_rate.den);
				//logging("AVStream->start_time %" PRId64, pFormatContext->streams[i]->start_time);
				//logging("AVStream->duration %" PRId64, pFormatContext->streams[i]->duration);

				//logging("finding the proper decoder (CODEC)");

				AVCodec *pLocalCodec = NULL;

				// finds the registered decoder for a codec ID
				// encuentra el decodificador registrado para un codec ID
				pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

				if (pLocalCodec==NULL) {
					//logging("ERROR unsupported codec!");
					strcpy(charError,"ERROR unsupported codec!");
				}

				// when the stream is a video we store its index, codec parameters and codec
				// cuando la transmisión es un video, almacenamos su índice, parámetros de códec y códec.
				if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
					video_stream_index = i;
					pCodec = pLocalCodec;
					pCodecParameters = pLocalCodecParameters;

					//logging("Video Codec: resolution %d x %d", pLocalCodecParameters->width, pLocalCodecParameters->height);
				}
				if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {

					// Set audio settings from codec info
					audio_stream_index = i;
					//logging("Audio Codec: %d channels, sample rate %d", pLocalCodecParameters->channels, pLocalCodecParameters->sample_rate);
				
				}
				

				
				// print its name, id and bitrate
				// imprime su nombre, id y bitrate
				//logging("\tCodec %s ID %d bit_rate %lld", pLocalCodec->name, pLocalCodec->id, pCodecParameters->bit_rate);
			}
			if(audio_stream_index==-1){
					//error=true;
					error=true;
					strcpy(charError,"audio_stream_index vale -1");
			}
			// https://ffmpeg.org/doxygen/trunk/structAVCodecContext.html
			AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
			if (!pCodecContext)
			{
				//logging("failed to allocated memory for AVCodecContext");
				strcpy(charError,"failed to allocated memory for AVCodecContext");
				error=true;
				//return -1;
			}

			// Fill the codec context based on the values from the supplied codec parameters
			// Rellene el contexto del códec en función de los valores de los parámetros de códec suministrados  
			if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0)
			{
				//logging("failed to copy codec params to codec context");
				strcpy(charError,"failed to copy codec params to codec context");
				error=true;
				//return -1;
			}

			// Initialize the AVCodecContext to use the given AVCodec.
			// Inicialice AVCodecContext para usar el AVCodec dado
			// https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#ga11f785a188d7d9df71621001465b0f1d
			if (avcodec_open2(pCodecContext, pCodec, NULL) < 0)
			{
				//logging("failed to open codec through avcodec_open2");
				strcpy(charError,"failed to open codec through avcodec_open2");
				//return -1;
			}

			// https://ffmpeg.org/doxygen/trunk/structAVFrame.html
			AVFrame *pFrame = av_frame_alloc();
			if (!pFrame)
			{
				//logging("failed to allocated memory for AVFrame");
				strcpy(charError,"failed to allocated memory for AVFrame");
				//return -1;
			}
			// https://ffmpeg.org/doxygen/trunk/structAVPacket.html
			AVPacket *pPacket = av_packet_alloc();
			if (!pPacket)
			{
				//logging("failed to allocated memory for AVPacket");
				strcpy(charError,"failed to allocated memory for AVPacket");
				//return -1;
			}

			int response = 0;
			//int how_many_packets_to_process = 1000;

			// fill the Packet with data from the Stream
			// llena el paquete con datos de la corriente
		  
		  
			// Empiezo aqui
			//texture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STATIC,1280, 720); 
		  

			//logging("releasing all the resources");
		  

			// Make a screen to put our video

			
			while (av_read_frame(pFormatContext, pPacket) >= 0)
			{
				//int gotFrame;// decode one frame
				
				// if it's the video stream
				if (pPacket->stream_index == video_stream_index)
				{
					//logging("AVPacket->pts %" PRId64, pPacket->pts);
					response = decode_packet(pPacket, pCodecContext, pFrame);
					
					if (response < 0){
						break;
					}
					
					// stop it, otherwise we'll be saving hundreds of frames
					//if (--how_many_packets_to_process <= 0) break; // LIMITAR LOS FRAMES
				}
				// https://ffmpeg.org/doxygen/trunk/group__lavc__packet.html#ga63d5a489b419bd5d45cfd09091cbcbc2
			
				
				av_packet_unref(pPacket);

				if (kDown & KEY_B)
				{
					mode_main_menu = true;
					mode_play_video=false;
					avformat_close_input(&pFormatContext);
					avformat_free_context(pFormatContext);
					av_packet_free(&pPacket);
					av_frame_free(&pFrame);
					avcodec_free_context(&pCodecContext);
					break;
				}		
			}
			avformat_close_input(&pFormatContext);
			avformat_free_context(pFormatContext);
			av_packet_free(&pPacket);
			av_frame_free(&pFrame);
			avcodec_free_context(&pCodecContext);
		}
		if(!error){
			mode_play_video=false;
			mode_main_menu=true;
		}
		else
		{
			mode_display_debug=true;
			mode_play_video=false;
		}
		
	}
	else if(mode_display_debug)
	{
		if (kUp & KEY_B)
		{
			mode_main_menu = true;
			mode_display_debug=false;
			error=false;
		}	
		
	}
	itoa(selector,charSelector,10);
	itoa(selectorVideo,charSelectorVideo,10);
}
int main()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	SDL_Init(SDL_INIT_AUDIO);
	romfsInit();
	TTF_Init();
	//gfxInitDefault();
	// consoleInit(NULL);

 
	//printf("Hello\n");
	// Create an SDL window & renderer
	window = SDL_CreateWindow("Main-Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	
	//BG
	surface = IMG_Load("romfs:/resources/main.png");
	background[0].texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	//BG
	surface = IMG_Load("romfs:/resources/main1.png");
	background[1].texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	//
	load_fonts();
	loadIniVars();
	leeDirMedia();
	contMenuMain=0;
	mode_main_menu=true;
	error=false;
	salir=false;
	while (appletMainLoop())
	{
		hidScanInput();
		kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		kUp = hidKeysUp(CONTROLLER_P1_AUTO);
		kHeld= hidKeysHeld(CONTROLLER_P1_AUTO);
		if (kDown & KEY_PLUS || salir) break;
		
		manageControls();
		displayPlayer();//gfxFlushBuffers();//gfxSwapBuffers();
		//gfxFlushBuffers();
		//gfxSwapBuffers();
	}
	avformat_close_input(&pFormatContext);
	avformat_free_context(pFormatContext);
	av_packet_free(&pPacket);
	av_frame_free(&pFrame);
	avcodec_free_context(&pCodecContext); 
	//gfxExit();
	TTF_Quit();
	romfsExit();				//romfs cleanup
	IMG_Quit();
	SDL_Quit();				// SDL cleanup
	return 0; 	// Clean exit to HBMenu
}
//static void logging(const char *fmt, ...)
//{
    //va_list args;
    //fprintf( stderr, "LOG: " );
    //va_start( args, fmt );
    //vfprintf( stderr, fmt, args );
    //va_end( args );
   // fprintf( stderr, "\n" );
//}


static int decode_packet(AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame)
{
  // Supply raw packet data as input to a decoder
  // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING,pCodecContext->width,pCodecContext->height);
  int response = avcodec_send_packet(pCodecContext, pPacket);
	//AVCodecContext *c= NULL;
  if (response < 0) {
   // logging("Error while sending a packet to the decoder: %s", av_err2str(response));
    return response;
  }

  while (response >= 0)
  {
    // Return decoded output data (into a frame) from a decoder
    // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
    response = avcodec_receive_frame(pCodecContext, pFrame);
    if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
      break;
    } else if (response < 0) {
     // logging("Error while receiving a frame from the decoder: %s", av_err2str(response));
      return response;
    }

    if (response >= 0) {
     // logging(
		//   "Frame %d (type=%c, size=%d bytes) pts %d key_frame %d [DTS %d]",
        // pCodecContext->frame_number,
        // av_get_picture_type_char(pFrame->pict_type),
        // pFrame->pkt_size,
        // pFrame->pts,
        // pFrame->key_frame,
        //  pFrame->coded_picture_number
		// );
		/* decode audio frame */
        //ret = avcodec_decode_audio4(audio_dec_ctx, pFrame, got_frame, &pPacket);
		//avcodec_receive_frame
		//avcodec_send_packet

		//*buffer = internalbuffer;
		//*buffersize = sizes;
		
		
		//char frame_filename[1024];
		//snprintf(frame_filename, sizeof(frame_filename), "%s-%d.pgm", "frame", pCodecContext->frame_number);
		// save a grayscale frame into a .pgm file
		//save_gray_frame(pFrame->data[0], pFrame->linesize[0], pFrame->width, pFrame->height, frame_filename);
		SDL_UpdateYUVTexture(texture, NULL, pFrame->data[0],
											pFrame->linesize[0],
											pFrame->data[1],
											pFrame->linesize[1],
											pFrame->data[2],
											pFrame->linesize[2]); 
	
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL,  NULL); 
		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(texture);
		//SDL_Delay(10);
    }
	av_frame_unref(pFrame);
  }
  return 0;
}

