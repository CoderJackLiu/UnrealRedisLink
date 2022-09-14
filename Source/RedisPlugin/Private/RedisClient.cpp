// Copyright (C) 2019 GameSeed - All Rights Reserved

#include "RedisClient.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#endif

#include "hiredis.h"

#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#include <sstream>




URedisClient::URedisClient()
{
	RedisContextPtr = nullptr;
	RedisReplyPtr = nullptr;
	Password = TEXT("");
	Host = TEXT("");
	Port = 0;
	bSubscribed = false;
}

URedisClient::~URedisClient()
{
	DisconnectRedis();
}

bool URedisClient::ConnectToRedis(const FString& InHost, int32 InPort, const FString& InPassword)
{
	Host = InHost;
	Port = InPort;
	Password = InPassword;
	timeval TimeOut = { 1, 0 };// one sec

	RedisContextPtr = redisConnectWithTimeout(TCHAR_TO_ANSI(*Host), Port, TimeOut);
	if (!RedisContextPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Connect redis failed"));
		return false;
	}
	else if (RedisContextPtr->err)
	{
		UE_LOG(LogTemp, Warning, TEXT("Connect redis failed. error = %d"), RedisContextPtr->err);
		redisFree(RedisContextPtr);
		RedisContextPtr = nullptr;
		return false;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "AUTH %s", TCHAR_TO_ANSI(*InPassword));
	if (!RedisReplyPtr)
	{
		redisFree(RedisContextPtr);
		RedisContextPtr = nullptr;
		return false;
	}

	if (RedisReplyPtr->type == REDIS_REPLY_ERROR)
	{
		if (InPassword.IsEmpty())
		{
			FString ErrorString = RedisReplyPtr->str;
			if (ErrorString.Contains(TEXT("ERR Client sent AUTH, but no password is set")))
			{
				return true;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Connect redis failed. error = %d"), RedisContextPtr->err);
		redisFree(RedisContextPtr);
		RedisContextPtr = nullptr;
		return false;
	}

	return true;
}

void URedisClient::DisconnectRedis()
{
	if (RedisReplyPtr)
	{
		freeReplyObject(RedisReplyPtr);
		RedisReplyPtr = nullptr;
	}
	if (RedisContextPtr)
	{
		redisFree(RedisContextPtr);
		RedisContextPtr = nullptr;
	}
}


void URedisClient::Quit()
{
	if (!RedisContextPtr)
	{
		return;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "QUIT");

	DisconnectRedis();
}


bool URedisClient::SelectIndex(int32 InIndex)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "SELECT %d", InIndex);
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::ExecCommand(const FString& InCommand)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "%s", TCHAR_TO_ANSI(*InCommand));
	if (!RedisReplyPtr)
	{
		return bResult;
	}
	
	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}


bool URedisClient::Subscribe(const FString& InChannel)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "SUBSCRIBE %s", TCHAR_TO_ANSI(*InChannel));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
		bSubscribed = true;
	}

// 	freeReplyObject(RedisReplyPtr);
// 	RedisReplyPtr = nullptr;

	return bResult;
}

void URedisClient::Unsubscribe()
{
	bSubscribed = false;
}

bool URedisClient::Subscribed()
{
	return bSubscribed;
}

bool URedisClient::SubscribeReply(FString& Channel, FString& Message)
{
	if (redisGetReply(RedisContextPtr, (void**)&RedisReplyPtr) == REDIS_OK)
	{
		if (RedisReplyPtr->type == REDIS_REPLY_ARRAY && RedisReplyPtr->elements == 3)
		{
			Channel = RedisReplyPtr->element[1]->str;
			Message = RedisReplyPtr->element[2]->str;
			return true;
		}
	}
	return false;
}

bool URedisClient::UnsubscribeChannel(const FString& InChannel)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "UNSUBSCRIBE %s", TCHAR_TO_ANSI(*InChannel));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}


