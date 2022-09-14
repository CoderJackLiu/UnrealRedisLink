// Copyright (C) 2019 GameSeed - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"
#include "AsyncRedisDefines.h"

DECLARE_DELEGATE_OneParam(FNotifyRedisResultNoReturn, FAsyncResultNoReturn*);
DECLARE_DELEGATE_OneParam(FNotifyRedisResultExistsKey, FAsyncResultExistsKey*);
DECLARE_DELEGATE_OneParam(FNotifyRedisResultMGet, FAsyncResultMGet*);
DECLARE_DELEGATE_OneParam(FNotifyRedisResultGetInt, FAsyncResultGetInt*);
DECLARE_DELEGATE_OneParam(FNotifyRedisResultGetStr, FAsyncResultGetStr*);
DECLARE_DELEGATE_OneParam(FNotifyRedisResultHGet, FAsyncResultHGet*);
DECLARE_DELEGATE_OneParam(FNotifyRedisResultHMGet, FAsyncResultHMGet*);
DECLARE_DELEGATE_OneParam(FNotifyRedisResultHGetAll, FAsyncResultHGetAll*);
DECLARE_DELEGATE_OneParam(FNotifyRedisResultSMembers, FAsyncResultSMembers*);

class AsyncExistsKeyTask : public FNonAbandonableTask
{
	FString	RedisKey;
	FNotifyRedisResultExistsKey NotifyRedisResult;
	FAsyncResultExistsKey* CallbackHandler;
public:

	friend class FAsyncTask<AsyncExistsKeyTask>;

