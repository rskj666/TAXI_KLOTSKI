#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <graphics.h>
#include <string>
#include "EasyXPng.h"
bool flag = false;//判断界面类型
int nowstep = 0;//当前的步数
const int dito[4][2] = {
	{0,-1},//上
	{1,0},//右
	{0,1},//下
    {-1,0}//左
};
//图片对象定义
IMAGE i_bluecar;
IMAGE i_redcar;
IMAGE i_greencar;
IMAGE i_greycar;
IMAGE i_taxi;
IMAGE i_add;
IMAGE i_carerror;
IMAGE i_delete;
IMAGE i_directiondown;
IMAGE i_directionleft;
IMAGE i_directionright;
IMAGE i_directionup;
IMAGE i_laststep;
IMAGE i_nextstep;
IMAGE i_mapbase;
IMAGE i_steperror;
IMAGE i_title;
IMAGE i_to0;
IMAGE i_to1;
IMAGE i_redcar2;
IMAGE i_bluecar2;
IMAGE i_greencar2;
IMAGE i_greycar2;
IMAGE i_directionerror;
IMAGE i_bk;
IMAGE i_upbk;
IMAGE i_downbk;

//汽车类型
enum CAR_TYPE {
    TAXI = 0,//2*1
    RED_CAR = 1,//2*1
    BLUE_CAR = 2,//1*2
    GREEN_CAR = 3,//3*1
    GREY_CAR = 4,//1*3
};
//汽车类型转换为字符串
std::string car_type_name[] = { "TAXI", "RED_CAR", "BLUE_CAR", "GREEN_CAR", "GREY_CAR" };//汽车的类型名称
std::string type2name(CAR_TYPE type) {//汽车的类型名称
	return car_type_name[type];
}
//汽车的方向
enum CAR_DIRECTION {
    LR = 0,//左右
    UD = 1,//上下
};

struct PLACE_HOLDER {
    int Hx;//占位符对应的相对横坐标
    int Hy;//占位符对应的相对纵坐标
};

class CAR {//汽车的基类
    //汽车的基类
    //汽车的类型有出租车、红色汽车、蓝色汽车、绿色汽车、灰色汽车
    //每种类型的汽车都有一个对应的形状
    //出租车和红色汽车是2*1的形状
    //蓝色汽车是1*2的形状
    //绿色汽车是3*1的形状
    //灰色汽车是1*3的形状
public:
    CAR_TYPE Type;//汽车的类型
    CAR_DIRECTION Direction;//汽车的方向
    int Cx;//汽车对应的横坐标
    int Cy;//汽车对应的纵坐标
    std::vector<PLACE_HOLDER> Shape;//汽车的形状,每个占位符对应汽车的一个部分
    CAR(CAR_TYPE type, int cx, int cy) {//汽车的构造函数
        if (CAR_TYPE::TAXI == type) {
            init_taxi(cx, cy);
        }
        else if (CAR_TYPE::RED_CAR == type) {
            init_red_car(cx, cy);
        }
        else if (CAR_TYPE::BLUE_CAR == type) {
            init_blue_car(cx, cy);
        }
        else if (CAR_TYPE::GREEN_CAR == type) {
            init_green_car(cx, cy);
        }
        else if (CAR_TYPE::GREY_CAR == type) {
            init_grey_car(cx, cy);
        }
    }
private:
    void init_taxi(int cx, int cy) {//初始化出租车
        Cx = cx;
        Cy = cy;
        Direction = CAR_DIRECTION::LR;
        Type = CAR_TYPE::TAXI;
        Shape.push_back({ 0,0 });
        Shape.push_back({ 1,0 });
    }
    void init_red_car(int cx, int cy) {//初始化红色汽车
        Cx = cx;
        Cy = cy;
        Direction = CAR_DIRECTION::LR;
        Type = CAR_TYPE::RED_CAR;
        Shape.push_back({ 0,0 });
        Shape.push_back({ 1,0 });
    }
    void init_blue_car(int cx, int cy) {//初始化蓝色汽车
        Cx = cx;
        Cy = cy;
        Direction = CAR_DIRECTION::UD;
        Type = CAR_TYPE::BLUE_CAR;
        Shape.push_back({ 0,0 });
        Shape.push_back({ 0,1 });
    }
    void init_green_car(int cx, int cy) {//初始化绿色汽车
        Cx = cx;
        Cy = cy;
        Direction = CAR_DIRECTION::LR;
        Type = CAR_TYPE::GREEN_CAR;
        Shape.push_back({ 0,0 });
        Shape.push_back({ 1,0 });
        Shape.push_back({ 2,0 });
    }
    void init_grey_car(int cx, int cy) {//初始化灰色汽车
        Cx = cx;
        Cy = cy;
        Direction = CAR_DIRECTION::UD;
        Type = CAR_TYPE::GREY_CAR;
        Shape.push_back({ 0,0 });
        Shape.push_back({ 0,1 });
        Shape.push_back({ 0,2 });
    }
};


class MAP_BASE {//地图的基类
public:
    CAR Taxi;//存放出租车的信息
    std::vector<CAR> Cars;//存放其他汽车的信息
    //地图的数组,方便调试，演示地图情况
    //0代表空位，1代表出租车，往后的数字代表Cars的下标+2
    int step = 0;//步数

    int xindex2position(int x) const//x地图方位转换为坐标
    {
        return 10 + x * 100;
    }

    int yindex2position(int y) const//y地图方位转换为坐标
    {
        return 80 + y * 100;
    }

    MAP_BASE() : Taxi(CAR_TYPE::TAXI, 0, 2) {
    }
    virtual ~MAP_BASE() = default;
    bool is_taxi_exit() const {//判断出租车是否到达出口
        if (Taxi.Cx == 4 && Taxi.Cy == 2) {
            return true;
        }
        return false;
    }
};

