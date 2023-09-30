#ifndef D3D11_BUFFER_HPP
#define D3D11_BUFFER_HPP

#include "Context.hpp"

class VertexBuffer
{

public:
	VertexBuffer(const void* data, size_t size, uint32_t stride, const Context* context);
	~VertexBuffer();

	void Stage() const;

private:
	ComPtr<ID3D11Buffer> m_VertexBuffer;
	ID3D11DeviceContext* m_DeviceContext;

	uint32_t m_Stride;
};

class IndexBuffer
{

public:
	IndexBuffer(const void* data, size_t count, const Context* context);
	~IndexBuffer();

	void Stage() const;
	uint32_t GetCount() const;

private:
	ComPtr<ID3D11Buffer> m_IndexBuffer;
	ID3D11DeviceContext* m_DeviceContext;

	uint32_t m_Count;
};


#endif