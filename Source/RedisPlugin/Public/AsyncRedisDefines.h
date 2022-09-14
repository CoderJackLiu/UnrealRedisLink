// Copyright (C) 2019 GameSeed - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AsyncRedisDefines.generated.h"

class URedisClient;

USTRUCT(BlueprintType)
struct FWrapMap
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Redis")
	TMap<FString, FString> RealMap;
};

USTRUCT(BlueprintType)
struct FWrapArray
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Redis")
	TArray<FString> RealArray;
};

/*  */
DECLARE_DYNAMIC_DELEGATE_OneParam(FExistsKeyFinished, bool, bResult);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FMGetFinished, bool, bResult, FWrapArray, OutMemberList);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FGetIntFinished, bool, bResult, int32, OutValue);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FGetStrFinished, bool, bResult, FString, OutValue);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FHGetFinished, bool, bResult, FString, OutValue);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FHMGetFinished, bool, bResult, FWrapMap, OutMemberMap);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FHGetAllFinished, bool, bResult, FWrapMap, OutMemberMap);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FSMembersFinished, bool, bResult, FWrapArray, OutMemberList);

DECLARE_MULTICAST_DELEGATE_OneParam(FRedisNoReturnFinished, bool);

USTRUCT()
struct FAsyncResultNoReturn
{
	GENERATED_BODY()

	FAsyncResultNoReturn() :
		bResult(false), AsyncRedisClient(nullptr)
	{	}

	void Reset()
	{
		bResult = false;
		NoReturnCallback.Clear();
		AsyncRedisClient = nullptr;
	}

	bool bResult;
	FRedisNoReturnFinished NoReturnCallback;
	TSharedPtr<URedisClient> AsyncRedisClient;
};

USTRUCT()
struct FAsyncResultExistsKey
{
	GENERATED_BODY()

		FAsyncResultExistsKey() :
		bResult(false), AsyncRedisClient(nullptr)
	{	}

	void Reset()
	{
		bResult = false;
		ExistsKeyCallback.Clear();
		AsyncRedisClient = nullptr;
	}

	bool bResult;
	FExistsKeyFinished ExistsKeyCallback;
	TSharedPtr<URedisClient> AsyncRedisClient;
};

USTRUCT()
struct FAsyncResultMGet
{
	GENERATED_BODY()

		FAsyncResultMGet() :
		bResult(false), AsyncRedisClient(nullptr)
	{	}

	void Reset()
	{
		bResult = false;
		ResultMemberList.Reset();
		MGetCallback.Clear();
		AsyncRedisClient = nullptr;
	}

	bool bResult;
	TArray<FString> ResultMemberList;
	FMGetFinished MGetCallback;
	TSharedPtr<URedisClient> AsyncRedisClient;
};

USTRUCT()
struct FAsyncResultGetInt
{
	GENERATED_BODY()

	FAsyncResultGetInt() :
		bResult(false), ResultValue(0), AsyncRedisClient(nullptr)
	{	}

	void Reset()
	{
		bResult = false;
		ResultValue = 0;
		GetIntCallback.Clear();
		AsyncRedisClient = nullptr;
	}

	bool bResult;
	int32 ResultValue;
	FGetIntFinished GetIntCallback;
	TSharedPtr<URedisClient> AsyncRedisClient;
};

USTRUCT()
struct FAsyncResultGetStr
{
	GENERATED_BODY()

	FAsyncResultGetStr() :
		bResult(false), ResultValue(TEXT("")), AsyncRedisClient(nullptr)
	{	}

	void Reset()
	{
		bResult = false;
		ResultValue = TEXT("");
		GetStrCallback.Clear();
		AsyncRedisClient = nullptr;
	}

	bool bResult;
	FString ResultValue;
	FGetStrFinished GetStrCallback;
	TSharedPtr<URedisClient> AsyncRedisClient;
};

USTRUCT()
struct FAsyncResultHGet
{
	GENERATED_BODY()

	FAsyncResultHGet() :
		bResult(false), ResultValue(TEXT("")), AsyncRedisClient(nullptr)
	{	}

	void Reset()
	{
		bResult = false;
		ResultValue = TEXT("");
		HGetCallback.Clear();
		AsyncRedisClient = nullptr;
	}

