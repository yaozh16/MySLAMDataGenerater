#ifndef MYDATAGENERATER_H
#define MYDATAGENERATER_H
#include <string>
#include <fstream>
#include <windef.h>
#include <wingdi.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>
typedef BYTE maptype;
#define MyDebug true
#define PI 3.1415926535
//typedef struct
//{
//    double x;
//   double y;
//}Point;


class MyDataGenerater
{
private:
    int MidVal;
    int width;
    int height;
    maptype* mapData;
    bool* boolMap;
    int step;
public:
    static int MaxRangeLength;                  //Lidar的光线长度范围（所占据的格子数目）
    static double halfWheelDistance;            //轮子之间的距离的一半（所占的格子数目）
    static int autoDataSize;                    //一帧中的光线数目
    static int autoDataRangeSize;               //一帧数据的角度范围
private:
    int CreateBoolMap();
    double directionScaler(int i);
public:
    struct Direction
    {
        double cos;
        double sin;
    };
    struct Data
    {
        //以下数据仅用于调试
        Direction direction;
        Point point;
        //以上数据仅用于调试

        int time;           //时间
        unsigned int odometry[2];    //里程计读数
                                //Odomentry[0]左轮，为（平均值-偏转角*轮距一半）
                                //Odomentry[1]右轮，为（平均值+偏转角*轮距一半）
        int size;           //一次扫射有多少光线
        int rangeSize;      //角度范围
        int* distance;
        Data(int time=0,int size=autoDataSize, int rangeSize=autoDataRangeSize):time(time),size(size),rangeSize(rangeSize){;}
    };
    MyDataGenerater(int MidVal=127):MidVal(MidVal){step=10;}
    vector<Data> Generate(int width,int height,maptype* mapData, vector<Point> Trajectory);
    int RangeShow(Data data, Point point, Direction direction,  maptype * mapData,maptype setVal=127);
};

int MyDataGenerater::MaxRangeLength=500;
double MyDataGenerater::halfWheelDistance=10;
int MyDataGenerater::autoDataSize=61;
int MyDataGenerater::autoDataRangeSize=180;
vector<MyDataGenerater::Data> MyDataGenerater::Generate(int _width, int _height, maptype *_mapData, vector<Point> Trajectory)
{
    cout<<"--MyDataGenerater: Generating Data..."<<setw(4)<<0<<"%\b\b\b\b\b";
    width=_width;
    height=_height;
    mapData=_mapData;

    CreateBoolMap();                            //创建bool地图

    vector<Data> returnData;                    //用于返回的Data;
    int trajectorySize=Trajectory.size();       //停留的格子数目
    Direction moveDirection;
    Direction oldDirection;

    //计算初始运动方向角
    {
        double dx=0;
        double dy=0;
        for(int j=1;j<step+1;j++)
        {
            double scaler=directionScaler(0);
            dx+=(Trajectory[j].x-Trajectory[0].x)*scaler;
            dy+=(Trajectory[j].y-Trajectory[0].y)*scaler;
        }
        double length=sqrt(dx*dx+dy*dy);
        oldDirection.cos=dx/length;
        oldDirection.sin=dy/length;
    }
    for(int i=0;i<trajectorySize-step;i+=step)                          //每一帧
    {//i最大为trajectorySize-step-1
        cout<<setw(4)<<((100*(i+1))/(trajectorySize-step))<<"%\b\b\b\b\b";
        //构造一帧Data
        Data data;
        //默认一帧时间一定                                                                  //保存一帧时间
        data.time=step*i;
        //计算方向角                                                                       //计算方向角
        double direx=0;
        double direy=0;
        for(int j=1;j<step+1;j++)
        {//j最大为step, i+j最大为trajectorySize-1
            double scaler=directionScaler(j);
            direx+=(Trajectory[i+j].x-Trajectory[i].x)*scaler;
            direy+=(Trajectory[i+j].y-Trajectory[i].y)*scaler;
        }

        double length=sqrt(direx*direx+direy*direy);
        moveDirection.cos=direx/length;
        moveDirection.sin=direy/length;
        if(MyDebug)
        {
            data.direction.cos=moveDirection.cos;
            data.direction.sin=moveDirection.sin;
            data.point.x=Trajectory[i].x;
            data.point.y=Trajectory[i].y;
        }
        //更新里程计数据                                                                    //更新里程计数据
        {
            double sinRadChange=moveDirection.sin*oldDirection.cos-oldDirection.sin*moveDirection.cos;
            double moveLength=(Trajectory[i].x-Trajectory[i+step].x)*(Trajectory[i].x-Trajectory[i+step].x)+(Trajectory[i+step].y-Trajectory[i].y)*(Trajectory[i].y-Trajectory[i+step].y);
            moveLength=sqrt(moveLength);
            double arcsin=asin(sinRadChange);
            data.odometry[0]=moveLength-halfWheelDistance*arcsin;
            data.odometry[1]=moveLength+halfWheelDistance*arcsin;
        }
        //准备写入数据                                                                      //准备写入Lidar数据
        data.distance=new int [data.size];
        for(int j=0;j<data.size;j++)
        {
            double localRad=(double(data.rangeSize))/(double(data.size))*(j-data.size/2)/180*PI;
            Direction localRay={cos(localRad),sin(localRad)};
            Direction globalRay={localRay.cos*moveDirection.cos-localRay.sin*moveDirection.sin,
                                localRay.sin*moveDirection.cos+localRay.cos*moveDirection.sin};
            int dist;
            for(dist=1;dist<MaxRangeLength;dist++)
            {
                int x=globalRay.cos*dist+Trajectory[i].x;
                int y=globalRay.sin*dist+Trajectory[i].y;
                if(x>=0&&y>=0&&x<width&&y<height)           //防止boolMap溢出
                    if (!boolMap[x+y*width])            //存在障碍物
                        break;
            }
            data.distance[j]=dist;
        }
        returnData.push_back(data);
    }
    cout<<"Done "<<endl;
    cout<<"--MyDataGenerater: Generate() Done"<<endl;
    return returnData;
}

int MyDataGenerater::CreateBoolMap()
{
    boolMap=new bool[width*height];
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++)
            //障碍物应该为false
            boolMap[i*width+j]=(((mapData[i*width+j])<MidVal)?false:true);
}

double MyDataGenerater::directionScaler(int i)
{
    return ((5+step-i)/5);//越近越重要
}

int MyDataGenerater::RangeShow(Data data, Point point, Direction moveDirection, maptype * mapData, maptype setVal)
{
    for(int j=0;j<data.size;j++)
    {
        double localDeg=(double(data.rangeSize))/(double(data.size))*(j-data.size/2);
        double localRad=localDeg/180*PI;
        Direction localRay={cos(localRad),sin(localRad)};
        Direction globalRay={localRay.cos*moveDirection.cos-localRay.sin*moveDirection.sin,
                             localRay.sin*moveDirection.cos+localRay.cos*moveDirection.sin};
        int dist=data.distance[j];
        for(int idist=1;idist<dist;idist++)
        {
            int x=globalRay.cos*idist+point.x;
            int y=globalRay.sin*idist+point.y;
            if(x>=0&&y>=0&&x<width&&y<height)           //防止dataMap溢出
                mapData[x+y*width]=setVal;
        }
    }
}
#endif
