#include "RenderState.h"
#include "Window.h"
RenderState::RenderState()
{
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = Window::getWidth();	//Get swap chain extent width from renderer
	viewport.height = Window::getHeight();	//Get swap chain extent height from renderer
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;


	scissor.offset = { 0,0 };
	scissor.extent = {Window::getWidth(), Window::getHeight()}; //Get swap chain from renderer


	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;


	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = vk::PolygonMode::eFill;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = vk::CullModeFlagBits::eBack;
	rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;


	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;


	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
										  vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	colorBlendAttachment.blendEnable = VK_FALSE;
	//Optional
	colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
	colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
	colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
	colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
	colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
	colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;


	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = vk::LogicOp::eCopy;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	//Optional
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;


	//depth stencil
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = vk::CompareOp::eLess;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

}

void RenderState::setWireFrame(bool wireFrame)
{
	rasterizer.polygonMode = wireFrame ? vk::PolygonMode::eLine : vk::PolygonMode::eFill;
}

vk::PipelineViewportStateCreateInfo * RenderState::getViewportState()
{
	return &viewportState;
}

vk::PipelineRasterizationStateCreateInfo * RenderState::getRasterizer()
{
	return &rasterizer;
}

vk::PipelineMultisampleStateCreateInfo * RenderState::getMultisampling()
{
	return &multisampling;
}

vk::PipelineColorBlendStateCreateInfo * RenderState::getColorBlending()
{
	return &colorBlending;
}

vk::PipelineDepthStencilStateCreateInfo * RenderState::getDepthStencil()
{
	return &depthStencil;
}
