# 说明

## MyTrajectoryCreater

### 1、作用

在给出概率地图后自动找出合适的轨迹（以相隔为1的vector点集表示）或者把报错信息保存在MyTrajectoryCreater::MyTrajectoryMessage结构体中

### 2、使用

构造
```
MyTrajectoryCreater();
```
寻找轨迹
```
MyTrajectoryMessage Create(maptype* mapData,int width,int height,Point originPoint,Point finalPoint);
```

## MyBmpReader

### 1、作用

利用bmp自动生成概率地图，可以自动加上边缘的墙壁

### 2、用法
构造
```
MyBmpReader(std::string fileName,int frameWallWidth=0);
```
读取（信息会保存在width、height、mapData、channel中）
```
int Read(int &width,int &height,maptype* &mapData,int &channel);
```

## MyDataGenerater

### 1、作用

构造数据集

### 2、其他
 
 自己看orz
