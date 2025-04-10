# TAXI_KLOTSKI(开心挪挪车)

![开心挪挪车](https://i-blog.csdnimg.cn/blog_migrate/2f734149323988b553454ea807970033.png#pic_center)

## 特别说明

地图为6*6，索引从(0,0)开始到(5,5)

存储了总共五种车

- TAXI 2*1只能左右移动，目标车，初始位置在(0,2),目标位置(4,2)
- RED_CAR 2*1只能左右移动
- BLUE_CAR 1*2只能上下移动
- GREEN_CAR 3*1只能左右移动
- GREY_CAR 1*2只能上下移动

一辆车朝一个方向移动任意距离直到碰到其他车或边界算一步

## 算法分析

整体使用的是A*算法

### 容器

用了`std::set`来存放关闭列表，只有所有车位置都不一样才视作不同的图(即使步数或前置位置不一样)，因为set判断重复的条件`!a<b&&!b<a`![image-20250410225100077](C:/Users/yikar/AppData/Roaming/Typora/typora-user-images/image-20250410225100077.png)

![image-20250410225330868](https://rskjyikar666.oss-cn-shanghai.aliyuncs.com/I:/%E5%9B%BE%E5%BA%8A/A/20250410225330929.png)

用了`std::std::priority_queue`来存放模拟小顶堆，每次取出代价最小的结点进行遍历

![image-20250410225745748](https://rskjyikar666.oss-cn-shanghai.aliyuncs.com/I:/%E5%9B%BE%E5%BA%8A/A/20250410225745800.png)

![image-20250410225845054](https://rskjyikar666.oss-cn-shanghai.aliyuncs.com/I:/%E5%9B%BE%E5%BA%8A/A/20250410225845116.png)

代价函数

$F(x)=当前已经走的步数*100+目标车离出库的最短距离*60+这条路上挡路的车数量*30$

用了`std::stack`来存放结果路径，如果没有解则为空，先进的是步数大的

![image-20250410230102426](https://rskjyikar666.oss-cn-shanghai.aliyuncs.com/I:/%E5%9B%BE%E5%BA%8A/A/20250410230102467.png)

### A*算法

![image-20250410230254426](https://rskjyikar666.oss-cn-shanghai.aliyuncs.com/I:/%E5%9B%BE%E5%BA%8A/A/20250410230254538.png)

1. 传入start<MAP>,将其加入开放和关闭列表
2. 每次取出开放列表中代价最小的结点
3. 判断目标车是否可以直接出库，如果是则把每步结果推入`path`并且返回`true`
4. 如果否则直接调用`move_all`函数获得这张图通过朝一个方向移动任一辆车(也就是花一步)可以获得的所有衍生图，存入一个`std::vector`中
5. 对std::vector中所有地图判断是否已经遍历过(利用关闭列表)，如果没有遍历过则同时加入开放和关闭列表
6. 直到遍历结束还没找到解，path中为空且返回`false`
