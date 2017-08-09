MyMapWriter
=============
一、作用
--------
输入概率地图，生成bmp格式地图

二、需要将maptype修改以适应需要
----------
maptype应该与传入的地图概率数据格式相同
```
 MyMapWriter(std::string fileName,        //输出文件名，需要带'.bmp'后缀
            int width,                    //地图宽度
            int height,                   //地图高度
            maptype* mapData,             //地图数据指针
            int channel=1,                //输出位图位数（如channel=1为8位图，channel=3为24位彩图）
            outputType type=BMP,          //暂时还没有别的输出格式
            std::string fileDirectory="") //文件地址（末尾需要带'/'）
```

三、设置轨迹
--------
使用AddTrajectory函数
可以自己设置画笔的宽度
```
int AddMarker(vector<Point> Trajectory, 
              RGBQUAD MarkerColor , 
              int MarkerWidth=2);
```
