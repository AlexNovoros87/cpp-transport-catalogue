#pragma once
#include<list>
#include<optional>
#include<stack>
#include<vector>
#include<sstream>
#include<queue>

#include"json.h"
namespace json {

    class Builder {
   
        class Context;
        class KeyContext;
        class ArrayContext;
        class DictContext;
       
        class Context{
            Context(Builder* builder) : main_base_(builder) {}
            virtual ~Context() {}

        protected:
            friend class Builder;
            Builder* main_base_;
            virtual void mute() = 0;
        };
  
        class DictContext : Context {
        public:
            DictContext(Builder* builder) : Context(builder) {}
            KeyContext Key(std::string value);
            Builder& EndDict();
        private:
            void mute() override {};
        };

       class KeyContext : Context {
            //Ќепосредственно после Key вызван не Value, не StartDict и не StartArray
        public:
            KeyContext(Builder* builder) : Context(builder) {}
            DictContext StartDict();
            DictContext Value(ValueObject value);
            ArrayContext StartArray();
        private:
            void mute() override {};

        };

        class ArrayContext : Context {
        public:
            ArrayContext(Builder* builder) : Context(builder) {}
            
            DictContext StartDict();  
            ArrayContext Value(ValueObject value);
            ArrayContext& StartArray();
            Builder& EndArray();
        
        private:
            void mute() override {};
        };
        

    public:
        //к-тор
        Builder();
        
        DictContext StartDict();
        ArrayContext StartArray();
        Builder& EndArray();
        Builder& EndDict();
        KeyContext Key(std::string value);
        Builder& Value(ValueObject value);

        Node Build();
        Node& GetNode();

    private:


        //блокировщик при констукции словар€  
        bool map_builder_blocker = false;
        //нуль- нод
        Node nullnode{};

        //тип элемента
        enum class ElementType {
            IS_SIMPLE_NODE, IS_KEY, BORDER_OPEN_ARRAY, BORDER_CLOSE_ARRAY, BORDER_OPEN_DICT, BORDER_CLOSE_DICT
        };

        //структура хран€ща€ единицу запроса 
        //std::variant считаю неактуально... сложный синтаксис извлечени€ значени€
        //невалидного использовани€ не предусмотрено никогда
        struct RequestBorder {
            ElementType el_type;
            Node data;
            std::string keyname;
        };
       
        //√лавна€ база очереди запосов
        std::vector<RequestBorder> central_DB_request_;

        //типы контейнеров в стеке
        enum class WhatContainerNowLoading {
            EMPTY, ARRAY, DICTIONARY
        };

        //тип райнего вызова
        enum class LastCall {
            VALUE, START_ARR, START_DICT, END_ARR, END_DICT, CONSTRUCTOR, BUILD, KEY
        };

        //счетчик контейнеров
        struct ContainersCounters {
            int arrays = 0;
            int dictionaries = 0;
        };

        // онструтор DOC (не используетс€ - но решил не удал€ть)
        struct DocumentConstructor {
            Document operator()(nullptr_t&) { return  Document(Node(nullptr)); }
            Document operator()(const Array& arr) { return  Document(Node(arr)); }
            Document operator()(const Dict& dct) { return  Document(Node(dct)); }
            Document operator()(bool val) { return Document(Node(val)); }
            Document operator()(const std::string& val) { return  Document(Node(val)); }
            Document operator()(int val) { return  Document(Node(val)); }
            Document operator()(double val) { return  Document(Node(val)); }
        };

        // онструтор NOD
        struct NodeConstructor {
            Node operator()(nullptr_t&) { return Node(nullptr); }
            Node operator()(const Array& arr) { return Node(arr); }
            Node operator()(const Dict& dct) { return Node(dct); }
            Node operator()(bool val) { return Node(val); }
            Node operator()(const std::string& val) { return Node(val); }
            Node operator()(int val) { return  Node(val); }
            Node operator()(double val) { return  Node(val); }
        };

        //стек обработки контейнеров
        std::stack<WhatContainerNowLoading> now_loading_container_;
        
        //крайний вызов 
        LastCall last_call_;

        //счетчик контейнеров
        ContainersCounters containers_count_;

        //Json - собран
        json::Node root_;

      // 1 и 2 –аботают в св€зке ища в друг-друге вложеные контейнеры
      /*1*/  void BuildArray(int begin, int end);
      /*2*/  void BuildDict(int begin, int end);

      //Ќепосредственный исполнитель начала словар€
        Builder& StartDictBuilder();
      //Ќепосредственный исполнитель начала массива  
        Builder& StartArrayBuilder();
       //Ќепосредственный исполнитель создани€ ключа
        Builder& KeyBuilder(std::string key);
        // friend class Context;
        
    };

}