class MAP : public MAP_BASE {//地图的类，计算用
public:
	std::string laststep = "";//上一步的操作
    std::set<MAP>::iterator prev;//前驱节点
    int Map[6][6] = { 0 };//地图的数组,方便调试，演示地图情况

    MAP() {
        update_map();
        //初始化地图
        //出租车在(0,1)的位置
    }

    bool add_car(CAR_TYPE AddType, int cx, int cy) {//添加汽车
        if (CAR_TYPE::TAXI != AddType && is_valid(AddType, cx, cy)) {
            Cars.push_back(CAR(AddType, cx, cy));
            update_map();
            return true;
        }
        return false;
    }

    void update_map() {//更新地图
        std::fill_n(&Map[0][0], 36, 0);
        fill_map(Taxi, 1);
        for (auto& car : Cars) {
            int index = &car - &Cars[0] + 2;
            fill_map(car, index);
        }
    }

    std::vector<MAP> move_all() const {//移动所有车辆，获得所有可能的地图
        std::vector<MAP> result;
        std::vector<MAP> TempAdd = move_taxi();
        result.insert(result.end(), TempAdd.begin(), TempAdd.end());
        for (int i = 0; i < Cars.size(); i++) {
            TempAdd = move_car(i);
            result.insert(result.end(), TempAdd.begin(), TempAdd.end());
        }
        return result;
    }

    ~MAP() override {
        //析构函数
        //释放资源
    }

    bool operator<(const MAP& other) const {//重载小于号,用于set去重
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                if (Map[i][j] != other.Map[i][j]) {
                    return Map[i][j] < other.Map[i][j];
                }
            }
        }
        return false;
    }
private:
    bool is_valid(CAR_TYPE AddType, int cx, int cy) const {//判断添加汽车是否合法
        CAR TempCar(AddType, cx, cy);
        for (auto& holder : TempCar.Shape) {
            int x = cx + holder.Hx;
            int y = cy + holder.Hy;
            if (x < 0 || x >= 6 || y < 0 || y >= 6) {
                return false;
            }
            if (Map[x][y] != 0) {
                return false;
            }
        }
        return true;
    }
    void fill_map(const CAR& TempCar, int index) {//把汽车的信息填入地图
        for (auto& holder : TempCar.Shape) {
            int x = TempCar.Cx + holder.Hx;
            int y = TempCar.Cy + holder.Hy;
            Map[x][y] = index;
        }
    }
    std::vector<MAP> move_taxi() const {//移动出租车
        std::vector<MAP> result;
        int dx = 0;
        while (true) {
            dx++;
            CAR tempcar = Taxi;
            tempcar.Cx += dx;
            if (carisvalid(1, tempcar)) {
                MAP tempmap = *this;
                tempmap.Taxi = tempcar;
                tempmap.laststep = "step ";
                tempmap.laststep += std::to_string(step+1);
                tempmap.laststep += "\n";
				tempmap.laststep += "move TAXI right\n";
                tempmap.laststep += "from (";
				tempmap.laststep += std::to_string(Taxi.Cx);
				tempmap.laststep += ",";
				tempmap.laststep += std::to_string(Taxi.Cy);
				tempmap.laststep += ") to (";
				tempmap.laststep += std::to_string(tempcar.Cx);
				tempmap.laststep += ",";
				tempmap.laststep += std::to_string(tempcar.Cy);
				tempmap.laststep += ")\n";
                tempmap.update_map();
                result.push_back(tempmap);
            }
            else {
                break;
            }
        }
        dx = 0;
        while (true) {
            dx--;
            CAR tempcar = Taxi;
            tempcar.Cx += dx;
            if (carisvalid(1, tempcar)) {
                MAP tempmap = *this;
                tempmap.Taxi = tempcar;
                tempmap.laststep = "step ";
                tempmap.laststep += std::to_string(step + 1);
                tempmap.laststep += "\n";
                tempmap.laststep += "move TAXI left\n";
                tempmap.laststep += "from (";
                tempmap.laststep += std::to_string(Taxi.Cx);
                tempmap.laststep += ",";
                tempmap.laststep += std::to_string(Taxi.Cy);
                tempmap.laststep += ") to (";
                tempmap.laststep += std::to_string(tempcar.Cx);
                tempmap.laststep += ",";
                tempmap.laststep += std::to_string(tempcar.Cy);
                tempmap.laststep += ")\n";
                tempmap.update_map();
                result.push_back(tempmap);
            }
            else {
                break;
            }
        }
        return result;
    }
    std::vector<MAP> move_car(int index) const {//移动其他车辆,使用的是Cars的下标
        if (Cars[index].Direction == CAR_DIRECTION::LR) {
            return lr_move_car(index);
        }
        else if (Cars[index].Direction == CAR_DIRECTION::UD) {
            return ud_move_car(index);
        }
        return std::vector<MAP>();
    }
    std::vector<MAP> lr_move_car(int index) const {//移动汽车
        std::vector<MAP> result;
        int dx = 0;
        while (true) {
            dx++;
            CAR tempcar = Cars[index];
            tempcar.Cx += dx;
            if (carisvalid(index + 2, tempcar)) {
                MAP tempmap = *this;
                tempmap.Cars[index] = tempcar;
				tempmap.laststep = "step ";
				tempmap.laststep += std::to_string(step + 1);
				tempmap.laststep += "\n";
				tempmap.laststep += "move ";
				tempmap.laststep += type2name(Cars[index].Type);
				tempmap.laststep += " right\n";
				tempmap.laststep += "from (";
				tempmap.laststep += std::to_string(Cars[index].Cx);
				tempmap.laststep += ",";
				tempmap.laststep += std::to_string(Cars[index].Cy);
				tempmap.laststep += ") to (";
				tempmap.laststep += std::to_string(tempcar.Cx);
				tempmap.laststep += ",";
				tempmap.laststep += std::to_string(tempcar.Cy);
				tempmap.laststep += ")\n";
                tempmap.update_map();
                result.push_back(tempmap);
            }
            else {
                break;
            }
        }
        dx = 0;
        while (true) {
            dx--;
            CAR tempcar = Cars[index];
            tempcar.Cx += dx;
            if (carisvalid(index + 2, tempcar)) {
                MAP tempmap = *this;
                tempmap.Cars[index] = tempcar;
                tempmap.Cars[index] = tempcar;
                tempmap.laststep = "step ";
                tempmap.laststep += std::to_string(step + 1);
                tempmap.laststep += "\n";
                tempmap.laststep += "move ";
                tempmap.laststep += type2name(Cars[index].Type);
                tempmap.laststep += " left\n";
                tempmap.laststep += "from (";
                tempmap.laststep += std::to_string(Cars[index].Cx);
                tempmap.laststep += ",";
                tempmap.laststep += std::to_string(Cars[index].Cy);
                tempmap.laststep += ") to (";
                tempmap.laststep += std::to_string(tempcar.Cx);
                tempmap.laststep += ",";
                tempmap.laststep += std::to_string(tempcar.Cy);
                tempmap.laststep += ")\n";
                tempmap.update_map();
                result.push_back(tempmap);
            }
            else {
                break;
            }
        }
        return result;
    }
    std::vector<MAP> ud_move_car(int index) const {//上下移动汽车
        std::vector<MAP> result;
        int dy = 0;
        while (true) {
            dy++;
            CAR tempcar = Cars[index];
            tempcar.Cy += dy;
            if (carisvalid(index + 2, tempcar)) {
                MAP tempmap = *this;
                tempmap.Cars[index] = tempcar;
				tempmap.laststep = "step ";
				tempmap.laststep += std::to_string(step + 1);
				tempmap.laststep += "\n";
				tempmap.laststep += "move ";
				tempmap.laststep += type2name(Cars[index].Type);
				tempmap.laststep += " down\n";
				tempmap.laststep += "from (";
				tempmap.laststep += std::to_string(Cars[index].Cx);
				tempmap.laststep += ",";
				tempmap.laststep += std::to_string(Cars[index].Cy);
				tempmap.laststep += ") to (";
				tempmap.laststep += std::to_string(tempcar.Cx);
				tempmap.laststep += ",";
				tempmap.laststep += std::to_string(tempcar.Cy);
				tempmap.laststep += ")\n";

                tempmap.update_map();
                result.push_back(tempmap);
            }
            else {
                break;
            }
        }
        dy = 0;
        while (true) {
            dy--;
            CAR tempcar = Cars[index];
            tempcar.Cy += dy;
            if (carisvalid(index + 2, tempcar)) {
                MAP tempmap = *this;
                tempmap.Cars[index] = tempcar;
                tempmap.Cars[index] = tempcar;
                tempmap.laststep = "step ";
                tempmap.laststep += std::to_string(step + 1);
                tempmap.laststep += "\n";
                tempmap.laststep += "move ";
                tempmap.laststep += type2name(Cars[index].Type);
                tempmap.laststep += " up\n";
                tempmap.laststep += "from (";
                tempmap.laststep += std::to_string(Cars[index].Cx);
                tempmap.laststep += ",";
                tempmap.laststep += std::to_string(Cars[index].Cy);
                tempmap.laststep += ") to (";
                tempmap.laststep += std::to_string(tempcar.Cx);
                tempmap.laststep += ",";
                tempmap.laststep += std::to_string(tempcar.Cy);
                tempmap.laststep += ")\n";
                tempmap.update_map();
                result.push_back(tempmap);
            }
            else {
                break;
            }
        }
        return result;
    }
    bool carisvalid(int index, const CAR& tempcar) const {//判断汽车是否合法,用索引和汽车对象
        for (auto& holder : tempcar.Shape) {
            int x = tempcar.Cx + holder.Hx;
            int y = tempcar.Cy + holder.Hy;
            if (x < 0 || x >= 6 || y < 0 || y >= 6) {
                return false;
            }
            if (Map[x][y] != 0 && Map[x][y] != index) {
                return false;
            }
        }
        return true;
    }

    friend class MAP_CASH;
};
MAP thismap;//地图的类，控制

