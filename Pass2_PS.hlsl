// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL0;
	float3 uv : COLOR0;
};

Texture2DArray<float4> g_gBuffer : register(t0);
SamplerState g_samplerState : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 position = g_gBuffer.Sample(g_samplerState, float3(input.uv.xy, 0));

	float4 normal = g_gBuffer.Sample(g_samplerState, float3(input.uv.xy, 1));

	float4 lightPosition = g_gBuffer.Sample(g_samplerState, float3(input.uv.xy, 2));

	float coverageCheat = length(normal);

	float4 ambientColor = float4(0.1f, 0.1f, 0.1f, 1);
	float4 ambientLight = coverageCheat * ambientColor;	
	
	float4 lightDir = lightPosition - position;
	lightDir = normalize(lightDir);

	normal = normalize(normal);
	float diffuseLighting = saturate(dot(lightDir, normal));
	diffuseLighting *= ((length(lightDir) * length(lightDir)) / dot(lightDir, lightDir));

	float4 diffuseColor = float4(1, 0, 0, 1);

	float4 result = diffuseLighting * diffuseColor + ambientLight;
	return result;
}
