#include "extendedmap.h"
#include <iostream>
using std::cerr, std::endl;
ExtendedMap::ExtendedMap()
{
}
void ExtendedMap::insert(const string& key, const string& value)
{
    attributes[key] = value;
}
string ExtendedMap::get(const string& key)
{
    if (attributes.find(key) == attributes.end())
        cerr << "Warning: attribute\"" << key << "\" not found in extMap" << endl;
    return attributes[key];
}
