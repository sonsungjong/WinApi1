#ifndef DEBUG_SHAPE_FX_
#define DEBUG_SHAPE_FX_

#include "value.fx"

struct VS_IN
{
	float3 vLocalPos : POSITION;
	float2 _vUV : TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
	float2 vUV : TEXCOORD;
};

VS_OUT VS_DebugShape(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;

	output.vPosition = mul(mul(mul(float4(_in.vLocalPos, 1.f), g_matWorld), g_matView), g_matProj);
	output.vUV = _in._vUV;

	return output;
}

float4 PS_DebugShape(VS_OUT _in) : SV_Target
{
	return g_vec4_0;
}


#endif