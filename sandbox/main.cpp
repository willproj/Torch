#include <engine/Engine.h>

int main()
{
	std::cout << "hello world" << std::endl;
	engine::Engine::GetEngine()();
	return 0;
}