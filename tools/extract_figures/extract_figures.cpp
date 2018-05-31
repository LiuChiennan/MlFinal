#include <fstream>
#include <iostream>
#include <string>
int main()
{
	std::string name;
	int choice;
	std::cout << "0: mnist_train_data\nothers: mnist_test_data\n:_______\b\b\b" << std::flush;
	std::cin >> choice;
	if (choice == 0)
		name = "mnist_train_data";
	else
		name = "mnist_test_data";
	std::fstream fin(name, std::ios::in | std::ios::binary);
	if (!fin.is_open())
	{
		std::cout << "confirm the path of file" << std::endl;
		std::system("pause");
		return 0;
	}
	std::fstream fout(name + "_extracted", std::ios::out | std::ios::binary);
	fin.unsetf(std::ios::skipws);
	fout.unsetf(std::ios::skipws);
	unsigned char figure[45 * 45];
	int total;
	if (choice == 0)
		total = 60000;
	else
		total = 10000;
	for (int i = 0; i < total; ++i)
	{
		for (int y = 0; y < 45; ++y)
		{
			for (int x = 0; x < 45; ++x)
			{
				fin >> figure[y * 45 + x];
			}
		}
		long long sum = 0;
		long long w_sum = 0;
		long long h_sum = 0;
		for (int j = 0; j < 45; ++j)
		{
			for (int i = 0; i < 45; ++i)
			{
				sum += figure[j * 45 + i];
				w_sum += figure[j * 45 + i] * i;
				h_sum += figure[j * 45 + i] * j;
			}
		}
		int w_center = w_sum / sum;
		int h_center = h_sum / sum;
		int last_w_center = 0;
		int last_h_center = 0;
		while (w_center != last_w_center || h_center != last_h_center)
		{
			sum = w_sum = h_sum = 0;
			for (int j = h_center - 14; j < h_center + 14; ++j)
			{
				for (int i = w_center - 14; i < w_center + 14; ++i)
				{
					if (i < 0 || i >= 45 || j < 0 || j >= 45)
					{
						sum += 0;
						w_sum += 0;
						h_sum += 0;
					}
					else
					{
						sum += figure[j * 45 + i];
						w_sum += figure[j * 45 + i] * (i - w_center + 14);
						h_sum += figure[j * 45 + i] * (j - h_center + 14);
					}
				}
			}
			last_w_center = w_center;
			last_h_center = h_center;
			int d_w_center = w_sum / sum + (last_w_center - 14);
			int d_h_center = h_sum / sum + (last_h_center - 14);
			w_center = (d_w_center + last_w_center) / 2;
			h_center = (d_h_center + last_h_center) / 2;
		}
		for (int j = h_center - 14; j < h_center + 14; ++j)
		{
			for (int i = w_center - 14; i < w_center + 14; ++i)
			{
				if (i < 0 || i >= 45 || j < 0 || j >= 45)
				{
					fout << (unsigned char)0;
				}
				else
				{
					fout << figure[j * 45 + i];
				}
			}
		}
		if (i % (total / 20) == 0)
		{
			std::cout << " ||";
			for (int j = 0; j < i / (total / 20) + 11; ++j)
			{
				std::cout << '\b';
			}
			std::cout << " || ";
			for (int j = 0; j < i / (total / 20); ++j)
			{
				std::cout << '#';
			}
			std::cout << (i / (total / 20)) * 5;
		}
	}
	for (int j = 0; j < 20 + 3; ++j)
	{
		std::cout << '\b';
	}
	for (int j = 0; j < 20; ++j)
	{
		std::cout << '#';
	}
	std::cout << 100;
	std::cout << std::endl;
	std::cout << "Done! " << "Outputs are in file " << name + "_extracted" << std::endl;
	std::system("pause");
}