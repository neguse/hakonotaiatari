
#include "precompile.h"
#include "variable.h"

#include <sys/types.h>
#include <sys/stat.h>

namespace {

	static std::map<std::string, std::string> s_map;
	static const char* VARIABLE_FILE = "..\\src\\variable.csv";
	static struct _stat s_stat;
}

u32 VariableHolder::get_u32( const std::string& name )
{
	std::map<std::string, std::string>::iterator hit = s_map.find(name);
	if (hit != s_map.end()) {
		return strtoul(hit->second.c_str(), NULL, 0);
	} else {
		return 0;
	}
}

f32 VariableHolder::get_f32( const std::string& name )
{
	std::map<std::string, std::string>::iterator hit = s_map.find(name);
	if (hit != s_map.end()) {
		return strtod(hit->second.c_str(), NULL);
	} else {
		return 0.f;
	}
}

void VariableHolder::init()
{
	_stat(VARIABLE_FILE, &s_stat);
	load(VARIABLE_FILE);
}

void VariableHolder::update()
{
	struct _stat newstat;
	_stat(VARIABLE_FILE, &newstat);
	if (newstat.st_mtime != s_stat.st_mtime) {
		init();
	}
}

void VariableHolder::load( const char* pPath )
{
	std::ifstream file(pPath);
	s_map.clear();
	char linebuf[1024];
	while (!file.eof()) {
		char databuf[4][1024];
		file.getline(linebuf, sizeof(linebuf));
		int count = sscanf(linebuf, "%[^,],%[^,],%[^,],%[^,]", databuf[0], databuf[1], databuf[2]);
		if (count == 3) {
			s_map.insert(std::map<std::string, std::string>::value_type(databuf[1], databuf[2]));
		}
	}

}

