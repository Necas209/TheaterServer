#pragma once
#include <list>
#include "Show.h"

class Theater final
{
public:
	// attributes
	std::string name;
	std::string location;
	std::list<Show> shows;
	// methods
	Theater();
	~Theater();
	// serializer and deserializer
	friend void to_json(json& j, const Theater& t);
	friend void from_json(const json& j, Theater& t);
};