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
#include "object.h"

static uint64_t g_objectCounter = 0;

Object::Object(const std::string& n, Mesh* mesh)
	: name(n), mesh(mesh), id(++g_objectCounter) {
}

void Object::OnUpdate(float dt) {}

void Object::OnDraw() {
	if (mesh) {
		mesh->Draw();
		spdlog::debug("[Object {}] Drawn", id);
		return;
	}
	spdlog::warn("[Object {}] OnDraw called but no mesh assigned!", id);
}

Node::Node(const std::string& n)
	: name(n) {
}

glm::mat4 Node::GetLocalMatrix() const {
	glm::mat4 m(1.0f);
	m = glm::translate(m, transform.position);
	m = glm::rotate(m, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
	m = glm::rotate(m, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
	m = glm::rotate(m, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
	m = glm::scale(m, transform.scale);
	return m;
}

glm::mat4 Node::GetWorldMatrix() const {
	if (auto p = parent.lock())
		return p->GetWorldMatrix() * GetLocalMatrix();
	return GetLocalMatrix();
}

void Node::AddChild(const std::shared_ptr<Node>& child) {
	child->parent = shared_from_this();
	children.push_back(child);
}

void Scene::AddRoot(const std::shared_ptr<Node>& node) {
	roots.push_back(node);
}

void Scene::Update(float dt) {
	for (auto& node : roots) {
		if (node->object && node->object->active)
			node->object->OnUpdate(dt);
	}
}

void Scene::Draw() {
	for (auto& node : roots) {
		if (node->object && node->object->active)
			node->object->OnDraw();
	}
}
