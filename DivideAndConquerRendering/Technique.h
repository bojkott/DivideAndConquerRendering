#pragma once

#include <vulkan\vulkan.hpp>

class Technique
{

private:
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline pipeline;

public:
	Technique(/*Material* m, RenderState* r*/);
	~Technique();

private:


};
