#include <bits/stdc++.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>

using namespace std;
enum class LogLevel{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};
class Utility{
    public:
    static string currentTimestamp(){
        auto now=chrono::system_clock::now();
        time_t t=chrono::system_clock::to_time_t(now);
        tm tm{};
        localtime_s(&tm,&t);
        ostringstream oss;
        oss <<put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};
class LogMessage{
    string timestamp;
    LogLevel level;
    string content;
    public:
    LogMessage(LogLevel level,const string& content):level(level),content(content){
        timestamp=Utility::currentTimestamp();
    }
    string getTimestamp() const{
        return timestamp;
    }
    LogLevel getLevel() const{
        return level;
    }
    string getContent() const{
        return content;
    }
};
class LogFormatter{
    protected:
    string levelToString(LogLevel level){
        switch(level){
            case LogLevel::DEBUG:
                return "DEBUG";
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARNING:
                return "WARNING";
            case LogLevel::ERROR:
                return "ERROR";
            case LogLevel::FATAL:
                return "FATAL";
        }
        return "UNKNOWN";
    }
    public:
    virtual string format(const LogMessage& msg)=0;
    virtual ~LogFormatter()=default;
};
class TextFormatter:public LogFormatter{
    public:
    string format(const LogMessage& msg){
        ostringstream oss;
        oss<<"["<<msg.getTimestamp()<<"] "<<"["<<levelToString(msg.getLevel())<<"]: "<<msg.getContent();
        return oss.str();
    }
};
class JsonFormatter:public LogFormatter{
    public:
    string format(const LogMessage& msg){
        ostringstream oss;
        oss << "{"
            << "\"timestamp\":\"" << msg.getTimestamp()<< "\","
            << "\"level\":\"" << levelToString(msg.getLevel()) << "\","
            << "\"message\":\"" << msg.getContent() << "\""
            << "}";
        return oss.str();
    }
};
class LogAppender{
    protected:
    LogFormatter* formatter;
    public:
    LogAppender():formatter(nullptr){}
    void setFormatter(LogFormatter* format){
        formatter=format;
    }
    virtual void appendMessage(const LogMessage& msg)=0;
    virtual ~LogAppender()=default;
};
class ConsoleAppender:public LogAppender{
    public:
    void appendMessage(const LogMessage& msg){
        cout<<formatter->format(msg)<<endl;
    }
};
class FileAppender:public LogAppender{
    ofstream file;
    public:
    FileAppender(){
        file.open("app.log",ios::out | ios::app);
        if (!file.is_open()) {
            throw runtime_error("Failed to open log file");
        }
    }
    void appendMessage(const LogMessage& msg){
        file<<formatter->format(msg)<<endl;
        file.flush();
    }
    ~FileAppender(){
        if (file.is_open()) {
            file.close();
        }
    }
};
class LogHandler{
    protected:
    LogHandler* next;
    vector<LogAppender*> appenders;
    public:
    LogHandler(){
        next=nullptr;
    }
    void setNext(LogHandler* nextHandler){
        next=nextHandler;
    }
    void addAppender(LogAppender* appender){
        appenders.push_back(appender);
    }
    virtual void handleLog(const LogMessage& msg)=0;
    virtual ~LogHandler()=default;
};
class DEBUGHandler:public LogHandler{
    public:
    void handleLog(const LogMessage& msg){
        if(msg.getLevel()==LogLevel::DEBUG){
            for(auto &appender:appenders){
                appender->appendMessage(msg);
            }
        }else{
           if(next) next->handleLog(msg);
        }
    }
};
class INFOHandler:public LogHandler{
    public:
    void handleLog(const LogMessage& msg){
        if(msg.getLevel()==LogLevel::INFO){
            for(auto &appender:appenders){
                appender->appendMessage(msg);
            }
        }else{
            if(next) next->handleLog(msg);
        }
    }
};
class WARNINGHandler:public LogHandler{
    public:
    void handleLog(const LogMessage& msg){
        if(msg.getLevel()==LogLevel::WARNING){
            for(auto &appender:appenders){
                appender->appendMessage(msg);
            }
        }else{
            if(next) next->handleLog(msg);
        }
    }
};
class ERRORHandler:public LogHandler{
    public:
    void handleLog(const LogMessage& msg){
        if(msg.getLevel()==LogLevel::ERROR){
            for(auto &appender:appenders){
                appender->appendMessage(msg);
            }
        }else{
            if(next) next->handleLog(msg);
        }
    }
};
class FATALHandler:public LogHandler{
    public:
    void handleLog(const LogMessage& msg){
        if(msg.getLevel()==LogLevel::FATAL){
            for(auto &appender:appenders){
                appender->appendMessage(msg);
            }
        }else{
            if(next) next->handleLog(msg);
        }
    }
};
class ConfigLogSystem{
    public:
    void addAppenderConfig(LogHandler* handler,LogAppender* appender){
        handler->addAppender(appender);
    }
    void addFormatterConfig(LogAppender* appender,LogFormatter* formatter){
        appender->setFormatter(formatter);
    }
};
class Logger{
    mutex mtx;
    LogLevel threshold;
    unique_ptr<ConfigLogSystem> configurer;
    LogHandler* handler;
    unique_ptr<DEBUGHandler> debugH;
    unique_ptr<INFOHandler> infoH;
    unique_ptr<WARNINGHandler> warnH;
    unique_ptr<ERRORHandler> errorH;
    unique_ptr<FATALHandler> fatalH;
    Logger(){
        threshold=LogLevel::DEBUG;
        configurer=make_unique<ConfigLogSystem>();
        debugH=make_unique<DEBUGHandler>();
        infoH=make_unique<INFOHandler>();
        warnH=make_unique<WARNINGHandler>();
        errorH=make_unique<ERRORHandler>();
        fatalH=make_unique<FATALHandler>();
        handler=debugH.get();
        debugH->setNext(infoH.get());
        infoH->setNext(warnH.get());
        warnH->setNext(errorH.get());
        errorH->setNext(fatalH.get());
    }
    LogMessage logMessage(LogLevel level,const string& content){
        return LogMessage(level,content);
    }
    public:
    Logger(const Logger&)=delete;
    Logger& operator=(const Logger&)=delete;
    static Logger& getInstance(){
        static Logger instance;
        return instance;
    }
    void setThreshold(LogLevel level) {
        threshold = level;
    }
    bool shouldLog(LogLevel level) {
        return static_cast<int>(level) >= static_cast<int>(threshold);
    }

