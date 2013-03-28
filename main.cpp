
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>
#include <opencv/cvwimage.h>
#include <opencv/cxmisc.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;
using namespace cv;
double maxnum=0.0;
double minnum=0.0;
CvPoint minLocation=cvPoint(0,0);
CvPoint maxLocation=cvPoint(0,0);
CvFont fout=cvFont(1,1);
double sumnum=0;
int inum=0;
void readimage(IplImage src) 
{
  IplImage *dst=cvCreateImage(cvGetSize(&src),8,1);
  IplImage *temp=cvLoadImage("t5.jpg");
  IplImage *mask=cvCloneImage(temp);
  cvZero(mask);
  
  IplImage *temp1=cvCreateImage(cvGetSize(temp),8,1);
  cvCvtColor(temp,temp1,CV_BGR2GRAY);
  IplImage *resualt=cvCreateImage(cvSize(src.width-temp->width+1,src.height-temp->height+1),32,1);

  cvMatchTemplate(&src,temp,resualt,CV_TM_CCOEFF);
  cvNormalize(resualt,resualt,1,0,CV_MINMAX);
  cvMinMaxLoc(resualt,&minnum,&maxnum,&minLocation,&maxLocation);
  
  cvDrawRect(resualt,cvPoint(maxLocation.x-temp->width/2,maxLocation.y-temp->height/2),
    cvPoint(maxLocation.x+temp->width/2,maxLocation.y+temp->height/2),cvScalar(0,128,255),2);
  cvCvtColor(&src,dst,CV_BGR2GRAY);
  maxLocation.x=temp->width/2+maxLocation.x;
  maxLocation.y=temp->height/2+maxLocation.y;
  CvRect ROI=cvRect(maxLocation.x-temp->width/2,maxLocation.y-temp->height/2,temp->width,temp->height);
  cvDrawRect(&src,cvPoint(maxLocation.x-temp->width/2,maxLocation.y-temp->height/2),
	  cvPoint(maxLocation.x+temp->width/2,maxLocation.y+temp->height/2),cvScalar(0,128,255),2);

  cvSetImageROI(&src,ROI);
  															
  cvSmooth(dst, dst, CV_GAUSSIAN,3, 0, 0);
  cvCanny(dst,dst,55,250,3);
  cvThreshold(dst,dst,100,180,CV_THRESH_BINARY);
  //cvAdaptiveThreshold(dst,dst,50,CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,3,5);
  CvSeq *seq=NULL;
  CvMemStorage *storage=cvCreateMemStorage(0);
  cvFindContours(dst,storage,&seq,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_CODE);
  cvZero(dst);
  cvDrawContours(dst,seq,cvScalar((rand()&255), (rand()&255), (rand()&255) )
	  ,cvScalar((rand()&255), (rand()&255), (rand()&255) ),10,1,8);
  
  cvSetImageROI(dst,ROI);
  IplImage *srctemp=cvCloneImage(dst);

  cout<<cvMatchShapes(temp1,srctemp,CV_CONTOURS_MATCH_I2)<<endl;

  sumnum=cvMatchShapes(temp1,srctemp,CV_CONTOURS_MATCH_I2)+sumnum;
  inum++;
  cout<<"==============================================="<<endl;
  cout<<sumnum/inum<<endl;
  cout<<inum<<endl;
  cout<<sumnum<<endl;
  cout<<"==============================================="<<endl;
  cvAddS(&src,cvScalar(0,255,0),&src);
  cvResetImageROI(&src);
  cvResetImageROI(dst);
  
  if (cvMatchShapes(temp1,srctemp,CV_CONTOURS_MATCH_I2)>0.3)
  {
	  cvPutText(&src,"MATH",maxLocation,&fout,cvScalar(255,100,0));
	  cout<<"采用HU矩进行匹配，精度不是很高"<<endl;
  }

  cvNamedWindow("srctemp");
  cvShowImage("srctemp",srctemp);
  cvNamedWindow("temp1");
  cvShowImage("temp1",temp1);
  cvNamedWindow("src");
  cvShowImage("src",&src);
  cvNamedWindow("dst");
  cvShowImage("dst",dst);
  cvNamedWindow("temp");
  cvShowImage("temp",temp);
  cvNamedWindow("resualt");
  cvShowImage("resualt",resualt);
  char key = (char)waitKey(2); 
  switch (key) {
  case 'q':
	  return ;
  default:
	  break;
  }

}

int process(VideoCapture& capture) {	
	Mat frame;
	for (;;) {
		capture>>frame;
		if (frame.empty())
			continue;
		readimage((IplImage)frame);
		char key = (char)waitKey(1); 
		switch (key) {
		case 'q':
			return 0;
		default:
			break;
		}
	}
	return 0;
}



int _tmain(int argc, _TCHAR* argv[])
{
	VideoCapture cap("video.wmv");
	if (!cap.isOpened())
	{
		 cap.open(-1);
	}
	process(cap);
	return 0;
}
