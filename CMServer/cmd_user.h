#pragma once

#define USER_DATA	0X01
typedef enum
{
	USER_ADD = 0X01,
	USER_LOGIN,
	USER_LIST,
}SUBCMD_USER;

bool cmd_user(msgpack::object* pRootArray, BUFFER_OBJ* bobj);