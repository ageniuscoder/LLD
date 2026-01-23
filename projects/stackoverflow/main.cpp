#include <bits/stdc++.h>
using namespace std;
class User{
    static int cnt;
    string id;
    string name;
    int score;
    public:
    User(const string& name):name(name){
        id="user"+to_string(cnt++);
        score=0;
    }
    string getId(){
        return id;
    }
    string getName(){
        return name;
    }
    void updateReputation(int val){
        score+=val;
    }
    int getReputation(){
        return score;
    }
};
int User::cnt=1;
enum class VoteType:int{
    UPVOTE=1,
    DOWNVOTE=-1
};
class Vote{
    User* voter;
    VoteType type;
    string postId;
    public:
    Vote(User* voter,VoteType type,const string& postId):voter(voter),type(type),postId(postId){}
    User* getVoter(){
        return voter;
    }
    VoteType getType(){
        return type;
    }
    string getPostId(){
        return postId;
    }
    string printType(){
        if(type==VoteType::UPVOTE){
            return "upvote";
        }
        return "downvote";
    }
};
class Tag{
    static int cnt;
    string id;
    string name;
    public:
    Tag(const string& name):name(name){
        id="tag"+to_string(cnt++);
    }
    string getId(){
        return id;
    }
    string getName(){
        return name;
    }
};
int Tag::cnt=1;
class Comment;
class Post{
    static int cnt;
    protected:
    string id;
    string content;
    User* author;
    vector<Comment*> comments;
    vector<unique_ptr<Vote>> votes;
    int upvote;
    int downvote;
    public:
    Post(const string& content,User* author):content(content),author(author){
        upvote=0; downvote=0;
        id="post"+to_string(cnt++);
    }
    string getId(){
        return id;
    }
    void addComment(Comment* comment){
        comments.push_back(comment);
        cout<<"comment is added to post: "<<id<<endl;
    }
    void removeComment(const string& cid);
    void addVote(User* user,VoteType type){
        for(auto &v : votes){
            if(v->getVoter() == user){
                cout << "User already voted\n";
                return;
            }
        }

        if(type==VoteType::UPVOTE){
            upvote+=static_cast<int>(type);
        }else{
            downvote+=abs(static_cast<int>(type));
        }
        votes.push_back(make_unique<Vote>(user, type, id));
        cout<<"vote is added to post: "<<id<<endl;
    }
    void removeVote(User* user){
        for(auto it=votes.begin();it!=votes.end();it++){
            if((*it)->getVoter()==user){
                if((*it)->getType()==VoteType::UPVOTE){
                    upvote-=static_cast<int>(VoteType::UPVOTE);
                }else{
                    downvote+=static_cast<int>(VoteType::DOWNVOTE);
                }
                it=votes.erase(it);
                cout<<"vote is removed"<<endl;
                return;
            }
        }
    }
    int getUpvote(){
        return upvote;
    }
    int getDownvote(){
        return downvote;
    }
    string getContent(){
        return content;
    }
    User* getAuthor(){
        return author;
    }
    void printComments();
    void printVoters(){
        for(auto& vote:votes){
            cout<<"[voter]: "<<vote->getVoter()->getName()<<endl;
            cout<<"[vote type]: "<<vote->printType()<<endl;
        }
    }

};
int Post::cnt=1;
class Comment:public Post{
    string parentPostid;
    public:
    Comment(const string& content,User* author):Post(content,author){
    }
    void setParent(const string& id){
        parentPostid=id;
    }
};
void Post::removeComment(const string& cid) {
    for (auto it = comments.begin(); it != comments.end(); ++it) {
        if ((*it)->getId() == cid) {
            comments.erase(it);
            cout << "comment is deleted" << endl;
            return;
        }
    }
}

