#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/photo.hpp>
using namespace std;
using namespace cv;
#define PIN 32
void servo(int input);
void show(int input,Mat& src,Mat& dst,int camnum);
void GetDateTime(char* datetime);
int main(int argc, char **argv)
{
    VideoCapture cap;
    cap.open("/dev/video0",CAP_V4L2);
    if(!cap.isOpened()){
        printf("Can't open Camera\n");
        return -1;
    }
    if (wiringPiSetupPhys() == -1){
        printf("fail setup\n");
        return -1;
    }
    pinMode(PIN, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS); 
    pwmSetClock(384);
    pwmSetRange(1000);
    Mat img;
    cap.read(img);
    Mat result(img.rows,img.cols,CV_8UC3);
    char key, buff, buff2, buff3, buff4;
    float deg = 75;
    int camnum=5, zoomon=0;
    int dev = open("/dev/my_upgpio", O_RDONLY);
    int dev2 = open("/dev/my_downgpio", O_RDONLY);
    int dev3 = open("/dev/my_zoomgpio",O_RDONLY);
    int dev4 = open("/dev/my_capturegpio",O_RDONLY);
    char datetime[40];
    while(1)
    {
        read(dev, &buff, 1);
        if(buff == '1')	deg=deg+0.5;
        read(dev2, &buff2, 1);
        if(buff2 == '1') deg=deg-0.5;
        if(deg<=25) deg=25;
        if(deg>=170) deg=170;
        servo(deg);
        read(dev3,&buff3,1);
        if(buff3 =='1'){
            camnum=5;
            delay(100);
            zoomon=~zoomon;
            zoomon=zoomon&1; 
        }
        cap.read(img);
        show(zoomon,img,result,camnum);
        read(dev4,&buff4,1);
        if(buff4 =='1'){
            GetDateTime(datetime);
            imwrite(datetime,result);
            delay(100);
        }
        key = waitKey(1);
        if (key == 27)break;
        else if (key == '1')camnum = 1;
        else if (key == '2')camnum = 2;
        else if (key == '3')camnum = 3;
        else if (key == '4')camnum = 4;
        else if (key == '5')camnum = 5;
        else if (key == '6')camnum = 6;
        else if (key == '7')camnum = 7;
        else if (key == '8')camnum = 8;
        else if (key == '9')camnum = 9;
    }
    return 0 ;
}

void servo(int input){ 
    pwmWrite(PIN, input); 
}

void show(int zoomon,Mat& src, Mat& dst,int camnum){
    Mat temp;
    dst = src;
    if(zoomon==0)imshow("cctv",src);
    if(zoomon==1)
    {
        resize(src,temp,Size(3*src.cols,3*src.rows));
        for(int j=0;j<src.rows;j++)  for(int i=0;i<src.cols;i++)
            dst.at<Vec3b>(j, i) = temp.at<Vec3b>(j + (camnum - 1) / 3 * src.rows, i + (camnum - 1) % 3 * src.cols);
        imshow("cctv",dst);
    }
}
void GetDateTime(char* datetime){
    struct tm *t = NULL;
    time_t curTime = time(NULL);
    t = (struct tm*)localtime(&curTime);
    sprintf(datetime,"/home/pi/Pictures/%04d%02d%02d%02d%02d%02d.bmp",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
}