#include <bits/stdc++.h>   //const protects data, final protects design.
using namespace std;
class ModelTrainer{
    public:
    virtual void load(string path)=0;
    virtual void preprocess(){
        cout<<"data is preprocessed"<<endl;
    }
    virtual void train()=0;
    virtual void evaluate()=0;
    virtual void save(){
        cout<<"Model is ready and saved for use"<<endl;
    }
    virtual void pipeline(string path) final{
        load(path);
        preprocess();
        train();
        evaluate();
        save();
    }

    virtual ~ModelTrainer()=default;
};
class NeuralNetwork:public ModelTrainer{
    public:
    void load(string path){
        cout<<"[NeuralNetwork]:data it load from "<<path<<endl;
    }
    void train(){
        cout<<"[NeuralNetwork]:model is trained"<<endl;
    }
    void evaluate(){
        cout<<"[NeuralNetwork]:modle is evaluated"<<endl;
    }
};
class DecisionTree:public ModelTrainer{
    public:
    void load(string path){
        cout<<"[DecisionTree]:data it load from "<<path<<endl;
    }
    void train(){
        cout<<"[DecisionTree]:model is trained"<<endl;
    }
    void evaluate(){
        cout<<"[DecisionTree]:modle is evaluated"<<endl;
    }
};
int main(){
    shared_ptr<ModelTrainer> model1=make_shared<NeuralNetwork>();
    //model1->load("mangal");
    model1->pipeline("mangal.csv");

    cout<<"_________________________________"<<endl;

    shared_ptr<ModelTrainer> model2=make_shared<DecisionTree>();
    //model2->load("shailu");
    model2->pipeline("shailu.csv");

}



//In C++, immutability is achieved using const member functions and const data members. A const method guarantees it does not modify object state. The final keyword prevents inheritance or method overriding and is used to enforce design constraints.