bool URedisClient::Publish(const FString& InChannel, const FString& InMessage)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "PUBLISH %s %s", TCHAR_TO_ANSI(*InChannel), TCHAR_TO_ANSI(*InMessage));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::ExistsKey(const FString& InKey)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "EXISTS %s", TCHAR_TO_ANSI(*InKey));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = (bool)RedisReplyPtr->integer;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::ExpireKey(const FString& InKey, int32 Sec)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "EXPIRE %s %d", TCHAR_TO_ANSI(*InKey), Sec);
	if (!RedisReplyPtr)
	{
		return bResult;
	}
	
	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = (bool)RedisReplyPtr->integer;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::PersistKey(const FString& InKey)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "PERSIST %s", TCHAR_TO_ANSI(*InKey));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = (bool)RedisReplyPtr->integer;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}


bool URedisClient::RenameKey(const FString& CurrentKey, const FString& NewKey)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "RENAME %s %s", TCHAR_TO_ANSI(*CurrentKey), TCHAR_TO_ANSI(*NewKey));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::DelKey(const FString& InKey)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "DEL %s", TCHAR_TO_ANSI(*InKey));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::TypeKey(const FString& InKey, FString& OutType)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "TYPE %s", TCHAR_TO_ANSI(*InKey));
	if (!RedisReplyPtr)
	{
		return bResult;
	}
	
	if (RedisReplyPtr->type == REDIS_REPLY_STATUS)
	{
		OutType = RedisReplyPtr->str;// none string list set zset hash 
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::MSet(TMap<FString, FString>& InMemberMap)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	std::ostringstream redisCmd;
	redisCmd << "MSET";
	for (auto &it : InMemberMap)
	{
		redisCmd << " " << TCHAR_TO_ANSI(*(it.Key)) << " " << TCHAR_TO_ANSI(*(it.Value));
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, redisCmd.str().c_str());
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::MGet(const TArray<FString>& InKeyList, TArray<FString>& OutMemberList)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	std::ostringstream redisCmd;
	redisCmd << "MGET";
	for (auto &it : InKeyList)
	{
		redisCmd << " " << TCHAR_TO_ANSI(*(it));
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, redisCmd.str().c_str());
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type == REDIS_REPLY_ARRAY)
	{
		bResult = true;
		for (auto i = 0; i < RedisReplyPtr->elements; i++)
		{
			if (RedisReplyPtr->element[i]->str != NULL)
			{
				OutMemberList.Add(RedisReplyPtr->element[i]->str);
			}
		}
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::SetInt(const FString& InKey, int32 InValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "SET %s %lld", TCHAR_TO_ANSI(*InKey), InValue);
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::GetInt(const FString& InKey, int32& OutValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "GET %s", TCHAR_TO_ANSI(*InKey));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	switch (RedisReplyPtr->type)
	{
		case REDIS_REPLY_INTEGER:
			{
				OutValue = RedisReplyPtr->integer;
				bResult = true;
			}
			break;
		case REDIS_REPLY_STRING:
			{
				OutValue = atoi(RedisReplyPtr->str);
				bResult = true;
			}
			break;
		default:
			break;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::SetStr(const FString& InKey, const FString& InValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "SET %s %s", TCHAR_TO_ANSI(*InKey), TCHAR_TO_ANSI(*InValue));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::GetStr(const FString& InKey, FString& OutValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "GET %s", TCHAR_TO_ANSI(*InKey));
	if (!RedisReplyPtr)
	{
		return bResult;
	}
	
	if (RedisReplyPtr->type == REDIS_REPLY_STRING)
	{
		OutValue = RedisReplyPtr->str;
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}


bool URedisClient::Append(const FString& InKey, const FString& InValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "APPEND %s %s", TCHAR_TO_ANSI(*InKey), TCHAR_TO_ANSI(*InValue));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::SAdd(const FString& InKey, const TArray<FString>& InMemberList)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	std::ostringstream redisCmd;
	redisCmd << "SADD " << TCHAR_TO_ANSI(*InKey);
	for (auto &it : InMemberList)
	{
		redisCmd << " " << TCHAR_TO_ANSI(*(it));
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, redisCmd.str().c_str());
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}


bool URedisClient::SCard(const FString& InKey, int32& OutValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "SCARD %s ", TCHAR_TO_ANSI(*InKey));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type == REDIS_REPLY_INTEGER)
	{
		bResult = true;
		OutValue = RedisReplyPtr->integer;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::SRem(const FString& InKey, const TArray<FString>& InMemberList)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	std::ostringstream redisCmd;
	redisCmd << "SREM " << TCHAR_TO_ANSI(*InKey);
	for (auto &it : InMemberList)
	{
		redisCmd << " " << TCHAR_TO_ANSI(*(it));
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, redisCmd.str().c_str());
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::SMembers(const FString& InKey, TArray<FString>& OutMemberList)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "SMEMBERS %s ", TCHAR_TO_ANSI(*InKey));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type == REDIS_REPLY_ARRAY)
	{
		bResult = true;
		for (auto i = 0; i < RedisReplyPtr->elements; i++)
		{
			if (RedisReplyPtr->element[i]->str != NULL)
			{
				OutMemberList.Add(RedisReplyPtr->element[i]->str);
			}
		}
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::HSet(const FString& InKey, const FString& InField, const FString& InValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "HSET %s %s %s", TCHAR_TO_ANSI(*InKey), TCHAR_TO_ANSI(*InField), TCHAR_TO_ANSI(*InValue));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::HGet(const FString& InKey, const FString& InField, FString& OutValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "HGET %s %s", TCHAR_TO_ANSI(*InKey), TCHAR_TO_ANSI(*InField));
	if (!RedisReplyPtr)
	{
		return bResult;
	}
	
	if (RedisReplyPtr->type == REDIS_REPLY_STRING)
	{
		OutValue = RedisReplyPtr->str;
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::HIncrby(const FString & InKey, const FString & InField, int32 Incre)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "HINCRBY %s %s %d", TCHAR_TO_ANSI(*InKey), TCHAR_TO_ANSI(*InField), Incre);
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}



