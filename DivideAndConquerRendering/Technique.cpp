#include "Technique.h"
#include "Renderer.h"

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
	vk::DescriptorSetLayout layouts[] = { descriptorSetLayout };
	vk::DescriptorSetAllocateInfo allocInfo = {};
	//allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	/*if (FAILED(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet))) {
	fprintf(stderr, "failed to allocate descriptor set!\n");
	exit(-1);
	}*/
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

	/*if (FAILED(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout))) {
		fprintf(stderr, "failed to create pipeline layout!\n");
		exit(-1);
	}*/
}
