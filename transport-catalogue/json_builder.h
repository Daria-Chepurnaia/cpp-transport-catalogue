#pragma once
#include "json.h" 
#include <string>
#include <stdexcept>
#include <optional>
#include <vector>

using namespace std::string_literals;

namespace json {   
  
  class StDictHelper;  
  class EndDictHelper;
  class EndArrayHelper;
  class KeyHelper;
  class ValueHelper;
  class BuildHelper;
  class Builder;
  class DictValueHelper;
  class ArrValueHelper;
    
class StArrayHelper {
public:
    StArrayHelper(Builder& br);    
    StArrayHelper StartArray();
    EndArrayHelper EndArray();
    StDictHelper StartDict();
    ArrValueHelper Value(Node value);
private:
    Builder& b;
};

class EndArrayHelper {
public:
    EndArrayHelper(Builder& br);
    EndDictHelper EndDict();
    ArrValueHelper Value(Node value);
    KeyHelper Key(std::string key);
    EndArrayHelper EndArray();

    Node Build();    
private:
    Builder& b;
};
    
class StDictHelper {
public:
    StDictHelper(Builder& br);     
    EndDictHelper EndDict();
    KeyHelper Key(std::string key);

private:
    Builder& b;   
};
    
class EndDictHelper {
public:    
    EndDictHelper(Builder& br);
    EndDictHelper EndArray();
    StDictHelper StartDict();
    ArrValueHelper Value(Node value);
    KeyHelper Key(std::string key);
    EndDictHelper EndDict();

    Node Build();
private:
    Builder& b;    
};
    
class DictValueHelper {
public:
    DictValueHelper(Builder& br);    
    KeyHelper Key(std::string key);
    EndDictHelper EndDict();
private:
    Builder& b;    
};
    
class ArrValueHelper {
public:
    ArrValueHelper(Builder& br);
    EndArrayHelper EndArray();    
    StDictHelper StartDict();
    ArrValueHelper Value(Node value);
    
private:
    Builder& b;
};
    
class ValueHelper {
public:
    ValueHelper(Builder& br);    
    Node Build();    
private:
    Builder& b;
};
    
class KeyHelper {
public:
    KeyHelper(Builder& br);
    DictValueHelper Value(Node value);
    StArrayHelper StartArray();
    StDictHelper StartDict();
private:
    Builder& b;
};
    
class Builder {
    
  friend class StArrayHelper;
  friend class StDictHelper;
  friend class EndArrayHelper;
  friend class EndDictHelper;
  friend class KeyHelper;
  friend class ValueHelper;
  friend class BuildHelper;
  friend class DictValueHelper;
  friend class ArrValueHelper;
    
  public:   
    Builder() = default;

    ValueHelper Value(Node value);
    StDictHelper StartDict();
    StArrayHelper StartArray();
    Node Build();
    
  private:
    Node root_;
    std::vector<Node*> nodes_stack_;
    bool formed_ = false;
    std::optional<std::string> key_;
};

} //namespace json