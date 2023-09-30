#include "Shader.hpp"
#include <cassert>
#include "FileHandler.hpp"

std::unordered_map<ShaderType, std::string_view> Shader::ShaderEntrypoints =
{
    {ShaderType::Vertex, "vs_main"},
    {ShaderType::Pixel, "ps_main"},
    {ShaderType::Geometry, "gs_main"},
    {ShaderType::Domain, "ds_main"},
    {ShaderType::Hull, "hs_main"},
    {ShaderType::Compute, "cs_main"}
};

std::unordered_map<ShaderType, std::string_view> Shader::ShaderFileExtensions =
{
    {ShaderType::Vertex, "vs"},
    {ShaderType::Pixel, "ps"},
    {ShaderType::Geometry, "gs"},
    {ShaderType::Domain, "ds"},
    {ShaderType::Hull, "hs"},
    {ShaderType::Compute, "cs"}
};

Shader::Shader(const Context* context, std::string_view relativePath, const std::initializer_list<BufferElement>& elements) :
    m_Layout(elements)
{
    HRESULT hr;

    m_DeviceContext = context->GetDeviceContext();
    ID3D11Device* device = context->GetDevice();


    auto nativeElements = m_Layout.GetElements();
    D3D11_INPUT_ELEMENT_DESC* ied = new D3D11_INPUT_ELEMENT_DESC[elements.size()];

    for (size_t i = 0; i < nativeElements.size(); i++)
    {
        ied[i].SemanticName = nativeElements[i].Name.c_str();
        ied[i].SemanticIndex = 0;
        ied[i].Format = GetFormat(nativeElements[i].Type);
        ied[i].InputSlot = 0;
        ied[i].AlignedByteOffset = nativeElements[i].Offset;
        ied[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        ied[i].InstanceDataStepRate = 0;
    }

    std::stringstream filename;
    filename << relativePath << ".json";
    if(!FileHandler::FileExists(filename.str()))
        BuildCSO(relativePath);

    LoadBlobs(relativePath, device);

    device->CreateInputLayout(ied, elements.size(), m_VertexBlob->GetBufferPointer(), m_VertexBlob->GetBufferSize(), m_InputLayout.GetAddressOf());

    delete[] ied;
}

Shader::~Shader()
{

}

void Shader::Stage() const
{
    m_DeviceContext->IASetInputLayout(m_InputLayout.Get());
    m_DeviceContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
    m_DeviceContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);
    if (m_GeometryShader)
        m_DeviceContext->GSSetShader(m_GeometryShader.Get(), nullptr, 0);
    if (m_DomainShader)
        m_DeviceContext->DSSetShader(m_DomainShader.Get(), nullptr, 0);
    if (m_HullShader)
        m_DeviceContext->HSSetShader(m_HullShader.Get(), nullptr, 0);
    if (m_ComputeShader)
        m_DeviceContext->CSSetShader(m_ComputeShader.Get(), nullptr, 0);
}

uint32_t Shader::GetStride() const
{
    return m_Layout.GetStride();
}

uint32_t Shader::GetOffset() const
{
    return 0;
}

DXGI_FORMAT Shader::GetFormat(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float: return DXGI_FORMAT_R32_FLOAT;
    case ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
    case ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
    case ShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case ShaderDataType::Uint: return DXGI_FORMAT_R32_UINT;
    case ShaderDataType::Uint2: return DXGI_FORMAT_R32G32_UINT;
    case ShaderDataType::Uint3: return DXGI_FORMAT_R32G32B32_UINT;
    case ShaderDataType::Uint4: return DXGI_FORMAT_R32G32B32A32_UINT;
    case ShaderDataType::Bool: return DXGI_FORMAT_R8_UINT;
    default: return DXGI_FORMAT_UNKNOWN;
    }
}

void Shader::BuildBlob(Json::Value* jsonInfos, std::string_view baseFilepath, ShaderType shaderType)
{
    
    bool debugMode;
    uint32_t compileFlags;
#ifdef _DEBUG
    compileFlags = D3DCOMPILE_DEBUG;
    debugMode = true;
#else
    compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
    debugMode = false;
#endif // _DEBUG

    ID3DBlob* shaderBlob;
    ID3DBlob* errorBlob;

    std::stringstream target;
    target << ShaderFileExtensions[shaderType] << "_5_0";

    HRESULT hr = D3DCompile(ShaderSource::shader.c_str(), ShaderSource::shader.size(), nullptr, nullptr, nullptr, 
        ShaderEntrypoints[shaderType].data(), target.str().c_str(), compileFlags, 0, &shaderBlob, &errorBlob);

    if (errorBlob)
    {
        if (errorBlob->GetBufferSize())
        {
            OutputDebugStringA((const char*)errorBlob->GetBufferPointer());
            OutputDebugStringA("\n");
        }
        errorBlob->Release();
        if ((shaderType == ShaderType::Vertex) || (shaderType == ShaderType::Pixel))
        {
            OutputDebugStringA("Vertex and Pixel shaders are mandatory");
            OutputDebugStringA("\n");
            exit(15);
        }
        if ((m_PipelineHasDomainOrHull && (shaderType == ShaderType::Hull)) || (m_PipelineHasDomainOrHull && (shaderType == ShaderType::Domain)))
        {
            OutputDebugStringA("If Domain or Hull shader exists, the another one is mandatory");
            OutputDebugStringA("\n");
            exit(15);
        }
        std::stringstream filename;
        filename << "";
        (*jsonInfos)[ShaderFileExtensions[shaderType].data()] = filename.str();
    }
    if (!errorBlob)
    {
        if((shaderType == ShaderType::Domain) || (shaderType == ShaderType::Hull))
            m_PipelineHasDomainOrHull = true;
        std::stringstream filename;
        filename << baseFilepath << "." << ShaderFileExtensions[shaderType] << ".cso";
        (*jsonInfos)[ShaderFileExtensions[shaderType].data()] = filename.str();
        FileHandler::WriteBinFile(filename.str(), (std::byte*)shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());
        shaderBlob->Release();
    }
}