class MAP_CASH:public MAP//地图的缓存
{
public:
    int chosencar = 0;
    CAR TempAddCar;//临时添加的汽车
    void tocash(const MAP& map) {//使用地图初始化
        this->Taxi = map.Taxi;
        this->Cars = map.Cars;
        this->chosencar = 0;
        update_map();
    }
	MAP returnmap() const {//返回地图
        MAP tempm;
		tempm.Taxi = this->Taxi;
		tempm.Cars = this->Cars;
        tempm.laststep = "START MAP";
        tempm.update_map();
        return tempm;
	}
	MAP_CASH() : TempAddCar(CAR_TYPE::RED_CAR, 0, 0) {
		//构造函数
		//初始化地图
		//出租车在(0,1)的位置
	}
    bool is_Mpress(int mouse_x, int mouse_y) {//判断鼠标是否点击在汽车上,并切换选择的汽车
        if (is_pressonecar(mouse_x, mouse_y, Taxi)) { chosencar = 1; return true; }
        for (int i = 0; i < Cars.size(); i++) {
			if (is_pressonecar(mouse_x, mouse_y, Cars[i])) { chosencar = i + 2; return true; }
		}
        if (mouse_x >= xindex2position(0) && mouse_x <= xindex2position(6) && mouse_y >= yindex2position(0) && mouse_y <= yindex2position(6))
            chosencar = 0;
        return false;
    }
    bool allmovecar(const int &direction)//让汽车，输入的是方向的代号移动一步 顺时针上为0
    {
	    if (chosencar== 0) return false;
		else if (chosencar == -1)
		{
            return movetempcar(TempAddCar, direction);
		}
        else if (chosencar==1)
        {
            return movecar(Taxi, direction);
        }
        else
        {
            return movecar(Cars[chosencar - 2], direction);
        }
    }
    bool deletecar()//删除被选中汽车
    {
        if (chosencar == 0) return false;
        else if (chosencar == -1)
        {
			TempAddCar = CAR(CAR_TYPE::RED_CAR, 0, 0);
            chosencar = 0;
        }
        else if (chosencar == 1)
        {
            return false;
        }
        else
        {
			Cars[chosencar - 2] = *(Cars.end() - 1);
            Cars.pop_back();
			update_map();
			chosencar = 0;
        }
    }

private:
    bool movetempcar(CAR &thiscar,const int &direction)//移动临时添加的汽车
    {
        for (auto &holder:thiscar.Shape)
        {
            int tx = thiscar.Cx+holder.Hx+ dito[direction][0];
            int ty = thiscar.Cy + holder.Hy + dito[direction][1];
            if (tx < 0 || tx >= 6 || ty < 0 || ty >= 6) return false;
        }
        int tx = thiscar.Cx + dito[direction][0];
        int ty = thiscar.Cy + dito[direction][1];
		thiscar.Cx = tx;
        thiscar.Cy = ty;
        return true;
    }
    bool movecar(CAR& thiscar, const int& direction)//移动汽车，传入的车的引用和方向的代号