	bool bResult;
	FString ResultValue;
	FHGetFinished HGetCallback;
	TSharedPtr<URedisClient> AsyncRedisClient;
};

USTRUCT()
struct FAsyncResultHMGet
{
	GENERATED_BODY()

	FAsyncResultHMGet() :
		bResult(false), AsyncRedisClient(nullptr)
	{	}

	void Reset()
	{
		bResult = false;
		ResultFieldValueMap.Reset();
		HMGetCallback.Clear();
		AsyncRedisClient = nullptr;
	}

	bool bResult;
	TMap<FString, FString> ResultFieldValueMap;
	FHMGetFinished HMGetCallback;
	TSharedPtr<URedisClient> AsyncRedisClient;
};

USTRUCT()
struct FAsyncResultHGetAll
{
	GENERATED_BODY()

	FAsyncResultHGetAll() :
		bResult(false), AsyncRedisClient(nullptr)
	{	}

	void Reset()
	{
		bResult = false;
		ResultFieldValueMap.Reset();
		HGetAllCallback.Clear();
		AsyncRedisClient = nullptr;
	}

	bool bResult;
	TMap<FString, FString> ResultFieldValueMap;
	FHGetAllFinished HGetAllCallback;
	TSharedPtr<URedisClient> AsyncRedisClient;
};

USTRUCT()
struct FAsyncResultSMembers
{
	GENERATED_BODY()

	FAsyncResultSMembers() :
		bResult(false), AsyncRedisClient(nullptr)
	{	}

	void Reset()
	{
		bResult = false;
		ResultMemberList.Reset();
		SMembersCallback.Clear();
		AsyncRedisClient = nullptr;
	}

	bool bResult;
	TArray<FString> ResultMemberList;
	FSMembersFinished SMembersCallback;
	TSharedPtr<URedisClient> AsyncRedisClient;
};

/** In URedisObject */
#define DECLARE_ASYNC_RESULTS(Type, Name)								\
private:																\
	TArray<Type*>						Name##Free##Results; 			\
	TQueue<Type*, EQueueMode::Mpsc>		Name##FinishedResults;			\
																		\
	void Create##Name##Results(int32 Count = 100);						\
																		\
	Type* FindOrAdd##Name##Result();									\
																		\
	void Recycle##Name##Result(Type* InResult);							\
																		\
	void OnNotify##Name##Result(Type* InResult);						\
																		\

/** In RedisObject.cpp */
#define IMPLEMENT_ASYNC_RESULTS(Type, Name)								\
	void URedisObject::Create##Name##Results(int32 Count)				\
	{																	\
		for (int i = 0; i < Count; ++i)									\
		{																\
			auto ResultPtr = new Type();								\
			Name##Free##Results.Push(ResultPtr);						\
		}																\
	}																	\
																		\
	Type* URedisObject::FindOrAdd##Name##Result()						\
	{																	\
		if (Name##Free##Results.Num())									\
		{																\
			return Name##Free##Results.Pop();							\
		}																\
																		\
		Create##Name##Results();										\
																		\
		return Name##Free##Results.Pop();								\
	}																	\
																		\
	void URedisObject::Recycle##Name##Result(Type* InResult)			\
	{																	\
		if (InResult != nullptr)										\
		{																\
			InResult->Reset();											\
			Name##Free##Results.Push(InResult);							\
		}																\
	}																	\
																		\
	void URedisObject::OnNotify##Name##Result(Type* InResult)			\
	{																	\
		Name##FinishedResults.Enqueue(InResult);						\
	}																	\


#define POP_ASYNC_RESULT(Name, Pointer)									\
	auto Pointer = FindOrAdd##Name##Result();							\
	Pointer->AsyncRedisClient = FindOrNewRedisClient();					\
	if (Pointer->AsyncRedisClient.Get() == nullptr)						\
	{																	\
		Recycle##Name##Result(Pointer);									\
		return;															\
	}																	\

#define PUSH_ASYNC_RESULT(Name, Pointer)								\
	RecycleRedisClient(Pointer->AsyncRedisClient);						\
	Recycle##Name##Result(Pointer);										

