#pragma once

namespace SpinningCube
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionNormalColor
	{
		VertexPositionNormalColor(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 n, DirectX::XMFLOAT3 c)
			: position(p), normal(n), color(c) {}
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 color;
	};
}