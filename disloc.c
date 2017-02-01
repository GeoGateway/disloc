/******************************************************************************
    Program to compute surface deformations for a point source or finite       
    source using Okada's method.  The reference for the paper is:              
        Okada, Yoshimitsu, SURFACE DEFORMATION DUE TO SHEAR AND TENSILE        
        FAULTS IN A HALF-SPACE, 1985, BSSA, vol 75, no. 4, pp 1135-1154.       
    This program will take multiple data sets and will sum the components 
    to give the total displacements, and strains.          
    
    To run the program type:  "disloc datafile outfile"  where datafile is 
    any file containing the input parameters.  If outfile is given the 
    output will be placed in that file, otherwise the output will be in a 
    file named "disloc.output.                                                

      type <CR> for data file format&
                                                                               
    Format of the data file is as follows:
    | lat of origin | lon of origin | generation parameter (1 or 0) | 
    if gen=0 follow with the number of points to be calculated.
    The next lines contain a list of the x coord | ycoord |
    if gen=1 the next line contains:                                           
   | x[0] | x increment | # to increment in x dirn | y[0] | y inc | # y dirn |
   | x coord | y coord | strike of fault |
    the line following the coordinate information contains:
   | type (1 or 0) | depth | dip | lambda | mu | u1 | u2 | u3 | len |  width |.
    The | symbol is only to distinquish input parameters in this documentation.
    The input file contains spaces between the input parameters.
    If gen=1 coordinates are generated.  They are not if gen=0.                
    If type=0 deformation for a point source is calculated.  If type=1         
    calculations are made for a finite rectangular fault.  The last two lines
    are repeated for each fault.  The results at each point are summed and
    output in ascii format

    NOTE:  axes are defined as: east = x-axis, north = y-axis.
                                                                               
      type <CR> for more &
                                                                               
    The coordinates are generated along E-W (x) and N-S (y) axes and are       
    transformed into the coordinate system given in Okada's paper.  The origin  
    is at the lower left corner of the fault in either case (as described in   
    the paper.)  After the calculations, results are transformed into the      
    original coordinate system.                                                
    NOTE:  Tilts have not been transformed back to the map system.             
                                                                               
    Andrea Donnellan, 10 May, 1989.  Revised 12 October, 1995
 ******************************************************************************
$
 */

#include   <stdio.h>
#include <stdlib.h>
#include   <math.h>
#include   <strings.h>

#define FL "%.16g"

struct data
   {
   int   type;
   int   xnum;
   int   ynum;
   int   genparm;
   float lat;
   float lon;
   float strike;
   float *xo;
   float *x;
   float xinc;
   float *yo;
   float *y;
   float yinc;
   float d;
   float delta;
   float l;
   float w;
   float lambda;
   float mu;
   float u1;
   float u2;
   float u3;
   }data;

struct ffault
   {
   double  u[9];
   double  e[18];
   double  up[9];
   double  ep[18];
   };

struct outp
   {
   double  dis[6];
   double  str[6];
   }outp;

struct sumout
   {
   double  d1,d2,d3;
   double  str1,str2,str3;
   }*sum;

static int test,j;
float  sa,ca;

//#define SOURCE  "/net/milhouse/usr1/andrea/src/disloc/disloc.c"
#define SOURCE  "./disloc.c"
#define FLF  " %8.3f "

