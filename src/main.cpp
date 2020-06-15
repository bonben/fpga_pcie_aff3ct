#include <iostream>
#include <aff3ct.hpp>

using namespace aff3ct;

int main(int argc, char** argv)
{
	auto source = std::unique_ptr<factory::Source>(new factory::Source());

    std::cout << "hello" << std::endl;
}