#include <cassert>
#include <iostream>

#include <boost/foreach.hpp>
#include <lua5.1/lua.hpp>

#include "./LuaParser.hpp"

bool LuaTable::operator == (const LuaTable& t) const {
	return (
		TblTblPairs == t.TblTblPairs &&
		TblStrPairs == t.TblStrPairs &&
		TblFltPairs == t.TblFltPairs &&
		StrTblPairs == t.StrTblPairs &&
		StrStrPairs == t.StrStrPairs &&
		StrFltPairs == t.StrFltPairs &&
		IntTblPairs == t.IntTblPairs &&
		IntStrPairs == t.IntStrPairs &&
		IntFltPairs == t.IntFltPairs
	);
}

LuaTable::~LuaTable() {
	for (std::map<LuaTable*, LuaTable*>::iterator it = TblTblPairs.begin(); it != TblTblPairs.end(); it++) {
		delete it->first;
		delete it->second;
	}
	for (std::map<LuaTable*, std::string>::iterator it = TblStrPairs.begin(); it != TblStrPairs.end(); it++) {
		delete it->first;
	}
	for (std::map<LuaTable*, float>::iterator it = TblFltPairs.begin(); it != TblFltPairs.end(); it++) {
		delete it->first;
	}
	for (std::map<std::string, LuaTable*>::iterator it = StrTblPairs.begin(); it != StrTblPairs.end(); it++) {
		delete it->second;
	}
	for (std::map<int, LuaTable*>::iterator it = IntTblPairs.begin(); it != IntTblPairs.end(); it++) {
		delete it->second;
	}
}

void LuaTable::Print(int depth) const {
	std::string tabs = "";
	for (int i = 0; i < depth; i++) {
		tabs += "\t";
	}

	for (std::map<LuaTable*, LuaTable*>::const_iterator it = TblTblPairs.begin(); it != TblTblPairs.end(); it++) {
		std::cout << tabs << "k<tbl>: ";
		std::cout << std::endl;
			it->first->Print(depth + 1);
		std::cout << tabs << "v<tbl>: ";
		std::cout << std::endl;
			it->second->Print(depth + 1);
	}
	for (std::map<LuaTable*, std::string>::const_iterator it = TblStrPairs.begin(); it != TblStrPairs.end(); it++) {
		std::cout << tabs << "k<tbl>: ";
		std::cout << std::endl;
			it->first->Print(depth + 1);
		std::cout << tabs << "v<str>: " << it->second;
		std::cout << std::endl;
	}
	for (std::map<LuaTable*, float>::const_iterator it = TblFltPairs.begin(); it != TblFltPairs.end(); it++) {
		std::cout << tabs << "k<tbl>: ";
		std::cout << std::endl;
			it->first->Print(depth + 1);
		std::cout << tabs << "v<float>: " << it->second;
		std::cout << std::endl;
	}

	for (std::map<std::string, LuaTable*>::const_iterator it = StrTblPairs.begin(); it != StrTblPairs.end(); it++) {
		std::cout << tabs << "k<str>: " << it->first;
		std::cout << ", v<tbl>: ";
		std::cout << std::endl;
			it->second->Print(depth + 1);
	}
	for (std::map<std::string, std::string>::const_iterator it = StrStrPairs.begin(); it != StrStrPairs.end(); it++) {
		std::cout << tabs << "k<str>: " << it->first;
		std::cout << ", v<str>: " << it->second;
		std::cout << std::endl;
	}
	for (std::map<std::string, float>::const_iterator it = StrFltPairs.begin(); it != StrFltPairs.end(); it++) {
		std::cout << tabs << "k<str>: " << it->first;
		std::cout << ", v<float>: " << it->second;
		std::cout << std::endl;
	}

	for (std::map<int, LuaTable*>::const_iterator it = IntTblPairs.begin(); it != IntTblPairs.end(); it++) {
		std::cout << tabs << "k<int>: " << it->first;
		std::cout << ", v<tbl>: ";
		std::cout << std::endl;
			it->second->Print(depth + 1);
	}
	for (std::map<int, std::string>::const_iterator it = IntStrPairs.begin(); it != IntStrPairs.end(); it++) {
		std::cout << tabs << "k<int>: " << it->first;
		std::cout << ", v<str>: " << it->second;
		std::cout << std::endl;
	}
	for (std::map<int, float>::const_iterator it = IntFltPairs.begin(); it != IntFltPairs.end(); it++) {
		std::cout << tabs << "k<int>: " << it->first;
		std::cout << ", v<float>: " << it->second;
		std::cout << std::endl;
	}
}

