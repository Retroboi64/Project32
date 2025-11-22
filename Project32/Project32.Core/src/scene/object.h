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

#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "../common.h"
 
struct Node;
class Object {
public:
	std::string name;
	uint64_t id; 
	int scriptID;
	bool active = true;
	Transform transform;
	glm::vec4 color = glm::vec4(1.0f);

	Mesh* mesh;

	Object(const std::string& name = "Unnamed Object", Mesh* mesh = nullptr);
	virtual ~Object() = default;

	virtual void OnUpdate(float dt);
	virtual void OnDraw();
};

struct Node : public std::enable_shared_from_this<Node> {
	std::string name;
	Transform transform;

	std::weak_ptr<Node> parent;
	std::vector<std::shared_ptr<Node>> children;
	std::shared_ptr<Object> object;

	Node(const std::string& n = "Node");

	glm::mat4 GetLocalMatrix() const;
	glm::mat4 GetWorldMatrix() const;
	void AddChild(const std::shared_ptr<Node>& child);
};

struct Scene {
	std::string name = "Untitled Scene";
	std::vector<std::shared_ptr<Node>> roots;

	void AddRoot(const std::shared_ptr<Node>& node);
	void Update(float dt);
	void Draw();
};

#endif // OBJECT_H