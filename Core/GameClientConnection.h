#pragma once
#include "stdafx.h"
#include <deque>
#include "GameConnection.h"
#include "../Utilities/AutoResetEvent.h"
#include "../Utilities/SimpleLock.h"
#include "BaseControlDevice.h"
#include "IInputProvider.h"
#include "ControlDeviceState.h"

class ClientConnectionData;

class GameClientConnection : public GameConnection, public INotificationListener, public IInputProvider
{
private:
	std::deque<ControlDeviceState> _inputData[BaseControlDevice::PortCount];
	atomic<uint32_t> _inputSize[BaseControlDevice::PortCount];
	AutoResetEvent _waitForInput[BaseControlDevice::PortCount];
	SimpleLock _writeLock;
	atomic<bool> _shutdown;
	atomic<bool> _enableControllers;
	atomic<uint32_t> _minimumQueueSize;

	vector<PlayerInfo> _playerList;

	shared_ptr<BaseControlDevice> _controlDevice;
	shared_ptr<BaseControlDevice> _newControlDevice;
	ControlDeviceState _lastInputSent;
	bool _gameLoaded = false;
	uint8_t _controllerPort = GameConnection::SpectatorPort;

private:
	void SendHandshake();
	void SendControllerSelection(uint8_t port);
	void ClearInputData();
	void PushControllerState(uint8_t port, ControlDeviceState state);
	void DisableControllers();

protected:
	void ProcessMessage(NetMessage* message) override;

public:
	GameClientConnection(shared_ptr<Socket> socket, shared_ptr<ClientConnectionData> connectionData);
	~GameClientConnection();

	void Shutdown();

	void ProcessNotification(ConsoleNotificationType type, void* parameter) override;

	bool SetInput(BaseControlDevice *device) override;
	void InitControlDevice();
	void SendInput();

	void SelectController(uint8_t port);
	uint8_t GetAvailableControllers();
	uint8_t GetControllerPort();
};