/*
 * This file is part of Project32 - A compact yet powerful and flexible C++ Game Engine
 * Copyright (c) 2025 Patrick Reese (Retroboi64)
 *
 * Licensed under MIT with Attribution Requirements
 * See LICENSE file for full terms
 * GitHub: https://github.com/Retroboi64/Project32
 *
 * This header must not be removed from any source file.
 */

#include "common.h"
#include "engine.h"
#include "renderer.h"
#include "input.h"
#include "window.h"

int Engine::s_nextID = 0;
std::unique_ptr<EngineManager> EngineManager::s_instance = nullptr;

Engine::Engine(const std::string& title)
	: title(title), _ID(s_nextID++)
{
	try {
		_windowManager = std::make_unique<WindowManager>();
		Init();
	}
	catch (const std::exception& e) {
		std::cerr << "[Engine::Constructor] Failed to initialize engine " << _ID << ": " << e.what() << std::endl;
		throw;
	}
}

Engine::~Engine() {
	Shutdown();
}

void Engine::Init() {
	try {
		if (_windowManager->Count() == 0) {
			std::cout << "[Engine::Init] Engine " << _ID << " - Warning: No windows in manager" << std::endl;
		}
		else if (_mainWindowID < 0) {
			std::cout << "[Engine::Init] Engine " << _ID << " - Warning: Windows exist but main window ID is invalid" << std::endl;
		}
		else {
			std::cout << "[Engine::Init] Engine " << _ID << " initialized successfully with windows "
				<< _mainWindowID << " (Total windows: " << _windowManager->Count() << ")" << std::endl;
		}

		isRunning = true;
		std::cout << "[Engine::Init] Engine " << _ID << " initialized successfully with window ID " << _mainWindowID << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "[Engine::Init] Fatal error in engine " << _ID << ": " << e.what() << std::endl;
		Shutdown();
		throw;
	}
}

void Engine::Run() {
	if (!isRunning) {
		std::cerr << "[Engine::Run] Engine " << _ID << " is not running, cannot start main loop" << std::endl;
		return;
	}

	FrameTimer timer;

	std::cout << "[Engine::Run] Starting main loop for engine " << _ID << std::endl;

	while (isRunning) {
		timer.Update();
		float dt = timer.GetDeltaTime();

		int windowCount = _windowManager->Count();
		for (int i = 0; i < windowCount; i++) {
			Window* window = _windowManager->GetWindowAt(i);

			if (!window) {
				std::cerr << "[Engine::Run] Warning: null window at index " << i << std::endl;
				continue;
			}

			window->PollEvents();

			if (!window->IsOpen()) {
				std::cout << "[Engine::Run] Window " << window->GetID() << " requested close" << std::endl;
				isRunning = false;
				break;
			}

			window->Render();
			window->SwapBuffers();
		}
	}

	std::cout << "[Engine::Run] Main loop ended for engine " << _ID << std::endl;

	Shutdown();
}

void Engine::Shutdown() {
	if (!isRunning) return;

	std::cout << "[Engine::Shutdown] Shutting down engine " << _ID << std::endl;

	if (_windowManager) {
		_windowManager->RemoveAllWindows();
		_windowManager.reset();
	}

	isRunning = false;
	_mainWindowID = -1;

	std::cout << "[Engine::Shutdown] Engine " << _ID << " shutdown complete" << std::endl;
}

// EngineManager Implementation
EngineManager* EngineManager::Instance() {
	if (!s_instance) {
		s_instance = std::make_unique<EngineManager>();
	}
	return s_instance.get();
}

void EngineManager::DestroyInstance() {
	if (s_instance) {
		s_instance->DestroyAllEngines();
		s_instance.reset();
	}
}

int EngineManager::CreateEngine(const std::string& title) {
	try {
		auto engine = std::make_unique<Engine>(title);
		int engineID = engine->GetID();

		_engines.push_back(std::move(engine));

		if (_currentEngine == nullptr) {
			SetCurrentEngine(engineID);
		}

		std::cout << "[EngineManager] Created engine " << engineID << ": " << title << std::endl;
		return engineID;
	}
	catch (const std::exception& e) {
		std::cerr << "[EngineManager] Failed to create engine: " << e.what() << std::endl;
		return -1;
	}
}

