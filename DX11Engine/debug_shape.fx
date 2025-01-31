#ifndef DEBUG_SHAPE_FX_
#define DEBUG_SHAPE_FX_

#include "value.fx"

struct VS_IN
{

};

struct VS_OUT
{

};

VS_OUT VS_DebugShape(VS_IN _in)
{
	VS_OUT output = (VS_OUT)0.f;


	return output;
}

float4 PS_DebugShape(VS_OUT _in) : SV_Target
{
	float vOutColor = (float4)0.f;

return vOutColor;
}


#endif