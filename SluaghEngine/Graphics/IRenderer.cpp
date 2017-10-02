#include <IRenderer.h>
#include "Renderer.h"

DECLDIR_R SE::Graphics::IRenderer * SE::Graphics::CreateRenderer()
{
	return new Renderer;
}