	AsyncExistsKeyTask(FString InKey, FAsyncResultExistsKey* InCallbackHandler) :
		RedisKey(InKey), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncExistsKeyTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultExistsKey& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->ExistsKey(RedisKey);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncExistsKeyTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncExpireKeyTask : public FNonAbandonableTask
{
	FString	RedisKey;
	int32 Sec;
	FNotifyRedisResultNoReturn NotifyRedisResult;
	FAsyncResultNoReturn* CallbackHandler;
public:

	friend class FAsyncTask<AsyncExpireKeyTask>;

	AsyncExpireKeyTask(FString InKey, int32 InSec, FAsyncResultNoReturn* InCallbackHandler) :
		RedisKey(InKey), Sec(InSec), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncExpireKeyTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultNoReturn& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->ExpireKey(RedisKey, Sec);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncExpireKeyTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncDelKeyTask : public FNonAbandonableTask
{
	FString	RedisKey;
	FNotifyRedisResultNoReturn NotifyRedisResult;
	FAsyncResultNoReturn* CallbackHandler;
public:

	friend class FAsyncTask<AsyncDelKeyTask>;

	AsyncDelKeyTask(FString InKey, FAsyncResultNoReturn* InCallbackHandler) :
		RedisKey(InKey), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncDelKeyTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultNoReturn& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->DelKey(RedisKey);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncDelKeyTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncMGetTask : public FNonAbandonableTask
{
	FString	RedisKey;
	TArray<FString> KeyList;
	FNotifyRedisResultMGet NotifyRedisResult;
	FAsyncResultMGet* CallbackHandler;
public:

	friend class FAsyncTask<AsyncMGetTask>;

	AsyncMGetTask(const TArray<FString>& InKeyList,  FAsyncResultMGet* InCallbackHandler) :
		KeyList(InKeyList), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncMGetTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultMGet& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->MGet(KeyList, CallbackHandler->ResultMemberList);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncMGetTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncSetIntTask : public FNonAbandonableTask
{
	FString	RedisKey;
	int32	RedisValue;
	FNotifyRedisResultNoReturn NotifyRedisResult;
	FAsyncResultNoReturn* CallbackHandler;
public:

	friend class FAsyncTask<AsyncSetIntTask>;

	AsyncSetIntTask(FString InKey, int32 InValue, FAsyncResultNoReturn* InCallbackHandler) :
		RedisKey(InKey), RedisValue(InValue), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncSetIntTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultNoReturn& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->SetInt(RedisKey, RedisValue);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSetIntTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncSetStrTask : public FNonAbandonableTask
{
	FString	RedisKey;
	FString	RedisValue;
	FNotifyRedisResultNoReturn NotifyRedisResult;
	FAsyncResultNoReturn* CallbackHandler;
public:

	friend class FAsyncTask<AsyncSetStrTask>;

	AsyncSetStrTask(FString InKey, FString InValue, FAsyncResultNoReturn* InCallbackHandler) :
		RedisKey(InKey), RedisValue(InValue), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncSetStrTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultNoReturn& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->SetStr(RedisKey, RedisValue);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSetStrTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncSAddTask : public FNonAbandonableTask
{
	FString	RedisKey;
	TArray<FString> MemberList;
	FNotifyRedisResultNoReturn NotifyRedisResult;
	FAsyncResultNoReturn* CallbackHandler;
public:

	friend class FAsyncTask<AsyncSAddTask>;

	AsyncSAddTask(FString InKey, TArray<FString> InMemberList, FAsyncResultNoReturn* InCallbackHandler) :
		RedisKey(InKey), MemberList(InMemberList), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncSAddTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultNoReturn& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->SAdd(RedisKey, MemberList);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSAddTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncSRemTask : public FNonAbandonableTask
{
	FString	RedisKey;
	TArray<FString> MemberList;
	FNotifyRedisResultNoReturn NotifyRedisResult;
	FAsyncResultNoReturn* CallbackHandler;
public:

	friend class FAsyncTask<AsyncSRemTask>;

	AsyncSRemTask(FString InKey, TArray<FString> InMemberList, FAsyncResultNoReturn* InCallbackHandler) :
		RedisKey(InKey), MemberList(InMemberList), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncSRemTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultNoReturn& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->SRem(RedisKey, MemberList);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSRemTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncHSetTask : public FNonAbandonableTask
{
	FString	RedisKey;
	FString RedisField;
	FString RedisValue;
	FNotifyRedisResultNoReturn NotifyRedisResult;
	FAsyncResultNoReturn* CallbackHandler;
public:

	friend class FAsyncTask<AsyncHSetTask>;

	AsyncHSetTask(FString InKey, FString InField, FString InValue, FAsyncResultNoReturn* InCallbackHandler) :
		RedisKey(InKey), RedisField(InField), RedisValue(InValue), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncHSetTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultNoReturn& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->HSet(RedisKey, RedisField, RedisValue);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncHSetTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncHMSetTask : public FNonAbandonableTask
{
	FString	RedisKey;
	TMap<FString, FString> FieldValueMap;
	FNotifyRedisResultNoReturn NotifyRedisResult;
	FAsyncResultNoReturn* CallbackHandler;
public:

	friend class FAsyncTask<AsyncHMSetTask>;

	AsyncHMSetTask(FString InKey, TMap<FString, FString> InFieldValueMap, FAsyncResultNoReturn* InCallbackHandler) :
		RedisKey(InKey), FieldValueMap(InFieldValueMap), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncHMSetTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultNoReturn& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->HMSet(RedisKey, FieldValueMap);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncHMSetTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncHDelTask : public FNonAbandonableTask
{
	FString	RedisKey;
	TArray<FString> FieldList;
	FNotifyRedisResultNoReturn NotifyRedisResult;
	FAsyncResultNoReturn* CallbackHandler;
public:

	friend class FAsyncTask<AsyncHDelTask>;

	AsyncHDelTask(FString InKey, TArray<FString> InFieldList, FAsyncResultNoReturn* InCallbackHandler) :
		RedisKey(InKey), FieldList(InFieldList), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncHDelTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultNoReturn& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->HDel(RedisKey, FieldList);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncHDelTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncGetIntTask : public FNonAbandonableTask
{
	FString	RedisKey;
	FNotifyRedisResultGetInt NotifyRedisResult;
	FAsyncResultGetInt* CallbackHandler;
public:

	friend class FAsyncTask<AsyncGetIntTask>;

	AsyncGetIntTask(FString InKey, FAsyncResultGetInt* InCallbackHandler) :
		RedisKey(InKey), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncGetIntTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultGetInt& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->GetInt(RedisKey, CallbackHandler->ResultValue);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncGetIntTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncGetStrTask : public FNonAbandonableTask
{
	FString	RedisKey;
	FNotifyRedisResultGetStr NotifyRedisResult;
	FAsyncResultGetStr* CallbackHandler;
public:

	friend class FAsyncTask<AsyncGetStrTask>;

	AsyncGetStrTask(FString InKey, FAsyncResultGetStr* InCallbackHandler) :
		RedisKey(InKey), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncGetStrTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultGetStr& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->GetStr(RedisKey, CallbackHandler->ResultValue);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncGetStrTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncHGetTask : public FNonAbandonableTask
{
	FString	RedisKey;
	FString RedisField;
	FNotifyRedisResultHGet NotifyRedisResult;
	FAsyncResultHGet* CallbackHandler;
public:

	friend class FAsyncTask<AsyncHGetTask>;

	AsyncHGetTask(FString InKey, FString InField, FAsyncResultHGet* InCallbackHandler) :
		RedisKey(InKey), RedisField(InField), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncHGetTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultHGet& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->HGet(RedisKey, RedisField, CallbackHandler->ResultValue);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncHGetTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncHMGetTask : public FNonAbandonableTask
{
	FString	RedisKey;
	TSet<FString> FieldList;
	FNotifyRedisResultHMGet NotifyRedisResult;
	FAsyncResultHMGet* CallbackHandler;
public:

	friend class FAsyncTask<AsyncHMGetTask>;

	AsyncHMGetTask(FString InKey, TSet<FString> InFieldList, FAsyncResultHMGet* InCallbackHandler) :
		RedisKey(InKey), FieldList(InFieldList), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncHMGetTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultHMGet& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->HMGet(RedisKey, FieldList, CallbackHandler->ResultFieldValueMap);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncHMGetTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncHGetAllTask : public FNonAbandonableTask
{
	FString	RedisKey;
	FNotifyRedisResultHGetAll NotifyRedisResult;
	FAsyncResultHGetAll* CallbackHandler;
public:

	friend class FAsyncTask<AsyncHGetAllTask>;

	AsyncHGetAllTask(FString InKey, FAsyncResultHGetAll* InCallbackHandler) :
		RedisKey(InKey), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncHGetAllTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultHGetAll& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->HGetAll(RedisKey, CallbackHandler->ResultFieldValueMap);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncHGetAllTask, STATGROUP_ThreadPoolAsyncTasks); }
};

class AsyncSMembersTask : public FNonAbandonableTask
{
	FString	RedisKey;
	FNotifyRedisResultSMembers NotifyRedisResult;
	FAsyncResultSMembers* CallbackHandler;
public:

	friend class FAsyncTask<AsyncSMembersTask>;

	AsyncSMembersTask(FString InKey, FAsyncResultSMembers* InCallbackHandler) :
		RedisKey(InKey), CallbackHandler(InCallbackHandler)
	{	}

	~AsyncSMembersTask()
	{
		NotifyRedisResult.Unbind();
	}

	FNotifyRedisResultSMembers& GetDelegate()
	{
		return NotifyRedisResult;
	}

	void DoWork()
	{
		CallbackHandler->bResult = CallbackHandler->AsyncRedisClient->SMembers(RedisKey, CallbackHandler->ResultMemberList);
		NotifyRedisResult.ExecuteIfBound(CallbackHandler);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSMembersTask, STATGROUP_ThreadPoolAsyncTasks); }
};