main(argc,argv)
int   argc;
char  *argv[];
{
   int     n,i,k,kk,l,piped_in,piped_out;
   float   dumx,dumy,xp,yp;
   float   latorig,lonorig;
   double  pi,sd,cd,mat,xpr,ypr;
   char    ibuff[80],str,opt;
   FILE    *fd,*srcfile,*fo;
/**********************************************************     
 * if no arguments, and not in a pipeline, self document  *    
 * if output is redirected, don't wait for input between  *   
 * pages of documentation                                 *  
 **********************************************************/ 
   if (argc == 1 && !piped_in)                           
      {                                                 
      if((srcfile = fopen(SOURCE,"r")) == NULL)        
         {                                            
         fprintf(stderr,"Documentation not at SOURCE=");      
         fprintf(stderr,SOURCE);                             
         exit(1);                                           
         }                                                 
      while((str=fgetc(srcfile)) != '$')                  
         {                                               
         if(str == '&' && !piped_out) gets(ibuff);      
         else putc(str,stdout);                        
         }                                            
      exit(0);                                       
      }                                             
/****************************************************
 *  check if ascii file is wanted and open files    *
 ****************************************************/
   if ((argv[2] != NULL))
     {
       if ((fo = fopen(argv[2],"w")) == NULL)
	 {
	   fprintf(stderr,"     ERROR:  cannot open %s\n",argv[2]);
	   exit(1);
	 }
     }
   else
     {
       if ((fo = fopen("disloc.output","w")) == NULL)
	 {
	   fprintf(stderr,"     ERROR:  cannot open disloc.output\n");
	     exit(1);
	 }
     }
   if ((fd = fopen(argv[1],"r")) == NULL)
      {
      fprintf(stderr,"     ERROR:  cannot open %s\n",argv[1]);
      exit(1);
      }
   pi = acos(-1.0);
   n = 0;
/*  fwrite((char *)&n, sizeof(n), 1, stdout);*/
   if (!strcmp(argv[1],"testfile")) test=1;
/****************************************************************
 *  read in origin of grid and generation parameter             *
 *  then read in x and y coordinates or generate grid           *
 ****************************************************************/
   fscanf (fd,"%f %f %d",&latorig,&lonorig,&data.genparm);
   if (!data.genparm)
      {
      fscanf(fd,"%d",&data.xnum);
      data.ynum=1;
      data.x = (float *) malloc(sizeof(float)*data.xnum);
      data.y = (float *) malloc(sizeof(float)*data.xnum);
      data.xo = (float *) malloc(sizeof(float)*data.xnum);
      data.yo = (float *) malloc(sizeof(float)*data.xnum);
      sum = (struct sumout * ) calloc( data.xnum, sizeof(struct sumout));
      for (k=0; k < data.xnum; ++k)
         {
	 fscanf(fd,"%f %f",&data.xo[k],&data.yo[k]);
	 }
      }
   else
      {
      fscanf(fd,"%f %f %d %f %f %d",&xp,&data.xinc,&data.xnum,
		&yp,&data.yinc,&data.ynum);
      data.x = (float *) malloc(sizeof(float)*data.xnum*data.ynum);
      data.y = (float *) malloc(sizeof(float)*data.xnum*data.ynum);
      sum = (struct sumout * ) calloc( data.xnum*data.ynum, 
                                            sizeof(struct sumout));
      dumx = xp;  dumy = yp;
      }
   if (!test)
      {
	fprintf(fo,"%d  %d  %f  %f\n",data.xnum,data.ynum,
		latorig,lonorig);
/*      fwrite((char *)&data.xnum, sizeof(data.xnum), 1, stdout);*/
/*      fwrite((char *)&data.ynum, sizeof(data.ynum), 1, stdout);*/
/*      fwrite((char *)&latorig, sizeof(latorig), 1, stdout);*/
/*      fwrite((char *)&lonorig, sizeof(lonorig), 1, stdout);*/
      }
/*********************************************************************
 *  read in data:  lon,lat,strike                                    *
 *                 type,x,y,depth,dip,length,width,lambda,mu,u1,u2,u3*
 *                 type,x,y,  d ,delta, l   ,  w  ,lambda,mu,u1,u2,u3*
 *although fault origin is called lat and lon it is actually the x,y *
 *coordinates of the fault in relation to the origin of the grid     *
 *********************************************************************/
   while (fscanf(fd,"%f %f %f",&data.lon,&data.lat,&data.strike) != EOF)
      {
      sa = sin(pi*(data.strike-90.0)/180.0);
      ca = cos(pi*(data.strike-90.0)/180.0);
      j = 0;
      fscanf(fd,"%d %f %f %f %f %f %f %f %f %f",&data.type,
               &data.d,&data.delta,&data.lambda,&data.mu,
               &data.u1,&data.u2,&data.u3,&data.l,&data.w);
      ++n;
      cd = cos(data.delta*pi/180.0);
      if (fabs(cd) <= 1.0e-08) cd = 0.0;
      sd = sin(data.delta*pi/180.0);
      mat = data.mu/(data.lambda + data.mu);
      if (test==1)
         printf("-------------------------\n");
      if (test)
         {
         if (!data.type)
	    printf("case %d       point source\n",n);
	 if (data.type)
	    printf("case %d       finite fault\n",n);
         printf("-------------------------\n");
         printf("x =%5.1f;  y =%5.1f;  d =%5.1f;",data.x[j],data.y[j],data.d);
	 printf("  delta =%5.1f; lambda =%5.1f; mu =%5.1f;\n",
	           data.delta,data.lambda,data.mu);
	 printf("d1 =%5.1f; d2 =%5.1f; d3 =%5.1f; l =%5.1f; w =%5.1f;\n",
	           data.u1,data.u2,data.u3,data.l,data.w);
         }
      else if (j == 0)
         {
	   /* fprintf(fo,"% 7.3f % 8.3f %8.3f\n",data.lon,data.lat,
		   data.strike);
	   fprintf(fo,
		   "%5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f\n",
		   data.d,data.delta,data.lambda,
		   data.mu,data.u1,data.u2,data.u3,data.l,data.w);
           */
	   fprintf(fo,""FL" "FL" "FL"\n",data.lon,data.lat,
		   data.strike);
	   fprintf(fo,
		   ""FL" "FL" "FL" "FL" "FL" "FL" "FL" "FL" "FL"\n",
		   data.d,data.delta,data.lambda,
		   data.mu,data.u1,data.u2,data.u3,data.l,data.w);
/*	 fwrite((char *)&data.lon, sizeof(data.lat), 1, stdout);*/
/*	 fwrite((char *)&data.lat, sizeof(data.lon), 1, stdout);*/
/*	 fwrite((char *)&data.strike, sizeof(data.strike), 1, stdout);*/
/*	 fwrite((char *)&data.d, sizeof(data.d), 1, stdout);*/
/*	 fwrite((char *)&data.delta, sizeof(data.delta), 1, stdout);*/
/*	 fwrite((char *)&data.l, sizeof(data.l), 1, stdout);*/
/*	 fwrite((char *)&data.w, sizeof(data.w), 1, stdout);*/
	 }
      for (k=0; k<data.ynum; ++k)
       for (l=0; l<data.xnum; ++l)
	 {
/*********************************************************************
 *  translate coordinates so that fault corner is the origin         *
 *  and rotate so that x-axis parallels strike of the fault
 *********************************************************************/
	 if (data.genparm)
	    {
            xp = dumx + (float)l * data.xinc - data.lon; 
            yp = dumy + (float)k * data.yinc - data.lat; 
	    data.x[j] = xp*ca-yp*sa;
	    data.y[j] = xp*sa+yp*ca;
	    }
         else if (!data.genparm)
            {
            xp = data.xo[l] - data.lon;
            yp = data.yo[l] - data.lat;
	    data.x[j] = xp*ca-yp*sa;
   	    data.y[j] = xp*sa+yp*ca;
            }
         if (test)
	    printf("-------------------------\n");
/* point source  */
         if (!data.type)
            point(sd,cd,pi,mat,fo);
/*  finite fault  */
         else if (data.type == 1)
            fault(sd,cd,pi,mat,fo);
         else
	    if (test==1)
	       {
	       printf("\7specify fault type\n");
	       printf("   point source:  0\n   finite fault:  1\n");
               printf("-------------------------\n\n");
	       }
	 ++j;
         }

      }
   j = 0;
   fprintf(fo,"  x      y       ux        uy        uz        ");
   fprintf(fo,"exx       exy       eyy\n");
   for (k=0; k < data.ynum; ++k)
      for (l=0; l < data.xnum; ++l)
         {
         xpr =  data.x[j]*ca + data.y[j]*sa + data.lon;
         ypr = -data.x[j]*sa + data.y[j]*ca + data.lat;
	 fprintf(fo,"  % 6.5f  % 6.5f  % 10.3e  % 10.3e  % 10.3e  ",xpr,ypr,
		 sum[j].d1,sum[j].d2,sum[j].d3);
	 fprintf(fo,"% 10.3e  % 10.3e  % 10.3e\n",sum[j].str1,sum[j].str2,
		 sum[j].str3);
/*         fwrite((char *)&xpr, sizeof(xpr), 1, stdout);*/
/*         fwrite((char *)&ypr, sizeof(ypr), 1, stdout);*/
/*	 fwrite((char *)(&sum[j]), sizeof (struct sumout),  1, stdout);*/
         ++j;
         }
/*   rewind(stdout);*/
/*   fwrite((char *)&n, sizeof(n), 1, stdout);*/
   fclose(fo); fclose(fd);
}
/*********************************************************************/

