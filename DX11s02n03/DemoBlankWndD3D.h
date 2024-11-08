#ifndef DEMOBLANKWNDD3D_H_
#define DEMOBLANKWNDD3D_H_

#include "Dx11Base.h"

class CDemoBlankWndD3D : public CDx11Base
{
// Constructors
public:
	CDemoBlankWndD3D();
	virtual ~CDemoBlankWndD3D();

// Overrides
public:
	virtual bool LoadContent();
	virtual void UnloadContent();

	virtual void Update();
	virtual void Render();
};

#endif			DEMOBLANKWNDD3D_H_