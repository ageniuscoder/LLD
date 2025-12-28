#include <bits/stdc++.h>   //no need to declare memory in heap
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
class HTTPBuilder{
    HTTPRequest req;
    public:
    HTTPBuilder& withUrl(const string& url){
        req.url=url;
        return *this;
    }
    HTTPBuilder& withMethod(const string& method){
        req.method=method;
        return *this;
    }
    HTTPBuilder& withHeader(const string& key,const string& value){        //A reference is an alias (another name) for an existing variable.
        req.headers[key]=value;
        return *this;
    }
    HTTPBuilder& withBody(const string& body){
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

class HTTPRequestDirector{
    public:
    static HTTPRequest createGetReq(const string &url){
        return HTTPBuilder()
            .withUrl(url)
            .withMethod("GET")
            .build();
    }

    static HTTPRequest createPostReq(const string &url,const string &body){
        return HTTPBuilder()
            .withUrl(url)
            .withMethod("POST")
            .withHeader("Content-Type","application/json")
            .withHeader("Accept","application/json")
            .withBody(body)
            .build();
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

    HTTPRequest req2=HTTPRequestDirector::createGetReq("https://mangal.com");
    req2.execute();


    HTTPRequest req3=HTTPRequestDirector::createPostReq("https://mangal.com","mangal is going");
    req3.execute();


}