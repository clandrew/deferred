#include "stdafx.h"
#include "Sample3DSceneRenderer.h"
#include "DirectXHelper.h"

using namespace SpinningCube;

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;

// Indices into the application state map.
std::string AngleKey = "Angle";
std::string TrackingKey = "Tracking";

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_radiansPerSecond(XM_PIDIV4),	// rotate 45 degrees per second
	m_angle(0),
	m_tracking(false),
	m_mappedConstantBuffer(nullptr),
	m_deviceResources(deviceResources)
{
	ZeroMemory(&m_constantBufferData, sizeof(m_constantBufferData));

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

Sample3DSceneRenderer::~Sample3DSceneRenderer()
{
	m_constantBuffer->Unmap(0, nullptr);
	m_mappedConstantBuffer = nullptr;
}

Sample3DSceneRenderer::UploadResource Sample3DSceneRenderer::CreateCubeGeometryResources()
{
	UploadResource uploadResource{};

	auto d3dDevice = m_deviceResources->GetD3DDevice();

	VertexPositionNormalColor cubeVertices[] =
	{
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(-1, 0, 0), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0  // -x
		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(-1, 0, 0), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 2
		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(-1, 0, 0), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 1
		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(-1, 0, 0), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 1
		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(-1, 0, 0), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 2
		{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(-1, 0, 0), XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 3

		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1, 0, 0), XMFLOAT3(1.0f, 0.0f, 0.0f) },  // 4 // +x
		{ XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(1, 0, 0), XMFLOAT3(1.0f, 0.0f, 1.0f) }, // 5
		{ XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(1, 0, 0), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 6
		{ XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(1, 0, 0), XMFLOAT3(1.0f, 0.0f, 1.0f) }, // 5
		{ XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(1, 0, 0), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 7
		{ XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(1, 0, 0), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 6

		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0, -1, 0), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0  // -y
		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0, -1, 0), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 1
		{ XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(0, -1, 0), XMFLOAT3(1.0f, 0.0f, 1.0f) }, // 5
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0, -1, 0), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
		{ XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(0, -1, 0), XMFLOAT3(1.0f, 0.0f, 1.0f) }, // 5
		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0, -1, 0), XMFLOAT3(1.0f, 0.0f, 0.0f) },  // 4

		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0, 1, 0), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 2
		{ XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(0, 1, 0), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 6
		{ XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(0, 1, 0), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 7
		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0, 1, 0), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 2
		{ XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(0, 1, 0), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 7
		{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0, 1, 0), XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 3

		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0, 0, -1), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(0, 0, -1), XMFLOAT3(1.0f, 0.0f, 0.0f) },  // 4
		{ XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(0, 0, -1), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 6
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0, 0, -1), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
		{ XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(0, 0, -1), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 6
		{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0, 0, -1), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 2

		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0, 0, 1), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 1
		{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0, 0, 1), XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 3
		{ XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(0, 0, 1), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 7
		{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0, 0, 1), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 1
		{ XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(0, 0, 1), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 7
		{ XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(0, 0, 1), XMFLOAT3(1.0f, 0.0f, 1.0f) }, // 5
	};


	// Cube vertices. Each vertex has a position and a color.

	const UINT vertexBufferSize = sizeof(cubeVertices);

	CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexBufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_cubeVertexBuffer)));

	CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadResource.VertexBufferUpload)));

	NAME_D3D12_OBJECT(m_cubeVertexBuffer);

	// Upload the vertex buffer to the GPU.
	{
		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(cubeVertices);
		vertexData.RowPitch = vertexBufferSize;
		vertexData.SlicePitch = vertexData.RowPitch;

		UpdateSubresources(m_commandList.Get(), m_cubeVertexBuffer.Get(), uploadResource.VertexBufferUpload.Get(), 0, 0, 1, &vertexData);

		CD3DX12_RESOURCE_BARRIER vertexBufferResourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(m_cubeVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		m_commandList->ResourceBarrier(1, &vertexBufferResourceBarrier);
	}

	std::vector<unsigned short> cubeIndices;
	for (unsigned short i = 0; i < _countof(cubeVertices); ++i)
	{
		cubeIndices.push_back(i);
	}

	const UINT indexBufferSize = cubeIndices.size() * sizeof(unsigned short);

	CD3DX12_RESOURCE_DESC indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&indexBufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_cubeIndexBuffer)));

	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&indexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadResource.IndexBufferUpload)));

	NAME_D3D12_OBJECT(m_cubeIndexBuffer);

	// Upload the index buffer to the GPU.
	{
		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<BYTE*>(cubeIndices.data());
		indexData.RowPitch = indexBufferSize;
		indexData.SlicePitch = indexData.RowPitch;

		UpdateSubresources(m_commandList.Get(), m_cubeIndexBuffer.Get(), uploadResource.IndexBufferUpload.Get(), 0, 0, 1, &indexData);

		CD3DX12_RESOURCE_BARRIER indexBufferResourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(m_cubeIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		m_commandList->ResourceBarrier(1, &indexBufferResourceBarrier);
	}

	// Create vertex/index buffer views.
	m_cubeVertexBufferView.BufferLocation = m_cubeVertexBuffer->GetGPUVirtualAddress();
	m_cubeVertexBufferView.StrideInBytes = sizeof(VertexPositionNormalColor);
	m_cubeVertexBufferView.SizeInBytes = sizeof(cubeVertices);

	m_cubeIndexBufferView.BufferLocation = m_cubeIndexBuffer->GetGPUVirtualAddress();
	m_cubeIndexBufferView.SizeInBytes = indexBufferSize;
	m_cubeIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

	return uploadResource;
}

