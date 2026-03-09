#include "lexer.hpp"
#include <cctype>
#include <unordered_map>

static std::unordered_map<std::string, TokenType> keywords = {

{"module",TokenType::MODULE},
{"use",TokenType::USE},
{"fn",TokenType::FN},
{"struct",TokenType::STRUCT},
{"enum",TokenType::ENUM},
{"trait",TokenType::TRAIT},
{"const",TokenType::CONST},
{"pub",TokenType::PUB},

{"if",TokenType::IF},
{"else",TokenType::ELSE},
{"while",TokenType::WHILE},
{"for",TokenType::FOR},
{"in",TokenType::IN},
{"match",TokenType::MATCH},
{"return",TokenType::RETURN},
{"break",TokenType::BREAK},
{"continue",TokenType::CONTINUE},
{"region",TokenType::REGION},

{"true",TokenType::TRUE},
{"false",TokenType::FALSE}

};

Lexer::Lexer(const std::string& src):source(src){}

char Lexer::advance(){return source[pos++];}

char Lexer::peek(){ if(pos>=source.size()) return '\0'; return source[pos]; }

char Lexer::peek_next(){ if(pos+1>=source.size()) return '\0'; return source[pos+1]; }

bool Lexer::is_at_end(){ return pos>=source.size(); }

void Lexer::skip_whitespace(){

while(!is_at_end()){

char c=peek();

if(c==' '||c=='\t'||c=='\r'){advance();}
else if(c=='\n'){advance();line++;column=1;}
else break;

}

}

Token Lexer::identifier(){

size_t start=pos;

while(isalnum(peek())||peek()=='_')
advance();

std::string text=source.substr(start,pos-start);

if(keywords.count(text))
return{keywords[text],text,line,column};

return{TokenType::IDENT,text,line,column};

}

Token Lexer::number(){

size_t start=pos;

while(isdigit(peek())) advance();

std::string text=source.substr(start,pos-start);

return{TokenType::INTEGER,text,line,column};

}

Token Lexer::string(){

advance();

size_t start=pos;

while(peek()!='"'&&!is_at_end())
advance();

std::string text=source.substr(start,pos-start);

advance();

return{TokenType::STRING,text,line,column};

}

std::vector<Token> Lexer::tokenize(){

std::vector<Token> tokens;

while(!is_at_end()){

skip_whitespace();

char c=peek();

if(isalpha(c)||c=='_'){
tokens.push_back(identifier());
continue;
}

if(isdigit(c)){
tokens.push_back(number());
continue;
}

if(c=='"'){
tokens.push_back(string());
continue;
}

switch(c){

case '+':
if(peek_next()=='='){advance();advance();tokens.push_back({TokenType::PLUS_EQ,"+=",line,column});}
else{advance();tokens.push_back({TokenType::PLUS,"+",line,column});}
break;

case '-':
if(peek_next()=='='){advance();advance();tokens.push_back({TokenType::MINUS_EQ,"-=",line,column});}
else{advance();tokens.push_back({TokenType::MINUS,"-",line,column});}
break;

case '*':
if(peek_next()=='='){advance();advance();tokens.push_back({TokenType::STAR_EQ,"*=",line,column});}
else{advance();tokens.push_back({TokenType::STAR,"*",line,column});}
break;

case '/':
if(peek_next()=='='){advance();advance();tokens.push_back({TokenType::SLASH_EQ,"/=",line,column});}
else{advance();tokens.push_back({TokenType::SLASH,"/",line,column});}
break;

case '%':
if(peek_next()=='='){advance();advance();tokens.push_back({TokenType::MOD_EQ,"%=",line,column});}
else{advance();tokens.push_back({TokenType::MOD,"%",line,column});}
break;

case '&':
if(peek_next()=='&'){advance();advance();tokens.push_back({TokenType::AND_AND,"&&",line,column});}
break;

case '|':
if(peek_next()=='|'){advance();advance();tokens.push_back({TokenType::OR_OR,"||",line,column});}
break;

case '!':
advance();
tokens.push_back({TokenType::BANG,"!",line,column});
break;

case '.':
if(peek_next()=='.'){
advance();
advance();
if(peek()=='='){advance();tokens.push_back({TokenType::DOT_DOT_EQ,"..=",line,column});}
else tokens.push_back({TokenType::DOT_DOT,"..",line,column});
}
else{advance();tokens.push_back({TokenType::DOT,".",line,column});}
break;

case '(':
advance();tokens.push_back({TokenType::LPAREN,"(",line,column});break;

case ')':
advance();tokens.push_back({TokenType::RPAREN,")",line,column});break;

case '{':
advance();tokens.push_back({TokenType::LBRACE,"{",line,column});break;

case '}':
advance();tokens.push_back({TokenType::RBRACE,"}",line,column});break;

case '[':
advance();tokens.push_back({TokenType::LBRACKET,"[",line,column});break;

case ']':
advance();tokens.push_back({TokenType::RBRACKET,"]",line,column});break;

case ',':
advance();tokens.push_back({TokenType::COMMA,",",line,column});break;

case ':':
advance();tokens.push_back({TokenType::COLON,":",line,column});break;

default:
advance();
break;

}

}

tokens.push_back({TokenType::END,"",line,column});

return tokens;

}
