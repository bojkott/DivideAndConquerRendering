#include "Technique.h"
#include "Renderer.h"
#include "DeviceContext.h"


Technique::Technique(DeviceContext* deviceContext, Material * m, RenderState * r)
{

	material = m;
	renderState = r;


	//input assembly
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	vk::GraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.pVertexInputState = &m->getVertexinputInfo();	//Specific for device? Don't think so. Should be same for the same shaders. So will only differ when we change technique.
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = r->getViewportState();
	pipelineInfo.pRasterizationState = r->getRasterizer();
	pipelineInfo.pMultisampleState = r->getMultisampling();
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = r->getColorBlending();
	pipelineInfo.subpass = 0;
	pipelineInfo.renderPass = deviceContext->getRenderpass(m->getRenderPassType());

	std::vector<vk::PipelineShaderStageCreateInfo> stages;
	m->getShaderStages(deviceContext, stages);

	pipelineInfo.stageCount = stages.size();
	pipelineInfo.pStages = stages.data();

	descriptorPool = deviceContext->getDevice().createDescriptorPool(material->getDescriptorPoolInfo());
	descriptorSetLayout = deviceContext->getDevice().createDescriptorSetLayout(material->getDescriptorSetLayoutInfo());

	vk::DescriptorSetAllocateInfo allocInfo;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;
	descriptorSets = deviceContext->getDevice().allocateDescriptorSets(allocInfo);


	vk::DescriptorSetLayout layouts[] = { descriptorSetLayout };
	vk::PipelineLayoutCreateInfo createInfo = material->getPipelineLayoutInfo();
	createInfo.setLayoutCount = 1;
	createInfo.pSetLayouts = layouts;
	pipelineLayout = deviceContext->getDevice().createPipelineLayout(createInfo);

	
	pipelineInfo.layout = pipelineLayout;
	

	pipeline = deviceContext->getDevice().createGraphicsPipeline({}, pipelineInfo);

}

Technique::~Technique()
{
	
}

void Technique::bind(vk::CommandBuffer& commandBuffer)
{
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSets, {});
}
