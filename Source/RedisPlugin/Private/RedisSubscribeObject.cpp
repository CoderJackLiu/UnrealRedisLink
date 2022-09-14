// Fill out your copyright notice in the Description page of Project Settings.


#include "RedisSubscribeObject.h"
#include "Async/AsyncWork.h"
#include "RedisClient.h"

class FSubscribeReplyTask : public FNonAbandonableTask
{
	TSharedPtr<URedisClient> RedisClient;
	FString CurrentChannel;
public:

	DECLARE_DELEGATE_TwoParams(FNotifySubscribeReply, FString, FString);

	FNotifySubscribeReply NotifySubscribeReply;

	friend class FAsyncTask<FSubscribeReplyTask>;

	FSubscribeReplyTask(TSharedPtr<URedisClient> InRedisClient, FString InChannel)
		: RedisClient(InRedisClient)
		, CurrentChannel(InChannel)
	{	}

	~FSubscribeReplyTask()
	{
		NotifySubscribeReply.Unbind();
	}

	FNotifySubscribeReply& GetDelegate()
	{
		return NotifySubscribeReply;
	}

	void DoWork()
	{
		while (RedisClient->Subscribed())
		{
			FString Channel, Message;
			if (RedisClient->SubscribeReply(Channel, Message))
			{
				NotifySubscribeReply.ExecuteIfBound(Channel, Message);
			}
		}
		RedisClient->UnsubscribeChannel(CurrentChannel);
	}

	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FSubscribeReplyTask, STATGROUP_ThreadPoolAsyncTasks); }
};

void URedisSubscribeObject::Init(const FString& InHost, int32 InPort, const FString& InPassword)
{
	RedisClient = MakeShareable(new URedisClient());
	RedisClient->ConnectToRedis(InHost, InPort, InPassword);
	
	// Start tick
// 	OnTickerDelegate = FTickerDelegate::CreateUObject(this, &URedisSubscribeObject::Tick);
// 	TickerHandle = FTicker::GetCoreTicker().AddTicker(OnTickerDelegate);
}

void URedisSubscribeObject::Subscribe(const FString& Channel)
{
	if (!SubscribeChannel.IsEmpty())
	{
		return;
	}
	SubscribeChannel = Channel;
	RedisClient->Subscribe(Channel);

	FAsyncTask<FSubscribeReplyTask>* AsyncTaskPtr = new FAsyncTask<FSubscribeReplyTask>(RedisClient, Channel);
	AsyncTaskPtr->GetTask().GetDelegate().BindUObject(this, &URedisSubscribeObject::OnNotifyReply);
	AsyncTaskPtr->StartBackgroundTask();
}

void URedisSubscribeObject::OnNotifyReply(FString Channel, FString Message)
{
	ReplyQueue.Enqueue(Channel);
	ReplyQueue.Enqueue(Message);
}

void URedisSubscribeObject::Unsubscribe()
{
	OnSubscribeReply.Unbind();
	RedisClient->Unsubscribe();
	SubscribeChannel.Empty();
}

bool URedisSubscribeObject::Tick(float DeltaTime)
{
	if (!ReplyQueue.IsEmpty())
	{
		FString Channel;
		FString Message;
		ReplyQueue.Dequeue(Channel);
		ReplyQueue.Dequeue(Message);
		OnSubscribeReply.ExecuteIfBound(Channel, Message);
	}
	return true;
}