void Post::printComments() {
    for (auto c : comments) {
        cout << "[Comment]: " << c->getId() << endl;
        cout << "[Content]: " << c->getContent() << endl;
    }
}
class Answer:public Post{
    bool accepted;
    public:
    Answer(const string& content,User* author):Post(content,author){
        accepted=false;
    };
    void markAccepted(){
        if(accepted){
            cout<<"answer is already accepted"<<endl;
            return;
        }
        accepted=true;
    }
    bool isAccepted(){
        return accepted;
    }
};
class Question:public Post{
    string title;
    vector<Answer*> answers;
    Answer* acceptedAnswer;
    vector<Tag*> tags;
    public:
    Question(const string& title,Tag* tag,const string& content,User* author):Post(content,author),title(title){
        tags.push_back(tag);
        acceptedAnswer=nullptr;
    }
    void addTag(Tag* tag){
        tags.push_back(tag);
    }
    string getTitle(){
        return title;
    }
    void printTags(){
        cout<<"Tags for Question: "<<id<<endl;
        for(auto tag:tags){
            cout<<tag->getName()<<"   ";
        }
        cout<<endl;
    }
    void addAnswer(Answer* answer){
        answers.push_back(answer);
    }
    void acceptAnswer(string uid,Answer* answer){
        if(acceptedAnswer){
            cout<<"answer is already accepted for this question"<<endl;
            return;
        }
        if(author->getId()!=uid){
            cout<<"only author of question can accept is answer"<<endl;
            return;
        }
        if(answer->isAccepted()){
            cout<<"Answer is already accepeted for other question"<<endl;
            return;
        }
        acceptedAnswer=answer;
        answer->markAccepted();
        cout<<"answer accepted"<<endl;
    }
    void getAcceptedAnswer(){
        if(acceptedAnswer){
            cout<<"[Answer]: "<<acceptedAnswer->getContent()<<endl;
        }else{
            cout<<"No correct answer as of now"<<endl;
        }
    }
    void printAllAnswer(){
        for(auto &answer:answers){
            cout<<"Author: "<<answer->getAuthor()->getName()<<"\n"<<"content: "<<answer->getContent()<<endl;
        }
    }
    vector<string> getTagNames(){
        vector<string> name;
        for(auto tag:tags){
            name.push_back(tag->getName());
        }
        return name;
    }
};
class TagManager{
    map<string,unique_ptr<Tag>> tags; //name,tag
    TagManager(){}
    public:
    TagManager(const TagManager&)=delete;
    TagManager& operator=(const TagManager&)=delete;
    static TagManager& getInstance(){
        static TagManager instance;
        return instance;
    }
    Tag* getTag(const string& name){
        if(!tags.count(name)){
            tags[name]=make_unique<Tag>(name);
        }
        return tags[name].get();
    }

};
class QuestionManager{
    map<string,unique_ptr<Question>> questions; //qid,q
    map<string,vector<string>> postByUser; //uid,qid
    map<string,vector<string>> taggedQues; //tname,qid
    QuestionManager(){}
    public:
    QuestionManager(const QuestionManager&)=delete;
    QuestionManager& operator=(const QuestionManager&)=delete;
    static QuestionManager& getInstance(){
        static QuestionManager instance;
        return instance;
    }
    void postQuestion(unique_ptr<Question> ques){
        string authorId=ques->getAuthor()->getId();
        string quesId=ques->getId();
        vector<string> tagNames=ques->getTagNames();
        questions[quesId]=move(ques);
        postByUser[authorId].push_back(quesId);
        for(auto &name:tagNames){
            taggedQues[name].push_back(quesId);
        }
        cout<<"[Question]: "<<quesId<<" is posted by User: "<<authorId<<endl;
    }
    void acceptAnswer(const string& uid,const string& qid,Answer* ans){
        if(!questions.count(qid)){
            cout<<"Question doesn,t exists"<<endl;
            return;
        }
        questions[qid]->acceptAnswer(uid,ans);
        cout<<"Congrats Your answer is Accepted"<<endl;

    }
    Question* getQuesById(const string& id){
        if(!questions.count(id)){
            cout<<"Question doesn,t exist"<<endl;
            return nullptr;
        }
        return questions[id].get();
    }
    void searchByTag(const string& keyword){
        if (!taggedQues.count(keyword)) {
            cout << "No question related to this tag is available\n";
            return;
        }
        cout << "All questions related to tag: " << keyword << endl;
        for (const auto& qid : taggedQues[keyword]) {
            if (questions.count(qid)) {
                cout << "[Question]: " << questions[qid]->getTitle() << endl;
            }
        }

    }
    void searchByUser(const string& keyword){

        if (!postByUser.count(keyword)) {
            cout << "No questions posted by this user\n";
            return;
        }
        cout << "All questions posted by user: " << keyword << endl;
        for (const auto& qid : postByUser[keyword]) {
            if (questions.count(qid)) {
                cout << "[Question]: " << questions[qid]->getTitle() << endl;
            }
        }
    }
    void printAnswer(const string& qid){
        if(!questions.count(qid)){
            cout<<"Question doesn,t exist"<<endl;
            return;
        }
        Question* ques=questions[qid].get();
        cout<<"Accepted Answer: ";
        ques->getAcceptedAnswer();
        cout<<"=========All available Answer============="<<endl;
        ques->printAllAnswer();
    }
};
class AnswerManager{
    map<string,unique_ptr<Answer>> answers;
    AnswerManager(){}
    public:
    AnswerManager(const AnswerManager&)=delete;
    AnswerManager& operator=(const AnswerManager&)=delete;
    static AnswerManager& getInstance(){
        static AnswerManager instance;
        return instance;
    }
    void postAnswer(Question* ques,unique_ptr<Answer> ans){
        string ansId=ans->getId();
        ques->addAnswer(ans.get());
        answers[ansId]=move(ans);
        cout<<"[answer]: "<<ansId<<" is posted for ques: "<<ques->getId()<<endl;
    }
};
class CommentManager{
    map<string,unique_ptr<Comment>> comments;
    map<string,map<string,string>> userPostComments;//uid,pid,cid
    CommentManager(){}
    public:
    CommentManager(const CommentManager&)=delete;
    CommentManager& operator=(const CommentManager&)=delete;
    static CommentManager& getInstance(){
        static CommentManager instance;
        return instance;
    }
    void postComment(const string& uid,Post* post,unique_ptr<Comment> comment){
        post->addComment(comment.get());
        comment->setParent(post->getId());
        string cid=comment->getId();
        comments[cid]=move(comment);
        userPostComments[uid][post->getId()]=cid;
        cout<<"comment is posted for Post: "<<post->getId()<<endl;
    }
    void deleteComment(const string& uid,Post* post){
        if(!userPostComments.count(uid)){
            cout<<"user has not posted any comments"<<endl;
            return;
        }
        if(!userPostComments[uid].count(post->getId())){
            cout<<"user has no comments on this post"<<endl;
            return;
        }
        string cid=userPostComments[uid][post->getId()];
        post->removeComment(cid);
        comments.erase(cid);
        userPostComments[uid].erase(post->getId());
        cout<<"Comment is succesfully deleted"<<endl;

    }
};
class ReputationStrategy{
    public:
    virtual void updateReputation(User* user)=0;
    virtual ~ReputationStrategy()=default;
};
class PostQuesReputation:public ReputationStrategy{
    const int score=2;
    public:
    void updateReputation(User* user){
        user->updateReputation(score);
    }
};
class PostAnsReputation:public ReputationStrategy{
    const int score=3;
    public:
    void updateReputation(User* user){
        user->updateReputation(score);
    }
};
class AnsAcceptedReputation:public ReputationStrategy{
    const int score=5;
    public:
    void updateReputation(User* user){
        user->updateReputation(score);
    }
};
class CommentReputation:public ReputationStrategy{
    const int score=1;
    public:
    void updateReputation(User* user){
        user->updateReputation(score);
    }
};
class SearchStrategy{
    protected:
    QuestionManager& manager;
    public:
    SearchStrategy():manager(QuestionManager::getInstance()){}
    virtual void search(const string& keyword)=0;
    virtual ~SearchStrategy()=default;
};
class SearchByTag:public SearchStrategy{
    public:
    SearchByTag():SearchStrategy(){}
    void search(const string& keyword) override {
        manager.searchByTag(keyword);
    }

};
class SearchByUser:public SearchStrategy{
    public:
    SearchByUser():SearchStrategy(){}
    void search(const string& keyword) override {
        manager.searchByUser(keyword);
    }

};
class StackOverFlow{
    map<string,unique_ptr<User>> users;
    QuestionManager& quesManager;
    AnswerManager& ansManager;
    CommentManager& commentManager;
    TagManager& tagManager;
    StackOverFlow()
        : quesManager(QuestionManager::getInstance()),
          ansManager(AnswerManager::getInstance()),
          commentManager(CommentManager::getInstance()),
          tagManager(TagManager::getInstance()) {}
    public:
    map<string,Post*> allPosts;
    StackOverFlow(const StackOverFlow&)=delete;
    StackOverFlow& operator=(const StackOverFlow&)=delete;
    static StackOverFlow& getInstance(){
        static StackOverFlow instance;
        return instance;
    }
    void addUser(unique_ptr<User> user){
        string uid=user->getId();
        if(users.count(uid)){
            cout<<"User is already added"<<endl;
            return;
        }
        users[uid]=move(user);
        cout<<"[User]: "<<uid<<" added"<<endl;
    }
    Tag* getTag(const string& name){
        return tagManager.getTag(name);
    }
    void postQues(ReputationStrategy* reputStrat,unique_ptr<Question> ques){
        User* user=ques->getAuthor();
        if(!users.count(user->getId())){
            cout<<"User not Exists"<<endl;
            return;
        }
        reputStrat->updateReputation(user);
        allPosts[ques->getId()]=ques.get();
        quesManager.postQuestion(move(ques));
        
    }
    void acceptAnswer(ReputationStrategy* reputStrat,const string& uid,const string& qid,Answer* ans){
        User* user=ans->getAuthor();
        reputStrat->updateReputation(user);
        quesManager.acceptAnswer(uid,qid,ans);
    }
    void postAnswer(ReputationStrategy* reputStrat,const string& quesId,unique_ptr<Answer> ans){
        Question* ques=quesManager.getQuesById(quesId);
        if(!ques){
            cout<<"Question can,t be answered"<<endl;
            return;
        }
        User* user=ans->getAuthor();
        reputStrat->updateReputation(user);
        allPosts[ans->getId()]=ans.get();
        ansManager.postAnswer(ques,move(ans));
    }

