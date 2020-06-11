#include <X11/Xlib.h>
#include "ADDA.h"
#include "FFT.h"
#include "XG.h"

#define SAMPLING	11	/* kHz			*/
#define SLOTPNTS	550	/* points / dot		*/
#define THINNER		55	/* changed to sf * 5	*/
#define AD_GAIN		1.0
#define MAX_TIME	3	/* stop after 3s	*/
#define FFTPNTS		512
#define CUT_OFF		40

short   wave[SAMPLING*1000*MAX_TIME];

double  real[FFTPNTS], imag[FFTPNTS];

int     np, kai;

void  DrawGraph();

int  main(void)
{
  Display *dp;
  Window   w1, window;
  XEvent   event;
  int      i, button, x, y;
  double   X, Y;

  /* A/D conversion */

  OpenADIN(SAMPLING,SLOTPNTS,AD_GAIN); 
  printf("A/D start ...\n");

  np  = SAMPLING*1000*MAX_TIME;
  kai = np/SLOTPNTS;
  for (i=0; i < kai ;i++) {
    ADIN(&wave[SLOTPNTS*i]);
  }

  printf("finished\n");
  CloseADIN();

  /* show waveform */
  
  w1 = XGOpenGraphic("Test of X11", 100, 100, 640, 480, "");
  dp = XGDisplay();
  
  XGSelectXform(1);
  XGXformPhysical( 20,   20,  600,  100  );
  XGXformVirtual ( 0., -5000., (double)np, 10000. );

  XGSelectXform(2);
  XGXformPhysical( 20,  140,  600,  320  );
  XGXformVirtual ( 0., -10., (double)FFTPNTS, 40. );
  
  XGSetGrade(2);	/* pen = green	*/
  XClearWindow(dp,w1);
  DrawGraph();
  
  for (;;) {
    XNextEvent(dp,&event);
    
    switch (event.type) {
    case Expose:
      DrawGraph();
      break;
    case ButtonRelease:
      window  = ((XButtonEvent *)&event)->window;
      button  = ((XButtonEvent *)&event)->button;
      x       = ((XButtonEvent *)&event)->x;
      y       = ((XButtonEvent *)&event)->y;


      if (window == w1 && button == 1) {

        XGSelectXform(1);
        X = XGXform_x(x);
        Y = XGXform_y(y);
        //printf("X= %g Y= %g\n", X, Y);

        if ((int)X < 0         ) continue;
        if ((int)X > np-FFTPNTS) continue;
  
        for (i=0; i<FFTPNTS ;i++) {
          real[i] = (double) wave[(int)X+i];
          imag[i] = 0.0;
        }

        vhan(real, FFTPNTS);
        vfft(real, imag, 9, 0);
        for (i=0; i<FFTPNTS ;i++) {
          real[i] = real[i]*real[i]+imag[i]*imag[i];

          real[i] = log(real[i]);
          imag[i] = 0.0;
        }

        vfft(real, imag, 9, 0);

        /* lifter */
        for (i=CUT_OFF; i<FFTPNTS-CUT_OFF ;i++) {
            real[i] = 0.0; 
            imag[i] = 0.0;
        }

        /* inverse Fourier transformation */
        vfft(real, imag, 9, 1);

        XClearWindow(dp,w1);
        DrawGraph();
      }
      else if (window == w1 && button == 3) {
        return 0;
      }
    }
  }
  return 0;
}

void  DrawGraph()
{
  int     i;
  double  X1,Y1,X2,Y2;

  XGSelectXform(1);
  XGDrawBorder();

  for (i=1; i<np ;i++) {
    X1 = (double)i-1; Y1 = (double)wave[i-1];
    X2 = (double)i;   Y2 = (double)wave[i];

    XGDrawLine(X1,Y1,X2,Y2);
  }

  XGSelectXform(2);
  XGDrawBorder();

  for (i=1; i<FFTPNTS ;i++) {
    X1 = (double)i-1; Y1 = (double)real[i-1];
    X2 = (double)i;   Y2 = (double)real[i];

    XGDrawLine(X1,Y1,X2,Y2);
  }
}
