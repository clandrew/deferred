// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 norm : NORMAL0;
	float3 color : COLOR0;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL0;
	float3 color : COLOR0;
	float4 lightPosition : POSITION1;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	float4 norm = float4(input.norm, 1.0f);
	norm = mul(norm, model);
	norm = mul(norm, view);
	norm = mul(norm, projection);
	output.norm = norm;

	// eye = { 0.0f, 0.7f, 1.5f, 0.0f };
	float4 lightPosition = float4(0.0f, 10.7f, 1.5f, 0.0f);
	lightPosition = mul(lightPosition, model);
	lightPosition = mul(lightPosition, view);
	lightPosition = mul(lightPosition, projection);
	output.lightPosition = lightPosition;

	// Pass the color through without modification.
	output.color = input.color;

	return output;
}
