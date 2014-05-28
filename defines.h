#define WINDOW_X 1024
#define WINDOW_Y 768

#define PARTICLE_COUNT 3333
#define PARTICLE_NEXT_DRAW 1

#define calc_gamma(alpha,beta) ((1+alpha*alpha) / beta)

#define RASTER_X (WINDOW_X/4)
#define RASTER_Y (WINDOW_Y/4)

//#define function(x,y) (x*x+y*y-1.0f)
#define function(x,y) (raum->gamma*x*x+2.0f*raum->alpha*x*y+raum->beta*y*y-raum->epsilon)

#define TRACK_COUNT 8
#define TRACK_SIZE 2048
#define TRACK_STEP 0.05f
