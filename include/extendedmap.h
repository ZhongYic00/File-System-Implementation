#ifndef EXTENDEDMAP_H
#define EXTENDEDMAP_H
#include <map>
using std::map, std::string;
class ExtendedMap {
public:
    ExtendedMap();
    void insert(const string& key, const string& value);
    string get(const string& key);

private:
    map<string, string> attributes;
};

#endif // EXTENDEDMAP_H
