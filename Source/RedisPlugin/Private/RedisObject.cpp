// Copyright (C) 2019 GameSeed - All Rights Reserved

#include "RedisObject.h"
#include "RedisClient.h"
#include "AsyncRedisTask.h"
#include "LatentActions.h"
#include "RedisSubscribeObject.h"

IMPLEMENT_ASYNC_RESULTS(FAsyncResultNoReturn, NoReturn);
IMPLEMENT_ASYNC_RESULTS(FAsyncResultExistsKey, ExistsKey);
IMPLEMENT_ASYNC_RESULTS(FAsyncResultMGet, MGet);
IMPLEMENT_ASYNC_RESULTS(FAsyncResultGetInt, GetInt);
IMPLEMENT_ASYNC_RESULTS(FAsyncResultGetStr, GetStr);
IMPLEMENT_ASYNC_RESULTS(FAsyncResultHGet, HGet);
IMPLEMENT_ASYNC_RESULTS(FAsyncResultHMGet, HMGet);
IMPLEMENT_ASYNC_RESULTS(FAsyncResultHGetAll, HGetAll);
IMPLEMENT_ASYNC_RESULTS(FAsyncResultSMembers, SMembers);

void URedisObject::Init(const FString& InHost, int32 InPort, const FString& InPassword)
{
	if (bInitFinished)
	{
		return;
	}
	Host = InHost;
	Port = InPort;
	Password = InPassword;
	SyncRedisClient = FindOrNewRedisClient();
	if (SyncRedisClient)
	{
		if (SyncRedisClient->ConnectToRedis(Host, Port, Password))
		{
			bInitFinished = true;
		}
	}

	int32 FreeClientNum = 0;

// 	for (int i = 0; i < FreeClientNum; ++i)
// 	{
// 		TSharedPtr<URedisClient> NewRedisClient(new URedisClient());
// 		NewRedisClient->ConnectToRedis(Host, Port, Password);
// 		IdleRedisClients.Push(NewRedisClient);
// 	}

	ResultsPoolSize = 100;

	CreateNoReturnResults(ResultsPoolSize);
	CreateGetIntResults(ResultsPoolSize);
	CreateGetStrResults(ResultsPoolSize);
	CreateHGetResults(ResultsPoolSize);
	CreateHMGetResults(ResultsPoolSize);
	CreateHGetAllResults(ResultsPoolSize);
	CreateSMembersResults(ResultsPoolSize);

	// Start tick
	OnTickerDelegate = FTickerDelegate::CreateUObject(this, &URedisObject::Tick);
	TickerHandle = FTicker::GetCoreTicker().AddTicker(OnTickerDelegate);

}


bool URedisObject::Reconnect()
{
	if (!bInitFinished)
	{
		if (SyncRedisClient->ConnectToRedis(Host, Port, Password))
		{
			bInitFinished = true;
		}
	}
	return bInitFinished;
}

void URedisObject::Quit()
{
	if (SyncRedisClient.Get())
	{
		 SyncRedisClient->Quit();
		 bInitFinished = false;
	}
}


void URedisObject::SelectIndex(int32 InIndex)
{
	if (SyncRedisClient.Get())
	{
		SyncRedisClient->SelectIndex(InIndex);
	}
}

/*
bool URedisObject::OnTest(const FString& InKey)
{
	if (SyncRedisClient.Get())
	{
		FString OutString;
		SyncRedisClient->PersistKey(InKey);
		SyncRedisClient->TypeKey(InKey, OutString);
		
		TMap<FString, FString> InMap;
		InMap.Add(TEXT("M1"), TEXT("V1"));
		InMap.Add(TEXT("M2"), TEXT("V2"));
		InMap.Add(TEXT("M3"), TEXT("V3"));
		SyncRedisClient->MSet(InMap);
		TArray<FString> InKeys;
		TArray<FString> OutKeys;
		InKeys.Add(TEXT("M3"));
		InKeys.Add(TEXT("M1"));
		SyncRedisClient->MGet(InKeys, OutKeys);
		SyncRedisClient->Append(InKey, TEXT("s2s2"));
		SyncRedisClient->RenameKey(InKey, TEXT("NewKey1"));
		SyncRedisClient->SelectIndex(1);
		SyncRedisClient->Quit();
	}
	return true;
}
*/

TSharedPtr<URedisClient> URedisObject::FindOrNewRedisClient()
{
	if (IdleRedisClients.Num())
	{
		return IdleRedisClients.Pop();
	}

	TSharedPtr<URedisClient> NewRedisClient(new URedisClient());
	NewRedisClient->ConnectToRedis(Host, Port, Password);

	return NewRedisClient;
}

