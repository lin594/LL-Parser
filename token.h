#include <string>
class Token {
   private:
    std::string kind;
    std::string content;

   public:
    Token() : kind{"undefined"}, content{"undefined"} {};
    Token(std::string kind) : kind{kind}, content{kind} {};
    Token(std::string kind, std::string content)
        : kind{kind}, content{content} {};
    std::string getKind() { return kind; }
    std::string getContent() { return content; }
    bool isValid(){
        return !(kind=="undefined"&&content=="undefined");
    }
};