    {
        if (thiscar.Direction == CAR_DIRECTION::LR && (direction == 0 || direction == 2)) return false;
        if (thiscar.Direction == CAR_DIRECTION::UD && (direction == 1 || direction == 3)) return false;
        int tx = thiscar.Cx + dito[direction][0];
        int ty = thiscar.Cy + dito[direction][1];
		CAR tempcar(thiscar.Type, tx, ty);
        if (!carisvalid(chosencar, tempcar))
        {
            return false;
        }
        thiscar.Cx = tx;
        thiscar.Cy = ty;
        update_map();
        return true;
    }
    bool is_pressonecar(int mouse_x, int mouse_y,const CAR& thiscar) const{//判断鼠标是否点击在汽车上，切换选择的汽车
		for (auto holder:thiscar.Shape)
		{
			if (mouse_x >= xindex2position(holder.Hx + thiscar.Cx) && mouse_x <= xindex2position(holder.Hx + thiscar.Cx+1) && mouse_y >= yindex2position(holder.Hy + thiscar.Cy) && mouse_y <= yindex2position(holder.Hy + thiscar.Cy+1)) {
				return true;
			}
		}
		return false;
	}
};
MAP_CASH mapcash;//地图的类，控制
int mapcmpin(const MAP& temp) {//计算代价的函数
    int tempnum = 0;
    tempnum += temp.step * 100;
    tempnum += (4 - temp.Taxi.Cx) * 60;
    for (int i = temp.Taxi.Cx + 2; i < 6; i++) {
        if (temp.Map[i][2] != 0) {
            tempnum += i * 30;
        }
    }
    return tempnum;
}
struct MAPCMP {//用于优先队列的排序
    bool operator()(const MAP& a, const MAP& b) const {
        return mapcmpin(a) > mapcmpin(b);
    }
};

//图形化组件对象定义
class ITEM {//图形化组件的基类
public:
    int lx, uy;//左上角坐标
    int width, height;//宽高
    virtual ~ITEM() {} // 虚析构函数
};

class LABEL :public ITEM {// 文字标签类
public:
    std::string text;
    int JHeigth = 10;
    int nHeight = 35;
    LABEL(int x, int y, int w, int h, const std::string& t = "") {//构造函数
        lx = x;
        uy = y;
        width = w;
        height = h;
        text = t;
    }
    void updatechar(const std::string& t) {//更新文字
        text = t;
    }
    void addchar(const std::string& t) {//添加文字
        text += t;
    }
    void clearall() {//清空文字
        text = "";
    }

    void draw() {//绘制文字
		setbkmode(TRANSPARENT);
        settextstyle(nHeight, 0, _T("Arial"));
        settextcolor(BLACK);
        int cnt = 0;
        std::vector<std::string> result = splitString(text);
        TCHAR s[40];
        for (auto line : result) {
            std::wstring wideString = std::wstring(result[cnt].begin(), result[cnt].end());
            wsprintf(s, wideString.c_str());
            outtextxy(lx, uy + cnt * (nHeight + JHeigth), s);
            cnt++;
        }
    }
private:
    std::vector<std::string> splitString(const std::string& input) {//将字符串按照换行符分割，输出的是一个字符串数组
        std::vector<std::string> result;
        std::string line;
        for (char c : input) {
            if (c == '\n') {
                result.push_back(line);
                line.clear();
            }
            else {
                line += c;
            }
        }
        if (!line.empty()) {
            result.push_back(line);
        }
        return result;
    }
};

class IMAGINATION :public ITEM// 图片类
{
public:
    IMAGE thisimage;
    IMAGINATION(int x, int y, int w, int h, IMAGE &image) {
        lx = x;
        uy = y;
        width = w;
        height = h;
		thisimage = image;
    }
    void draw() {
        putimagePng(lx, uy, &thisimage);
    }
};

class BUTTON :public ITEM {//按钮类
public:
    IMAGE* thisimage;//按钮的图片
    IMAGE* notuseimage;//如果按钮不可用，显示的图片
    bool type = 0;//按钮的类型，1表示不存在会有不可用情况
    bool is_used = false;//按钮是否可用
    BUTTON(int x, int y, int w, int h, IMAGE& t) {//构造函数
        lx = x;
        uy = y;
        width = w;
        height = h;
        thisimage = &t;
        notuseimage = &t;
		type = 0;
    }
    void setnotuse(IMAGE& t) {//设置按钮不可用的图片
		notuseimage = &t;
        type = 1;
    }
    void draw() {//绘制按钮
        setlinecolor(YELLOW);
        setlinestyle(PS_SOLID, 1);
		rectangle(lx, uy, lx + width, uy + height);
        if (is_used == false && type == 1) {
			putimagePng(lx, uy, notuseimage);
            return;
        }
        putimagePng(lx, uy,thisimage);
        // 绘制按钮
    }
    bool is_Mpress(int mouse_x, int mouse_y) {//判断鼠标是否点击在按钮上
        if (!is_used) {
            return false;
        }
        if (mouse_x >= lx && mouse_x <= lx + width && mouse_y >= uy && mouse_y <= uy + height) {
        	return true;
        }
        else {
            return false;
        }
    }
};

