/*
By Kannagichan (kannagichan@gmail.com)
*/
#include <stdint.h>
#include "crt.h"
static unsigned char line_buf[0x8000];

/*
static inline void __attribute__((optimize("-O1"), inline)) CRT_initline2(unsigned char *dstPtr,int l,int n,int bytepixel)
{
	for(int j = 0;j < n;j+= bytepixel)
	{
		dstPtr[l+j+0] = (line_buf[j+0])&0xF8;
		dstPtr[l+j+1] = (line_buf[j+1])&0xF8;
		dstPtr[l+j+2] = (line_buf[j+2])&0xF8;
		j+= bytepixel;

		dstPtr[l+j+0] = (line_buf[j+0])|7;
		dstPtr[l+j+1] = (line_buf[j+1])|7;
		dstPtr[l+j+2] = (line_buf[j+2])|7;
	}

}

static inline void __attribute__((optimize("-O3"), inline)) CRT_drawline2(unsigned char *dstPtr,int l,int n,int bytepixel,float *fading)
{

	for(int j = 0;j < n;j+= bytepixel)
	{
		dstPtr[l+j+0] = (line_buf[j+0])*fading[0];
		dstPtr[l+j+1] = (line_buf[j+1])*fading[0];
		dstPtr[l+j+2] = (line_buf[j+2])*fading[0];
		j+= bytepixel;

		dstPtr[l+j+0] = (line_buf[j+0])*fading[1];
		dstPtr[l+j+1] = (line_buf[j+1])*fading[1];
		dstPtr[l+j+2] = (line_buf[j+2])*fading[1];
	}

}*/

static inline void __attribute__((optimize("-O3"), inline)) CRT_initline(unsigned char *dstPtr,int l,int n,int bytepixel)
{
	uint32_t *pline_buf = (uint32_t*)line_buf;
	uint32_t *pdstPtr = (uint32_t*)dstPtr;

	n = n>>2;
	l = l>>2;
	for(int j = 0;j < n;j++)
	{
		pdstPtr[l+j] = (pline_buf[j])&0xF8F8F8F8;
		j++;
		pdstPtr[l+j] = (pline_buf[j])|0x07070707;
	}

}

static inline void __attribute__((optimize("-O2"), inline)) CRT_drawline(unsigned char *dstPtr,int l,int n,int bytepixel,float *fading)
{
	float fdg1 = fading[0];
	float fdg2 = fading[1];

	for(int j = 0;j < n;j+= bytepixel)
	{
		dstPtr[l+j+0] = (line_buf[j+0])*fdg1;
		dstPtr[l+j+1] = (line_buf[j+1])*fdg1;
		dstPtr[l+j+2] = (line_buf[j+2])*fdg1;
		j+= bytepixel;

		dstPtr[l+j+0] = (line_buf[j+0])*fdg2;
		dstPtr[l+j+1] = (line_buf[j+1])*fdg2;
		dstPtr[l+j+2] = (line_buf[j+2])*fdg2;
	}

}





void __attribute__((optimize("-O3"))) CRTx22(unsigned char *srcPtr,unsigned char *dstPtr,int width, int height,int srcpitch,int pitch)
{
	const int bytepixel = 4;
	int i;

	int R = 0,G = 0,B = 0,RS,GS,BS;
	int l = 0;
	int x,y;

	for(y = 0;y < height;y++)
	{
		for(x = 0;x < width;x++)
		{

			i = (x*bytepixel) + (y*srcpitch);
			R = srcPtr[i+0];
			G = srcPtr[i+1];
			B = srcPtr[i+2];


			i+=bytepixel;
			RS = ( ((int)srcPtr[i+0]+R)/2 );
			GS = ( ((int)srcPtr[i+1]+G)/2 );
			BS = ( ((int)srcPtr[i+2]+B)/2 );


			int tmp = x*bytepixel*2;
			line_buf[tmp+0] = R;
			line_buf[tmp+1] = G;
			line_buf[tmp+2] = B;
			tmp += bytepixel;
			line_buf[tmp+0] = RS;
			line_buf[tmp+1] = GS;
			line_buf[tmp+2] = BS;

		}

		int n = bytepixel*width*2;

		CRT_initline(dstPtr,l,n,bytepixel);
		l += pitch;

		float fading[2];

		fading[0] = 1.0/1.25;
		fading[1] = 1.0/1.125;

		CRT_drawline(dstPtr,l,n,bytepixel,fading);

		l += pitch;


	}

}

