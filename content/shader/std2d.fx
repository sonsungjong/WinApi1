#ifndef _STD2D
#define _STD2D

struct VS_IN
{
	float3 vPos : POSITION0;
};

struct VS_OUT
{
	float4 vPosition : SV_Position;
};

VS_OUT VS_Std2D(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.0f;

	output.vPosition = float4(_in.vPos, 1.0f);

	return output;
}

float4 PS_Std2D(VS_OUT _in) : SV_Target
{
	return float4(1.0f, 0.0f, 1.0f, 1.0f);
}

#endif