point(sd,cd,pi,mat,fo)
double  sd,cd,pi,mat;
{
   int     i,jj;
   double  r,p,q,i01,i02,i03,i04,i05,u011,u021,u031,u012,u022,u032,u013;
   double  u023,u033,t1,t2,t3,temp,r3,r5;
   double  up11,up12,up13,up21,up22,up23;
   double  e0[18],e0pr[18],xpr,ypr;
   double  j01,j02,j03,j04,rd,rd3,rd4,jnk,jnk1,s,k01,k02,k03;
   double  x,y,d;

   x=data.x[j]; y=data.y[j]; d=data.d;
   r = (sqrt(x*x + y*y + d*d));
   p = ((y*cd) + (d*sd));
   q = ((y*sd) - (d*cd));
   temp = data.l*data.w/(2.0*pi);
   r3 = r*r*r;     r5 = r*r*r*r*r;
   rd = r+d;       rd3 = rd*rd*rd;  rd4 = rd*rd*rd*rd;
   t1 = -data.u1*temp;  t2 = -data.u2*temp;   t3 = data.u3*temp;
/*
 *  calculate and print surface displacements
 */
   i01 =mat*y*((1.0/(r*(r+d)*(r+d)))-(x*x*(3.0*r+d)/(r3*(r+d)*(r+d)*(r+d))));
   i02 =mat*x*((1.0/(r*(r+d)*(r+d)))-(y*y*(3.0*r+d)/(r3*(r+d)*(r+d)*(r+d))));
   i03 =mat*(x/(r3)) - i02;
   i04 =mat*(-x*y*(2.0*r+d)/(r3*(r+d)*(r+d)));
   i05 =mat*((1.0/(r*(r+d)))-(x*x*(2.0*r+d)/(r3*(r+d)*(r+d))));

   u011 = t1*(3.0*x*x*q/(r5) + i01*sd);
   u021 = t1*(3.0*x*y*q/(r5) + i02*sd);
   u031 = t1*(3.0*x*d*q/(r5) + i04*sd);
   u012 = t2*(3.0*x*p*q/(r5) - i03*sd*cd);
   u022 = t2*(3.0*y*p*q/(r5) - i01*sd*cd);
   u032 = t2*(3.0*d*p*q/(r5) - i05*sd*cd);
   u013 = t3*(3.0*x*q*q/(r5) - i03*sd*sd);
   u023 = t3*(3.0*y*q*q/(r5) - i01*sd*sd);
   u033 = t3*(3.0*d*q*q/(r5) - i05*sd*sd);
   up11 = u011*ca+u021*sa;    up21 = -u011*sa+u021*ca;
   up12 = u012*ca+u022*sa;    up22 = -u012*sa+u022*ca;
   up13 = u013*ca+u023*sa;    up23 = -u013*sa+u023*ca;
   if (test==1)
      {
      printf("                u01            u02            u03\n\n");
      printf("strike      % 9.3e     % 9.3e     % 9.3e\n",up11,up21,u031);
      printf("dip         % 9.3e     % 9.3e     % 9.3e\n",up12,up22,u032);
      printf("tensile     % 9.3e     % 9.3e     % 9.3e\n\n",up13,up23,u033);
      }
   else
      {
      outp.dis[0] = up11+up12+up13;
      outp.dis[1] = up21+up22+up23;
      outp.dis[2] = u031+u032+u033;
      xpr = data.x[j]*ca + data.y[j]*sa;
      ypr = -data.x[j]*sa + data.y[j]*ca;
      }
/*
 *  calculate strains
 */
   jnk = (5.0*r*r+4.0*r*d+d*d)/(r5*rd4);
   j01 = mat*((-3.0*x*y*(3.0*r+d)/(r3*rd3))+(3.0*x*x*x*y*jnk));
   j02 = mat*((1.0/r3)-(3.0/(r*rd*rd))+3.0*x*x*y*y*jnk);
   j03 = mat*((1.0/r3)-(3.0*x*x/r5))-j02;
   j04 = mat*(-3.0*x*y/r5)-j01;
   s = p*sd+q*cd;
   
   e0[0] = t1*((3.0*x*q/r5)*(2.0-5.0*x*x/(r*r))+j01*sd);
   e0[1] = t1*(-15.0*x*x*y*q/(r5*r*r)+(3.0*x*x/r5+j02)*sd);
   e0[2] = t1*((3.0*y*q/r5)*(1.0-5.0*x*x/(r*r))+j02*sd);
   e0[3] = t1*((3.0*x*q/r5)*(1.0-5.0*y*y/(r*r))+(3.0*x*y/r5+j04)*sd);
   e0[4] = t2*((3.0*p*q/r5)*(1.0-5.0*x*x/(r*r))-j03*sd*cd);
   e0[5] = t2*((3.0*x/r5)*(s-(5.0*y*p*q)/(r*r))-j01*sd*cd);
   e0[6] = t2*((-15.0*x*y*p*q/(r5*r*r))-j01*sd*cd);
   e0[7] = t2*((3.0*p*q/r5)*(1.0-5.0*y*y/(r*r))+3.0*y*s/r5-j02*sd*cd);
   e0[8] = t3*((3.0*q*q/r5)*(1.0-5.0*x*x/(r*r))-j03*sd*sd);
   e0[9] = t3*((3.0*x*q/r5)*(2.0*sd-5.0*y*q/(r*r))-j01*sd*sd);
   e0[10] = t3*((-15.0*x*y*q*q/(r5*r*r))-j01*sd*sd);
   e0[11] = t3*((3.0*q/r5)*(q+2.0*y*sd-5.0*y*y*q/(r*r))-j02*sd*sd);
   transform(e0,e0pr);
   if (test==1)
      {
   printf("               e011           e012           e021         e022\n\n");
     printf("strike      % 9.3e     % 9.3e     % 9.3e     % 9.3e\n",e0pr[0],
		                                       e0pr[1],e0pr[2],e0pr[3]);
     printf("dip         % 9.3e     % 9.3e     % 9.3e     % 9.3e\n",e0pr[4],
                                		       e0pr[5],e0pr[6],e0pr[7]);
     printf("tensile     % 9.3e     % 9.3e     % 9.3e     % 9.3e\n\n",e0pr[8],
                               		             e0pr[9],e0pr[10],e0pr[11]);
      }
   else
      {
      outp.str[0] = e0pr[0]+e0pr[4]+e0pr[8];
      outp.str[1] = 0.5*(e0pr[1]+e0pr[2]+e0pr[5]+e0pr[6]+e0pr[9]+e0pr[10]);
      outp.str[2] = e0pr[3]+e0pr[7]+e0pr[11];
      sum[j].d1 += outp.dis[0];  sum[j].str1 += outp.str[0];
      sum[j].d2 += outp.dis[1];  sum[j].str2 += outp.str[1];
      sum[j].d3 += outp.dis[2];  sum[j].str3 += outp.str[2];
/*
      if (p1)
	 {
         fprintf(fo,"% 5.1f  % 5.1f  % 10.3e  % 10.3e  % 10.3e  ",xpr,ypr,
		     outp.dis[0],outp.dis[1],outp.dis[2]);
         fprintf(fo,"% 10.3e  % 10.3e  % 10.3e\n",outp.str[0],outp.str[1],
							   outp.str[2]);
	 }
      fwrite((char *)&xpr, sizeof(xpr), 1, stdout);
      fwrite((char *)&ypr, sizeof(ypr), 1, stdout);
      fwrite((char *)outp.dis, sizeof(*outp.dis), 3, stdout);
      fwrite((char *)outp.str, sizeof(*outp.str), 3, stdout);
      */
      } 
/*
 *   calculate tilts
 */
   jnk = (2.0*r+d)/(r3*(rd*rd));
   jnk1 = (8.0*r*r+9.0*r*d+3.0*d*d)/(r5*(rd3));
   k01 = -mat*y*(jnk-x*x*jnk1);
   k02 = -mat*x*(jnk-y*y*jnk1);
   k03 = -mat*(3.0*x*d/r5)-k02;
   e0[12] = t1*((3.0*d*q/r5)*(1.0-5.0*x*x/(r*r))+k01*sd);
   e0[13] = t1*((-15.0*x*y*d*q/(r5*r*r))+(3.0*x*d/r5+k02)*sd);
   e0[14] = t2*((-15.0*x*d*p*q/(r5*r*r))-k03*sd*cd);
   e0[15] = t2*((3.0*d/r5)*(s-5.0*y*p*q/(r*r))-k01*sd*cd);
   e0[16] = t3*((-15.0*x*d*q*q/(r5*r*r))-k03*sd*sd);
   e0[17] = t3*((3*d*q/r5)*(2.0*sd-5.0*y*q/(r*r))-k01*sd*sd);
   if (test==1)
      {
      printf("                e031           e032\n\n");
      printf("strike      % 9.3e     % 9.3e\n",e0[12],e0[13]);
      printf("dip         % 9.3e     % 9.3e\n",e0[14],e0[15]);
      printf("tensile     % 9.3e     % 9.3e\n\n",e0[16],e0[17]);
      }
}
/*********************************************************************/

