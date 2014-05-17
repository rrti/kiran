#ifndef KIRAN_LUA_PARSER_HDR
#define KIRAN_LUA_PARSER_HDR

#include <list>
#include <map>
#include <string>

#include "../math/vec3fwd.hpp"

struct lua_State;

struct LuaTable {
public:
	bool operator < (const LuaTable& t) const { return (this < &t); }
	bool operator == (const LuaTable&) const;

	~LuaTable();

	void Print(int) const;
	void Parse(lua_State*, int);

	typedef std::pair<LuaTable*, LuaTable*>     TblTblPair;
	typedef std::pair<LuaTable*, std::string>   TblStrPair;
	typedef std::pair<LuaTable*, float>         TblFltPair;
	typedef std::pair<std::string, LuaTable*>   StrTblPair;
	typedef std::pair<std::string, std::string> StrStrPair;
	typedef std::pair<std::string, float>       StrFltPair;
	typedef std::pair<int, LuaTable*>           IntTblPair;
	typedef std::pair<int, std::string>         IntStrPair;
	typedef std::pair<int, float>               IntFltPair;

	void GetTblTblKeys(std::list<LuaTable*>*  ) const;
	void GetTblStrKeys(std::list<LuaTable*>*  ) const;
	void GetTblFltKeys(std::list<LuaTable*>*  ) const;
	void GetStrTblKeys(std::list<std::string>*) const;
	void GetStrStrKeys(std::list<std::string>*) const;
	void GetStrFltKeys(std::list<std::string>*) const;
	void GetIntTblKeys(std::list<int>*        ) const;
	void GetIntStrKeys(std::list<int>*        ) const;
	void GetIntFltKeys(std::list<int>*        ) const;

	const LuaTable* GetTblVal(LuaTable*, LuaTable* defVal = 0) const;
	const LuaTable* GetTblVal(const std::string&, LuaTable* defVal = 0) const;
	const LuaTable* GetTblVal(int, LuaTable* defVal = 0) const;
	const std::string& GetStrVal(LuaTable*, const std::string& defVal) const;
	const std::string& GetStrVal(const std::string&, const std::string& defVal) const;
	const std::string& GetStrVal(int, const std::string& defVal) const;
	float GetFltVal(LuaTable*, float defVal) const;
	float GetFltVal(const std::string&, float defVal) const;
	float GetFltVal(int, float defVal) const;

	bool HasStrTblKey(const std::string& key) const { return (StrTblPairs.find(key) != StrTblPairs.end()); }
	bool HasStrStrKey(const std::string& key) const { return (StrStrPairs.find(key) != StrStrPairs.end()); }
	bool HasStrFltKey(const std::string& key) const { return (StrFltPairs.find(key) != StrFltPairs.end()); }



	template<typename T> void GetArray(const LuaTable* tbl, T* array, int len) const {
		// Lua is one-based, so the first element in
		// a table like t={0, 1, 2, 3} is at index 1
		for (int i = 0; i < len; i++) {
			array[i] = T(tbl->GetFltVal(i + 1, T(0)));
		}
	}

	// special accessor: this converts a plain table value into
	// a <len>-component templatized vector; we assume that all
	// vectors are written as "t = {0, 1, 2, 3}" in scripts
	template<typename V> V GetVec(const std::string& key, int len) const {
		const std::map<std::string, LuaTable*>::const_iterator it = StrTblPairs.find(key);

		V v;

		if (it != StrTblPairs.end()) {
			GetArray(it->second, &v.x, len);
		}

		return v;
	}

private:
	std::map<LuaTable*, LuaTable*>     TblTblPairs;
	std::map<LuaTable*, std::string>   TblStrPairs;
	std::map<LuaTable*, float>         TblFltPairs;
	std::map<std::string, LuaTable*>   StrTblPairs;
	std::map<std::string, std::string> StrStrPairs;
	std::map<std::string, float>       StrFltPairs;
	std::map<int, LuaTable*>           IntTblPairs;
	std::map<int, std::string>         IntStrPairs;
	std::map<int, float>               IntFltPairs;
};




struct LuaParser {
public:
	LuaParser();
	~LuaParser();

	bool Execute(const std::string&, const std::string&);

	const LuaTable* GetRootTbl() const { return root; }
	const std::string& GetError() const { return error; }

private:
	lua_State* L;
	LuaTable* root;

	std::map<std::string, LuaTable*> tables;
	std::string error;
};

#endif
