#include "FileUtils.h"
#include "ServiceLocator.h"

namespace utils
{
	std::vector<char> FileUtils::ReadFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			throw std::runtime_error("Failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}


	std::string FileUtils::ReadShaderFile(const std::string& filePath)
	{
		std::ifstream shaderFile;
		std::stringstream shaderStream;

		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			shaderFile.open(filePath.c_str());
			shaderStream << shaderFile.rdbuf();
			shaderFile.close();
		}
		catch (std::ifstream::failure& e)
		{
			TORCH_LOG_ERROR("[{}:{}] ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: {}", __FILE__, __LINE__, e.what());
		}
		return shaderStream.str();
	}

	std::string FileUtils::GetRootPath()
	{
		return std::string(PROJECT_ROOT);
	}

	std::string FileUtils::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)ServiceLocator::GetWindow()->GetWinSpecification().glfwWindow);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}
	std::string FileUtils::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)ServiceLocator::GetWindow()->GetWinSpecification().glfwWindow);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		// Sets the default extension by extracting it from the filter
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}

	std::string FileUtils::OpenFolder()
	{
		// Initialize COM
		HRESULT hr = CoInitialize(NULL);
		if (FAILED(hr))
		{
			printf("Failed to initialize COM.\n");
			return std::string();
		}

		// Create a new instance of the File Open dialog box
		IFileDialog* pfd;
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		if (FAILED(hr))
		{
			printf("Failed to create instance of File Open dialog box.\n");
			CoUninitialize();
			return std::string();
		}

		// Set the options for the dialog box
		DWORD dwOptions;
		hr = pfd->GetOptions(&dwOptions);
		if (FAILED(hr))
		{
			printf("Failed to get options for File Open dialog box.\n");
			pfd->Release();
			CoUninitialize();
			return std::string();
		}
		hr = pfd->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
		if (FAILED(hr))
		{
			printf("Failed to set options for File Open dialog box.\n");
			pfd->Release();
			CoUninitialize();
			return std::string();
		}

		// Show the dialog box
		hr = pfd->Show(NULL);
		if (FAILED(hr))
		{
			printf("Failed to show File Open dialog box.\n");
			pfd->Release();
			CoUninitialize();
			return std::string();
		}

		// Get the selected folder path
		IShellItem* psi;
		hr = pfd->GetResult(&psi);
		if (FAILED(hr))
		{
			printf("Failed to get selected folder from File Open dialog box.\n");
			pfd->Release();
			CoUninitialize();
			return std::string();
		}

		PWSTR pszFolderPath;
		hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
		if (FAILED(hr))
		{
			printf("Failed to get selected folder path.\n");
			psi->Release();
			pfd->Release();
			CoUninitialize();
			return std::string();
		}

		std::wstring wstrFolderPath(pszFolderPath);
		CoTaskMemFree(pszFolderPath);

		// Release COM objects
		psi->Release();
		pfd->Release();
		CoUninitialize();

		return *(std::string*)&wstrFolderPath;
	}

	std::string FileUtils::GetFilenameInFilepath(const std::string& path)
	{
		return path.substr(path.find_last_of("/\\") + 1);
	}
}
