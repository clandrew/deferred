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
	// vector not a position, so it isn't transformed to the view
	// It is transformed in model space though, because as the model turns around the normal does too
	norm = mul(norm, model);
	output.norm = norm;

	float4 lightPosition = float4(2000, 2000, -2000, 0.0f);
	// Light position is fixed in world space, and does not move around with the model transform.
	lightPosition = mul(lightPosition, view);
	lightPosition = mul(lightPosition, projection);
	output.lightPosition = lightPosition;

	// Pass the color through without modification.
	output.color = input.color;

	return output;
}
