#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <stack>
#include <algorithm>

enum CAR_TYPE {
    TAXI = 0,//2*1
    RED_CAR = 1,//2*1
    BLUE_CAR = 2,//1*2
    GREEN_CAR = 3,//3*1
    GREY_CAR = 4,//1*3
};
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
    CAR(CAR_TYPE type, int cx, int cy) {
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
                tempmap.update_map();
                result.push_back(tempmap);
            }
            else {
                break;
            }
        }
        return result;
    }
    std::vector<MAP> move_car(int index) const {//使用的是Cars的下标
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
                tempmap.update_map();
                result.push_back(tempmap);
            }
            else {
                break;
            }
        }
        return result;
    }
    bool carisvalid(int index, const CAR& tempcar) const {//判断汽车是否合法
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
};

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

void printmap(const MAP& map) {//打印地图(调试用)
    std::cout << "step:" << map.step << std::endl;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            std::cout << map.Map[j][i] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "------------------" << std::endl;
}


std::set<MAP> visited;//存放已经访问过的地图
std::priority_queue<MAP, std::vector<MAP>, MAPCMP> pq;//优先队列
std::stack<MAP> path;//存放路径

bool Astar(const MAP& start) {//A*算法(主要函数)
    pq.push(start);
    visited.insert(start);
    while (!pq.empty()) {
        MAP tempmap = pq.top();
        pq.pop();
        if (tempmap.is_taxi_exit()) {
            std::set<MAP>::iterator tempmapfindit = visited.find(tempmap);
            while (tempmapfindit != visited.end()) {
                path.push(*tempmapfindit);
                tempmapfindit = tempmapfindit->prev;
            }
            return true;
        }
        std::vector<MAP> result = tempmap.move_all();
        std::set<MAP>::iterator tempmapit = visited.find(tempmap);
        for (auto& nextmap : result) {
            if (visited.find(nextmap) == visited.end()) {
                nextmap.prev = tempmapit;
                nextmap.step = tempmap.step + 1;
                visited.insert(nextmap);
                pq.push(nextmap);
            }
        }
    }
    return false;
}

class MAP_DRAW : public MAP_BASE {//地图的类，绘制用 饿汉式单例
private:
    MAP_DRAW() {
        //初始化地图
        //出租车在(0,1)的位置
    }
    ~MAP_DRAW() override {
        //析构函数
        //释放资源
    }
    MAP_DRAW(const MAP_DRAW& other) = delete;
    MAP_DRAW& operator=(const MAP_DRAW& other) = delete;

    static MAP_DRAW WinMap;
public:
    static MAP_DRAW& getInstance() {
        return WinMap;
    }
    void towins(const MAP& map) {
        //使用地图初始化
        Taxi = map.Taxi;
        Cars = map.Cars;
        step = map.step;
    }
};

MAP_DRAW MAP_DRAW::WinMap;//静态成员变量初始化

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
    printmap(map);
    map.prev = visited.end();
    return map;
}
//主函数
int main() {
    MAP map = init_map();
    Astar(map);
    while (!path.empty()) {
        printmap(path.top());
        path.pop();
    }
    return 0;
}