void URedisObject::RecycleRedisClient(TSharedPtr<URedisClient> InRedisClient)
{
	if (InRedisClient.Get())
	{
		IdleRedisClients.Push(InRedisClient);
	}
}

bool URedisObject::Tick(float DeltaTime)
{
	int32 LimitCount = 0;

	FAsyncResultNoReturn* CurrentNoReturnResult = nullptr;
	while (NoReturnFinishedResults.Dequeue(CurrentNoReturnResult))
	{
		PUSH_ASYNC_RESULT(NoReturn, CurrentNoReturnResult);
	}
	
	FAsyncResultExistsKey* CurrentExistsKeyResult = nullptr;
	while (ExistsKeyFinishedResults.Dequeue(CurrentExistsKeyResult))
	{
		CurrentExistsKeyResult->ExistsKeyCallback.ExecuteIfBound(CurrentExistsKeyResult->bResult);
		PUSH_ASYNC_RESULT(ExistsKey, CurrentExistsKeyResult);
	}

	FAsyncResultMGet* CurrentMGetResult = nullptr;
	while (MGetFinishedResults.Dequeue(CurrentMGetResult))
	{
		FWrapArray TmpWrapArray;
		TmpWrapArray.RealArray = CurrentMGetResult->ResultMemberList;
		CurrentMGetResult->MGetCallback.ExecuteIfBound(CurrentMGetResult->bResult, TmpWrapArray);
		PUSH_ASYNC_RESULT(MGet, CurrentMGetResult);
	}

	FAsyncResultGetInt* CurrentGetIntResult = nullptr;
	while (GetIntFinishedResults.Dequeue(CurrentGetIntResult))
	{
		CurrentGetIntResult->GetIntCallback.ExecuteIfBound(CurrentGetIntResult->bResult, CurrentGetIntResult->ResultValue);
		PUSH_ASYNC_RESULT(GetInt, CurrentGetIntResult);
	}

	FAsyncResultGetStr* CurrentGetStrResult = nullptr;
	while (GetStrFinishedResults.Dequeue(CurrentGetStrResult))
	{
		CurrentGetStrResult->GetStrCallback.ExecuteIfBound(CurrentGetStrResult->bResult, CurrentGetStrResult->ResultValue);
		PUSH_ASYNC_RESULT(GetStr, CurrentGetStrResult);
	}

	FAsyncResultHGet* CurrentHGetResult = nullptr;
	while (HGetFinishedResults.Dequeue(CurrentHGetResult))
	{
		CurrentHGetResult->HGetCallback.ExecuteIfBound(CurrentHGetResult->bResult, CurrentHGetResult->ResultValue);
		PUSH_ASYNC_RESULT(HGet, CurrentHGetResult);
	}

	FWrapMap TmpWrapMap;

	FAsyncResultHMGet* CurrentHMGetResult = nullptr;
	while (HMGetFinishedResults.Dequeue(CurrentHMGetResult))
	{
		TmpWrapMap.RealMap = CurrentHMGetResult->ResultFieldValueMap;
		CurrentHMGetResult->HMGetCallback.ExecuteIfBound(CurrentHMGetResult->bResult, TmpWrapMap);
		PUSH_ASYNC_RESULT(HMGet, CurrentHMGetResult);
	}

	FAsyncResultHGetAll* CurrentHGetAllResult = nullptr;
	while (HGetAllFinishedResults.Dequeue(CurrentHGetAllResult))
	{
		TmpWrapMap.RealMap = CurrentHGetAllResult->ResultFieldValueMap;
		CurrentHGetAllResult->HGetAllCallback.ExecuteIfBound(CurrentHGetAllResult->bResult, TmpWrapMap);
		PUSH_ASYNC_RESULT(HGetAll, CurrentHGetAllResult);
	}

	FAsyncResultSMembers* CurrentSMembersResult = nullptr;
	while (SMembersFinishedResults.Dequeue(CurrentSMembersResult))
	{
		FWrapArray TmpWrapArray;
		TmpWrapArray.RealArray = CurrentSMembersResult->ResultMemberList;
		CurrentSMembersResult->SMembersCallback.ExecuteIfBound(CurrentSMembersResult->bResult, TmpWrapArray);
		PUSH_ASYNC_RESULT(SMembers, CurrentSMembersResult);
	}

	for (auto& Iter : SubscribeMap)
	{
		if (Iter.Value)
		{
			Iter.Value->Tick(DeltaTime);
		}
	}

	return true;

	//return false;	// false for one-shot
}

