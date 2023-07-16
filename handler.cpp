#include "handler.hpp"
#include <array>
#include <charconv>
#include <complex>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <stack>
#include <stdio.h>
#include <string>
#include <vector>

int depth = 0;
static std::string value;
std::string tagName;
std::map<std::string, std::string> properties;
std::stack<ProgramState *> programStack;
std::map<int, std::string> depthMap;
std::vector<Token> tokens;
std::stack<int> tokenCache;
std::map<std::string, int> programMap;

void run(int argc, char *argv[]) {

  for (int i = 0; i < tokens.size(); i++) {
    Token token = tokens[i];
    if (!programStack.empty() && programStack.top()->LOGGING) {
      printf(
          "LOG: {depth: %i, end: %i, startToken: %i, value: %s, name: %s }\n",
          token.depth, (int)token.end, token.startToken, token.value.c_str(),
          token.tagName.c_str());
    }
    if (!token.end) {
      if (token.tagName == std::string("PROGRAM")) {
        programStack.push(new ProgramState(token.properties));
      }
      if (token.tagName == std::string("CALL")) {
        programStack.top()->vars["name"] = token.properties["name"];
      }
    } else {
      if (token.tagName == "CALL") { // Switch is broken in cpp i am sorry.
        std::string callName = programStack.top()->vars["name"];

        if (programMap.count(callName) == 0) {
          printf("ERROR: Call name \"%s\" not found\n", callName.c_str());
          exit(1);
        }

        i = programMap[callName] - 1;
        programStack.pop();
        continue;
      } else if (token.tagName == "QUIT") {
        exit(0);
      } else if (token.tagName == "ARG") {
        int index = std::stoi(token.properties["index"]);
        if (index >= argc) {
          printf("ERROR: Index %i is greater that argc %i", index, argc);
          exit(1);
        }
        depthMap[token.depth] = std::string(argv[index]);

        if (programStack.top()->LOGGING) {
          printf("LOG: Depth map `%i` was set with value `%s`\n", token.depth,
                 depthMap[token.depth].c_str());
        }

      } else if (token.tagName == "SET") {
        std::string varName;
        std::string varValue = token.properties["value"];

        if (depthMap.count(token.depth + 1) == 1) {
          varValue = depthMap[token.depth + 1];
        }

        if (token.properties.find("name") != token.properties.end()) {
          varName = token.properties["name"];
        }

        else {
          printf("ERROR: Exiting bc your code doesnt have a var name.\n");
          exit(-1);
        }

        programStack.top()->vars[varName] = varValue;
        if (programStack.top()->LOGGING) {
          printf("LOG: Var `%s` was set with value `%s`\n", varName.c_str(),
                 varValue.c_str());
        }

        depthMap[token.depth] = varValue;

        if (programStack.top()->LOGGING) {
          printf("LOG: Depth map `%i` was set with value `%s`\n", token.depth,
                 varValue.c_str());
        }
      } else if (token.tagName == "PROGRAM") {
        programStack.pop();
      } else if (token.tagName == "VAR") {
        std::string varName;

        if (token.properties.find("name") != token.properties.end()) {
          varName = token.properties["name"];
        } else {
          printf("ERROR: Exiting bc your code doesnt have a var name.\n");
          exit(-1);
        }

        depthMap[token.depth] = programStack.top()->vars[varName];
      } else if (token.tagName == "PRINTLN") {

        std::string printValue = "";

        if (depthMap.count(token.depth + 1) == 1) {
          printValue = depthMap[token.depth + 1];
        } else if (programStack.top()->LOGGING) {
          printf("LOG: Nothing to print\n");
        }

        printf("%s\n", printValue.c_str());
      }
      depthMap.erase(token.depth + 1);
    }
  }
}
std::map<std::string, std::string> makeMap(char **atts) {
  std::map<std::string, std::string> ret;
  for (int i = 0; atts[i]; i += 2) {

    ret[std::string(atts[i])] = std::string(atts[i + 1]);
  }
  return ret;
}
void startElement(void *userData, const XML_Char *name, const char **atts) {
  tagName = std::string(name);
  properties = makeMap((char **)atts);

  depth++;
  Token cache = {depth, false, -1, "", tagName, properties};
  tokens.push_back(cache);
  tokenCache.push(tokens.size() - 1);
  if (tagName != "PROGRAM")
    return;
  if (properties.find("name") != properties.end()) {
    programMap[properties["name"]] = tokens.size() - 1;
  } else {
    printf("ERROR: Exiting bc your code doesnt have a program name.\n");
    exit(-1);
  }
}
void endElement(void *userData, const XML_Char *name) {
  int startToken = tokenCache.top();
  tokenCache.pop();
  tokens.push_back({depth, true, startToken, tokens[startToken].value, name,
                    tokens[startToken].properties});
  tokens[startToken].startToken = tokens.size() - 1;

  depth--;
}

void elementData(void *userData, const char *content, int length) {
  std::string fullBuff(content);
  value = fullBuff.substr(0, length);
  tokens[tokens.size() - 1].value = value;

  // std::cout << "START value" << value << "END value";
}

ProgramState::ProgramState(std::map<std::string, std::string> atts) {
  LOGGING = false;

  if (atts.find("logging") != atts.end()) {
    LOGGING = true;
    printf("LOG: Loging enabled\n");
  }
}

int *getDepth() { return &depth; }
