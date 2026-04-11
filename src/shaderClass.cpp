#include"shaderClass.h"

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

// CONSTRUCTOR
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	int success;       // Status of shader compilation
	char infoLog[512]; //

	// Read vertexFile/fragmentFile
	std::string vertexCode   = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);
	// Convert to c-strings
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	//----------------------------------------------------------------------
	// VERTEX SHADER

	// Create shader Object
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Attach shader-source to shader-object
	glShaderSource(vertexShader, 1, &vertexSource, NULL);

	// Compile shader
	glCompileShader(vertexShader);

	// Check status
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	// Compilation fail
	if(!success) {
		glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
		std::cerr << "Vertex shader failed to compile.\n";
		std::cout << infoLog << std::endl;
	}

	//----------------------------------------------------------------------
	// FRAGMENT SHADER

	// Create shader object
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Attach shader-source to shader-object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

	// Compile fragment shader
	glCompileShader(fragmentShader);

	// Check status
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	// Compilation fail
	if(!success) {
		glGetShaderInfoLog(fragmentShader,512,NULL,infoLog);
		std::cerr << "Fragment shader failed to compile.\n";
		std::cout << infoLog << std::endl;
	}

	//----------------------------------------------------------------------
	// SHADER PROGRAM

	// Create shader-program object
	ID = glCreateProgram();

	// Attach the vert/frag shaders to the shader-program
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);

	// Link shaders
	glLinkProgram(ID);

	// Delete 'useless' shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

// Activate shader-program
void Shader::Activate()
{
	glUseProgram(ID);
}

// Deletes the Shader Program
void Shader::Delete()
{
	glDeleteProgram(ID);
}