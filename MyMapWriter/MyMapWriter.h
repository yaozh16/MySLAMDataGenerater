#ifndef MYMAPWRITER_H
#define MYMAPWRITER_H
#include <string>
#include <fstream>
#include <windef.h>
#include <wingdi.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;
typedef unsigned short maptype;
typedef struct
{
    int x;
    int y;
} Point;
class MyMapWriter
{

public:
    enum outputType {BMP};
    MyMapWriter(std::string fileName,int width,int height,
                maptype* mapData,int channel=1,outputType type=BMP,
                std::string fileDirectory=""){
        Set(fileName,width,height,mapData,channel,type,fileDirectory);
        Trajectory.clear();
    }
    int Set(std::string fileName,int width,int height,
            maptype* mapData,int channel=1,outputType type=BMP,
            std::string fileDirectory="");
    int Write();
    int AddMarker(vector<Point> _Trajectory, RGBQUAD MarkerColor , int MarkerWidth=2);
private:
    std::string fileName;
    std::string fileDirectory;
    int width;
    int height;
    maptype* mapData;
    vector<Point> Trajectory;
    RGBQUAD MarkerColor;
    int MarkerWidth;
    outputType type;
    int step;
    int channel;
};

int MyMapWriter::Set(string _fileName,int _width,int _height,maptype *_mapData,int _channel,outputType _type,string _fileDirectory)
{
    fileName=_fileName,
    width=_width,
    height=_height,
    mapData=_mapData,
    channel=_channel,
    type=_type,
    fileDirectory=_fileDirectory;
    Trajectory.clear();
        return 0;
}

int MyMapWriter::AddMarker(vector<Point> _Trajectory,RGBQUAD _MarkerColor, int _MarkerWidth)
{
    Trajectory=_Trajectory;
    MarkerColor=_MarkerColor;
    MarkerWidth=_MarkerWidth;
    channel=3;
    return 0;
}