std::set<MAP> visited;//存放已经访问过的地图
std::priority_queue<MAP, std::vector<MAP>, MAPCMP> pq;//优先队列
std::vector<MAP> path;//存放路径

bool Astar(const MAP& start) {//A*算法(主要函数)
    pq.push(start);//将起始地图加入优先队列
    visited.insert(start);//将起始地图加入访问过的集合
    while (!pq.empty()) {
        MAP tempmap = pq.top();//取出优先队列的最小值
        pq.pop();//弹出优先队列的最小值
        if (tempmap.is_taxi_exit()) {//判断是否到达终点
            std::set<MAP>::iterator tempmapfindit = visited.find(tempmap);
            while (tempmapfindit != visited.end()) {
                path.push_back(*tempmapfindit);
                tempmapfindit = tempmapfindit->prev;
            }
            return true;
        }
        std::vector<MAP> result = tempmap.move_all();//获取所有可能的地图
        std::set<MAP>::iterator tempmapit = visited.find(tempmap);//获取当前地图在访问过的集合中的迭代器
        for (auto& nextmap : result) {
            if (visited.find(nextmap) == visited.end()) {//判断是否已经访问过
                nextmap.prev = tempmapit;
                nextmap.step = tempmap.step + 1;
                visited.insert(nextmap);
                pq.push(nextmap);
            }
        }
    }
    return false;
}

//图形化组件对象定义
class UPWIDGET :public ITEM {//上半部分的窗口
public:
    //按钮
    BUTTON *redcar;
    BUTTON *bluecar;
	BUTTON *greencar;
	BUTTON *greycar;
    BUTTON* addcar;
	BUTTON* deletecar;
    //输出框
	LABEL* label;
    UPWIDGET(int x, int y, int w, int h)//构造函数
    {
		lx = x;
		uy = y;
		width = w;
		height = h;
		redcar = new BUTTON(690, 220, 80, 40, i_redcar2);
        bluecar = new BUTTON(840, 220, 40, 80, i_bluecar2);
        greencar = new BUTTON(690, 270, 120, 40, i_greencar2);
        greycar = new BUTTON(900, 220, 40, 120, i_greycar2);
        addcar = new BUTTON(690, 360, 90, 30, i_add);
        addcar->setnotuse(i_carerror);
        deletecar = new BUTTON(860, 360, 90, 30, i_delete);
		deletecar->setnotuse(i_carerror);
        label = new LABEL(650, 90, 330, 120,"");
		redcar->is_used = true;
		bluecar->is_used = true;
		greencar->is_used = true;
		greycar->is_used = true;
    }
    ~UPWIDGET() {//析构函数
        delete redcar;
        delete bluecar;
        delete greencar;
        delete greycar;
        delete addcar;
        delete deletecar;
        delete label;
    }
    void draw() {//绘制窗口
        if (::flag==false)
        {
			redcar->draw();
            bluecar->draw();
            greencar->draw();
			greycar->draw();
			addcar->draw();
			deletecar->draw();
			label->draw();
		}
        else
        {
	        label->draw();
        }
    }
    bool update(int mouse_x,int mouse_y)//判断鼠标是否点击在按钮上
    {
        if (::flag == true) return false;//如果选择的页面不符，就不再响应鼠标事件
        if (redcar->is_Mpress(mouse_x, mouse_y)) {
			mapcash.TempAddCar = CAR(CAR_TYPE::RED_CAR, 0, 0);
            mapcash.chosencar = -1;
            return true;
		}
		else if (bluecar->is_Mpress(mouse_x, mouse_y)) {
            mapcash.TempAddCar = CAR(CAR_TYPE::BLUE_CAR, 0, 0);
			mapcash.chosencar = -1;
			return true;
		}
		else if (greencar->is_Mpress(mouse_x, mouse_y)) {
			mapcash.TempAddCar = CAR(CAR_TYPE::GREEN_CAR, 0, 0);
            mapcash.chosencar = -1;
			return true;
		}
		else if (greycar->is_Mpress(mouse_x, mouse_y)) {
			mapcash.TempAddCar = CAR(CAR_TYPE::GREY_CAR, 0, 0);
			mapcash.chosencar = -1;
			return true;
		}
		else if (addcar->is_Mpress(mouse_x, mouse_y)) {//确认添加汽车
			if (mapcash.add_car(mapcash.TempAddCar.Type, mapcash.TempAddCar.Cx, mapcash.TempAddCar.Cy))//成功添加汽车
			{
                label->updatechar("Successfully added a car\n");
				label->addchar("Car type: ");
				label->addchar(type2name(mapcash.TempAddCar.Type));
				label->addchar("\n");
				label->addchar("Car position: (");
				label->addchar(std::to_string(mapcash.TempAddCar.Cx));
				label->addchar(",");
				label->addchar(std::to_string(mapcash.TempAddCar.Cy));
				label->addchar(")");
				mapcash.chosencar = 0;
			}
            else {
				label->updatechar("Fail to add a car\n");
			}
            return true;
		}
		else if (deletecar->is_Mpress(mouse_x, mouse_y)) {//确认删除汽车
			if (mapcash.deletecar()) {//成功删除汽车
				label->updatechar("Successfully delete a car\n");
                mapcash.chosencar = 0;
			}
			else {
				label->updatechar("Fail to delete a car\n");
			}
			return true;
		}
		else {
			return false;
		}

    }
};
UPWIDGET upwidget(640, 80, 350, 320);//上半部分的窗口