    void postComment(ReputationStrategy* reputStrat,const string& uid,const string& pid,unique_ptr<Comment> comment){
        if(!users.count(uid)){
            cout<<"User doesn,t exits"<<endl;
            return;
        }
        if(!allPosts.count(pid)){
            cout<<"Can,t comment on this post"<<endl;
            return;
        }
        User* user=users[uid].get();
        Post* post=allPosts[pid];
        reputStrat->updateReputation(user);
        allPosts[comment->getId()]=comment.get();
        commentManager.postComment(uid,post,move(comment));
    }
    void deleteComment(const string& uid,const string& pid){
        //not deleting from allPost kepping for company purpose
        if(!allPosts.count(pid)){
            cout<<"Can,t comment on this post"<<endl;
            return;
        }
        Post* post=allPosts[pid];
        commentManager.deleteComment(uid,post);
    }

    void Vote(const string& uid,VoteType type,const string& pid){
        if(!users.count(uid)){
            cout<<"User can,t vote"<<endl;
            return;
        }
        if(!allPosts.count(pid)){
            cout<<"post doesn,t exits"<<endl;
            return;
        }
        Post* post=allPosts[pid];
        User* user=users[uid].get();
        post->addVote(user,type);
    }
    void removeVote(const string& uid,const string& pid){
        if(!users.count(uid)){
            cout<<"User can,t vote"<<endl;
            return;
        }
        if(!allPosts.count(pid)){
            cout<<"post doesn,t exits"<<endl;
            return;
        }
        Post* post=allPosts[pid];
        User* user=users[uid].get();
        post->removeVote(user);
    }
    void printVoters(const string& pid){
        if(!allPosts.count(pid)){
            cout<<"post doesn,t exits"<<endl;
            return;
        }
        Post* post=allPosts[pid];
        post->printVoters();
    }
    void printComments(const string& pid){
        if(!allPosts.count(pid)){
            cout<<"post doesn,t exits"<<endl;
            return;
        }
        Post* post=allPosts[pid];
        post->printComments();
    }
    void searchQues(SearchStrategy* strat,const string& keyword){
        strat->search(keyword);
    }
    void printQuesAns(const string& qid){
        Question* ques=quesManager.getQuesById(qid);
        if(!ques){
            cout<<"Question doesn,t exist for answer"<<endl;
            return;
        }
        cout<<"[Question: ]"<<ques->getContent()<<endl;
        quesManager.printAnswer(qid);
    }
    void printVoteCount(const string& pid){
        if(!allPosts.count(pid)){
            cout<<"post doesn,t exits"<<endl;
            return;
        }
        Post* post=allPosts[pid];
        cout<<"Upvote: "<<post->getUpvote()<<"      "<<"DownUpvote: "<<post->getDownvote()<<endl;
        
    }

};
class Utility{
    public:
    static string name(int len = 6) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist('a', 'z');

