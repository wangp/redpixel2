#define LUA_NUMBER		long
#define LUA_NUMBER_SCAN		"%ld"
#define LUA_NUMBER_FMT		"%ld"
#define lua_str2number(s,p)     strtol((s), (p))
