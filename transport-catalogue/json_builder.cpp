#include "json_builder.h"

namespace json {

    StArrayHelper::StArrayHelper(Builder& br) : b(br){};    

    StArrayHelper StArrayHelper::StartArray() {
        if (b.formed_) {
            throw std::logic_error("Invoking StartArray is not allowed"s);
        }
        if (b.nodes_stack_.empty()) {
            b.root_ = Array{};
            b.nodes_stack_.push_back(&b.root_);
        } else {        
            Node* current = b.nodes_stack_.back();
            if (current->IsArray()) {
                current->AsArray().emplace_back(Array{});
                b.nodes_stack_.push_back(&current->AsArray().back());
            }        
        }
        return StArrayHelper(b);
    }

    EndArrayHelper StArrayHelper::EndArray() {
        if (b.formed_ || b.nodes_stack_.empty()) {
            throw std::logic_error("Invoking EndArray is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();
        if (current->IsArray()) {
            b.nodes_stack_.pop_back();
            if (b.nodes_stack_.empty()) {
                b.formed_ = true;
            }       
        }
        return EndArrayHelper(b);
    }
    
    StDictHelper StArrayHelper::StartDict() {
        if (b.formed_) {
            throw std::logic_error("Invoking StartArray is not allowed"s);
        }
        if (b.nodes_stack_.empty()) {
            b.root_ = Dict{};
            b.nodes_stack_.push_back(&b.root_);
        } else {
            Node* current = b.nodes_stack_.back();
            if (current->IsArray()) {
                current->AsArray().emplace_back(Dict{});
                b.nodes_stack_.push_back(&current->AsArray().back());
            } else if (current->IsMap() && b.key_) {
                current->AsMap()[b.key_.value()] = Dict{};
                b.nodes_stack_.emplace_back(&current->AsMap()[b.key_.value()]);
                b.key_.reset();
            }
        }
        return StDictHelper(b);
    }
    
    ArrValueHelper StArrayHelper::Value(Node value) {
        if (b.formed_) {
            throw std::logic_error("Invoking Value is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();        
        if (current->IsArray()) {
            current->AsArray().push_back(value);
        } else if (current->IsMap() && b.key_) {
            current->AsMap()[b.key_.value()] = value;
            b.key_.reset();
        }
        return ArrValueHelper(b);
    }
    
    StDictHelper::StDictHelper(Builder& br) : b(br){}
    
    EndDictHelper StDictHelper::EndDict() {
        if (b.formed_ || b.nodes_stack_.empty() || b.key_) {
            throw std::logic_error("Invoking EndDict is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();
        if (current->IsMap()) {
            b.nodes_stack_.pop_back();
            if (b.nodes_stack_.empty()) {
                b.formed_ = true;
            }
        } 
        return EndDictHelper(b);
    }
    
    EndDictHelper::EndDictHelper(Builder& br) : b(br){}
    
    ArrValueHelper EndDictHelper::Value(Node value) {
        if (b.formed_) {
            throw std::logic_error("Invoking Value is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();        
        if (current->IsArray()) {
            current->AsArray().push_back(value);
        }
        return ArrValueHelper(b);
    }
    
    EndDictHelper EndDictHelper::EndArray() {
        if (b.formed_ || b.nodes_stack_.empty()) {
            throw std::logic_error("Invoking EndArray is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();
        if (current->IsArray()) {
            b.nodes_stack_.pop_back();
            if (b.nodes_stack_.empty()) {
                b.formed_ = true;
            }       
        } else {
            throw std::logic_error("Invoking EndArray is not allowed"s);
        }
        return EndDictHelper(b);
    }
    
    EndDictHelper EndArrayHelper::EndDict() {
        if (b.formed_ || b.nodes_stack_.empty() || b.key_) {
            throw std::logic_error("Invoking EndDict is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();
        if (current->IsMap()) {
            b.nodes_stack_.pop_back();
            if (b.nodes_stack_.empty()) {
                b.formed_ = true;
            }
        } else {
            throw std::logic_error("Invoking EndDict is not allowed"s);
        }        
        return EndDictHelper(b);
    }
    
    EndArrayHelper EndArrayHelper::EndArray() {
        if (b.formed_ || b.nodes_stack_.empty()) {
            throw std::logic_error("Invoking EndArray is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();
        if (current->IsArray()) {
            b.nodes_stack_.pop_back();
            if (b.nodes_stack_.empty()) {
                b.formed_ = true;
            }
        }
        return EndArrayHelper(b);
    }
    ArrValueHelper EndArrayHelper::Value(Node value) {
        if (b.formed_) {
            throw std::logic_error("Invoking Value is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();        
        if (current->IsArray()) {
            current->AsArray().push_back(value);
        }
        return ArrValueHelper(b);
    }
    
    KeyHelper EndArrayHelper::Key(std::string key) {
        if (b.formed_ || b.nodes_stack_.empty()) {
            throw std::logic_error("Invoking Key is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();
        if (current->IsMap()) {
            b.key_ = key;
        }
        return KeyHelper(b);
    }
    
    StDictHelper EndDictHelper::StartDict() {
        if (b.formed_) {
            throw std::logic_error("Invoking StartDict is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();
        if (current->IsArray()) {
            current->AsArray().emplace_back(Dict{});
            b.nodes_stack_.push_back(&current->AsArray().back());
        }
        return StDictHelper(b);
    }
    
    EndDictHelper EndDictHelper::EndDict() {
        if (b.formed_ || b.nodes_stack_.empty() || b.key_) {
            throw std::logic_error("Invoking EndDict is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();
        if (current->IsMap()) {
            b.nodes_stack_.pop_back();
            if (b.nodes_stack_.empty()) {
                b.formed_ = true;
            }
        }     
        return EndDictHelper(b);
    }
    
    EndDictHelper DictValueHelper::EndDict() {
        if (b.formed_ || b.nodes_stack_.empty() || b.key_) {
            throw std::logic_error("Invoking EndDict is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();
        if (current->IsMap()) {
            b.nodes_stack_.pop_back();
            if (b.nodes_stack_.empty()) {
                b.formed_ = true;
            }
        }      
        return EndDictHelper(b);
    }

    EndArrayHelper::EndArrayHelper(Builder& br) : b(br){};

    Node EndArrayHelper::Build() {
        return b.Build();
    }
    
    Node EndDictHelper::Build() {
        return b.Build();
    }
    
    KeyHelper EndDictHelper::Key(std::string key) {
        if (b.formed_ || b.nodes_stack_.empty()) {
            throw std::logic_error("Invoking Key is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();
        if (current->IsMap()) {
            b.key_ = key;
        } 
        return KeyHelper(b);
    }
    
    Node ValueHelper::Build() {
        return b.Build();
    }
    
    ValueHelper::ValueHelper(Builder& br) : b(br){}; 
    ArrValueHelper::ArrValueHelper(Builder& br) : b(br){}; 
    DictValueHelper::DictValueHelper(Builder& br) : b(br){};     
    KeyHelper::KeyHelper(Builder& br) : b(br){};
    
    EndArrayHelper ArrValueHelper::EndArray() {
        if (b.formed_ || b.nodes_stack_.empty()) {
            throw std::logic_error("Invoking EndArray is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();
        if (current->IsArray()) {
            b.nodes_stack_.pop_back();
            if (b.nodes_stack_.empty()) {
                b.formed_ = true;
            }       
        }
        return EndArrayHelper(b); 
    }
    
    StDictHelper ArrValueHelper::StartDict() {
        if (b.formed_) {
            throw std::logic_error("Invoking StartDict is not allowed"s);
        }        
        Node* current = b.nodes_stack_.back();
        if (current->IsArray()) {
            current->AsArray().emplace_back(Dict{});
            b.nodes_stack_.push_back(&current->AsArray().back());
        }
        return StDictHelper(b);
    }
    
    KeyHelper DictValueHelper::Key(std::string key) {
        if (b.formed_ || b.nodes_stack_.empty()) {
            throw std::logic_error("Invoking Key is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();
        if (current->IsMap()) {
            b.key_ = key;
        } 
        return KeyHelper(b);
    }
    
    ArrValueHelper ArrValueHelper::Value(Node value) {
        if (b.formed_) {
            throw std::logic_error("Invoking Value is not allowed"s);
        }        
        Node* current = b.nodes_stack_.back();        
        if (current->IsArray()) {
            current->AsArray().push_back(value);
        }
        return ArrValueHelper(b);
    }
    
    KeyHelper StDictHelper::Key(std::string key) {
        if (b.formed_ || b.nodes_stack_.empty()) {
            throw std::logic_error("Invoking Key is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();
        if (current->IsMap()) {
            b.key_ = key;
        }
        return KeyHelper(b);
    }
    
    DictValueHelper KeyHelper::Value(Node value) {
        if (b.formed_) {
            throw std::logic_error("Invoking Value is not allowed"s);
        }
        Node* current = b.nodes_stack_.back();        
        if (current->IsMap() && b.key_) {
            current->AsMap()[b.key_.value()] = value;
            b.key_.reset();
        }        
        return DictValueHelper(b);
    }
    
    StArrayHelper KeyHelper::StartArray() {
        if (b.formed_) {
            throw std::logic_error("Invoking StartArray is not allowed"s);
        }        
        Node* current = b.nodes_stack_.back();
        if (current->IsMap() && b.key_) {
            current->AsMap()[b.key_.value()] = Array{};
            b.nodes_stack_.push_back(&current->AsMap()[b.key_.value()]);
            b.key_.reset();                       
        }
        return StArrayHelper(b);
    }
    
    StDictHelper KeyHelper::StartDict() {
        if (b.formed_) {
            throw std::logic_error("Invoking StartDict is not allowed"s);
        }        
        Node* current = b.nodes_stack_.back();
        if (current->IsMap() && b.key_) {
            current->AsMap()[b.key_.value()] = Dict{};
            b.nodes_stack_.emplace_back(&current->AsMap()[b.key_.value()]);
            b.key_.reset();                       
        }
        return StDictHelper(b);
    }

    ValueHelper Builder::Value(Node value) {       
        if (formed_) {
            throw std::logic_error("Invoking Value is not allowed"s);
        }
        if (nodes_stack_.empty()) {
            root_ = value;
            formed_ = true;
            return *this;
        }        
        return ValueHelper(*this);
    }
    
    StDictHelper Builder::StartDict() {
        if (formed_) {
            throw std::logic_error("Invoking StartDict is not allowed"s);
        }
        if (nodes_stack_.empty()) {
            root_ = Dict{};
            nodes_stack_.push_back(&root_);
        }
        return StDictHelper(*this);
    }
    
    StArrayHelper Builder::StartArray() {
        if (formed_) {
            throw std::logic_error("Invoking StartArray is not allowed"s);
        }
        if (nodes_stack_.empty()) {
            root_ = Array{};
            nodes_stack_.push_back(&root_);
        }
        return StArrayHelper(*this);
    }
    
    Node Builder::Build() {
        if (formed_) {
           return root_; 
        } else {
            throw std::logic_error("Invoking Build is not allowed"s);
        }
        return root_;
    }
    
} //namespace json