void __attribute__((optimize("-O3"))) CRTx32(unsigned char *srcPtr,unsigned char *dstPtr,int width, int height,int srcpitch,int pitch)
{
	const int bytepixel = 4;
	int i;
	int R = 0,G = 0,B = 0,RS[3],GS[3],BS[3];
	int l = 0,j;
	int x,y;

	for(y = 0;y < height;y++)
	{
		for(x = 0;x < width;x++)
		{
			i = (x*bytepixel) + (y*srcpitch);
			R = srcPtr[i+0];
			G = srcPtr[i+1];
			B = srcPtr[i+2];


			i+=bytepixel;
			RS[0] = ( ((float)srcPtr[i+0]*0.3)+((float)R*0.7) );
			GS[0] = ( ((float)srcPtr[i+1]*0.3)+((float)G*0.7) );
			BS[0] = ( ((float)srcPtr[i+2]*0.3)+((float)B*0.7) );


			RS[2] = ( ((float)srcPtr[i+0]*0.7)+((float)R*0.4) );
			GS[2] = ( ((float)srcPtr[i+1]*0.7)+((float)G*0.4) );
			BS[2] = ( ((float)srcPtr[i+2]*0.7)+((float)B*0.4) );



			int tmp = x*bytepixel*4;
			line_buf[tmp+0] = R;
			line_buf[tmp+1] = G;
			line_buf[tmp+2] = B;
			line_buf[tmp+3] = 0xFF;

			for(j = 0;j < 2;j++)
			{
				line_buf[tmp+4] = RS[j];
				line_buf[tmp+5] = GS[j];
				line_buf[tmp+6] = BS[j];
				line_buf[tmp+7] = 0xFF;

				tmp += bytepixel;
			}
		}

		int n = bytepixel*width*2;

		CRT_initline(dstPtr,l,n,bytepixel);
		l += pitch;

		float fading[2];

		fading[0] = 1.0/1.25;
		fading[1] = 1.0/1.125;

		CRT_drawline(dstPtr,l,n,bytepixel,fading);

		l += pitch;
	}

}

void __attribute__((optimize("-O3"))) CRTx33(unsigned char *srcPtr,unsigned char *dstPtr,int width, int height,int srcpitch,int pitch)
{
	const int bytepixel = 4;
	int i;
	int R = 0,G = 0,B = 0,RS[3],GS[3],BS[3];
	int l = 0,j;
	int x,y;

	for(y = 0;y < height;y++)
	{
		for(x = 0;x < width;x++)
		{
			i = (x*bytepixel) + (y*srcpitch);
			R = srcPtr[i+0];
			G = srcPtr[i+1];
			B = srcPtr[i+2];


			i+=bytepixel;
			RS[0] = ( ((float)srcPtr[i+0]*0.3)+((float)R*0.7) );
			GS[0] = ( ((float)srcPtr[i+1]*0.3)+((float)G*0.7) );
			BS[0] = ( ((float)srcPtr[i+2]*0.3)+((float)B*0.7) );


			RS[2] = ( ((float)srcPtr[i+0]*0.7)+((float)R*0.4) );
			GS[2] = ( ((float)srcPtr[i+1]*0.7)+((float)G*0.4) );
			BS[2] = ( ((float)srcPtr[i+2]*0.7)+((float)B*0.4) );

			int tmp = x*bytepixel*4;
			line_buf[tmp+0] = R;
			line_buf[tmp+1] = G;
			line_buf[tmp+2] = B;
			line_buf[tmp+3] = 0xFF;

			for(j = 0;j < 2;j++)
			{
				line_buf[tmp+4] = RS[j];
				line_buf[tmp+5] = GS[j];
				line_buf[tmp+6] = BS[j];
				line_buf[tmp+7] = 0xFF;

				tmp += bytepixel;
			}
		}

		int n = bytepixel*width*4;
		CRT_initline(dstPtr,l,n,bytepixel);
		l += pitch;

		float fading[2];
		for(i = 0;i < 2;i++)
		{
			if(i == 0)
			{
				fading[0] = 1.0/1.25;
				fading[1] = 1.0/1.125;
			}
			else
			{
				fading[0] = 1.0/1.75;
				fading[1] = 1.0/1.5;
			}

			CRT_drawline(dstPtr,l,n,bytepixel,fading);

			l += pitch;
		}
	}

}


