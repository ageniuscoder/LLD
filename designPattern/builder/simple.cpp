#include <bits/stdc++.h>   //no need to declare memory in heap
using namespace std;
class HTTPRequest{
    string url;
    string method;
    map<string,string> headers;
    string body;
    int timeout=0         ;

    HTTPRequest(){}
    public:
    friend class HTTPBuilder;
    void execute(){
        cout<<"Executing http " <<method<< " to url: "<<url<<endl;
        if(!headers.empty()){
            cout<<"Headers: "<<endl;
            for(auto [a,b]:headers){
                cout<<a<<" : "<<b<<endl;
            }
        }
        if(!body.empty()){
            cout<<"Body: "<<body<<endl;
        }
        if(timeout>0){
            cout<<"Timeout: "<<timeout<<endl;
        }
    }
};
class HTTPBuilder{
    HTTPRequest req;
    public:
    HTTPBuilder& withUrl(string url){
        req.url=url;
        return *this;
    }
    HTTPBuilder& withMethod(string method){
        req.method=method;
        return *this;
    }
    HTTPBuilder& withHeader(string key,string value){        //A reference is an alias (another name) for an existing variable.
        req.headers[key]=value;
        return *this;
    }
    HTTPBuilder& withBody(string body){
        req.body=body;
        return *this;
    } 
    HTTPBuilder& withTimeout(int timeout){
        req.timeout=timeout;
        return *this;                                   //this is pointer and *this is object
    }

    HTTPRequest build(){
        if(req.url.empty()){
            throw runtime_error("Provide url to the request");
        }
        return req;
    }


};
int main(){
    HTTPRequest req1=HTTPBuilder()
                .withUrl("https://example.api.com")
                .withMethod("POST")
                .withHeader("Content-type","application/json")
                .withBody("mangal")
                .withTimeout(4)
                .build();
    req1.execute();
}