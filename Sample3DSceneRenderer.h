#pragma once

#include "DeviceResources.h"
#include "ShaderStructures.h"
#include "StepTimer.h"

namespace SpinningCube
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~Sample3DSceneRenderer();
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void Update(DX::StepTimer const& timer);
		bool Render();

	private:
		void Rotate(float radians);

		struct UploadResource
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUpload;
			Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUpload;
		};
		UploadResource CreateCubeGeometryResources();
		UploadResource CreateQuadGeometryResources();

	private:
		// Constant buffers must be 256-byte aligned.
		static const UINT c_alignedConstantBufferSize = (sizeof(ModelViewProjectionConstantBuffer) + 255) & ~255;

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_commandList;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_writeGbufferRootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_writeGbufferPipelineState;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_cbvHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_constantBuffer;
		ModelViewProjectionConstantBuffer					m_constantBufferData;
		UINT8* m_mappedConstantBuffer;
		UINT												m_cbvDescriptorSize;

		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_writeTargetRootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_writeTargetPipelineState;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_gbufferSrvHeap;

		D3D12_RECT											m_scissorRect;

		Microsoft::WRL::ComPtr<ID3D12Resource>				m_cubeVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_cubeIndexBuffer;
		D3D12_VERTEX_BUFFER_VIEW							m_cubeVertexBufferView;
		D3D12_INDEX_BUFFER_VIEW								m_cubeIndexBufferView;
		std::vector<VertexPositionNormalColor> m_cubeVertices;

		Microsoft::WRL::ComPtr<ID3D12Resource>				m_quadVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_quadIndexBuffer;
		D3D12_VERTEX_BUFFER_VIEW							m_quadVertexBufferView;
		D3D12_INDEX_BUFFER_VIEW								m_quadIndexBufferView;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_radiansPerSecond;
		float	m_angle;
		bool	m_tracking;
	};
}