class M_DOWNWIDGET :public ITEM//下半部分的窗口 编辑模式
{
public:
    //按钮
    BUTTON* direction_up;
    BUTTON* direction_down;
    BUTTON* direction_left;
    BUTTON* direction_right;
    BUTTON* to1;
    M_DOWNWIDGET(int x, int y, int w, int h)//构造函数
    {
		lx = x;
		uy = y;
		width = w;
		height = h;
		direction_up = new BUTTON(740, 430, 60, 60, i_directionup);
		direction_down = new BUTTON(740, 550, 60, 60, i_directiondown);
		direction_left = new BUTTON(680, 490, 60, 60, i_directionleft);
		direction_right = new BUTTON(800, 490, 60, 60, i_directionright);
		direction_up->setnotuse(i_directionerror);
		direction_down->setnotuse(i_directionerror);
		direction_left->setnotuse(i_directionerror);
		direction_right->setnotuse(i_directionerror);

		to1 = new BUTTON(860, 610, 110, 30, i_to1);
		to1->is_used = true;
    }
    ~M_DOWNWIDGET() {//析构函数
        delete direction_up;
        delete direction_down;
        delete direction_left;
        delete direction_right;
        delete to1;
    }
    void draw()//绘制窗口
    {
		if (::flag == true) return;
		direction_up->draw();
		direction_down->draw();
		direction_left->draw();
		direction_right->draw();
		to1->draw();
    }
    bool update(int mouse_x, int mouse_y)//判断鼠标是否点击在按钮上
    {
		if (::flag == true) return false;//如果选择的页面不符，就不再响应鼠标事件
        //移动汽车
        if (direction_up->is_Mpress(mouse_x, mouse_y))
        {
            if (mapcash.allmovecar(0))
            {
                return true;
            }
        }
		else if (direction_down->is_Mpress(mouse_x, mouse_y))
		{
			if (mapcash.allmovecar(2))
			{
                return true;
			}
		}
		else if (direction_left->is_Mpress(mouse_x, mouse_y))
		{
			if (mapcash.allmovecar(3))
			{
                return true;
			}
		}
		else if (direction_right->is_Mpress(mouse_x, mouse_y))
		{
			if (mapcash.allmovecar(1))
			{
                return true;
			}
		}
		else if (to1->is_Mpress(mouse_x, mouse_y))//切换界面到解模式
        {
		    //初始化A*列表
            nowstep = 0;
			path.clear();
            visited.clear();
            while (!pq.empty())
            {
                pq.pop();
            }
			thismap = mapcash.returnmap();
            thismap.prev = visited.end();
			if (Astar(thismap))//如果找到解
			{
				::flag = true;//切换到解模式
				mapcash.chosencar = 0;
				mapcash.update_map();
				nowstep = path.size() - 1;
                upwidget.label->updatechar("FOUND A SOLUTION\nPress the following button\nto view the solution.");
                return true;
			}
			else
			{//如果没有找到解
				upwidget.label->updatechar("THERE ARE NO\nSOLUTION");//输出没有解并保持
                return true;
			}
        }
        return false;
    }
};
M_DOWNWIDGET move_downwidget(640,410,350,270);//下半部分的窗口 编辑模式

class A_DOWNWIDGET :public ITEM//下半部分的窗口 解模式
{
public:
    //按钮
    BUTTON* last_step;
    BUTTON* next_step;
    BUTTON* to0;
    A_DOWNWIDGET(int x, int y, int w, int h)//构造函数
    {
        lx = x;
        uy = y;
        width = w;
        height = h;
        last_step = new BUTTON(670, 440, 70, 170, i_laststep);
		last_step->setnotuse(i_steperror);
        next_step = new BUTTON(760, 440, 70, 170, i_nextstep);
		next_step->setnotuse(i_steperror);
        to0 = new BUTTON(860, 610, 110, 30, i_to0);
        to0->is_used = true;
    }
    ~A_DOWNWIDGET() {//析构函数
        delete last_step;
        delete next_step;
        delete to0;
    }
    void draw() {//绘制窗口
		if (::flag == false) return;
		last_step->draw();
		next_step->draw();
		to0->draw();
    }
    int update(int mouse_x, int mouse_y)//判断鼠标是否点击在按钮,返回的操作类型，等到update函数一起操作上
    {
		if (::flag == false) return 0;//如果选择的页面不符，就不再响应鼠标事件
        if (last_step->is_Mpress(mouse_x, mouse_y))
        {
            return 1;
        }
        if (next_step->is_Mpress(mouse_x, mouse_y))
        {
            return 2;
        }
        if (to0->is_Mpress(mouse_x, mouse_y))
        {
            return 3;
        }
        else return 0;
    }

};
A_DOWNWIDGET answer_downwidget(640, 410, 350, 270);//下半部分的窗口 解模式

class MAP_DRAW : public MAP_BASE {//地图的类，绘制用 饿汉式单例
private:
	MAP_DRAW():TempAddCar(RED_CAR, 0, 0){
		//构造函数
		//初始化地图
		//出租车在(0,1)的位置
	}
    ~MAP_DRAW() override {
        //析构函数
        //释放资源
    }
    MAP_DRAW(const MAP_DRAW& other) = delete;//禁止拷贝构造函数
    MAP_DRAW& operator=(const MAP_DRAW& other) = delete;//禁止赋值运算符重载
    static MAP_DRAW WinMap;//静态成员变量

