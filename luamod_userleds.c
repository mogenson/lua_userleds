/****************************************************************************
 * apps/external/lua_userleds/luamod_userleds.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include <nuttx/config.h>
#include <nuttx/leds/userled.h>

#include <lauxlib.h>
#include <lua.h>

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int userleds_ioctl(lua_State *L);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: userleds_ioctl
 *
 *   Call ioctl for userleds command.
 *
 ****************************************************************************/

static int userleds_ioctl(lua_State *L)
{
  /* Get fd from first argument on stack */

  luaL_Stream *p = (luaL_Stream *)luaL_checkudata(L, 1, LUA_FILEHANDLE);
  int fd = fileno(p->f);
  int command = luaL_checkinteger(L, 2);

  switch (command)
    {
      case ULEDIOC_SUPPORTED:
        {
          userled_set_t userled_set = 0;
          if (ioctl(fd, ULEDIOC_SUPPORTED, &userled_set) < 0)
            {
              goto error;
            }
          lua_pushinteger(L, (int)userled_set);
          return 1;
        }
        break;
      case ULEDIOC_SETLED:
        {
          int led = luaL_checkinteger(L, 3);
          luaL_argcheck(L, (led >= 0) && (led <= 255), 3, 0);
          luaL_checkany(L, 4); /* make sure argument 4 exists */
          bool on = lua_toboolean(L, 4);
          struct userled_s userled = {.ul_led = (uint8_t)led, .ul_on = on};
          if (ioctl(fd, ULEDIOC_SETLED, &userled) < 0)
            {
              goto error;
            }
          return 0;
        }
        break;
      case ULEDIOC_SETALL:
        {
          userled_set_t userled_set = (userled_set_t)luaL_checkinteger(L, 3);
          if (ioctl(fd, ULEDIOC_SETALL, userled_set) < 0)
            {
              goto error;
            }
          return 0;
        }
        break;
      case ULEDIOC_GETALL:
        {
          userled_set_t userled_set = 0;
          if (ioctl(fd, ULEDIOC_GETALL, &userled_set) < 0)
            {
              goto error;
            }
          lua_pushinteger(L, (int)userled_set);
          return 1;
        }
        break;

      default:
        return luaL_error(L, "Unknown ioctl command: %d\n", command);
    }

  return 0; /* should never get here */

error:
  return luaL_error(L, "ioctl command %d failed: %d\n", command, errno);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: luaopen_userleds
 *
 *   Open the "userleds" Lua module.
 *
 ****************************************************************************/

int luaopen_userleds(lua_State *L)
{
  /* add the "ioctl" method to the "FILE*" type's metamethod table */

  luaL_getmetatable(L, LUA_FILEHANDLE);
  lua_getfield(L, -1, "__index");
  lua_pushcfunction(L, userleds_ioctl);
  lua_setfield(L, -2, "ioctl");

  /* create global "ioctls" table of ioctl commands */

  lua_createtable(L, 0, 4);
  lua_pushinteger(L, ULEDIOC_SUPPORTED);
  lua_setfield(L, -2, "ULEDIOC_SUPPORTED");
  lua_pushinteger(L, ULEDIOC_SETLED);
  lua_setfield(L, -2, "ULEDIOC_SETLED");
  lua_pushinteger(L, ULEDIOC_SETALL);
  lua_setfield(L, -2, "ULEDIOC_SETALL");
  lua_pushinteger(L, ULEDIOC_GETALL);
  lua_setfield(L, -2, "ULEDIOC_GETALL");
  lua_setglobal(L, "ioctls");

  return 0;
}
