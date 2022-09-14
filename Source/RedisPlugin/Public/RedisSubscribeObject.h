// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Core/Public/Containers/Queue.h"
#include "Runtime/Core/Public/Containers/Ticker.h"
#include "RedisSubscribeObject.generated.h"

class URedisClient;
DECLARE_DELEGATE_TwoParams(FOnSubscribeReply, FString, FString);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class REDISPLUGIN_API URedisSubscribeObject : public UObject
{
	GENERATED_BODY()
public:
	virtual void Init(const FString& InHost, int32 InPort, const FString& InPassword);

	virtual void Subscribe(const FString& Channel);

	virtual void Unsubscribe();

	bool Tick(float DeltaTime);

	FOnSubscribeReply OnSubscribeReply;

	FString SubscribeChannel;

private:
	virtual void OnNotifyReply(FString Channel, FString Message);

	TSharedPtr<URedisClient> RedisClient;
	TQueue<FString, EQueueMode::Mpsc> ReplyQueue;
	
	
	FDelegateHandle TickerHandle;
	FTickerDelegate OnTickerDelegate;
};
