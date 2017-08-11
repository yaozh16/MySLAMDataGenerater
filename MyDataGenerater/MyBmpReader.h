#include <iostream>
#include <string>
#include <fstream>
#include <windef.h>
#include <wingdi.h>

typedef BYTE maptype;

using namespace std;

typedef struct
{
    int x;
    int y;
}Point;


//读取1bit位图时，值越大表示空地概率越大，障碍概率越小
class MyBmpReader
{
public:
    MyBmpReader(std::string fileName,int frameWallWidth=0):fileName(fileName),frameWallWidth(frameWallWidth){;}
    int Read(int &width,int &height,maptype* &mapData,int &channel);
private:
    void addFrameWall();

    int frameWallWidth;
    maptype ReadOnePixel(ifstream& pf);
    std::string fileName;
    int width;
    int height;
    maptype* mapData;
    int step;
    int channel;
    BITMAPFILEHEADER bmFileHeader;
    BITMAPINFOHEADER bmInfoHeader;
};
int MyBmpReader::Read(int &_width, int &_height, maptype *& _mapData, int &_channel){
    ifstream pf;
    int offset;                                  //每行中需要空读的位数
    pf.open(fileName,ios_base::in);
    cout<<"--MyBmpReader: Opening file...";
    if (!pf.is_open())
    {
        cout<<"Fail: File missing or broken"<<endl;
        return 1;                               //文件无法打开
    }
    cout<<"Done"<<endl;
    pf.read((char*)&bmFileHeader,sizeof(BITMAPFILEHEADER));
    pf.read((char*)&bmInfoHeader,sizeof(BITMAPINFOHEADER));
    if (bmInfoHeader.biCompression)
        return 2;                                //暂时不提供解压功能
    _width=width=bmInfoHeader.biWidth;
    _height=height=bmInfoHeader.biHeight;
    channel=bmInfoHeader.biBitCount /8;
    _channel=channel;
    step=bmInfoHeader.biSizeImage/height;
    if(channel==1)
        offset=step-width;                //每行中需要空读的位数
    else if(channel==3)
        offset=step-3*width;
    else if(bmInfoHeader.biBitCount==1)
        offset=8*step-width;
    pf.seekg(bmFileHeader.bfOffBits,ios_base::beg); //到达数据位置
    _mapData=new maptype[height*width];
    mapData=_mapData;
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            _mapData[i*width+j]=ReadOnePixel(pf);
        }
        for(int t=0;t<offset;t++)
            ReadOnePixel(pf);
    }
    pf.close();
    if (frameWallWidth!=0)
        addFrameWall();
    cout<<"--MyBmpReader: Read() Done"<<endl;
    return 0;
}

maptype MyBmpReader::ReadOnePixel(ifstream& pf)
{
    static BYTE tem[3]="";
    if (channel==1)
    {
        pf.read((char*)tem,1);
        return (maptype)(tem[1]);
    }
    else if(channel==3)
    {
        pf.read((char*)tem,3);
        return (maptype)(tem[0]+tem[1]+tem[2])/3;
    }
    else if (bmInfoHeader.biBitCount==1)
    {
        static char bit[8]={0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1};
        static int num=7;
        num++;
        if(num==8)
        {
            num=0;
            pf.read((char*)tem,1);                          //读了8bit
        }
        return ((tem[0]&bit[num])?maptype(-1):maptype(0));//1对应于最大概率也就是空地，0对应于最小概率也就是障碍
    }
    else
        system("Pause");
    return -1;
}

void MyBmpReader::addFrameWall()
{
    cout<<"--MyBmpReader: Adding Frame Wall...";
    for(int w=0;w<frameWallWidth;w++)
        for(int i=0;i<height;i++)
            mapData[i*width+w]=mapData[i*width+width-1-w]=maptype(0);
    for(int w=0;w<frameWallWidth;w++)
        for(int i=0;i<width;i++)
            mapData[i+w*width]=mapData[i+(height-1-w)*width]=maptype(0);
    cout<<"Done"<<endl;
}
