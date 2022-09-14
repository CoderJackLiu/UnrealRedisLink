// Copyright (C) 2019 GameSeed - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Core/Public/Containers/Queue.h"
#include "Runtime/Core/Public/Containers/Ticker.h"
#include "AsyncRedisDefines.h"
#include "RedisSubscribeObject.h"
#include "RedisObject.generated.h"


class URedisClient;
class URedisSubscribeObject;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSubscribeReply, FString, Channel, FString, Message);


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class REDISPLUGIN_API URedisObject : public UObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Redis|Connect", meta = (DisplayName = "InitRedisConnection"))
		virtual void Init(const FString& InHost, int32 InPort, const FString& InPassword);

	UFUNCTION(BlueprintCallable, Category = "Redis|Connect", meta = (DisplayName = "Reconnect"))
		virtual bool Reconnect();

	UFUNCTION(BlueprintCallable, Category = "Redis|Connect", meta = (DisplayName = "Quit"))
		virtual void Quit();

	UFUNCTION(BlueprintCallable, Category = "Redis|Connect", meta = (DisplayName = "SelectIndex"))
		virtual void SelectIndex(int32 InIndex);

	virtual bool ExecCommand(const FString& InCommand);

	UFUNCTION(BlueprintCallable, Category = "Redis|Key", meta = (DisplayName = "ExpireKey"))
		virtual bool ExpireKey(const FString& InKey, int32 InSec);

	UFUNCTION(BlueprintCallable, Category = "Redis|Key", meta = (DisplayName = "ExistsKey"))
		virtual bool ExistsKey(const FString& InKey);

	UFUNCTION(BlueprintCallable, Category = "Redis|Key", meta = (DisplayName = "PersistKey"))
		virtual bool PersistKey(const FString& InKey);

	UFUNCTION(BlueprintCallable, Category = "Redis|Key", meta = (DisplayName = "RenameKey"))
		virtual bool RenameKey(const FString& CurrentKey, const FString& NewKey);

	UFUNCTION(BlueprintCallable, Category = "Redis|Key", meta = (DisplayName = "DelKey"))
		virtual bool DelKey(const FString& InKey);

	UFUNCTION(BlueprintCallable, Category = "Redis|Key", meta = (DisplayName = "TypeKey"))
		virtual bool TypeKey(const FString& InKey, FString& OutType);

	UFUNCTION(BlueprintCallable, Category = "Redis|String", meta = (DisplayName = "MSet"))
		virtual bool MSet(TMap<FString, FString>& InMemberMap);

	UFUNCTION(BlueprintCallable, Category = "Redis|String", meta = (DisplayName = "MGet"))
		virtual bool MGet(const TArray<FString>& InKeyList, TArray<FString>& OutMemberList);

	UFUNCTION(BlueprintCallable, Category = "Redis|String", meta = (DisplayName = "SetInt"))
		virtual bool SetInt(const FString& InKey, int32 InValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|String", meta = (DisplayName = "GetInt"))
		virtual bool GetInt(const FString& InKey, int32& OutValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|String", meta = (DisplayName = "SetStr"))
		virtual bool SetStr(const FString& InKey, const FString& InValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|String", meta = (DisplayName = "GetStr"))
		virtual bool GetStr(const FString& InKey, FString& OutValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|String", meta = (DisplayName = "Append"))
		virtual bool Append(const FString& InKey, const FString& InValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|Set", meta = (DisplayName = "SAdd"))
		virtual bool SAdd(const FString& InKey, const TArray<FString>& InMemberList);

	UFUNCTION(BlueprintCallable, Category = "Redis|Set", meta = (DisplayName = "SCard"))
		virtual bool SCard(const FString& InKey, int32& OutValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|Set", meta = (DisplayName = "SRem"))
		virtual bool SRem(const FString& InKey, const TArray<FString>& InMemberList);

	UFUNCTION(BlueprintCallable, Category = "Redis|Set", meta = (DisplayName = "SMembers"))
		virtual bool SMembers(const FString& InKey, TArray<FString>& OutMemberList);

	UFUNCTION(BlueprintCallable, Category = "Redis|Hash", meta = (DisplayName = "HSet"))
		virtual bool HSet(const FString& InKey, const FString& InField, const FString& InValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|Hash", meta = (DisplayName = "HGet"))
		virtual bool HGet(const FString& InKey, const FString& InField, FString& OutValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|Hash", meta = (DisplayName = "HMSet"))
		virtual bool HMSet(const FString& InKey, const TMap<FString, FString>& InMemberMap);

	UFUNCTION(BlueprintCallable, Category = "Redis|Hash", meta = (DisplayName = "HDel"))
		virtual bool HDel(const FString& InKey, const TArray<FString>& InFieldList);

	UFUNCTION(BlueprintCallable, Category = "Redis|Hash", meta = (DisplayName = "HExists"))
		virtual bool HExists(const FString& InKey, const FString& Field);

	UFUNCTION(BlueprintCallable, Category = "Redis|Hash", meta = (DisplayName = "HMGet"))
		virtual bool HMGet(const FString& InKey, const TSet<FString>& InFieldList, TMap<FString, FString>& OutMemberMap);

	UFUNCTION(BlueprintCallable,  Category = "Redis|Hash", meta = (DisplayName = "HGetAll"))
		virtual bool HGetAll(const FString& InKey, TMap<FString, FString>& OutMemberMap);

	UFUNCTION(BlueprintCallable, Category = "Redis|Hash", meta = (DisplayName = "HIncrby"))
		virtual bool HIncrby(const FString& Key, const FString& Field, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Redis|List", meta = (DisplayName = "LIndex"))
		virtual bool LIndex(const FString& InKey, int32 InIndex, FString& OutValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|List", meta = (DisplayName = "LInsertBefore"))
		virtual bool LInsertBefore(const FString& InKey, const FString& Pivot, const FString& InValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|List", meta = (DisplayName = "LInsertAfter"))
		virtual bool LInsertAfter(const FString& InKey, const FString& Pivot, const FString& InValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|List", meta = (DisplayName = "LLen"))
		virtual bool LLen(const FString& InKey, int32& Len);

	UFUNCTION(BlueprintCallable, Category = "Redis|List", meta = (DisplayName = "LPop"))
		virtual bool LPop(const FString& InKey, FString& OutValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|List", meta = (DisplayName = "LPush"))
		virtual bool LPush(const FString& InKey, const TArray<FString>& InFieldList);

	UFUNCTION(BlueprintCallable, Category = "Redis|List", meta = (DisplayName = "LRange"))
		virtual bool LRange(const FString& InKey, int32 Start, int32 End, TArray<FString>& OutMemberList);

	UFUNCTION(BlueprintCallable, Category = "Redis|List", meta = (DisplayName = "LRem"))
		virtual bool LRem(const FString& InKey, const FString& InValue, int32 Count = 0);

	UFUNCTION(BlueprintCallable, Category = "Redis|List", meta = (DisplayName = "LSet"))
		virtual bool LSet(const FString& InKey, int32 InIndex, const FString& InValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|List", meta = (DisplayName = "LTrim"))
		virtual bool LTrim(const FString& InKey, int32 Start, int32 Stop);

	UFUNCTION(BlueprintCallable, Category = "Redis|List", meta = (DisplayName = "RPop"))
		virtual bool RPop(const FString& InKey, FString& OutValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|List", meta = (DisplayName = "RPush"))
		virtual bool RPush(const FString& InKey, const TArray<FString>& InFieldList);

	/* Async redis operations. (Non-blocking call to the Redis command) */

	UFUNCTION(BlueprintCallable, Category = "Redis|Key", meta = (DisplayName = "ExistsKey-Async"))
		virtual void AsyncExistsKey(const FString& InKey, FExistsKeyFinished OnFinished);

//	UFUNCTION(BlueprintCallable, Category = "Redis|Key", meta = (DisplayName = "ExpireKey-Async"))
		virtual void AsyncExpireKey(const FString& InKey, int32 InSec);

//	UFUNCTION(BlueprintCallable, Category = "Redis|Key", meta = (DisplayName = "DelKey-Async"))
		virtual void AsyncDelKey(const FString& InKey);

	UFUNCTION(BlueprintCallable, Category = "Redis|String", meta = (DisplayName = "MGet-Async"))
		virtual void AsyncMGet(const TArray<FString>& InKeyList, FMGetFinished OnFinished);

//	UFUNCTION(BlueprintCallable, Category = "Redis|String", meta = (DisplayName = "SetInt-Async"))
		virtual void AsyncSetInt(const FString& InKey, int32 InValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|String", meta = (DisplayName = "GetInt-Async"))
		virtual void AsyncGetInt(const FString& InKey, FGetIntFinished OnFinished);

//	UFUNCTION(BlueprintCallable, Category = "Redis|String", meta = (DisplayName = "SetStr-Async"))
		virtual void AsyncSetStr(const FString& InKey, const FString& InValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|String", meta = (DisplayName = "GetStr-Async"))
		virtual void AsyncGetStr(const FString& InKey, FGetStrFinished OnFinished);

//	UFUNCTION(BlueprintCallable, Category = "Redis|Set", meta = (DisplayName = "SAdd-Async"))
		virtual void AsyncSAdd(const FString& InKey, const TArray<FString>& InMemberList);

//	UFUNCTION(BlueprintCallable, Category = "Redis|Set", meta = (DisplayName = "SRem-Async"))
		virtual void AsyncSRem(const FString& InKey, const TArray<FString>& InMemberList);

	UFUNCTION(BlueprintCallable, Category = "Redis|Set", meta = (DisplayName = "SMembers-Async"))
		virtual void AsyncSMembers(const FString& InKey, FSMembersFinished OnFinished);

//	UFUNCTION(BlueprintCallable, Category = "Redis|Hash", meta = (DisplayName = "HSet-Async"))
		virtual void AsyncHSet(const FString& InKey, const FString& InField, const FString& InValue);

	UFUNCTION(BlueprintCallable, Category = "Redis|Hash", meta = (DisplayName = "HGet-Async"))
		virtual void AsyncHGet(const FString& InKey, const FString& InField, FHGetFinished OnFinished);

//	UFUNCTION(BlueprintCallable, Category = "Redis|Hash", meta = (DisplayName = "HMSet-Async"))
		virtual void AsyncHMSet(const FString& InKey, const TMap<FString, FString>& InMemberMap);

//	UFUNCTION(BlueprintCallable, Category = "Redis|Hash", meta = (DisplayName = "HDel-Async"))
		virtual void AsyncHDel(const FString& InKey, const TArray<FString>& InFieldList);

	UFUNCTION(BlueprintCallable, Category = "Redis|Hash", meta = (DisplayName = "HMGet-Async"))
		virtual void AsyncHMGet(const FString& InKey, const TSet<FString>& InFieldList, FHMGetFinished OnFinished);

	UFUNCTION(BlueprintCallable, Category = "Redis|Hash", meta = (DisplayName = "HGetAll-Async"))
		virtual void AsyncHGetAll(const FString& InKey, FHGetAllFinished OnFinished);

	UFUNCTION(BlueprintCallable, Category = "Redis|Pub/Sub")
		virtual bool Publish(const FString& Channel, const FString& Message);
	
	UFUNCTION(BlueprintCallable, Category = "Redis|Pub/Sub")
		virtual void Subscribe(const FString& Channel);

	UFUNCTION(BlueprintCallable, Category = "Redis|Pub/Sub")
		virtual void Unsubscribe(const FString& Channel);

	bool Tick(float DeltaTime);

// 	UFUNCTION(BlueprintCallable, Category = "Redis", meta = (DisplayName = "OnTestRedis"))
// 		virtual bool OnTest(const FString& InKey);


	UPROPERTY(BlueprintAssignable)
	FSubscribeReply SubscribeReply;

private:

	TSharedPtr<URedisClient> FindOrNewRedisClient();

	void RecycleRedisClient(TSharedPtr<URedisClient> InRedisClient);

	void SubscribeCallback(FString Channel, FString Message);

private:
	UPROPERTY()
	FString		Host;
	UPROPERTY()
	uint16		Port;
	UPROPERTY()
	FString		Password;
	UPROPERTY()
	int32		ResultsPoolSize;
private:
	bool bInitFinished;

	TSharedPtr<URedisClient> SyncRedisClient;

	TSharedPtr<URedisClient> SubscribeRedisClient;

	TArray<TSharedPtr<URedisClient>> IdleRedisClients;

	UPROPERTY()
	URedisSubscribeObject* SubscribeObject;
	UPROPERTY()
		TMap<FString, URedisSubscribeObject*> SubscribeMap;

	bool bSubscribed;

	FDelegateHandle TickerHandle;
	FTickerDelegate OnTickerDelegate;

	DECLARE_ASYNC_RESULTS(FAsyncResultNoReturn, NoReturn);
	DECLARE_ASYNC_RESULTS(FAsyncResultExistsKey, ExistsKey);
	DECLARE_ASYNC_RESULTS(FAsyncResultMGet, MGet);
	DECLARE_ASYNC_RESULTS(FAsyncResultGetInt, GetInt);
	DECLARE_ASYNC_RESULTS(FAsyncResultGetStr, GetStr);
	DECLARE_ASYNC_RESULTS(FAsyncResultHGet, HGet);
	DECLARE_ASYNC_RESULTS(FAsyncResultHMGet, HMGet);
	DECLARE_ASYNC_RESULTS(FAsyncResultHGetAll, HGetAll);
	DECLARE_ASYNC_RESULTS(FAsyncResultSMembers, SMembers);

};
