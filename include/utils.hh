#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

inline void getValues(std::vector<double>* x,
               std::vector<double>* y,
               const std::string& name,
               double unitx,
               double unity,
            std::string labelx,std::string labely)
{
    if (!x || !y) throw std::invalid_argument("getValues: x/y == nullptr");

    x->clear();
    y->clear();

    std::ifstream my_in(name);
    if (!my_in) throw std::runtime_error("getValues: nao consegui abrir '" + name + "'");

    json in_json = json::parse(my_in);
    size_t n = in_json.size();

    for (size_t i=0; i<n; i++) 
    {
        x->push_back(in_json[i][labelx].get<double>()*unitx );
        y->push_back(in_json[i][labely].get<double>()*unity );
    }

}
