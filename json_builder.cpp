
#include"json_builder.h"
namespace json {

    //////////////////////////////////////////////////////////
    //                                                      //
    //                  BUILDER PUBLIC                      // 
    //                                                      //
    //////////////////////////////////////////////////////////
    
    ///////////////
    //DICTCONTEXT
    //////////////
    
    
    Builder::KeyContext  Builder::DictContext::Key(std::string value) {
        Builder::KeyContext  kk(main_base_);
        main_base_->KeyBuilder(std::move(value));
        return kk;
    }
    
    Builder& Builder::DictContext::EndDict() {
        return main_base_->EndDict();
    }


   ///////////////
   // KEYCONTEXT
   //////////////
    Builder::DictContext Builder::KeyContext::StartDict() {
        return main_base_->StartDict();
    }

    Builder::DictContext Builder::KeyContext::Value(ValueObject value) {
        DictContext dc(main_base_);
        main_base_->Value(std::move(value));
        return dc;
    }

    Builder::ArrayContext Builder::KeyContext::StartArray() {
        ArrayContext array_(main_base_);
        array_.StartArray();
        return array_;
    }

   ///////////////
   // ARRAYCONTEXT
   //////////////

    Builder::DictContext Builder::ArrayContext::StartDict() {
        return main_base_->StartDict();
    }

    Builder::ArrayContext Builder::ArrayContext::Value(ValueObject value) {
        ArrayContext ac(main_base_);
        main_base_->Value(value);
        return ac;
    }

    Builder::ArrayContext& Builder::ArrayContext::StartArray() {
        main_base_->StartArrayBuilder();
        return *this;
    }

    Builder& Builder::ArrayContext::EndArray() {
        return main_base_->EndArray();
    }
}


namespace json {
    //////////////////////////////////////////////////////////
    //                                                      //
    //                  BUILDER PUBLIC                      // 
    //                                                      //
    //////////////////////////////////////////////////////////
    
    Builder::Builder() {
        last_call_ = (LastCall::CONSTRUCTOR);
        now_loading_container_.push(WhatContainerNowLoading::EMPTY);
    };

    Builder::DictContext Builder::StartDict() {
        DictContext dct(this);
        this->StartDictBuilder();
        return dct;
    }

    Builder::ArrayContext Builder::StartArray() {
        ArrayContext arr(this);
        arr.StartArray();
        return arr;
    }

    Builder::KeyContext Builder::Key(std::string value) {
        KeyContext kc(this);
        this->KeyBuilder(std::move(value));
        return kc;
    }

    Builder& Builder::Value(ValueObject value) {
        
        ///////////////////////////////////////////////////////////////////////////////
        //ИСКЛЮЧЕНИЯ
        //если обьект построен - модификации недоступны
        if (last_call_ == LastCall::BUILD) throw std::logic_error("Build Allready was Called");
       //прошлый вызов - словарь, нужен ключ
        if (last_call_ == LastCall::START_DICT) throw std::logic_error("Latt Call was Dict, next call must be Key");
        //если заблокирован map-предохранитель
        if (map_builder_blocker)throw std::logic_error("Incorrect DICT building");
        //повтор значений без контейнера 
        if (last_call_ == LastCall::VALUE && now_loading_container_.top() != WhatContainerNowLoading::ARRAY)
           throw std::logic_error("Repeat Value without container");
        //////////////////////////////////////////////////////////////////////////////////

        if (last_call_ == LastCall::KEY) {
            map_builder_blocker = true;
        }

        Node nod = std::visit(NodeConstructor{}, value);
        central_DB_request_.push_back({ ElementType::IS_SIMPLE_NODE, nod , {} });

        //модерн стека вызова операций
        last_call_ = (LastCall::VALUE);
        return *this;
    }

    json::Node Builder::Build() {
        
        ///////////////////////////////////////////////////////////////////////////////
        //ИСКЛЮЧЕНИЯ
        //Если еще есть в стеке контейнеры
        if (now_loading_container_.top() != WhatContainerNowLoading::EMPTY)
            throw std::logic_error("Object was not constructed"); 
       //Если сразу после конструктора
        if (last_call_ == LastCall::CONSTRUCTOR)throw std::logic_error("Calling build after constructor");
       /////////////////////////////////////////////////////////////////////////////////
       
        while (central_DB_request_.size() > 1) {
            BuildDict(0, static_cast<int>(central_DB_request_.size() - 1));
        }
        return central_DB_request_[0].data;
    };

    Node& Builder::GetNode() {
        if (central_DB_request_.empty()) return nullnode;
        return central_DB_request_[0].data;

    };

