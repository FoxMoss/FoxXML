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
std::map<int, Variable> depthMap;
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
        programStack.top()->vars["name"] =
            Variable(stringType, token.properties["name"]);
      }
    } else {
      if (token.tagName == "CALL") { // Switch is broken in cpp i am sorry.
        if (programStack.top()->vars["name"].type != stringType) {
          printf("ERROR: Variable 'name' is not string.\n");
          exit(1);
        }
        std::string callName = programStack.top()->vars["name"].valueString;

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
          printf("ERROR: Index %i is greater that argc %i \n", index, argc);
          exit(1);
        }
        depthMap[token.depth] = Variable(stringType, std::string(argv[index]));

        if (programStack.top()->LOGGING) {
          printf("LOG: Depth map `%i` was set with value `%s`\n", token.depth,
                 depthMap[token.depth].valueString.c_str());
        }

      } else if (token.tagName == "SET") {
        std::string varName;
        Type varType;

        Variable varValue = Variable(stringToType(token.properties["type"]),
                                     token.properties["value"]);

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
                 varValue.GetString().c_str());
        }

        depthMap[token.depth] = varValue;

        if (programStack.top()->LOGGING) {
          printf("LOG: Depth map `%i` was set with value `%s`\n", token.depth,
                 varValue.GetString().c_str());
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
          printValue = depthMap[token.depth + 1].GetString();
        } else if (programStack.top()->LOGGING) {
          printf("LOG: Nothing to print\n");
        }

        printf("%s\n", printValue.c_str());
      } else if (token.tagName == "ADDINT") {

        if (token.properties.find("val1") == token.properties.end() ||
            token.properties.find("val2") == token.properties.end() ||
            programStack.top()->vars.count(token.properties["val1"]) == 0 ||
            programStack.top()->vars.count(token.properties["val2"]) == 0) {
          printf("ERROR: Vals in ADDINT are undef\n");
          exit(1);
        }
        Variable val1 = programStack.top()->vars[token.properties["val1"]];
        Variable val2 = programStack.top()->vars[token.properties["val2"]];
        if (val1.type != intType || val2.type != intType) {
          printf("ERROR: Vals are not both ints in ADDINT\n");
          exit(1);
        }
        depthMap[token.depth] =
            Variable(intType, std::to_string(val1.valueInt + val2.valueInt));
      }
      depthMap.erase(token.depth + 1);
    }
  }
}
Type stringToType(std::string type) {
  if (type == "string") {

    return stringType;
  }
  if (type == "int") {
    return intType;
  }
  if (type == "float") {
    return floatType;
  }
  return stringType;
}
Variable::Variable(Type cType, std::string value) {
  switch (cType) {
  case stringType:
    valueString = value;
    break;
  case intType:
    valueInt = std::stoi(value);
    break;
  case floatType:
    valueFloat = std::stof(value);
    break;
  }
  type = cType;
}
Variable::~Variable() {}
Variable::Variable() {
  type = stringType;
  valueString = "";
}
std::string Variable::GetString() {
  switch (type) {
  case stringType:
    return valueString;
    break;
  case intType:
    return std::to_string(valueInt);
    break;
  case floatType:
    return std::to_string(valueFloat);
    break;
  }
  return "bad type";
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