bool URedisClient::HMSet(const FString& InKey, const TMap<FString, FString>& InMemberMap)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	std::ostringstream redisCmd;
	redisCmd << "HMSET " << TCHAR_TO_ANSI(*InKey);
	for (auto &it : InMemberMap)
	{
		redisCmd << " " << TCHAR_TO_ANSI(*(it.Key)) << " " << TCHAR_TO_ANSI(*(it.Value));
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, redisCmd.str().c_str());
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::HDel(const FString& InKey, const TArray<FString>& InFieldList)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	std::ostringstream redisCmd;
	redisCmd << "HDEL " << TCHAR_TO_ANSI(*InKey);
	for (auto &it : InFieldList)
	{
		redisCmd << " " << TCHAR_TO_ANSI(*(it));
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, redisCmd.str().c_str());
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}


bool URedisClient::HExists(const FString& InKey, const FString& InField)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "HEXISTS %s %s", TCHAR_TO_ANSI(*InKey), TCHAR_TO_ANSI(*InField));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type == REDIS_REPLY_STRING)
	{
		bResult = (bool)RedisReplyPtr->integer;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::HMGet(const FString& InKey, const TSet<FString>& InFieldList, TMap<FString, FString>& OutMemberMap)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	std::ostringstream redisCmd;
	redisCmd << "HMGET " << TCHAR_TO_ANSI(*InKey);
	for (auto &it : InFieldList)
	{
		redisCmd << " " << TCHAR_TO_ANSI(*it);
		OutMemberMap.Add(it);
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, redisCmd.str().c_str());
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type == REDIS_REPLY_ARRAY)
	{
		int i = 0;
		for (auto &it : OutMemberMap)
		{
			if (i >= RedisReplyPtr->elements)
			{
				break;
			}

			if (RedisReplyPtr->element[i]->str != NULL)
			{
				it.Value = RedisReplyPtr->element[i]->str;
			}

			++i;
		}
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}


