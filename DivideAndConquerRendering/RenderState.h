#pragma once

#include <vulkan\vulkan.hpp>

class RenderState
{
private:
	vk::Viewport viewport;
	vk::Rect2D scissor;
	vk::PipelineViewportStateCreateInfo viewportState;
	vk::PipelineRasterizationStateCreateInfo rasterizer;
	vk::PipelineMultisampleStateCreateInfo multisampling;
	vk::PipelineColorBlendAttachmentState colorBlendAttachment;
	vk::PipelineColorBlendStateCreateInfo colorBlending;
	vk::PipelineDepthStencilStateCreateInfo depthStencil;
public:
	RenderState();

	void setWireFrame(bool);
	

	vk::PipelineViewportStateCreateInfo* getViewportState();
	vk::PipelineRasterizationStateCreateInfo* getRasterizer();
	vk::PipelineMultisampleStateCreateInfo* getMultisampling();
	vk::PipelineColorBlendStateCreateInfo* getColorBlending();
	vk::PipelineDepthStencilStateCreateInfo* getDepthStencil();
private:


};