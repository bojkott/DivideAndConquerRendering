#pragma once
#include <fstream>
#include <vulkan/vulkan.hpp>
class DeviceContext;
class Shader
{
public:
	enum class ShaderType { VS = VK_SHADER_STAGE_VERTEX_BIT, PS = VK_SHADER_STAGE_FRAGMENT_BIT, GS = VK_SHADER_STAGE_GEOMETRY_BIT, CS = VK_SHADER_STAGE_COMPUTE_BIT };
private:
	vk::ShaderModule shaderModule;
	DeviceContext* deviceContext;
	ShaderType shaderType;
public:
	

	Shader(const std::string& shaderFilename, ShaderType shaderType, DeviceContext* deviceContext);
	~Shader();
	vk::PipelineShaderStageCreateInfo getShaderStageInfo();

private:	
	std::vector<char> readFile(const std::string& filename);
	void createShaderModule(const std::string & filename);

};