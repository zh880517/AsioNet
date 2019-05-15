#include "AsioService.h"
#include <vector>
#include <iostream>
#include "ClientTest.h"
void main()
{
	AsioNet::AsioService seivice;
	std::vector<ClientTest*> vec;
	for (int i=0; i<100; ++i)
	{
		vec.push_back(new ClientTest(seivice.CreateKCPClent()));
	}
	while (true)
	{
		seivice.Update();
		seivice.Sleep(500);
	}
}