    DEBUGHandler* getDebug(){
        return debugH.get();
    }
    INFOHandler* getInfo(){
        return infoH.get();
    }
    WARNINGHandler* getWarning(){
        return warnH.get();
    }
    ERRORHandler* getError(){
        return errorH.get();
    }
    FATALHandler* getFatal(){
        return fatalH.get();
    }
    void addAppenderConfig(LogHandler* handler,LogAppender* appender){
        configurer->addAppenderConfig(handler,appender);
    }
    void addFormatterConfig(LogAppender* appender,LogFormatter* formatter){
        configurer->addFormatterConfig(appender,formatter);
    }
    void debug(const string& content){
        if (!shouldLog(LogLevel::DEBUG)) return;
        lock_guard<mutex> lock(mtx);
        handler->handleLog(logMessage(LogLevel::DEBUG, content));
    }

    void info(const string& content){
        if (!shouldLog(LogLevel::INFO)) return;
        lock_guard<mutex> lock(mtx);
        handler->handleLog(logMessage(LogLevel::INFO, content));
    }

    void warning(const string& content){
        if (!shouldLog(LogLevel::WARNING)) return;
        lock_guard<mutex> lock(mtx);
        handler->handleLog(logMessage(LogLevel::WARNING, content));
    }

    void error(const string& content){
        if (!shouldLog(LogLevel::ERROR)) return;
        lock_guard<mutex> lock(mtx);
        handler->handleLog(logMessage(LogLevel::ERROR, content));
    }

    void fatal(const string& content){
        if (!shouldLog(LogLevel::FATAL)) return;
        lock_guard<mutex> lock(mtx);
        handler->handleLog(logMessage(LogLevel::FATAL, content));
    }

};
int main(){
    Logger& app=Logger::getInstance();
    app.setThreshold(LogLevel::WARNING);
    FileAppender fileAppender;
    ConsoleAppender consoleAppender;
    TextFormatter textFormatter;
    JsonFormatter jsonFormatter;
    app.addFormatterConfig(&consoleAppender,&textFormatter);
    app.addFormatterConfig(&fileAppender,&jsonFormatter);
    app.addAppenderConfig(app.getDebug(),&consoleAppender);
    app.addAppenderConfig(app.getInfo(),&consoleAppender);
    app.addAppenderConfig(app.getWarning(),&consoleAppender);
    app.addAppenderConfig(app.getWarning(),&fileAppender);
    app.addAppenderConfig(app.getError(),&fileAppender);
    app.addAppenderConfig(app.getFatal(),&fileAppender);

    app.info("info message");
    app.debug("debug message");
    app.warning("warning message");
    app.error("error message");
    app.fatal("fatal error");
    
}