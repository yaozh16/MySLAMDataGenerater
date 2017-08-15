#include <iostream>
#include "MyBmpReader.h"
#include "MyMapWriter.h"
#include "MyTrajectoryCreater.h"
#include "MyDataGenerater.h"
using namespace std;
typedef BYTE maptype;


int main(int argc, char *argv[])
{
    cout << "Hello World!" << endl;

    //读取
    MyBmpReader reader("maps/map.bmp",10);
    int width,height;
    maptype* mapData=NULL;
    int channel;
    reader.Read(width,height,mapData,channel);
    //显示读取效果
    MyMapWriter writer("maps/Reverse.bmp",width,height,mapData,3);
    writer.Write();
    //构造轨迹
    MyTrajectoryCreater creater(100);
    MyTrajectoryCreater::MyTrajectoryMessage message=creater.Create(mapData,width,height,Point{40,40},Point{70,720});

    if(message.err)
        return 1;
    //画图
    writer.Set("maps/WithTrajectory.bmp",width,height,mapData,3);
    RGBQUAD marker;
    marker.rgbBlue=255;
    marker.rgbGreen=0;
    marker.rgbRed=0;
    writer.AddMarker(message.Trajectory,marker,3);
    writer.Write();
    //生成数据
    MyDataGenerater generater;
    vector<MyDataGenerater::Data> data=generater.Generate(width,height,mapData,message.Trajectory);

    maptype* blankMap=new maptype[width*height];
    for(long i=0;i<width*height;i++)
        blankMap[i]=255;
    for(auto iter:data)
    {
        generater.RangeShow(iter,iter.point,iter.direction,blankMap);
        generater.RangeShow(iter,iter.point,iter.direction,mapData);

    }
    writer.Set(("maps/WithAllRange1.bmp"),width,height,blankMap,3);
    writer.AddMarker(message.Trajectory,marker,3);
    writer.Write();
    writer.Set(("maps/WithAllRange2.bmp"),width,height,mapData,3);
    writer.AddMarker(message.Trajectory,marker,3);
    writer.Write();

    cout<<"All Done"<<endl;

    return 0;
}
