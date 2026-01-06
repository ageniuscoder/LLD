#include <bits/stdc++.h>
using namespace std;
class AsteriodFlyweight{
    int height; int width;
    int weight; string color;
    string texture;
    public:
    AsteriodFlyweight(int h,int w,int wt,const string &c,const string &t):height(h),width(w),weight(wt),color(c),texture(t){}
    void render(int x,int y,int vx,int vy){
        cout<<"[Asteroid]: height:"<<height<<" width:"<<width<<" weight:"<<weight<<endl;
        cout<<"color:"<<color<<" texture:"<<texture<<endl;
        cout<<"postionX:"<<x<<" positionY:"<<y<<endl;
        cout<<"velocityX:"<<vx<<" velocityY:"<<vy<<endl;
    }

    static int getMemory(){
        return sizeof(int)*3+sizeof(string)*2+32*2;
    }
};
class FlyweightFactory{
    static unordered_map<string,shared_ptr<AsteriodFlyweight>> mp;
    public:
    static shared_ptr<AsteriodFlyweight> getFlyweight(int h,int w,int wt,const string &c,const string &t){
        string key=to_string(h)+"|"+to_string(w)+"|"+to_string(wt)+"|"+c+"|"+t;
        if(!mp.count(key)){
            mp[key]=make_shared<AsteriodFlyweight>(h,w,wt,c,t);
        }
        return mp[key];
    }

    static int getMemory(){
        return mp.size()*AsteriodFlyweight::getMemory();
    }
};
unordered_map<string,shared_ptr<AsteriodFlyweight>> FlyweightFactory::mp;
class AsteroidContext{
    shared_ptr<AsteriodFlyweight> af;
    int x; int y; int vx; int vy;
    public:
    AsteroidContext(shared_ptr<AsteriodFlyweight> af,int x,int y,int vx,int vy):af(af),x(x),y(y),vx(vx),vy(vy){}
    void render(){
        af->render(x,y,vx,vy);
    }
    static int getMemory(){
        return sizeof(af)+4*sizeof(int);
    }
};
class GameSpace{
    vector<unique_ptr<AsteroidContext>> asteroids;
    public:
    void spawnAsteroids(int cnt){
        vector<int> height={10,20,30};
        vector<string> color={"red","green","blue"};
        vector<string> text={"soft","hard","semi"};
        for(int i=0;i<cnt;i++){
            int idx=i%3;
            shared_ptr<AsteriodFlyweight> af=FlyweightFactory::getFlyweight(height[idx],height[idx],height[idx]*10,color[idx],text[idx]);
            unique_ptr<AsteroidContext> asteroid=make_unique<AsteroidContext>(af,4*i+1,5*i+2,1,2);
            asteroids.push_back(move(asteroid));
        }
    }

    void renderAll(){
        cout<<"Rendering only first 10 obejects"<<endl;
        for(int i=0;i<min(10,(int)asteroids.size());i++){
            asteroids[i]->render();
        }
    }

    int getMemory(){
        return asteroids.size()*AsteroidContext::getMemory()+FlyweightFactory::getMemory();
    }
};
int main(){
    const int count=1000000;
    unique_ptr<GameSpace> game=make_unique<GameSpace>();
    game->spawnAsteroids(count);
    game->renderAll();
    

    cout<<"Memory Usage"<<endl;
    cout<<"In kb: "<<game->getMemory()<<" kbs"<<endl;
    cout<<"In mb: "<<game->getMemory()/(1024*1024)<<endl;

}