    void re(const CAR& thiscar,int &left,int &top,int &right,int &bottom)const//将汽车的坐标转换为矩形四个角坐标
    {
		for (auto holder : thiscar.Shape)
		{
			left = min(left, holder.Hx + thiscar.Cx);
			top = min(top, holder.Hy + thiscar.Cy);
			right = max(right, holder.Hx + thiscar.Cx);
			bottom = max(bottom, holder.Hy + thiscar.Cy);
		}
    }
public:
    CAR TempAddCar;//临时添加的汽车
    int is_chosen = 0;//选择的汽车
    static MAP_DRAW& getInstance() {//获取单例对象
        return WinMap;
    }
    void towins(const MAP& map) {//将MAP类型的对象转换为MAP_DRAW类型的对象
        //使用地图初始化
        Taxi = map.Taxi;
        Cars = map.Cars;
        step = map.step;
    }
    void towins(const MAP_CASH& map)//将MAP_CASH类型的对象转换为MAP_DRAW类型的对象
    {
        Taxi = map.Taxi;
        Cars = map.Cars;
        step = map.step;
		TempAddCar = map.TempAddCar;
		is_chosen = map.chosencar;
    }
    void show()//绘制地图
    {
        setlinecolor(WHITE);
        setlinestyle(PS_DASH, 5);
        putimagePng(10, 80, &i_mapbase);
        putimagePng(xindex2position(Taxi.Cx), yindex2position(Taxi.Cy), &i_taxi);//绘制出租车
        for (auto& car : Cars) {//绘制汽车
			putimagePng(xindex2position(car.Cx), yindex2position(car.Cy), showothercar(car.Type));
		}
        if (is_chosen==-1) {//如果选择的是临时添加的汽车
            int left = 6;
            int top = 6;
            int right = 0;
			int bottom = 0;
			re(TempAddCar, left, top, right, bottom);
			putimagePng(xindex2position(TempAddCar.Cx), yindex2position(TempAddCar.Cy), showothercar(TempAddCar.Type));
            setfillstyle(BS_HATCHED, HS_FDIAGONAL);
            setbkmode(1);
            setfillcolor(WHITE);
			fillrectangle(xindex2position(left), yindex2position(top), xindex2position(right + 1), yindex2position(bottom + 1));
		}
        else if (is_chosen==1)//如果选择的是出租车
        {
            int left = 6;
            int top = 6;
            int right = 0;
            int bottom = 0;
            re(Taxi, left, top, right, bottom);
            rectangle(xindex2position(left), yindex2position(top), xindex2position(right + 1), yindex2position(bottom + 1));
        }
		else if (is_chosen - 2 < Cars.size())//如果选择的是其他汽车
		{
			int left = 6;
			int top = 6;
			int right = 0;
			int bottom = 0;
			re(Cars[is_chosen - 2], left, top, right, bottom);
			rectangle(xindex2position(left), yindex2position(top), xindex2position(right + 1), yindex2position(bottom + 1));
		}
        return;
    }
	IMAGE* showothercar(CAR_TYPE type) {//根据汽车类型返回对应的图片
		switch (type) {
		case CAR_TYPE::RED_CAR:
			return &i_redcar;
		case CAR_TYPE::BLUE_CAR:
			return &i_bluecar;
		case CAR_TYPE::GREEN_CAR:
			return &i_greencar;
		case CAR_TYPE::GREY_CAR:
			return &i_greycar;
		default:
			return nullptr;
		}
	}

};

MAP_DRAW MAP_DRAW::WinMap;//静态成员变量初始化


void graph_init() {//初始化图形界面
	initgraph(1000, 700);//初始化窗口
	setbkcolor(WHITE);//设置背景颜色
	cleardevice();
    //加载图片
    loadimage(&i_add, _T("ADD.png"), 90, 30, true);
    loadimage(&i_delete, _T("DELETE.png"), 90, 30, true);
    loadimage(&i_taxi, _T("TAXI.png"), 200, 100, true);
    loadimage(&i_redcar, _T("RED_CAR.png"), 200, 100, true);
    loadimage(&i_bluecar, _T("BLUE_CAR.png"), 100, 200, true);
    loadimage(&i_greencar, _T("GREEN_CAR.png"), 300, 100, true);
    loadimage(&i_greycar, _T("GREY_CAR.png"), 100, 300, true);
	loadimage(&i_redcar2, _T("RED_CAR.png"), 80, 40, true);
    loadimage(&i_bluecar2, _T("BLUE_CAR.png"), 40, 80, true);
    loadimage(&i_greencar2, _T("GREEN_CAR.png"), 120, 40, true);
    loadimage(&i_greycar2, _T("GREY_CAR.png"), 40, 120, true);
	loadimage(&i_carerror, _T("CAR_ERROR.png"), 90, 30, true);
	loadimage(&i_directiondown, _T("DIRECTION_DOWN.png"), 60, 60, true);
    loadimage(&i_directionleft, _T("DIRECTION_LEFT.png"), 60, 60, true);
    loadimage(&i_directionright, _T("DIRECTION_RIGHT.png"), 60, 60, true);
    loadimage(&i_directionup, _T("DIRECTION_UP.png"), 60, 60, true);
	loadimage(&i_laststep, _T("LAST_STEP.png"), 70, 170, true);
    loadimage(&i_nextstep, _T("NEXT_STEP.png"), 70, 170, true);
	loadimage(&i_mapbase, _T("MAP_BASE.png"), 600, 600, true);
	loadimage(&i_steperror, _T("STEP_ERROR.png"), 70, 170, true);
	loadimage(&i_title, _T("TITLE.png"), 900, 60, true);
    loadimage(&i_to0, _T("TO0.png"), 110, 30, true);
    loadimage(&i_to1, _T("TO1.png"), 110, 30, true);
    loadimage(&i_directionerror, _T("DIRECTION_ERROR.png"), 60, 60, true);
    loadimage(&i_bk, _T("BK.png"), 1000, 700, true);
	loadimage(&i_upbk, _T("UBK.png"), 350, 320, true);
	loadimage(&i_downbk, _T("DBK.png"), 350, 270, true);
    BeginBatchDraw(); // 开始批量绘制
}

void setdirectionbutton(CAR_TYPE temp) {//设置方向按钮的状态
	if (temp == CAR_TYPE::RED_CAR|| temp == CAR_TYPE::TAXI|| temp == CAR_TYPE::GREEN_CAR) {//红色车和出租车
		move_downwidget.direction_up->is_used = false;
		move_downwidget.direction_down->is_used = false;
		move_downwidget.direction_left->is_used = true;
		move_downwidget.direction_right->is_used = true;
	}
	else if (temp == CAR_TYPE::BLUE_CAR|| temp == CAR_TYPE::GREY_CAR) {//蓝色车和灰色车
		move_downwidget.direction_up->is_used = true;
		move_downwidget.direction_down->is_used = true;
		move_downwidget.direction_left->is_used = false;
		move_downwidget.direction_right->is_used = false;
	}

}

