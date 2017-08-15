#ifndef MY_TRAJECTORY_CREATER_H
#define MY_TRAJECTORY_CREATER_H
#include <vector>
#include <windef.h>
#include <iostream>
#include <iomanip>
using maptype=BYTE;



class MyTrajectoryCreater
{
private:
    int CreateBoolMap();
    int ExpandMap();
    int Reachable(bool* boolMap);
    void ReleaseMaps();
public:
    void SetMaxAvoidRadius(int radius){maxAvoidRadius=radius;}
    struct MyTrajectoryMessage
    {
        int step;                   //总步数，等于Trajectory.size()
        bool err;                   //是否出错                  
        Point Pfinal;               //终点
        Point Pstart;               //起点
        int AvoidRadius;            //回避半径
        vector<Point> Trajectory;   //路径
    };
    MyTrajectoryCreater(int maxAvoidRadius=10,int midValue=127):maxAvoidRadius(maxAvoidRadius),midValue(midValue){height=0;stepMap=NULL;report.err=false;}
    ~MyTrajectoryCreater(){ReleaseMaps();}
    MyTrajectoryMessage Create(maptype* mapData,int width,int height,Point startPoint,Point finalPoint);
private:
    MyTrajectoryMessage report;
    maptype* pmapData;
    int maxAvoidRadius;     //最大需要回避的半径
    int midValue;           //区分障碍物与空地的阈值
    bool* boolMap1;          //地图
    bool* boolMap2;        //布尔地图
    unsigned short* stepMap;           //标记步数使用的地图
    int width;
    int height;

    struct Swifter  //扫描器
    {
        int Start;
        int End;
        int Step;
        int Scaler;
        Swifter(int Start=0,int End=0,int Step=0,int Scaler=0):Start(Start),End(End),Step(Step),Scaler(Scaler){;}
        void ShowContent(){cout<<"(start:"<<setw(4)<<Start<<", end:"<<setw(4)<<End<<", step:"<<setw(4)<<Step<<", scaler:"<<setw(4)<<Scaler<<")"<<endl;}
    };
    struct Swifters //含有x,y两个方向的扫描器
    {
        Swifter iSw;
        Swifter jSw;
        void ShowContent(){cout<<"i:";iSw.ShowContent();cout<<"j:";jSw.ShowContent();}
    };
    vector<Swifters> MySw;
    void AutoConfigSwifter();

    static Point direction[4];
};

MyTrajectoryCreater::MyTrajectoryMessage MyTrajectoryCreater::Create(maptype *_mapData,int _width,int _height,Point p_start,Point p_final)
{
    pmapData=_mapData;
    width=_width;
    height=_height;
    report.AvoidRadius=0;
    report.Pfinal=p_final;
    report.Pstart=p_start;

    CreateBoolMap();
    AutoConfigSwifter();
    cout<<"--TrajectoryCreater: Looking for Trajectory...";
    while(1)
    {
        if(Reachable(boolMap2)==0)
            break;
        for(int i=0;i<height*width;i++)
                boolMap1[i]=boolMap2[i];
        if(ExpandMap()!=0)
            break;
    }
    int step=Reachable(boolMap1);
    report.step=step;
    if(step==0)
    {
        cout<<"Fail: Unable to Reach initially"<<endl;
        report.err=true;
        return report;
    }
    else
    {
        cout<<"Succeed\r\n--TrajectoryCreater: Try To Write Trajectory..."<<setw(4)<<(100*(report.step-step)/report.step)<<"%\b\b\b\b\b";
        report.err=false;
        int px=report.Pstart.x;
        int py=report.Pstart.y;
        report.Trajectory.push_back(Point{px,py});
        while(px!=report.Pfinal.x||py!=report.Pfinal.y)
        {
            for (int d=0;d<4;d++)
            {
                int npx=px+direction[d].x;
                int npy=py+direction[d].y;
                if(npy<height&&npx<width&&npy>=0&&npx>=0)
                    if(stepMap[npy*width+npx]<stepMap[py*width+px])
                    {
                        px=npx;
                        py=npy;
                        report.Trajectory.push_back(Point{px,py});
                    }
            }
            cout<<setw(4)<<(100*(report.step-step)/report.step)<<"%\b\b\b\b\b";
        }
        cout<<"Done "<<endl;
    }
    cout<<"--TrajectoryCreater: Create() Done"<<endl;
    return report;
}

