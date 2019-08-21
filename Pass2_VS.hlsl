
// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float4 norm : NORMAL0;
	float3 uv : COLOR0;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL0;
	float3 uv : COLOR0;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);
	output.pos = pos;

	output.norm = input.norm;

	output.uv = input.uv;

	return output;
}