bool URedisClient::HGetAll(const FString& InKey, TMap<FString, FString>& OutMemberMap)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "HGETALL %s", TCHAR_TO_ANSI(*InKey));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type == REDIS_REPLY_ARRAY)
	{
		for (auto i = 0; i < RedisReplyPtr->elements; i += 2)
		{
			if (RedisReplyPtr->element[i]->str != NULL && RedisReplyPtr->element[i + 1]->str != NULL)
			{
				OutMemberMap.Add(RedisReplyPtr->element[i]->str, RedisReplyPtr->element[i + 1]->str);
			}
		}
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::LIndex(const FString& InKey, int32 InIndex, FString& OutValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "LINDEX %s %d", TCHAR_TO_ANSI(*InKey), InIndex);
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type == REDIS_REPLY_STRING)
	{
		OutValue = RedisReplyPtr->str;
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::LInsertBefore(const FString& InKey, const FString& Pivot, const FString& InValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "LINSERT %s BEFORE %s %s", TCHAR_TO_ANSI(*InKey), TCHAR_TO_ANSI(*Pivot), TCHAR_TO_ANSI(*InValue));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::LInsertAfter(const FString& InKey, const FString& Pivot, const FString& InValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "LINSERT %s AFTER %s %s", TCHAR_TO_ANSI(*InKey), TCHAR_TO_ANSI(*Pivot), TCHAR_TO_ANSI(*InValue));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::LLen(const FString& InKey, int32& Len)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "LLEN %s", TCHAR_TO_ANSI(*InKey));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	switch (RedisReplyPtr->type)
	{
		case REDIS_REPLY_INTEGER:
			{
				Len = RedisReplyPtr->integer;
				bResult = true;
			}
			break;
		case REDIS_REPLY_STRING:
			{
				Len = atoi(RedisReplyPtr->str);
				bResult = true;
			}
			break;
		default:
			break;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::LPop(const FString& InKey, FString& OutValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "LPOP %s", TCHAR_TO_ANSI(*InKey));
	if (!RedisReplyPtr)
	{
		return bResult;
	}
	if (RedisReplyPtr->type == REDIS_REPLY_STRING)
	{
		OutValue = RedisReplyPtr->str;
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::LPush(const FString& InKey, const TArray<FString>& InFieldList)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	std::ostringstream redisCmd;
	redisCmd << "LPUSH " << TCHAR_TO_ANSI(*InKey);
	for (auto& it : InFieldList)
	{
		redisCmd << " " << TCHAR_TO_ANSI(*(it));
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, redisCmd.str().c_str());
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::LRange(const FString& InKey, int32 Start, int32 End, TArray<FString>& OutMemberList)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "LRANGE %s %d %d", TCHAR_TO_ANSI(*InKey), Start, End);
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type == REDIS_REPLY_ARRAY)
	{
		bResult = true;
		for (auto i = 0; i < RedisReplyPtr->elements; i++)
		{
			if (RedisReplyPtr->element[i]->str != NULL)
			{
				OutMemberList.Add(RedisReplyPtr->element[i]->str);
			}
		}
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::LRem(const FString& InKey, const FString& InValue, int32 Count /*= 0*/)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "LREM %s %d %s", TCHAR_TO_ANSI(*InKey), Count, TCHAR_TO_ANSI(*InValue));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::LSet(const FString& InKey, int32 InIndex, const FString& InValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "LSET %s %d %s", TCHAR_TO_ANSI(*InKey), InIndex, TCHAR_TO_ANSI(*InValue));
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::LTrim(const FString& InKey, int32 Start, int32 Stop)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "LTRIM %s %d %d", TCHAR_TO_ANSI(*InKey), Start, Stop);
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::RPop(const FString& InKey, FString& OutValue)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, "RPOP %s", TCHAR_TO_ANSI(*InKey));
	if (!RedisReplyPtr)
	{
		return bResult;
	}
	if (RedisReplyPtr->type == REDIS_REPLY_STRING)
	{
		OutValue = RedisReplyPtr->str;
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

bool URedisClient::RPush(const FString& InKey, const TArray<FString>& InFieldList)
{
	bool bResult = false;

	if (!RedisContextPtr)
	{
		return bResult;
	}

	std::ostringstream redisCmd;
	redisCmd << "RPUSH " << TCHAR_TO_ANSI(*InKey);
	for (auto& it : InFieldList)
	{
		redisCmd << " " << TCHAR_TO_ANSI(*(it));
	}

	RedisReplyPtr = (redisReply*)redisCommand(RedisContextPtr, redisCmd.str().c_str());
	if (!RedisReplyPtr)
	{
		return bResult;
	}

	if (RedisReplyPtr->type != REDIS_REPLY_ERROR)
	{
		bResult = true;
	}

	freeReplyObject(RedisReplyPtr);
	RedisReplyPtr = nullptr;

	return bResult;
}

