#include "Technique.h"
#include "Renderer.h"

int Technique::numberOfTechniques = 0;

Technique::Technique(Material * m, RenderState * r)
{
	id = numberOfTechniques;
	numberOfTechniques++;

	material = m;
	renderState = r;


	std::vector<vk::VertexInputBindingDescription> vertexBindingDescriptions = m->getBindingDescriptions();
	std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescriptions = m->getAttributeDescriptions();


	//vertex input
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.vertexBindingDescriptionCount = vertexBindingDescriptions.size();
	vertexInputInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
	vertexInputInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
	vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();

	//input assembly
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	vk::GraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = r->getViewportState();
	pipelineInfo.pRasterizationState = r->getRasterizer();
	pipelineInfo.pMultisampleState = r->getMultisampling();
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = r->getColorBlending();
	pipelineInfo.subpass = 0;

	pipelineGroup = Renderer::deviceGroup.createPipeline(pipelineInfo, pipelineCacheGroup, m->getVertexShader(), m->getFragmentShader(), pipelineLayoutGroup);
	
}

Technique::~Technique()
{
}

void Technique::createDescriptorPool()
{
	std::vector<vk::DescriptorPoolSize> poolSizes;
	vk::DescriptorPoolSize poolSize = {};
	poolSize.type = vk::DescriptorType::eUniformBuffer;
	poolSize.descriptorCount = 2;
	poolSizes.push_back(poolSize);
	poolSize.type = vk::DescriptorType::eCombinedImageSampler;
	poolSize.descriptorCount = 1;
	poolSizes.push_back(poolSize);

	vk::DescriptorPoolCreateInfo poolInfo = {};
	poolInfo.poolSizeCount = poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();

	poolInfo.maxSets = 1;

	
	descriptorPoolGroup = Renderer::deviceGroup.createDescriptorPool(poolInfo);
}

void Technique::createDescriptorSet()
{
	//If this fail, check the function for more info.
	descriptorSetGroup = Renderer::deviceGroup.allocateDescriptorSet(descriptorPoolGroup, descriptorSetLayout);
}

void Technique::createPipelineLayout()
{
	std::vector<vk::PushConstantRange> pushConstants;

	vk::PushConstantRange pushConstantRange = {};
	pushConstantRange.size = sizeof(float) * 4; //float4
	pushConstantRange.offset = 0;
	pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
	pushConstants.push_back(pushConstantRange);
	pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eFragment;
	pushConstantRange.offset = sizeof(float) * 4;
	pushConstants.push_back(pushConstantRange);

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.setLayoutCount = 1; // Optional
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = pushConstants.size(); // Optional
	pipelineLayoutInfo.pPushConstantRanges = pushConstants.data(); // Optional

	pipelineLayoutGroup = Renderer::deviceGroup.createPipelineLayout(pipelineLayoutInfo);
}

void Technique::createPipelineCache()
{
	pipelineCacheGroup = Renderer::deviceGroup.createPipelineCache();
}