void LuaTable::Parse(lua_State* L, int depth) {
	assert(lua_istable(L, -1));
	lua_pushnil(L);
	assert(lua_istable(L, -2));


	while (lua_next(L, -2) != 0) {
		assert(lua_istable(L, -3));

		switch (lua_type(L, -2)) {
			case LUA_TTABLE: {
				LuaTable* key = new LuaTable();

				switch (lua_type(L, -1)) {
					case LUA_TTABLE: {
						TblTblPairs[key] = new LuaTable();
						TblTblPairs[key]->Parse(L, depth + 1);

						lua_pop(L, 1);

						key->Parse(L, depth + 1);
					} break;
					case LUA_TSTRING: {
						TblStrPairs[key] = lua_tostring(L, -1);
						lua_pop(L, 1);

						key->Parse(L, depth + 1);
					} break;
					case LUA_TNUMBER: {
						TblFltPairs[key] = lua_tonumber(L, -1);
						lua_pop(L, 1);

						key->Parse(L, depth + 1);
					} break;
				}

				continue;
			} break;

			case LUA_TSTRING: {
				const std::string key = lua_tostring(L, -2);

				switch (lua_type(L, -1)) {
					case LUA_TTABLE: {
						StrTblPairs[key] = new LuaTable();
						StrTblPairs[key]->Parse(L, depth + 1);
					} break;
					case LUA_TSTRING: {
						StrStrPairs[key] = lua_tostring(L, -1);
					} break;
					case LUA_TNUMBER: {
						StrFltPairs[key] = lua_tonumber(L, -1);
					} break;
				}

				lua_pop(L, 1);
				continue;
			} break;

			case LUA_TNUMBER: {
				const int key = lua_tointeger(L, -2);

				switch (lua_type(L, -1)) {
					case LUA_TTABLE: {
						IntTblPairs[key] = new LuaTable();
						IntTblPairs[key]->Parse(L, depth + 1);
					} break;
					case LUA_TSTRING: {
						IntStrPairs[key] = lua_tostring(L, -1);
					} break;
					case LUA_TNUMBER: {
						IntFltPairs[key] = lua_tonumber(L, -1);
					} break;
				}

				lua_pop(L, 1);
				continue;
			} break;
		}
	}

	assert(lua_istable(L, -1));
}



void LuaTable::GetTblTblKeys(std::list<LuaTable*>*   keys) const { BOOST_FOREACH(TblTblPair p, TblTblPairs) { keys->push_back(p.first); } }
void LuaTable::GetTblStrKeys(std::list<LuaTable*>*   keys) const { BOOST_FOREACH(TblStrPair p, TblStrPairs) { keys->push_back(p.first); } }
void LuaTable::GetTblFltKeys(std::list<LuaTable*>*   keys) const { BOOST_FOREACH(TblFltPair p, TblFltPairs) { keys->push_back(p.first); } }
void LuaTable::GetStrTblKeys(std::list<std::string>* keys) const { BOOST_FOREACH(StrTblPair p, StrTblPairs) { keys->push_back(p.first); } }
void LuaTable::GetStrStrKeys(std::list<std::string>* keys) const { BOOST_FOREACH(StrStrPair p, StrStrPairs) { keys->push_back(p.first); } }
void LuaTable::GetStrFltKeys(std::list<std::string>* keys) const { BOOST_FOREACH(StrFltPair p, StrFltPairs) { keys->push_back(p.first); } }
void LuaTable::GetIntTblKeys(std::list<int>*         keys) const { BOOST_FOREACH(IntTblPair p, IntTblPairs) { keys->push_back(p.first); } }
void LuaTable::GetIntStrKeys(std::list<int>*         keys) const { BOOST_FOREACH(IntStrPair p, IntStrPairs) { keys->push_back(p.first); } }
void LuaTable::GetIntFltKeys(std::list<int>*         keys) const { BOOST_FOREACH(IntFltPair p, IntFltPairs) { keys->push_back(p.first); } }



