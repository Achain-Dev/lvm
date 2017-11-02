/*
    author: pli
    date: 2017.10.19
    The method data
*/

#ifndef _METHOD_DATA_HANDLER_H_
#define _METHOD_DATA_HANDLER_H_

#include <map>
#include <memory>
#include <vector>

struct MethodData {
    std::string                 name;
    std::string                 description;
    std::string                 return_type;
};

class MethodDataHandler {
public:
    MethodDataHandler();
    virtual ~MethodDataHandler();

    MethodData* find_method_data(const std::string& method);

private:
    void  register_methods();
    void  unregister_methods();

private:
    std::map<std::string, MethodData*>  _map_method_datas;
};

typedef std::shared_ptr<MethodDataHandler> MethodDataHandlerPtr;

#endif
