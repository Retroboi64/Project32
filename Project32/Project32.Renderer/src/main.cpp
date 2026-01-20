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

#include "r_common.h" 

extern "C" {
	__declspec(dllexport) void SetBackend() {
		// Placeholder function to simulate setting up the graphics backend
	}

	__declspec(dllexport) const char* GetBackendName() {
		return "OpenGL Backend";
	}

	__declspec(dllexport) GraphicsBackend* CreateBackend(Window& window, BackendType type) {
		std::unique_ptr<GraphicsBackend> backend;
		backend = std::make_unique<GraphicsBackend>();

		IGraphicsBackend* IBackend = backend->Get();
		if (!IBackend || !IBackend->Init()) {
			return nullptr;
		}
	}

	__declspec(dllexport) void InitializeBackend(Window& window) {
		// Placeholder function to simulate initializing the graphics backend
	}

	__declspec(dllexport) void ShutdownBackend() {
		// Placeholder function to simulate shutting down the graphics backend
	}
}