void button_move_update() {//更新按钮的状态 编辑模式
    //更新按钮的状态,初始化全部可用
    upwidget.addcar->is_used = true;
	upwidget.deletecar->is_used = true;
	move_downwidget.direction_up->is_used = true;
	move_downwidget.direction_down->is_used = true;
	move_downwidget.direction_left->is_used = true;
	move_downwidget.direction_right->is_used = true;
    if (mapcash.chosencar != -1) {//如果未选择了临时汽车
        if (mapcash.chosencar == 0) {//如果没有选择汽车
            move_downwidget.direction_up->is_used = false;
			move_downwidget.direction_down->is_used = false;
			move_downwidget.direction_left->is_used = false;
			move_downwidget.direction_right->is_used = false;
		}
		else if (mapcash.chosencar == 1) {//如果选择了出租车
			setdirectionbutton(mapcash.Taxi.Type);
		}
		else {//如果选择了其他汽车
			setdirectionbutton(mapcash.Cars[mapcash.chosencar - 2].Type);
		}
	}
	else {//如果选择了临时汽车
    }
    if (mapcash.chosencar != -1) {
        upwidget.addcar->is_used = false;
    }
	if (mapcash.chosencar == 0|| mapcash.chosencar == 1) {
		upwidget.deletecar->is_used = false;
	}
}

void movecar_update(int mouse_x,int mouse_y)//编辑界面的更新函数
{
    move_downwidget.update(mouse_x, mouse_y);
    upwidget.update(mouse_x, mouse_y);
    mapcash.is_Mpress(mouse_x, mouse_y);
	button_move_update();
    MAP_DRAW::getInstance().towins(mapcash);
}

void button_answer_update() {//更新按钮的状态解模式
	answer_downwidget.last_step->is_used = true;
	answer_downwidget.next_step->is_used = true;
	if (nowstep == 0) {
		answer_downwidget.next_step->is_used = false;
	}
	if (nowstep == path.size() - 1) {
		answer_downwidget.last_step->is_used = false;
	}
}

void answer_update(int mouse_x, int mouse_y)//解模式的更新函数
{
	int push = answer_downwidget.update(mouse_x, mouse_y);//获取鼠标点击的按钮行为编号
    if (push == 0) return;//如果没有点击按钮
    else if (push == 2) {//下一步
        if (path.empty()) return;
        nowstep = max(nowstep - 1, 0);
    }
    else if (push == 1) {//上一步
        if (path.empty()) return;
		nowstep = min(nowstep + 1, (int)path.size() - 1);
	}
    else if (push == 3) {//返回编辑模式
        nowstep = 0;
        path.clear();
        visited.clear();
        while (!pq.empty())
        {
            pq.pop();
        }
        thismap = mapcash.returnmap();
        MAP_DRAW::getInstance().towins(mapcash);
        thismap.prev = visited.end();
        mapcash.chosencar = 0;
		upwidget.label->updatechar("EDIT MODE\n");
        ::flag = false;
    }
    if (!path.empty()) {
        thismap = path[nowstep];
        MAP_DRAW::getInstance().towins(thismap);
        mapcash.tocash(thismap);
        mapcash.chosencar = 0;
        mapcash.update_map();
        upwidget.label->updatechar(thismap.laststep);
    }
}

void graph_update() {//更新图形界面
    //更新地图
    ExMessage m;
    m = getmessage(EX_MOUSE | EX_KEY);
    if (m.message == WM_LBUTTONDOWN)
    {
        if (flag == false) {
            movecar_update(m.x,m.y);
        }
        else {
            answer_update(m.x, m.y);
        }
    }
}

void movecar_show()//编辑模式的显示函数
{
	move_downwidget.draw();
}

void answer_show()//解模式的显示函数
{
    button_answer_update();
    answer_downwidget.draw();
}

void graph_show()//整体显示函数
{
	cleardevice();
	putimage(0, 0, &i_bk);//背景
    //绘制上半部分的窗口的背景
    putimage(640, 80, &i_upbk);
    //绘制下半部分的窗口的背景
	putimage(640, 410, &i_downbk);
    //绘制地图
	MAP_DRAW::getInstance().show();
    //绘制标题
    putimagePng(50, 10, &i_title);
    //绘制上半部分的窗口
    upwidget.draw();
    //根据模式绘制其他部分
    if (flag == false) {
        movecar_show();
    }
    else {
        answer_show();
    }
    FlushBatchDraw(); // 批量绘制
}


MAP init_map() {//初始化地图
    MAP map;
    map.add_car(CAR_TYPE::RED_CAR, 4, 3);
    map.add_car(CAR_TYPE::BLUE_CAR, 2, 2);
    map.add_car(CAR_TYPE::BLUE_CAR, 2, 4);
    map.add_car(CAR_TYPE::BLUE_CAR, 3, 3);
    map.add_car(CAR_TYPE::GREEN_CAR, 0, 0);
    map.add_car(CAR_TYPE::GREEN_CAR, 3, 5);
    map.add_car(CAR_TYPE::GREY_CAR, 5, 0);
    std::cout << "Initial Map:" << std::endl;
    map.prev = visited.end();
    return map;
}
//主函数
int main() {
    MAP map = init_map();//初始化地图
    mapcash.tocash(map);//将地图缓存
    MAP_DRAW::getInstance().towins(map);//将地图转换为绘制用的对象
    graph_init();//初始化图形界面
    while (true)//图形化循环
    {
        graph_update();//更新图形界面
        graph_show();//显示图形界面
        Sleep(1);// 延时1秒
    }
    return 0;
}