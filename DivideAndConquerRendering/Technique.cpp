#include "Technique.h"
#include "Renderer.h"
#include "DeviceContext.h"
#include "UniformBuffer.h"
#include "materials\ForwardMaterial.h"

#include "Camera.h"

class ForwardMaterial;

std::map<Material*, vkGroups::TechniqueGroup> Technique::loadedTechniques;
Technique::Technique(DeviceContext* deviceContext, Material * m, RenderState * r)
{
	this->deviceContext = deviceContext;
	material = m;
	renderState = r;

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = m->getVertexinputInfo();

	//input assembly
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	vk::GraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.pVertexInputState = &vertexInputInfo;	//Specific for device? Don't think so. Should be same for the same shaders. So will only differ when we change technique.
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = r->getViewportState();
	pipelineInfo.pRasterizationState = r->getRasterizer();
	pipelineInfo.pMultisampleState = r->getMultisampling();
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.pDepthStencilState = r->getDepthStencil();
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

	if (m->getMaterialBufferSize() > 0)
	{
		createTextureSampler();
		createTexturesFromMaterial();
		materialBuffer = new UniformBuffer(deviceContext, m->getMaterialBufferSize());
		materialBuffer->setData(m->getMaterialBufferData());
	}

	bindMaterial();
}

Technique * Technique::createOrGetTechnique(DeviceContext * deviceContext, Material * m, RenderState* r)
{
	if (loadedTechniques.find(m) != loadedTechniques.end())
	{
		vkGroups::TechniqueGroup& techGroup = loadedTechniques.at(m);
		if (techGroup.sets.find(deviceContext) != techGroup.sets.end())
			return techGroup.sets.at(deviceContext);
		else
		{
			Technique* tech = new Technique(deviceContext, m, r);
			techGroup.sets.insert(std::make_pair(deviceContext, tech));
			return tech;
		}
			
	}
	else
	{
		loadedTechniques[m];
		vkGroups::TechniqueGroup& techGroup = loadedTechniques.at(m);
		Technique* tech = new Technique(deviceContext, m, r);
		techGroup.sets.insert(std::make_pair(deviceContext, tech));
		return tech;
	}
}

Technique::~Technique()
{
	
}

void Technique::bindMaterial()
{
	if (materialBuffer)
	{

		materialBuffer->bind(PER_MATERIAL_BINDING, descriptorSets[0]);
		
		if (textures.size())
			bindTextures();

		Camera::getInstance()->bindCamera(deviceContext, descriptorSets[0]);
	}
		
}

void Technique::bindPipeline(vk::CommandBuffer& commandBuffer)
{
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	
}

void Technique::bindDescriptorSet(vk::CommandBuffer & commandBuffer)
{
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSets, {});
}

std::vector<vk::DescriptorSet> Technique::getDescriptionSets()
{
	return descriptorSets;
}

vk::PipelineLayout & Technique::getPipelineLayout()
{
	return pipelineLayout;
}

void Technique::createTexturesFromMaterial()
{
	ForwardMaterial* fm = (ForwardMaterial*)material;
	if (!fm->getAmbientTexname().empty())
	{
		textures[AMBIENT_TEXTURE] = Texture::loadFromFile(deviceContext, fm->getAmbientTexname());
	}
	if (!fm->getDiffuseTexname().empty())
	{
		textures[DIFFUSE_TEXTURE] = Texture::loadFromFile(deviceContext, fm->getDiffuseTexname());
	}
	if (!fm->getSpecularTexname().empty())
	{
		textures[SPECULAR_TEXTURE] = Texture::loadFromFile(deviceContext, fm->getSpecularTexname());
	}
	if (!fm->getSpecularHighlightTexname().empty())
	{
		textures[SPECULAR_HIGHLIGHT_TEXTURE] = Texture::loadFromFile(deviceContext, fm->getSpecularHighlightTexname());
	}
	if (!fm->getBumpTexname().empty())
	{
		textures[BUMP_TEXTURE] = Texture::loadFromFile(deviceContext, fm->getBumpTexname());
	}
	if (!fm->getDisplacementTexname().empty())
	{
		textures[DISPLACEMENT_TEXTURE] = Texture::loadFromFile(deviceContext, fm->getDisplacementTexname());
	}
	if (!fm->getAlphaTexname().empty())
	{
		textures[ALPHA_TEXTURE] = Texture::loadFromFile(deviceContext, fm->getAlphaTexname());
	}
	if (!fm->getReflectionTexname().empty())
	{
		textures[REFLECTION_TEXTURE] = Texture::loadFromFile(deviceContext, fm->getReflectionTexname());
	}
}

void Technique::bindTextures()
{
	std::vector<vk::WriteDescriptorSet> descWrites;

	vk::DescriptorImageInfo imageInfo;
	imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	imageInfo.sampler = textureSampler;

	vk::WriteDescriptorSet desc;
	desc.dstSet = descriptorSets[0];
	desc.dstArrayElement = 0;
	desc.descriptorType = vk::DescriptorType::eCombinedImageSampler;
	desc.descriptorCount = 1;
	desc.pImageInfo = &imageInfo;

	for (auto& texture : textures)
	{
		desc.dstBinding = texture.first;
		imageInfo.imageView = texture.second->getImageView();
		descWrites.push_back(desc);
	}

	deviceContext->getDevice().updateDescriptorSets(descWrites.size(), descWrites.data(), 0, nullptr);
}

void Technique::createTextureSampler()
{
	vk::SamplerCreateInfo samplerInfo;
	samplerInfo.magFilter = vk::Filter::eLinear;
	samplerInfo.minFilter = vk::Filter::eLinear;
	samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
	samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 1;
	samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = vk::CompareOp::eAlways;
	samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;

	textureSampler = deviceContext->getDevice().createSampler(samplerInfo);
}
