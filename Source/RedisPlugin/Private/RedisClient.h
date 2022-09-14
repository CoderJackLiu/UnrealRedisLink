// Copyright (C) 2019 GameSeed - All Rights Reserved

#pragma once

#include "CoreMinimal.h"

struct redisContext;
struct redisReply;

/**
 * 
 */
class REDISPLUGIN_API URedisClient
{
	
public:

	URedisClient();
	~URedisClient();

	/* Connect */
	bool ConnectToRedis(const FString& InHost, int32 InPort, const FString& InPassword);

	void DisconnectRedis();

	void Quit();

	bool SelectIndex(int32 InIndex);

	bool ExecCommand(const FString& InCommand);

	/* Pub/Sub */
	bool Subscribe(const FString& InChannel);

	void Unsubscribe();

	bool Subscribed();

	/* Block! */
	bool SubscribeReply(FString& Channel, FString& Message);

	bool UnsubscribeChannel(const FString& InChannel);
	
	bool Publish(const FString& InChannel, const FString& InMessage);

	/* Key */
	bool ExistsKey(const FString& InKey);

	bool ExpireKey(const FString& InKey, int32 Sec);

	bool PersistKey(const FString& InKey);

	bool RenameKey(const FString& CurrentKey, const FString& NewKey);

	bool DelKey(const FString& InKey);

	bool TypeKey(const FString& InKey, FString& OutType);

	/* Str */
	bool MSet(TMap<FString, FString>& InMemberMap);

	bool MGet(const TArray<FString>& InKeyList, TArray<FString>& OutMemberList);

	bool SetInt(const FString& InKey, int32 InValue);

	bool GetInt(const FString& InKey, int32& OutValue);

	bool SetStr(const FString& InKey, const FString& InValue);

	bool GetStr(const FString& InKey, FString& OutValue);

	bool Append(const FString& InKey, const FString& InValue);

	/* Sorted Set */ // todo

	/* Set */
	bool SAdd(const FString& InKey, const TArray<FString>& InMemberList);

	bool SCard(const FString& InKey, int32& OutValue);

	bool SRem(const FString& InKey, const TArray<FString>& InMemberList);

	bool SMembers(const FString& InKey, TArray<FString>& OutMemberList);

	/* Hash */
	bool HSet(const FString& InKey, const FString& Field, const FString& InValue);

	bool HGet(const FString& InKey, const FString& Field, FString& OutValue);

	bool HIncrby(const FString& InKey, const FString& Field, int32 Incre);

	bool HMSet(const FString& InKey, const TMap<FString, FString>& InMemberMap);

	bool HDel(const FString& InKey, const TArray<FString>& InFieldList);

	bool HExists(const FString& InKey, const FString& Field);

	bool HMGet(const FString& InKey, const TSet<FString>& InFieldList, TMap<FString, FString>& OutMemberMap);

	bool HGetAll(const FString& InKey, TMap<FString, FString>& OutMemberMap);
	
	/* List */
	bool LIndex(const FString& InKey, int32 InIndex, FString& OutValue);

	bool LInsertBefore(const FString& InKey, const FString& Pivot, const FString& InValue);

	bool LInsertAfter(const FString& InKey, const FString& Pivot, const FString& InValue);

	bool LLen(const FString& InKey, int32& Len);

	bool LPop(const FString& InKey, FString& OutValue);

	bool LPush(const FString& InKey, const TArray<FString>& InFieldList);

	bool LRange(const FString& InKey, int32 Start, int32 End, TArray<FString>& OutMemberList);

	bool LRem(const FString& InKey, const FString& InValue, int32 Count = 0);

	bool LSet(const FString& InKey, int32 InIndex, const FString& InValue);

	bool LTrim(const FString& InKey, int32 Start, int32 Stop);

	bool RPop(const FString& InKey, FString& OutValue);

	bool RPush(const FString& InKey, const TArray<FString>& InFieldList);

private:
	redisContext*	RedisContextPtr;
	redisReply*		RedisReplyPtr;
	FString			Password;
	FString			Host;
	uint16			Port;
	bool			bSubscribed;
};
