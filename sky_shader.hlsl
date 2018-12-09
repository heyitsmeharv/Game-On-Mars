TextureCube texture0;
SamplerState sampler0;

cbuffer CBuffer0 {
	matrix WVPMatrix;	// 64 bytes
}; 

struct VOut
{
	float4 position : SV_POSITION;
	float3 texcoord : TEXCOORD;
};

VOut ModelVS(float4 position : POSITION, float3 texcoord : TEXCOORD)
{
	VOut output;

	output.texcoord = position.xyz;
	output.position = mul(WVPMatrix, position);

	return output;
}

float4 ModelPS(float4 position : SV_POSITION, float3 texcoord : TEXCOORD) : SV_TARGET
{
	return texture0.Sample(sampler0, texcoord);
}