fault(sd,cd,pi,mat,fo)
double  sd,cd,pi,mat;
{
   struct  ffault f;
   int     i,jj;
   double  xi,eta;
   double  p,epr[18],upr[6],xpr,ypr;
   p = data.y[j]*cd+data.d*sd;
   for (i=0; i<9; ++i) f.u[i]=0.0;
   for (i=0; i<18; ++i) f.e[i]=0.0;
   xi = data.x[j];  eta = p;
   chin(sd,cd,pi,mat,xi,eta,&f);
   for (i=0; i<9; ++i) 
      f.u[i]=f.u[i]+f.up[i];
   for (i=0; i<18; ++i)
      f.e[i]=f.e[i]+f.ep[i];
   xi = data.x[j]; eta = p-data.w;
   chin(sd,cd,pi,mat,xi,eta,&f);
   for (i=0; i<9; ++i) 
      f.u[i]=f.u[i]-f.up[i];
   for (i=0; i<18; ++i)
      f.e[i]=f.e[i]-f.ep[i];
   xi = data.x[j]-data.l; eta = p;
   chin(sd,cd,pi,mat,xi,eta,&f);
   for (i=0; i<9; ++i) 
      f.u[i]=f.u[i]-f.up[i];
   for (i=0; i<18; ++i)
      f.e[i]=f.e[i]-f.ep[i];
   xi = data.x[j]-data.l; eta = p-data.w;
   chin(sd,cd,pi,mat,xi,eta,&f);
   for (i=0; i<9; ++i) 
      f.u[i]=f.u[i]+f.up[i];
   for (i=0; i<18; ++i)
      f.e[i]=f.e[i]+f.ep[i];
   for (i=0; i < 9; ++i) 
      if (fabs(f.u[i]) <= 1.0e-08) 
	 f.u[i] = 0.0;
   upr[0] = f.u[0]*ca+f.u[1]*sa;    upr[1] = -f.u[0]*sa+f.u[1]*ca;
   upr[2] = f.u[3]*ca+f.u[4]*sa;    upr[3] = -f.u[3]*sa+f.u[4]*ca;
   upr[4] = f.u[6]*ca+f.u[7]*sa;    upr[5] = -f.u[6]*sa+f.u[7]*ca;
   for (i=0; i < 6; ++i)
      if (fabs(upr[i]) <= 1.0e-08)
	 upr[i] = 0.0;
   transform(f.e,epr);
   for (i=0; i < 18; ++i) 
      if (fabs(epr[i]) <= 1.0e-08) 
	 epr[i] = 0.0;
   
   if (test==1)
      {
      printf("                u1             u2            u3\n\n");
      printf("strike      % 9.3e     % 9.3e     % 9.3e\n",upr[0],upr[1],f.u[2]);
      printf("dip         % 9.3e     % 9.3e     % 9.3e\n",upr[2],upr[3],f.u[5]);
    printf("tensile     % 9.3e     % 9.3e     % 9.3e\n\n",upr[4],upr[5],f.u[8]);

    printf("               e11            e12            e21          e22\n\n");
      printf("strike      % 9.3e     % 9.3e     % 9.3e     % 9.3e\n",
                        	      epr[0],epr[1],epr[2],epr[3]);
      printf("dip         % 9.3e     % 9.3e     % 9.3e     % 9.3e\n",
                        	      epr[4],epr[5],epr[6],epr[7]);
      printf("tensile     % 9.3e     % 9.3e     % 9.3e     % 9.3e\n\n",
	                              epr[8],epr[9],epr[10],epr[11]);
      printf("                e031           e032\n\n");
      printf("strike      % 9.3e     % 9.3e\n",f.e[12],f.e[13]);
      printf("dip         % 9.3e     % 9.3e\n",f.e[14],f.e[15]);
      printf("tensile     % 9.3e     % 9.3e\n\n",f.e[16],f.e[17]);
      }
   else
      {
      outp.dis[0] = upr[0]+upr[2]+upr[4];
      outp.dis[1] = upr[1]+upr[3]+upr[5];
      outp.dis[2] = f.u[2]+f.u[5]+f.u[8];
 /*     xpr = data.x[j]*ca + data.y[j]*sa;
      ypr = -data.x[j]*sa + data.y[j]*ca;*/
      outp.str[0] = epr[0]+epr[4]+epr[8];
      outp.str[1] = 0.5*(epr[1]+epr[5]+epr[9]+epr[2]+epr[6]+epr[10]);
      outp.str[2] = epr[3]+epr[7]+epr[11];
      sum[j].d1 += outp.dis[0];  sum[j].str1 += outp.str[0];
      sum[j].d2 += outp.dis[1];  sum[j].str2 += outp.str[1];
      sum[j].d3 += outp.dis[2];  sum[j].str3 += outp.str[2];
      }
}
/*********************************************************************/
chin(sd,cd,pi,mat,xi,eta,f)
struct  ffault *f;
double  sd,cd,pi,mat;
double  xi,eta;
{
   /* srpe, srpx: integers used as logic flags */
   int     i,srpe,srpx; 
   double  p,q,yt,dt,rr,xx,mat2,t1,t2,t3;
   double  at,i1,i2,i3,i4,i5,rdt,rdt2,jnk,jnk2,lg;
   double  j1,j2,j3,j4,axi,aeta,k1,k2,k3;
   double  xi2,xi3,rr3,rpe,rpx;

   p = ((data.y[j]*cd) + (data.d*sd));
   q = ((data.y[j]*sd) - (data.d*cd));
   yt = eta*cd + q*sd;
   dt = eta*sd - q*cd;
   rr = sqrt(xi*xi + eta*eta + q*q);
   xx = sqrt(xi*xi + q*q);
   mat2 = mat/2.0;
   t1 = -data.u1/(2.0*pi);  t2 = -data.u2/(2.0*pi);  t3 = data.u3/(2.0*pi);
   rdt = rr+dt; rdt2 = rdt*rdt;
   xi2 = xi*xi; xi3 = xi*xi*xi; rr3 = rr*rr*rr; rpe = rr*(rr+eta);
   rpx = rr*(rr+xi);
 
   srpe = fabs(rr+eta) <= 1.0e-08; /* eta term is singular */
   srpx = fabs(rr+xi) <= 1.0e-08;  /* xi term is singular */
/*
 *  find displacement parts
 */
   jnk2 =  q*rr <= 1.0e-8 ? 0.0 : (xi*eta)/(q*rr);
   at = (fabs(q) <= 1.0e-08) ? 0.0 : atan(jnk2);
   if (fabs(rr) <= 1.0e-8 && fabs(eta) <= 1.0e-8)
   {
      lg = 0.0;
   }
   else
   {
      lg = srpe ? -log(rr-eta) : log(rr+eta);
   }
   
   if (cd == 0.0)
      {
      /* apparently point at fault corner can generate rdt2 == 0.  Handle. */
      i1 =  rdt2 <= 1.0e-8? 0.0: -mat2*xi*q/rdt2;
      i3 = rdt2 <= 1.0e-8? mat2*(-lg) : mat2*(eta/rdt+yt*q/rdt2-lg);
      i4 = rdt2 <= 1.0e-8? 0.0: -mat*q/rdt;
      i5 = rr+dt  <= 1.0e-8? 0.0: -mat*xi*sd/(rr+dt);
      i2 = mat*(-lg)-i3;
      }
   else
      {
      jnk = (eta*(xx+q*cd)+xx*(rr+xx)*sd)/(xi*(rr+xx)*cd);
      i5 = (fabs(xi) <= 1.0e-08) ? 0.0 : (mat*2.0/cd)*atan(jnk);
      i4 = mat*(1.0/cd)*(log(rdt)-sd*lg);
      i3 = mat*((1.0/cd)*yt/rdt-lg)+sd*i4/cd;
      i2 = mat*(-lg)-i3;
      i1 = mat*(((-1.0)/cd)*(xi/rdt))-sd*i5/cd;
      }

   if(srpe && srpx )
      {
      f->up[0] = t1*(at+i1*sd);
      f->up[1] = t1*(i2*sd);
      f->up[2] = t1*(i4*sd);
      f->up[3] = (fabs(rr) <= 1.0e-8)? -t2*i3*sd*cd : t2*(q/rr - i3*sd*cd);
      f->up[4] = t2*(cd*at-i1*sd*cd);
      f->up[5] = t2*(sd*at-i5*sd*cd);
      f->up[7] = t3*(sd*at-i1*sd*sd);
      f->up[8] = t3*(-cd*at-i5*sd*sd);
      }
   else if (srpe)
      {
      f->up[0] = t1*(at+i1*sd);
      f->up[1] = t1*(i2*sd);
      f->up[2] = t1*(i4*sd);
      f->up[3] = t2*(q/rr - i3*sd*cd);
      f->up[4] = t2*((yt*q)/(rr*(rr+xi))+cd*at-i1*sd*cd);
      f->up[5] = t2*((dt*q)/(rr*(rr+xi))+sd*at-i5*sd*cd);
      f->up[7] = t3*((-dt*q)/(rr*(rr+xi))+sd*at-i1*sd*sd);
      f->up[8] = t3*((yt*q)/(rr*(rr+xi))-cd*at-i5*sd*sd);
      }
   else if (srpx)
      {
      f->up[0] = t1*((xi*q)/(rr*(rr+eta))+at+i1*sd);
      f->up[1] = t1*((yt*q)/(rr*(rr+eta))+(q*cd)/(rr+eta)+i2*sd);
      f->up[2] = t1*((dt*q)/(rr*(rr+eta))+(q*sd)/(rr+eta)+i4*sd);
      f->up[3] = t2*(q/rr - i3*sd*cd);
      f->up[4] = t2*(cd*at-i1*sd*cd);
      f->up[5] = t2*(sd*at-i5*sd*cd);
      f->up[7] = t3*(sd*((xi*q)/(rr*(rr+eta))-at)-i1*sd*sd);
      f->up[8] = t3*(cd*((xi*q)/(rr*(rr+eta))-at)-i5*sd*sd);
      }
   else
      { /* nonsingular case */
      f->up[0] = t1*((xi*q)/(rr*(rr+eta))+at+i1*sd);
      f->up[1] = t1*((yt*q)/(rr*(rr+eta))+(q*cd)/(rr+eta)+i2*sd);
      f->up[2] = t1*((dt*q)/(rr*(rr+eta))+(q*sd)/(rr+eta)+i4*sd);
      f->up[3] = t2*(q/rr - i3*sd*cd);
      f->up[4] = t2*((yt*q)/(rr*(rr+xi))+cd*at-i1*sd*cd);
      f->up[5] = t2*((dt*q)/(rr*(rr+xi))+sd*at-i5*sd*cd);
      f->up[7] = t3*((-dt*q)/(rr*(rr+xi))-sd*((xi*q)/(rr*(rr+eta))-at)-i1*sd*sd);
      f->up[8] = t3*((yt*q)/(rr*(rr+xi))+cd*((xi*q)/(rr*(rr+eta))-at)-i5*sd*sd);
      }
   
/*
 *  find strain parts
 */
   if (cd == 0.0)
      {
      k1 = (fabs(rr*rdt2) <= 1.0e-8)? 0.0 : mat*xi*q/(rr*rdt2);
      k3 = (fabs(rr*rdt) <= 1.0e-8 || fabs(rdt) <= 1.0e-8)? -1.0: mat*sd/rdt*(xi*xi/(rr*rdt)-1.0);
      if (srpe) k2 = (fabs(rr) <= 1.0e-8) ? -k3 : (mat*(-sd/rr)-k3);
      else k2 = (fabs(rr) <= 1.0e-8) ? mat*(-k3): (mat*(-sd/rr+q*cd/(rr*(rr+eta)) )- k3);
      }
   else
      {
      if (fabs(rr*rdt2) <= 1.0e-8)
         {
         k3 = 0.0;
         k2 = (-k3); 
         k1 = 0.0;

         }
      else
         {
         k3 = srpe ? (mat/cd*(-yt/(rr*rdt))) : 
			   (mat/cd*(q/(rr*(rr+eta))-yt/(rr*rdt)));
         k2 = srpe ? (mat*(-sd/rr)-k3) :
		           (mat*(-sd/rr+q*cd/(rr*(rr+eta)))-k3);
         k1 = srpe ? (mat*xi/cd*(1.0/(rr*rdt))):
			   (mat*xi/cd*(1.0/(rr*rdt)-sd/(rr*(rr+eta))));
         }
      }
   if (fabs(rr) <= 1.0e-8){
      j1 = (cd == 0.0) ? 0.0 : 
	       	      (mat/cd*(xi*xi/(rr*rdt2)-1.0/rdt)-sd*k3/cd);
      j2 = (cd == 0.0) ? 0.0:
		         (mat/cd*(xi*yt/(rr*rdt2))-sd*k1/cd);
      j3 = -j2;
      j4 = (-j1);
      axi = 0.0 ;
      aeta =  0.0;
   }
   else{
   
      j1 = (cd == 0.0) ? (mat2*q/rdt2*(2.0*xi*xi/(rr*rdt)-1.0)):
	       	      (mat/cd*(xi*xi/(rr*rdt2)-1.0/rdt)-sd*k3/cd);
      j2 = (cd == 0.0) ? (mat2*xi*sd/rdt2*(2.0*q*q/(rr*rdt)-1.0)):
		         (mat/cd*(xi*yt/(rr*rdt2))-sd*k1/cd);
      j3 = srpe ? (-j2): (mat*(-xi/(rr*(rr+eta)))-j2);
      j4 = srpe ? (mat*(-cd/rr)-j1): (mat*(-cd/rr-q*sd/(rr*(rr+eta)))-j1);
      axi = srpx ? 0.0 : (2.0*rr+xi)/(rr*rr*rr*(rr+xi)*(rr+xi));
      aeta = srpe ? 0.0 : (2.0*rr+eta)/(rr*rr*rr*(rr+eta)*(rr+eta));
   }

   /* treat the ones with no rr+xi or rr+eta removable singularities first */
   f->ep[0] = -t1*(xi2*q*aeta-j1*sd);
   if (fabs(eta*eta+q*q) <= 1.0e-8)
      f->ep[1] = fabs(rr) <= 1.0e-8 ? -(xi3*aeta+j2)*sd : 
           -t1*( -(xi3*aeta+j2)*sd);
   else
      f->ep[1] = fabs(rr) <= 1.0e-8 ? -(xi3*aeta+j2)*sd : 
           -t1*(xi3*dt/(rr3*(eta*eta+q*q))-(xi3*aeta+j2)*sd);

   f->ep[2] = fabs(rr) <= 1.0e-8 ? (xi*q*q*aeta-j2)*sd : -t1*(xi*q*cd/rr3+(xi*q*q*aeta-j2)*sd);
   f->ep[4] = fabs(rr) <= 1.0e-8 ?  -t2*j3*sd*cd : -t2*(xi*q/rr3+j3*sd*cd);
   f->ep[5] = fabs(rr) <= 1.0e-8 ?  -t2*(j1*sd*cd) : -t2*(yt*q/rr3-sd/rr+j1*sd*cd);
   f->ep[8] = -t3*(xi*q*q*aeta+j3*sd*sd);
   f->ep[9] = -t3*(-xi2*q*aeta*sd+j1*sd*sd) ;
   f->ep[10] = fabs(rr) <= 1.0e-8 ? -t3*(q*q*q*aeta*sd+j1*sd*sd): -t3*(q*q*cd/rr3+q*q*q*aeta*sd+j1*sd*sd);
   f->ep[12] =  fabs(rr) <= 1.0e-8 ? -t1*(-xi*q*q*aeta*cd+(-k1)*sd): -t1*(-xi*q*q*aeta*cd+(xi*q/rr3-k1)*sd);
   f->ep[13] = fabs(rr) <= 1.0e-8 ?-t1*((xi*xi*q*aeta*cd-k2)*sd): -t1*(dt*q*cd/rr3+(xi*xi*q*aeta*cd-sd/rr+yt*q/rr3-k2)*sd);
   f->ep[16] = fabs(rr) <= 1.0e-8 ?-t3*(-q*q*q*aeta*cd+k3*sd*sd): -t3*(q*q*sd/rr3-q*q*q*aeta*cd+k3*sd*sd);

   if(srpe && srpx)
       {
       f->ep[7] = -t2*(yt*yt*q*axi + j2*sd*cd);
       f->ep[15] = -t2*(yt*dt*q*axi + k1*sd*cd);

       } 
   else if (srpe) 
       {
          f->ep[3] = fabs(rr) <= 1.0e-8 ? -t1*(q*q*q*aeta*sd-(xi2  - j4)*sd) : -t1*(yt*q*cd/rr3+(q*q*q*aeta*sd-(xi2+  (eta*eta) )*cd/rr3- j4)*sd);
          f->ep[6] =  fabs(rr) <= 1.0e-8 ? -t2*(j1*sd*cd) :-t2*(yt*q/rr3+j1*sd*cd);
          f->ep[7] = fabs(rr) <= 1.0e-8 ?  -t2*(yt*yt*q*axi + j2*sd*cd) :-t2*(yt*yt*q*axi-(2.0*yt/(rr*(rr+xi)))*sd+ j2*sd*cd);
          f->ep[14] =fabs(rr) <= 1.0e-8 ?  -t2*(k3*sd*cd) : -t2*(dt*q/rr3+k3 *sd*cd);
          f->ep[15] = fabs(rr) <= 1.0e-8 ?-t2*(yt*dt*q*axi +k1*sd*cd) :-t2*(yt*dt*q*axi-(2.0*dt/(rr*(rr+xi)))* sd+k1*sd*cd);
       } 
    else if (srpx) 
       {
          f->ep[7] = -t2*(yt*yt*q*axi + j2*sd*cd);
          f->ep[11] = -t3*((yt*cd-dt*sd)*q*q*axi- (xi*q*q*aeta-j2)*sd*sd);
          f->ep[15] = -t2*(yt*dt*q*axi -xi*sd/rpe* sd+k1*sd*cd);
          f->ep[17] = -t3*((yt*sd+dt*cd)*q*q*axi+xi*q*q*aeta*sd*cd+ k1*sd*sd);
       } 
    else  
       {
          f->ep[3] = fabs(rr) <= 1.0e-8 ?  
             -t1*((q*q*q*aeta*sd-2.0*q*sd/rpe + j4)*sd) : 
             -t1*(yt*q*cd/rr3+(q*q*q*aeta*sd-2.0*q*sd/rpe- (xi2+(eta*eta))*cd/rr3-j4)*sd);
          f->ep[6] = fabs(rr) <= 1.0e-8 ? -t2*(yt*q/rr3+q*cd/rpe+j1*sd*cd) : -t2*(yt*q/rr3+q*cd/rpe+j1*sd*cd);
          f->ep[7] = fabs(rr) <= 1.0e-8 ? -t2*(yt*yt*q*axi-( +xi*cd/rpe) *sd+j2*sd*cd):
                                          -t2*(yt*yt*q*axi-(2.0*yt/(rr*(rr+xi))+xi*cd/rpe) *sd+j2*sd*cd);
          f->ep[11] = fabs(rr) <= 1.0e-8 ? -t3*((yt*cd-dt*sd)*q*q*axi- (xi*q*q*aeta-j2)*sd*sd):
          -t3*((yt*cd-dt*sd)*q*q*axi-q*2.0*sd*cd/(rr*(rr+xi))- (xi*q*q*aeta-j2)*sd*sd);
          f->ep[14] = fabs(rr) <= 1.0e-8 ? -t2*(+q*sd/rpe+k3*sd*cd):
           -t2*(dt*q/rr3+q*sd/rpe+k3*sd*cd);
          f->ep[15] = fabs(rr) <= 1.0e-8 ? -t2*(yt*dt*q*axi-(+xi*sd/rpe)* sd+k1*sd*cd):
           -t2*(yt*dt*q*axi-(2.0*dt/(rr*(rr+xi))+xi*sd/rpe)* sd+k1*sd*cd);
          f->ep[17] = fabs(rr) <= 1.0e-8 ? -t3*((yt*sd+dt*cd)*q*q*axi+xi*q*q*aeta*sd*cd+ k1*sd*sd): 
            -t3*((yt*sd+dt*cd)*q*q*axi+xi*q*q*aeta*sd*cd-(2.0*q/(rr*(rr+xi))- k1)*sd*sd);
       }
}

/*********************************************************************/
transform(e,epr)
double  *e,*epr;
{
   int    n;
   double exy;

   for (n=0; n < 12; n += 4)
      {
      exy = .5*(e[n+1]+e[n+2]);
      epr[n] = e[n]*ca*ca + e[n+3]*sa*sa + 2.0*exy*sa*ca;
      epr[n+1] = -(e[n]-e[n+3])*sa*ca + exy*(ca*ca-sa*sa) -
				       0.5*(e[n+2]-e[n+1]);
      epr[n+2] = 2.0*(e[n+3]-e[n])*sa*ca + 2.0*exy*(ca*ca-sa*sa)-epr[n+1];
      epr[n+3] = e[n]*sa*sa + e[n+3]*ca*ca - (e[n+1]+e[n+2])*sa*ca;
      }
}
