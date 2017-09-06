#ifndef SE_GRAPHICS_RENDERER_H_
#define SE_GRAPHICS_RENDERER_H_


class Renderer
{
public:
	virtual void CreateRenderObject(/*Entity, renderObjectInfo)*/) = 0;

private:
	Renderer();
	Renderer(const Renderer& other) = delete;
	Renderer(const Renderer&& other) = delete;
	Renderer& operator=(const Renderer& other) = delete;
};


#endif //SE_GRAPHICS_RENDERER_H_