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
	double fps;

	deque<char> input_buffer;

	bool running = true;

public:
	EngineImpl(double fps)
	{
		this->registry = RegistryFactory::New();
		this->console = make_shared<ConsoleImpl>();
		this->fps = fps;
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
			perror("tcsetattr()");
		old.c_lflag &= ~ICANON;
		old.c_lflag &= ~ECHO;
		old.c_cc[VMIN] = 1;
		old.c_cc[VTIME] = 0;
		if(tcsetattr(0, TCSANOW, &old) < 0)
			perror("tcsetattr ICANON");
		if(read(0, &buf, 1) < 0)
			perror("read()");
		old.c_lflag |= ICANON;
		old.c_lflag |= ECHO;
		if(tcsetattr(0, TCSADRAIN, &old) < 0)
			perror("tcsetattr ~ICANON");
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

			timer t;
			for(auto& it : systems)
			{
				it->Update(shared_from_this());
			}

			console->Render();

			double elapsed = t.elapsed();
			double target = 1.0 / fps;
			double tusleep = (target - elapsed) * 1000 * 1000;
			if(tusleep > 0)
				usleep(tusleep);
		}
	}
};

} // namespace entidy::spaceinvaders
