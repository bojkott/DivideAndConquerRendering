#include "Technique.h"

int Technique::numberOfTechniques = 0;

Technique::Technique(/*Material * m,*/ RenderState * r)
{
	id = numberOfTechniques;
	numberOfTechniques++;

	//material = m;
	renderState = r;

	//std::vector<vk::VertexInputBindingDescription> vertexBindingDescriptions = m->getBindingDescriptions();
	//std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescriptions = m->getAttributeDescriptions();


	//vertex input
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	//vertexInputInfo.vertexBindingDescriptionCount = vertexBindingDescriptions.size();
	//vertexInputInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
	//vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
	//vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();

	//input assembly
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	vk::GraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.stageCount = 2;
	//pipelineInfo.pStages = m->getShaderStages();	//Or something like tahat
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = r->getViewportState();
	pipelineInfo.pRasterizationState = r->getRasterizer();
	pipelineInfo.pMultisampleState = r->getMultisampling();
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = r->getColorBlending();
	//pipelineInfo.layout = VulkanRenderer::pipelineLayout;
	//pipelineInfo.renderPass = VulkanRenderer::renderPass;	//Or something like tahat
	pipelineInfo.subpass = 0;

	/*
	Create graphics pipelines with deice. Throw if error
	*/

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.setLayoutCount = 1;
	//pipelineLayoutInfo.pSetLayouts = m->getDescriptorSetLayout();	//Or something like tahat
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = 0;

	/*
	Create pipeline layot with device. Throw if error
	 */

	
}

Technique::~Technique()
{
	/*
	Destroy pipeline with device
	Destroy pipeline layout with device
	*/
}