const LuaTable* LuaTable::GetTblVal(LuaTable* key, LuaTable* defVal) const {
	const std::map<LuaTable*, LuaTable*>::const_iterator it = TblTblPairs.find(key);
	return ((it != TblTblPairs.end())? it->second: defVal);
}
const LuaTable* LuaTable::GetTblVal(const std::string& key, LuaTable* defVal) const {
	const std::map<std::string, LuaTable*>::const_iterator it = StrTblPairs.find(key);
	return ((it != StrTblPairs.end())? it->second: defVal);
}
const LuaTable* LuaTable::GetTblVal(int key, LuaTable* defVal) const {
	const std::map<int, LuaTable*>::const_iterator it = IntTblPairs.find(key);
	return ((it != IntTblPairs.end())? it->second: defVal);
}

const std::string& LuaTable::GetStrVal(LuaTable* key, const std::string& defVal) const {
	const std::map<LuaTable*, std::string>::const_iterator it = TblStrPairs.find(key);
	return ((it != TblStrPairs.end())? it->second: defVal);
}
const std::string& LuaTable::GetStrVal(const std::string& key, const std::string& defVal) const {
	const std::map<std::string, std::string>::const_iterator it = StrStrPairs.find(key);
	return ((it != StrStrPairs.end())? it->second: defVal);
}
const std::string& LuaTable::GetStrVal(int key, const std::string& defVal) const {
	const std::map<int, std::string>::const_iterator it = IntStrPairs.find(key);
	return ((it != IntStrPairs.end())? it->second: defVal);
}

float LuaTable::GetFltVal(LuaTable* key, float defVal) const {
	const std::map<LuaTable*, float>::const_iterator it = TblFltPairs.find(key);
	return ((it != TblFltPairs.end())? it->second: defVal);
}
float LuaTable::GetFltVal(const std::string& key, float defVal) const {
	const std::map<std::string, float>::const_iterator it = StrFltPairs.find(key);
	return ((it != StrFltPairs.end())? it->second: defVal);
}
float LuaTable::GetFltVal(int key, float defVal) const {
	const std::map<int, float>::const_iterator it = IntFltPairs.find(key);
	return ((it != IntFltPairs.end())? it->second: defVal);
}



LuaParser::LuaParser(): L(NULL), root(NULL) {
	L = lua_open();
	luaL_openlibs(L);
}
LuaParser::~LuaParser() {
	for (std::map<std::string, LuaTable*>::iterator it = tables.begin(); it != tables.end(); it++) {
		delete it->second;
	}

	lua_close(L);
	root = NULL;
}

bool LuaParser::Execute(const std::string& file, const std::string& table) {
	bool ret = false;

	int loadErr = 0; // 0 | LUA_ERRFILE | LUA_ERRSYNTAX | LUA_ERRMEM
	int callErr = 0; // 0 | LUA_ERRRUN | LUA_ERRMEM | LUA_ERRERR

	if ((loadErr = luaL_loadfile(L, file.c_str())) != 0 || (callErr = lua_pcall(L, 0, 0, 0)) != 0) {
		error = "[LuaParser::Execute] " + std::string(lua_tostring(L, -1));
		lua_pop(L, 1);
		return false;
	}

	if (tables.find(file) == tables.end()) {
		tables[file] = new LuaTable();
		root = tables[file];

		assert(lua_gettop(L) == 0);
		lua_getglobal(L, table.c_str());

		if (lua_isnil(L, -1) == 0) {
			assert(lua_istable(L, -1));
			root->Parse(L, 0);
			ret = true;
		} else {
			error = "[LuaParser::Execute] no global variable \'" + table + "\' in chunk \'" + file + "\'";
		}

		lua_pop(L, 1);
		assert(lua_gettop(L) == 0);
	} else {
		root = tables[file];
		ret = true;
	}

	return ret;
}
