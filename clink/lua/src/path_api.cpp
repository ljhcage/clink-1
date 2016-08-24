// Copyright (c) 2015 Martin Ridgers
// License: http://opensource.org/licenses/MIT

#include "pch.h"
#include "lua_state.h"

#include <core/base.h>
#include <core/path.h>
#include <core/str.h>

//------------------------------------------------------------------------------
static const char* get_string(lua_State* state, int index)
{
    if (lua_gettop(state) < index || !lua_isstring(state, index))
        return nullptr;

    return lua_tostring(state, index);
}

//------------------------------------------------------------------------------
static int clean(lua_State* state)
{
    str<288> out(get_string(state, 1));
    if (out.length() == 0)
        return 0;

    const char* separator = get_string(state, 2);
    if (separator == nullptr)
        separator = "\\";

    path::clean(out, separator[0]);
    lua_pushstring(state, out.c_str());
    return 1;
}

//------------------------------------------------------------------------------
static int get_base_name(lua_State* state)
{
    const char* path = get_string(state, 1);
    if (path == nullptr)
        return 0;

    str<288> out;
    path::get_base_name(path, out);
    lua_pushstring(state, out.c_str());
    return 1;
}

//------------------------------------------------------------------------------
static int get_directory(lua_State* state)
{
    str<288> out(get_string(state, 1));
    if (out.length() == 0)
        return 0;

    if (!path::get_directory(out))
        return 0;

    lua_pushstring(state, out.c_str());
    return 1;
}

//------------------------------------------------------------------------------
static int get_drive(lua_State* state)
{
    str<8> out(get_string(state, 1));
    if (out.length() == 0)
        return 0;

    if (!path::get_drive(out))
        return 0;

    lua_pushstring(state, out.c_str());
    return 1;
}

//------------------------------------------------------------------------------
static int get_extension(lua_State* state)
{
    const char* path = get_string(state, 1);
    if (path == nullptr)
        return 0;

    str<32> ext;
    path::get_extension(path, ext);
    lua_pushstring(state, ext.c_str());
    return 1;
}

//------------------------------------------------------------------------------
static int get_name(lua_State* state)
{
    const char* path = get_string(state, 1);
    if (path == nullptr)
        return 0;

    str<> name;
    path::get_name(path, name);
    lua_pushstring(state, name.c_str());
    return 1;
}

//------------------------------------------------------------------------------
static int join(lua_State* state)
{
    const char* lhs = get_string(state, 1);
    if (lhs == nullptr)
        return 0;

    const char* rhs = get_string(state, 2);
    if (rhs == nullptr)
        return 0;

    str<288> out;
    path::join(lhs, rhs, out);
    lua_pushstring(state, out.c_str());
    return 1;
}

//------------------------------------------------------------------------------
void path_lua_initialise(lua_state& lua)
{
    struct {
        const char* name;
        int         (*method)(lua_State*);
    } methods[] = {
        { "clean",         &clean },
        { "getbasename",   &get_base_name },
        { "getdirectory",  &get_directory },
        { "getdrive",      &get_drive },
        { "getextension",  &get_extension },
        { "getname",       &get_name },
        { "join",          &join },
    };

    lua_State* state = lua.get_state();

    lua_createtable(state, sizeof_array(methods), 0);

    for (int i = 0; i < sizeof_array(methods); ++i)
    {
        lua_pushstring(state, methods[i].name);
        lua_pushcfunction(state, methods[i].method);
        lua_rawset(state, -3);
    }

    lua_setglobal(state, "path");
}
