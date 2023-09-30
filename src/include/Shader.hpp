#ifndef SHADER_HPP
#define SHADER_HPP

#include <vector>
#include <cassert>
#include <cstdint>
#include <string>
#include <unordered_map>
#include "Context.hpp"
#include <d3dcompiler.h>
#include <json/json.h>


namespace ShaderSource
{
	const std::string shader = R"(struct VS_In
{
	float3 pos : POSITION;
	float4 color : COLOR;
};

struct PS_In
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

PS_In vs_main(VS_In input)
{
	PS_In output;
	output.pos = float4(input.pos, 1.0f);
	output.color = input.color;
	return output;
}


float4 ps_main(PS_In input) : SV_TARGET
{
    return input.color;
}
)";
}

enum class ShaderDataType
{
	None = 0,
	Float,
	Float2,
	Float3,
	Float4,
	Uint,
	Uint2,
	Uint3,
	Uint4,
	Bool
};

enum class ShaderType
{
	Vertex = 0,
	Pixel,
	Geometry,
	Domain,
	Hull,
	Compute
};

static uint32_t ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float: return 4;
	case ShaderDataType::Float2: return 4 * 2;
	case ShaderDataType::Float3: return 4 * 3;
	case ShaderDataType::Float4: return 4 * 4;
	case ShaderDataType::Uint: return 4;
	case ShaderDataType::Uint2: return 4 * 2;
	case ShaderDataType::Uint3: return 4 * 3;
	case ShaderDataType::Uint4: return 4 * 4;
	case ShaderDataType::Bool: return 1;
	default: assert(false); return 0;
	}
}

struct BufferElement
{
	std::string Name;
	uint32_t Offset;
	uint32_t Size;
	ShaderDataType Type;
	bool Normalized;

	BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
		: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
	{}

	uint32_t GetElementCount() const
	{
		switch (Type)
		{
		case ShaderDataType::Bool:
		case ShaderDataType::Float:
		case ShaderDataType::Uint:
			return 1Ui32;
		case ShaderDataType::Float2:
		case ShaderDataType::Uint2:
			return 2Ui32;
		case ShaderDataType::Float3:
		case ShaderDataType::Uint3:
			return 3Ui32;
		case ShaderDataType::Float4:
		case ShaderDataType::Uint4:
			return 4Ui32;
		default:
			return 0Ui32;
		}
	}
};

class BufferLayout
{
public:
	BufferLayout() {}
	BufferLayout(const std::initializer_list<BufferElement>& elements)
		: m_Elements(elements)
	{
		CalculateOffsetAndStride();
	}

	inline const uint32_t GetStride() const { return m_Stride; }
	inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

private:

	void CalculateOffsetAndStride()
	{
		uint32_t offset = 0;
		m_Stride = 0;
		for (auto& element : m_Elements)
		{
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}

	std::vector<BufferElement> m_Elements;
	uint32_t m_Stride = 0;
};

class Shader
{
public:
	Shader(const Context* context, std::string_view relativePath, const std::initializer_list<BufferElement>& elements);
	~Shader();

	virtual void Stage() const;

	virtual uint32_t GetStride() const;
	virtual uint32_t GetOffset() const;
private:

	ComPtr<ID3D11InputLayout> m_InputLayout;

	ComPtr<ID3D11VertexShader> m_VertexShader;
	ComPtr<ID3DBlob> m_VertexBlob;
	ComPtr<ID3D11PixelShader> m_PixelShader;
	ComPtr<ID3DBlob> m_PixelBlob;
	ComPtr<ID3D11GeometryShader> m_GeometryShader;
	ComPtr<ID3DBlob> m_GeometryBlob;
	ComPtr<ID3D11DomainShader> m_DomainShader;
	ComPtr<ID3DBlob> m_DomainBlob;
	ComPtr<ID3D11HullShader> m_HullShader;
	ComPtr<ID3DBlob> m_HullBlob;
	ComPtr<ID3D11ComputeShader> m_ComputeShader;
	ComPtr<ID3DBlob> m_ComputeBlob;

	BufferLayout m_Layout;
	ID3D11DeviceContext* m_DeviceContext;

	DXGI_FORMAT GetFormat(ShaderDataType type);
	void BuildCSO(std::string_view baseFilepath);
	void BuildBlob(Json::Value* jsonInfos, std::string_view baseFilepath, ShaderType shaderType);
	bool m_PipelineHasDomainOrHull = false;

	void LoadBlobs(std::string_view relativePath, ID3D11Device* device);

	static std::unordered_map<ShaderType, std::string_view> ShaderEntrypoints;
	static std::unordered_map<ShaderType, std::string_view> ShaderFileExtensions;
};

#endif
