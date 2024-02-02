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
            //��������������� ����� Key ������ �� Value, �� StartDict � �� StartArray
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
        //�-���
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


        //����������� ��� ���������� �������  
        bool map_builder_blocker = false;
        //����- ���
        Node nullnode{};

        //��� ��������
        enum class ElementType {
            IS_SIMPLE_NODE, IS_KEY, BORDER_OPEN_ARRAY, BORDER_CLOSE_ARRAY, BORDER_OPEN_DICT, BORDER_CLOSE_DICT
        };

        //��������� �������� ������� ������� 
        //std::variant ������ �����������... ������� ��������� ���������� ��������
        //����������� ������������� �� ������������� �������
        struct RequestBorder {
            ElementType el_type;
            Node data;
            std::string keyname;
        };
       
        //������� ���� ������� �������
        std::vector<RequestBorder> central_DB_request_;

        //���� ����������� � �����
        enum class WhatContainerNowLoading {
            EMPTY, ARRAY, DICTIONARY
        };

        //��� ������� ������
        enum class LastCall {
            VALUE, START_ARR, START_DICT, END_ARR, END_DICT, CONSTRUCTOR, BUILD, KEY
        };

        //������� �����������
        struct ContainersCounters {
            int arrays = 0;
            int dictionaries = 0;
        };

        //���������� DOC (�� ������������ - �� ����� �� �������)
        struct DocumentConstructor {
            Document operator()(nullptr_t&) { return  Document(Node(nullptr)); }
            Document operator()(const Array& arr) { return  Document(Node(arr)); }
            Document operator()(const Dict& dct) { return  Document(Node(dct)); }
            Document operator()(bool val) { return Document(Node(val)); }
            Document operator()(const std::string& val) { return  Document(Node(val)); }
            Document operator()(int val) { return  Document(Node(val)); }
            Document operator()(double val) { return  Document(Node(val)); }
        };

        //���������� NOD
        struct NodeConstructor {
            Node operator()(nullptr_t&) { return Node(nullptr); }
            Node operator()(const Array& arr) { return Node(arr); }
            Node operator()(const Dict& dct) { return Node(dct); }
            Node operator()(bool val) { return Node(val); }
            Node operator()(const std::string& val) { return Node(val); }
            Node operator()(int val) { return  Node(val); }
            Node operator()(double val) { return  Node(val); }
        };

        //���� ��������� �����������
        std::stack<WhatContainerNowLoading> now_loading_container_;
        
        //������� ����� 
        LastCall last_call_;

        //������� �����������
        ContainersCounters containers_count_;

        //Json - ������
        json::Node root_;

      // 1 � 2 �������� � ������ ��� � ����-����� �������� ����������
      /*1*/  void BuildArray(int begin, int end);
      /*2*/  void BuildDict(int begin, int end);

      //���������������� ����������� ������ �������
        Builder& StartDictBuilder();
      //���������������� ����������� ������ �������  
        Builder& StartArrayBuilder();
       //���������������� ����������� �������� �����
        Builder& KeyBuilder(std::string key);
        // friend class Context;
        
    };

}

