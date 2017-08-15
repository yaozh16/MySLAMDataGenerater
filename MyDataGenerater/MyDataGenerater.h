//需要修改的参数：
//halfWheelDistance
//MaxRangeLength
//maptype
//MidVal
//step
//autoDataSize
//autoDataRangeSize
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

#define MyDebug


#ifndef STRUCT_POINT
#define STRUCT_POINT
typedef struct
{
    int x;
    int y;
}Point;
#endif

using namespace std;
class MyDataGenerater
{

private:
    int MidVal;
    int width;
    int height;
    maptype* mapData;
    bool* boolMap;
    int step;       //构造时对于路径点的挑选步长
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


        int time;           //时间
        unsigned int odometry[2];    //里程计读数
                                //Odomentry[0]左轮，为（平均值-偏转角*轮距一半）
                                //Odomentry[1]右轮，为（平均值+偏转角*轮距一半）
        int size;           //一次扫射有多少光线
        int rangeSize;      //角度范围
        int* distance;
        //以下数据仅用于调试
        Direction direction;
        Point point;
        //以上数据仅用于调试
        Data(int time=0,int size=autoDataSize, int rangeSize=autoDataRangeSize):time(time),size(size),rangeSize(rangeSize){;}
    };
    MyDataGenerater(int MidVal=127,int step=10):MidVal(MidVal),step(step){;}
    ~MyDataGenerater(){if (boolMap!=NULL) delete boolMap;}
    vector<Data> Generate(int width,int height,maptype* mapData, vector<Point> Trajectory);
    int RangeShow(Data data, Point point, Direction direction,  maptype * mapData,maptype setVal=127);
};

#endif