bool URedisObject::ExecCommand(const FString& InCommand)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->ExecCommand(InCommand);
	}
	return false;
}

bool URedisObject::ExpireKey(const FString& InKey, int32 InSec)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->ExpireKey(InKey, InSec);
	}
	return false;
}

bool URedisObject::ExistsKey(const FString& InKey)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->ExistsKey(InKey);
	}
	return false;
}


bool URedisObject::PersistKey(const FString& InKey)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->PersistKey(InKey);
	}
	return false;
}


bool URedisObject::RenameKey(const FString& CurrentKey, const FString& NewKey)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->RenameKey(CurrentKey, NewKey);
	}
	return false;
}

bool URedisObject::DelKey(const FString& InKey)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->DelKey(InKey);
	}
	return false;
}


bool URedisObject::TypeKey(const FString& InKey, FString& OutType)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->TypeKey(InKey, OutType);
	}
	return false;
}


bool URedisObject::MSet(TMap<FString, FString>& InMemberMap)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->MSet(InMemberMap);
	}
	return false;
}


bool URedisObject::MGet(const TArray<FString>& InKeyList, TArray<FString>& OutMemberList)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->MGet(InKeyList, OutMemberList);
	}
	return false;
}

bool URedisObject::SetInt(const FString& InKey, int32 InValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->SetInt(InKey, InValue);
	}
	return false;
}

bool URedisObject::GetInt(const FString& InKey, int32& OutValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->GetInt(InKey, OutValue);
	}
	return false;
}

bool URedisObject::SetStr(const FString& InKey, const FString& InValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->SetStr(InKey, InValue);
	}
	return false;
}

bool URedisObject::GetStr(const FString& InKey, FString& OutValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->GetStr(InKey, OutValue);
	}
	return false;
}


bool URedisObject::Append(const FString& InKey, const FString& InValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->Append(InKey, InValue);
	}
	return false;
}

bool URedisObject::SAdd(const FString& InKey, const TArray<FString>& InMemberList)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->SAdd(InKey, InMemberList);
	}
	return false;
}


bool URedisObject::SCard(const FString& InKey, int32& OutValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->SCard(InKey, OutValue);
	}
	return false;
}

bool URedisObject::SRem(const FString& InKey, const TArray<FString>& InMemberList)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->SRem(InKey, InMemberList);
	}
	return false;
}

bool URedisObject::SMembers(const FString& InKey, TArray<FString>& OutMemberList)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->SMembers(InKey, OutMemberList);
	}
	return false;
}

bool URedisObject::HSet(const FString& InKey, const FString& InField, const FString& InValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->HSet(InKey, InField, InValue);
	}
	return false;
}

bool URedisObject::HGet(const FString& InKey, const FString& InField, FString& OutValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->HSet(InKey, InField, OutValue);
	}
	return false;
}

bool URedisObject::HMSet(const FString& InKey, const TMap<FString, FString>& InMemberMap)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->HMSet(InKey, InMemberMap);
	}
	return false;
}

bool URedisObject::HDel(const FString& InKey, const TArray<FString>& InFieldList)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->HDel(InKey, InFieldList);
	}
	return false;
}


bool URedisObject::HExists(const FString& InKey, const FString& Field)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->HExists(InKey, Field);
	}
	return false;
}

bool URedisObject::HMGet(const FString& InKey, const TSet<FString>& InFieldList, TMap<FString, FString>& OutMemberMap)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->HMGet(InKey, InFieldList, OutMemberMap);
	}
	return false;
}

bool URedisObject::HGetAll(const FString& InKey, TMap<FString, FString>& OutMemberMap)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->HGetAll(InKey, OutMemberMap);
	}
	return false;
}

bool URedisObject::HIncrby(const FString & Key, const FString & Field, int32 Value)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->HIncrby(Key, Field, Value);
	}
	return false;
}

bool URedisObject::LIndex(const FString& InKey, int32 InIndex, FString& OutValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->LIndex(InKey, InIndex, OutValue);
	}
	return false;
}

bool URedisObject::LInsertBefore(const FString& InKey, const FString& Pivot, const FString& InValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->LInsertBefore(InKey, Pivot, InValue);
	}
	return false;
}

bool URedisObject::LInsertAfter(const FString& InKey, const FString& Pivot, const FString& InValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->LInsertAfter(InKey, Pivot, InValue);
	}
	return false;
}

bool URedisObject::LLen(const FString& InKey, int32& Len)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->LLen(InKey, Len);
	}
	return false;
}