    Builder& Builder::EndArray() {
        ////////////////////////////////////////////////////////////////////////////////////
        //ИСКЛЮЧЕНИЯ
        //если обьект построен - модификации недоступны
        if (last_call_ == LastCall::BUILD) throw std::logic_error("Build Allready was Called");
        //если массивов нет
        if (containers_count_.arrays == 0)throw std::logic_error("No ARRAYS was started");
        //если попытка закрыть не свой контейнер
        if (now_loading_container_.top() == WhatContainerNowLoading::DICTIONARY)
            throw std::logic_error("You Want to end ARRAY, but DICT was started");
        ///////////////////////////////////////////////////////////////////////////////////////
        
        //модерн центральную БД запросов
        central_DB_request_.push_back({ ElementType::BORDER_CLOSE_ARRAY , Node{}, "" });
        //модерн стека вызова операций
        last_call_ = (LastCall::END_ARR);
        // достаем завершенный контейнер со стека
        now_loading_container_.pop();
        return *this;
    };


    Builder& Builder::EndDict() {

        ///////////////////////////////////////////////////////////////////////////////
        //ИСКЛЮЧЕНИЯ 
        //если обьект построен - модификации недоступны
        if (last_call_ == LastCall::BUILD) throw std::logic_error("Build Allready was Called");
        //если словарей нет
        if (containers_count_.dictionaries == 0)throw std::logic_error("No DICTIONARIES was started");
        //если сейчас открыто формирование массива
        if (now_loading_container_.top() == WhatContainerNowLoading::ARRAY)
            throw std::logic_error("You Want to end DICT, but ARRAY was started");
        ////////////////////////////////////////////////////////////////////////////////
        
        
        //модерн центральную БД запросов
        central_DB_request_.push_back({ ElementType::BORDER_CLOSE_DICT, Node{}, "" });
        //модерн стека вызова операций
        last_call_ = (LastCall::END_DICT);
        // достаем завершенный контейнер со стека
        now_loading_container_.pop();
        //разблокируем управляющий созданием DICT флаг
        map_builder_blocker = false;

        return *this;
    };
    
    //////////////////////////////////////////////////////////
    //                                                      //
    //                  BUILDER PRIVATE                     // 
    //                                                      //
    //////////////////////////////////////////////////////////
   void Builder::BuildArray(int begin, int end) {

        if (end - begin == 1) {
            central_DB_request_.erase(central_DB_request_.begin() + begin, central_DB_request_.begin() + end + 1);
            central_DB_request_.insert(central_DB_request_.begin() + begin, { ElementType::IS_SIMPLE_NODE,  Array{} , "" });
            return;
        }
        int Lcatchdict = -1;
        int Rcatchdict = -1;
        for (int i = begin; i <= end; ++i) {
            auto& sw = central_DB_request_[i].el_type;
            if (sw == ElementType::BORDER_OPEN_DICT)  Lcatchdict = i;
            else if (sw == ElementType::BORDER_CLOSE_DICT) {
                Rcatchdict = i;
                break;
            }
        }
        
        if (Lcatchdict != -1 && Rcatchdict != -1) {
            BuildDict(Lcatchdict, Rcatchdict);
        }

        else {
            Array array_;
            for (int i = begin + 1; i < end; ++i) {
                array_.push_back(central_DB_request_[i].data);
            }
            central_DB_request_.erase(central_DB_request_.begin() + begin, central_DB_request_.begin() + end + 1);
            central_DB_request_.insert(central_DB_request_.begin() + begin, { ElementType::IS_SIMPLE_NODE, std::move(array_), "" });
        }
    }