void __attribute__((optimize("-O3"))) CRTx43(unsigned char *srcPtr,unsigned char *dstPtr,int width, int height,int srcpitch,int pitch)
{
	const int bytepixel = 4;
	int i;
	int R = 0,G = 0,B = 0,RS[3],GS[3],BS[3];
	int l = 0,j;
	int x,y;

	for(y = 0;y < height;y++)
	{
		for(x = 0;x < width;x++)
		{
			i = (x*bytepixel) + (y*srcpitch);
			R = srcPtr[i+0];
			G = srcPtr[i+1];
			B = srcPtr[i+2];


			i+=bytepixel;
			RS[0] = ( ((float)srcPtr[i+0]*0.3)+((float)R*0.7) );
			GS[0] = ( ((float)srcPtr[i+1]*0.3)+((float)G*0.7) );
			BS[0] = ( ((float)srcPtr[i+2]*0.3)+((float)B*0.7) );

			RS[1] = ((int)srcPtr[i+0]+R)/2;
			GS[1] = ((int)srcPtr[i+1]+G)/2;
			BS[1] = ((int)srcPtr[i+2]+B)/2;

			RS[2] = ( ((float)srcPtr[i+0]*0.7)+((float)R*0.3) );
			GS[2] = ( ((float)srcPtr[i+1]*0.7)+((float)G*0.3) );
			BS[2] = ( ((float)srcPtr[i+2]*0.7)+((float)B*0.3) );


			int tmp = x*bytepixel*4;
			line_buf[tmp+0] = R;
			line_buf[tmp+1] = G;
			line_buf[tmp+2] = B;
			line_buf[tmp+3] = 0xFF;

			for(j = 0;j < 3;j++)
			{
				line_buf[tmp+4] = RS[j];
				line_buf[tmp+5] = GS[j];
				line_buf[tmp+6] = BS[j];
				line_buf[tmp+7] = 0xFF;

				tmp += bytepixel;
			}
		}

		int n = bytepixel*width*4;
		CRT_initline(dstPtr,l,n,bytepixel);
		l += pitch;

		float fading[2];
		for(i = 0;i < 2;i++)
		{
			if(i == 0)
			{
				fading[0] = 1.0/1.25;
				fading[1] = 1.0/1.125;
			}
			else
			{
				fading[0] = 1.0/1.75;
				fading[1] = 1.0/1.5;
			}

			CRT_drawline(dstPtr,l,n,bytepixel,fading);

			l += pitch;
		}
	}

}

void __attribute__((optimize("-O3"))) CRTx44(unsigned char *srcPtr,unsigned char *dstPtr,int width, int height,int srcpitch,int pitch)
{
	const int bytepixel = 4;
	int i;

	int R = 0,G = 0,B = 0,RS[3],GS[3],BS[3];
	int l = 0,j;
	int x,y;

	for(y = 0;y < height;y++)
	{
		for(x = 0;x < width;x++)
		{
			i = (x*bytepixel) + (y*srcpitch);
			R = srcPtr[i+0];
			G = srcPtr[i+1];
			B = srcPtr[i+2];


			i+=bytepixel;
			RS[0] = ( ((float)srcPtr[i+0]*0.3)+((float)R*0.7) );
			GS[0] = ( ((float)srcPtr[i+1]*0.3)+((float)G*0.7) );
			BS[0] = ( ((float)srcPtr[i+2]*0.3)+((float)B*0.7) );

			RS[1] = ((int)srcPtr[i+0]+R)/2;
			GS[1] = ((int)srcPtr[i+1]+G)/2;
			BS[1] = ((int)srcPtr[i+2]+B)/2;

			RS[2] = ( ((float)srcPtr[i+0]*0.7)+((float)R*0.3) );
			GS[2] = ( ((float)srcPtr[i+1]*0.7)+((float)G*0.3) );
			BS[2] = ( ((float)srcPtr[i+2]*0.7)+((float)B*0.3) );


			int tmp = x*bytepixel*4;
			line_buf[tmp+0] = R;
			line_buf[tmp+1] = G;
			line_buf[tmp+2] = B;
			line_buf[tmp+3] = 0xFF;

			for(j = 0;j < 3;j++)
			{
				line_buf[tmp+4] = RS[j];
				line_buf[tmp+5] = GS[j];
				line_buf[tmp+6] = BS[j];
				line_buf[tmp+7] = 0xFF;
				tmp += bytepixel;
			}

		}

		int n = bytepixel*width*4;
		CRT_initline(dstPtr,l,n,bytepixel);
		l += pitch;

		float fading[2];
		for(i = 0;i < 3;i++)
		{
			if(i == 0)
			{
				fading[0] = 1.0/1.25;
				fading[1] = 1.0/1.125;
			}

			if(i == 1)
			{
				fading[0] = 1.0/1.75;
				fading[1] = 1.0/1.5;
			}

			if(i == 2)
			{
				fading[0] = 1.0/2;
				fading[1] = 1.0/1.8;
			}

			CRT_drawline(dstPtr,l,n,bytepixel,fading);

			l += pitch;
		}

	}

}


