#include "Technique.h"
#include "Renderer.h"
#include "DeviceContext.h"
int Technique::numberOfTechniques = 0;

Technique::Technique(Material * m, RenderState * r)
{
	id = numberOfTechniques;
	numberOfTechniques++;

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

	pipelineGroup = Renderer::deviceGroup.createPipeline(pipelineInfo, pipelineCacheGroup, m->getVertexShader(), m->getFragmentShader(), pipelineLayoutGroup);
	
	descriptorPoolGroup = Renderer::deviceGroup.createDescriptorPool(material->getDescriptorPoolInfo());
	descriptorSetLayoutGroup = Renderer::deviceGroup.createDescriptorSetLayout(material->getDescriptorSetLayoutInfo());
	descriptorSetGroup = Renderer::deviceGroup.allocateDescriptorSet(descriptorPoolGroup, descriptorSetLayoutGroup);
	pipelineLayoutGroup = Renderer::deviceGroup.createPipelineLayout(material->getPipelineLayoutInfo(), descriptorSetLayoutGroup);
	pipelineCacheGroup = Renderer::deviceGroup.createPipelineCache();

}

Technique::~Technique()
{
	
}
