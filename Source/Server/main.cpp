#include "AsioService.h"
#include "ServerTest.h"

void main()
{
	AsioNet::AsioService seivice;
	ServerTest test(seivice.CreateKCPServer("127.0.0.1", 4567));

	while (true)
	{
		seivice.Update();
		seivice.Sleep(5);
	}
}