bool URedisObject::LPop(const FString& InKey, FString& OutValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->LPop(InKey, OutValue);
	}
	return false;
}

bool URedisObject::LPush(const FString& InKey, const TArray<FString>& InFieldList)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->LPush(InKey, InFieldList);
	}
	return false;
}

bool URedisObject::LRange(const FString& InKey, int32 Start, int32 End, TArray<FString>& OutMemberList)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->LRange(InKey, Start, End, OutMemberList);
	}
	return false;
}

bool URedisObject::LRem(const FString& InKey, const FString& InValue, int32 Count /*= 0*/)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->LRem(InKey, InValue, Count);
	}
	return false;
}

bool URedisObject::LSet(const FString& InKey, int32 InIndex, const FString& InValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->LSet(InKey, InIndex, InValue);
	}
	return false;
}

bool URedisObject::LTrim(const FString& InKey, int32 Start, int32 Stop)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->LTrim(InKey, Start, Stop);
	}
	return false;
}

bool URedisObject::RPop(const FString& InKey, FString& OutValue)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->RPop(InKey, OutValue);
	}
	return false;
}

bool URedisObject::RPush(const FString& InKey, const TArray<FString>& InFieldList)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->RPush(InKey, InFieldList);
	}
	return false;
}

void URedisObject::AsyncExistsKey(const FString& RedisKey, FExistsKeyFinished OnFinished)
{
	POP_ASYNC_RESULT(ExistsKey, ResultHandler);

	FAsyncTask<AsyncExistsKeyTask>* AsyncTaskPtr = new FAsyncTask<AsyncExistsKeyTask>(RedisKey, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyExistsKeyResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncExpireKey(const FString& InKey, int32 InSec)
{
	POP_ASYNC_RESULT(NoReturn, ResultHandler);

	FAsyncTask<AsyncExpireKeyTask>* AsyncTaskPtr = new FAsyncTask<AsyncExpireKeyTask>(InKey, InSec, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyNoReturnResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncDelKey(const FString& InKey)
{
	POP_ASYNC_RESULT(NoReturn, ResultHandler);

	FAsyncTask<AsyncDelKeyTask>* AsyncTaskPtr = new FAsyncTask<AsyncDelKeyTask>(InKey, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyNoReturnResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncSetInt(const FString& InKey, int32 InValue)
{
	POP_ASYNC_RESULT(NoReturn, ResultHandler);

	FAsyncTask<AsyncSetIntTask>* AsyncTaskPtr = new FAsyncTask<AsyncSetIntTask>(InKey, InValue, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyNoReturnResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncSetStr(const FString& InKey, const FString& InValue)
{
	POP_ASYNC_RESULT(NoReturn, ResultHandler);

	FAsyncTask<AsyncSetStrTask>* AsyncTaskPtr = new FAsyncTask<AsyncSetStrTask>(InKey, InValue, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyNoReturnResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncSAdd(const FString& InKey, const TArray<FString>& InMemberList)
{
	POP_ASYNC_RESULT(NoReturn, ResultHandler);

	FAsyncTask<AsyncSAddTask>* AsyncTaskPtr = new FAsyncTask<AsyncSAddTask>(InKey, InMemberList, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyNoReturnResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncSRem(const FString& InKey, const TArray<FString>& InMemberList)
{
	POP_ASYNC_RESULT(NoReturn, ResultHandler);

	FAsyncTask<AsyncSRemTask>* AsyncTaskPtr = new FAsyncTask<AsyncSRemTask>(InKey, InMemberList, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyNoReturnResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncHSet(const FString& InKey, const FString& InField, const FString& InValue)
{
	POP_ASYNC_RESULT(NoReturn, ResultHandler);

	FAsyncTask<AsyncHSetTask>* AsyncTaskPtr = new FAsyncTask<AsyncHSetTask>(InKey, InField, InValue, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyNoReturnResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncHMSet(const FString& InKey, const TMap<FString, FString>& InFieldValueMap)
{
	POP_ASYNC_RESULT(NoReturn, ResultHandler);

	FAsyncTask<AsyncHMSetTask>* AsyncTaskPtr = new FAsyncTask<AsyncHMSetTask>(InKey, InFieldValueMap, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyNoReturnResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncHDel(const FString& InKey, const TArray<FString>& InFieldList)
{
	POP_ASYNC_RESULT(NoReturn, ResultHandler);

	FAsyncTask<AsyncHDelTask>* AsyncTaskPtr = new FAsyncTask<AsyncHDelTask>(InKey, InFieldList, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyNoReturnResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncMGet(const TArray<FString>& InKeyList, FMGetFinished OnFinished)
{
	POP_ASYNC_RESULT(MGet, ResultHandler);
	ResultHandler->MGetCallback = OnFinished;

	FAsyncTask<AsyncMGetTask>* AsyncTaskPtr = new FAsyncTask<AsyncMGetTask>(InKeyList, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyMGetResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncGetInt(const FString& InKey, FGetIntFinished OnFinished)
{
	POP_ASYNC_RESULT(GetInt, ResultHandler);
	ResultHandler->GetIntCallback = OnFinished;

	FAsyncTask<AsyncGetIntTask>* AsyncTaskPtr = new FAsyncTask<AsyncGetIntTask>(InKey, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyGetIntResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncGetStr(const FString& InKey, FGetStrFinished OnFinished)
{
	POP_ASYNC_RESULT(GetStr, ResultHandler);
	ResultHandler->GetStrCallback = OnFinished;

	FAsyncTask<AsyncGetStrTask>* AsyncTaskPtr = new FAsyncTask<AsyncGetStrTask>(InKey, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyGetStrResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncHGet(const FString& InKey, const FString& InField, FHGetFinished OnFinished)
{
	POP_ASYNC_RESULT(HGet, ResultHandler);
	ResultHandler->HGetCallback = OnFinished;

	FAsyncTask<AsyncHGetTask>* AsyncTaskPtr = new FAsyncTask<AsyncHGetTask>(InKey, InField, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyHGetResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncHMGet(const FString& InKey, const TSet<FString>& InFieldList, FHMGetFinished OnFinished)
{
	POP_ASYNC_RESULT(HMGet, ResultHandler);
	ResultHandler->HMGetCallback = OnFinished;

	FAsyncTask<AsyncHMGetTask>* AsyncTaskPtr = new FAsyncTask<AsyncHMGetTask>(InKey, InFieldList, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyHMGetResult);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisObject::AsyncHGetAll(const FString& InKey, FHGetAllFinished OnFinished)
{
	POP_ASYNC_RESULT(HGetAll, ResultHandler);
	ResultHandler->HGetAllCallback = OnFinished;

	FAsyncTask<AsyncHGetAllTask>* AsyncTaskPtr = new FAsyncTask<AsyncHGetAllTask>(InKey, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifyHGetAllResult);
	AsyncTaskPtr->StartBackgroundTask();
}

bool URedisObject::Publish(const FString& Channel, const FString& Message)
{
	if (SyncRedisClient.Get())
	{
		return SyncRedisClient->Publish(Channel, Message);
	}
	return false;
}

void URedisObject::Subscribe(const FString& Channel)
{
	URedisSubscribeObject* SubObject = nullptr;
	if (SubscribeMap.Contains(Channel))
	{
		SubObject = *SubscribeMap.Find(Channel);
		if (SubObject)
		{
			if (SubObject->SubscribeChannel.IsEmpty())
			{
				SubObject->Subscribe(Channel);
				SubObject->OnSubscribeReply.BindUObject(this, &URedisObject::SubscribeCallback);
			}
		}
	}
	else
	{
		SubObject = NewObject<URedisSubscribeObject>(this);
		SubObject->Init(Host, Port, Password);
		SubObject->Subscribe(Channel);
		SubObject->OnSubscribeReply.BindUObject(this, &URedisObject::SubscribeCallback);
		SubscribeMap.Add(Channel, SubObject);
	}
}

void URedisObject::SubscribeCallback(FString Channel, FString Message)
{
	if (SubscribeReply.IsBound())
	{
		SubscribeReply.Broadcast(Channel, Message);
	}
}

void URedisObject::Unsubscribe(const FString& Channel)
{
	if (SubscribeMap.Contains(Channel))
	{
		URedisSubscribeObject* SubObject = *SubscribeMap.Find(Channel);
		SubObject->Unsubscribe();
	}
}

void URedisObject::AsyncSMembers(const FString& InKey, FSMembersFinished OnFinished)
{
	POP_ASYNC_RESULT(SMembers, ResultHandler);
	ResultHandler->SMembersCallback = OnFinished;

	FAsyncTask<AsyncSMembersTask>* AsyncTaskPtr = new FAsyncTask<AsyncSMembersTask>(InKey, ResultHandler);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisObject::OnNotifySMembersResult);
	AsyncTaskPtr->StartBackgroundTask();
}