void EngineManager::RunAllEngines() {
	std::cout << "[EngineManager] Starting main loop for all engines" << std::endl;

	if (_engines.empty()) {
		std::cerr << "[EngineManager] No engines to run!" << std::endl;
		return;
	}

	bool hasWindows = false;
	for (const auto& enginePtr : _engines) {
		if (enginePtr && enginePtr->GetWindowManager() && enginePtr->GetWindowManager()->Count() > 0) {
			hasWindows = true;
			break;
		}
	}

	if (!hasWindows) {
		std::cerr << "[EngineManager] No windows in any engine, cannot run!" << std::endl;
		return;
	}

	FrameTimer timer;

	while (true) {
		timer.Update();
		float dt = timer.GetDeltaTime();

		glfwPollEvents();

		bool anyRunning = false;

		for (auto& enginePtr : _engines) {
			Engine* engine = enginePtr.get();
			if (!engine || !engine->IsRunning()) continue;

			anyRunning = true;

			WindowManager* wm = engine->GetWindowManager();
			if (!wm) continue;

			int windowCount = wm->Count();
			bool engineHasOpenWindow = false;

			for (int i = 0; i < windowCount; i++) {
				Window* window = wm->GetWindowAt(i);

				if (!window) {
					std::cerr << "[EngineManager::RunAllEngines] Warning: null window at index " << i << std::endl;
					continue;
				}

				window->PollEvents();

				if (!window->IsOpen()) {
					std::cout << "[EngineManager::RunAllEngines] Window " << window->GetID()
						<< " in engine " << engine->GetID() << " closed" << std::endl;
					continue;
				}

				engineHasOpenWindow = true;
				window->Render();
				window->SwapBuffers();
			}

			if (!engineHasOpenWindow) {
				std::cout << "[EngineManager::RunAllEngines] Engine " << engine->GetID()
					<< " has no open windows, shutting down" << std::endl;
				engine->Shutdown();
			}
		}

		if (!anyRunning) {
			std::cout << "[EngineManager] No running engines, exiting main loop" << std::endl;
			break;
		}
	}

	std::cout << "[EngineManager] Main loop exited, ensuring all engines are shut down" << std::endl;

	for (auto& engine : _engines) {
		if (engine && engine->IsRunning()) {
			engine->Shutdown();
		}
	}

	glfwTerminate();
}

bool EngineManager::DestroyEngine(int engineID) {
	auto it = std::find_if(_engines.begin(), _engines.end(),
		[engineID](const std::unique_ptr<Engine>& engine) {
			return engine->GetID() == engineID;
		});

	if (it != _engines.end()) {
		if (_currentEngine && _currentEngine->GetID() == engineID) {
			_currentEngine = nullptr;
			_currentEngineID = -1;
		}

		std::cout << "[EngineManager] Destroying engine " << engineID << std::endl;
		_engines.erase(it);

		if (!_engines.empty() && _currentEngine == nullptr) {
			SetCurrentEngine(_engines[0]->GetID());
		}

		return true;
	}
	return false;
}

void EngineManager::DestroyAllEngines() {
	std::cout << "[EngineManager] Destroying all engines (" << _engines.size() << ")" << std::endl;
	_currentEngine = nullptr;
	_currentEngineID = -1;
	_engines.clear();
}

bool EngineManager::SetCurrentEngine(int engineID) {
	auto it = std::find_if(_engines.begin(), _engines.end(),
		[engineID](const std::unique_ptr<Engine>& engine) {
			return engine->GetID() == engineID;
		});

	if (it != _engines.end()) {
		_currentEngine = it->get();
		_currentEngineID = engineID;
		std::cout << "[EngineManager] Set current engine to " << engineID << std::endl;
		return true;
	}

	std::cerr << "[EngineManager] Warning: Could not set current engine to " << engineID << " (not found)" << std::endl;
	return false;
}

Engine* EngineManager::GetEngineByID(int engineID) const {
	auto it = std::find_if(_engines.begin(), _engines.end(),
		[engineID](const std::unique_ptr<Engine>& engine) {
			return engine->GetID() == engineID;
		});

	return (it != _engines.end()) ? it->get() : nullptr;
}

std::vector<Engine*> EngineManager::GetAllEngines() const {
	std::vector<Engine*> engines;
	engines.reserve(_engines.size());

	for (const auto& engine : _engines) {
		engines.push_back(engine.get());
	}

	return engines;
}

bool EngineManager::HasEngine(int engineID) const {
	return GetEngineByID(engineID) != nullptr;
}

void EngineManager::PrintEngineInfo() const {
	std::cout << "[EngineManager] Engine Count: " << _engines.size() << std::endl;
	std::cout << "[EngineManager] Current Engine: " << _currentEngineID << std::endl;

	for (const auto& engine : _engines) {
		std::cout << "  Engine " << engine->GetID() << ": "
			<< (engine->IsRunning() ? "Running" : "Stopped") << std::endl;
	}
}