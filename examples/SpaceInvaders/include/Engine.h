#pragma once

#include <chrono>
#include <deque>
#include <entidy/Entidy.h>
#include <future>
#include <iostream>
#include <mutex>
#include <termios.h>
#include <unistd.h>
#include <vector>

#include "Console.h"

namespace entidy::spaceinvaders
{
using namespace std;
using namespace entidy;

class EngineImpl;

using Engine = shared_ptr<EngineImpl>;

class System
{
public:
	virtual void Init(Engine engine) = 0;
	virtual void Update(Engine engine) = 0;
};

struct timer final
{
	timer()
		: start{std::chrono::system_clock::now()}
	{ }

	double elapsed()
	{
		auto now = std::chrono::system_clock::now();
		return std::chrono::duration<double>(now - start).count();
	}

private:
	std::chrono::time_point<std::chrono::system_clock> start;
};

class EngineImpl : public enable_shared_from_this<EngineImpl>
{
protected:
	Registry registry;
	Console console;

	vector<shared_ptr<System>> systems;

	deque<char> input_buffer;

	bool running = true;

public:
	EngineImpl()
	{
		this->registry = RegistryFactory::New();
		this->console = make_shared<ConsoleImpl>();
	}

	void AddSystem(shared_ptr<System> system)
	{
		this->systems.push_back(system);
	}

	Registry Entidy()
	{
		return registry;
	}

	Console Renderer()
	{
		return console;
	}

	void Stop()
	{
		running = false;
	}

	char GetChar()
	{
		char buf = 0;
		struct termios old = {0};
		if(tcgetattr(0, &old) < 0)
			return 0;
		old.c_lflag &= ~ICANON;
		old.c_lflag &= ~ECHO;
		old.c_cc[VMIN] = 1;
		old.c_cc[VTIME] = 0;
		if(tcsetattr(0, TCSANOW, &old) < 0)
			return 0;
		if(read(0, &buf, 1) < 0)
			return 0;
		old.c_lflag |= ICANON;
		old.c_lflag |= ECHO;
		if(tcsetattr(0, TCSADRAIN, &old) < 0)
			return 0;
		return (buf);
	}

	deque<char>* InputBuffer()
	{
		return &input_buffer;
	}

	void Run()
	{
		for(auto& it : systems)
		{
			it->Init(shared_from_this());
		}

		auto future = std::async(std::launch::async, &EngineImpl::GetChar, this);

		while(running)
		{
			if(future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				input_buffer.push_back(future.get());
				future = std::async(std::launch::async, &EngineImpl::GetChar, this);
			}

			for(auto& it : systems)
			{
				it->Update(shared_from_this());
			}

			console->Render();

			usleep(1000);
		}
	}
};

} // namespace entidy::spaceinvaders
