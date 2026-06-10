#include"shaderClass.h"

#include <chrono>
#include <map>
#include <memory>
//----------------------------------------------------------------------------------------------------------------------
struct ShaderComponents {
	std::string vertexCode;
	std::string geometryCode;
	std::string fragmentCode;
};
//----------------------------------------------------------------------------------------------------------------------
enum ShaderType {
	Vertex,
	Geometry,
	Fragment
};
//----------------------------------------------------------------------------------------------------------------------
// GET_FILE_CONTENTS
// Reads a text file and outputs a string with everything in the text file
std::string get_file_contents(const char* filename)
{
	std::string path = "shaders/" + std::string(filename);

	std::ifstream in(path, std::ios::binary);

	if (in)
	{
		std::string contents;

		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());

		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());

		// Output File Contents
		// std::cout << contents;

		in.close();
		return contents;
	}
	std::cerr << "Failed to open file: " << path << std::endl;
	throw errno;
}
//----------------------------------------------------------------------------------------------------------------------
GLuint compileShaderCode(const char* code, ShaderType type) {
	int success;       // Compilation status
	char infoLog[512]; // Compilation log
	GLuint shader;

	switch (type) {
		case ShaderType::Vertex:
			shader = glCreateShader(GL_VERTEX_SHADER);     // Create shader Object
			glShaderSource(shader, 1, &code, NULL);// Attach shader-source to shader-object
			glCompileShader(shader);									 // Compile shader
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // Check status
			if(!success) {
				glGetShaderInfoLog(shader,512,NULL,infoLog);
				std::cerr << "Vertex shader failed to compile.\n";
				std::cout << infoLog << std::endl;
			}
			break;
		case ShaderType::Geometry:
			shader = glCreateShader(GL_GEOMETRY_SHADER);	 // Create shader object
			glShaderSource(shader, 1, &code, NULL);// Attach shader-source to shader-object
			glCompileShader(shader);									 // Compile geometry shader
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);	 // Check status
			if(!success) {
				glGetShaderInfoLog(shader,512,NULL,infoLog);
				std::cerr << "Geometry shader failed to compile.\n";
				std::cout << infoLog << std::endl;
			}
			break;
		case ShaderType::Fragment:
			shader = glCreateShader(GL_FRAGMENT_SHADER);	 // Create shader object
			glShaderSource(shader, 1, &code, NULL);// Attach shader-source to shader-object
			glCompileShader(shader);									 // Compile fragment shader
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);	 // Check status
			if(!success) {
				glGetShaderInfoLog(shader,512,NULL,infoLog);
				std::cerr << "Fragment shader failed to compile.\n";
				std::cout << infoLog << std::endl;
			}
			break;
		}
	return shader;
	}
//----------------------------------------------------------------------------------------------------------------------
// PARSE TYPE BLOCKS
ShaderComponents parseTypeBlocks(std::string &code) {

	ShaderComponents blocks = {"", "", ""};
	std::vector<std::string> markers = {	"#type vertex", "#type geometry", "#type fragment"};
	std::map<size_t, std::string> foundMarkers;

	// 1. Locate all type markers
	for (const auto& marker : markers) {
		size_t pos = code.find(marker);
		if (pos != std::string::npos) {
			foundMarkers[pos] = marker;
		}
	}

	// 2. Slice string by marker location
	for (auto it = foundMarkers.begin(); it != foundMarkers.end(); ++it) {
		size_t currentPos = it->first;
		std::string currentType = it->second;
		size_t codeStart = currentPos + currentType.length();
		auto nextIt = std::next(it);
		size_t codeEnd = (nextIt != foundMarkers.end()) ? nextIt->first : code.length();



		std::string codeBlock = code.substr(codeStart, codeEnd - codeStart);
		if      (currentType == "#type vertex") blocks.vertexCode     += codeBlock;
		else if (currentType == "#type geometry") blocks.geometryCode += codeBlock;
		else if (currentType == "#type fragment") blocks.fragmentCode += codeBlock;
		std::cout << "--- Found code for: " << currentType << std::endl;
	}
	return blocks;
}


//----------------------------------------------------------------------------------------------------------------------
// CONSTRUCTOR OVERLOAD
Shader::Shader(const char* glslFile)
{
	// --- Parse file ---
	std::string glslCode;
	glslCode = get_file_contents(glslFile);
	// glslCode = parseIncludes(glslCode);
	ShaderComponents parsedCode = parseTypeBlocks(glslCode);

	// --- Compile Shaders ---
	const GLuint vertexShader   = compileShaderCode(parsedCode.vertexCode.c_str(),   ShaderType::Vertex);
	const GLuint geometryShader = compileShaderCode(parsedCode.geometryCode.c_str(), ShaderType::Geometry);
	const GLuint fragmentShader = compileShaderCode(parsedCode.fragmentCode.c_str(), ShaderType::Fragment);

	// --- Link/Build Shader Program ---
	ID = glCreateProgram();				        // Create shader-program object
	glAttachShader(ID, vertexShader);   // Attach vertex shader component
	if (!parsedCode.geometryCode.empty())
		glAttachShader(ID, geometryShader); // Attach geometry shader component
	glAttachShader(ID, fragmentShader); // Attach fragment shader component
	glLinkProgram(ID);						    // Link shader program

	// --- Clean up ---
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}
//----------------------------------------------------------------------------------------------------------------------
// CONSTRUCTOR
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode   = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	// --- Compile Shaders ---
	const GLuint vertexShader   = compileShaderCode(vertexCode.c_str(),   ShaderType::Vertex);
	const GLuint fragmentShader = compileShaderCode(fragmentCode.c_str(), ShaderType::Fragment);

	// --- Link/Build Shader Program ---
	ID = glCreateProgram();				       // Create shader-program object
	glAttachShader(ID, vertexShader);  // Attach shader component to program
	glAttachShader(ID, fragmentShader);//
	glLinkProgram(ID);						   // Link shader program

	// --- Clean up ---
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}
//----------------------------------------------------------------------------------------------------------------------
void Shader::Activate()
{
	glUseProgram(ID);
}
//----------------------------------------------------------------------------------------------------------------------
void Shader::Delete()
{
	glDeleteProgram(ID);
}