int MyMapWriter::Write()
{
    ofstream pf;
    streampos pixelDataOffset;
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    int step;
    int offset;
    unsigned char pixVal = '\0';
    int i, j;
    RGBQUAD* quad;
    std::string path=fileDirectory+fileName;
    pf.open(path.c_str(),ios_base::binary|ios_base::out);
    bmpFileHeader.bfType=0x4D42;

    if (channel == 3)//24位，通道，彩图
    {
        cout<<"output from 24bit map...";
        step =channel*width;
        offset = step%4;
        if (offset != 0)
        {
            step += 4-offset;
        }

        bmpFileHeader.bfSize =height*step + 54;
        bmpFileHeader.bfReserved1 = 0;
        bmpFileHeader.bfReserved2 = 0;
        bmpFileHeader.bfOffBits = 54;
        pf.write((const char*)&bmpFileHeader,sizeof(BITMAPFILEHEADER));

        bmpInfoHeader.biSize = 40;
        bmpInfoHeader.biWidth = width;
        bmpInfoHeader.biHeight = height;
        bmpInfoHeader.biPlanes = 1;
        bmpInfoHeader.biBitCount = 24;
        bmpInfoHeader.biCompression = 0;
        bmpInfoHeader.biSizeImage = height*step;
        bmpInfoHeader.biXPelsPerMeter = 0;
        bmpInfoHeader.biYPelsPerMeter = 0;
        bmpInfoHeader.biClrUsed = 0;
        bmpInfoHeader.biClrImportant = 0;
        pf.write((const char*)&bmpInfoHeader, sizeof(BITMAPINFOHEADER));

        pixelDataOffset=pf.tellp();

        cout<<"setting pixels..."<<endl;
        cout<<"output "<<fileName<<"..."<<std::setw(4)<<100<<"% left...";
        for(int tem=0;tem<13;tem++)
            cout<<"\b";
        /////////////////////
        //存储方式：对于图像
        //123
        //456   转化为789_456_123
        //789
        //      (x,y)在(x*channel+(height-y)*step)
        ///////////////////
        for (i=height-1; i>-1; i--)
        {
            cout<<std::setw(4)<<int(double(i+1)/double(height)*100)<<"% left...";
            for (j=0; j<width; j++)
            {
                pixVal = mapData[i* width+j];
                pf.write((const char*)&pixVal, sizeof(unsigned char));
                pf.write((const char*)&pixVal, sizeof(unsigned char));
                pf.write((const char*)&pixVal, sizeof(unsigned char));
            }
            if (offset!=0)
            {
                for (j=0; j<offset; j++)
                {
                    pixVal = 0;
                    pf.write((const char*)&pixVal, sizeof(unsigned char));
                }
            }
            for(int tem=0;tem<13;tem++)
                cout<<"\b";
        }
        cout<<"Done         "<<endl;
        if(!Trajectory.empty())
        {
            unsigned char Marker[3]={MarkerColor.rgbBlue,MarkerColor.rgbGreen,MarkerColor.rgbRed};
            for(auto iter:Trajectory)
            {
                for(int px=iter.x-MarkerWidth+1;px<iter.x+MarkerWidth;px++)
                    for(int py=iter.y-MarkerWidth+1;py<iter.y+MarkerWidth;py++)
                        if(px>=0&&px<width&&py>=0&&py<height)
                        {
                            int dx=px-iter.x;
                            int dy=py-iter.y;
                            if(dx*dx+dy*dy<=MarkerWidth*MarkerWidth)
                            {
                                pf.seekp(pixelDataOffset);
                                pf.seekp((px*channel+(height-py)*(step)),ios_base::cur);
                                pf.write((const char*)Marker,3*(sizeof(unsigned char)));
                            }
                        }
            }
        }
    }
    else if (channel == 1)//8位，单通道，灰度图
    {
        cout<<"8bit map...\n";
        step = width;
        offset = step%4;
        if (offset != 4)
        {
            step += 4-offset;
        }

        bmpFileHeader.bfSize = 54 + 256*4 +  width;
        bmpFileHeader.bfReserved1 = 0;
        bmpFileHeader.bfReserved2 = 0;
        bmpFileHeader.bfOffBits = 54 + 256*4;
        pf.write((const char*)&bmpFileHeader,sizeof(BITMAPFILEHEADER));

        bmpInfoHeader.biSize = 40;
        bmpInfoHeader.biWidth =  width;
        bmpInfoHeader.biHeight =  height;
        bmpInfoHeader.biPlanes = 1;
        bmpInfoHeader.biBitCount = 8;
        bmpInfoHeader.biCompression = 0;
        bmpInfoHeader.biSizeImage =  height*step;
        bmpInfoHeader.biXPelsPerMeter = 0;
        bmpInfoHeader.biYPelsPerMeter = 0;
        bmpInfoHeader.biClrUsed = 256;
        bmpInfoHeader.biClrImportant = 256;
        pf.write((const char*)&bmpInfoHeader, sizeof( BITMAPINFOHEADER));

        cout<<"setting color bands...";
        quad = (RGBQUAD*)malloc(sizeof(RGBQUAD)*256);
        for (i=0; i<256; i++)
        {
            quad[i].rgbBlue = i;
            quad[i].rgbGreen = i;
            quad[i].rgbRed = i;
            quad[i].rgbReserved = 0;
        }
        pf.write((const char*)quad, 256*sizeof(RGBQUAD));
        free(quad);
        cout<<"Done."<<endl;

        cout<<"setting pixels..."<<endl;
        cout<<"output "<<fileName<<"..."<<std::setw(4)<<100<<"% left...";
        for(int tem=0;tem<13;tem++)
            cout<<"\b";
        for (i= height-1; i>-1; i--)
        {
            for (j=0; j< width; j++)
            {
                pixVal =  mapData[i* width+j];
                pf.write((const char*)&pixVal, sizeof(unsigned char));
            }
            if (offset!=0)
            {
                for (j=0; j<offset; j++)
                {
                    pixVal = 0;
                    pf.write((const char*)&pixVal, sizeof(unsigned char));
                }
            }
            for(int tem=0;tem<13;tem++)
                cout<<"\b";
        }
        cout<<"Done."<<endl;
    }
    pf.close();
    return 0;
}

#endif // MYMAPWRITER_H