int MyTrajectoryCreater::CreateBoolMap()
{
    boolMap1=new bool[width*height];
    boolMap2=new bool[width*height];
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++)
        {

            //障碍物应该为false
            boolMap1[i*width+j]=(((pmapData[i*width+j])<midValue)?false:true);
            boolMap2[i*width+j]=(((pmapData[i*width+j])<midValue)?false:true);
        }
}

int MyTrajectoryCreater::ExpandMap()
{
    if(report.AvoidRadius==maxAvoidRadius)
        return 1;
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++)
            if(!(boolMap1[i*width+j]))            //对于某一个障碍物
                for (int d=0;d<4;d++)
                {
                    int px=j+direction[d].x;
                    int py=i+direction[d].y;
                    if(py<height&&px<width&&py>=0&&px>=0)
                        boolMap2[py*width+px]=false;
                }

    report.AvoidRadius++;
    return 0;
}

int MyTrajectoryCreater::Reachable(bool* boolMap)
{
    if(stepMap)
        delete stepMap;
    unsigned short Max=-2;
    stepMap=new unsigned short[width*height];
    for(long i=0;i<height*width;i++)
        stepMap[i]=Max;                                             //重置
    stepMap[report.Pfinal.x+report.Pfinal.y*width]=0;             //最初步
    bool changeFlag=true;
    int direNum=-1;
    while(changeFlag)
    {
        changeFlag=false;
        //扫描方向总共有四种：
        //大方向上下，小方向左右
        //大方向左右，小方向下上
        //大方向下上，小方向右左
        //大方向右左，小方向上下
        direNum=(direNum+1)%MySw.size();
        for(int i=MySw[direNum].iSw.Start;i!=MySw[direNum].iSw.End;i+=MySw[direNum].iSw.Step)
            for(int j=MySw[direNum].jSw.Start;j!=MySw[direNum].jSw.End;j+=MySw[direNum].jSw.Step)
            {
                long pos=i*(MySw[direNum].iSw.Scaler)+j*(MySw[direNum].jSw.Scaler);
                if(boolMap[pos]==true)
                {
                    int y=pos/width;
                    int x=pos%width;
                    for(int d=0;d<4;d++)
                    {
                        int px=x+direction[d].x;
                        int py=y+direction[d].y;
                        if(py<height&&px<width&&py>=0&&px>=0)
                        {
                            int posv=stepMap[y*width+x];
                            int posd=stepMap[py*width+px];
                            if (posd+1<posv)
                            {

                                changeFlag=true;
                                stepMap[pos]=posd+1;
                            }
                        }
                    }
                }
            }
    }
    unsigned short returnvalue;
    if(stepMap[report.Pstart.x+report.Pstart.y*width]!=Max)
    {
        returnvalue= stepMap[report.Pstart.x+report.Pstart.y*width];
    }else
        returnvalue= 0;
    return returnvalue;
}

void MyTrajectoryCreater::ReleaseMaps()
{
    if(boolMap1)
        delete boolMap1;
    if(boolMap2)
        delete boolMap2;
    if(stepMap)
        delete stepMap;
}

//取周边点方式有四种
Point MyTrajectoryCreater::direction[4]={{1,0},{-1,0},{0,1},{0,-1}};

//扫描方向总共有四种：
//大方向上下，小方向左右
//大方向左右，小方向下上
//大方向下上，小方向右左
//大方向右左，小方向上下

void MyTrajectoryCreater::AutoConfigSwifter()
{
    MySw.push_back(MyTrajectoryCreater::Swifters{MyTrajectoryCreater::Swifter(0,         height, 1,  width), MyTrajectoryCreater::Swifter(0,         width,  1,  1)});
    MySw.push_back(MyTrajectoryCreater::Swifters{MyTrajectoryCreater::Swifter(0,         width,  1,  1),     MyTrajectoryCreater::Swifter(height-1,  -1,     -1, width)});
    MySw.push_back(MyTrajectoryCreater::Swifters{MyTrajectoryCreater::Swifter(height-1,  -1,     -1, width), MyTrajectoryCreater::Swifter(width,     -1,     -1, 1)});
    MySw.push_back(MyTrajectoryCreater::Swifters{MyTrajectoryCreater::Swifter(width,     -1,     -1, 1),     MyTrajectoryCreater::Swifter(0,         height, 1,  width)});
}



#endif