void __attribute__((optimize("-O3"))) CRTx54(unsigned char *srcPtr,unsigned char *dstPtr,int width, int height,int srcpitch,int pitch)
{
	const int bytepixel = 4;
	int i;

	int R = 0,G = 0,B = 0,RS[5],GS[5],BS[5];
	int l = 0,j;
	int x,y;

	int tmp =0;

	for(y = 0;y < height;y++)
	{
		tmp = 0;
		int yp = y*srcpitch;
		for(x = 0;x < width;x++)
		{
			i = (x*bytepixel) + yp;
			float srcR,srcG,srcB,RF,GF,BF;

			RF = R = srcPtr[i+0];
			GF = G = srcPtr[i+1];
			BF = B = srcPtr[i+2];

			srcR = srcPtr[i+0];
			srcG = srcPtr[i+1];
			srcB = srcPtr[i+2];

			i+=bytepixel;
			RS[0] = ( (srcR*0.2)+(RF*0.8) );
			GS[0] = ( (srcG*0.2)+(GF*0.8) );
			BS[0] = ( (srcB*0.2)+(BF*0.8) );

			RS[1] = ( (srcR*0.4)+(RF*0.6) );
			GS[1] = ( (srcG*0.4)+(GF*0.6) );
			BS[1] = ( (srcB*0.4)+(BF*0.6) );

			RS[2] = ( (srcR*0.6)+(RF*0.4) );
			GS[2] = ( (srcB*0.6)+(GF*0.4) );
			BS[2] = ( (srcB*0.6)+(BF*0.4) );

			RS[3] = ( (srcR*0.8)+(RF*0.2) );
			GS[3] = ( (srcG*0.8)+(GF*0.2) );
			BS[3] = ( (srcB*0.8)+(BF*0.2) );

			line_buf[tmp+0] = R;
			line_buf[tmp+1] = G;
			line_buf[tmp+2] = B;
			line_buf[tmp+3] = 0xFF;
			tmp += bytepixel;

			for(j = 0;j < 4;j++)
			{
				line_buf[tmp+0] = RS[j];
				line_buf[tmp+1] = GS[j];
				line_buf[tmp+2] = BS[j];
				line_buf[tmp+3] = 0xFF;
				tmp += bytepixel;
			}

		}

		int n = bytepixel*width*5;
		CRT_initline(dstPtr,l,n,bytepixel);
		l += pitch;

		float fading[2];

		for(i = 0;i < 3;i++)
		{
			if(i == 0)
			{
				fading[0] = 1.0/1.25;
				fading[1] = 1.0/1.125;
			}

			if(i == 1)
			{
				fading[0] = 1.0/1.75;
				fading[1] = 1.0/1.5;
			}

			if(i == 2)
			{
				fading[0] = 1.0/2;
				fading[1] = 1.0/1.8;
			}

			CRT_drawline(dstPtr,l,n,bytepixel,fading);

			l += pitch;
		}

	}

}
