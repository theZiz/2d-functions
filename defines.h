#define WINDOW_X 800
#define WINDOW_Y 480

#define PARTICLE_COUNT 50000
#define PARTICLE_NEXT_DRAW 1

#define calc_gamma(alpha,beta) ((1+alpha*alpha) / beta)

#define RASTER_X (WINDOW_X/4)
#define RASTER_Y (WINDOW_Y/4)

//#define function(x,y) (x*x+y*y-1.0f)
#define function(x,y) (raum->gamma*x*x+2.0f*raum->alpha*x*y+raum->beta*y*y-raum->epsilon)

//Drift Strecke:
// | 1  -2*l   l*l |
// | 0     1    -l |
// | 0     0     1 |
//C function
#define Cf(s) 1
//C derivation
#define Cd(s) 0
//S function
#define Sf(s) s
//S derivation
#define Sd(s) 1
