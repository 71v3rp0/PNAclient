#include "Gumming.h"
#include <random>

const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz 0123456789";
static const int SP = 3;

int getIndex(char symbol)
{
	for (size_t i = 0; i < strlen(alphabet); i++)
	{
		if (*(alphabet + i) == symbol)
			return i;
	}

	return 0;
}

void createGamma(int gamma[], int messageSize)
{
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(0, strlen(alphabet));

	for (int i = SP; i < messageSize; i++)
	{
		gamma[i] = uid(dre);
	}
}

void encryption(char data[], int gamma[], int messageSize)
{
	int alphabetSize = strlen(alphabet) - 1;
	int index, _gamma;

	for (int i = SP; i < messageSize; i++)
	{
		index = getIndex(data[i]);
		_gamma = (gamma[i] + index > alphabetSize ? (gamma[i] + index) % alphabetSize : gamma[i] + index);
		data[i] = alphabet[_gamma];
	}
}

void decryption(char data[], int gamma[], int messageSize)
{
	int alphabetSize = strlen(alphabet) - 1;
	int index, SP;

	for (int i = ::SP; i < messageSize - 1; i++)
	{
		index = getIndex(data[i]);
		index = (index + alphabetSize - gamma[i] > alphabetSize ? (index + alphabetSize - gamma[i]) % alphabetSize : index + alphabetSize - gamma[i]);
		data[i] = alphabet[index];
	}

	for (SP = 3; SP < messageSize - 1 - 3; SP += 3)
	{
		data[SP - 3] = data[SP];
		data[SP - 2] = data[SP + 1];
		data[SP - 1] = data[SP + 2];
	}

	for (; SP < messageSize - 1; SP++)
	{
		data[SP - 3] = data[SP];
	}

	data[SP - 3] = '\0';
}
