// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 normal : NORMAL0;
	float3 color : COLOR0;
	float4 lightPosition : POSITION1;
};

// A pass-through function for the (interpolated) color data.

struct PixelShaderOutput
{
	float4 position : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 lightPosition : SV_TARGET2;
};

PixelShaderOutput main(PixelShaderInput input)
{
	PixelShaderOutput output;
	output.position = input.position;
	output.normal = input.normal;
	output.lightPosition = input.lightPosition;
	return output;
}
