#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
using namespace std;


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int upsample(double dx,int flag) {
            int Dx[25];
            double Wp, Gr, Gs, S,Y;
            double mr=0;
            double *x; double *y; double *dim; int sd;
            int u=0,v=0,k=0;

            Gr = x[k+2*sd]/mr;
            //Gs =  sqrt( (u - x[k])*(u - x[k]) + (v+dim[0]-x[k+sd])*(v+dim[0]-x[k+sd]) );
            Gs =  ( (u - x[k])*(u - x[k]) + (v+dim[0]-x[k+sd])*(v+dim[0]-x[k+sd]) );
            Wp = 1/sqrt(Gs*Gr);
            //mexPrintf("Filter Gain = %f\n",WGain);
            S = S + Wp;
            Y = Y + Wp*(x[k+2*sd]);
            Dx[u*(int)dim[1]  + v] = Y/S;
  return Dx[u*(int)dim[1]  + v];
}
