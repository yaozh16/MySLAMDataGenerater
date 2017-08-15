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

    MyBmpReader reader("map2.bmp",10);
    int width,height;
    maptype* mapData=NULL;
    int channel;
    reader.Read(width,height,mapData,channel);

    MyMapWriter writer("Test.bmp",width,height,mapData,3);
    writer.Write();

    MyTrajectoryCreater creater(100);
    MyTrajectoryCreater::MyTrajectoryMessage message=creater.Create(mapData,width,height,Point{40,40},Point{70,720});

    /*int num=0;
    for(auto iter:message.Trajectory)
    {
        cout<<"-trajectory:("<<setw(4)<<iter.x<<","<<setw(4)<<iter.y<<")"<<endl;
        num++;
    }*/

    if(message.err)
        return 1;
    writer.Set("WithTrajectory.bmp",width,height,mapData,3);
    RGBQUAD marker;
    marker.rgbBlue=255;
    marker.rgbGreen=0;
    marker.rgbRed=0;
    writer.AddMarker(message.Trajectory,marker,3);
    writer.Write();

    MyDataGenerater generater;
    vector<MyDataGenerater::Data> data=generater.Generate(width,height,mapData,message.Trajectory);
    cout<<"range number="<<data.size()<<endl;
    for(int i=0;i<data.size();i++)
    {
        cout<<"-time:\t"<<data[i].time<<endl;
        cout<<"-Odometry:\tLeft("<<data[i].odometry[0]<<")\tRight("<<data[i].odometry[1]<<")"<<endl;
        cout<<"-ray number:\t"<<data[i].size<<endl;
        system("pause");
        cout<<"-distance:"<<endl;
        for(int j=0;j<data[i].size;j++)
            if(data[i].distance[j]<MyDataGenerater::MaxRangeLength)
                cout<<"\t"<<setw(5)<<data[i].distance[j]<<endl;
            else
                cout<<"\t"<<"No reflection :equal to\\larger than  "<<MyDataGenerater::MaxRangeLength<<endl;
    }

    maptype* blankMap=new maptype[width*height];
    for(int i=0;i<width*height;i++)
        blankMap[i]=255;
    for(int i=0;i<(data.size()-10);i+=1)
    {
        generater.RangeShow(data[i],data[i].point,(data[i].direction),blankMap,127);
        generater.RangeShow(data[i],data[i].point,(data[i].direction),mapData,127);
    }
    writer.Set(("range/WithAllRange1.bmp"),width,height,blankMap,3);
    writer.AddMarker(message.Trajectory,marker,3);
    writer.Write();
    writer.Set(("range/WithAllRange2.bmp"),width,height,mapData,3);
    writer.AddMarker(message.Trajectory,marker,3);
    writer.Write();

    cout<<"All Done"<<endl;
    /*generater.RangeShow(data[20],(Point{800,30}),(MyDataGenerater::Direction{-1,0}),mapData,127);
    writer.Set(("range/WithRange.bmp"),width,height,mapData,3);
    writer.AddMarker(message.Trajectory,marker,3);
    writer.Write();*/

    /*MyDataGenerater::Data testData(0,51,180);
    testData.distance=new int[51];
    for(int i=0;i<51;i++)
        testData.distance[i]=500;
    generater.RangeShow(testData,(Point{100,30}),(MyDataGenerater::Direction{1,0}),mapData,127);
    writer.Set(("range/WithRangeTest.bmp"),width,height,mapData,3);
    writer.AddMarker(message.Trajectory,marker,3);
    writer.Write();*/

    return 0;
}
