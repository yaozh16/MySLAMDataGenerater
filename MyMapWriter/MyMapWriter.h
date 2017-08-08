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
typedef char maptype;
typedef struct
{
    int x;
    int y;
}Point;
class MyMapWriter
{

public:
    enum outputType {BMP};
    MyMapWriter(std::string fileName,int width,int height,maptype* mapData,int channel=1,outputType type=BMP,std::string fileDirectory=""){
        Set(fileName,width,height,mapData,channel,type,fileDirectory);
        Trajectory.clear();
    }
    int Set(std::string fileName,int width,int height,maptype* mapData,int channel=1,outputType type=BMP,std::string fileDirectory="");
    int Write();
    int AddMarker(vector<Point> _Trajectory, RGBQUAD MarkerColor);
private:
    std::string fileName;
    std::string fileDirectory;
    int width;
    int height;
    maptype* mapData;
    vector<Point> Trajectory;
    RGBQUAD MarkerColor;
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
        return 0;
}

int MyMapWriter::AddMarker(vector<Point> _Trajectory,RGBQUAD _MarkerColor)
{
    Trajectory=_Trajectory;
    MarkerColor=_MarkerColor;
    return 0;
}

int MyMapWriter::Write()
{
    FILE *pFile;
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    int step;
    int offset;
    unsigned char pixVal = '\0';
    int i, j;
    RGBQUAD* quad;
    std::string path=fileDirectory+fileName;
    pFile = fopen(path.c_str(), "wb");
    if (!pFile)
        return 1;

    bmpFileHeader.bfType=0x4D42;

    if (channel == 3)//24位，通道，彩图
    {
        cout<<"output from 24bit map...";
        step =channel*width;
        offset = step%4;
        if (offset != 4)
        {
            step += 4-offset;
        }

        bmpFileHeader.bfSize =height*step + 54;
        bmpFileHeader.bfReserved1 = 0;
        bmpFileHeader.bfReserved2 = 0;
        bmpFileHeader.bfOffBits = 54;
        fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);

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
        fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);
        cout<<"setting pixels..."<<endl;
        cout<<"output "<<fileName<<"..."<<std::setw(4)<<100<<"% left...";
        for(int tem=0;tem<13;tem++)
            cout<<"\b";
        for (i=height-1; i>-1; i--)
        {
            cout<<std::setw(4)<<int(double(i+1)/double(height)*100)<<"% left...";
            for (j=0; j<width; j++)
            {
                pixVal = mapData[i* width+j];
                fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
                fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
                fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
            }
            if (offset!=0)
            {
                for (j=0; j<offset; j++)
                {
                    pixVal = 0;
                    fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
                }
            }
            for(int tem=0;tem<13;tem++)
                cout<<"\b";
        }
        cout<<"Done."<<endl;
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
        fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);

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
        fwrite(&bmpInfoHeader, sizeof( BITMAPINFOHEADER), 1, pFile);

        cout<<"setting color bands...";
        quad = (RGBQUAD*)malloc(sizeof(RGBQUAD)*256);
        for (i=0; i<256; i++)
        {
            quad[i].rgbBlue = i;
            quad[i].rgbGreen = i;
            quad[i].rgbRed = i;
            quad[i].rgbReserved = 0;
        }
        fwrite(quad, sizeof(RGBQUAD), 256, pFile);
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
                fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
            }
            if (offset!=0)
            {
                for (j=0; j<offset; j++)
                {
                    pixVal = 0;
                    fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
                }
            }
            for(int tem=0;tem<13;tem++)
                cout<<"\b";
        }
        cout<<"Done."<<endl;
    }
    fclose(pFile);
    return 0;
}

#endif // MYMAPWRITER_H
