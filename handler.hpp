#ifndef HANDLER_H
#define HANDLER_H

#include <expat.h>
#include <map>
#include <string>

int *getDepth();
void startElement(void *userData, const XML_Char *name, const char **atts);
void endElement(void *userData, const XML_Char *name);
void elementData(void *data, const char *content, int length);
void run(int argc, char *argv[]);

class ProgramState {
public:
  ProgramState(std::map<std::string, std::string> atts);
  ~ProgramState();

  bool LOGGING;

  std::map<std::string, std::string> vars;
};

struct Token {
  int depth;
  bool end;
  int startToken;
  std::string value;
  std::string tagName;
  std::map<std::string, std::string> properties;
};

#endif // !HANDLER_H