        string name;
        for (int i = 0; i < len; i++) {
            name += char(dist(gen));
        }
        return name;
    }
    static void printUserReputations(const vector<User*>& users) {
        cout << "\n========== USER REPUTATION LEADERBOARD ==========\n";
        for (auto user : users) {
            cout << "User: " << user->getName()
                << " | ID: " << user->getId()
                << " | Reputation: " << user->getReputation()
                << endl;
        }
        cout << "================================================\n";
    }

};
int main() {

    StackOverFlow& app = StackOverFlow::getInstance();

    // ---------- Reputation strategies ----------
    PostQuesReputation quesRep;
    PostAnsReputation ansRep;
    AnsAcceptedReputation acceptRep;
    CommentReputation commentRep;

    // ---------- Create Users ----------
    vector<User*> users;
    for (int i = 0; i < 12; i++) {
        auto u = make_unique<User>(Utility::name());
        users.push_back(u.get());
        app.addUser(move(u));
    }

    // ---------- Tags ----------
    Tag* cpp    = app.getTag("C++");
    Tag* oop    = app.getTag("OOP");
    Tag* stl    = app.getTag("STL");
    Tag* lld    = app.getTag("LLD");
    Tag* design = app.getTag("DesignPatterns");

    // ---------- Questions ----------
    vector<string> qids;

    vector<string> titles = {
        "What is RAII in C++?",
        "Difference between struct and class?",
        "Why virtual destructor is important?",
        "Explain Rule of Five",
        "What is Diamond Problem?",
        "How shared_ptr works internally?",
        "Explain move semantics",
        "What is const correctness?",
        "When to use mutable keyword?",
        "Shallow vs Deep Copy?",
        "Explain Strategy Pattern",
        "Factory vs Abstract Factory",
        "Observer Pattern use cases",
        "State vs Strategy Pattern",
        "Decorator Pattern example",
        "How unordered_map works?",
        "map vs unordered_map",
        "How vector grows internally?",
        "Why prefer unique_ptr?",
        "What is type erasure?"
    };

    vector<string> contents = {
        "Explain RAII with real-life example.",
        "Explain differences and usage.",
        "Explain memory safety.",
        "Explain copy/move semantics.",
        "Explain multiple inheritance issue.",
        "Explain reference counting.",
        "Explain rvalue references.",
        "Explain const usage.",
        "Explain logical constness.",
        "Explain copy behavior.",
        "Explain runtime behavior selection.",
        "Explain object creation patterns.",
        "Explain publish-subscribe model.",
        "Explain behavioral difference.",
        "Explain dynamic extension.",
        "Explain hashing mechanism.",
        "Explain ordering vs hashing.",
        "Explain capacity growth.",
        "Explain ownership semantics.",
        "Explain hiding concrete types."
    };

    for (int i = 0; i < 20; i++) {
        auto q = make_unique<Question>(
            titles[i],
            cpp,
            contents[i],
            users[i % users.size()]
        );

        if (i % 2 == 0) q->addTag(oop);
        if (i % 3 == 0) q->addTag(stl);
        if (i % 4 == 0) q->addTag(lld);
        if (i % 5 == 0) q->addTag(design);

        qids.push_back(q->getId());
        app.postQues(&quesRep, move(q));
    }

    // ---------- Answers ----------
    vector<string> answers = {
        "RAII binds resource lifetime to object lifetime.",
        "struct is public; class is private.",
        "Virtual destructor prevents memory leaks.",
        "Rule of Five manages resources safely.",
        "Diamond problem solved by virtual inheritance.",
        "shared_ptr uses reference counting.",
        "Move semantics avoids unnecessary copies.",
        "Const correctness improves safety.",
        "mutable allows logical constness.",
        "Deep copy duplicates resources.",
        "Strategy selects algorithms at runtime.",
        "Abstract Factory creates object families.",
        "Observer supports event notification.",
        "State changes behavior internally.",
        "Decorator adds behavior dynamically.",
        "unordered_map uses hashing.",
        "map is ordered, unordered_map is not.",
        "vector grows geometrically.",
        "unique_ptr enforces ownership.",
        "Type erasure hides implementation."
    };

    vector<Answer*> answerPtrs;

    for (int i = 0; i < 20; i++) {
        auto ans = make_unique<Answer>(
            answers[i],
            users[(i + 3) % users.size()]
        );
        answerPtrs.push_back(ans.get());
        app.postAnswer(&ansRep, qids[i], move(ans));
    }

    // ---------- Accept Answers ----------
    for (int i = 0; i < 5; i++) {
        app.acceptAnswer(
            &acceptRep,
            users[i]->getId(),
            qids[i],
            answerPtrs[i]
        );
    }

    // ---------- Collect ALL Post IDs ----------
    vector<string> postIds;
    for (auto& [pid, _] : app.allPosts) {
        postIds.push_back(pid);
    }

    // ---------- Comments ----------
    vector<string> comments = {
        "Very clear explanation.",
        "Helped me understand.",
        "Good example.",
        "Nice explanation.",
        "Well written.",
        "Thanks!",
        "Short and precise.",
        "Helpful.",
        "Good comparison.",
        "Excellent."
    };

    string deleteCommentUser, deleteCommentPost;

    for (int i = 0; i < 10; i++) {
        auto c = make_unique<Comment>(
            comments[i],
            users[(i + 6) % users.size()]
        );

        string pid = postIds[i % postIds.size()];

        if (i == 0) {
            deleteCommentUser = users[(i + 6) % users.size()]->getId();
            deleteCommentPost = pid;
        }

        app.postComment(
            &commentRep,
            users[(i + 6) % users.size()]->getId(),
            pid,
            move(c)
        );
    }

    // ---------- 50 VOTES ----------
    cout << "\n===== Performing 50 Votes =====\n";
    vector<pair<string,string>> voteLog;

    for (int i = 0; i < 100; i++) {
        string uid = users[i % users.size()]->getId();
        string pid = postIds[i % postIds.size()];
        app.Vote(uid, VoteType::UPVOTE, pid);
        voteLog.push_back({uid, pid});
    }

    // ---------- REMOVE 12 VOTES ----------
    cout << "\n===== Removing 12 Votes =====\n";
    for (int i = 0; i < 12; i++) {
        app.removeVote(voteLog[i].first, voteLog[i].second);
    }

    // ---------- DELETE COMMENT ----------
    cout << "\n===== Deleting Comment =====\n";
    app.deleteComment(deleteCommentUser, deleteCommentPost);

    // ---------- Search ----------
    SearchByTag searchTag;
    SearchByUser searchUser;
    app.searchQues(&searchTag, "C++");
    app.searchQues(&searchUser, users[0]->getId());

    // ---------- Display ----------
    app.printQuesAns(qids[0]);
    app.printComments(qids[0]);
    app.printVoters(qids[0]);

    // ---------- Reputation ----------
    Utility::printUserReputations(users);

    //----------vote coutns-------- 
    cout<<"+++++++++VoteCount++++++++++++++++"<<endl; 
    for(auto &id:postIds){ app.printVoteCount(id); }

    return 0;
}