void Shader::LoadBlobs(std::string_view relativePath, ID3D11Device* device)
{
    HRESULT hr;
    std::string jsonStr;
    std::wstring w_Filepath;

    std::stringstream jsonFilename;
    jsonFilename << relativePath << ".json";
    FileHandler::ReadTextFile(jsonFilename.str(), &jsonStr);

    Json::Value infos;
    Json::Reader jsonContent;
    jsonContent.parse(jsonStr, infos);
    std::string shaderPath;
    
    if (infos["vs"].asString().size() == 0)
        exit(16);
    if (infos["ps"].asString().size() == 0)
        exit(17);


    shaderPath = infos["vs"].asString();
    w_Filepath = std::wstring(shaderPath.begin(), shaderPath.end());
    D3DReadFileToBlob(w_Filepath.data(), m_VertexBlob.GetAddressOf());
    hr = device->CreateVertexShader(m_VertexBlob->GetBufferPointer(), m_VertexBlob->GetBufferSize(), nullptr, m_VertexShader.GetAddressOf());
    assert(hr == S_OK);

    shaderPath = infos["ps"].asString();
    w_Filepath = std::wstring(shaderPath.begin(), shaderPath.end());
    D3DReadFileToBlob(w_Filepath.data(), m_PixelBlob.GetAddressOf());
    hr = device->CreatePixelShader(m_PixelBlob->GetBufferPointer(), m_PixelBlob->GetBufferSize(), nullptr, m_PixelShader.GetAddressOf());
    assert(hr == S_OK);

    if (infos["gs"].asString().size() > 0)
    {
        shaderPath = infos["gs"].asString();
        w_Filepath = std::wstring(shaderPath.begin(), shaderPath.end());
        D3DReadFileToBlob(w_Filepath.data(), m_GeometryBlob.GetAddressOf());
        hr = device->CreateGeometryShader(m_GeometryBlob->GetBufferPointer(), m_GeometryBlob->GetBufferSize(), nullptr, m_GeometryShader.GetAddressOf());
        assert(hr == S_OK);
    }

    if (infos["ds"].asString().size() > 0)
    {
        shaderPath = infos["ds"].asString();
        w_Filepath = std::wstring(shaderPath.begin(), shaderPath.end());
        D3DReadFileToBlob(w_Filepath.data(), m_DomainBlob.GetAddressOf());
        hr = device->CreateDomainShader(m_DomainBlob->GetBufferPointer(), m_DomainBlob->GetBufferSize(), nullptr, m_DomainShader.GetAddressOf());
        assert(hr == S_OK);
    }

    if (infos["hs"].asString().size() > 0)
    {
        shaderPath = infos["hs"].asString();
        w_Filepath = std::wstring(shaderPath.begin(), shaderPath.end());
        D3DReadFileToBlob(w_Filepath.data(), m_HullBlob.GetAddressOf());
        hr = device->CreateHullShader(m_HullBlob->GetBufferPointer(), m_HullBlob->GetBufferSize(), nullptr, m_HullShader.GetAddressOf());
        assert(hr == S_OK);
    }

    if (infos["cs"].asString().size() > 0)
    {
        shaderPath = infos["cs"].asString();
        w_Filepath = std::wstring(shaderPath.begin(), shaderPath.end());
        D3DReadFileToBlob(w_Filepath.data(), m_ComputeBlob.GetAddressOf());
        hr = device->CreateComputeShader(m_ComputeBlob->GetBufferPointer(), m_ComputeBlob->GetBufferSize(), nullptr, m_ComputeShader.GetAddressOf());
        assert(hr == S_OK);
    }
}

void Shader::BuildCSO(std::string_view baseFilepath)
{
    Json::Value infos;
    BuildBlob(&infos, baseFilepath, ShaderType::Vertex);
    BuildBlob(&infos, baseFilepath, ShaderType::Pixel);
    BuildBlob(&infos, baseFilepath, ShaderType::Geometry);
    BuildBlob(&infos, baseFilepath, ShaderType::Domain);
    BuildBlob(&infos, baseFilepath, ShaderType::Hull);
    BuildBlob(&infos, baseFilepath, ShaderType::Compute);
    std::stringstream filename;
    filename << baseFilepath << ".json";
    FileHandler::WriteTextFile(filename.str(), infos.toStyledString());
}

