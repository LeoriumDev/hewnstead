#pragma once

#include <hewnstead/core/gl_handle.hpp>

#include <glad/gl.h>

#include <type_traits>

namespace hs {

using VertexArrayHandle = GlHandle<+[](unsigned int id) { glDeleteVertexArrays(1, &id); }>;
using VertexBufferHandle = GlHandle<+[](unsigned int id) { glDeleteBuffers(1, &id); }>;
using TextureHandle = GlHandle<+[](unsigned int id) { glDeleteTextures(1, &id); }>;
using ProgramHandle = GlHandle<+[](unsigned int id) { glDeleteProgram(id); }>;
using ShaderObjectHandle = GlHandle<+[](unsigned int id) { glDeleteShader(id); }>;
using QueryHandle = GlHandle<+[](unsigned int id) { glDeleteQueries(1, &id); }>;

static_assert(std::is_nothrow_move_constructible_v<VertexArrayHandle>);
static_assert(std::is_nothrow_move_constructible_v<VertexBufferHandle>);
static_assert(std::is_nothrow_move_constructible_v<TextureHandle>);
static_assert(std::is_nothrow_move_constructible_v<ProgramHandle>);
static_assert(std::is_nothrow_move_constructible_v<ShaderObjectHandle>);
static_assert(std::is_nothrow_move_constructible_v<QueryHandle>);
}  // namespace hs
