#include "DaQCombineMaterial.h"
#include "../Renderer.h"
DaQCombineMaterial::DaQCombineMaterial() : Material("DaQCombine.vert", "DaQCombine.frag", DeviceContext::RENDERPASS_TYPE::Final)
{
	for (int i = 0; i < Renderer::deviceGroup.getGroupSize()+1; i++)
	{
		addBinding(i, vk::DescriptorType::eInputAttachment, vk::ShaderStageFlagBits::eFragment);
	}




	//vk::VertexInputBindingDescription bindingDescription;
	//bindingDescription.binding = 0;
	//bindingDescription.stride = sizeof(float) * 4;
	//bindingDescription.inputRate = vk::VertexInputRate::eVertex;
	//bindingDescriptions.push_back(bindingDescription);

	//bindingDescription.binding = 1;
	//bindingDescription.stride = sizeof(float) * 2;
	//bindingDescriptions.push_back(bindingDescription);

	//bindingDescription.binding = 2;
	//bindingDescription.stride = sizeof(float) * 4;
	//bindingDescriptions.push_back(bindingDescription);



	//VkVertexInputAttributeDescription attributeDescription = {};
	//attributeDescription.binding = 0;
	//attributeDescription.location = 0;
	//attributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	//attributeDescription.offset = 0;
	//attributeDescriptions.push_back(attributeDescription);

	//attributeDescription.binding = 1;
	//attributeDescription.location = 1;
	//attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
	//attributeDescription.offset = 0;
	//attributeDescriptions.push_back(attributeDescription);

	//attributeDescription.binding = 2;
	//attributeDescription.location = 2;
	//attributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	//attributeDescription.offset = 0;
	//attributeDescriptions.push_back(attributeDescription);
}