Sample3DSceneRenderer::UploadResource Sample3DSceneRenderer::CreateQuadGeometryResources()
{
	UploadResource uploadResource{};

	auto d3dDevice = m_deviceResources->GetD3DDevice();

	VertexPositionNormalColor quadVertices[] =
	{
		{ XMFLOAT3(-1, 1, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // top left
		{ XMFLOAT3(1, 1, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(1.0f, 0.0f, 0.0f) }, // top right
		{ XMFLOAT3(-1, -1, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // bottom left
		{ XMFLOAT3(1, -1,  0), XMFLOAT3(0, 0, 0), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // bottom right
	};

	const UINT vertexBufferSize = sizeof(quadVertices);

	CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexBufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_quadVertexBuffer)));

	CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadResource.VertexBufferUpload)));

	NAME_D3D12_OBJECT(m_quadVertexBuffer);

	// Upload the vertex buffer to the GPU.
	{
		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(quadVertices);
		vertexData.RowPitch = vertexBufferSize;
		vertexData.SlicePitch = vertexData.RowPitch;

		UpdateSubresources(m_commandList.Get(), m_quadVertexBuffer.Get(), uploadResource.VertexBufferUpload.Get(), 0, 0, 1, &vertexData);

		CD3DX12_RESOURCE_BARRIER vertexBufferResourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(m_quadVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		m_commandList->ResourceBarrier(1, &vertexBufferResourceBarrier);
	}

	// Load mesh indices. Each trio of indices represents a triangle to be rendered on the screen.
	// For example: 0,2,1 means that the vertices with indexes 0, 2 and 1 from the vertex buffer compose the
	// first triangle of this mesh.
	unsigned short quadIndices[] =
	{
		0, 1, 2,
		2, 1, 3,
	};

	const UINT indexBufferSize = sizeof(quadIndices);

	CD3DX12_RESOURCE_DESC indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&indexBufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_quadIndexBuffer)));

	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&indexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadResource.IndexBufferUpload)));

	NAME_D3D12_OBJECT(m_quadIndexBuffer);

	// Upload the index buffer to the GPU.
	{
		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<BYTE*>(quadIndices);
		indexData.RowPitch = indexBufferSize;
		indexData.SlicePitch = indexData.RowPitch;

		UpdateSubresources(m_commandList.Get(), m_quadIndexBuffer.Get(), uploadResource.IndexBufferUpload.Get(), 0, 0, 1, &indexData);

		CD3DX12_RESOURCE_BARRIER indexBufferResourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(m_quadIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		m_commandList->ResourceBarrier(1, &indexBufferResourceBarrier);
	}

	// Create vertex/index buffer views.
	m_quadVertexBufferView.BufferLocation = m_quadVertexBuffer->GetGPUVirtualAddress();
	m_quadVertexBufferView.StrideInBytes = sizeof(VertexPositionNormalColor);
	m_quadVertexBufferView.SizeInBytes = sizeof(quadVertices);

	m_quadIndexBufferView.BufferLocation = m_quadIndexBuffer->GetGPUVirtualAddress();
	m_quadIndexBufferView.SizeInBytes = sizeof(quadIndices);
	m_quadIndexBufferView.Format = DXGI_FORMAT_R16_UINT;

	return uploadResource;
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	auto d3dDevice = m_deviceResources->GetD3DDevice();

	// Create a root signature with a single constant buffer slot.
	{
		CD3DX12_DESCRIPTOR_RANGE range;
		CD3DX12_ROOT_PARAMETER parameter;

		range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		parameter.InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_VERTEX);

		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // Only the input assembler stage needs access to the constant buffer.
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		CD3DX12_ROOT_SIGNATURE_DESC descRootSignature;
		descRootSignature.Init(1, &parameter, 0, nullptr, rootSignatureFlags);

		ComPtr<ID3DBlob> pSignature;
		ComPtr<ID3DBlob> pError;
		DX::ThrowIfFailed(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(), pError.GetAddressOf()));
		DX::ThrowIfFailed(d3dDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_writeGbufferRootSignature)));
        NAME_D3D12_OBJECT(m_writeGbufferRootSignature);
	}
	{
		CD3DX12_DESCRIPTOR_RANGE range;
		CD3DX12_ROOT_PARAMETER parameter;

		// 3 descriptors; one for each array slice
		range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);
		parameter.InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_PIXEL);

		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // Only the input assembler stage needs access to the constant buffer.
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;

		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		CD3DX12_ROOT_SIGNATURE_DESC descRootSignature;
		descRootSignature.Init(1, &parameter, 1, &sampler, rootSignatureFlags);

		ComPtr<ID3DBlob> pSignature;
		ComPtr<ID3DBlob> pError;
		DX::ThrowIfFailed(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(), pError.GetAddressOf()));
		DX::ThrowIfFailed(d3dDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_writeTargetRootSignature)));
		NAME_D3D12_OBJECT(m_writeTargetRootSignature);
	}

	// Create the pipeline state once the shaders are loaded.
	{
		// Load shaders
		std::vector<byte> vertexShader = DX::ReadData(L"Pass1_VS.cso");
		std::vector<byte> pixelShader = DX::ReadData(L"Pass1_PS.cso");

		static const D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC state = {};
		state.InputLayout = { inputLayout, _countof(inputLayout) };
		state.pRootSignature = m_writeGbufferRootSignature.Get();
        state.VS = CD3DX12_SHADER_BYTECODE(&vertexShader[0], vertexShader.size());
        state.PS = CD3DX12_SHADER_BYTECODE(&pixelShader[0], pixelShader.size());
		state.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		state.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		state.SampleMask = UINT_MAX;
		state.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		state.NumRenderTargets = 3;
		state.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		state.RTVFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		state.RTVFormats[2] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		state.DSVFormat = m_deviceResources->GetDepthBufferFormat();
		state.SampleDesc.Count = 1;

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateGraphicsPipelineState(&state, IID_PPV_ARGS(&m_writeGbufferPipelineState)));
	};

	// Create the pipeline state once the shaders are loaded.
	{
		// Load shaders
		std::vector<byte> vertexShader = DX::ReadData(L"Pass2_VS.cso");
		std::vector<byte> pixelShader = DX::ReadData(L"Pass2_PS.cso");

		static const D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC state = {};
		state.InputLayout = { inputLayout, _countof(inputLayout) };
		state.pRootSignature = m_writeTargetRootSignature.Get();
		state.VS = CD3DX12_SHADER_BYTECODE(&vertexShader[0], vertexShader.size());
		state.PS = CD3DX12_SHADER_BYTECODE(&pixelShader[0], pixelShader.size());
		state.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		state.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		state.DepthStencilState.DepthEnable = FALSE;
		state.SampleMask = UINT_MAX;
		state.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		state.NumRenderTargets = 1;
		state.RTVFormats[0] = m_deviceResources->GetBackBufferFormat();
		state.SampleDesc.Count = 1;

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateGraphicsPipelineState(&state, IID_PPV_ARGS(&m_writeTargetPipelineState)));
	};

	// Create and upload cube geometry resources to the GPU.
	{
		auto d3dDevice = m_deviceResources->GetD3DDevice();

		// Create a command list.
		DX::ThrowIfFailed(d3dDevice->CreateCommandList(
			0, 
			D3D12_COMMAND_LIST_TYPE_DIRECT, 
			m_deviceResources->GetCommandAllocator(), 
			m_writeGbufferPipelineState.Get(), 
			IID_PPV_ARGS(&m_commandList)));
        NAME_D3D12_OBJECT(m_commandList);

		auto cubeUploadResource = CreateCubeGeometryResources();
		auto quadUploadResource = CreateQuadGeometryResources();

		// Create a descriptor heap for the constant buffers.
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
			heapDesc.NumDescriptors = DX::c_frameCount;
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			// This flag indicates that this descriptor heap can be bound to the pipeline and that descriptors contained in it can be referenced by a root table.
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			DX::ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_cbvHeap)));

            NAME_D3D12_OBJECT(m_cbvHeap);
		}
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
			heapDesc.NumDescriptors = 1;
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			// This flag indicates that this descriptor heap can be bound to the pipeline and that descriptors contained in it can be referenced by a root table.
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			DX::ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_gbufferSrvHeap)));

			NAME_D3D12_OBJECT(m_gbufferSrvHeap);
			
			CD3DX12_CPU_DESCRIPTOR_HANDLE srvDescriptor(m_gbufferSrvHeap->GetCPUDescriptorHandleForHeapStart());
			D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc{};
			viewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			viewDesc.Texture2DArray.ArraySize = 3;
			viewDesc.Texture2DArray.MipLevels = 1;
			viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			d3dDevice->CreateShaderResourceView(m_deviceResources->GetGBufferResource(), &viewDesc, srvDescriptor);
		}

		CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(DX::c_frameCount * c_alignedConstantBufferSize);
		DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&constantBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_constantBuffer)));

        NAME_D3D12_OBJECT(m_constantBuffer);

		// Create constant buffer views to access the upload buffer.
		D3D12_GPU_VIRTUAL_ADDRESS cbvGpuAddress = m_constantBuffer->GetGPUVirtualAddress();
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle(m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
		m_cbvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (int n = 0; n < DX::c_frameCount; n++)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
			desc.BufferLocation = cbvGpuAddress;
			desc.SizeInBytes = c_alignedConstantBufferSize;
			d3dDevice->CreateConstantBufferView(&desc, cbvCpuHandle);

			cbvGpuAddress += desc.SizeInBytes;
			cbvCpuHandle.Offset(m_cbvDescriptorSize);
		}

		// Map the constant buffers.
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		DX::ThrowIfFailed(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedConstantBuffer)));
		ZeroMemory(m_mappedConstantBuffer, DX::c_frameCount * c_alignedConstantBufferSize);
		// We don't unmap this until the app closes. Keeping things mapped for the lifetime of the resource is okay.

		// Close the command list and execute it to begin the vertex/index buffer copy into the GPU's default heap.
		DX::ThrowIfFailed(m_commandList->Close());
		ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
		m_deviceResources->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// Wait for the command list to finish executing; the vertex/index buffers need to be uploaded to the GPU before the upload resources go out of scope.
		m_deviceResources->WaitForGpu();
	};

	m_loadingComplete = true;
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	DX::SizeU outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = static_cast<float>(outputSize.Width) / static_cast<float>(outputSize.Height);
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	D3D12_VIEWPORT viewport = m_deviceResources->GetScreenViewport();
	m_scissorRect = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height)};


	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix)
		);

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMVECTORF32 eye = { 0.0f, 0.7f, 1.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	if (m_loadingComplete)
	{
		if (!m_tracking)
		{
			// Rotate the cube a small amount.
			m_angle += static_cast<float>(timer.GetElapsedSeconds()) * m_radiansPerSecond;

			Rotate(m_angle);
		}

		// Update the constant buffer resource.
		UINT8* destination = m_mappedConstantBuffer + (m_deviceResources->GetCurrentFrameIndex() * c_alignedConstantBufferSize);
		memcpy(destination, &m_constantBufferData, sizeof(m_constantBufferData));
	}
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader.
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

// Renders one frame using the vertex and pixel shaders.
bool Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return false;
	}

	DX::ThrowIfFailed(m_deviceResources->GetCommandAllocator()->Reset());

	// The command list can be reset anytime after ExecuteCommandList() is called.
	DX::ThrowIfFailed(m_commandList->Reset(m_deviceResources->GetCommandAllocator(), m_writeGbufferPipelineState.Get()));

	// Pass 1
	{
		// Set the graphics root signature and descriptor heaps to be used by this frame.
		m_commandList->SetGraphicsRootSignature(m_writeGbufferRootSignature.Get());
		ID3D12DescriptorHeap* ppHeaps[] = { m_cbvHeap.Get() };
		m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		// Bind the current frame's constant buffer to the pipeline.
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_cbvHeap->GetGPUDescriptorHandleForHeapStart(), m_deviceResources->GetCurrentFrameIndex(), m_cbvDescriptorSize);
		m_commandList->SetGraphicsRootDescriptorTable(0, gpuHandle);

		// Set the viewport and scissor rectangle.
		D3D12_VIEWPORT viewport = m_deviceResources->GetScreenViewport();
		m_commandList->RSSetViewports(1, &viewport);
		m_commandList->RSSetScissorRects(1, &m_scissorRect);
		
		// Record drawing commands.
		D3D12_CPU_DESCRIPTOR_HANDLE gBufferPositionView = m_deviceResources->GetGBufferPositionView();
		D3D12_CPU_DESCRIPTOR_HANDLE gBufferNormalView = m_deviceResources->GetGBufferNormalView();
		D3D12_CPU_DESCRIPTOR_HANDLE gBufferLightPositionView = m_deviceResources->GetGBufferLightPositionView();
		float black[] = { 0, 0, 0, 0 };
		m_commandList->ClearRenderTargetView(gBufferPositionView, black, 0, nullptr);
		m_commandList->ClearRenderTargetView(gBufferNormalView, black, 0, nullptr);
		m_commandList->ClearRenderTargetView(gBufferLightPositionView, black, 0, nullptr);

		D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = m_deviceResources->GetDepthStencilView();
		m_commandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[3] = { gBufferPositionView, gBufferNormalView, gBufferLightPositionView };
		m_commandList->OMSetRenderTargets(_countof(renderTargets), renderTargets, false, &depthStencilView);

		m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_commandList->IASetVertexBuffers(0, 1, &m_cubeVertexBufferView);
		m_commandList->IASetIndexBuffer(&m_cubeIndexBufferView);
		m_commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);
	}

	// Pass 2
	{
		m_commandList->SetGraphicsRootSignature(m_writeTargetRootSignature.Get());
		ID3D12DescriptorHeap* ppHeaps[] = { m_gbufferSrvHeap.Get() };
		m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_gbufferSrvHeap->GetGPUDescriptorHandleForHeapStart());
		m_commandList->SetGraphicsRootDescriptorTable(0, gpuHandle);

		m_commandList->SetPipelineState(m_writeTargetPipelineState.Get());

		CD3DX12_RESOURCE_BARRIER renderTargetResourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(
				m_deviceResources->GetSwapChainRenderTargetResource(),
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList->ResourceBarrier(1, &renderTargetResourceBarrier);

		CD3DX12_RESOURCE_BARRIER gBufferShaderResourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(
				m_deviceResources->GetGBufferResource(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		m_commandList->ResourceBarrier(1, &gBufferShaderResourceBarrier);

		D3D12_CPU_DESCRIPTOR_HANDLE swapChainRenderTargetView = m_deviceResources->GetSwapChainRenderTargetView();

		D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[1] = { swapChainRenderTargetView };
		m_commandList->OMSetRenderTargets(_countof(renderTargets), renderTargets, false, nullptr);

		m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_commandList->IASetVertexBuffers(0, 1, &m_quadVertexBufferView);
		m_commandList->IASetIndexBuffer(&m_quadIndexBufferView);
		m_commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

		CD3DX12_RESOURCE_BARRIER presentResourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(
				m_deviceResources->GetSwapChainRenderTargetResource(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1, &presentResourceBarrier);

		CD3DX12_RESOURCE_BARRIER gBufferTargetResourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(
				m_deviceResources->GetGBufferResource(),
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
				D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList->ResourceBarrier(1, &gBufferTargetResourceBarrier);
	}

	DX::ThrowIfFailed(m_commandList->Close());

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
	m_deviceResources->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	return true;
}
