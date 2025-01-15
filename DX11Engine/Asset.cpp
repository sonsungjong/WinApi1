#include "pch.h"
#include "Asset.h"

CAsset::CAsset(ASSET_TYPE _type)
	: m_type(_type)
	, m_nRefCount(0)
{
}

CAsset::~CAsset()
{
}