    void Builder::BuildDict(int begin, int end) {

        if (end - begin == 1) {
            //Если в эту функцию попали позиции которые конструируют массив
            if (central_DB_request_[begin].el_type == ElementType::BORDER_OPEN_ARRAY && central_DB_request_[end].el_type == ElementType::BORDER_CLOSE_ARRAY) {
                BuildArray(begin, end);
                return;
            }
            central_DB_request_.erase(central_DB_request_.begin() + begin, central_DB_request_.begin() + end + 1);
            central_DB_request_.insert(central_DB_request_.begin() + begin, { ElementType::IS_SIMPLE_NODE,  Dict{}, "" });
            return;
        }
        int Lcatcharr = -1;
        int Rcatcharr = -1;

        for (int i = begin; i <= end; ++i) {
            auto& sw = central_DB_request_[i].el_type;
            if (sw == ElementType::BORDER_OPEN_ARRAY)  Lcatcharr = i;
            else if (sw == ElementType::BORDER_CLOSE_ARRAY) {
                Rcatcharr = i;
                break;
            }
        }

        if (Lcatcharr != -1 && Rcatcharr != -1) {
            BuildArray(Lcatcharr, Rcatcharr);
        }

        else {
            Dict dictionary_;
            std::optional<Node> nodetemp{std::nullopt};
            std::optional<std::string> strtemp{std::nullopt};

            for (int i = begin + 1; i < end; ++i) {
                auto& sw = central_DB_request_[i].el_type;
                if (sw == ElementType::IS_KEY)   strtemp = std::move(central_DB_request_[i].keyname);
                else if (sw == ElementType::IS_SIMPLE_NODE)  nodetemp = std::move(central_DB_request_[i].data);
                if (nodetemp && strtemp) {
                    dictionary_.insert({ std::move(*strtemp), std::move(*nodetemp) });
                    nodetemp = strtemp = std::nullopt;
                }
            }
            if (!nodetemp && strtemp) { dictionary_.insert({ std::move(*strtemp) , Node{} }); }
            central_DB_request_.erase(central_DB_request_.begin() + begin, central_DB_request_.begin() + end + 1);
            central_DB_request_.insert(central_DB_request_.begin() + begin, { ElementType::IS_SIMPLE_NODE, std::move(dictionary_), "" });
        }
    }

    Builder& Builder::StartDictBuilder() {
       
        ///////////////////////////////////////////////////////////////////////////////
        //ИСКЛЮЧЕНИЯ
        //если обьект построен - модификации недоступны
        if (last_call_ == LastCall::BUILD) throw std::logic_error("Build Allready was Called");
        //если map-блокировщик активен
        if (map_builder_blocker)throw std::logic_error("Incorrect DICT building");
        // прошлый вызов - словарь, нужен ключ
        if (last_call_ == LastCall::START_DICT) throw std::logic_error("Latt Call was Dict, next call must be Key");
        /////////////////////////////////////////////////////////////////////////////////

        // в стек какой сейчас контейнер обрабатываетя
        now_loading_container_.push(WhatContainerNowLoading::DICTIONARY);
        //модерн база учета кол-ва контейнеров и их позиций открытия и закрытия
        ++containers_count_.dictionaries;
        //модерн центральную БД запросов
        central_DB_request_.push_back({ ElementType::BORDER_OPEN_DICT , Node{}, "" });
        //модерн стека вызова операций
        last_call_ = (LastCall::START_DICT);

        return *this;
    };

    Builder& Builder::StartArrayBuilder() {
          
        ///////////////////////////////////////////////////////////////////////////////
        //ИСКЛЮЧЕНИЯ
        //если обьект построен - модификации недоступны
        if (last_call_ == LastCall::BUILD) throw std::logic_error("Build Allready was Called");
        //если map-блокировщик активен          
        if (map_builder_blocker)throw std::logic_error("Incorrect DICT building");
        //прошлый вызов - создание словаря, нужен ключ
        if (last_call_ == LastCall::START_DICT) throw std::logic_error("Latt Call was Dict, next call must be Key");
        ///////////////////////////////////////////////////////////////////////////////
        
        
        // в стек какой сейчас контейнер обрабатываетя
        now_loading_container_.push(WhatContainerNowLoading::ARRAY);
        //модерн база учета кол-ва контейнеров и их позиций открытия и закрытия
        ++containers_count_.arrays;
        //модерн центральную БД запросов
        central_DB_request_.push_back({ ElementType::BORDER_OPEN_ARRAY , Node{}, "" });
        //модерн стека вызова операций
        last_call_ = (LastCall::START_ARR);
       
        return *this;
    };


    Builder& Builder::KeyBuilder(std::string key) {

        //////////////////////////////////////////////////////////////////////////////////////
        //ИСКЛЮЧЕНИЯ
        //если обьект построен - модификации недоступны
        if (last_call_ == LastCall::BUILD) throw std::logic_error("Build Allready was Called");
        //если прежний вызов не словарь
        if (last_call_ == LastCall::CONSTRUCTOR) throw std::logic_error("Key afrer constuctor");
        //если ключ после ключа
        if (last_call_ == LastCall::KEY) throw std::logic_error("Key afrer key");
        //если ключ после создания массива
        if (last_call_ == LastCall::START_ARR) throw std::logic_error("Key afrer making array");
        //////////////////////////////////////////////////////////////////////////////////////
        
        //модерн центральную БД запросов
        central_DB_request_.push_back({ ElementType::IS_KEY, {}, std::move(key) });
        //модерн стека вызова операций
        last_call_ = (LastCall::KEY);
        //разблокируем управляющий созданием DICT флаг
        map_builder_blocker = false;

        return *this;
    };

}

