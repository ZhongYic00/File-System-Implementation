#ifndef EXTENDEDMAP_H
#define EXTENDEDMAP_H
#include <map>
#include <string>
using std::map, std::string;
//using std::;
class ExtendedMap {
public:
    ExtendedMap();
    void insert(const string& key, const string& value);
    string get(const string& key);

private:
    map<string, string> attributes;
};

#endif // EXTENDEDMAP_H
