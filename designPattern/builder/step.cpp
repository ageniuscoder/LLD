#include <bits/stdc++.h>
using namespace std;
class HTTPRequest{
    string url;
    string method="GET";
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
class MethodStep;
class HeadersStep;
class OptionsStep;
class UrlStep{
    public:
    virtual MethodStep& withUrl(const string &url)=0;
    virtual ~UrlStep()=default;
};
class MethodStep{
    public:
    virtual HeadersStep& withMethod(const string &method)=0;
    virtual ~MethodStep()=default;
};
class HeadersStep{
    public:
    virtual OptionsStep& withHeaders(const string &key,const string &val)=0;
    virtual ~HeadersStep()=default;
};
class OptionsStep{
    public:
    virtual OptionsStep& withBody(const string &body)=0;
    virtual OptionsStep& withTimeout(int timeout)=0;
    virtual HTTPRequest build()=0;
    virtual ~OptionsStep()=default;
};
class HTTPBuilder:
      public UrlStep,
      public MethodStep,
      public HeadersStep,
      public OptionsStep{

    HTTPRequest req;
    public:
    MethodStep& withUrl(const string &url){
        req.url=url;
        return *this;
    }

    HeadersStep& withMethod(const string &method){
        req.method=method;
        return *this;
    }

    OptionsStep& withHeaders(const string &key,const string &val){
        req.headers[key]=val;
        return *this;
    }

    OptionsStep& withBody(const string &body){
        req.body=body;
        return *this;
    }

    OptionsStep& withTimeout(int timeout){
        req.timeout=timeout;
        return *this;
    }

    HTTPRequest build(){
        if(req.url.empty()){
            throw runtime_error("provide url please");
        }
        return req;
    }

    static HTTPBuilder getBuilder(){      //it,s safe but doesn,t enforces to start with url
        return HTTPBuilder();
    }

    // static UrlStep& getBuilder(){             //issue with it is builder is shared member for class so it is not thread safe
    //     static HTTPBuilder builder;
    //     return builder;
    // }
    // static UrlStep& getBuilder(){   //this is not safe memory leak immediately bcz i return reference so their is no way to delete the pointer
        
    //     return *(new HTTPBuilder());
    // }


};

int main(){
    HTTPRequest req1=HTTPBuilder::getBuilder()
                                .withUrl("https://play.org")
                                .withMethod("POST")
                                .withHeaders("content-type","application/json")
                                .withBody("mangal")
                                .build();
                
    req1.execute();
    
}